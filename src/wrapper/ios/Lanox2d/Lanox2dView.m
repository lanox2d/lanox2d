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
 * @file        Lanox2dView.m
 *
 */

#import "Lanox2dView.h"
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import "lanox2d/lanox2d.h"

@interface Lanox2dView() {
    EAGLContext*    _glContext;
    GLuint          _glFrame;
    GLuint          _glRenderColor;
    GLint           _glWidth;
    GLint           _glHeight;
    GLuint          _glRenderStencil;
    GLuint          _glFrameMsaa;
    GLuint          _glRenderColorMsaa;
    GLuint          _glRenderStencilMssa;
    CADisplayLink*  _displayLink;
}
@end

@implementation Lanox2dView

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
    _glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:_glContext];

    // init frame
    [self glFrameInit];

    // init window
//    _window = lx_window_init(frame.size.width, frame.size.height, lx_null);

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
    if (_window) {
        lx_window_exit(_window);
        _window = lx_null;
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

    // add stencil render to frame
    glGenRenderbuffers(1, &_glRenderStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, _glRenderStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, _glWidth, _glHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _glRenderStencil);
    lx_assert_and_check_return(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    // init mssa frame
    glGenFramebuffers(1, &_glFrameMsaa);
    glBindFramebuffer(GL_FRAMEBUFFER, _glFrameMsaa);

    // add color render to the mssa frame
    glGenRenderbuffers(1, &_glRenderColorMsaa);
    glBindRenderbuffer(GL_RENDERBUFFER, _glRenderColorMsaa);
    glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, 4, GL_RGBA8_OES, _glWidth, _glHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _glRenderColorMsaa);
    lx_assert_and_check_return(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    // add stencil render to the mssa frame
    glGenRenderbuffers(1, &_glRenderStencilMssa);
    glBindRenderbuffer(GL_RENDERBUFFER, _glRenderStencilMssa);
    glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, 4, GL_STENCIL_INDEX8, _glWidth, _glHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _glRenderStencilMssa);
    lx_assert_and_check_return(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
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

    // free stencil render
    if (_glRenderStencil) {
        glDeleteRenderbuffers(1, &_glRenderStencil);
        _glRenderStencil = 0;
    }

    // free frame for mssa
    if (_glFrameMsaa) {
        glDeleteFramebuffers(1, &_glFrameMsaa);
        _glFrameMsaa = 0;
    }

    // free color render for mssa
    if (_glRenderColorMsaa) {
        glDeleteRenderbuffers(1, &_glRenderColorMsaa);
        _glRenderColorMsaa = 0;
    }

    // free stencil render for mssa
    if (_glRenderStencilMssa) {
        glDeleteRenderbuffers(1, &_glRenderStencilMssa);
        _glRenderStencilMssa = 0;
    }
}

- (BOOL)lock {
    if (_glFrame) {
        if (lx_quality() > LX_QUALITY_LOW) {
            glBindFramebuffer(GL_FRAMEBUFFER, _glFrameMsaa);
            glBindRenderbuffer(GL_RENDERBUFFER, _glRenderColorMsaa);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, _glFrame);
        }
        return YES;
    }
    return NO;
}

- (void)draw {
    if (_glFrame) {
        if (lx_quality() > LX_QUALITY_LOW) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, _glFrameMsaa);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, _glFrame);
            glResolveMultisampleFramebufferAPPLE();
            glBindFramebuffer(GL_FRAMEBUFFER, _glFrame);
            glBindRenderbuffer(GL_RENDERBUFFER, _glRenderColor);
        } else {
            glBindRenderbuffer(GL_RENDERBUFFER, _glRenderColor);
        }
    }
    if (_glContext) {
        [_glContext presentRenderbuffer:GL_RENDERBUFFER];
    }
}

- (void)displayLinkHandle:(CADisplayLink *)link {
    if ([self lock]) {
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        [self draw];
    }
}

@end
