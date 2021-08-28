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
 * @file        MetalRenderer.m
 *
 */

#import "matrix.h"
#import "MetalRenderer.h"
#import "RenderPipeline.h"
#include "../../tess/tess.h"

@implementation MetalRenderer {
    MTKView*                    _view;
    id<MTLDevice>               _device;
    id<MTLCommandQueue>         _commandQueue;
    id<MTLCommandBuffer>        _commandBuffer;
    id<MTLRenderCommandEncoder> _renderEncoder;
    RenderPipeline*             _renderPipeline;
    MTLClearColor               _clearColor;
    lx_device_t*                _baseDevice;
    lx_stroker_ref_t            _stroker;
    lx_tessellator_ref_t        _tessellator;
    lx_metal_matrix_t           _matrixProject;
}

- (nonnull instancetype)initWithView:(nonnull MTKView*)mtkView baseDevice:(nonnull lx_device_t*)baseDevice {
    self = [super init];
    if (self) {

        // init view and device
        _view = mtkView;
        _view.device = MTLCreateSystemDefaultDevice();
        NSAssert(_view.device, @"metal is not supported on this device");
        _device = _view.device;
        _baseDevice = baseDevice;

        // init command queue
        _commandQueue = [_device newCommandQueue];

        // init render pipeline
        _renderPipeline = [[RenderPipeline alloc] initWithView:mtkView];

        // init clear color
        _clearColor = MTLClearColorMake(0, 0, 0, 1);

        // init projection matrix
        [self updateMatrixProjection];

        // init stroker
        _stroker = lx_stroker_init();
        lx_assert(_stroker);

        // init tessellator
        _tessellator = lx_tessellator_init();
        lx_assert(_tessellator);

        // init tessellator mode
        lx_tessellator_mode_set(_tessellator, LX_TESSELLATOR_MODE_TRIANGULATION);
    }
    return self;
}

- (void)dealloc {
    if (_stroker) {
        lx_stroker_exit(_stroker);
        _stroker = lx_null;
    }
    if (_tessellator) {
        lx_tessellator_exit(_tessellator);
        _tessellator = lx_null;
    }
}

- (lx_void_t)updateMatrixProjection {
    /* metal (origin)
     *          y
     *         /|\
     *          |
     *          |
     * ---------O--------> x
     *          |
     *          |
     *          |
     *
     * to (world)
     *
     *  O----------> x
     *  |
     *  |
     * \|/
     *  y
     *
     */
    CGSize frameSize = [self frameSize];
    lx_metal_matrix_orthof(&_matrixProject, 0.0f, frameSize.width, frameSize.height, 0.0f, -1.0f, 1.0f);
}

- (CGSize)frameSize {
    CGSize frameSize = _view.drawableSize;
    return frameSize;
}

- (lx_void_t)drawLock {
    _commandBuffer = nil;
}

