//
// Created by expleoene on 5/1/26.
//

#ifndef VIEWER_FILMSCRIPT_H
#define VIEWER_FILMSCRIPT_H
#include <vlc/vlc.h>
#include <cstdlib>
#include <mutex>
#include <vector>
#include <iostream>
#include "ObjectScript.h"
#include "../core/Texture.h"
#include "../components/LightComponent.h"
#include "../components/ModelComponent.h"
#include "../core/ResourceManager.h"
#include "../core/SceneObject.h"

namespace holubiho {







    // Context passed to VLC callbacks


    // --- VLC Video Callbacks ---

    // 1. Lock: VLC is about to write a frame. We give it our pixel buffer.
    class VLCManager {
    public:
        static libvlc_instance_t* get() {
            static libvlc_instance_t* instance = []() -> libvlc_instance_t* {
// #ifdef __linux__
//                 setenv("VLC_PLUGIN_PATH", "/usr/lib/x86_64-linux-gnu/vlc/plugins", 1);
// #endif
                // Removed "--quiet" and added "--verbose=2" for debug output
                const char* fallbackArgs[] = {"--verbose=2", "--file-logging", "--logfile=vlc_log.txt", "--no-video-title-show", "--video-filter=transform",
    "--transform-type=vflip"};
                return libvlc_new(6, fallbackArgs);
            }();
            return instance;
        }
    };


    class FilmScript : public ObjectScript {




        struct VideoContext {
            std::mutex mutex;
            void* pixels = nullptr;
            bool newFrameReady = false;
            int width = 1920;
            int height = 1080;
        };

        VideoContext context{};
        libvlc_media_player_t* media_player = nullptr;
        Texture* tex = nullptr;
        bool videoPlaybackEnabled = false;
        bool isPaused = true;
        libvlc_state_t status;
        glm::vec3 averageColor{0, 0, 0};





    public:
        typedef enum Status
        {
            FilmScript_NothingSpecial=0,
            FilmScript_Opening,
            FilmScript_Buffering, /* XXX: Deprecated value. Check the
                               * libvlc_MediaPlayerBuffering event to know the
                               * buffering state of a libvlc_media_player */
            Filmscript_Playing,
            FilmScript_Paused,
            FilmScript_Stopped,
            FilmScript_Ended,
            FilmScript_Error
        } FilmStatus;
        FilmScript(SceneObject* owner) : ObjectScript(owner) {
            libvlc_instance_t* vlc_instance = VLCManager::get();
            if (!vlc_instance) {
                return;
            }

            media_player = libvlc_media_player_new(vlc_instance);
            if (!media_player) {
                return;
            }

            context.pixels = malloc(context.width * context.height * 4);

            libvlc_video_set_format(media_player, "RGBA", context.width, context.height, context.width * 4);

            libvlc_video_set_callbacks(
                media_player,
                lock_callback,
                unlock_callback,
                display_callback,
                &context
            );
            tex = ResourceManager::CreateTexture("videoTexture", TexType::Tex2D, context.width, context.height, true, GL_RGBA);
            videoPlaybackEnabled = true;

        }

        ~FilmScript() override {
            if (media_player) {
                libvlc_media_player_stop(media_player);
                libvlc_media_player_release(media_player);
            }
            if (context.pixels) {
                free(context.pixels);
            }
        }

        void Update() override;
        void SetVideo(const char* filepath);
        FilmStatus PlayerStatus();
        void Pause();
        void Play();
        glm::vec3 GetAverageColor() const {
            return averageColor;
        }

        Texture* GetTexture();
    private:
        static void* lock_callback(void* opaque, void** planes) {
            auto* ctx = static_cast<VideoContext*>(opaque);
            ctx->mutex.lock();
            *planes = ctx->pixels;
            return nullptr;
        }

        static void unlock_callback(void* opaque, void* picture, void* const* planes) {
            auto* ctx = static_cast<VideoContext*>(opaque);
            ctx->newFrameReady = true;
            ctx->mutex.unlock();
        }

        static void display_callback(void* opaque, void* picture) {
            // Handled in render loop
        }
    };

} // BiBuild

#endif //VIEWER_FILMSCRIPT_H
