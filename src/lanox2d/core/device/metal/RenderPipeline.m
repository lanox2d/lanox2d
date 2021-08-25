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
 * @file        RenderPipeline.m
 *
 */

#import "RenderPipeline.h"

@implementation RenderPipeline {
    MTKView*             _view;
    id<MTLDevice>        _device;
    NSMutableDictionary* _pipelineStates;
}

- (id)initWithView:(MTKView*)view {
    self = [super init];
    if (self) {
        _view = view;
        _device = view.device;
        _pipelineStates = [NSMutableDictionary dictionaryWithCapacity:8];
    }
    return self;
}

- (id<MTLRenderPipelineState>)renderPipelineWithName:(NSString*)name shaderSource:(NSString*)source {
    id<MTLRenderPipelineState> pipelineState = [_pipelineStates valueForKey:name];
    if (pipelineState == nil) {
        // load metal library
        NSError *error;
        id<MTLLibrary> library = [_device newLibraryWithSource:source options:nil error:&error];
        NSAssert(library, @"failed to create metal library: %@ state: %@", name, error);

        if (library != nil) {
            id<MTLFunction> vertexFunction = [library newFunctionWithName:@"vertexShader"];
            id<MTLFunction> fragmentFunction = [library newFunctionWithName:@"fragmentShader"];

            // configure a pipeline descriptor that is used to create a pipeline state.
            MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.label = name;
            pipelineStateDescriptor.vertexFunction = vertexFunction;
            pipelineStateDescriptor.fragmentFunction = fragmentFunction;
            pipelineStateDescriptor.colorAttachments[0].pixelFormat = _view.colorPixelFormat;
            pipelineState = [_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
            NSAssert(pipelineState, @"failed to create pipeline: %@ state: %@", name, error);
            if (pipelineState) {
                [_pipelineStates setObject:pipelineState forKey:name];
            }
        }
    }
    return pipelineState;
}

- (id<MTLRenderPipelineState>)renderPipelineSolid {
    static lx_char_t const solid_metal[] = {
#   include "solid.metal.h"
    };
    return [self renderPipelineWithName:@"Solid Pipeline" shaderSource:[NSString stringWithUTF8String:solid_metal]];
}

- (id<MTLRenderPipelineState>)renderPipelineTexture {
    return nil;
}

@end
