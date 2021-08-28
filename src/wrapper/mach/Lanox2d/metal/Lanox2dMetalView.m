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
#   import "MetalKeys.h"
#endif

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

- (void)onTouchMove:(PlatformPanGestureRecognizer*)recognizer {
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

#ifdef LX_CONFIG_OS_MACOSX
- (BOOL)acceptsFirstResponder {
  return YES;
}

- (lx_uint16_t)keyCode:(NSEvent*)keyEvent {
    lx_uint16_t code = LX_KEY_NUL;
    lx_bool_t has_shift = [keyEvent modifierFlags] & NSEventModifierFlagShift;
    switch ([keyEvent keyCode]) {
        case KEY_A: code = 'a'; break;
        case KEY_B: code = 'b'; break;
        case KEY_C: code = 'c'; break;
        case KEY_D: code = 'd'; break;
        case KEY_E: code = 'e'; break;
        case KEY_F: code = 'f'; break;
        case KEY_G: code = 'g'; break;
        case KEY_H: code = 'h'; break;
        case KEY_I: code = 'i'; break;
        case KEY_J: code = 'j'; break;
        case KEY_K: code = 'k'; break;
        case KEY_L: code = 'l'; break;
        case KEY_M: code = 'm'; break;
        case KEY_N: code = 'n'; break;
        case KEY_O: code = 'o'; break;
        case KEY_P: code = 'p'; break;
        case KEY_Q: code = 'q'; break;
        case KEY_R: code = 'r'; break;
        case KEY_S: code = 's'; break;
        case KEY_T: code = 't'; break;
        case KEY_U: code = 'u'; break;
        case KEY_V: code = 'v'; break;
        case KEY_W: code = 'w'; break;
        case KEY_X: code = 'x'; break;
        case KEY_Y: code = 'y'; break;
        case KEY_Z: code = 'z'; break;
        case KEY_0: code = has_shift? ')' : '0'; break;
        case KEY_1: code = has_shift? '!' : '1'; break;
        case KEY_2: code = has_shift? '@' : '2'; break;
        case KEY_3: code = has_shift? '#' : '3'; break;
        case KEY_4: code = has_shift? '$' : '4'; break;
        case KEY_5: code = has_shift? '%' : '5'; break;
        case KEY_6: code = has_shift? '^' : '6'; break;
        case KEY_7: code = has_shift? '&' : '7'; break;
        case KEY_8: code = has_shift? '*' : '8'; break;
        case KEY_9: code = has_shift? '(' : '9'; break;
        case KEY_MINUS: code = has_shift? '_' : '-'; break;
        case KEY_EQUAL: code = has_shift? '+' : '='; break;
        case KEY_COMMA: code = has_shift? '<' : ','; break;
        case KEY_DOT:   code = has_shift? '>' : '.'; break;
        case KEY_SLASH: code = has_shift? '?' : '/'; break;
        case KEY_BACKAPOSTROPHE: code = has_shift? '~' : '`'; break;
        case KEY_BACKSLASH:  code = has_shift? '|' : '\\'; break;
        case KEY_LEFTBRACKET:  code = has_shift? '{' : '['; break;
        case KEY_RIGHTBRACKET: code = has_shift? '}' : ']'; break;
        case KEY_SEMICOLON:  code = has_shift? ':' : ';'; break;
        case KEY_APOSTROPHE: code = has_shift? '"' : '\''; break;
        case KEY_TAB:   code = LX_KEY_TAB; break;
        case KEY_LEFT:  code = LX_KEY_LEFT; break;
        case KEY_RIGHT: code = LX_KEY_RIGHT; break;
        case KEY_UP:    code = LX_KEY_UP; break;
        case KEY_DOWN:  code = LX_KEY_DOWN; break;
        case KEY_SPACE: code = LX_KEY_SPACE; break;
        case KEY_ESCAPE:code = LX_KEY_ESCAPE; break;
        case KEY_RETURN: code = LX_KEY_ENTER; break;
        default:
            lx_trace_d("%d", [keyEvent keyCode]);
            break;
    }
    if (code >= 'a' && code <= 'z' && has_shift) {
        code = lx_toupper(code);
    }
    return code;
}

- (void)keyDown:(NSEvent*)keyEvent {
    lx_event_t event = {0};
    event.type               = LX_EVENT_TYPE_KEYBOARD;
    event.u.keyboard.pressed = lx_true;
    event.u.keyboard.code    = [self keyCode:keyEvent];
    if (_lanox2dWindow && event.u.keyboard.code) {
        lx_window_notify(_lanox2dWindow, &event);
    }
}

- (void)keyUp:(NSEvent*)keyEvent {
    lx_event_t event = {0};
    event.type               = LX_EVENT_TYPE_KEYBOARD;
    event.u.keyboard.pressed = lx_false;
    event.u.keyboard.code    = [self keyCode:keyEvent];
    if (_lanox2dWindow && event.u.keyboard.code) {
        lx_window_notify(_lanox2dWindow, &event);
    }
}
#endif

@end
