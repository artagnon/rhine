//
//  ViewController.m
//  RhineCo
//
//  Created by Ramkumar Ramachandra on 6/16/15.
//  Copyright (c) 2015 Ramkumar Ramachandra. All rights reserved.
//

#import "ViewController.h"

@implementation RhineViewController
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
    }
    return self;
}
-(void)keyDown:(NSEvent*)theEvent
{
    NSLog(@"Caught key event");
}
-(void)mouseDown:(NSEvent*)theEvent
{
    NSLog(@"Caught mouse event");
}
@end