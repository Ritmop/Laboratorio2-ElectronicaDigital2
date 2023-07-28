#include <SoftwareSerial.h>
#define rxPIC 2 //Recieve from PIC
#define txPIC 3 //Transmit to PIC

SoftwareSerial SerialPIC(rxPIC, txPIC);

void setup() {
  Serial.begin(9600);
  SerialPIC.begin(9600);
}

void loop() {
  //Recepcion PIC desplegada en Monitor Arduino
  while(SerialPIC.available()){
    char picTx = SerialPIC.read();
    Serial.print(picTx);
  }
  //Recepcion Monitor Ardiono transmitida a PIC
  while(Serial.available()){
    char picRx = Serial.read();
    SerialPIC.write(picRx);    
    delay(50);
  }
}
