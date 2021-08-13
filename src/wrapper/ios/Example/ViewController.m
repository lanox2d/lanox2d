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

//#define TEST_EMPTY_WINDOW
#define TEST_SHAPE_WINDOW
#ifdef TEST_EMPTY_WINDOW
#   include "../../../examples/window.c"
#endif
#ifdef TEST_SHAPE_WINDOW
#   include "../../../examples/shape.c"
#endif

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
    
#ifdef TEST_EMPTY_WINDOW
    [self initEmptyWindow];
#endif

#ifdef TEST_SHAPE_WINDOW
    [self initShapeWindow];
#endif
}

#ifdef TEST_EMPTY_WINDOW
- (void)initEmptyWindow {
    lx_window_ref_t window = self.lanox2dView.lanox2dWindow;
    if (window) {
        lx_window_on_draw(window, on_draw);
        lx_window_on_event(window, on_event);
    }
}
#endif

#ifdef TEST_SHAPE_WINDOW
- (void)initShapeWindow {
    lx_window_ref_t window = self.lanox2dView.lanox2dWindow;
    if (window) {
        static lx_char_t* argv[] = {"", lx_null, lx_null};
        argv[1] = (lx_char_t*)"tiger";
        window_init((lx_window_ref_t)window, 2, argv);
        (lx_void_t)window_exit;
    }
}
#endif

@end
