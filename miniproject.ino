#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

int LED[3] = {5, 18, 22};
int touch[3] = {27, 32, 33}; //purple = 1, green = 2, orange = 3
bool t[3] = {false, false, false};
int check[3] = {0, 0, 0};
int room[3];
String txt[2] = {" is used.", " is available."};

char str[50];
const char* ssid = "ZisZtR";
const char* password = "asd123456";
const char* put_url = "https://ecourse.cpe.ku.ac.th/exceed07/api/toilet";
const int _size = 2*JSON_OBJECT_SIZE(2);
StaticJsonDocument<_size> JSONPut;

void WiFi_Connect(){
  WiFi.disconnect();
  delay(2000);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print("...");  
  }
  Serial.println("Connected to network"); 
  Serial.print("IP Adress : "); 
  Serial.println(WiFi.localIP());   
}

void _put(int room){
  if(WiFi.status() == WL_CONNECTED){
    int state = digitalRead(LED[room-1]);
    HTTPClient http;
    http.begin(put_url);
    http.addHeader("Content-Type", "application/json");

    JSONPut["room"] = room;
    if(room==1) JSONPut["available"] = state;
    else JSONPut["available"] = !state;
    
    serializeJson(JSONPut, str);
    int httpCode = http.PUT(str);
    if(httpCode == HTTP_CODE_OK){
      String payload = http.getString();
      Serial.print("Status: ");  
      Serial.println(httpCode);
      Serial.println(payload); 
    }else{
      Serial.print("Status: "); 
      Serial.println(httpCode);
      Serial.println("ERROR on HTTP request");        
    }
  }else{
    WiFi_Connect();  
  }
  delay(100);  
}

void setup() {
  for(int i=0;i<3;i++){
     pinMode(LED[i], OUTPUT);
     pinMode(touch[i], INPUT);
     if(i==0) digitalWrite(LED[i], LOW);
     else digitalWrite(LED[i], HIGH);
  }
  Serial.begin(9600);
  WiFi_Connect();
  delay(1000);
}

void loop() {
  while(1){
    for(int i=0;i<3;i++){
      room[i] = touchRead(touch[i]);
      /*Serial.print("room");
      Serial.print(i+1);
      Serial.print(" : ");
      Serial.println(room[i]);*/
    }

    for(int i=0;i<3;i++){
      if(t[i] && room[i] > 50 && check[i]>2){
        t[i] = false;
        digitalWrite(LED[i], !digitalRead(LED[i]));
        Serial.print("toilet ");
        Serial.print(i+1);
        if(i!=0) Serial.println(txt[digitalRead(LED[i])]); 
        else Serial.println(txt[!digitalRead(LED[i])]);
        _put(i+1);
      }
      if(room[i] < 25 && room[i]!=0){
        t[i]=true;
        check[i]++;
      }
      if(room[i] > 50) check[i]=0;
    }
  }
  delay(500);
}
