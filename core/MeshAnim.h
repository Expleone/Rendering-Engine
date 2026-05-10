//
// Created by expleoene on 4/2/26.
//

#ifndef VIEWER_MESHANIM_H
#define VIEWER_MESHANIM_H
#include "Mesh.h"

namespace holubiho {
    class MeshAnim : public Mesh {
        int currentFrame = 0; // How many frames into the animation we are
        int verticesPerFrame = 0; // How many vertices are in each frame of the animation
    public:    MeshAnim(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices = {}, const std::string& name = "MeshAnim") : Mesh(vertices, indices, name) {}

    };
}



#endif //VIEWER_MESHANIM_H
