/*!A lightweight and fast 2D vector graphics engine
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2021-present, Lanox2D Open Source Group.
 *
 * @author      ruki
 * @file        RenderPipeline.h
 *
 */

#import "prefix.h"

// for all pipelines
#define kVerticesIndex          0
#define kMatrixProjectionIndex  1
#define kMatrixModelIndex       2

// only for solid pipeline
#define kVertexColorIndex       3

// only for texture pipeline
#define kTexcoordsIndex         3
#define kMatrixTexcoordIndex    4

#define kColorTextureIndex      0
#define kTextureSamplerIndex    1

@interface RenderPipeline : NSObject

- (id)initWithView:(MTKView*)view;

- (id<MTLRenderPipelineState>)renderPipelineWithName:(NSString*)name shaderSource:(NSString*)source;
- (id<MTLRenderPipelineState>)renderPipelineSolid;
- (id<MTLRenderPipelineState>)renderPipelineTexture;

@end


