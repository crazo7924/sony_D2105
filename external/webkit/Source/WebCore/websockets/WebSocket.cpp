/*
 * Copyright (C) 2009 Google Inc.  All rights reserved.
 * Copyright (C) 2014 Sony Mobile Communications AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(WEB_SOCKETS)

#include "WebSocket.h"

//SOMC KK PATCH Patches_w15.5_begin
#include "Blob.h"
#include "BlobData.h"
#include "CloseEvent.h"
#include "ContentSecurityPolicy.h"
//SOMC KK PATCH Patches_w15.5_end
#include "DOMWindow.h"
#include "Event.h"
#include "EventException.h"
#include "EventListener.h"
#include "EventNames.h"
//SOMC KK PATCH Patches_w15.5_begin
#include "ExceptionCode.h"
//SOMC KK PATCH Patches_w15.5_end
#include "Logging.h"
#include "MessageEvent.h"
#include "ScriptCallStack.h"
#include "ScriptExecutionContext.h"
//SOMC KK PATCH Patches_w15.5_begin
#include "SecurityOrigin.h"
//SOMC KK PATCH Patches_w15.5_end
#include "ThreadableWebSocketChannel.h"
#include "WebSocketChannel.h"
//SOMC KK PATCH Patches_w15.5_begin
#include <wtf/HashSet.h>
#include <wtf/OwnPtr.h>
#include <wtf/PassOwnPtr.h>
//SOMC KK PATCH Patches_w15.5_end
#include <wtf/StdLibExtras.h>
#include <wtf/text/CString.h>
#include <wtf/text/StringBuilder.h>
//SOMC KK PATCH Patches_w15.5_begin
//#include <wtf/text/StringConcatenate.h>
#include <wtf/text/WTFString.h>

using namespace std;

//SOMC KK PATCH Patches_w15.5_end
namespace WebCore {

//SOMC KK PATCH Patches_w15.5_begin
const size_t maxReasonSizeInBytes = 123;

static inline bool isValidProtocolCharacter(UChar character)
{
    // Hybi-10 says "(Subprotocol string must consist of) characters in the range U+0021 to U+007E not including
    // separator characters as defined in [RFC2616]."
    const UChar minimumProtocolCharacter = '!'; // U+0021.
    const UChar maximumProtocolCharacter = '~'; // U+007E.
    return character >= minimumProtocolCharacter && character <= maximumProtocolCharacter
        && character != '"' && character != '(' && character != ')' && character != ',' && character != '/'
        && !(character >= ':' && character <= '@') // U+003A - U+0040 (':', ';', '<', '=', '>', '?', '@').
        && !(character >= '[' && character <= ']') // U+005B - U+005D ('[', '\\', ']').
        && character != '{' && character != '}';
}
//SOMC KK PATCH Patches_w15.5_end

static bool isValidProtocolString(const String& protocol)
//SOMC KK PATCH Patches_w15.5_begin
{
    if (protocol.isEmpty())
        return false;
    for (size_t i = 0; i < protocol.length(); ++i) {
        if (!isValidProtocolCharacter(protocol[i]))
            return false;
    }
    return true;
}

static bool isValidProtocolStringHixie76(const String& protocol)
//SOMC KK PATCH Patches_w15.5_end
{
    if (protocol.isNull())
        return true;
    if (protocol.isEmpty())
        return false;
    const UChar* characters = protocol.characters();
    for (size_t i = 0; i < protocol.length(); i++) {
        if (characters[i] < 0x20 || characters[i] > 0x7E)
            return false;
    }
    return true;
}

static String encodeProtocolString(const String& protocol)
{
    StringBuilder builder;
    for (size_t i = 0; i < protocol.length(); i++) {
        if (protocol[i] < 0x20 || protocol[i] > 0x7E)
            builder.append(String::format("\\u%04X", protocol[i]));
        else if (protocol[i] == 0x5c)
            builder.append("\\\\");
        else
            builder.append(protocol[i]);
    }
    return builder.toString();
}

//SOMC KK PATCH Patches_w15.5_begin
static String joinStrings(const Vector<String>& strings, const char* separator)
{
    StringBuilder builder;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i)
            builder.append(separator);
        builder.append(strings[i]);
    }
    return builder.toString();
}

static unsigned long saturateAdd(unsigned long a, unsigned long b)
{
    if (numeric_limits<unsigned long>::max() - a < b)
        return numeric_limits<unsigned long>::max();
    return a + b;
}
//SOMC KK PATCH Patches_w15.5_end

static bool webSocketsAvailable = false;

void WebSocket::setIsAvailable(bool available)
{
    webSocketsAvailable = available;
}

bool WebSocket::isAvailable()
{
    return webSocketsAvailable;
}

//SOMC KK PATCH Patches_w15.5_begin
const char* WebSocket::subProtocolSeperator()
{
    return ", ";
}
//SOMC KK PATCH Patches_w15.5_end

WebSocket::WebSocket(ScriptExecutionContext* context)
    : ActiveDOMObject(context, this)
    , m_state(CONNECTING)
//SOMC KK PATCH Patches_w15.5_begin
    , m_bufferedAmount(0)
//SOMC KK PATCH Patches_w15.5_end
    , m_bufferedAmountAfterClose(0)
//SOMC KK PATCH Patches_w15.5_begin
    , m_binaryType(BinaryTypeBlob)
    , m_useHixie76Protocol(true)
    , m_subprotocol("")
    , m_extensions("")
//SOMC KK PATCH Patches_w15.5_end
{
}

WebSocket::~WebSocket()
{
    if (m_channel)
        m_channel->disconnect();
}

//SOMC KK PATCH Patches_w15.5_begin
PassRefPtr<WebSocket> WebSocket::create(ScriptExecutionContext* context)
{
    RefPtr<WebSocket> webSocket(adoptRef(new WebSocket(context)));
    //webSocket->suspendIfNeeded();
    return webSocket.release();
}
//SOMC KK PATCH Patches_w15.5_end
//SOMC KK PATCH Patches_w15.5_begin
//void WebSocket::connect(const KURL& url, ExceptionCode& ec)
//{
//    connect(url, String(), ec);
//}
void WebSocket::connect(const String& url, ExceptionCode& ec)
{
    Vector<String> protocols;
    connect(url, protocols, ec);
}
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
/*
void WebSocket::connect(const KURL& url, const String& protocol, ExceptionCode& ec)
{
    LOG(Network, "WebSocket %p connect to %s protocol=%s", this, url.string().utf8().data(), protocol.utf8().data());
    m_url = url;
    m_protocol = protocol;

    if (!m_url.isValid()) {
        scriptExecutionContext()->addMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "Invalid url for WebSocket " + url.string(), 0, scriptExecutionContext()->securityOrigin()->toString(), 0);
        m_state = CLOSED;
        ec = SYNTAX_ERR;
        return;
    }

    if (!m_url.protocolIs("ws") && !m_url.protocolIs("wss")) {
        scriptExecutionContext()->addMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "Wrong url scheme for WebSocket " + url.string(), 0, scriptExecutionContext()->securityOrigin()->toString(), 0);
        m_state = CLOSED;
        ec = SYNTAX_ERR;
        return;
    }
    if (m_url.hasFragmentIdentifier()) {
        scriptExecutionContext()->addMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "URL has fragment component " + url.string(), 0, scriptExecutionContext()->securityOrigin()->toString(), 0);
        m_state = CLOSED;
        ec = SYNTAX_ERR;
        return;
    }
    if (!isValidProtocolString(m_protocol)) {
        scriptExecutionContext()->addMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "Wrong protocol for WebSocket '" + encodeProtocolString(m_protocol) + "'", 0, scriptExecutionContext()->securityOrigin()->toString(), 0);
        m_state = CLOSED;
        ec = SYNTAX_ERR;
        return;
    }
    if (!portAllowed(url)) {
        scriptExecutionContext()->addMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, makeString("WebSocket port ", String::number(url.port()), " blocked"), 0, scriptExecutionContext()->securityOrigin()->toString(), 0);
        m_state = CLOSED;
        ec = SECURITY_ERR;
        return;
    }

    m_channel = ThreadableWebSocketChannel::create(scriptExecutionContext(), this, m_url, m_protocol);
    m_channel->connect();
    ActiveDOMObject::setPendingActivity(this);
}
*/
void WebSocket::connect(const String& url, const String& protocol, ExceptionCode& ec)
{
    Vector<String> protocols;
    protocols.append(protocol);
    connect(url, protocols, ec);
}

