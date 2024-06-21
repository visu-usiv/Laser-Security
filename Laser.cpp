/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>
#include "DHT.h"

#define DHTPIN 14    // what digital pin we're connected to

#define laser_sensor 16




#define buzzer 2

#define DHTTYPE DHT11   // DHT 11

ESP8266WiFiMulti WiFiMulti;
String s="";

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(9600);
  // Serial.setDebugOutput(true);
  Serial.println();

 pinMode(buzzer,OUTPUT);
 digitalWrite(buzzer,0);
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("laser", "laser123");
   Serial.println("DHTxx test!");

  dht.begin();

  pinMode(laser_sensor,INPUT);

    pinMode(buzzer,OUTPUT);
    for(int i=0;i<10;i++)
    {
      digitalWrite(buzzer,1);
      delay(200);
      digitalWrite(buzzer,0);
        delay(200);
    
    }
}

long times = 0;
byte Available = 0;

void loop() {

  
delay(100);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    //return;
  }
  Serial.print("sensor Staus= ");
  Serial.print(digitalRead(laser_sensor));
  if(digitalRead(laser_sensor)==1)
  {
     digitalWrite(buzzer,1);
  }
  else
  {
      digitalWrite(buzzer,0);
  }


 

      delay(10);
      s = "http://api.thingspeak.com/update?api_key=ETDK6SVA8RHXDTX5&field1=";
      s += h;
      s += "&field2=";
      s += t;
      s += "&field3=";
      s += String(digitalRead(laser_sensor));
      s += "&field4=";
      s += String(digitalRead(buzzer));
        
      
      Serial.println(s);
      Available = 1;
  

  if(Available == 1 & millis()>times+16000)
  {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    //String s ="http://api.thingspeak.com/update?api_key=UJTBX6XN3FUB6WVR&field1=10&field2=11&field3=12&field4=13&field5=14";
    int len = s.length();
    char c[len+5];
    s.toCharArray(c, len+1);
    Serial.println(s);
    if (http.begin(client, c)) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);      
          Available = 0;
          times = millis();
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
  }

}