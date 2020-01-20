#include <SoftwareSerial.h>
SoftwareSerial ArduinoUno(7,8); //Rx , Tx
String f;
int measurePin = 0; //Connect dust sensor to Arduino A0 pin
int ledPower = 2; //Connect 3 led driver pins of dust sensor to Arduino D2int samplingTime = 280;
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void setup(){
Serial.begin(9600);
pinMode(ledPower,OUTPUT);
ArduinoUno.begin(115200);
}

void loop(){
digitalWrite(ledPower,LOW); // power on the LED
delayMicroseconds(samplingTime);
voMeasured = analogRead(measurePin); // read the dust value

delayMicroseconds(deltaTime);
digitalWrite(ledPower,HIGH); // turn the LED off
delayMicroseconds(sleepTime);


calcVoltage = voMeasured * (5.0 / 1024.0);


dustDensity = 0.17 * calcVoltage - 0.1;



ArduinoUno.print(String(dustDensity*1000));

Serial.print(" - Dust Density: ");
Serial.print(dustDensity * 1000); // 這裡將數值呈現改成較常用的單位( ug/m3 )
Serial.println(" ug/m3 ");
delay(1000);
 
   //傳送字串
}
