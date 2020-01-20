#include<SoftwareSerial.h>
SoftwareSerial ArduinoUno(7,8);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ArduinoUno.begin(115200);
  delay(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