void WebSocket::connect(const String& url, const Vector<String>& protocols, ExceptionCode& ec)
{
    LOG(Network, "WebSocket %p connect to %s", this, url.utf8().data());
    m_url = KURL(KURL(), url);

    if (!m_url.isValid()) {
        scriptExecutionContext()->addConsoleMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "Invalid url for WebSocket " + m_url.string(), scriptExecutionContext()->securityOrigin()->toString());
        m_state = CLOSED;
        ec = SYNTAX_ERR;
        return;
    }

    if (!m_url.protocolIs("ws") && !m_url.protocolIs("wss")) {
        scriptExecutionContext()->addConsoleMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "Wrong url scheme for WebSocket " + m_url.string(), scriptExecutionContext()->securityOrigin()->toString());
        m_state = CLOSED;
        ec = SYNTAX_ERR;
        return;
    }
    if (m_url.hasFragmentIdentifier()) {
        scriptExecutionContext()->addConsoleMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "URL has fragment component " + m_url.string(), scriptExecutionContext()->securityOrigin()->toString());
        m_state = CLOSED;
        ec = SYNTAX_ERR;
        return;
    }
    if (!portAllowed(m_url)) {
        scriptExecutionContext()->addConsoleMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "WebSocket port " + String::number(m_url.port()) + " blocked", scriptExecutionContext()->securityOrigin()->toString());
        m_state = CLOSED;
        ec = SECURITY_ERR;
        return;
    }

    /*
    if (!scriptExecutionContext()->contentSecurityPolicy()->allowConnectFromSource(m_url)) {
        m_state = CLOSED;

        // FIXME: Should this be throwing an exception?
        ec = SECURITY_ERR;
        return;
    }
    */

    m_channel = ThreadableWebSocketChannel::create(scriptExecutionContext(), this);
    m_useHixie76Protocol = m_channel->useHixie76Protocol();

    String protocolString;
    if (m_useHixie76Protocol) {
        if (!protocols.isEmpty()) {
            // Emulate JavaScript's Array.toString() behavior.
            protocolString = joinStrings(protocols, ",");
        }
        if (!isValidProtocolStringHixie76(protocolString)) {
            scriptExecutionContext()->addConsoleMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "Wrong protocol for WebSocket '" + encodeProtocolString(protocolString) + "'", scriptExecutionContext()->securityOrigin()->toString());
            m_state = CLOSED;
            ec = SYNTAX_ERR;
            return;
        }
    } else {
        // FIXME: There is a disagreement about restriction of subprotocols between WebSocket API and hybi-10 protocol
        // draft. The former simply says "only characters in the range U+0021 to U+007E are allowed," while the latter
        // imposes a stricter rule: "the elements MUST be non-empty strings with characters as defined in [RFC2616],
        // and MUST all be unique strings."
        //
        // Here, we throw SYNTAX_ERR if the given protocols do not meet the latter criteria. This behavior does not
        // comply with WebSocket API specification, but it seems to be the only reasonable way to handle this conflict.
        for (size_t i = 0; i < protocols.size(); ++i) {
            if (!isValidProtocolString(protocols[i])) {
                scriptExecutionContext()->addConsoleMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "Wrong protocol for WebSocket '" + encodeProtocolString(protocols[i]) + "'", scriptExecutionContext()->securityOrigin()->toString());
                m_state = CLOSED;
                ec = SYNTAX_ERR;
                return;
            }
        }
        HashSet<String> visited;
        for (size_t i = 0; i < protocols.size(); ++i) {
            if (visited.contains(protocols[i])) {
                scriptExecutionContext()->addConsoleMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "WebSocket protocols contain duplicates: '" + encodeProtocolString(protocols[i]) + "'", scriptExecutionContext()->securityOrigin()->toString());
                m_state = CLOSED;
                ec = SYNTAX_ERR;
                return;
            }
            visited.add(protocols[i]);
        }

        if (!protocols.isEmpty())
            protocolString = joinStrings(protocols, subProtocolSeperator());
    }

    m_channel->connect(m_url, protocolString);
    ActiveDOMObject::setPendingActivity(this);
}

