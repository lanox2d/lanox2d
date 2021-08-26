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
 * @file        MetalDevice.m
 *
 */

#import "MetalDevice.h"
#import "RenderPipeline.h"

@implementation MetalDevice {
    MTKView*                    _view;
    id<MTLDevice>               _device;
    id<MTLCommandQueue>         _commandQueue;
    id<MTLCommandBuffer>        _commandBuffer;
    id<MTLRenderCommandEncoder> _renderEncoder;
    RenderPipeline*             _renderPipeline;
}

- (nonnull instancetype)initWithView:(nonnull MTKView*)mtkView {
    self = [super init];
    if (self) {
        [self initDevice:mtkView];
    }
    return self;
}

- (lx_void_t)initDevice:(nonnull MTKView*)mtkView {

    // init view
    _view = mtkView;

    // init device
    _view.device = MTLCreateSystemDefaultDevice();
    NSAssert(_view.device, @"metal is not supported on this device");
    _device = _view.device;

    // init command queue
    _commandQueue = [_device newCommandQueue];

    // init render pipeline
    _renderPipeline = [[RenderPipeline alloc] initWithView:mtkView];
}

- (lx_bool_t)drawLock {

    // create a new command buffer for each render pass to the current drawable.
    _commandBuffer = [_commandQueue commandBuffer];
    _commandBuffer.label = @"Lanox2dCommand";

    // obtain a renderPassDescriptor generated from the view's drawable textures.
    MTLRenderPassDescriptor* renderPassDescriptor = _view.currentRenderPassDescriptor;
    if (renderPassDescriptor != nil) {

        // create a render command encoder.
        _renderEncoder = [_commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        _renderEncoder.label = @"Lanox2dRenderEncoder";

        [self drawTest];

        // ok
        return lx_true;
    }
    return lx_false;
}

- (lx_void_t)drawCommit {
    if (_renderEncoder) {
        [_renderEncoder endEncoding];
    }
    if (_commandBuffer != nil) {
        [_commandBuffer presentDrawable:_view.currentDrawable];
        [_commandBuffer commit];
    }
}

- (lx_void_t)drawTest {

    const vector_float2 triangleVertices[] = {
        {  250,  -250 },
        { -250,  -250 },
        {    0,   250 },
    };

    // Set the region of the drawable to draw into.
    [_renderEncoder setViewport:(MTLViewport){0.0, 0.0, _view.drawableSize.width, _view.drawableSize.height, 0.0, 1.0 }];

    id<MTLRenderPipelineState> pipelineState = [_renderPipeline renderPipelineSolid];
    [_renderEncoder setRenderPipelineState:pipelineState];

    // Pass in the parameter data.
    [_renderEncoder setVertexBytes:triangleVertices length:sizeof(triangleVertices) atIndex:0];

    vector_uint2 _viewportSize;
    _viewportSize.x = _view.drawableSize.width;
    _viewportSize.y = _view.drawableSize.height;
    [_renderEncoder setVertexBytes:&_viewportSize length:sizeof(_viewportSize) atIndex:1];

    vector_float4 color = {1, 0, 0, 1};
    [_renderEncoder setVertexBytes:&color length:sizeof(color) atIndex:2];

    matrix_float4x4 matrixProject;
    [_renderEncoder setVertexBytes:&matrixProject length:sizeof(matrixProject) atIndex:3];

    // Draw the triangle.
    [_renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
}

- (lx_void_t)drawClear:(lx_color_t)color {
}

- (lx_void_t)drawLines:(nonnull lx_point_ref_t)points count:(lx_size_t)count bounds:(nullable lx_rect_ref_t)bounds {
}

- (lx_void_t)drawPoints:(nonnull lx_point_ref_t)points count:(lx_size_t)count bounds:(nullable lx_rect_ref_t)bounds {
}

- (lx_void_t)drawPolygon:(nonnull lx_polygon_ref_t)polygon hint:(nullable lx_shape_ref_t)hint bounds:(nullable lx_rect_ref_t)bounds {
}

- (lx_void_t)drawPath:(nonnull lx_path_ref_t)path {
}

@end
