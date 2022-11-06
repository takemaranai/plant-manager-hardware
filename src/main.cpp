#include <Wifi.h>
#include "config.hpp" //configにプライベートな情報は記載

const int VOUTPING = 35;
const int VOLT = 3.3; // 3.3Vを電源とした場合
const int ANALOG_MAX = 4096; // ESP32の場合
const int WATERVALUE = 969;
const int AIRVALUE = 2650;

bool tweetMsg(String msg);
String getSensorParam();
static int counter = 0;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  counter = 0;
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop() {
  //1分に1回データ取得
  String message = "ESP32からのツイート(StewGateを利用)\n";
  message += getSensorParam();

  // 10分に一回 tweetしたい
  if(counter >= 3600){
    boolean can_tweet = tweetMsg(message);
    Serial.print("already tweet?");
    Serial.println(can_tweet);
    counter = 0;
  }else{
    counter++;
  }
  delay(1000); //1秒 単位:ms
}

String getSensorParam(){
  String message = "";
  int sensor_param = analogRead(VOUTPING);
  float Moisture = 100 - (100 * (sensor_param - WATERVALUE )/(AIRVALUE - WATERVALUE));
  Serial.print("rawdata : ");
  Serial.print(sensor_param);
  Serial.print(" moisture : ");
  Serial.print(Moisture);
  Serial.print("% \n");

  message += "rawdata : ";
  message += String(sensor_param);
  message += String(" moisture : ");
  message += String(Moisture);
  message += String("% \n");
  return message;
}

bool tweetMsg(String msg) {
  WiFiClient client;
  const int httpPort = 80;
  if(!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return false;
  }

  client.println("POST /api/post/ HTTP/1.0");

  client.print("Host: ");
  client.println(host);

  // メッセージ以外の長さ（トークン含む）にメッセージ長を加算
  int msgLength = 40;
  msgLength += msg.length();
  client.print("Content-length:");
  client.println(msgLength);
  client.println("");

  client.print("_t=");
  client.print(token);
  client.print("&msg=");
  client.println(msg);

  delay(10);

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  return true;
}