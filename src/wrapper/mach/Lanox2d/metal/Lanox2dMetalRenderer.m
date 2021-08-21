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
#import "Lanox2dMetalView.h"
#import "../Lanox2dViewDelegate.h"
#import "lanox2d/lanox2d.h"

@implementation Lanox2dMetalRenderer {
    id<MTLDevice> _device;
    id <Lanox2dViewDelegate> _delegate;
    CFTimeInterval _basetime;
}

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView delegate:(id)delegate {
    self = [super init];
    if(self) {
        _device = mtkView.device;
        _delegate = delegate;
        _basetime = (CFTimeInterval)lx_mclock();
    }
    return self;
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
    NSLog(@"drawableSizeWillChange");
}

- (void)drawInMTKView:(nonnull MTKView *)view {
    if (_delegate) {
        [_delegate onDrawFrame: (CFTimeInterval)((lx_mclock() - _basetime) / 1000)];
    }
}

@end
