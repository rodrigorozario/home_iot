
/* DHT22 temperature and humidity data are uploaded to thingspeak using
 * WeMOS ESP8266 MCU.
 * 
 * Details: http://embedded-lab.com/blog/iot-temperature-and-humidity-logger/
 */
#include <ESP8266WiFi.h>
#include <DHT.h>
#include "Math.h"
#define DHTTYPE DHT22
#define DHTPIN  D3   // DHT22 data pin connects to D4

// Time to sleep (in seconds):
const int sleepTimeS = 60;
int period = 60000;
unsigned long time_now = 0;

DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266


float prevTemp = 0;
const char* server = "184.106.153.149";//"api.thingspeak.com";
String apiKey ="";
const char* MY_SSID = ""; 
const char* MY_PWD = "";
int sent = 0;
void setup() {
  Serial.begin(115200);
  dht.begin();
  connectWifi();
  time_now = millis();
  
}

void loop() {
  //Serial.println(millis()-time_now);
  
  if(millis() - time_now > period){
    time_now = millis();
    Serial.print("Time now: ");
    Serial.println(time_now);
    float t1,humidity;
    //char buffer[10];
    humidity = dht.readHumidity();          // Read humidity (percent)
    t1 = dht.readTemperature(false);     // Read temperature as Celcius
      // Check if any reads failed and exit early (to try again).
    
    if (isnan(humidity) || isnan(t1)) {
       Serial.println("Failed to read from DHT sensor!");
        return;
    }
    
    
    Serial.print(String(sent)+" Temperature: ");
    Serial.println(t1);
    Serial.print(String(sent)+" Humidity: ");
    Serial.println(humidity);
    sendTeperatureTS(t1, humidity);
    //ESP.deepSleep(sleepTimeS*1000000, WAKE_RF_DEFAULT); // Sleep for 60 seconds

  }
}

void connectWifi()
{
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  
}//end connect

void sendTeperatureTS(float temp1, float temp2)
{  
   WiFiClient client;
  
   if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("WiFi Client connected ");
   
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(temp1);
   postStr += "&field2=";
   postStr += String(temp2);
   postStr += "\r\n\r\n";

   Serial.println(postStr);
   
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   delay(1000);
   
   }else{
    Serial.println("Client not connected...");
   }
   
   //end if
   sent++;
 client.stop();
}//end send
