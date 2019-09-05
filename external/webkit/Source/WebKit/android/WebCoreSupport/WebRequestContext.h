/*
 * Copyright 2010, The Android Open Source Project
 * Copyright (C) 2012 Sony Ericsson Mobile Communications AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * NOTE: This file has been modified by Sony Ericsson Mobile Communications AB.
 * Modifications are licensed under the License.
 */

#ifndef WebRequestContext_h
#define WebRequestContext_h

#include "ChromiumIncludes.h"
#include "PlatformString.h"

#include <wtf/ThreadingPrimitives.h>

namespace android {

// This class is generally not threadsafe.
class WebRequestContext : public net::URLRequestContext {
public:
    // URLRequestContext overrides.
    virtual const std::string& GetUserAgent(const GURL&) const;
//SOMC KK PATCH Patches_w12.5_begin    
    virtual const std::string& GetUserAgentProfile() const;
//SOMC KK PATCH Patches_w12.5_end
    virtual const std::string& accept_language() const;

    WebRequestContext(bool isPrivateBrowsing);

    // These methods are threadsafe.
    void setUserAgent(const WTF::String&);
   //SOMC KK PATCH Patches_w12.5_begin
    void setUserAgentProfile(const WTF::String&);
   //SOMC KK PATCH Patches_w12.5_end
    void setCacheMode(int);
    int getCacheMode();
    static void setAcceptLanguage(const WTF::String&);
    static const WTF::String& acceptLanguage();

private:
    WebRequestContext();
    ~WebRequestContext();

    std::string m_userAgent;
    int m_cacheMode;
    mutable WTF::Mutex m_userAgentMutex;
    bool m_isPrivateBrowsing;
   //SOMC KK PATCH Patches_w12.5_begin   
    std::string m_userAgentProfile;
    mutable WTF::Mutex m_userAgentProfileMutex;
  //SOMC KK PATCH Patches_w12.5_end
};

} // namespace android

#endif
