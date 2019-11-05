#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <WiFiClient.h>
#include <Servo.h>


SoftwareSerial finger(D7, D8);
Servo servo1;
Servo servo2;
//SoftwareSerial toUno(D4,D5);
String last = "10";
String door = "";
String app;
int i = 1;
String last_command;
String last_door;
String last_app;
const char* ssid      = "2019 TCA_CHT";
const char* password  = "20192019";
#define FIREBASE_HOST "https://smart-house-622bd.firebaseio.com/"
#define FIREBASE_AUTH "GOaVTdNwC2Pgos7nbYQ30niW68mbnl5QBv5RsHOX"
String command;
String path = "/house";
FirebaseData firebaseData;
void printJsonObjectContent(FirebaseData &data);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  finger.begin(9600);

  delay(10);
  pinMode(D3, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D7, INPUT);
  pinMode(D8, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin("2019 TCA_CHT", "20192019");

  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

  Serial.println(" ");
  Serial.print("connected：");
  Serial.println(WiFi.localIP());

  //  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);

  delay(10);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  Firebase.enableClassicRequest(firebaseData, true);

  servo1.attach(D2);//宣告伺服馬達腳位
  servo2.attach(D3);

}

void loop() {

  fromFirebase();
  from_app();
  int i = 1;
  int t = 1;
  if (last_command != command || last_app != app) { //當指紋辨識或app門禁系統狀況發生改變
    if ( command == "5" || app == "1" ) { 
      Serial.println("open");
      
      servo1.write(140);
      servo2.write(140);
      delay(1000);
      
      if (Firebase.setString(firebaseData, path + "/gate" , "1")) {  //將Firebase的gate設為開啟狀態
        Serial.println("gate open");
      }
      else
      {
        Serial.println("gate FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
      
      if (Firebase.setString(firebaseData, path + "/gate_app" , "1")) {  //將Firebase的gate_app設為開啟狀態
        delay(0);
      }
      else
      {
        Serial.println("gate FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
      delay(6000);
     
      servo1.write(0);
      servo2.write(0);  //開啟7秒後門自動關閉
     
      if (Firebase.setString(firebaseData, path + "/gate" , "0")) {  //當門開啟7秒後將Firebase的gate設為關閉狀態
        Serial.println("gate close");
      }
      else
      {
        Serial.println("gate FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
      
      if (Firebase.setString(firebaseData, path + "/gate_app" , "0")) {  //當門開啟7秒將Firebase的gate_app設為關閉狀態
        delay(0);
      }
      else
      {
        Serial.println("gate FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }

    }
    else {
      if (Firebase.setString(firebaseData, path + "/gate", "0")) {  //預設將Firebase的gate設為關閉狀態
        Serial.println("gate close");
      }
      else
      {
        Serial.println("gate FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
      
      if (Firebase.setString(firebaseData, path + "/gate_app", "0")) {  //預設將Firebase的gate_app設為關閉狀態
        Serial.println("gate close");
      }
      else
      {
        Serial.println("gate FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
    }
  }
  last_command = command; //記錄當下門的指紋模組辨識狀況
  last_app = app; //記錄當下app門禁系統狀況
}

void printJsonObjectContent(FirebaseData &data) {
  size_t tokenCount = data.jsonObject().parse(false).getJsonObjectIteratorCount();
  String key;
  String value;
  FirebaseJsonObject jsonParseResult;
  Serial.println();
  for (size_t i = 0; i < tokenCount; i++)
  {
    data.jsonObject().jsonObjectiterator(i, key, value);
    jsonParseResult = data.jsonObject().parseResult();
    Serial.print("KEY: ");
    Serial.print(key);
    Serial.print(", ");
    Serial.print("VALUE: ");
    Serial.print(value);
    Serial.print(", ");
    Serial.print("TYPE: ");
    Serial.println(jsonParseResult.type);

  }
}




void fromFirebase() {
  String path = "/house";
  FirebaseJson json;
  if ( Firebase.getString( firebaseData , path + "/lcdprint" ) ) {  //讀取Firebase上指紋辨識的狀況
    Serial.print("lcdprint: ");
    Serial.println( firebaseData.stringData() );
    command = firebaseData.stringData();
    if ( last != command ) {  //確認指令是否改變
      finger.println(command);
      Serial.println("passed");
    }
  }
  last = command;
}

void from_app() { //取app門禁系統指令

  String path = "/house";
  FirebaseJson json;
  if ( Firebase.getString( firebaseData , path + "/gate_app" ) ) {
    Serial.print("gate_app: ");
    Serial.println( firebaseData.stringData() );
    app = firebaseData.stringData();
  }
}
//void opendoor() {
//
//  if (finger.available()) {
//    char dr = finger.read();
//    door.concat(dr);
//    Serial.print(door);
//  }
//  last_door = door;
//}

//void read_id() {
//  String ID = "";
//  if (finger.available())
//  {
//    char id = finger.read();
//
//    ID.concat(id);
//    Serial.print(ID);
//
//  }
//
//  if (ID != "") {
//    if (Firebase.setInt(firebaseData, path + "/finger_id" , ID.toInt()) )
//    {
//      Serial.println("ID PASSED");
//    }
//    else
//    {
//      Serial.println("FAILED");
//      Serial.println("REASON: " + firebaseData.errorReason());
//      Serial.println("------------------------------------");
//      Serial.println();
//    }
//  }
//}
