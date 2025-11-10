#include <HardwareSerial.h>
HardwareSerial mySerial(0); // UART0 (Serial0)

void setup() {
  Serial.begin(9600);  // USB serial
  mySerial.begin(9600); // UART0
}

void loop() {
  if (Serial.available()) {
    char data = Serial.read();
    Serial.print("Received on USB: ");
    Serial.println(data);
  }
  
  if (mySerial.available()) {
    char data = mySerial.read();
    Serial.print("Received on UART0: ");
    Serial.println(data);
  }
}
