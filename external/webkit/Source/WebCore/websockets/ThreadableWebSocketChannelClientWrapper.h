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

#ifndef ThreadableWebSocketChannelClientWrapper_h
#define ThreadableWebSocketChannelClientWrapper_h

//SOMC KK PATCH Patches_w15.5_begin
//#if ENABLE(WEB_SOCKETS)
#if ENABLE(WEB_SOCKETS) && ENABLE(WORKERS)
//SOMC KK PATCH Patches_w15.5_end

#include "PlatformString.h"
//SOMC KK PATCH Patches_w15.5_begin
#include "ScriptExecutionContext.h"
#include "ThreadableWebSocketChannel.h"
//SOMC KK PATCH Patches_w15.5_end
#include "WebSocketChannelClient.h"
//SOMC KK PATCH Patches_w15.5_begin
#include "WorkerThreadableWebSocketChannel.h"
//SOMC KK PATCH Patches_w15.5_end
#include <wtf/Forward.h>
//SOMC KK PATCH Patches_w15.5_begin
//#include <wtf/PassRefPtr.h>
#include <wtf/OwnPtr.h>
#include <wtf/PassOwnPtr.h>
//SOMC KK PATCH Patches_w15.5_end
#include <wtf/Threading.h>
#include <wtf/Vector.h>

namespace WebCore {

//SOMC KK PATCH Patches_w15.5_begin
class ScriptExecutionContext;
class WebSocketChannelClient;
//SOMC KK PATCH Patches_w15.5_end

class ThreadableWebSocketChannelClientWrapper : public ThreadSafeRefCounted<ThreadableWebSocketChannelClientWrapper> {
public:
//SOMC KK PATCH Patches_w15.5_begin
//    static PassRefPtr<ThreadableWebSocketChannelClientWrapper> create(WebSocketChannelClient* client)
//    {
//        return adoptRef(new ThreadableWebSocketChannelClientWrapper(client));
//    }
    static PassRefPtr<ThreadableWebSocketChannelClientWrapper> create(ScriptExecutionContext*, WebSocketChannelClient*);
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//    void clearSyncMethodDone()
//    {
//        m_syncMethodDone = false;
//    }
    void clearSyncMethodDone();
//SOMC KK PATCH Patches_w15.5_end
//SOMC KK PATCH Patches_w15.5_begin
//    void setSyncMethodDone()
//    {
//        m_syncMethodDone = true;
//    }
    void setSyncMethodDone();
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//    bool syncMethodDone() const
//    {
//        return m_syncMethodDone;
//    }
    bool syncMethodDone() const;
//SOMC KK PATCH Patches_w15.5_end
//SOMC KK PATCH Patches_w15.5_begin

//    bool sent() const
//    {
//        return m_sent;
//    }
//    void setSent(bool sent)
//    {
//        m_sent = sent;
//        m_syncMethodDone = true;
//    }
    WorkerThreadableWebSocketChannel::Peer* peer() const;
    void didCreateWebSocketChannel(WorkerThreadableWebSocketChannel::Peer*, bool useHixie76Protocol);
    void clearPeer();

    // The value of useHixie76Protocol flag is cachable; this value is saved after WebSocketChannel (on the main
    // thread) is constructed.
    bool useHixie76Protocol() const;

    // Subprotocol and extensions are cached too. Will be available when didConnect() callback is invoked.
    String subprotocol() const;
    void setSubprotocol(const String&);
    String extensions() const;
    void setExtensions(const String&);

    ThreadableWebSocketChannel::SendResult sendRequestResult() const;
    void setSendRequestResult(ThreadableWebSocketChannel::SendResult);
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//    unsigned long bufferedAmount() const
//    {
//        return m_bufferedAmount;
//    }
    unsigned long bufferedAmount() const;
//SOMC KK PATCH Patches_w15.5_end
//SOMC KK PATCH Patches_w15.5_begin
//    void setBufferedAmount(unsigned long bufferedAmount)
//    {
//        m_bufferedAmount = bufferedAmount;
//        m_syncMethodDone = true;
//    }
    void setBufferedAmount(unsigned long);
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//    void clearClient()
//    {
//        m_client = 0;
//    }
    void clearClient();
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//    void didConnect()
//    {
//        m_pendingConnected = true;
//        if (!m_suspended)
//            processPendingEvents();
//    }
    void didConnect();
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//    void didReceiveMessage(const String& msg)
//    {
//        m_pendingMessages.append(msg);
//        if (!m_suspended)
//            processPendingEvents();
//    }
    void didReceiveMessage(const String& message);

