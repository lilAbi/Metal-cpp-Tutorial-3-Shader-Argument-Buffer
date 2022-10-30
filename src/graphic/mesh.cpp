
#include "mesh.h"

void gfx::Mesh::Release() {
    vertexPositionsBuffer->release();
    vertexColorsBuffer->release();

}
