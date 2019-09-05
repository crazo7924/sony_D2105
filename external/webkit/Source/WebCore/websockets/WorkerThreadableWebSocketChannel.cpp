/*
 * Copyright (C) 2011, 2012 Google Inc.  All rights reserved.
 * Copyright (C) 2014 Sony Mobile Communications AB
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

#if ENABLE(WEB_SOCKETS) && ENABLE(WORKERS)

#include "WorkerThreadableWebSocketChannel.h"

//SOMC KK PATCH Patches_w15.5_begin
#include "Blob.h"
//SOMC KK PATCH Patches_w15.5_end
#include "CrossThreadTask.h"
//SOMC KK PATCH Patches_w15.5_begin
#include "Document.h"
//SOMC KK PATCH Patches_w15.5_end
#include "PlatformString.h"
#include "ScriptExecutionContext.h"
#include "ThreadableWebSocketChannelClientWrapper.h"
#include "WebSocketChannel.h"
#include "WebSocketChannelClient.h"
#include "WorkerContext.h"
#include "WorkerLoaderProxy.h"
#include "WorkerRunLoop.h"
#include "WorkerThread.h"
//SOMC KK PATCH Patches_w15.5_begin
#include "ArrayBuffer.h"
#include <wtf/MainThread.h>
//SOMC KK PATCH Patches_w15.5_end
#include <wtf/PassRefPtr.h>

namespace WebCore {

//SOMC KK PATCH Patches_w15.5_begin
//WorkerThreadableWebSocketChannel::WorkerThreadableWebSocketChannel(WorkerContext* context, WebSocketChannelClient* client, const String& taskMode, const KURL& url, const String& protocol)
//    : m_workerContext(context)
//    , m_workerClientWrapper(ThreadableWebSocketChannelClientWrapper::create(client))
//    , m_bridge(Bridge::create(m_workerClientWrapper, m_workerContext, taskMode, url, protocol))
//{
//}
WorkerThreadableWebSocketChannel::WorkerThreadableWebSocketChannel(WorkerContext* context, WebSocketChannelClient* client, const String& taskMode)
    : m_workerContext(context)
    , m_workerClientWrapper(ThreadableWebSocketChannelClientWrapper::create(context, client))
    , m_bridge(Bridge::create(m_workerClientWrapper, m_workerContext, taskMode))
{
    m_bridge->initialize();
}
//SOMC KK PATCH Patches_w15.5_end

WorkerThreadableWebSocketChannel::~WorkerThreadableWebSocketChannel()
{
    if (m_bridge)
        m_bridge->disconnect();
}

//SOMC KK PATCH Patches_w15.5_begin
bool WorkerThreadableWebSocketChannel::useHixie76Protocol()
{
    ASSERT(m_workerClientWrapper);
    return m_workerClientWrapper->useHixie76Protocol();
}
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//void WorkerThreadableWebSocketChannel::connect()
//{
//    if (m_bridge)
//        m_bridge->connect();
//}
void WorkerThreadableWebSocketChannel::connect(const KURL& url, const String& protocol)
{
    if (m_bridge)
        m_bridge->connect(url, protocol);
}
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//bool WorkerThreadableWebSocketChannel::send(const String& message)
//{
//    if (!m_bridge)
//        return false;
//    return m_bridge->send(message);
//}

String WorkerThreadableWebSocketChannel::subprotocol()
{
    ASSERT(m_workerClientWrapper);
    return m_workerClientWrapper->subprotocol();
}

String WorkerThreadableWebSocketChannel::extensions()
{
    ASSERT(m_workerClientWrapper);
    return m_workerClientWrapper->extensions();
}

ThreadableWebSocketChannel::SendResult WorkerThreadableWebSocketChannel::send(const String& message)
{
    if (!m_bridge)
        return ThreadableWebSocketChannel::SendFail;
    return m_bridge->send(message);
}

ThreadableWebSocketChannel::SendResult WorkerThreadableWebSocketChannel::send(const ArrayBuffer& binaryData)
{
    if (!m_bridge)
        return ThreadableWebSocketChannel::SendFail;
    return m_bridge->send(binaryData);
}

ThreadableWebSocketChannel::SendResult WorkerThreadableWebSocketChannel::send(const Blob& binaryData)
{
    if (!m_bridge)
        return ThreadableWebSocketChannel::SendFail;
    return m_bridge->send(binaryData);
}

//SOMC KK PATCH Patches_w15.5_end
unsigned long WorkerThreadableWebSocketChannel::bufferedAmount() const
{
    if (!m_bridge)
        return 0;
    return m_bridge->bufferedAmount();
}

//SOMC KK PATCH Patches_w15.5_begin
void WorkerThreadableWebSocketChannel::close(int code, const String& reason)
{
    if (m_bridge)
        m_bridge->close(code, reason);
}
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//void WorkerThreadableWebSocketChannel::close()
//{
//    if (m_bridge)
//        m_bridge->close();
//}
void WorkerThreadableWebSocketChannel::fail(const String& reason)
{
    if (m_bridge)
        m_bridge->fail(reason);
}
//SOMC KK PATCH Patches_w15.5_end

void WorkerThreadableWebSocketChannel::disconnect()
{
    m_bridge->disconnect();
    m_bridge.clear();
}

void WorkerThreadableWebSocketChannel::suspend()
{
    m_workerClientWrapper->suspend();
    if (m_bridge)
        m_bridge->suspend();
}

void WorkerThreadableWebSocketChannel::resume()
{
    m_workerClientWrapper->resume();
    if (m_bridge)
        m_bridge->resume();
}

//SOMC KK PATCH Patches_w15.5_begin
//WorkerThreadableWebSocketChannel::Peer::Peer(RefPtr<ThreadableWebSocketChannelClientWrapper> clientWrapper, WorkerLoaderProxy& loaderProxy, ScriptExecutionContext* context, const String& taskMode, const KURL& url, const String& protocol)
WorkerThreadableWebSocketChannel::Peer::Peer(PassRefPtr<ThreadableWebSocketChannelClientWrapper> clientWrapper, WorkerLoaderProxy& loaderProxy, ScriptExecutionContext* context, const String& taskMode)
//SOMC KK PATCH Patches_w15.5_end
    : m_workerClientWrapper(clientWrapper)
    , m_loaderProxy(loaderProxy)
//SOMC KK PATCH Patches_w15.5_begin
//    , m_mainWebSocketChannel(WebSocketChannel::create(context, this, url, protocol))
    , m_mainWebSocketChannel(WebSocketChannel::create(static_cast<Document*>(context), this))
//SOMC KK PATCH Patches_w15.5_end
    , m_taskMode(taskMode)
{
    ASSERT(isMainThread());
}

WorkerThreadableWebSocketChannel::Peer::~Peer()
{
    ASSERT(isMainThread());
    if (m_mainWebSocketChannel)
        m_mainWebSocketChannel->disconnect();
}

//SOMC KK PATCH Patches_w15.5_begin
bool WorkerThreadableWebSocketChannel::Peer::useHixie76Protocol()
{
    ASSERT(isMainThread());
    ASSERT(m_mainWebSocketChannel);
    return m_mainWebSocketChannel->useHixie76Protocol();
}
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//void WorkerThreadableWebSocketChannel::Peer::connect()
void WorkerThreadableWebSocketChannel::Peer::connect(const KURL& url, const String& protocol)
//SOMC KK PATCH Patches_w15.5_end
{
    ASSERT(isMainThread());
    if (!m_mainWebSocketChannel)
        return;
//SOMC KK PATCH Patches_w15.5_begin
//    m_mainWebSocketChannel->connect();
    m_mainWebSocketChannel->connect(url, protocol);
//SOMC KK PATCH Patches_w15.5_end
}

//SOMC KK PATCH Patches_w15.5_begin
//static void workerContextDidSend(ScriptExecutionContext* context, RefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, bool sent)
static void workerContextDidSend(ScriptExecutionContext* context, PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, ThreadableWebSocketChannel::SendResult sendRequestResult)
//SOMC KK PATCH Patches_w15.5_end
{
    ASSERT_UNUSED(context, context->isWorkerContext());
//SOMC KK PATCH Patches_w15.5_begin
//    workerClientWrapper->setSent(sent);
    workerClientWrapper->setSendRequestResult(sendRequestResult);
//SOMC KK PATCH Patches_w15.5_end
}

void WorkerThreadableWebSocketChannel::Peer::send(const String& message)
{
    ASSERT(isMainThread());
    if (!m_mainWebSocketChannel || !m_workerClientWrapper)
        return;
//SOMC KK PATCH Patches_w15.5_begin
//    bool sent = m_mainWebSocketChannel->send(message);
//    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidSend, m_workerClientWrapper, sent), m_taskMode);
    ThreadableWebSocketChannel::SendResult sendRequestResult = m_mainWebSocketChannel->send(message);
    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidSend, m_workerClientWrapper, sendRequestResult), m_taskMode);
//SOMC KK PATCH Patches_w15.5_end
}

//SOMC KK PATCH Patches_w15.5_begin
void WorkerThreadableWebSocketChannel::Peer::send(const ArrayBuffer& binaryData)
{
    ASSERT(isMainThread());
    if (!m_mainWebSocketChannel || !m_workerClientWrapper)
        return;
    ThreadableWebSocketChannel::SendResult sendRequestResult = m_mainWebSocketChannel->send(binaryData);
    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidSend, m_workerClientWrapper, sendRequestResult), m_taskMode);
}

void WorkerThreadableWebSocketChannel::Peer::send(const Blob& binaryData)
{
    ASSERT(isMainThread());
    if (!m_mainWebSocketChannel || !m_workerClientWrapper)
        return;
    ThreadableWebSocketChannel::SendResult sendRequestResult = m_mainWebSocketChannel->send(binaryData);
    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidSend, m_workerClientWrapper, sendRequestResult), m_taskMode);
}
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//static void workerContextDidGetBufferedAmount(ScriptExecutionContext* context, RefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, unsigned long bufferedAmount)
static void workerContextDidGetBufferedAmount(ScriptExecutionContext* context, PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, unsigned long bufferedAmount)
//SOMC KK PATCH Patches_w15.5_end
{
    ASSERT_UNUSED(context, context->isWorkerContext());
    workerClientWrapper->setBufferedAmount(bufferedAmount);
}

void WorkerThreadableWebSocketChannel::Peer::bufferedAmount()
{
    ASSERT(isMainThread());
    if (!m_mainWebSocketChannel || !m_workerClientWrapper)
        return;
    unsigned long bufferedAmount = m_mainWebSocketChannel->bufferedAmount();
    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidGetBufferedAmount, m_workerClientWrapper, bufferedAmount), m_taskMode);
}

//SOMC KK PATCH Patches_w15.5_begin
//void WorkerThreadableWebSocketChannel::Peer::close()
void WorkerThreadableWebSocketChannel::Peer::close(int code, const String& reason)
//SOMC KK PATCH Patches_w15.5_end
{
    ASSERT(isMainThread());
    if (!m_mainWebSocketChannel)
        return;
//SOMC KK PATCH Patches_w15.5_begin
//    m_mainWebSocketChannel->close();
//    m_mainWebSocketChannel = 0;
    m_mainWebSocketChannel->close(code, reason);
//SOMC KK PATCH Patches_w15.5_end
}

//SOMC KK PATCH Patches_w15.5_begin
void WorkerThreadableWebSocketChannel::Peer::fail(const String& reason)
{
    ASSERT(isMainThread());
    if (!m_mainWebSocketChannel)
        return;
    m_mainWebSocketChannel->fail(reason);
}
//SOMC KK PATCH Patches_w15.5_end

void WorkerThreadableWebSocketChannel::Peer::disconnect()
{
    ASSERT(isMainThread());
    if (!m_mainWebSocketChannel)
        return;
    m_mainWebSocketChannel->disconnect();
    m_mainWebSocketChannel = 0;
}

void WorkerThreadableWebSocketChannel::Peer::suspend()
{
    ASSERT(isMainThread());
    if (!m_mainWebSocketChannel)
        return;
    m_mainWebSocketChannel->suspend();
}

void WorkerThreadableWebSocketChannel::Peer::resume()
{
    ASSERT(isMainThread());
    if (!m_mainWebSocketChannel)
        return;
    m_mainWebSocketChannel->resume();
}

//SOMC KK PATCH Patches_w15.5_begin
//static void workerContextDidConnect(ScriptExecutionContext* context, RefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper)
static void workerContextDidConnect(ScriptExecutionContext* context, PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, const String& subprotocol, const String& extensions)
//SOMC KK PATCH Patches_w15.5_end
{
    ASSERT_UNUSED(context, context->isWorkerContext());
//SOMC KK PATCH Patches_w15.5_begin
    workerClientWrapper->setSubprotocol(subprotocol);
    workerClientWrapper->setExtensions(extensions);
//SOMC KK PATCH Patches_w15.5_end
    workerClientWrapper->didConnect();
}

void WorkerThreadableWebSocketChannel::Peer::didConnect()
{
    ASSERT(isMainThread());
//SOMC KK PATCH Patches_w15.5_begin
//    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidConnect, m_workerClientWrapper), m_taskMode);
    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidConnect, m_workerClientWrapper, m_mainWebSocketChannel->subprotocol(), m_mainWebSocketChannel->extensions()), m_taskMode);
//SOMC KK PATCH Patches_w15.5_end
}

//SOMC KK PATCH Patches_w15.5_begin
//static void workerContextDidReceiveMessage(ScriptExecutionContext* context, RefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, const String& message)
static void workerContextDidReceiveMessage(ScriptExecutionContext* context, PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, const String& message)
//SOMC KK PATCH Patches_w15.5_end
{
    ASSERT_UNUSED(context, context->isWorkerContext());
    workerClientWrapper->didReceiveMessage(message);
}

void WorkerThreadableWebSocketChannel::Peer::didReceiveMessage(const String& message)
{
    ASSERT(isMainThread());
    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidReceiveMessage, m_workerClientWrapper, message), m_taskMode);
}

//SOMC KK PATCH Patches_w15.5_begin
static void workerContextDidReceiveBinaryData(ScriptExecutionContext* context, PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, PassOwnPtr<Vector<char> > binaryData)
{
    ASSERT_UNUSED(context, context->isWorkerContext());
    workerClientWrapper->didReceiveBinaryData(binaryData);
}

void WorkerThreadableWebSocketChannel::Peer::didReceiveBinaryData(PassOwnPtr<Vector<char> > binaryData)
{
    ASSERT(isMainThread());
    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidReceiveBinaryData, m_workerClientWrapper, binaryData), m_taskMode);
}
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
/*
static void workerContextDidClose(ScriptExecutionContext* context, RefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, unsigned long unhandledBufferedAmount)
{
    ASSERT_UNUSED(context, context->isWorkerContext());
    workerClientWrapper->didClose(unhandledBufferedAmount);
}

void WorkerThreadableWebSocketChannel::Peer::didClose(unsigned long unhandledBufferedAmount)
{
    ASSERT(isMainThread());
    m_mainWebSocketChannel = 0;
    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidClose, m_workerClientWrapper, unhandledBufferedAmount), m_taskMode);
}

void WorkerThreadableWebSocketChannel::Bridge::setWebSocketChannel(ScriptExecutionContext* context, Bridge* thisPtr, Peer* peer, RefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper)
{
    ASSERT_UNUSED(context, context->isWorkerContext());
    thisPtr->m_peer = peer;
    workerClientWrapper->setSyncMethodDone();
}

void WorkerThreadableWebSocketChannel::Bridge::mainThreadCreateWebSocketChannel(ScriptExecutionContext* context, Bridge* thisPtr, RefPtr<ThreadableWebSocketChannelClientWrapper> clientWrapper, const String& taskMode, const KURL& url, const String& protocol)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());

    Peer* peer = Peer::create(clientWrapper, thisPtr->m_loaderProxy, context, taskMode, url, protocol);
    thisPtr->m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&Bridge::setWebSocketChannel, thisPtr, peer, clientWrapper), taskMode);
}

WorkerThreadableWebSocketChannel::Bridge::Bridge(PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, PassRefPtr<WorkerContext> workerContext, const String& taskMode, const KURL& url, const String& protocol)
    : m_workerClientWrapper(workerClientWrapper)
    , m_workerContext(workerContext)
    , m_loaderProxy(m_workerContext->thread()->workerLoaderProxy())
    , m_taskMode(taskMode)
    , m_peer(0)
{
    ASSERT(m_workerClientWrapper.get());
    setMethodNotCompleted();
    m_loaderProxy.postTaskToLoader(createCallbackTask(&Bridge::mainThreadCreateWebSocketChannel, this, m_workerClientWrapper, m_taskMode, url, protocol));
    waitForMethodCompletion();
    ASSERT(m_peer);
}
*/
static void workerContextDidUpdateBufferedAmount(ScriptExecutionContext* context, PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, unsigned long bufferedAmount)
{
    ASSERT_UNUSED(context, context->isWorkerContext());
    workerClientWrapper->didUpdateBufferedAmount(bufferedAmount);
}

