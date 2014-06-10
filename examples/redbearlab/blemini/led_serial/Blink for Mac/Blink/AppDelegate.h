//
//  AppDelegate.h
//  Blink
//
//  Created by Alasdair Allan on 05/06/2014.
//  Copyright (c) 2014 Babilim Light Industries. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "BLE.h"

@interface AppDelegate : NSObject <NSApplicationDelegate, BLEDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (strong, nonatomic) BLE *ble;

@property (weak) IBOutlet NSButton *connectButton;
@property (weak) IBOutlet NSButton *toggleButton;
@property (weak) IBOutlet NSTextField *label;

- (IBAction)pushedConnect:(id)sender;
- (IBAction)pushedToggle:(id)sender;

@end