    void didReceiveBinaryData(PassOwnPtr<Vector<char> >);
    void didUpdateBufferedAmount(unsigned long bufferedAmount);
    void didStartClosingHandshake();

//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//    void didClose(unsigned long unhandledBufferedAmount)
//    {
//        m_pendingClosed = true;
//        m_bufferedAmount = unhandledBufferedAmount;
//        if (!m_suspended)
//            processPendingEvents();
//    }
    void didClose(unsigned long unhandledBufferedAmount, WebSocketChannelClient::ClosingHandshakeCompletionStatus, unsigned short code, const String& reason);
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//    void suspend()
//    {
//        m_suspended = true;
//    }
    void suspend();
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//    void resume()
//    {
//        m_suspended = false;
//        processPendingEvents();
//    }
    void resume();
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//protected:
//    ThreadableWebSocketChannelClientWrapper(WebSocketChannelClient* client)
//        : m_client(client)
//        , m_syncMethodDone(false)
//        , m_sent(false)
//        , m_bufferedAmount(0)
//        , m_suspended(false)
//        , m_pendingConnected(false)
//        , m_pendingClosed(false)
//    {
//    }

//    void processPendingEvents()
//    {
//        ASSERT(!m_suspended);
//        if (m_pendingConnected) {
//            m_pendingConnected = false;
//            if (m_client)
//                m_client->didConnect();
//        }

//        Vector<String> messages;
//        messages.swap(m_pendingMessages);
//        for (Vector<String>::const_iterator iter = messages.begin(); iter != messages.end(); ++iter) {
//            if (m_client)
//                m_client->didReceiveMessage(*iter);
//        }

//        if (m_pendingClosed) {
//            m_pendingClosed = false;
//            if (m_client)
//                m_client->didClose(m_bufferedAmount);
//        }
//    }
private:
    ThreadableWebSocketChannelClientWrapper(ScriptExecutionContext*, WebSocketChannelClient*);

    void processPendingTasks();

    static void didConnectCallback(ScriptExecutionContext*, PassRefPtr<ThreadableWebSocketChannelClientWrapper>);
    static void didReceiveMessageCallback(ScriptExecutionContext*, PassRefPtr<ThreadableWebSocketChannelClientWrapper>, const String& message);
    static void didReceiveBinaryDataCallback(ScriptExecutionContext*, PassRefPtr<ThreadableWebSocketChannelClientWrapper>, PassOwnPtr<Vector<char> >);
    static void didUpdateBufferedAmountCallback(ScriptExecutionContext*, PassRefPtr<ThreadableWebSocketChannelClientWrapper>, unsigned long bufferedAmount);
    static void didStartClosingHandshakeCallback(ScriptExecutionContext*, PassRefPtr<ThreadableWebSocketChannelClientWrapper>);
    static void didCloseCallback(ScriptExecutionContext*, PassRefPtr<ThreadableWebSocketChannelClientWrapper>, unsigned long unhandledBufferedAmount, WebSocketChannelClient::ClosingHandshakeCompletionStatus, unsigned short code, const String& reason);
    static void processPendingTasksCallback(ScriptExecutionContext*, PassRefPtr<ThreadableWebSocketChannelClientWrapper>);

    ScriptExecutionContext* m_context;

//SOMC KK PATCH Patches_w15.5_end

    WebSocketChannelClient* m_client;
//SOMC KK PATCH Patches_w15.5_begin
    WorkerThreadableWebSocketChannel::Peer* m_peer;
//SOMC KK PATCH Patches_w15.5_end
    bool m_syncMethodDone;
//SOMC KK PATCH Patches_w15.5_begin
//    bool m_sent;
    bool m_useHixie76Protocol;
    // ThreadSafeRefCounted must not have String member variables.
    Vector<UChar> m_subprotocol;
    Vector<UChar> m_extensions;
    ThreadableWebSocketChannel::SendResult m_sendRequestResult;
//SOMC KK PATCH Patches_w15.5_end
    unsigned long m_bufferedAmount;
    bool m_suspended;
//SOMC KK PATCH Patches_w15.5_begin
//    bool m_pendingConnected;
//    Vector<String> m_pendingMessages;
//    bool m_pendingClosed;
    Vector<OwnPtr<ScriptExecutionContext::Task> > m_pendingTasks;
//SOMC KK PATCH Patches_w15.5_end
};

} // namespace WebCore

#endif // ENABLE(WEB_SOCKETS)

#endif // ThreadableWebSocketChannelClientWrapper_h
