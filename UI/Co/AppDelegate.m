//
//  AppDelegate.m
//  co
//
//  Created by Ramkumar Ramachandra on 4/23/15.
//  Copyright (c) 2015 Ramkumar Ramachandra. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate
@synthesize miniBuffer;
@synthesize mainBuffer;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

- (IBAction)miniBufferAction:(NSTextField *)sender {
    [self.mainBuffer setStringValue:@"Foom"];
}
@end
