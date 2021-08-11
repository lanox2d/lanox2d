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
    EAGLContext*    glContext;
    GLuint          glFrame;
    GLuint          glRenderColor;
    GLint           glWidth;
    GLint           glHeight;
    GLuint          glRenderStencil;
    GLuint          glFrameMsaa;
    GLuint          glRenderColorMsaa;
    GLuint          glRenderStencilMssa;
}
@end

@implementation Lanox2dView

- (void)dealloc {
    [self glExit];
}

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        [self glInit];
    }
    return self;
}

- (void)glInit {

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
    glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:glContext];
    
    // init frame
    [self glFrameInit];

}

- (void)glExit {
    
    // exit frame
    [self glFrameExit];
    
    // exit gl context
    if ([EAGLContext currentContext] == glContext)
        [EAGLContext setCurrentContext:nil];
    glContext = nil;
}

- (void)glFrameInit {
    
    // init frame
    glGenFramebuffers(1, &glFrame);
    glBindFramebuffer(GL_FRAMEBUFFER, glFrame);
    
    // add color render to frame
    glGenRenderbuffers(1, &glRenderColor);
    glBindRenderbuffer(GL_RENDERBUFFER, glRenderColor);
    [glContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(id<EAGLDrawable>)self.layer];
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, glRenderColor);
    lx_assert_and_check_return(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    
    // get width & height
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &glWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &glHeight);
    
    // add stencil render to frame
    glGenRenderbuffers(1, &glRenderStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, glRenderStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, glWidth, glHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glRenderStencil);
    lx_assert_and_check_return(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    // init mssa frame
    glGenFramebuffers(1, &glFrameMsaa);
    glBindFramebuffer(GL_FRAMEBUFFER, glFrameMsaa);
    
    // add color render to the mssa frame
    glGenRenderbuffers(1, &glRenderColorMsaa);
    glBindRenderbuffer(GL_RENDERBUFFER, glRenderColorMsaa);
    glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, 4, GL_RGBA8_OES, glWidth, glHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, glRenderColorMsaa);
    lx_assert_and_check_return(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    // add stencil render to the mssa frame
    glGenRenderbuffers(1, &glRenderStencilMssa);
    glBindRenderbuffer(GL_RENDERBUFFER, glRenderStencilMssa);
    glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, 4, GL_STENCIL_INDEX8, glWidth, glHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glRenderStencilMssa);
    lx_assert_and_check_return(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

- (void)glFrameExit {
    
    // free frame
    if (glFrame) {
        glDeleteFramebuffers(1, &glFrame);
        glFrame = 0;
    }
    
    // free color render
    if (glRenderColor) {
        glDeleteRenderbuffers(1, &glRenderColor);
        glRenderColor = 0;
    }
    
    // free stencil render
    if (glRenderStencil) {
        glDeleteRenderbuffers(1, &glRenderStencil);
        glRenderStencil = 0;
    }
    
    // free frame for mssa
    if (glFrameMsaa) {
        glDeleteFramebuffers(1, &glFrameMsaa);
        glFrameMsaa = 0;
    }
    
    // free color render for mssa
    if (glRenderColorMsaa) {
        glDeleteRenderbuffers(1, &glRenderColorMsaa);
        glRenderColorMsaa = 0;
    }
    
    // free stencil render for mssa
    if (glRenderStencilMssa) {
        glDeleteRenderbuffers(1, &glRenderStencilMssa);
        glRenderStencilMssa = 0;
    }
}

@end
