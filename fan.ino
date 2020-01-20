#include <SoftwareSerial.h>

SoftwareSerial ArduinoUno(7, 8); //Rx , Tx
char inChar;
void setup() {
  Serial.begin(9600);
  ArduinoUno.begin(9600); //傳至ESP8266鮑率
  pinMode(12, OUTPUT);
}

void loop() {
  //讀取ESP8266傳送的指令
  if (ArduinoUno.available())
  {
    inChar = ArduinoUno.read();
    Serial.println(inChar);
  }
  delay(100);

  //當接收到指令為開，且目前電扇狀態為關時，就開啟電扇
  if (inChar == '1') {
    if (digitalRead(12) != 1) {
      digitalWrite(12, HIGH);
      Serial.print("digitalRead");
      Serial.println(digitalRead(12));
      delay(100);
    }
  }

  //當接收到指令為關，且目前電扇狀態為開時，就關閉電扇
  else if (inChar == '0') {
    if (digitalRead(12) != 0) {
      digitalWrite(12, LOW);
      Serial.print("digitalRead");
      Serial.println(digitalRead(12));
      delay(100);
    }
  }
}
