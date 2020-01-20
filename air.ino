#include <SoftwareSerial.h>


int smokeA5 = A5;
int smokeA4 = A4;
int smokeA3 = A3;
int smokeA2 = A2;
int smokeA1 = A1;

int sensorThres = 400;

String mq2;
String mq3;
String mq9;
String mq135;

SoftwareSerial esp(7, 8);
void setup() {

  pinMode(smokeA5, INPUT);
  pinMode(smokeA3, INPUT);
  pinMode(smokeA4, INPUT);
  pinMode(smokeA2, INPUT);
  pinMode(smokeA1, INPUT);
  Serial.begin(9600);
  esp.begin(115200);
}

void loop() {
  int MQ2 = analogRead(smokeA5);//讀取MQ2數值
  mq2 = String("A=") + String(MQ2); //整理要傳送到ESP8266的數值

  int MQ9 = analogRead(smokeA4);//讀取MQ9數值
  mq9 = String("B=") + String(MQ9); //整理要傳送到ESP8266的數值

  int MQ135 = analogRead(smokeA3);//讀取M1352數值
  mq135 = String("C=") + String(MQ135); //整理要傳送到ESP8266的數值

  int MQ3 = analogRead(smokeA1);//讀取MQ3數值
  mq3 = String("D=") + String(MQ3); //整理要傳送到ESP8266的數值


  Serial.print("MQ2: ");
  Serial.println(MQ2);
  esp.print(mq2); //傳送MQ2數值
  Serial.println("");
  delay(300);

  Serial.print("MQ3: ");
  Serial.println(MQ3);
  esp.println(mq3); //傳送MQ3數值
  Serial.println("");
  delay(300);

  Serial.print("MQ9: ");
  Serial.println(MQ9);
  esp.println(mq9); //傳送MQ9數值
  Serial.println("");
  delay(300);


  Serial.print("MQ135: ");
  Serial.println(MQ135);
  esp.println(mq135); //傳送MQ135數值
  Serial.println("");
  delay(300);

  Serial.println("------------------------------------------------");
  Serial.println("");

}
