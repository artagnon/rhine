//-*- ObjC -*-

#import "View.h"

@implementation RhineView
@synthesize miniBuffer;
@synthesize mainBuffer;

- (BOOL)acceptsFirstResponder {
    return YES;
}
- (void)keyDown : (NSEvent *)theEvent
{
}
- (IBAction)keyDownAction : (NSTextField *)sender {
    [self.mainBuffer setStringValue:@"KeyDown"];
}
- (IBAction)miniBufferAction : (NSTextField *)sender {
    [self.mainBuffer setStringValue:@"MiniBuffer"];
}
@end
