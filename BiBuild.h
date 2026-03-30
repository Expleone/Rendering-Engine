//
// Created by expleoene on 3/26/26.
//

#ifndef VIEWER_BIBUILD_H
#define VIEWER_BIBUILD_H
#include "core/InputHandler.h"
#include  "./core/SceneManager.h"
#include <random>
#include <uuid.h>

inline uuids::uuid genUUID() {
    std::mt19937 engine{std::random_device{}()};
    uuids::uuid_random_generator gen{engine};
    uuids::uuid const id = gen();
    return id;
}


#endif //VIEWER_BIBUILD_H