void WorkerThreadableWebSocketChannel::Peer::didUpdateBufferedAmount(unsigned long bufferedAmount)
{
    ASSERT(isMainThread());
    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidUpdateBufferedAmount, m_workerClientWrapper, bufferedAmount), m_taskMode);
}

static void workerContextDidStartClosingHandshake(ScriptExecutionContext* context, PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper)
{
    ASSERT_UNUSED(context, context->isWorkerContext());
    workerClientWrapper->didStartClosingHandshake();
}

void WorkerThreadableWebSocketChannel::Peer::didStartClosingHandshake()
{
    ASSERT(isMainThread());
    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidStartClosingHandshake, m_workerClientWrapper), m_taskMode);
}

static void workerContextDidClose(ScriptExecutionContext* context, PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, unsigned long unhandledBufferedAmount, WebSocketChannelClient::ClosingHandshakeCompletionStatus closingHandshakeCompletion, unsigned short code, const String& reason)
{
    ASSERT_UNUSED(context, context->isWorkerContext());
    workerClientWrapper->didClose(unhandledBufferedAmount, closingHandshakeCompletion, code, reason);
}

void WorkerThreadableWebSocketChannel::Peer::didClose(unsigned long unhandledBufferedAmount, ClosingHandshakeCompletionStatus closingHandshakeCompletion, unsigned short code, const String& reason)
{
    ASSERT(isMainThread());
    m_mainWebSocketChannel = 0;
    m_loaderProxy.postTaskForModeToWorkerContext(createCallbackTask(&workerContextDidClose, m_workerClientWrapper, unhandledBufferedAmount, closingHandshakeCompletion, code, reason), m_taskMode);
}

