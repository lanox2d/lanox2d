//
//  main.m
//  Example
//
//  Created by ruki on 2021/8/10.
//  Copyright © 2021 lanox2d. All rights reserved.
//

#include "lanox2d/lanox2d.h"
#ifdef LX_CONFIG_OS_MACOSX
#   import <Cocoa/Cocoa.h>
#else
#   import <UIKit/UIKit.h>
#   import <TargetConditionals.h>
#   import <Availability.h>
#   import "AppDelegate.h"
#endif

#ifdef LX_CONFIG_OS_MACOSX
int main(int argc, const char * argv[]) {
    return NSApplicationMain(argc, argv);
}
#else
int main(int argc, char * argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
#endif
