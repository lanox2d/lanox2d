#include <metal_stdlib>

using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float4 color;
};

vertex VertexOut vertexShader(uint vertexID [[vertex_id]],
                              constant float2* vertices [[buffer(0)]],
                              constant float4* color [[buffer(1)]],
                              constant float4x4& matrixProject [[buffer(2)]]) {
    VertexOut out;
    float4 position = float4(0.0, 0.0, 0.0, 1.0);
    position.xy = vertices[vertexID].xy;
    out.position = matrixProject * position;
    out.color = *color;
    return out;
}

fragment float4 fragmentShader(VertexOut vertexIn [[stage_in]]) {
    return vertexIn.color;
}

