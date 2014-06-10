// Set Serial Monitor to 115200
// Currently this will output large amounts of debugging data
 
// ----------------------------------------------------------------------
// The services.h file MUST be symlinked to the nrf8001 library directory
// ----------------------------------------------------------------------

#define NRF_DEBUG 1
#include <SPI.h>
#include <nRF8001.h>

#define LED_SWITCH_PIPE 1

// change nRF8001 reset pin to -1 if it's not connected
// Redbear BLE Shield users: to my knowledge reset pin is not connected so use -1!
// NOTE: if you choose -1, youll need to manually reset your device after powerup!!
#define RESET_PIN 9
#define REQN_PIN 10
#define RDYN_PIN 2

nRF8001 *nrf;

uint8_t pipeStatusReceived, dataSent;
unsigned long lastSent;

// Generic event handler, here it's just for debugging all received events
void eventHandler(nRFEvent *event)
{
  Serial.println("event handler");
 // nrf->debugEvent(event);
  Serial.print("event = ");
  Serial.println( event->event );
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
}

void setup() {
  pipeStatusReceived = 0;
  lastSent = 0;
  
  pinMode( 3, OUTPUT );
  
  Serial.begin(115200);
  Serial.println("Hello");
  
  // nRF8001 class initialized with pin numbers
  nrf = new nRF8001(RESET_PIN, REQN_PIN, RDYN_PIN);

  // Register event handles
  nrf->setEventHandler(&eventHandler);
  if ((nrf->setup()) == cmdSuccess) {
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
  Serial.println("polling");
  
  // Polling will block - times out after 2 seconds
  nrf->poll(2000);
  
  if (nrf->getConnectionStatus() == Disconnected) {
    Serial.println("Reconnecting");
    dataSent = 0;
    nrf->connect(0, 32);
  }
}
