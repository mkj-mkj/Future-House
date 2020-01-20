#include <Servo.h>
#include <SoftwareSerial.h>
Servo window_right;
Servo window_left;
SoftwareSerial ArduinoUno(7,8); //Rx , Tx
char inChar;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ArduinoUno.begin(9600); //傳至ESP8266鮑率
  pinMode(12, OUTPUT);
  window_right.attach(2);
  window_left.attach(4);
}

void loop() {
  // put your main code here, to run repeatedly:
 
  if (ArduinoUno.available())
  {
    inChar = ArduinoUno.read();
    Serial.println(inChar);
    
  }

  delay(100);
//  switch(inChar){
//    case '1':
//      digitalWrite(12,HIGH);
//
//    case '0':
//      digitalWrite(12,LOW);
//  }
//  

  if(inChar=='1'){
    if(window_right.read()!=70 ||window_right.read()!=0){
      window_right.write(70);
      window_left.write(0);
      delay(50);
    }
    else{
      
    }
  }
  else if(inChar=='0'){
    if(window_right.read()!=0 ||window_right.read()!=70){
      window_right.write(0);
      window_left.write(70);
      delay(50);
    }
    else{
      
    }
  }
}
