
#include "renderer.h"

gfx::Renderer::Renderer(MTL::Device* device) : device{device->retain()}{
    commandQueue = device->newCommandQueue();
    BuildShaders();
    BuildBuffers();
}


gfx::Renderer::~Renderer() {
    commandQueue->release();
    device->release();
    renderPass.renderPipelineState->release();
    renderPass.mesh.Release();
    argBuffer->release();
    shaderLibrary->release();
}

void gfx::Renderer::Draw(MTK::View *view) {
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    //get the current command buffer object to encode commands for execution in the GPU
    auto* commandBuffer = commandQueue->commandBuffer();
    //get the current render pass descriptor that will be populated with different render targets and their information
    auto* renderPassDescriptor = view->currentRenderPassDescriptor();
    //encodes the renderPass descriptor into actually commands
    auto* renderCommandEncoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);

    //tell the encoder to set the render pipeline state to this object
    renderCommandEncoder->setRenderPipelineState(renderPass.renderPipelineState);
    gfx::Mesh& mesh = renderPass.mesh;

    //set argument buffer to index 0 in the buffer argument table
    renderCommandEncoder->setVertexBuffer(argBuffer, 0, 0);
    //tell metal that it should be aware about this buffer reference in the vertex stage
    renderCommandEncoder->useResource(mesh.vertexPositionsBuffer, MTL::ResourceUsageRead);
    //tell metal that it should be aware about this buffer reference in the vertex stage
    renderCommandEncoder->useResource(mesh.vertexColorsBuffer, MTL::ResourceUsageRead);

    //invoke a draw command and how many vertices to use
    renderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(mesh.numberOfVertices));

    renderCommandEncoder->endEncoding();
    //tell gpu we got something to draw
    commandBuffer->presentDrawable(view->currentDrawable());
    //this ain't a marriage, commit to the damn draw
    commandBuffer->commit();

    pool->release();

}

void gfx::Renderer::BuildShaders() {
    using NS::StringEncoding::UTF8StringEncoding;

    //load shader file
    std::string shaderFile{std::move(util::ReadFileIntoString("/Users/abi/CLionProjects/ironEngine/src/resources/shaders/shader.metal"))};

    NS::Error* error{nullptr};
    //object containing metal shading language complied source code
    MTL::Library* library = device->newLibrary( NS::String::string(shaderFile.c_str(),UTF8StringEncoding),nullptr, &error);

    if (!library) {
        std::cout << error->localizedDescription()->utf8String() << "\n";
        assert( false );
    }
    //object that represents a shader function in the library
    MTL::Function* vertexFunction = library->newFunction( NS::String::string("vertexMain", UTF8StringEncoding));
    MTL::Function* fragmentFunction = library->newFunction( NS::String::string("fragmentMain", UTF8StringEncoding));

    //object to specify the rendering configuration state
    MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    //in a render pass we use this vertex function
    renderPipelineDescriptor->setVertexFunction( vertexFunction );
    //in a render pass we use this fragment function
    renderPipelineDescriptor->setFragmentFunction( fragmentFunction );
    // format of the pixel buffer
    renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );

    //create a render pipeline state
    renderPass.renderPipelineState = device->newRenderPipelineState( renderPipelineDescriptor, &error );
    if ( !renderPass.renderPipelineState ){
        std::cout << error->localizedDescription()->utf8String()<< "\n";
        assert( false );
    }

    //store the pointer to the newly created shader library
    shaderLibrary = library;

    //release objects no longer needed
    vertexFunction->release();
    fragmentFunction->release();
    renderPipelineDescriptor->release();
}

void gfx::Renderer::BuildBuffers() {
    using NS::StringEncoding::UTF8StringEncoding;

    //number of total vertices
    const int numOfVertices = 6;

    //buffer of vertex position data in a float type
    simd::float3 positions[numOfVertices] = {
            { -0.5f,  0.5f, 0.0f },
            { -0.5f, -0.5f, 0.0f },
            {  0.5f,  0.5f, 0.0f },
            {  0.5f,  0.5f, 0.0f },
            {  0.5f, -0.5f, 0.0f },
            { -0.5f, -0.5f, 0.0f }};

    //buffer of vertex color data in a flat type
    simd::float3 colors[numOfVertices] = {
            {  0.0f, 0.0f, 1.0f },
            {  .5f, 0.0, 0.0f },
            {  0.0f, 0.6f, 0.0f },
            {  0.1f, 0.0f, 0.9f },
            {  0.1f, 0.4, 0.0f },
            {  0.3f, 0.6f, 0.5f }};

    //size of the position data buffer
    int positionsDataSize = numOfVertices * sizeof( simd::float3 );
    //size of the color data buffer
    int colorDataSize = numOfVertices * sizeof( simd::float3 );

    //buffer to store data on the gpu
    MTL::Buffer* vertexPositionBuffer = device->newBuffer( positionsDataSize, MTL::ResourceStorageModeManaged );
    MTL::Buffer* vertexColorBuffer = device->newBuffer( colorDataSize, MTL::ResourceStorageModeManaged );

    //copy data from cpu buffer to gpu buffer
    memcpy(vertexPositionBuffer->contents(), positions, positionsDataSize);
    memcpy(vertexColorBuffer->contents(), colors, colorDataSize);

    //tell metal that we modify the buffer
    vertexPositionBuffer->didModifyRange(NS::Range::Make(0,vertexPositionBuffer->length()));
    vertexColorBuffer->didModifyRange(NS::Range::Make(0,vertexPositionBuffer->length()));

    //update the mesh
    renderPass.mesh = Mesh{numOfVertices, vertexPositionBuffer, vertexColorBuffer};

    //obtain the vertex shader function object
    MTL::Function* vertexFunction = shaderLibrary->newFunction(NS::String::string( "vertexMain", UTF8StringEncoding));
    //encoder object to encode data into an argument buffer for index 0
    MTL::ArgumentEncoder* argumentEncoder = vertexFunction->newArgumentEncoder(0);

    //buffer to store argument data
    argBuffer = device->newBuffer(argumentEncoder->encodedLength(), MTL::ResourceStorageModeManaged);

    //set the buffer that the argument encoder should write to
    argumentEncoder->setArgumentBuffer(argBuffer, 0);
    //encode a reference to the vertex position buffer, in the argument buffer
    argumentEncoder->setBuffer(vertexPositionBuffer, 0, 0);
    //encode a reference to the vertex color buffer, in the argument buffer
    argumentEncoder->setBuffer(vertexColorBuffer, 0, 1);

    //tell metal that buffer has been modified
    argBuffer->didModifyRange(NS::Range::Make(0, argBuffer->length()));

    //release objects no longer needed
    vertexFunction->release();
    argumentEncoder->release();
}
