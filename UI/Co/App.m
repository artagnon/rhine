//
//  App.m
//  RhineCo
//
//  Created by Ramkumar Ramachandra on 6/16/15.
//  Copyright (c) 2015 Ramkumar Ramachandra. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "App.h"
#import "View.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    self.viewController = [[RhineViewController alloc] initWithNibName:@"RhineViewController"bundle:nil];
    [self.window.contentView addSubview:self.viewController.view];
    self.viewController.view.frame = ((NSView*)self.window.contentView).bounds;
    [self.window makeFirstResponder:self.viewController.view];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end