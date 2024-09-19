#include <NimBLEDevice.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Define MQTT server settings
const char* mqtt_server = "139.59.246.168";
const int mqtt_port = 1883;
const char* mqtt_user = "qsentinel";
const char* mqtt_password = "12345678";
const char* mqtt_topic = "test/topic";

// Define WiFi settings
const char* ssid = "SLT Fiber ";
const char* wifi_password = "20030710";

WiFiClient espClient;
PubSubClient client(espClient);

// Define BLE service and characteristic UUIDs
#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "12345678-1234-5678-1234-56789abcdef1"

// BLE Client
NimBLEClient *pClient = NULL;
NimBLECharacteristic *pCharacteristic = NULL;
bool connected = false;

class MyClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) {
        connected = true;
        Serial.println("Connected to server");
    }

    void onDisconnect(NimBLEClient* pClient) {
        connected = false;
        Serial.println("Disconnected from server");
    }

    void onNotify(NimBLECharacteristic *pChr) {
        Serial.println("i");
        String data = pChr->getValue();
        Serial.println("Received data: " + data);

        // Parse and process the received data here
        // For example, you can extract temperature, humidity, and time
        float temperature = data.substring(0, data.indexOf(',')).toFloat();
        float humidity = data.substring(data.indexOf(',') + 1, data.lastIndexOf(',')).toFloat();
        int currentTime = data.substring(data.lastIndexOf(',') + 1).toInt();

        // Use the parsed data as needed
        Serial.println("Temperature: " + String(temperature));
        Serial.println("Humidity: " + String(humidity));
        Serial.println("Current Time: " + String(currentTime));
    }
};

void setup() {
    Serial.begin(115200);
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Set up MQTT
  client.setServer(mqtt_server, mqtt_port);

  // Attempt to connect to MQTT
  reconnect();
    // Initialize BLE
    NimBLEDevice::init("ESP32_Receiver");

    // Create BLE client
    pClient = NimBLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallbacks());

    // Connect to the server
    NimBLEAddress serverAddress("B0:B2:1C:B1:D2:A6"); // Replace with the server's MAC address

    if (pClient->connect(serverAddress)) {
        Serial.println("Connected to server");

        // Wait for a short period before discovering services
        delay(1000); // Wait for 1 second

        // Discover the service and characteristic
        NimBLERemoteService *pService = pClient->getService(SERVICE_UUID);
        if (pService) {
            NimBLERemoteCharacteristic *pCharacteristic = pService->getCharacteristic(CHARACTERISTIC_UUID);
            if (pCharacteristic) {
                // Subscribe to notifications
                if (pCharacteristic->subscribe(true, notifyCB)) {
                    Serial.println("Subscribed to notifications");
                } else {
                    Serial.println("Failed to subscribe to notifications");
                }
            } else {
                Serial.println("Characteristic not found");
            }
        } else {
            Serial.println("Service not found");
        }
    }
}
// Define notifyCB function
void notifyCB(NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {

std::string str = (isNotify == true) ? "Notification" : "Indication";
    str += " from ";
    /** NimBLEAddress and NimBLEUUID have std::string operators */
    str += std::string(pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress());
    str += ": Service = " + std::string(pRemoteCharacteristic->getRemoteService()->getUUID());
    str += ", Characteristic = " + std::string(pRemoteCharacteristic->getUUID());
    str += ", Value = " + std::string((char*)pData);
    Serial.println(str.c_str());
    Serial.println(String((char*) pData, length));
    String data = String((char*) pData, length);
     // Extract temperature
  int commaIndex1 = data.indexOf(',');        // Find the first comma
  float temperature = data.substring(0, commaIndex1).toFloat();  // Convert to float

  // Extract humidity
  int commaIndex2 = data.indexOf(',', commaIndex1 + 1);  // Find the second comma
  float humidity = data.substring(commaIndex1 + 1, commaIndex2).toFloat();  // Convert to float

  // Extract time
  int timeValue = data.substring(commaIndex2 + 1).toInt();  // Convert to integer

  // Print the extracted values
  Serial.print("Temperature: ");
  Serial.println(temperature);

  Serial.print("Humidity: ");
  Serial.println(humidity);

  Serial.print("Time: ");
  Serial.println(timeValue);
    if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publish a message
  String message = data;
  if (client.connected()) {
    client.publish(mqtt_topic, message.c_str());
    // Wait for 5 seconds before publishing the next message
  }
    // Example: Use the data to update a display or perform other actions
    // ...
}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Subscribe to a topic if needed
      // client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop() {
    // Handle client connections and notifications
    // You might want to add a delay or other logic here
}