//-*- ObjC -*-

#import <Cocoa/Cocoa.h>

@interface RhineView : NSView <NSWindowDelegate>
@property (strong) IBOutlet NSTextField *mainBuffer;
@property (strong) IBOutlet NSTextField *miniBuffer;
- (IBAction)keyDownAction : (NSTextField *)sender;
- (IBAction)miniBufferAction:(NSTextField *)sender;
@end

