//
//  ViewController.m
//  Example
//
//  Created by ruki on 2021/8/10.
//  Copyright © 2021 lanox2d. All rights reserved.
//

#import "ViewController.h"

//#define TEST_EMPTY_WINDOW
#define TEST_SHAPE_WINDOW
#ifdef TEST_EMPTY_WINDOW
#   include "../../../examples/window.c"
#endif
#ifdef TEST_SHAPE_WINDOW
#   include "../../../examples/shape.c"
#endif

@interface ViewController ()
{
    Lanox2dView*    _lanox2dView;
    float           _fpsCount;
    double          _fpsTime;
#ifdef LX_CONFIG_OS_IOS
    UILabel*        _infoView;
#endif
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // init lanox2d view
    CGRect frameBounds = [self.view frame];
    _lanox2dView = [[Lanox2dView alloc] initWithFrame:frameBounds delegate:self];
    [self.view addSubview:_lanox2dView];

    // init info view
#ifdef LX_CONFIG_OS_IOS
    CGRect infoBounds = CGRectMake(frameBounds.origin.x + frameBounds.size.width - 100, frameBounds.origin.y + 30, 100, 30);
    _infoView = [[UILabel alloc] initWithFrame:infoBounds];
    [_infoView setTextColor:[UIColor redColor]];
    [self.view addSubview:_infoView];
#endif

#ifdef TEST_EMPTY_WINDOW
    [self initEmptyWindow];
#endif

#ifdef TEST_SHAPE_WINDOW
    [self initShapeWindow:@"path"];
#endif
}

- (void)onDrawFrame:(CFTimeInterval)timestamp {
    double time = timestamp;
    if (!_fpsTime) {
        _fpsTime = time;
    } else {
        _fpsCount++;
    }
    if (time > _fpsTime + 1) {
        float framerate = (float)(_fpsCount / (time - _fpsTime));
#ifdef LX_CONFIG_OS_MACOSX
        self.view.window.title = [NSString stringWithFormat:@"Lanox2d (%0.02f fps)", framerate];
#else
        [_infoView setText:[NSString stringWithFormat:@"%0.02f fps", framerate]];
#endif
        _fpsCount = 0;
        _fpsTime = time;
    }
}

#ifdef TEST_EMPTY_WINDOW
- (void)initEmptyWindow {
    lx_window_ref_t window = _lanox2dView.lanox2dWindow;
    if (window) {
        lx_window_on_draw(window, on_draw);
        lx_window_on_event(window, on_event);
    }
}
#endif

#ifdef TEST_SHAPE_WINDOW
- (void)initShapeWindow:(NSString*)name {
    lx_window_ref_t window = _lanox2dView.lanox2dWindow;
    if (window) {
        static char* argv[] = {"", lx_null, lx_null};
        argv[1] = (char*)[name UTF8String];
        window_init((lx_window_ref_t)window, 2, argv);
        (lx_void_t)window_exit;
    }
}
#endif

@end
