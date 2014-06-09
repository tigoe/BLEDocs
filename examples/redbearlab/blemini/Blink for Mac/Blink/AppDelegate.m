//
//  AppDelegate.m
//  Blink
//
//  Created by Alasdair Allan on 05/06/2014.
//  Copyright (c) 2014 Babilim Light Industries. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    self.ble = [[BLE alloc] init];
    [self.ble controlSetup:1];
    self.ble.delegate = self;
    self.toggleButton.enabled = false;
}

- (IBAction)pushedConnect:(id)sender {
    if (self.ble.activePeripheral) {
        if(self.ble.activePeripheral.isConnected) {
            [[self.ble CM] cancelPeripheralConnection:[self.ble activePeripheral]];
            return;
        }
    }
    
    if (self.ble.peripherals) {
        self.ble.peripherals = nil;
    }
    
    [self.ble findBLEPeripherals:2];
    [NSTimer scheduledTimerWithTimeInterval:(float)2.0 target:self selector:@selector(connectionTimer:) userInfo:nil repeats:NO];
    
}

-(void) connectionTimer:(NSTimer *)timer {
    if (self.ble.peripherals.count > 0) {
        int biscuit = -1;
        for (int i = 0; i < self.ble.peripherals.count; i++) {
            CBPeripheral *peripheral = [self.ble.peripherals objectAtIndex:i];
            if ( [peripheral.name isEqualToString:@"Biscuit"] ) {
                biscuit = i;
            }
        }
        
        if ( biscuit != -1  ) {
            CBPeripheral *peripheral = [self.ble.peripherals objectAtIndex:biscuit];
            [self.ble connectPeripheral:peripheral];
            self.label.stringValue = [NSString stringWithFormat:@"Connecting to '%@'",peripheral.name];
        } else {
            self.label.stringValue = @"Can not find BLE mini board";
        }
        
    } else {
        self.label.stringValue = @"No BLE devices found";
    }
    
}

- (IBAction)pushedToggle:(id)sender {
    UInt8 buf[1] = {0x01};
    NSData *data = [[NSData alloc] initWithBytes:buf length:1];
    [self.ble write:data];
    NSLog(@"pushed button");
}

#pragma mark - BLE delegate

-(void) bleDidConnect {
    self.label.stringValue = @"Connected to BLE device";
    self.connectButton.title = @"Disconnect";
    self.toggleButton.enabled = true;
}

- (void)bleDidDisconnect {
    self.label.stringValue = @"Disconnected from BLE device";
    self.connectButton.title = @"Connect";
    self.toggleButton.enabled = false;
}

-(void) bleDidReceiveData:(unsigned char *)data length:(int)length {
  
}

-(void) bleDidUpdateRSSI:(NSNumber *) rssi {
    self.label.stringValue = [NSString stringWithFormat:@"RSSI Value: %@", rssi.stringValue];

}

@end
