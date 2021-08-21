//
//  ViewController.h
//  Example
//
//  Created by ruki on 2021/8/10.
//  Copyright © 2021 lanox2d. All rights reserved.
//

#include "Lanox2d.h"
#ifdef LX_CONFIG_OS_MACOSX
#   import <AppKit/AppKit.h>
#   define PlatformViewController NSViewController
#else
#   import <UIKit/UIKit.h>
#   define PlatformViewController UIViewController
#endif

@interface ViewController : PlatformViewController<Lanox2dViewDelegate>

@end

