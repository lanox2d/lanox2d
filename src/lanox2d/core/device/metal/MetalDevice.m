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

@implementation MetalDevice {
    MTKView*                    _view;
    id<MTLDevice>               _device;
    id<MTLCommandQueue>         _commandQueue;
    id<MTLCommandBuffer>        _commandBuffer;
    id<MTLRenderCommandEncoder> _renderEncoder;
    id<MTLRenderPipelineState>  _pipelineState;
}

- (nonnull instancetype)initWithView:(nonnull MTKView*)mtkView {
    self = [super init];
    if(self) {
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

#if 1
    // Load all the shader files with a .metal file extension in the project.
    //id<MTLLibrary> defaultLibrary = [_device newDefaultLibrary];

    // https://developer.apple.com/documentation/metal/libraries/building_a_library_with_metal_s_command-line_tools
    NSError *error;
    id<MTLLibrary> defaultLibrary = [_device newLibraryWithFile:@"/tmp/test.metallib" error:&error];


    id<MTLFunction> vertexFunction = [defaultLibrary newFunctionWithName:@"vertexShader"];
    id<MTLFunction> fragmentFunction = [defaultLibrary newFunctionWithName:@"fragmentShader"];

    // Configure a pipeline descriptor that is used to create a pipeline state.
    MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineStateDescriptor.label = @"Simple Pipeline";
    pipelineStateDescriptor.vertexFunction = vertexFunction;
    pipelineStateDescriptor.fragmentFunction = fragmentFunction;
    pipelineStateDescriptor.colorAttachments[0].pixelFormat = mtkView.colorPixelFormat;

    _pipelineState = [_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                             error:&error];

    // Pipeline State creation could fail if the pipeline descriptor isn't set up properly.
    //  If the Metal API validation is enabled, you can find out more information about what
    //  went wrong.  (Metal API validation is enabled by default when a debug build is run
    //  from Xcode.)
    NSAssert(_pipelineState, @"Failed to create pipeline state: %@", error);
#endif
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

#define AAPLVertexInputIndexVertices     0
#define AAPLVertexInputIndexViewportSize 1

    typedef struct
    {
        vector_float2 position;
        vector_float4 color;
    } AAPLVertex;

    static const AAPLVertex triangleVertices[] = {
        // 2D positions,    RGBA colors
        { {  250,  -250 }, { 1, 0, 0, 1 } },
        { { -250,  -250 }, { 0, 1, 0, 1 } },
        { {    0,   250 }, { 0, 0, 1, 1 } },
    };

    // Set the region of the drawable to draw into.
    [_renderEncoder setViewport:(MTLViewport){0.0, 0.0, _view.drawableSize.width, _view.drawableSize.height, 0.0, 1.0 }];

    [_renderEncoder setRenderPipelineState:_pipelineState];

    // Pass in the parameter data.
    [_renderEncoder setVertexBytes:triangleVertices
                           length:sizeof(triangleVertices)
                          atIndex:AAPLVertexInputIndexVertices];

    static vector_uint2 _viewportSize;
    _viewportSize.x = _view.drawableSize.width;
    _viewportSize.y = _view.drawableSize.height;
    [_renderEncoder setVertexBytes:&_viewportSize
                           length:sizeof(_viewportSize)
                          atIndex:AAPLVertexInputIndexViewportSize];

    // Draw the triangle.
    [_renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                      vertexStart:0
                      vertexCount:3];
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
