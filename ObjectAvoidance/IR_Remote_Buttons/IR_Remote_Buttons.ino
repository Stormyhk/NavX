#include <IRremote.hpp>

const int IR_RECEIVER_PIN = 9; // IR sensor pin on Elegoo Uno

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVER_PIN);
  Serial.println("Press IR button to see hex code");
}

void loop() {
  if (IrReceiver.decode()) {
    // print the raw hex code for each button 
    Serial.print("0x");
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    
    IrReceiver.resume(); // ready for next signal
  }
}