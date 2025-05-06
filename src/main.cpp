#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BLEDevice.h>
#include <BLEServer.h>

// URLs for API endpoints
String URL1 = "http://proiectia.bogdanflorea.ro/api/the-wolf-among-us/characters";
String URL2 = "http://proiectia.bogdanflorea.ro/api/the-wolf-among-us/character?id=";

// BLE server, service, and characteristic UUIDs
#define SERVER "API_Fetcher"
#define SERVICE "1b2e4970-7e52-43ed-93ee-7dd01880e8b3"
#define CHARACTERISTIC "bd104210-8851-42d2-a610-3548f79dad88"

// Connect to a WiFi network
bool connect(String ssid, String password)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid.c_str(), password.c_str());
  int startConnection = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    if (millis() - startConnection > 10000)
      return false;
  }
  return true;
}

bool connected;
class ServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer * pServer)
  {
    connected = true;
    Serial.println("ESP connected to PIA app.");
  };

  void onDisconnect(BLEServer * pServer)
  {
    connected = false;
    Serial.println("ESP disconnected from PIA app.");
  }
};

// Handle BLE characteristic callbacks
class CharacteristicCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *characteristic)
  {
    String response;
    std::string jsonData = characteristic->getValue();
    Serial.println(jsonData.c_str());
    JsonDocument jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, jsonData.c_str());
    String action = jsonDoc["action"].as<String>();

    if (action == "getNetworks")
    {
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(100);
      int networksFound = WiFi.scanNetworks();
      for (int i = 0; i < networksFound; ++i)
      {
        JsonDocument network;
        network["ssid"] = WiFi.SSID(i);
        network["strength"] = WiFi.RSSI(i);
        network["encryption"] = WiFi.encryptionType(i);
        network["teamId"] = "A35";
        serializeJson(network, response);
        characteristic->setValue(response.c_str());
        characteristic->notify();
      }
    }
    else if (action == "connect")
    {
      String ssid = jsonDoc["ssid"].as<String>();
      String password = jsonDoc["password"].as<String>();
      bool connectedToWiFi = connect(ssid, password);
      JsonDocument network;
      network["teamId"] = "A35";
      network["ssid"] = ssid;
      connectedToWiFi ? network["connected"] = true : network["connected"] = false;
      serializeJson(network, response);
      characteristic->setValue(response.c_str());
      characteristic->notify();
    }
    else if (action == "getData")
    {
      HTTPClient http;
      http.begin(URL1);
      http.setConnectTimeout(15000);
      http.setTimeout(15000);
      int httpResponseCode = http.GET();
      String payload = http.getString();
      JsonDocument requestDoc, responseDoc;
      deserializeJson(requestDoc, payload.c_str());
      JsonArray characters = requestDoc.as<JsonArray>();
      for (JsonVariant character : characters)
      {
        JsonObject listItem = character.as<JsonObject>();
        responseDoc["id"] = listItem["id"].as<String>();
        responseDoc["name"] = listItem["name"].as<String>();
        responseDoc["image"] = listItem["imagePath"].as<String>();
        responseDoc["teamId"] = "A35";                            
        serializeJson(responseDoc, response);                     
        characteristic->setValue(response.c_str());               
        characteristic->notify();                                 
      }
      http.end();
    }
    else if (action == "getDetails")
    {                                         
      String id = jsonDoc["id"].as<String>(); 
      HTTPClient http;
      http.begin(URL2 + id);             
      http.setConnectTimeout(15000);     
      http.setTimeout(15000);            
      int httpResponseCode = http.GET(); 
      String payload = http.getString(); 
      JsonDocument requestDoc, responseDoc;
      deserializeJson(requestDoc, payload.c_str());           
      responseDoc["name"] = requestDoc["name"];               
      responseDoc["image"] = requestDoc["imagePath"];         
      responseDoc["description"] = requestDoc["description"]; 
      responseDoc["teamId"] = "A35";                          
      serializeJson(responseDoc, response);                   
      Serial.println(response.c_str());
      characteristic->setValue(response.c_str());
      characteristic->notify();                  
      http.end();                                
    }
  }
};

void setup()
{
  Serial.begin(115200);

  BLEDevice::init(SERVER);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *bleService = pServer->createService(SERVICE);

  BLECharacteristic *characteristic = bleService->createCharacteristic
  (
    CHARACTERISTIC,
    BLECharacteristic::PROPERTY_READ | 
      BLECharacteristic::PROPERTY_WRITE | 
      BLECharacteristic::PROPERTY_NOTIFY
  );
  BLEDescriptor *characteristicDescriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2902));

  characteristic->addDescriptor(characteristicDescriptor);     
  characteristic->setCallbacks(new CharacteristicCallbacks()); 
  bleService->start();                                         

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising(); 
  pAdvertising->addServiceUUID(SERVICE);                      
  pAdvertising->start();                                      
}

void loop() {}
