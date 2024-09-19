#include <NimBLEDevice.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
// Define BLE service and characteristic UUIDs
// #define SERVICE_UUID  "12345678-1234-5678-1234-56789abcdef0"
// #define CHARACTERISTIC_UUID "12345678-1234-5678-1234-56789abcdef1
#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "12345678-1234-5678-1234-56789abcdef1"
const char *ssid = "SLT Fiber ";
const char *password = "20030710";
// BLE Server
NimBLEServer *pServer = NULL;
NimBLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

class MyServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer)  {
    deviceConnected = true;
    Serial.println("Client connected");
  }

  void onDisconnect(NimBLEServer* pServer)  {
    deviceConnected = false;
    Serial.println("Client disconnected");
  }
};
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org");
void setup() {
  Serial.begin(115200);
     WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    timeClient.begin();
    timeClient.setTimeOffset(0);
  // Initialize BLE
  NimBLEDevice::init("ESP32_Sender");
  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE service and characteristic
  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      NIMBLE_PROPERTY::NOTIFY
                    );
  pCharacteristic->setValue("Hello World");
  pService->start();

  // Start advertising
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();
  Serial.println("Waiting for a client connection to notify...");
}

void loop() {
timeClient.update();
    float temperature = 22.5;
    float humidity = 80;
    
    int currentTime = timeClient.getEpochTime();; // example time data
    Serial.println(currentTime);
    String data = String(temperature) + "," + String(humidity) + "," + String(currentTime);

    String valueString = "10";
pCharacteristic->setValue(data);
pCharacteristic->notify();
  // Notify every 2 seconds

  // String numberStr = String(NimBLEDevice::getAddress());



  
  // char macAddress[18];
  // uint64_t number = NimBLEDevice::getAddress();
  // // Extract each byte and format it as a MAC address
  // snprintf(macAddress, sizeof(macAddress), "%02X:%02X:%02X:%02X:%02X:%02X", 
  //           (uint8_t)(number >> 40), 
  //           (uint8_t)(number >> 32), 
  //           (uint8_t)(number >> 24), 
  //           (uint8_t)(number >> 16), 
  //           (uint8_t)(number >> 8), 
  //           (uint8_t)(number));

  // // Print the formatted MAC address
  // Serial.println(macAddress);
  delay(5000);
}
