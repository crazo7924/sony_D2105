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

#ifndef ThreadableWebSocketChannel_h
#define ThreadableWebSocketChannel_h

#if ENABLE(WEB_SOCKETS)

//SOMC KK PATCH Patches_w15.5_begin
#include "ArrayBuffer.h"
//SOMC KK PATCH Patches_w15.5_end

#include <wtf/Forward.h>
#include <wtf/Noncopyable.h>
#include <wtf/PassRefPtr.h>

namespace WebCore {

//SOMC KK PATCH Patches_w15.5_begin
class Blob;
//SOMC KK PATCH Patches_w15.5_end
class KURL;
class ScriptExecutionContext;
class WebSocketChannelClient;

class ThreadableWebSocketChannel {
    WTF_MAKE_NONCOPYABLE(ThreadableWebSocketChannel);
public:
    ThreadableWebSocketChannel() { }
//SOMC KK PATCH Patches_w15.5_begin
//    static PassRefPtr<ThreadableWebSocketChannel> create(ScriptExecutionContext*, WebSocketChannelClient*, const KURL&, const String& protocol);
    static PassRefPtr<ThreadableWebSocketChannel> create(ScriptExecutionContext*, WebSocketChannelClient*);
//SOMC KK PATCH Patches_w15.5_end


//SOMC KK PATCH Patches_w15.5_begin
//    virtual void connect() = 0;
//    virtual bool send(const String& message) = 0;
    enum SendResult {
        SendSuccess,
        SendFail,
        InvalidMessage
    };

    virtual bool useHixie76Protocol() = 0;
    virtual void connect(const KURL&, const String& protocol) = 0;
    virtual String subprotocol() = 0; // Will be available after didConnect() callback is invoked.
    virtual String extensions() = 0; // Will be available after didConnect() callback is invoked.
    virtual SendResult send(const String& message) = 0;
    virtual SendResult send(const ArrayBuffer&) = 0;
    virtual SendResult send(const Blob&) = 0;

//SOMC KK PATCH Patches_w15.5_end
    virtual unsigned long bufferedAmount() const = 0;
//SOMC KK PATCH Patches_w15.5_begin
//    virtual void close() = 0;
    virtual void close(int code, const String& reason) = 0;
    // Log the reason text and close the connection. Will call didClose().
    virtual void fail(const String& reason) = 0;
//SOMC KK PATCH Patches_w15.5_end
    virtual void disconnect() = 0; // Will suppress didClose().

    virtual void suspend() = 0;
    virtual void resume() = 0;

    void ref() { refThreadableWebSocketChannel(); }
    void deref() { derefThreadableWebSocketChannel(); }

protected:
    virtual ~ThreadableWebSocketChannel() { }
    virtual void refThreadableWebSocketChannel() = 0;
    virtual void derefThreadableWebSocketChannel() = 0;
};

} // namespace WebCore

#endif // ENABLE(WEB_SOCKETS)

#endif // ThreadableWebSocketChannel_h
