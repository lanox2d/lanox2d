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
* @file        Lanox2dMetalRenderer.m
*
*/

#import "Lanox2dMetalRenderer.h"

@implementation Lanox2dMetalRenderer {
    id<MTLDevice> _device;
}

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView {
    self = [super init];
    if(self) {
        _device = mtkView.device;
    }
    return self;
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
    NSLog(@"drawableSizeWillChange");
}

- (void)drawInMTKView:(nonnull MTKView *)view {
    NSLog(@"drawInMTKView");
}

@end
