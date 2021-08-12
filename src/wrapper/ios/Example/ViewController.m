//
//  ViewController.m
//  Example
//
//  Created by ruki on 2021/8/10.
//  Copyright © 2021 lanox2d. All rights reserved.
//

#import "ViewController.h"
#import <Lanox2d/Lanox2d.h>

@interface ViewController ()
//@property (strong, atomic) Lanox2dView* lanox2dView;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

#if 0
    // init lanox2d view
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    self.lanox2dView = [[Lanox2dView alloc] initWithFrame:screenBounds];
    [self.view addSubview:self.lanox2dView];
#endif
}


@end
