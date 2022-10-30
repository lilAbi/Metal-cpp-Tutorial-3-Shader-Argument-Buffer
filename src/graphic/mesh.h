
#ifndef IRONENGINE_MESH_H
#define IRONENGINE_MESH_H
#include "PCH.h"


namespace gfx{

    //buffers that make up a "mesh"
    class Mesh {
    public:
        void Release();
    public:
        int numberOfVertices{0};
        MTL::Buffer* vertexPositionsBuffer{nullptr};
        MTL::Buffer* vertexColorsBuffer{nullptr};
    };
}


#endif //IRONENGINE_MESH_H