WorkerThreadableWebSocketChannel::Bridge::Bridge(PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper, PassRefPtr<WorkerContext> workerContext, const String& taskMode)
    : m_workerClientWrapper(workerClientWrapper)
    , m_workerContext(workerContext)
    , m_loaderProxy(m_workerContext->thread()->workerLoaderProxy())
    , m_taskMode(taskMode)
    , m_peer(0)
{
    ASSERT(m_workerClientWrapper.get());
}
//SOMC KK PATCH Patches_w15.5_end

WorkerThreadableWebSocketChannel::Bridge::~Bridge()
{
    disconnect();
}

//SOMC KK PATCH Patches_w15.5_begin
class WorkerContextDidInitializeTask : public ScriptExecutionContext::Task {
public:
    static PassOwnPtr<ScriptExecutionContext::Task> create(WorkerThreadableWebSocketChannel::Peer* peer,
                                                           PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper,
                                                           bool useHixie76Protocol)
    {
        return adoptPtr(new WorkerContextDidInitializeTask(peer, workerClientWrapper, useHixie76Protocol));
    }

    virtual ~WorkerContextDidInitializeTask() { }
    virtual void performTask(ScriptExecutionContext* context)
    {
        ASSERT_UNUSED(context, context->isWorkerContext());
        m_workerClientWrapper->didCreateWebSocketChannel(m_peer, m_useHixie76Protocol);
    }
    virtual bool isCleanupTask() const { return true; }

private:
    WorkerContextDidInitializeTask(WorkerThreadableWebSocketChannel::Peer* peer,
                                   PassRefPtr<ThreadableWebSocketChannelClientWrapper> workerClientWrapper,
                                   bool useHixie76Protocol)
        : m_peer(peer)
        , m_workerClientWrapper(workerClientWrapper)
        , m_useHixie76Protocol(useHixie76Protocol)
    {
    }

