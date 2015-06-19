//
//  App.h
//  RhineCo
//
//  Created by Ramkumar Ramachandra on 6/16/15.
//  Copyright (c) 2015 Ramkumar Ramachandra. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "ViewController.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) NSWindow *window;
@property (nonatomic,strong) IBOutlet RhineViewController *viewController;

@end