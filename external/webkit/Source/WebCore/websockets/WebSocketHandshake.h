/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
 * Copyright (C) 2012 Sony Mobile Communications AB
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

#ifndef WebSocketHandshake_h
#define WebSocketHandshake_h

#if ENABLE(WEB_SOCKETS)

#include "KURL.h"
#include "PlatformString.h"
//SOMC KK PATCH Patches_w15.5_begin
#include "WebSocketExtensionDispatcher.h"
#include "WebSocketExtensionProcessor.h"
//SOMC KK PATCH Patches_w15.5_end
#include "WebSocketHandshakeRequest.h"
#include "WebSocketHandshakeResponse.h"
//SOMC KK PATCH Patches_w15.5_begin
#include <wtf/PassOwnPtr.h>
//SOMC KK PATCH Patches_w15.5_end

namespace WebCore {

class ScriptExecutionContext;

class WebSocketHandshake {
    WTF_MAKE_NONCOPYABLE(WebSocketHandshake);
public:
    enum Mode {
        Incomplete, Normal, Failed, Connected
    };
//SOMC KK PATCH Patches_w15.5_begin
//        WebSocketHandshake(const KURL&, const String& protocol, ScriptExecutionContext*);
    WebSocketHandshake(const KURL&, const String& protocol, ScriptExecutionContext*, bool useHixie76Protocol);
//SOMC KK PATCH Patches_w15.5_end
    ~WebSocketHandshake();

    const KURL& url() const;
    void setURL(const KURL&);
    const String host() const;

    const String& clientProtocol() const;
//SOMC KK PATCH Patches_w15.5_begin
//        void setClientProtocol(const String& protocol);
    void setClientProtocol(const String&);
//SOMC KK PATCH Patches_w15.5_end

    bool secure() const;

    String clientOrigin() const;
    String clientLocation() const;

    CString clientHandshakeMessage() const;
//SOMC KK PATCH Patches_w15.5_begin
//        WebSocketHandshakeRequest clientHandshakeRequest() const;
    PassRefPtr<WebSocketHandshakeRequest> clientHandshakeRequest() const;
//SOMC KK PATCH Patches_w15.5_end

    void reset();
    void clearScriptExecutionContext();

    int readServerHandshake(const char* header, size_t len);
    Mode mode() const;
//SOMC KK PATCH Patches_w15.5_begin
    String failureReason() const; // Returns a string indicating the reason of failure if mode() == Failed.
//SOMC KK PATCH Patches_w15.5_end

    String serverWebSocketOrigin() const; // Only for hixie-76 handshake.
    String serverWebSocketLocation() const; // Only for hixie-76 handshake.
    String serverWebSocketProtocol() const;
    String serverSetCookie() const;
    String serverSetCookie2() const;
    String serverUpgrade() const;
    String serverConnection() const;
//SOMC KK PATCH Patches_w15.5_begin
    String serverWebSocketAccept() const; // Only for hybi-10 handshake.
    String acceptedExtensions();
//SOMC KK PATCH Patches_w15.5_end

    const WebSocketHandshakeResponse& serverHandshakeResponse() const;

//SOMC KK PATCH Patches_w15.5_begin
    void addExtensionProcessor(PassOwnPtr<WebSocketExtensionProcessor>);

    static String getExpectedWebSocketAccept(const String& secWebSocketKey);
//SOMC KK PATCH Patches_w15.5_end

private:
    KURL httpURLForAuthenticationAndCookies() const;

    int readStatusLine(const char* header, size_t headerLength, int& statusCode, String& statusText);

    // Reads all headers except for the two predefined ones.
    const char* readHTTPHeaders(const char* start, const char* end);
    void processHeaders();
    bool checkResponseHeaders();

    KURL m_url;
    String m_clientProtocol;
    bool m_secure;
    ScriptExecutionContext* m_context;
//SOMC KK PATCH Patches_w15.5_begin
    bool m_useHixie76Protocol;
//SOMC KK PATCH Patches_w15.5_end

    Mode m_mode;

//SOMC KK PATCH Patches_w15.5_begin
//        String m_secWebSocketKey1;
//        String m_secWebSocketKey2;
//        unsigned char m_key3[8];
//        unsigned char m_expectedChallengeResponse[16];
//SOMC KK PATCH Patches_w15.5_end

    WebSocketHandshakeResponse m_response;

//SOMC KK PATCH Patches_w15.5_begin
    String m_failureReason;

    // For hixie-76 handshake.
    String m_hixie76SecWebSocketKey1;
    String m_hixie76SecWebSocketKey2;
    unsigned char m_hixie76Key3[8];
    unsigned char m_hixie76ExpectedChallengeResponse[16];

    // For hybi-10 handshake.
    String m_secWebSocketKey;
    String m_expectedAccept;

    WebSocketExtensionDispatcher m_extensionDispatcher;
//SOMC KK PATCH Patches_w15.5_end
};

} // namespace WebCore

#endif // ENABLE(WEB_SOCKETS)

#endif // WebSocketHandshake_h