    WorkerThreadableWebSocketChannel::Peer* m_peer;
    RefPtr<ThreadableWebSocketChannelClientWrapper> m_workerClientWrapper;
    bool m_useHixie76Protocol;
};

void WorkerThreadableWebSocketChannel::Bridge::mainThreadInitialize(ScriptExecutionContext* context, WorkerLoaderProxy* loaderProxy, PassRefPtr<ThreadableWebSocketChannelClientWrapper> prpClientWrapper, const String& taskMode)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());

    RefPtr<ThreadableWebSocketChannelClientWrapper> clientWrapper = prpClientWrapper;

    Peer* peer = Peer::create(clientWrapper, *loaderProxy, context, taskMode);
    bool sent = loaderProxy->postTaskForModeToWorkerContext(
        WorkerContextDidInitializeTask::create(peer, clientWrapper, peer->useHixie76Protocol()), taskMode);
    if (!sent) {
        clientWrapper->clearPeer();
        delete peer;
    }
}

void WorkerThreadableWebSocketChannel::Bridge::initialize()
{
    ASSERT(!m_peer);
    setMethodNotCompleted();
    RefPtr<Bridge> protect(this);
    m_loaderProxy.postTaskToLoader(
        createCallbackTask(&Bridge::mainThreadInitialize,
                           AllowCrossThreadAccess(&m_loaderProxy), m_workerClientWrapper, m_taskMode));
    waitForMethodCompletion();
    // m_peer may be null when the nested runloop exited before a peer has created.
    m_peer = m_workerClientWrapper->peer();
}
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
//void WorkerThreadableWebSocketChannel::mainThreadConnect(ScriptExecutionContext* context, Peer* peer)
void WorkerThreadableWebSocketChannel::mainThreadConnect(ScriptExecutionContext* context, Peer* peer, const KURL& url, const String& protocol)
//SOMC KK PATCH Patches_w15.5_end
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());
    ASSERT(peer);