//SOMC KK PATCH Patches_w15.5_end

bool WebSocket::send(const String& message, ExceptionCode& ec)
{
    LOG(Network, "WebSocket %p send %s", this, message.utf8().data());
    if (m_state == CONNECTING) {
        ec = INVALID_STATE_ERR;
        return false;
    }
    // No exception is raised if the connection was once established but has subsequently been closed.
//SOMC KK PATCH Patches_w15.5_begin
//    if (m_state == CLOSED) {
//        m_bufferedAmountAfterClose += message.utf8().length() + 2; // 2 for frameing
    if (m_state == CLOSING || m_state == CLOSED) {
        size_t payloadSize = message.utf8().length();
        m_bufferedAmountAfterClose = saturateAdd(m_bufferedAmountAfterClose, payloadSize);
        m_bufferedAmountAfterClose = saturateAdd(m_bufferedAmountAfterClose, getFramingOverhead(payloadSize));
//SOMC KK PATCH Patches_w15.5_end
        return false;
    }
    // FIXME: check message is valid utf8.
    ASSERT(m_channel);
//SOMC KK PATCH Patches_w15.5_begin
//    return m_channel->send(message);
    ThreadableWebSocketChannel::SendResult result = m_channel->send(message);
    if (result == ThreadableWebSocketChannel::InvalidMessage) {
        scriptExecutionContext()->addConsoleMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "Websocket message contains invalid character(s).");
        ec = SYNTAX_ERR;
        return false;
    }
    return result == ThreadableWebSocketChannel::SendSuccess;
