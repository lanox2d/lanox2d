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
{
    Lanox2dView*    _lanox2dView;
    UILabel*        _infoView;
    CADisplayLink*  _displayLink;
    float           _fpsCount;
    float           _fpsTime;
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // init lanox2d view
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    _lanox2dView = [[Lanox2dView alloc] initWithFrame:screenBounds];
    [self.view addSubview:_lanox2dView];
    
    // init info view
    CGRect infoBounds = CGRectMake(screenBounds.origin.x + screenBounds.size.width - 100, screenBounds.origin.y + 30, 100, 30);
    _infoView = [[UILabel alloc] initWithFrame:infoBounds];
    [_infoView setTextColor:[UIColor redColor]];
    [self.view addSubview:_infoView];
    
    // init display link for fps
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(displayLinkHandle:)];
    [_displayLink setPaused:NO];
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];

#ifdef TEST_EMPTY_WINDOW
    [self initEmptyWindow];
#endif

#ifdef TEST_SHAPE_WINDOW
    [self initShapeWindow:@"path"];
#endif
}

- (void)displayLinkHandle:(CADisplayLink *)link {
    float time = link.timestamp;
    if (!_fpsTime) {
        _fpsTime = time;
    } else {
        _fpsCount++;
    }
    if (time > _fpsTime + 1) {
        float framerate = _fpsCount / (time - _fpsTime);
        [_infoView setText:[NSString stringWithFormat:@"%0.02f fps", framerate]];
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
