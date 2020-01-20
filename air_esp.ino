#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

SoftwareSerial NodeMCU(D7, D8);

#define FIREBASE_HOST "smart-house-622bd.firebaseio.com"  
#define FIREBASE_AUTH "GOaVTdNwC2Pgos7nbYQ30niW68mbnl5QBv5RsHOX"
#define WIFI_SSID "2019 TCA_CHT"
#define WIFI_PASSWORD "20192019"
float value;

String co;
String haze;
String gas;
const char* apiKey = "TETWR4CQ24MAMP4O";
const char* resource = "/update?api_key=";
int t = 0;
int c = 0;
int g = 0;
int h = 0;
// Thing Speak API server
const char* server = "api.thingspeak.com";

FirebaseData firebaseData;

void printJsonObjectContent(FirebaseData &data);

void setup() {
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //Firebase的路徑跟金鑰
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60); 
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  Serial.begin(9600);
  NodeMCU.begin(115200);
  delay(10);

  pinMode(D7, INPUT);

  pinMode(D8, OUTPUT);

  Serial.println("");
  Serial.println("");
  Serial.print("Connecting To: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //設定連線ap端的ID、PASSWORD

  // 等待連線，並從 Console顯示 IP
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    delay(0);
  }

  Serial.println("");
  Serial.println("WiFi Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(0);

}



void loop() {
  
  String path = "/house"; //資料庫分頁名
  FirebaseJson json;
  String content = "";
  char character;
  
  //讀取UNO板傳送的數值
  while (NodeMCU.available()) { 
    character = NodeMCU.read();
    content.concat(character);
    Serial.println(character);
    delay(0);
  }

  String a = content.substring(0, 1); //以讀取字串中的第一個字元辨識是何種氣體的數據
  
  value = content.substring(2, 6).toFloat();  //取讀取字串中第2個到第5個字元作為氣體的數據，並將字串轉換成浮點數

  //當氣體數值超標，即控制電扇及窗戶開啟
  if (value > 300) {
    if (Firebase.setString(firebaseData, path + "window", "1")) {
      Serial.println("window open");
    }
    if (Firebase.setString(firebaseData, path + "fan", "1")) {
      Serial.println("fan open");
    }
  }

  //若接收字串的第一個字元為A，則將其視為MQ2的數值
  if (a == "A")
  {
    Serial.println(content);
    //傳送MQ2的數值至firebase
    if (Firebase.setFloat(firebaseData, path + "/MQ2" , content.substring(2, 6).toFloat()) )
    {
      gas = content.substring(2, 6);
      Serial.println("MQ2 PASSED");
      Serial.print("Connecting to ");
      Serial.print(server);
      
      //每過5秒，將數據傳送至thingspeak
      if (g == 50)
      {
        WiFiClient client;

        // 使用 80 Port 連線
        if (client.connect(server, 80)) {
          Serial.println(F("connected"));
        }
        else  {
          Serial.println(F("connection failed"));
          return;
        }

        Serial.print("Request resource: ");
        Serial.println(resource);
        //將數值傳送至field2
        client.print(String("GET ") + resource + apiKey + "&field2=" + (content.substring(2, 6)) +
                     " HTTP/1.1\r\n" +
                     "Host: " + server + "\r\n" +
                     "Connection: close\r\n\r\n");

        int timeout = 5 * 10; // 5 seconds
        while (!!!client.available() && (timeout-- > 0)) {
          delay(100);
        }

        if (!client.available()) {
          Serial.println("No response, going back to sleep");
        }
        while (client.available()) {
          Serial.write(client.read());
        }

        Serial.println("\nclosing connection");
        client.stop();
        g = 0;  //重製秒數
      }


    }
    else
    {
      Serial.println("MQ2 FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }


  }
  if (a == "B")
  {
    Serial.println(content);
    if (Firebase.setFloat(firebaseData, path + "/MQ9" , content.substring(2, 6).toFloat()) )
    {
      co = content.substring(2, 6);
      Serial.println("MQ9 PASSED");
      Serial.print("Connecting to ");
      Serial.print(server);
      if (c == 50)
      {
        WiFiClient client;

        // 使用 80 Port 連線
        if (client.connect(server, 80)) {
          Serial.println(F("connected"));
        }
        else  {
          Serial.println(F("connection failed"));
          return;
        }

        Serial.print("Request resource: ");
        Serial.println(resource);

        client.print(String("GET ") + resource + apiKey + "&field1=" + (content.substring(2, 6)) +
                     " HTTP/1.1\r\n" +
                     "Host: " + server + "\r\n" +
                     "Connection: close\r\n\r\n");

        int timeout = 5 * 10; // 5 seconds
        while (!!!client.available() && (timeout-- > 0)) {
          delay(100);
        }

        if (!client.available()) {
          Serial.println("No response, going back to sleep");
        }
        while (client.available()) {
          Serial.write(client.read());
        }

        Serial.println("\nclosing connection");
        client.stop();
        g = 0;
      }
    }
    else
    {
      Serial.println("MQ9 FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }
  if (a == "C")
  {
    Serial.println(content);
    if (Firebase.setFloat(firebaseData, path + "/MQ135" , content.substring(4, 8).toFloat()) )
    {
      Serial.println("MQ135 PASSED");
    }
    else
    {
      Serial.println("MQ135 FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }
  if (a == "D")
  {
    Serial.println(content);
    if (Firebase.setFloat(firebaseData, path + "/MQ3" , content.substring(2, 6).toFloat()) )
    {
      haze = content.substring(2, 6);
      Serial.println("MQ3 PASSED");
      Serial.print("Connecting to ");
      Serial.print(server);
      if (h == 50)
      {
        WiFiClient client;

        // 使用 80 Port 連線
        if (client.connect(server, 80)) {
          Serial.println(F("connected"));
        }
        else  {
          Serial.println(F("connection failed"));
          return;
        }

        Serial.print("Request resource: ");
        Serial.println(resource);

        client.print(String("GET ") + resource + apiKey + "&field3=" + (content.substring(2, 6)) +
                     " HTTP/1.1\r\n" +
                     "Host: " + server + "\r\n" +
                     "Connection: close\r\n\r\n");

        int timeout = 5 * 10; // 5 seconds
        while (!!!client.available() && (timeout-- > 0)) {
          delay(100);
        }

        if (!client.available()) {
          Serial.println("No response, going back to sleep");
        }
        while (client.available()) {
          Serial.write(client.read());
        }

        Serial.println("\nclosing connection");
        client.stop();
        h = 0;
      }
    }
    else
    {
      Serial.println("MQ3 FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }



  c++;
  g++;
  h++;
  Serial.print("co: ");
  Serial.println(co);
  Serial.print("haze: ");
  Serial.println(haze);
  Serial.print("gas: ");
  Serial.println(gas);
  delay(500);
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
