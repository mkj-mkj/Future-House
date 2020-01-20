#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>
#include "FirebaseESP8266.h"
#include <Adafruit_Fingerprint.h>
SoftwareSerial mySerial(D2, D3);
String fingermode = "0";
String path = "/house";
int t = 1;
String character;
String content;
String stat;
int ID = 1;
// 設定網路基地台SSID跟密碼
const char* ssid      = "2019 TCA_CHT";
const char* password  = "20192019";
#define FIREBASE_HOST "https://smart-house-622bd.firebaseio.com/"
#define FIREBASE_AUTH "GOaVTdNwC2Pgos7nbYQ30niW68mbnl5QBv5RsHOX"
FirebaseData firebaseData;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

void printJsonObjectContent(FirebaseData &data);
void firebase();
void fingerid();
void enroll();
void fingerprint();
uint8_t getFingerprintEnroll();
int getFingerprintIDez();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  finger.begin(57600);
  delay(10);

  Serial.println("");
  Serial.println("");
  Serial.print("Connecting To: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // 等待連線，並從 Console顯示 IP
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //設定firebase的路徑、金鑰
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  Firebase.enableClassicRequest(firebaseData, true);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");
  
  //檢查是否有接上指紋感測器
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }
  
  //傳送"8"到firebase的lcdprint(關門)
  if (Firebase.setString(firebaseData, path + "/lcdprint" , "8")) {
    Serial.println("remove fingerprint PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}


void loop() {
  
  FirebaseJson json;
  firebase();
  Serial.print("ID: ");
  Serial.println(ID); //顯示目前已註冊的指紋數
  Serial.print("mode= ");
  Serial.println(fingermode); //顯示目前指紋模組的模式

  if (fingermode == "1") {
    enroll();
    delay(100);
    firebase();
    if (Firebase.setString(firebaseData, path + "/fingerprint" , "0")) {
      Serial.println("fingerprint PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
    ID++;

  }


  else if (fingermode == "2") {

    getFingerprintIDez();
    delay(50);

    firebase();
  }

  else if (fingermode == "0") {
    delay(0);
    if (Firebase.setString(firebaseData, path + "/lcdprint" , "8")) {
    Serial.println("fingerprint PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
    firebase();
  }
  
  if (Firebase.setInt(firebaseData, path + "/finger_id", ID)) {
    delay(0);
  }
}


void firebase() {
  if (Firebase.getString(firebaseData, path + "/fingerprint")) {
    Serial.println(firebaseData.stringData());
    fingermode = firebaseData.stringData();
    Serial.println("------------------------------------");
    Serial.println();
    if (content == "1") {
      fingermode = "1";
    }
    else if (content == "2") {
      if (Firebase.setString(firebaseData, path + "/lcdprint" , "4")) {
        Serial.println("fingerprint PASSED");
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
      fingermode = "2";
    }
    //    else if (content == "3") {
    //      fingermode = "3";
    //    }
    else if (content == "0") {
      fingermode = "0";
    }
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }


}

//void fingerid() {
//  if (Firebase.getInt(firebaseData, path + "/finger_id")) {
//    int a = firebaseData.stringData().toInt();
//    Serial.print("a=");
//    Serial.println(a);
//    ID = a + 1;
//    Serial.print("ID=");
//    Serial.println(ID);
//    if (Firebase.setInt(firebaseData, path + "/finger_id", ID)) {
//      delay(0);
//    }
//  }
//
//}

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



//---------------------------enroll------------------------------------------------
uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(ID);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }
  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  if (Firebase.setString(firebaseData, path + "/lcdprint" , "1")) {
    Serial.println("remove fingerprint PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(ID);
  p = -1;
  Serial.println("Place same finger again");
  if (Firebase.setString(firebaseData, path + "/lcdprint" , "2")) {
    Serial.println("same fingerprint PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(ID);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(ID);
  p = finger.storeModel(ID);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;

    fingermode = "0";
  }
}
void enroll() {


  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  if (Firebase.setString(firebaseData, path + "/lcdprint" , "0")) {
    Serial.println("start enroll fingerprint PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  //fingerid();



  if (ID == 0)
  { // ID #0 not allowed, try again!
    return;
  }

  Serial.print("Enrolling ID #");
  Serial.println(ID);
  getFingerprintEnroll();

  if (Firebase.setString(firebaseData, path + "/lcdprint" , "3")) {
    Serial.println("finsih enroll fingerprint PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  delay(300);
  if (Firebase.setString(firebaseData, path + "/fingerprint" , "8")) {
    Serial.println("reset fingerprint PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();

  }
}

//---------------------------------------------------------------------------------




//-----------------------recognize-------------------------------------------------
uint8_t getFingerprintID() {
  if (Firebase.setString(firebaseData, path + "/lcdprint" , "4")) {
    Serial.println("recognize PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  if (Firebase.setString(firebaseData, path + "/lcdprint" , "5")) {
    Serial.println("welcome PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  delay(5000);
  if (Firebase.setString(firebaseData, path + "/lcdprint" , "8")) {
    Serial.println("reset PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  return finger.fingerID;
}
int getFingerprintIDez() {
  if (Firebase.setString(firebaseData, path + "/lcdprint" , "4")) {
    Serial.println("recognize PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  if (Firebase.setString(firebaseData, path + "/lcdprint" , "5")) {
    Serial.println("welcome PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  delay(5000);
  if (Firebase.setString(firebaseData, path + "/lcdprint" , "8")) {
    Serial.println("reset PASSED");
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  return finger.fingerID;
}
void fingerprint() {

  getFingerprintIDez();
  delay(50);

}
//---------------------------------------------------------------------------------




////----------------------------------delete-----------------------------------------
//uint8_t deleteFingerprint(uint8_t id) {
//  uint8_t p = -1;
//
//  p = finger.deleteModel(id);
//
//  if (p == FINGERPRINT_OK) {
//    Serial.println("Deleted!");
//  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//    Serial.println("Communication error");
//    return p;
//  } else if (p == FINGERPRINT_BADLOCATION) {
//    Serial.println("Could not delete in that location");
//    return p;
//  } else if (p == FINGERPRINT_FLASHERR) {
//    Serial.println("Error writing to flash");
//    return p;
//  } else {
//    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
//    return p;
//  }
//}
//void deletefinger() {
//
//  Serial.println("Please type in the ID # (from 1 to 127) you want to delete...");
//  if (Firebase.setString(firebaseData, path + "/lcdprint" , "6")) {
//    Serial.println("fingerprint PASSED");
//  }
//  else {
//    Serial.println("FAILED");
//    Serial.println("REASON: " + firebaseData.errorReason());
//    Serial.println("------------------------------------");
//    Serial.println();
//  }
//  uint8_t id = readnumber();
//  if (id == 0)
//  { // ID #0 not allowed, try again!
//    return;
//  }
//  Serial.print("Deleting ID #");
//  Serial.println(id);
//  deleteFingerprint(id);
//  if (Firebase.setString(firebaseData, path + "/lcdprint" , "7")) {
//    Serial.println("fingerprint PASSED");
//  }
//  else {
//    Serial.println("FAILED");
//    Serial.println("REASON: " + firebaseData.errorReason());
//    Serial.println("------------------------------------");
//    Serial.println();
//  }
//  fingermode = "0";
//
//  delay(3000);
//  if (Firebase.setString(firebaseData, path + "/fingerprint" , "8")) {
//    Serial.println("fingerprint PASSED");
//  }
//  else {
//    Serial.println("FAILED");
//    Serial.println("REASON: " + firebaseData.errorReason());
//    Serial.println("------------------------------------");
//    Serial.println();
//  }
//}
////---------------------------------------------------------------------------------
