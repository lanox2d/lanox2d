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

#ifdef LX_CONFIG_OS_MACOSX
#   define PlatformPanGestureRecognizer NSPanGestureRecognizer
#else
#   define PlatformPanGestureRecognizer UIPanGestureRecognizer
#endif

@implementation Lanox2dMetalView {
    Lanox2dMetalRenderer* _renderer;
}

- (void)dealloc {
    [self metalExit];
}

- (id)initWithFrame:(CGRect)frame delegate:(id)delegate {
    if (self = [super initWithFrame:frame]) {

        // enable resize
#ifdef LX_CONFIG_OS_MACOSX
        [self setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
#endif

        // init move gesture
        PlatformPanGestureRecognizer* moveGesture = [[PlatformPanGestureRecognizer alloc] initWithTarget:self action: @selector(onTouchMove:)];
        [self addGestureRecognizer:moveGesture];

        // init metal
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

- (void)onTouchMove:(PlatformPanGestureRecognizer *)recognizer {
	CGPoint pt = [recognizer locationInView:self];
    if (_lanox2dWindow) {
        lx_event_t event = {0};
        lx_touch_t touches[1];
        event.type            = LX_EVENT_TYPE_TOUCH;
        event.u.touch.code    = LX_TOUCH_MOVED;
        event.u.touch.count   = 1;
        event.u.touch.touches = touches;
        lx_point_make(&touches[0].start, pt.x, pt.y);
        lx_point_make(&touches[0].prev,  pt.x, pt.y);
        lx_point_make(&touches[0].point, pt.x, pt.y);
        lx_window_notify(_lanox2dWindow, &event);
    }
}

@end
