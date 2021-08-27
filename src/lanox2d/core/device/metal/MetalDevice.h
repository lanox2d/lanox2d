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
 * @file        MetalDevice.h
 *
 */

#import "prefix.h"

@interface MetalDevice : NSObject

- (nonnull instancetype)initWithView:(nonnull MTKView*)mtkView;

- (lx_void_t)drawLock;
- (lx_void_t)drawCommit;
- (lx_void_t)drawClear:(lx_color_t)color;
- (lx_void_t)drawLines:(nonnull lx_point_ref_t)points count:(lx_size_t)count bounds:(nullable lx_rect_ref_t)bounds;
- (lx_void_t)drawPoints:(nonnull lx_point_ref_t)points count:(lx_size_t)count bounds:(nullable lx_rect_ref_t)bounds;
- (lx_void_t)drawPolygon:(nonnull lx_polygon_ref_t)polygon hint:(nullable lx_shape_ref_t)hint bounds:(nullable lx_rect_ref_t)bounds;
- (lx_void_t)drawPath:(nonnull lx_path_ref_t)path;

@end


