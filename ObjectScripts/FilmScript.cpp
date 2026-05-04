//
// Created by expleoene on 5/1/26.
//

#include "FilmScript.h"

#include <cmath>


namespace BiBuild {
    void FilmScript::Update() {
        if (!tex || !videoPlaybackEnabled || !media_player) return;
        uint8_t average_color[4];
        if (context.newFrameReady) {
            context.mutex.lock();

            tex->UpdateTexture(context.pixels, context.width, context.height, GL_RGBA);

            int max_dim = std::max(context.width, context.height);
            int lowest_mip_level = static_cast<int>(std::floor(std::log2(max_dim)));

            glBindTexture(GL_TEXTURE_2D, tex->GetID());
            glGetTexImage(GL_TEXTURE_2D, lowest_mip_level, GL_RGBA, GL_UNSIGNED_BYTE, average_color);
            glBindTexture(GL_TEXTURE_2D, 0);

            context.newFrameReady = false;
            context.mutex.unlock();
        }

        if (owner->hasBeenInteracted) {
            isPaused = !isPaused;
            libvlc_media_player_set_pause(media_player, isPaused);
            owner->hasBeenInteracted = false;
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

} // BiBuild