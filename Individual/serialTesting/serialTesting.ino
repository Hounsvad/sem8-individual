//#include <WiFiClient.h>
//#include <WiFiMulti.h>
//#include <WiFiAP.h>
//#include <WiFiScan.h>
//#include <WiFiServer.h>
//#include <WiFiGeneric.h>
//#include <WiFi.h>
//#include <WiFiType.h>
//#include <WiFiUdp.h>
//#include <WiFiSTA.h>



//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"
#include <ArduinoNvs.h>
#include <WiFi.h>
#include <cstring>
#include <esp_log.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;



struct wifi_cred_t {
  String ssid;
  String password;
};

wifi_cred_t getWifiCredentialsFromStorage();
wifi_cred_t requestWifiCredentials();
void saveWifiCredentialsToStorage(wifi_cred_t *credentials);
bool scanForSelectedNetwork(String *selectedNetwork);
bool btWrite(String message);
String btRead();

void setup() {
  Serial.begin(115200);
  ESP_LOGI("TestLog", "test start");
  SerialBT.begin("SEM8ESP");
  WiFi.mode(WIFI_STA);
  btStart();
  while(!Serial);
  NVS.begin();
  Serial.println("ESP - Started");
  while(WiFi.status() != WL_CONNECTED){
    Serial.println("Wifi not connected");
    Serial.println("Getting creds from storage");
    wifi_cred_t creds = getWifiCredentialsFromStorage();
    Serial.println("Creds retrieved");
    if(creds.ssid.length() < 1){
      Serial.println("Creds were empty - Retrieving new creds");
      creds = requestWifiCredentials();
      Serial.println("Saving creds");
      saveWifiCredentialsToStorage(&creds);
    }
    Serial.println("Scan for networks");
    //Scan for selected wifi
    while(!scanForSelectedNetwork(&creds.ssid)){
      Serial.println("Network not found re-requesting creds");
      creds = requestWifiCredentials();
      Serial.println("Creds returned. SSID: " + creds.ssid + ", Pass: " + creds.password);
    }
    Serial.println("Network found");
    //Connect to the network
    WiFi.disconnect();
    delay(100);
    Serial.println("Connecting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(creds.ssid.c_str(), creds.password.c_str());
    int WiFiRetrySeconds = 30;
    int WifiRetriedSeconds = 0;
    while(WifiRetriedSeconds < WiFiRetrySeconds){
      if(WiFi.status() == WL_CONNECTED) 
        break;
      Serial.println("Wifi Not Connected, waiting for connection");
      Serial.println("Status: " + String(WiFi.status()));
      delay(1000);
      WifiRetriedSeconds++;
    }
  }
  Serial.println("Setup done");
}

bool scanForSelectedNetwork(String *selectedNetwork) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int networkCount = WiFi.scanNetworks();
  if(networkCount == 0){
    Serial.println("No networks discovered");
    return false;
  }
  Serial.println("Networks found: " + String(networkCount));
  for(int i = 0; i<networkCount;i++){
    String curr = WiFi.SSID(i);
    if(curr.equalsIgnoreCase(*selectedNetwork)){
      return true;
    }
  }
  return false;
}

wifi_cred_t requestWifiCredentials() {
  String get_ssid_command = "getSSID\n";
  String get_pass_command = "getPASS\n";
  
  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(100);

  wifi_cred_t cred;  
  while(!SerialBT.connected(10000)){
    Serial.println("BT not connected waiting");
    delay(100);
  }
  
  
  btWrite(get_ssid_command);
  btRead();//fix for rfcomm behaving strangely echoing what is read from it
  cred.ssid = btRead();  
  
  btWrite(get_pass_command);
  btRead();//fix for rfcomm behaving strangely echoing what is read from it
  cred.password = btRead();
  SerialBT.disconnect();
  return cred;
}

bool btWrite(String message){
  while(!SerialBT.connected());
  int bufferSize = message.length()+1;
  uint8_t buffer[bufferSize];
  char charBuffer[bufferSize];

  message.toCharArray(charBuffer, bufferSize);
  memcpy(buffer, charBuffer, bufferSize);
  SerialBT.write(buffer, bufferSize);
  SerialBT.flush();
  return true;
}

String btRead(){
  String buffer = "";
  buffer.reserve(100);
  int recievedChars;
  uint8_t recievedChar;
  while(!SerialBT.available()) 
    delay(50);
  while(SerialBT.peek() != -1){
    recievedChar = SerialBT.read();
    if(recievedChar == 13){//Remove newline
      break;
    }
    buffer+=String((char)recievedChar);
    //ESP_LOGI("DEBUG","%s %i",(char)recievedChar,(int)recievedChar);
  }
  while(SerialBT.peek() != -1){
    SerialBT.read();
  }
  SerialBT.flush();
  return buffer;
}

wifi_cred_t getWifiCredentialsFromStorage() {
  wifi_cred_t returnCreds = {
    .ssid = NVS.getString("WifiSSID"),
    .password = NVS.getString("WifiPassword"),
  };
  return returnCreds;
}

void saveWifiCredentialsToStorage(wifi_cred_t *credentials) {
  NVS.setString("WifiSSID", (*credentials).ssid);
  NVS.setString("WifiSSID", (*credentials).ssid);
  return;
}

void loop() {
  Serial.println("Setup has been completed - commence normal function");
  delay(20000);
}
