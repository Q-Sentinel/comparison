#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <vector>
#include <Arduino.h>

#include "loten.h"

typedef struct hi {
    uint32_t seq_num;
    char topic[16];  // Adjust the size based on your topic length
    char keys[2][8]; // Assuming 2 keys with a max length of 8 characters each
    char values[2][16]; // Assuming 2 values with a max length of 16 characters each
    uint8_t data_types[2];
    uint32_t time;  // Unix time from NTP server
} hi;

const char *ssid = "SLT Fiber ";
const char *password = "20030710";

void userProcessReceivedData(const uint8_t* data, uint16_t length) {
    Serial.println("User defined received data:");
    if (length != sizeof(hi)) {
        Serial.println("Received data length does not match hi structure size");
        return;
    }
    hi receivedData;
    memcpy(&receivedData, data, sizeof(hi));

    Serial.print("Sequence Number: ");
    Serial.println(receivedData.seq_num);
    Serial.print("Time: ");
    Serial.println(receivedData.time);
    Serial.print("Topic: ");
    Serial.println(receivedData.topic);
    Serial.print("Key 1: ");
    Serial.print(receivedData.keys[0]);
    Serial.print(" Value 1: ");
    Serial.println(receivedData.values[0]);
    Serial.print("Key 2: ");
    Serial.print(receivedData.keys[1]);
    Serial.print(" Value 2: ");
    Serial.println(receivedData.values[1]);
}

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

uint8_t destMacList[MAX_CONNECTIONS][6] = {
      {0xB0, 0xB2, 0x1C, 0xB1, 0xD1, 0xA8},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0xB0, 0xB2, 0x1C, 0xB1, 0xD2, 0xA4}
};

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

    setProcessReceivedDataCallback(userProcessReceivedData);
    lotenInit(destMacList);
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        timeClient.update();

        float humidity = 55.5; // Example humidity
        float temperature = 22.2; // Example temperature

        static uint32_t seq_num = 0;

        std::vector<String> keys = {"humid", "temp"};
        std::vector<String> values = {String(humidity), String(temperature)};
        std::vector<uint8_t> data_types = {1, 2};
        String topic = "topic1";

        hi data;
        data.seq_num = seq_num++;
        data.time = timeClient.getEpochTime();
        strncpy(data.topic, topic.c_str(), sizeof(data.topic));
        strncpy(data.keys[0], keys[0].c_str(), sizeof(data.keys[0]));
        strncpy(data.keys[1], keys[1].c_str(), sizeof(data.keys[1]));
        strncpy(data.values[0], values[0].c_str(), sizeof(data.values[0]));
        strncpy(data.values[1], values[1].c_str(), sizeof(data.values[1]));
        data.data_types[0] = data_types[0];
        data.data_types[1] = data_types[1];

        lotenSend(&data, sizeof(data), destMacList[0], false);
    }

    delay(5000); // Wait for 5 seconds before sending the next message
}
