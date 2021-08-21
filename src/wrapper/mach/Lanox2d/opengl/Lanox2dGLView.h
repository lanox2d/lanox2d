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
 * @file        Lanox2dGLView.h
 *
 */

#import "lanox2d/lanox2d.h"
#ifdef LX_CONFIG_OS_MACOSX
#   import <AppKit/AppKit.h>
#   define PlatformView NSView
#else
#   import <UIKit/UIKit.h>
#   define PlatformView UIView
#endif

NS_ASSUME_NONNULL_BEGIN

@interface Lanox2dGLView : PlatformView
- (id)initWithFrame:(CGRect)frame delegate:(id)delegate;
@property (nonatomic) lx_window_ref_t lanox2dWindow;
@end

NS_ASSUME_NONNULL_END
