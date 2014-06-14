// Set Serial Monitor to 115200
// Currently this will output large amounts of debugging data

// This version of the code acts more like a light switch. The LED doesn't go off when you
// release the button. Notifications are made on button press and release as before however.

#include "services.h"

#define NRF_DEBUG 1
#include <SPI.h>
#include <nRF8001.h>

#define LED_SWITCH_PIPE 1
#define BUTTON_TX_PIPE 3
#define BUTTON_STATE_PIPE 4

// change nRF8001 reset pin to -1 if it's not connected
// Redbear BLE Shield users: to my knowledge reset pin is not connected so use -1!
// NOTE: if you choose -1, youll need to manually reset your device after powerup!!
#define RESET_PIN 9
#define REQN_PIN 10
#define RDYN_PIN 2

hal_aci_data_t setup_msgs[NB_SETUP_MESSAGES] = SETUP_MESSAGES_CONTENT;
nRF8001 *nrf;

int val;               
int buttonState;                // variable to hold the last button state
int ledState;

uint8_t pipeStatusReceived, dataSent;
unsigned long lastSent;

// Generic event handler, here it's just for debugging all received events
void eventHandler(nRFEvent *event)
{
  Serial.println("event handler");
 // nrf->debugEvent(event);
  Serial.print("event = ");
  Serial.println( event->event );
  
  // Data Received
  if ( event->event == NRF_DATARECEIVEDEVENT ) {
     Serial.println( "We have data" ); 
     Serial.print( "pipe = " );
     Serial.println( event->msg.dataReceived.servicePipeNo );
     
     Serial.print( "length = " );
     Serial.println( event->length );
     
     Serial.print( "data = " );
     for ( int i = 0; i < event->length - 2; i++ ) {
       Serial.print( event->msg.dataReceived.data[i] );
     }
     Serial.println("");
     if ( event->msg.dataReceived.data[0] > 0 ) {
       digitalWrite( 3, HIGH );
     } else {
       digitalWrite( 3, LOW );
     }
  }
  
  // Credit Received
  if ( event->event == NRF_DATACREDITEVENT ) {
     Serial.println( "We have a credit" ); 

  }  
  
}

void setup() {
  pipeStatusReceived = 0;
  lastSent = 0;
  
  pinMode( 3, OUTPUT );
  pinMode( 4, OUTPUT );
  ledState = 0;
  digitalWrite( 3, LOW ); // Turn the LED off on boot
  
  Serial.begin(115200);
  Serial.println("Hello");
  
  // nRF8001 class initialized with pin numbers
  nrf = new nRF8001(RESET_PIN, REQN_PIN, RDYN_PIN);

  // Register event handles
  nrf->setEventHandler(&eventHandler);
  if ((nrf->setup( setup_msgs, NB_SETUP_MESSAGES)) == cmdSuccess) {
    Serial.println("SUCCESS");
  } else {
    Serial.println("FAIL");
    while (1);
  }
  
  // These functions merely request device address and temperature,
  // actual responses are asynchronous. They'll return error codes
  // if somehow the request itself failed, for example because
  // the device is not ready for these commands.
  nrf->getDeviceAddress();
  nrf->poll();
  
  nrf->connect(0, 32);
}

void loop() {
  //Serial.println("polling");
  
  // Polling will block - times out after 50 milliseconds
  nrf->poll(50);
  
  if (nrf->getConnectionStatus() == Disconnected) {
    Serial.println("Reconnecting");
    dataSent = 0;
    nrf->connect(0, 32);
  }
  
  val = digitalRead(4);      // read input value and store it in val
  if (val != buttonState) {          // the button state has changed!
    uint8_t foo = -1;
    uint8_t bar = -1;
    Serial.print("credits available ");
    Serial.println( nrf->creditsAvailable() );
    
    if (val == LOW) {                // check if the button is pressed
      Serial.println("Button just released");
      //digitalWrite( 3, LOW );
      //bar = 0;
      //nrf->setLocalData(LED_SWITCH_PIPE, 1, &bar);
      
      // STATE
      if (nrf->isPipeOpen(BUTTON_STATE_PIPE) && nrf->creditsAvailable()) {
         Serial.println("Setting 0");
         foo = 0;
         nrf->setLocalData(BUTTON_STATE_PIPE, 1, &foo);
         
      } 
      
      // TX
      if ( nrf->isPipeOpen(BUTTON_TX_PIPE) ) {
         Serial.println("Sending 0");
         foo = 0;
         nrf->sendData(BUTTON_TX_PIPE, 1, &foo);         
      } 
      
    } else {                         // the button is -not- pressed...
      Serial.println("Button just pressed");
      if ( ledState == 1 ) {
        // turn the LED off
        digitalWrite(3, LOW );
        ledState = 0;
        bar = 0;
        nrf->setLocalData(LED_SWITCH_PIPE, 1, &bar);
      } else {
        // turn it on
        digitalWrite( 3, HIGH );
        ledState = 1;
        bar = 1;
        nrf->setLocalData(LED_SWITCH_PIPE, 1, &bar);
      }
      


      // STATE
      if (nrf->isPipeOpen(BUTTON_STATE_PIPE) && nrf->creditsAvailable()) {
         Serial.println("Setting 1");
         foo = 1;
         nrf->setLocalData(BUTTON_STATE_PIPE, 1, &foo);
      } 
      
      // TX
      if ( nrf->isPipeOpen(BUTTON_TX_PIPE) ) {
         Serial.println("Sending 1");
         foo = 1;
         nrf->sendData(BUTTON_TX_PIPE, 1, &foo);
      }
    }
    
  }

  buttonState = val;                 // save the new state in our variable
  
}

