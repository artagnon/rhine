//-*- ObjC -*-

#import <Cocoa/Cocoa.h>
#import "ViewController.h"

@interface RhineView : NSView <NSApplicationDelegate>
@property (strong, nonatomic) IBOutlet RhineViewController *viewController;
@property (strong) IBOutlet NSTextField *mainBuffer;
@property (strong) IBOutlet NSTextField *miniBuffer;
@end