//SOMC KK PATCH Patches_w15.5_begin
//    peer->connect();
    peer->connect(url, protocol);
//SOMC KK PATCH Patches_w15.5_end
}

//SOMC KK PATCH Patches_w15.5_begin
//void WorkerThreadableWebSocketChannel::Bridge::connect()
void WorkerThreadableWebSocketChannel::Bridge::connect(const KURL& url, const String& protocol)
//SOMC KK PATCH Patches_w15.5_end
{
    ASSERT(m_workerClientWrapper);
//SOMC KK PATCH Patches_w15.5_begin
//    ASSERT(m_peer);
//    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadConnect, m_peer));
    if (!m_peer)
        return;
    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadConnect, AllowCrossThreadAccess(m_peer), url, protocol));
//SOMC KK PATCH Patches_w15.5_end
}

void WorkerThreadableWebSocketChannel::mainThreadSend(ScriptExecutionContext* context, Peer* peer, const String& message)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());
    ASSERT(peer);

    peer->send(message);
}

//SOMC KK PATCH Patches_w15.5_begin
void WorkerThreadableWebSocketChannel::mainThreadSendArrayBuffer(ScriptExecutionContext* context, Peer* peer, PassOwnPtr<Vector<char> > data)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());
    ASSERT(peer);

    RefPtr<ArrayBuffer> arrayBuffer = ArrayBuffer::create(data->data(), data->size());
    peer->send(*arrayBuffer);
}

