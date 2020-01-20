#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial finger(7, 8); //Rx , Tx

const byte ROWS = 4; //四行
const byte COLS = 4; //四列
//定义键盘上的按键标识
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {4, 5, 6, 13};
byte colPins[COLS] = {12, 9, 10, 11};
String f;

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
char id_array[] = {'a'};
int i = 0;
char last = 10;
Servo servo1;
Servo servo2;
String c;
String ID = "";
char command;
String com;
String space;
char password[3];
char ans[3] = {'3', '0', '6'};
void setup() {
  Serial.begin(115200);
  finger.begin(9600);
  pinMode(7, INPUT);
  pinMode(8, OUTPUT);
  lcd.begin(16, 2);
  lcd.backlight();
  servo1.attach(2);
  servo2.attach(3);
  servo1.write(0);
  servo2.write(0);
  finger.print(String("0"));

}

void loop() {

  //  char id = customKeypad.getKey();
  //  int num = sizeof(id_array) + 1;
  //  id_array[num] = id;
  //  for(int a = 0 ; a<sizeof(id_array) ; i++){
  //    if(id_array[i]=='D'){
  //      for(int b =0;b<=(a-1);b++){
  //        ID = String(ID)+String(id_array[b]);
  //      }
  //    }
  //  }
  lcd.setCursor(0, 1);
  while (finger.available())
  {
    command = finger.read();
    com.concat(command);
    
  delay(200);
  Serial.println(com);
  if (com == "0") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  start enroll  ");
  } else if (com == "1") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" remove  finger ");
  } else if (com == "2") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  put it again  ");
  } else if (com == "3") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" enroll  finish ");
  } else if (com == "4") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" start recogize ");
  } else if (com == "5") {
    lcd.setCursor(0, 0);
    lcd.print("    Welcome     ");
    Serial.println("welcome");
    servo1.write(90);
    servo2.write(90);
    delay(5000);
    servo1.write(0);
    servo2.write(0);
  } else if (com == "8") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("               ");
  }


  last = command;
 


}
}
  
  
