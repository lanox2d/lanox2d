#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VertexIn {
    vector_float2 position;
    vector_float4 color;
};

struct VertexOut {
    float4 position [[position]];
    float4 color;
};

vertex VertexOut vertexShader(uint vertexID [[vertex_id]],
                              constant VertexIn *vertices [[buffer(0)]],
                              constant vector_uint2 *viewportSizePointer [[buffer(1)]]) {
    VertexOut out;
    float2 pixelSpacePosition = vertices[vertexID].position.xy;
    vector_float2 viewportSize = vector_float2(*viewportSizePointer);
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.xy = pixelSpacePosition / (viewportSize / 2.0);
    out.color = vertices[vertexID].color;
    return out;
}

fragment float4 fragmentShader(VertexOut vertexIn [[stage_in]]) {
    return vertexIn.color;
}

