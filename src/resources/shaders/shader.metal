#include <metal_stdlib>
using namespace metal;

struct FragmentOutput{
    float4 position [[position]];
    float4 color;
};

struct VertexData {
    device float3* positions [[id(0)]];
    device float3* colors [[id(1)]];
};

FragmentOutput vertex vertexMain( device const VertexData* vertexData [[buffer(0)]], uint vertexId [[vertex_id]]){
    FragmentOutput out;
    out.position = float4(vertexData->positions[vertexId], 1.0);
    out.color = float4(vertexData->colors[vertexId], 1.0);
    return out;
}

float4 fragment fragmentMain(FragmentOutput in [[stage_in]]){
    return in.color;
}