void WorkerThreadableWebSocketChannel::mainThreadSendBlob(ScriptExecutionContext* context, Peer* peer, const KURL& url, const String& type, long long size)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());
    ASSERT(peer);

    RefPtr<Blob> blob = Blob::create(url, type, size);
    peer->send(*blob);
}
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
/*
bool WorkerThreadableWebSocketChannel::Bridge::send(const String& message)
{
    if (!m_workerClientWrapper)
        return false;
    ASSERT(m_peer);
    setMethodNotCompleted();
    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadSend, m_peer, message));
    RefPtr<Bridge> protect(this);
    waitForMethodCompletion();
    ThreadableWebSocketChannelClientWrapper* clientWrapper = m_workerClientWrapper.get();
    return clientWrapper && clientWrapper->sent();
}
*/
ThreadableWebSocketChannel::SendResult WorkerThreadableWebSocketChannel::Bridge::send(const String& message)
{
    if (!m_workerClientWrapper || !m_peer)
        return ThreadableWebSocketChannel::SendFail;
    setMethodNotCompleted();
    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadSend, AllowCrossThreadAccess(m_peer), message));
    RefPtr<Bridge> protect(this);
    waitForMethodCompletion();
    ThreadableWebSocketChannelClientWrapper* clientWrapper = m_workerClientWrapper.get();
    if (!clientWrapper)
        return ThreadableWebSocketChannel::SendFail;
    return clientWrapper->sendRequestResult();
}

