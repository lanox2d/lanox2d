//
//  ViewController.m
//  Example
//
//  Created by ruki on 2021/8/10.
//  Copyright © 2021 lanox2d. All rights reserved.
//

#import "ViewController.h"
#import "lanox2d/lanox2d.h"
#import "Lanox2dView.h"
#include "../../../examples/window.c"

@interface ViewController ()
@property (strong) Lanox2dView* lanox2dView;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // init lanox2d view
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    self.lanox2dView = [[Lanox2dView alloc] initWithFrame:screenBounds];
    [self.view addSubview:self.lanox2dView];

    lx_window_ref_t window = self.lanox2dView.window;
    if (window) {
        lx_window_on_draw(window, on_draw);
        lx_window_on_event(window, on_event);
    }
}


@end