//SOMC KK PATCH Patches_w15.5_end
}

//SOMC KK PATCH Patches_w15.5_begin
bool WebSocket::send(ArrayBuffer* binaryData, ExceptionCode& ec)
{
    LOG(Network, "WebSocket %p send arraybuffer %p", this, binaryData);
    ASSERT(binaryData);
    if (m_useHixie76Protocol)
        return send("[object ArrayBuffer]", ec);
    if (m_state == CONNECTING) {
        ec = INVALID_STATE_ERR;
        return false;
    }
    if (m_state == CLOSING || m_state == CLOSED) {
        unsigned payloadSize = binaryData->byteLength();
        m_bufferedAmountAfterClose = saturateAdd(m_bufferedAmountAfterClose, payloadSize);
        m_bufferedAmountAfterClose = saturateAdd(m_bufferedAmountAfterClose, getFramingOverhead(payloadSize));
        return false;
    }
    ASSERT(m_channel);
    return m_channel->send(*binaryData) == ThreadableWebSocketChannel::SendSuccess;
}

bool WebSocket::send(Blob* binaryData, ExceptionCode& ec)
{
    LOG(Network, "WebSocket %p send blob %s", this, binaryData->url().string().utf8().data());
    ASSERT(binaryData);
    if (m_useHixie76Protocol)
        return send("[object Blob]", ec);
    if (m_state == CONNECTING) {
        ec = INVALID_STATE_ERR;
        return false;
    }
    if (m_state == CLOSING || m_state == CLOSED) {
        unsigned long payloadSize = static_cast<unsigned long>(binaryData->size());
        m_bufferedAmountAfterClose = saturateAdd(m_bufferedAmountAfterClose, payloadSize);
        m_bufferedAmountAfterClose = saturateAdd(m_bufferedAmountAfterClose, getFramingOverhead(payloadSize));
        return false;
    }
    ASSERT(m_channel);
    return m_channel->send(*binaryData) == ThreadableWebSocketChannel::SendSuccess;
}
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
/*
void WebSocket::close()
{
    LOG(Network, "WebSocket %p close", this);
    if (m_state == CLOSED)
        return;
    m_state = CLOSED;
    m_bufferedAmountAfterClose = m_channel->bufferedAmount();
    // didClose notification may be already queued, which we will inadvertently process while waiting for bufferedAmount() to return.
    // In this case m_channel will be set to null during didClose() call, thus we need to test validness of m_channel here.
    if (m_channel)
        m_channel->close();
}
*/
void WebSocket::close(int code, const String& reason, ExceptionCode& ec)
{
    if (code == WebSocketChannel::CloseEventCodeNotSpecified)
        LOG(Network, "WebSocket %p close without code and reason", this);
    else {
        LOG(Network, "WebSocket %p close with code = %d, reason = %s", this, code, reason.utf8().data());
        if (!(code == WebSocketChannel::CloseEventCodeNormalClosure || (WebSocketChannel::CloseEventCodeMinimumUserDefined <= code && code <= WebSocketChannel::CloseEventCodeMaximumUserDefined))) {
            ec = INVALID_ACCESS_ERR;
            return;
        }
        CString utf8 = reason.utf8(true);
        if (utf8.length() > maxReasonSizeInBytes) {
            scriptExecutionContext()->addConsoleMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "WebSocket close message is too long.");
            ec = SYNTAX_ERR;
            return;
        }
        // Checks whether reason is valid utf8.
        if (utf8.isNull() && reason.length()) {
            scriptExecutionContext()->addConsoleMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, "WebSocket close message contains invalid character(s).");
            ec = SYNTAX_ERR;
            return;
        }
    }

    if (m_state == CLOSING || m_state == CLOSED)
        return;
    if (m_state == CONNECTING) {
        m_state = CLOSING;
        m_channel->fail("WebSocket is closed before the connection is established.");
        return;
    }
    m_state = CLOSING;
    if (m_channel)
        m_channel->close(code, reason);
}
//SOMC KK PATCH Patches_w15.5_end

