
#ifndef IRONENGINE_RENDERPASS_H
#define IRONENGINE_RENDERPASS_H
#include "mesh.h"



namespace gfx{

    //contains all the info for a render pass
    class RenderPass {
    public:
        //An object that contains graphics functions and configuration state to use in a render command
        MTL::RenderPipelineState* renderPipelineState{nullptr};
        gfx::Mesh mesh;
    };

}


#endif //IRONENGINE_RENDERPASS_H
