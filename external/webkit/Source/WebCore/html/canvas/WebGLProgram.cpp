/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"

#if ENABLE(WEBGL)

#include "WebGLProgram.h"

#include "WebGLRenderingContext.h"

namespace WebCore {

PassRefPtr<WebGLProgram> WebGLProgram::create(WebGLRenderingContext* ctx)
{
    return adoptRef(new WebGLProgram(ctx));
}

WebGLProgram::WebGLProgram(WebGLRenderingContext* ctx)
    : WebGLObject(ctx)
    , m_linkStatus(false)
    , m_linkCount(0)
    , m_infoValid(true)    //SOMC KK PATCH Patches_w16.5
{
    setObject(context()->graphicsContext3D()->createProgram());
}

void WebGLProgram::deleteObjectImpl(Platform3DObject obj)
{
    context()->graphicsContext3D()->deleteProgram(obj);
    if (m_vertexShader) {
        m_vertexShader->onDetached();
        m_vertexShader = 0;
    }
    if (m_fragmentShader) {
        m_fragmentShader->onDetached();
        m_fragmentShader = 0;
    }
}

//SOMC KK PATCH Patches_w16.5_begin
/*
bool WebGLProgram::cacheActiveAttribLocations()
{
    m_activeAttribLocations.clear();
    if (!object())
        return false;
    GraphicsContext3D* context3d = context()->graphicsContext3D();

    // Assume link status has already been cached.
    if (!m_linkStatus)
        return false;

    GC3Dint numAttribs = 0;
    context3d->getProgramiv(object(), GraphicsContext3D::ACTIVE_ATTRIBUTES, &numAttribs);
    m_activeAttribLocations.resize(static_cast<size_t>(numAttribs));
    for (int i = 0; i < numAttribs; ++i) {
        ActiveInfo info;
        context3d->getActiveAttrib(object(), i, info);
        m_activeAttribLocations[i] = context3d->getAttribLocation(object(), info.name.charactersWithNullTermination());
    }

    return true;
}
*/
//SOMC KK PATCH Patches_w16.5_end

unsigned WebGLProgram::numActiveAttribLocations()    //SOMC KK PATCH Patches_w16.5
{
    cacheInfoIfNeeded();    //SOMC KK PATCH Patches_w16.5
    return m_activeAttribLocations.size();
}

GC3Dint WebGLProgram::getActiveAttribLocation(GC3Duint index)    //SOMC KK PATCH Patches_w16.5
{
    cacheInfoIfNeeded();    //SOMC KK PATCH Patches_w16.5
    if (index >= numActiveAttribLocations())
        return -1;
    return m_activeAttribLocations[index];
}

bool WebGLProgram::isUsingVertexAttrib0()    //SOMC KK PATCH Patches_w16.5
{
    cacheInfoIfNeeded();    //SOMC KK PATCH Patches_w16.5
    for (unsigned ii = 0; ii < numActiveAttribLocations(); ++ii) {
        if (!getActiveAttribLocation(ii))
            return true;
    }
    return false;
}

//SOMC KK PATCH Patches_w16.5_begin
bool WebGLProgram::getLinkStatus()
{
    cacheInfoIfNeeded();
    return m_linkStatus;
}

void WebGLProgram::setLinkStatus(bool status)
{
    cacheInfoIfNeeded();
    m_linkStatus = status;
}

void WebGLProgram::increaseLinkCount()
{
    ++m_linkCount;
    m_infoValid = false;
}
//SOMC KK PATCH Patches_w16.5_end

WebGLShader* WebGLProgram::getAttachedShader(GC3Denum type)
{
    switch (type) {
    case GraphicsContext3D::VERTEX_SHADER:
        return m_vertexShader.get();
    case GraphicsContext3D::FRAGMENT_SHADER:
        return m_fragmentShader.get();
    default:
        return 0;
    }
}

bool WebGLProgram::attachShader(WebGLShader* shader)
{
    if (!shader || !shader->object())
        return false;
    switch (shader->getType()) {
    case GraphicsContext3D::VERTEX_SHADER:
        if (m_vertexShader)
            return false;
        m_vertexShader = shader;
        return true;
    case GraphicsContext3D::FRAGMENT_SHADER:
        if (m_fragmentShader)
            return false;
        m_fragmentShader = shader;
        return true;
    default:
        return false;
    }
}

bool WebGLProgram::detachShader(WebGLShader* shader)
{
    if (!shader || !shader->object())
        return false;
    switch (shader->getType()) {
    case GraphicsContext3D::VERTEX_SHADER:
        if (m_vertexShader != shader)
            return false;
        m_vertexShader = 0;
        return true;
    case GraphicsContext3D::FRAGMENT_SHADER:
        if (m_fragmentShader != shader)
            return false;
        m_fragmentShader = 0;
        return true;
    default:
        return false;
    }
}

//SOMC KK PATCH Patches_w16.5_begin
void WebGLProgram::cacheActiveAttribLocations(GraphicsContext3D* context3d)
{
    m_activeAttribLocations.clear();

    GC3Dint numAttribs = 0;
    context3d->getProgramiv(object(), GraphicsContext3D::ACTIVE_ATTRIBUTES, &numAttribs);
    m_activeAttribLocations.resize(static_cast<size_t>(numAttribs));
    for (int i = 0; i < numAttribs; ++i) {
        ActiveInfo info;
        context3d->getActiveAttrib(object(), i, info);
        m_activeAttribLocations[i] = context3d->getAttribLocation(object(), info.name.charactersWithNullTermination());
    }
}

void WebGLProgram::cacheInfoIfNeeded()
{
    if (m_infoValid)
        return;

    if (!object())
        return;

    GraphicsContext3D* ctx = context()->graphicsContext3D();
    if (!ctx)
        return;
    GC3Dint linkStatus = 0;
    ctx->getProgramiv(object(), GraphicsContext3D::LINK_STATUS, &linkStatus);
    m_linkStatus = linkStatus;
    if (m_linkStatus)
        cacheActiveAttribLocations(ctx);
    m_infoValid = true;
}
//SOMC KK PATCH Patches_w16.5_end

}

#endif // ENABLE(WEBGL)