const KURL& WebSocket::url() const
{
    return m_url;
}

WebSocket::State WebSocket::readyState() const
{
    return m_state;
}

unsigned long WebSocket::bufferedAmount() const
{
//SOMC KK PATCH Patches_w15.5_begin
//    if (m_state == OPEN)
//        return m_channel->bufferedAmount();
//    return m_bufferedAmountAfterClose;
    return saturateAdd(m_bufferedAmount, m_bufferedAmountAfterClose);
//SOMC KK PATCH Patches_w15.5_end
}

//SOMC KK PATCH Patches_w15.5_begin
String WebSocket::protocol() const
{
    if (m_useHixie76Protocol)
        return String();
    return m_subprotocol;
}

String WebSocket::extensions() const
{
    if (m_useHixie76Protocol)
        return String();
    return m_extensions;
}

String WebSocket::binaryType() const
{
    if (m_useHixie76Protocol)
        return String();
    switch (m_binaryType) {
    case BinaryTypeBlob:
        return "blob";
    case BinaryTypeArrayBuffer:
        return "arraybuffer";
    }
    ASSERT_NOT_REACHED();
    return String();
}

void WebSocket::setBinaryType(const String& binaryType, ExceptionCode& ec)
{
    if (m_useHixie76Protocol)
        return;
    if (binaryType == "blob") {
        m_binaryType = BinaryTypeBlob;
        return;
    }
    if (binaryType == "arraybuffer") {
        m_binaryType = BinaryTypeArrayBuffer;
        return;
    }
    ec = SYNTAX_ERR;
    return;
}

    /*
const AtomicString& WebSocket::interfaceName() const
{
    return eventNames().interfaceForWebSocket;
}
    */
