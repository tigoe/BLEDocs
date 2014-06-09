
int pinState = 0;

void setup() {
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(57600);
  digitalWrite( 12, HIGH );
}

void loop() {
  while( Serial.available() ) {
    byte dat = Serial.read();
    //if ( dat == 0x01 ) {
      if ( pinState == 0 ) {
         pinState = 1;
         digitalWrite( 13, HIGH);
       } else {
         pinState = 0;
         digitalWrite( 13, LOW );
       }
     //}
   }
}
