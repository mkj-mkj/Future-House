#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>
#define FIREBASE_HOST "smart-house-622bd.firebaseio.com"
#define FIREBASE_AUTH "GOaVTdNwC2Pgos7nbYQ30niW68mbnl5QBv5RsHOX"
#define WIFI_SSID "2019 TCA_CHT"
#define WIFI_PASSWORD "20192019"

SoftwareSerial NodeMCU(D7, D8);

const char* apiKey = "YNDQLFJB3M83Q1GF";
const char* resource = "/update?api_key=";
int t = 0;
// Thing Speak API server
const char* server = "api.thingspeak.com";

//Define FirebaseESP8266 data object
FirebaseData firebaseData;

void printJsonObjectContent(FirebaseData &data);

void setup()
{
  Serial.begin(9600);
  NodeMCU.begin(115200);
  delay(10);

  pinMode(D7, INPUT);

  pinMode(D8, OUTPUT);

  Serial.println("");
  Serial.println("");
  Serial.print("Connecting To: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // 等待連線，並從 Console顯示 IP
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  /*
    This option allows get and delete functions (PUT and DELETE HTTP requests) works for device connected behind the
    Firewall that allows only GET and POST requests.

    Firebase.enableClassicRequest(firebaseData, true);
  */
}

void loop()
{

  String content = "";

  char character;

  while (NodeMCU.available()) {

    character = NodeMCU.read();

    content.concat(character);

  }
  Serial.println(content);
  delay(100);

  String path = "/house"; //資料庫分頁名

  FirebaseJson json;



  //傳送資料至Firebase從這裡開始

  if (content.toFloat() > 0) {
    if (Firebase.setFloat(firebaseData, path + "/PM" , content.toFloat()) )
    {
      Serial.println("PM PASSED");
    }


    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }
  //傳送ThingSpeak從這開始
  if ( t == 50)                   /*改成你要的條件*/
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

    //field1後括號填入pm2.5值

    client.print(String("GET ") + resource + apiKey + "&field1=" + content +
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
    t = 0;
  }
  t++;



}

//下面不知道要幹嘛 還沒研究 先加著不要刪掉
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
