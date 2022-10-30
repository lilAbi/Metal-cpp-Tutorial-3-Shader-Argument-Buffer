#ifndef IRONENGINE_RENDERER_H
#define IRONENGINE_RENDERER_H
#include <simd/simd.h>
#include <cassert>
#include "utility/fileIO.h"
#include "renderPass.h"

namespace gfx{

    class Renderer {
    public:
        explicit Renderer(MTL::Device* device);
        ~Renderer();
        void BuildShaders();
        void BuildBuffers();
        void Draw(MTK::View* view);

    private:
        MTL::Device* device{nullptr};
        MTL::CommandQueue* commandQueue{nullptr};
        //object representing a renderPass
        RenderPass renderPass;
        //argument buffer
        MTL::Library* shaderLibrary{nullptr};
        MTL::Buffer* argBuffer{nullptr};

    };

}


#endif //IRONENGINE_RENDERER_H