ThreadableWebSocketChannel::SendResult WorkerThreadableWebSocketChannel::Bridge::send(const ArrayBuffer& binaryData)
{
    if (!m_workerClientWrapper || !m_peer)
        return ThreadableWebSocketChannel::SendFail;
    // ArrayBuffer isn't thread-safe, hence the content of ArrayBuffer is copied into Vector<char>.
    OwnPtr<Vector<char> > data = adoptPtr(new Vector<char>(binaryData.byteLength()));
    if (binaryData.byteLength())
        memcpy(data->data(), binaryData.data(), binaryData.byteLength());
    setMethodNotCompleted();
    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadSendArrayBuffer, AllowCrossThreadAccess(m_peer), data.release()));
    RefPtr<Bridge> protect(this);
    waitForMethodCompletion();
    ThreadableWebSocketChannelClientWrapper* clientWrapper = m_workerClientWrapper.get();
    if (!clientWrapper)
        return ThreadableWebSocketChannel::SendFail;
    return clientWrapper->sendRequestResult();
}

ThreadableWebSocketChannel::SendResult WorkerThreadableWebSocketChannel::Bridge::send(const Blob& binaryData)
{
    if (!m_workerClientWrapper || !m_peer)
        return ThreadableWebSocketChannel::SendFail;
    setMethodNotCompleted();
    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadSendBlob, AllowCrossThreadAccess(m_peer), binaryData.url(), binaryData.type(), binaryData.size()));
    RefPtr<Bridge> protect(this);
    waitForMethodCompletion();
    ThreadableWebSocketChannelClientWrapper* clientWrapper = m_workerClientWrapper.get();
    if (!clientWrapper)
        return ThreadableWebSocketChannel::SendFail;
    return clientWrapper->sendRequestResult();
}
//SOMC KK PATCH Patches_w15.5_end

void WorkerThreadableWebSocketChannel::mainThreadBufferedAmount(ScriptExecutionContext* context, Peer* peer)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());
    ASSERT(peer);

    peer->bufferedAmount();
}

unsigned long WorkerThreadableWebSocketChannel::Bridge::bufferedAmount()
{
//SOMC KK PATCH Patches_w15.5_begin
//    if (!m_workerClientWrapper)
//        return 0;
//    ASSERT(m_peer);
    if (!m_workerClientWrapper || !m_peer)
        return 0;
//SOMC KK PATCH Patches_w15.5_end
    setMethodNotCompleted();
//SOMC KK PATCH Patches_w15.5_begin
//    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadBufferedAmount, m_peer));
    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadBufferedAmount, AllowCrossThreadAccess(m_peer)));
//SOMC KK PATCH Patches_w15.5_end
    RefPtr<Bridge> protect(this);
    waitForMethodCompletion();
    ThreadableWebSocketChannelClientWrapper* clientWrapper = m_workerClientWrapper.get();
    if (clientWrapper)
        return clientWrapper->bufferedAmount();
    return 0;
}

//SOMC KK PATCH Patches_w15.5_begin
/*
void WorkerThreadableWebSocketChannel::mainThreadClose(ScriptExecutionContext* context, Peer* peer)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());
    ASSERT(peer);

    peer->close();
}

void WorkerThreadableWebSocketChannel::Bridge::close()
{
    ASSERT(m_peer);
    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadClose, m_peer));
}

void WorkerThreadableWebSocketChannel::mainThreadDestroy(ScriptExecutionContext* context, Peer* peer)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());
    ASSERT(peer);

    delete peer;
}
*/
void WorkerThreadableWebSocketChannel::mainThreadClose(ScriptExecutionContext* context, Peer* peer, int code, const String& reason)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());
    ASSERT(peer);

    peer->close(code, reason);
}

void WorkerThreadableWebSocketChannel::Bridge::close(int code, const String& reason)
{
    if (!m_peer)
        return;
    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadClose, AllowCrossThreadAccess(m_peer), code, reason));
}

