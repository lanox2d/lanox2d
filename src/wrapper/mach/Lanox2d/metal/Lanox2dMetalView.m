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
 * @file        Lanox2dMetalView.m
 *
 */

#import "Lanox2dMetalView.h"
#import "Lanox2dMetalRenderer.h"
#import "lanox2d/lanox2d.h"

@implementation Lanox2dMetalView {
    Lanox2dMetalRenderer* _renderer;
}

- (void)dealloc {
    [self metalExit];
}

- (id)initWithFrame:(CGRect)frame delegate:(id)delegate {
    if (self = [super initWithFrame:frame]) {
        [self setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        [self metalInit:delegate];
    }
    return self;
}

- (void)metalInit:(id)delegate {

    // init window
    _lanox2dWindow = lx_window_init(self.drawableSize.width, self.drawableSize.height, lx_null, (__bridge lx_cpointer_t)self);
    
    // init renderer
    _renderer = [[Lanox2dMetalRenderer alloc] initWithMetalKitView:self delegate:delegate];
    NSAssert(_renderer, @"init renderer failed");

    // init our renderer with the view size
    [_renderer mtkView:self drawableSizeWillChange:self.drawableSize];
    self.delegate = _renderer;
}

- (void)metalExit {

    // exit window
    if (_lanox2dWindow) {
        lx_window_exit(_lanox2dWindow);
        _lanox2dWindow = lx_null;
    }
}

@end
