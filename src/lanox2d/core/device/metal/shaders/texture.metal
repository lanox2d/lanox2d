#include <metal_stdlib>

#define kVerticesIndex          0
#define kMatrixProjectionIndex  1
#define kMatrixModelIndex       2
#define kVertexColorIndex       3

using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float4 color;
};

vertex VertexOut vertexShader(uint vertexID [[vertex_id]],
                              constant float2* vertices [[buffer(kVerticesIndex)]],
                              constant float4x4& matrixProject [[buffer(kMatrixProjectionIndex)]],
                              constant float4x4& matrixModel [[buffer(kMatrixModelIndex)]],
                              constant float4* color [[buffer(kVertexColorIndex)]]) {
    VertexOut out;
    float4 position = float4(0.0, 0.0, 0.0, 1.0);
    position.xy = vertices[vertexID].xy;
    out.position = matrixProject * matrixModel * position;
    out.color = *color;
    return out;
}

fragment float4 fragmentShader(VertexOut vertexIn [[stage_in]]) {
    return vertexIn.color;
}

