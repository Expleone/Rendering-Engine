//
// Created by expleoene on 4/6/26.
//
// MANUALLY GENERATED to match jsmodel2c format

#include "cube.h"

static const float cube_vertices[] = {
    // Front face
    -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,
    // Back face
    -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
    // Left face
    -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,
    // Right face
     1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,
    // Top face
    -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,
    // Bottom face
    -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f
};

static const float cube_normals[] = {
    // Front
    0.0f,  0.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f,  0.0f,  1.0f,
   // Back
    0.0f,  0.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f,  0.0f, -1.0f,
   // Left
   -1.0f,  0.0f,  0.0f,  -1.0f,  0.0f,  0.0f,  -1.0f,  0.0f,  0.0f,  -1.0f,  0.0f,  0.0f,
   // Right
    1.0f,  0.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f,  0.0f,  0.0f,
   // Top
    0.0f,  1.0f,  0.0f,   0.0f,  1.0f,  0.0f,   0.0f,  1.0f,  0.0f,   0.0f,  1.0f,  0.0f,
   // Bottom
    0.0f, -1.0f,  0.0f,   0.0f, -1.0f,  0.0f,   0.0f, -1.0f,  0.0f,   0.0f, -1.0f,  0.0f
};

static const unsigned short cube_faces[] = {
    0,  1,  2,      0,  2,  3,    // Front
    4,  5,  6,      4,  6,  7,    // Back
    8,  9, 10,      8, 10, 11,    // Left
   12, 13, 14,     12, 14, 15,    // Right
   16, 17, 18,     16, 18, 19,    // Top
   20, 21, 22,     20, 22, 23     // Bottom
};

const struct cube_data_t cube_data = {
    12,                     // nFaces (12 triangles)
    cube_faces,             // pointer to index array
    24,                      // nVertices (8 corners)
    cube_vertices,          // pointer to vertex array
    cube_normals,           // pointer to normal array
    {0.8f, 0.8f, 0.8f},     // base color
    0                       // extra padding/flags variable
  };