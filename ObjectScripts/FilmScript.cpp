//
// Created by expleoene on 5/1/26.
//

#include "FilmScript.h"

#include <cmath>


namespace BiBuild {
    void FilmScript::Update() {
        if (!tex || !videoPlaybackEnabled || !media_player) return;

        status = libvlc_media_player_get_state(media_player);

        // uint8_t average_color[4] = {0, 0, 0, 255};

        if (context.newFrameReady) {
            context.mutex.lock();

            if (context.pixels && context.width > 0 && context.height > 0) {
                long long r = 0, g = 0, b = 0;
                int num_samples = 0;
                int total_pixels = context.width * context.height;
                uint8_t* px = static_cast<uint8_t*>(context.pixels);

                // Sample every 64th pixel to save CPU cycles
                for (int i = 0; i < total_pixels; i += 64) {
                    r += px[i * 4];     // R
                    g += px[i * 4 + 1]; // G
                    b += px[i * 4 + 2]; // B
                    num_samples++;
                }

                if (num_samples > 0) {
                    averageColor.x= static_cast<uint8_t>(r / num_samples);
                    averageColor.y = static_cast<uint8_t>(g / num_samples);
                    averageColor.z = static_cast<uint8_t>(b / num_samples);
                }
            }

            tex->UpdateTexture(context.pixels, context.width, context.height, GL_RGBA);


            context.newFrameReady = false;
            context.mutex.unlock();

        }
    }

    void FilmScript::SetVideo(const char* filepath) {
        if (!videoPlaybackEnabled || !media_player || !filepath || !*filepath) return;
        libvlc_media_player_stop(media_player);
        isPaused = true;
        libvlc_media_t* new_media = libvlc_media_new_path(VLCManager::get(), filepath);
        if (!new_media) return;
        libvlc_media_player_set_media(media_player, new_media);
        libvlc_media_release(new_media);

        libvlc_media_player_play(media_player);
        libvlc_media_player_set_pause(media_player, 1);
        isPaused = true;
    }

    Texture *FilmScript::GetTexture() {
        return tex;
    }

    FilmScript::FilmStatus FilmScript::PlayerStatus() {
        return static_cast<FilmStatus>(status);
    }

    void FilmScript::Pause() {
        if (videoPlaybackEnabled)
        libvlc_media_player_set_pause(media_player, 1);
    }

    void FilmScript::Play() {
        if (!videoPlaybackEnabled || !media_player) return;

        // 1. Get the absolute latest state
        libvlc_state_t currentState = libvlc_media_player_get_state(media_player);

        if (currentState == libvlc_Ended || currentState == libvlc_Stopped) {
            // Reset and restart
            libvlc_media_player_stop(media_player);
            libvlc_media_player_play(media_player);

            isPaused = false; // Update your internal tracker if you use it

            // 2. RETURN EARLY! Do not call set_pause while libVLC is Opening.
            return;
        }

        // 3. Only call set_pause if we are resuming a normally paused video
        libvlc_media_player_set_pause(media_player, 0);
        isPaused = false;
    }
} // BiBuild