//SOMC KK PATCH Patches_w15.5_end

ScriptExecutionContext* WebSocket::scriptExecutionContext() const
{
    return ActiveDOMObject::scriptExecutionContext();
}

void WebSocket::contextDestroyed()
{
    LOG(Network, "WebSocket %p scriptExecutionContext destroyed", this);
    ASSERT(!m_channel);
    ASSERT(m_state == CLOSED);
    ActiveDOMObject::contextDestroyed();
}

bool WebSocket::canSuspend() const
{
    return !m_channel;
}

void WebSocket::suspend(ReasonForSuspension)
{
    if (m_channel)
        m_channel->suspend();
}

void WebSocket::resume()
{
    if (m_channel)
        m_channel->resume();
}

void WebSocket::stop()
{
    bool pending = hasPendingActivity();
    if (m_channel)
        m_channel->disconnect();
    m_channel = 0;
    m_state = CLOSED;
    ActiveDOMObject::stop();
    if (pending)
        ActiveDOMObject::unsetPendingActivity(this);
}

void WebSocket::didConnect()
{
    LOG(Network, "WebSocket %p didConnect", this);
    if (m_state != CONNECTING) {
//SOMC KK PATCH Patches_w15.5_begin
//        didClose(0);
        didClose(0, ClosingHandshakeIncomplete, WebSocketChannel::CloseEventCodeAbnormalClosure, "");
//SOMC KK PATCH Patches_w15.5_end
        return;
    }
    ASSERT(scriptExecutionContext());
    m_state = OPEN;
//SOMC KK PATCH Patches_w15.5_begin
    m_subprotocol = m_channel->subprotocol();
    m_extensions = m_channel->extensions();
//SOMC KK PATCH Patches_w15.5_end
    dispatchEvent(Event::create(eventNames().openEvent, false, false));
}

void WebSocket::didReceiveMessage(const String& msg)
{
    LOG(Network, "WebSocket %p didReceiveMessage %s", this, msg.utf8().data());
//SOMC KK PATCH Patches_w15.5_begin
//    if (m_state != OPEN)
    if (m_state != OPEN && m_state != CLOSING)
//SOMC KK PATCH Patches_w15.5_end
        return;
    ASSERT(scriptExecutionContext());
//SOMC KK PATCH Patches_w15.5_begin
//    RefPtr<MessageEvent> evt = MessageEvent::create();
//    evt->initMessageEvent(eventNames().messageEvent, false, false, SerializedScriptValue::create(msg), "", "", 0, 0);
//    dispatchEvent(evt);
    dispatchEvent(MessageEvent::create(msg));
//SOMC KK PATCH Patches_w15.5_end
}

//SOMC KK PATCH Patches_w15.5_begin
void WebSocket::didReceiveBinaryData(PassOwnPtr<Vector<char> > binaryData)
{
    switch (m_binaryType) {
    case BinaryTypeBlob: {
        size_t size = binaryData->size();
        RefPtr<RawData> rawData = RawData::create();
        binaryData->swap(*rawData->mutableData());
        OwnPtr<BlobData> blobData = BlobData::create();
        blobData->appendData(rawData.release(), 0, BlobDataItem::toEndOfFile);
        RefPtr<Blob> blob = Blob::create(blobData.release(), size);
        dispatchEvent(MessageEvent::create(blob.release()));
        break;
    }

    case BinaryTypeArrayBuffer:
        dispatchEvent(MessageEvent::create(ArrayBuffer::create(binaryData->data(), binaryData->size())));
        break;
    }
}
//SOMC KK PATCH Patches_w15.5_end