void WorkerThreadableWebSocketChannel::mainThreadFail(ScriptExecutionContext* context, Peer* peer, const String& reason)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());
    ASSERT(peer);

    peer->fail(reason);
}

void WorkerThreadableWebSocketChannel::Bridge::fail(const String& reason)
{
    if (!m_peer)
        return;
    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadFail, AllowCrossThreadAccess(m_peer), reason));
}
//SOMC KK PATCH Patches_w15.5_end

//SOMC KK PATCH Patches_w15.5_begin
void WorkerThreadableWebSocketChannel::mainThreadDestroy(ScriptExecutionContext* context, PassOwnPtr<Peer> peer)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());
    ASSERT_UNUSED(peer, peer);

    // Peer object will be deleted even if the task does not run in the main thread's cleanup period, because
    // the destructor for the task object (created by createCallbackTask()) will automatically delete the peer.
}
//SOMC KK PATCH Patches_w15.5_end
void WorkerThreadableWebSocketChannel::Bridge::disconnect()
{
    clearClientWrapper();
    if (m_peer) {
//SOMC KK PATCH Patches_w15.5_begin
//        Peer* peer = m_peer;
//        m_peer = 0;
//        m_loaderProxy.postTaskToLoader(createCallbackTask(&mainThreadDestroy, peer));
        OwnPtr<Peer> peer = adoptPtr(m_peer);
        m_peer = 0;
        m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadDestroy, peer.release()));
//SOMC KK PATCH Patches_w15.5_end
    }
    m_workerContext = 0;
}

void WorkerThreadableWebSocketChannel::mainThreadSuspend(ScriptExecutionContext* context, Peer* peer)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());
    ASSERT(peer);

    peer->suspend();
}

void WorkerThreadableWebSocketChannel::Bridge::suspend()
{
//SOMC KK PATCH Patches_w15.5_begin
//    ASSERT(m_peer);
//    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadSuspend, m_peer));
    if (!m_peer)
        return;
    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadSuspend, AllowCrossThreadAccess(m_peer)));
//SOMC KK PATCH Patches_w15.5_end
}

void WorkerThreadableWebSocketChannel::mainThreadResume(ScriptExecutionContext* context, Peer* peer)
{
    ASSERT(isMainThread());
    ASSERT_UNUSED(context, context->isDocument());
    ASSERT(peer);

    peer->resume();
}

void WorkerThreadableWebSocketChannel::Bridge::resume()
{
//SOMC KK PATCH Patches_w15.5_begin
//    ASSERT(m_peer);
//    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadResume, m_peer));
    if (!m_peer)
        return;
    m_loaderProxy.postTaskToLoader(createCallbackTask(&WorkerThreadableWebSocketChannel::mainThreadResume, AllowCrossThreadAccess(m_peer)));
//SOMC KK PATCH Patches_w15.5_end
}

void WorkerThreadableWebSocketChannel::Bridge::clearClientWrapper()
{
    m_workerClientWrapper->clearClient();
}

void WorkerThreadableWebSocketChannel::Bridge::setMethodNotCompleted()
{
    ASSERT(m_workerClientWrapper);
    m_workerClientWrapper->clearSyncMethodDone();
}

// Caller of this function should hold a reference to the bridge, because this function may call WebSocket::didClose() in the end,
// which causes the bridge to get disconnected from the WebSocket and deleted if there is no other reference.
void WorkerThreadableWebSocketChannel::Bridge::waitForMethodCompletion()
{
    if (!m_workerContext)
        return;
    WorkerRunLoop& runLoop = m_workerContext->thread()->runLoop();
    MessageQueueWaitResult result = MessageQueueMessageReceived;
    ThreadableWebSocketChannelClientWrapper* clientWrapper = m_workerClientWrapper.get();
    while (m_workerContext && clientWrapper && !clientWrapper->syncMethodDone() && result != MessageQueueTerminated) {
        result = runLoop.runInMode(m_workerContext.get(), m_taskMode); // May cause this bridge to get disconnected, which makes m_workerContext become null.
        clientWrapper = m_workerClientWrapper.get();
    }
}

} // namespace WebCore

#endif // ENABLE(WEB_SOCKETS)
