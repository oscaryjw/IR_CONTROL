#ifndef UNIT_TEST
#include <Arduino.h> 
#endif
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
// for yeelink api
#define APIKEY         "******************" // replace your yeelink api key here
#define DEVICEID       361299 // replace your device ID
#define SENSORID_DATA      412551 // replace your sensor ID
// wait time
#define UPDATE_SECS2  10000//SEND DATA
#define UPDATE_SECS3  100//IR RECIVER
#define POWEROFF  10// ssPOWEROFF
// IR KEY defin
#define KEY1      0xFF18E7  
#define KEY2      0x89FE72BB  
#define KEY3      0x89FE72BA  
#define KEY4      0x5E7DB3DD  
#define KEY5      0x5E7DB3DC  
#define KEY6      0x5CA43345
#define KEY7      0xBE4F6EE6
#define KEY8      0xC04F7208
#define KEY9      0xE41400B8
#define KEY10      0xE41400B9
  
//IO
#define RECV_PIN   3 // GPIOx
#define SPEAKER   0  // GPIOx
#define RELAY      1  // GPIOx
//net config
const char* server = "api.yeelink.net"; //42.96.164.52
unsigned long lastConnectionTime1= 0;          // last time you connected to the server, in milliseconds  
unsigned long lastConnectionTime2 = 0;   
unsigned long lastConnectionTime3 = 0;   
boolean lastConnected = false;                 // state of the connection last time through the main loop
String returnValue="";
bool ResponseBegin;
const char* ssid = "v2";
const char* password = "623610468v17!";
WiFiClient client;
//IR INIt..
IRrecv irrecv(RECV_PIN);
decode_results results;
void setup() {
  WiFi.mode(WIFI_STA);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);// turn off 
    pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, HIGH);// turn off     
  WiFiClient client;
  irrecv.enableIRIn();  // Start the receiver
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  if (!client.connect(server, 80)) {
 //   Serial.println("connection failed");
    return;
  }
 speaker(1);
}

void loop() {
 if (!client.connected() && (millis()-lastConnectionTime2>UPDATE_SECS2)) {
  bool temp=digitalRead(RELAY);
  sendDatas(float(temp));//send data
 }  
 if (!client.connected() && lastConnected) {
    client.stop();
  }
  getIR();
}
void getIR(void){
    if (irrecv.decode(&results)) {
    serialPrintUint64(results.value, HEX);
    uint64_t tempstr=results.value;
  if(tempstr==KEY1||tempstr==KEY2||tempstr==KEY3||tempstr==KEY4||tempstr==KEY5||tempstr==KEY6||tempstr==KEY7||tempstr==KEY8|tempstr==KEY9||tempstr==KEY10)
   {//    Serial.println("KEY2");
          bool t=digitalRead(RELAY);
          if(!t){
          speaker(POWEROFF);
          digitalWrite(RELAY, !t);// turn RE 
          }else
          {
          digitalWrite(RELAY, !t);// turn RE 
          speaker(POWEROFF);
          }
   }
    irrecv.resume();  // Receive the next value
  }
  lastConnectionTime3=millis();
  }

void sendDatas(float thisData) {
  if (client.connect(server, 80)) {
    client.print("POST /v1.1/device/");
    client.print(DEVICEID);    
    client.print("/sensor/");
    client.print(SENSORID_DATA);
    client.print("/datapoints");
    client.println(" HTTP/1.1");
    client.println("Host: api.yeelink.net");
    client.print("Accept: *");
    client.print("/");
    client.println("*");
    client.print("U-ApiKey: ");
    client.println(APIKEY);
    client.print("Content-Length: ");
    int thisLength = 10 + getLength(thisData);
    client.println(thisLength);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection:close");
    client.println();
    client.print("{\"value\":");
    client.print(thisData);
    client.println("}");
  }
  else {
    client.stop();
  }
  lastConnectionTime2=millis();
}
int getLength(int someValue) {
  int digits = 1;
  int dividend = someValue / 10;
  while (dividend > 0) {
    dividend = dividend / 10;
    digits++;
  }
  digits = digits + 3;
  return digits;
}

void speaker(int t){
     digitalWrite(SPEAKER, LOW);  
     delay(t*1000);
     digitalWrite(SPEAKER, HIGH);  
  }
