/*
 * Copyright (C) 2010 Google Inc.  All rights reserved.
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

#ifndef WebSocketHandshakeRequest_h
#define WebSocketHandshakeRequest_h

#if ENABLE(WEB_SOCKETS)

//SOMC KK PATCH Patches_w15.5_begin
//#include "HTTPHeaderMap.h"
//#include "KURL.h"
//#include "PlatformString.h"
#include "HTTPRequest.h"
//SOMC KK PATCH Patches_w15.5_end

namespace WebCore {

//SOMC KK PATCH Patches_w15.5_begin
//class WebSocketHandshakeRequest {
class WebSocketHandshakeRequest : public HTTPRequest {
//SOMC KK PATCH Patches_w15.5_end
public:
//SOMC KK PATCH Patches_w15.5_begin
//    WebSocketHandshakeRequest(const String& requestMethod, const KURL&);
    static PassRefPtr<WebSocketHandshakeRequest> create(const String& requestMethod, const KURL& url) { return adoptRef(new WebSocketHandshakeRequest(requestMethod, url)); }
//SOMC KK PATCH Patches_w15.5_end
    ~WebSocketHandshakeRequest();

//SOMC KK PATCH Patches_w15.5_begin
//    String requestMethod() const;
//    KURL url() const;

//    const HTTPHeaderMap& headerFields() const;
//    void addHeaderField(const char* name, const String& value);
//SOMC KK PATCH Patches_w15.5_end

    struct Key3 {
        unsigned char value[8];

        Key3();
        void set(const unsigned char key3[8]);
    };
    Key3 key3() const;
    void setKey3(const unsigned char key3[8]);

private:
//SOMC KK PATCH Patches_w15.5_begin
//    KURL m_url;
//    String m_requestMethod;
//    HTTPHeaderMap m_headerFields;
    WebSocketHandshakeRequest(const String& requestMethod, const KURL&);
//SOMC KK PATCH Patches_w15.5_end
    Key3 m_key3;
};

} // namespace WebCore

#endif // ENABLE(WEB_SOCKETS)

#endif // WebSocketHandshakeRequest_h