void WebSocket::didReceiveMessageError()
{
    LOG(Network, "WebSocket %p didReceiveErrorMessage", this);
//SOMC KK PATCH Patches_w15.5_begin
//    if (m_state != OPEN)
    if (m_state != OPEN && m_state != CLOSING)
//SOMC KK PATCH Patches_w15.5_end
        return;
    ASSERT(scriptExecutionContext());
    dispatchEvent(Event::create(eventNames().errorEvent, false, false));
}

//SOMC KK PATCH Patches_w15.5_begin
void WebSocket::didUpdateBufferedAmount(unsigned long bufferedAmount)
{
    LOG(Network, "WebSocket %p didUpdateBufferedAmount %lu", this, bufferedAmount);
    if (m_state == CLOSED)
        return;
    m_bufferedAmount = bufferedAmount;
}

void WebSocket::didStartClosingHandshake()
{
    LOG(Network, "WebSocket %p didStartClosingHandshake", this);
    m_state = CLOSING;
}
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//void WebSocket::didClose(unsigned long unhandledBufferedAmount)
void WebSocket::didClose(unsigned long unhandledBufferedAmount, ClosingHandshakeCompletionStatus closingHandshakeCompletion, unsigned short code, const String& reason)
//SOMC KK PATCH Patches_w15.5_end
{
    LOG(Network, "WebSocket %p didClose", this);
    if (!m_channel)
        return;
//SOMC KK PATCH Patches_w15.5_begin
//    m_state = CLOSED;
//    m_bufferedAmountAfterClose += unhandledBufferedAmount;
//    ASSERT(scriptExecutionContext());
//    dispatchEvent(Event::create(eventNames().closeEvent, false, false));
    bool wasClean = m_state == CLOSING && !unhandledBufferedAmount && closingHandshakeCompletion == ClosingHandshakeComplete;
    m_state = CLOSED;
    m_bufferedAmount = unhandledBufferedAmount;
    ASSERT(scriptExecutionContext());
    RefPtr<CloseEvent> event = CloseEvent::create(wasClean, code, reason);
    dispatchEvent(event);
//SOMC KK PATCH Patches_w15.5_end
    if (m_channel) {
        m_channel->disconnect();
        m_channel = 0;
    }
    if (hasPendingActivity())
        ActiveDOMObject::unsetPendingActivity(this);
}

EventTargetData* WebSocket::eventTargetData()
{
    return &m_eventTargetData;
}

EventTargetData* WebSocket::ensureEventTargetData()
{
    return &m_eventTargetData;
}

//SOMC KK PATCH Patches_w15.5_begin
size_t WebSocket::getFramingOverhead(size_t payloadSize)
{
    static const size_t hixie76FramingOverhead = 2; // Payload is surrounded by 0x00 and 0xFF.
    if (m_useHixie76Protocol)
        return hixie76FramingOverhead;

    static const size_t hybiBaseFramingOverhead = 2; // Every frame has at least two-byte header.
    static const size_t hybiMaskingKeyLength = 4; // Every frame from client must have masking key.
    static const size_t minimumPayloadSizeWithTwoByteExtendedPayloadLength = 126;
    static const size_t minimumPayloadSizeWithEightByteExtendedPayloadLength = 0x10000;
    size_t overhead = hybiBaseFramingOverhead + hybiMaskingKeyLength;
    if (payloadSize >= minimumPayloadSizeWithEightByteExtendedPayloadLength)
        overhead += 8;
    else if (payloadSize >= minimumPayloadSizeWithTwoByteExtendedPayloadLength)
        overhead += 2;
    return overhead;
}
//SOMC KK PATCH Patches_w15.5_end

}  // namespace WebCore

#endif
