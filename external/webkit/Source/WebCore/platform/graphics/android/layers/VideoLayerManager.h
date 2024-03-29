/*
 * Copyright 2011 The Android Open Source Project
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
 */

#ifndef VideoLayerManager_h
#define VideoLayerManager_h

#include "GLUtils.h"
#include "IntRect.h"
#include "RenderSkinMediaButton.h"
#include <wtf/HashMap.h>
#include <wtf/Vector.h>
#include <utils/threads.h>

#if USE(ACCELERATED_COMPOSITING)

namespace WebCore {

enum IconState {
    Registered,
    PlayIconShown,
    PauseIconShown
};

enum IconType {
    PlayIcon,
    PauseIcon
};

// Every video layer can use its uniqueId to query VideoLayerManager about such
// info globally.
struct VideoLayerInfo {
    GLuint textureId; // GL texture bound with the surface texture.
    int videoSize; // The size of the video.
    float aspectRatio; // The aspect ratio of the video.
    int timeStamp; // Used to decide which VideoLayerInfo is the oldest one.
    GLfloat surfaceMatrix[16];

    double lastIconShownTime;
    IconState iconState;
};


class VideoLayerManager {

public:
    typedef HashMap<int, VideoLayerInfo*>::const_iterator InfoIterator;

    VideoLayerManager();

    // Register the texture when we got setSurfaceTexture call.
    void registerTexture(const int layerId, const GLuint textureId);
    // Update the size when the video is prepared.
    void updateVideoLayerSize(const int layerId, const int size, const float ratio);
    // At draw time, update the matrix for every video frame update.
    void updateMatrix(const int layerId, const GLfloat* matrix);
    // Remove the layer info from the mapping.
    void removeLayer(const int layerId);

    // Return the texture name corresponding to the layerId
    GLuint getTextureId(const int layerId);
    // Return the matrix for surface texture corresponding to the layerId
    GLfloat* getMatrix(const int layerId);
    // Return the aspect ratio for the video corresponding to the layerId
    float getAspectRatio(const int layerId);

    // Delete the GL textures
    void deleteUnusedTextures();

    double drawIcon(const int layerId, IconType type);

    GLuint getSpinnerInnerTextureId() { return m_spinnerInnerTextureId; }
    GLuint getSpinnerOuterTextureId() { return m_spinnerOuterTextureId; }
    GLuint getPosterTextureId() { return m_posterTextureId; }
    GLuint getPlayTextureId() { return m_playTextureId; }
    GLuint getPauseTextureId() { return m_pauseTextureId; }

    void initGLResourcesIfNeeded();
    void cleanupGLResources();
    void forceNeedsInit() { m_createdTexture = false; }

    static int getButtonSize();
private:
    // Get the sum of all the video size stored in m_videoLayerInfoMap.
    int getTotalMemUsage();
    // If the memory consumption is out of bound, recycle some textures.
    bool recycleTextureMem();
    // The private function to remove layer.
    void removeLayerInternal(const int layerId);
    void initGLResources();
    // Indexed by each layer's uniqueId, this map contains the important info
    // used for showing the video when playing or the screenshot when paused.
    HashMap<int, VideoLayerInfo*> m_videoLayerInfoMap;
    android::Mutex m_videoLayerInfoMapLock;

    // Everytime we add one item to the map, we update the timestamp.
    int m_currentTimeStamp;

    // The retiredTextures is used to communicate between UI thread and webcore
    // thread. Basically, GL textures are determined to retire in the webcore
    // thread, and really get deleted in the UI thread.
    WTF::Vector<GLuint> m_retiredTextures;    //SOMC KK PATCH Patches_w16.5
    android::Mutex m_retiredTexturesLock;

    GLuint createTextureFromImage(RenderSkinMediaButton::MediaButton buttonType);

    // Texture for showing the static image will be created at native side.
    bool m_createdTexture;
    GLuint m_posterTextureId;
    GLuint m_spinnerOuterTextureId;
    GLuint m_spinnerInnerTextureId;
    GLuint m_playTextureId;
    GLuint m_pauseTextureId;

    IntRect m_buttonRect;
};

} // namespace WebCore

#endif // USE(ACCELERATED_COMPOSITING)
       //
#endif // VideoLayerManager_h
