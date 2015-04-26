//
//  AppDelegate.h
//  co
//
//  Created by Ramkumar Ramachandra on 4/23/15.
//  Copyright (c) 2015 Ramkumar Ramachandra. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong) IBOutlet NSTextField *mainBuffer;
@property (strong) IBOutlet NSTextField *miniBuffer;
- (IBAction)miniBufferAction:(NSTextField *)sender;
@end

