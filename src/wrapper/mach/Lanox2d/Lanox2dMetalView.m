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
#import "lanox2d/lanox2d.h"

@interface Lanox2dMetalView() {
}
@end

@implementation Lanox2dMetalView

- (void)dealloc {
}

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        [self metalInit];
    }
    return self;
}

- (void)metalInit {
    self.device = MTLCreateSystemDefaultDevice();
    NSAssert(self.device, @"Metal is not supported on this device");
}

@end
