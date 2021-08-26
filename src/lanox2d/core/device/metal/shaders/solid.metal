#include <metal_stdlib>

using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float4 color;
};

vertex VertexOut vertexShader(uint vertexID [[vertex_id]],
                              constant float2* vertices [[buffer(0)]],
                              constant vector_uint2* viewportSizePointer [[buffer(1)]],
                              constant float4* color [[buffer(2)]]) {
    VertexOut out;
    float2 position = vertices[vertexID].xy;
    vector_float2 viewportSize = vector_float2(*viewportSizePointer);
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.xy = position / (viewportSize / 2.0);
    out.color = *color;
    return out;
}

fragment float4 fragmentShader(VertexOut vertexIn [[stage_in]]) {
    return vertexIn.color;
}

