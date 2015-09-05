//-*- ObjC -*-

#import "View.h"

@implementation RhineView
@synthesize miniBuffer;
@synthesize mainBuffer;

- (BOOL)acceptsFirstResponder {
    return YES;
}
- (void)setViewController:(RhineViewController *)newController
{
    if (self.viewController)
    {
        NSResponder *controllerNextResponder = [self.viewController nextResponder];
        [super setNextResponder:controllerNextResponder];
        [self.viewController setNextResponder:nil];
    }
    self.viewController = newController;
    if (newController)
    {
        NSResponder *ownNextResponder = [self nextResponder];
        [super setNextResponder: self.viewController];
        [self.viewController setNextResponder:ownNextResponder];
    }  
}
- (void)setNextResponder:(NSResponder *)newNextResponder
{
    if (self.viewController)
    {
        [self.viewController setNextResponder:newNextResponder];
        return;
    }
    [super setNextResponder:newNextResponder];
}

- (IBAction)miniBufferAction:(id)sender {
    NSLog(@"Caught miniBufferAction");
}

@end
