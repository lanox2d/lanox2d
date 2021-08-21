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
 * @file        Lanox2dGLView.m
 *
 */

#import "Lanox2dGLView.h"
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import "lanox2d/lanox2d.h"

@interface Lanox2dGLView() {
    EAGLContext*    _glContext;
    GLuint          _glFrame;
    GLuint          _glRenderColor;
    GLint           _glWidth;
    GLint           _glHeight;
    CADisplayLink*  _displayLink;
}
@end

@implementation Lanox2dGLView

+ (Class) layerClass {
    return [CAEAGLLayer class];
}

- (void)dealloc {
    [self glExit];
}

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        [self glInit:frame];
    }
    return self;
}

- (void)glInit:(CGRect)frame {

    // init layer
    CAEAGLLayer* glLayer = (CAEAGLLayer*) self.layer;
    glLayer.opaque = NO;
    glLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

    // is retina?
    if ([UIScreen instancesRespondToSelector:@selector(currentMode)] && CGSizeEqualToSize(CGSizeMake(640, 960), [[UIScreen mainScreen] currentMode].size)) {
        self.contentScaleFactor = 2.0f;
        glLayer.contentsScale = 2.0f;
    }

    // init gl context
    _glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    [EAGLContext setCurrentContext:_glContext];

    // init frame
    [self glFrameInit];

    // init window
    _lanox2dWindow = lx_window_init(frame.size.width * glLayer.contentsScale, frame.size.height * glLayer.contentsScale, lx_null);

    // init move gesture
    UIPanGestureRecognizer* moveGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action: @selector(onTouchMove:)];
    [moveGesture setMinimumNumberOfTouches:1];
    [moveGesture setMaximumNumberOfTouches:1];
    [self addGestureRecognizer:moveGesture];

    // start display link
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(displayLinkHandle:)];
    [_displayLink setPaused:NO];
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
}

- (void)glExit {

    // stop display link
    [_displayLink setPaused:YES];
    [_displayLink removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    _displayLink = nil;

    // exit window
    if (_lanox2dWindow) {
        lx_window_exit(_lanox2dWindow);
        _lanox2dWindow = lx_null;
    }

    // exit frame
    [self glFrameExit];

    // exit gl context
    if ([EAGLContext currentContext] == _glContext) {
        [EAGLContext setCurrentContext:nil];
    }
    _glContext = nil;
}

- (void)glFrameInit {

    // init frame
    glGenFramebuffers(1, &_glFrame);
    glBindFramebuffer(GL_FRAMEBUFFER, _glFrame);

    // add color render to frame
    glGenRenderbuffers(1, &_glRenderColor);
    glBindRenderbuffer(GL_RENDERBUFFER, _glRenderColor);
    [_glContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(id<EAGLDrawable>)self.layer];
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _glRenderColor);
    lx_assert_and_check_return(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    // get width & height
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_glWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_glHeight);
}

- (void)glFrameExit {

    // free frame
    if (_glFrame) {
        glDeleteFramebuffers(1, &_glFrame);
        _glFrame = 0;
    }

    // free color render
    if (_glRenderColor) {
        glDeleteRenderbuffers(1, &_glRenderColor);
        _glRenderColor = 0;
    }
}

- (BOOL)lock {
    if (_glFrame) {
        glBindFramebuffer(GL_FRAMEBUFFER, _glFrame);
        return YES;
    }
    return NO;
}

- (void)draw {
    if (_glFrame) {
        glBindRenderbuffer(GL_RENDERBUFFER, _glRenderColor);
    }
    if (_glContext) {
        [_glContext presentRenderbuffer:GL_RENDERBUFFER];
    }
}

- (void)displayLinkHandle:(CADisplayLink *)link {
    if ([self lock]) {
        if (_lanox2dWindow) {
            lx_window_draw(_lanox2dWindow);
        }
        [self draw];
    }
}

- (void)onTouchMove:(UIPanGestureRecognizer *)recognizer {
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