- (lx_void_t)drawPrepare {
    if (_commandBuffer == nil) {
        _commandBuffer = [_commandQueue commandBuffer];
        _commandBuffer.label = @"Lanox2dCommand";
        MTLRenderPassDescriptor* renderPassDescriptor = _view.currentRenderPassDescriptor;
        if (renderPassDescriptor != nil) {

            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].clearColor = _clearColor;
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

            _renderEncoder = [_commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
            _renderEncoder.label = @"Lanox2dRenderEncoder";
        }
    }
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

- (lx_void_t)drawClear:(lx_color_t)color {
    _clearColor = MTLClearColorMake((lx_float_t)color.r / 0xff, (lx_float_t)color.g / 0xff, (lx_float_t)color.b / 0xff, (lx_float_t)color.a / 0xff);
    [self drawPrepare];
}

- (lx_void_t)drawLines:(nonnull lx_point_ref_t)points count:(lx_size_t)count bounds:(nullable lx_rect_ref_t)bounds {
    lx_assert(_baseDevice && _baseDevice->paint && points && count);
    [self drawPrepare];

    lx_check_return(lx_paint_mode(_baseDevice->paint) & LX_PAINT_MODE_STROKE);
    lx_check_return((lx_paint_stroke_width(_baseDevice->paint) > 0));

    [self applyPaint:bounds];
    if ([self strokeOnly]) {
        [self strokeLines:points count:count];
    } else {
        [self strokeFill:lx_stroker_make_from_lines(_stroker, _baseDevice->paint, points, count)];
    }
}

- (lx_void_t)drawPoints:(nonnull lx_point_ref_t)points count:(lx_size_t)count bounds:(nullable lx_rect_ref_t)bounds {
    lx_assert(_baseDevice && _baseDevice->paint && points && count);
    [self drawPrepare];

    lx_check_return(lx_paint_mode(_baseDevice->paint) & LX_PAINT_MODE_STROKE);
    lx_check_return((lx_paint_stroke_width(_baseDevice->paint) > 0));

    [self applyPaint:bounds];
    if ([self strokeOnly]) {
        [self strokePoints:points count:count];
    } else {
        [self strokeFill:lx_stroker_make_from_lines(_stroker, _baseDevice->paint, points, count)];
    }
}

- (lx_void_t)drawPolygon:(nonnull lx_polygon_ref_t)polygon hint:(nullable lx_shape_ref_t)hint bounds:(nullable lx_rect_ref_t)bounds {
    lx_assert(_baseDevice && _baseDevice->paint && polygon && polygon->points && polygon->counts);

    [self drawPrepare];

    if (hint && hint->type == LX_SHAPE_TYPE_LINE) {
        lx_point_t points[2];
        points[0] = hint->u.line.p0;
        points[1] = hint->u.line.p1;
        [self drawLines:points count:2 bounds:bounds];
        return ;
    } else if (hint && hint->type == LX_SHAPE_TYPE_POINT) {
        [self drawPoints:&hint->u.point count:1 bounds:bounds];
        return ;
    }

    [self applyPaint:bounds];
    lx_size_t mode = lx_paint_mode(_baseDevice->paint);
    if (mode & LX_PAINT_MODE_FILL) {
        [self fillPolygon:polygon bounds:bounds rule:lx_paint_fill_rule(_baseDevice->paint)];
    }

    if ((mode & LX_PAINT_MODE_STROKE) && (lx_paint_stroke_width(_baseDevice->paint) > 0)) {
        if ([self strokeOnly]) {
            [self strokePolygon:polygon];
        } else {
            [self strokeFill:lx_stroker_make_from_polygon(_stroker, _baseDevice->paint, polygon, hint)];
        }
    }
}

- (lx_void_t)drawPath:(nonnull lx_path_ref_t)path {
    lx_assert(_baseDevice && _baseDevice->paint && path);
    [self drawPrepare];

    lx_size_t mode = lx_paint_mode(_baseDevice->paint);
    if (mode & LX_PAINT_MODE_FILL) {
        [self drawPolygon:lx_path_polygon(path) hint:lx_path_hint(path) bounds:lx_path_bounds(path)];
    }

    if ((mode & LX_PAINT_MODE_STROKE) && (lx_paint_stroke_width(_baseDevice->paint) > 0)) {
        if ([self strokeOnly]) {
            [self drawPolygon:lx_path_polygon(path) hint:lx_path_hint(path) bounds:lx_path_bounds(path)];
        } else {
            [self strokeFill:lx_stroker_make_from_path(_stroker, _baseDevice->paint, path)];
        }
    }
}

- (lx_void_t)applyPaintShader:(nonnull lx_shader_ref_t)shader bounds:(nullable lx_rect_ref_t)bounds {
}

- (lx_void_t)applyPaintSolid {

    // get paint
    lx_paint_ref_t paint = _baseDevice->paint;
    lx_assert(paint);

    // get color
    lx_color_t color = lx_paint_color(paint);
    lx_byte_t alpha = lx_paint_alpha(paint);
    if (alpha != 0xff) {
        color.a = alpha;
    }

    // apply color
    id<MTLRenderPipelineState> pipelineState = [_renderPipeline renderPipelineSolid];
    [_renderEncoder setRenderPipelineState:pipelineState];

    vector_float4 vertexColor = {(lx_float_t)color.r / 0xff, (lx_float_t)color.g / 0xff, (lx_float_t)color.b / 0xff, (lx_float_t)color.a / 0xff};
    [_renderEncoder setVertexBytes:&vertexColor length:sizeof(vertexColor) atIndex:kVertexColorIndex];
}

- (lx_void_t)applyPaint:(nullable lx_rect_ref_t)bounds {

    // set the region of the drawable to draw into.
    CGSize frameSize = [self frameSize];
    [_renderEncoder setViewport:(MTLViewport){0.0, 0.0, frameSize.width, frameSize.height, 0.0, 1.0}];

    // set projection matrix
    [_renderEncoder setVertexBytes:&_matrixProject length:sizeof(_matrixProject) atIndex:kMatrixProjectionIndex];

    // set model matrix
    lx_metal_matrix_t matrixModel;
    lx_metal_matrix_convert(&matrixModel, _baseDevice->matrix);
    [_renderEncoder setVertexBytes:&matrixModel length:sizeof(matrixModel) atIndex:kMatrixModelIndex];

    // apply paint
    lx_shader_ref_t shader = lx_paint_shader(_baseDevice->paint);
    if (shader) {
        [self applyPaintShader:shader bounds:bounds];
    } else {
        [self applyPaintSolid];
    }
}

- (lx_void_t)fillPolygon:(nonnull lx_polygon_ref_t)polygon bounds:(nullable lx_rect_ref_t)bounds rule:(lx_size_t)rule {
    lx_assert(_tessellator);
    lx_tessellator_rule_set(_tessellator, rule);
    lx_polygon_ref_t result = polygon->convex? polygon : lx_tessellator_make(_tessellator, polygon, bounds);
    if (result) {

        // apply vertices
        lx_assert(result && result->points);
        [_renderEncoder setVertexBytes:result->points length:(result->total * sizeof(lx_point_t)) atIndex:kVerticesIndex];

        // draw vertices
        lx_size_t mode = lx_tessellator_mode(_tessellator);
        if (mode == LX_TESSELLATOR_MODE_TRIANGULATION) {
            lx_size_t index = 0;
            lx_size_t total = result->total;
            while (index + 4 <= total) {
                [_renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:index vertexCount:4];
                index += 4;
            }
        } else {
            lx_uint16_t  count;
            lx_size_t    index = 0;
            lx_uint16_t* counts = result->counts;
            while ((count = *counts++)) {
                [_renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:index vertexCount:count];
                index += count;
            }
        }
    }
}

- (lx_bool_t)strokeOnly {
    lx_assert(_baseDevice && _baseDevice->paint && _baseDevice->matrix);

    // width == 1 and solid? only stroke it
    return (    1.0f == lx_paint_stroke_width(_baseDevice->paint)
            &&  1.0f == lx_abs(_baseDevice->matrix->sx)
            &&  1.0f == lx_abs(_baseDevice->matrix->sy)
            &&  !lx_paint_shader(_baseDevice->paint));
}

- (lx_void_t)strokePoints:(nonnull lx_point_ref_t)points count:(lx_uint16_t)count {
    [_renderEncoder setVertexBytes:points length:(count * sizeof(lx_point_t)) atIndex:kVerticesIndex];
    [_renderEncoder drawPrimitives:MTLPrimitiveTypePoint vertexStart:0 vertexCount:count];
}

- (lx_void_t)strokeLines:(nonnull lx_point_ref_t)points count:(lx_uint16_t)count {
    [_renderEncoder setVertexBytes:points length:(count * sizeof(lx_point_t)) atIndex:kVerticesIndex];
    [_renderEncoder drawPrimitives:MTLPrimitiveTypeLineStrip vertexStart:0 vertexCount:count];
}

- (lx_void_t)strokePolygon:(nonnull lx_polygon_ref_t)polygon {
    lx_assert(polygon && polygon->points && polygon->counts);

    // apply vertices
    [_renderEncoder setVertexBytes:polygon->points length:(polygon->total * sizeof(lx_point_t)) atIndex:kVerticesIndex];

    // draw vertices
    lx_uint16_t  count;
    lx_size_t    index = 0;
    lx_uint16_t* counts = polygon->counts;
    while ((count = *counts++)) {
        [_renderEncoder drawPrimitives:MTLPrimitiveTypeLineStrip vertexStart:index vertexCount:count];
        index += count;
    }
}

- (lx_void_t)strokeFill:(nonnull lx_path_ref_t)path {
    lx_assert(_baseDevice && _baseDevice->paint && path);
    lx_check_return(!lx_path_empty(path));

    // the mode
    lx_size_t mode = lx_paint_mode(_baseDevice->paint);

    // the rule
    lx_size_t rule = lx_paint_fill_rule(_baseDevice->paint);

    // switch to the fill mode
    lx_paint_mode_set(_baseDevice->paint, LX_PAINT_MODE_FILL);

    // switch to the non-zero fill rule
    lx_paint_fill_rule_set(_baseDevice->paint, LX_PAINT_FILL_RULE_NONZERO);

    // draw path
    [self drawPath:path];

    // restore the mode
    lx_paint_mode_set(_baseDevice->paint, mode);

    // restore the fill mode
    lx_paint_fill_rule_set(_baseDevice->paint, rule);
}

@end
