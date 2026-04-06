#ifndef CUBE_H
#define CUBE_H

#ifdef __cplusplus
extern "C" {
#endif

// Define the struct layout so C++ knows how to read it
struct cube_data_t {
    int nFaces;
    const unsigned short* faces;
    int nVertices;
    const float* vertices;
    const float* normals;
    float color[3];
    int flags; // The '11' from your birds file
};

// Expose the data
extern const struct cube_data_t cube_data;

#ifdef __cplusplus
}
#endif

#endif // CUBE_H