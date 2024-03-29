#include <metal_stdlib>

#define kVerticesIndex          0
#define kMatrixProjectionIndex  1
#define kMatrixModelIndex       2
#define kTexcoordsIndex         3
#define kMatrixTexcoordIndex    4

#define kColorTextureIndex      0
#define kTextureSamplerIndex    1

using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float2 texcoords;
};

vertex VertexOut vertexShader(uint vertexID [[vertex_id]],
                              constant float2* vertices [[buffer(kVerticesIndex)]],
                              constant float4x4& matrixProject [[buffer(kMatrixProjectionIndex)]],
                              constant float4x4& matrixModel [[buffer(kMatrixModelIndex)]],
                              constant float2* textureCoords [[buffer(kTexcoordsIndex)]],
                              constant float4x4& matrixTexcoord [[buffer(kMatrixTexcoordIndex)]]) {
    VertexOut out;
    float4 position = float4(0.0, 0.0, 0.0, 1.0);
    float4 texcoords = float4(0.0, 0.0, 0.0, 1.0);
    position.xy = vertices[vertexID].xy;
    texcoords.xy = textureCoords[vertexID].xy;
    out.position = matrixProject * matrixModel * position;
    out.texcoords = (matrixTexcoord * texcoords).xy;
    return out;
}

fragment float4 fragmentShader(VertexOut vertexIn [[stage_in]],
                               texture2d<half> colorTexture [[texture(kColorTextureIndex)]],
                               sampler textureSampler [[sampler(kTextureSamplerIndex)]]) {
    //constexpr sampler textureSampler (mag_filter::linear, min_filter::linear);
    const half4 colorSample = colorTexture.sample(textureSampler, vertexIn.texcoords);
    return float4(colorSample);
}

