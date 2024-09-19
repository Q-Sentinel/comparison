
#include <WiFi.h>
#include <WiFiUdp.h>
#include <vector>
#include <NTPClient.h>
#include <loten.h>
// #include "DHT.h"
#define DHTPIN 23
#define DHTTYPE DHT22

// DHT dht(DHTPIN,DHTTYPE);
const char *ssid = "SLT Fiber ";
const char *password = "20030710";

const char *udpAddress = "139.59.246.168"; // Destination IP address

// const char *udpAddress = "192.168.1.28";
const int udpPort = 54000;                // Destination UDP port
typedef struct hi {
    uint32_t seq_num;
    char topic[16];  // Adjust the size based on your topic length
    char keys[2][8]; // Assuming 2 keys with a max length of 8 characters each
    char values[2][16]; // Assuming 2 values with a max length of 16 characters each
    uint8_t data_types[2];
    uint32_t time;  // Unix time from NTP server
} hi;
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org");
// Define the packet structure
struct Packetn
{
    uint8_t version;
    uint8_t type;
    uint32_t seq_num;
    uint32_t ack_num;
    uint16_t payload_length;
    uint8_t flags;
    uint16_t checksum;
    char topic[32];
    char payload[1024]; // Example payload size
} __attribute__((packed));
uint8_t destMacList[MAX_CONNECTIONS][6] = {
      {0xB0, 0xB2, 0x1C, 0xB1, 0xD1, 0xA8},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0xB0, 0xB2, 0x1C, 0xB1, 0xD2, 0xA4}
};
// Function to calculate a simple checksum
uint16_t calculate_checksum(const Packetn &packet)
{
    uint16_t sum = 0;
    sum += packet.version;
    sum += packet.type;

    sum += packet.payload_length;
    sum += packet.flags;
    for (size_t i=0; i< sizeof(packet.topic);i++){
      sum += packet.payload[i];
    }
    for (size_t i = 0; i < packet.payload_length; ++i)
    {
        sum += packet.payload[i];
    }
    return ~sum;
}

bool create_and_send_packet(uint32_t &seq_num,
                            const std::vector<String> &keys,
                            const std::vector<String> &values,
                            const std::vector<uint8_t> &data_types, const String &topic)
{
    if (keys.size() != values.size() || keys.size() != data_types.size())
    {
        Serial.println("Mismatch in size of keys, values, or data types");
        return false;
    }

    // Create a packet to send
    Packetn packet;
    packet.version = 1;
    packet.type = 1; // Type 1 for data packet
    packet.seq_num = seq_num++;
    packet.ack_num = 1;
    packet.flags = 0;

    size_t offset = 0;
    strncpy(packet.topic, topic.c_str(), sizeof(packet.topic));
    packet.topic[sizeof(packet.topic) - 1] = '\0';

    // Prepare the payload
    for (size_t i = 0; i < keys.size(); ++i)
    {
        size_t key_length = std::min(static_cast<size_t>(8), keys[i].length());
        size_t value_length = values[i].length();

        // Ensure payload does not exceed maximum size
        if (offset + key_length + 2 + value_length > sizeof(packet.payload) )
        {
            Serial.println("Payload size exceeds maximum allowed size");
            return false;
        }

        // Copy key
        memcpy(packet.payload + offset, keys[i].c_str(), key_length);
        packet.payload[offset + key_length] = '\0';
        offset += 8;

        // Copy value length (1 byte)
        packet.payload[offset++] = static_cast<uint8_t>(value_length);

        // Copy data type (1 byte)
        packet.payload[offset++] = data_types[i];

        // Copy value
        memcpy(packet.payload + offset, values[i].c_str(), value_length);
        offset += value_length;
    }

    packet.payload_length = offset; // Set actual payload length
    packet.checksum = calculate_checksum(packet);

    // Send the packet
    udp.beginPacket(udpAddress, udpPort);
    
    udp.write((uint8_t *)&packet, sizeof(packet));
    udp.endPacket();
    timeClient.begin();
    timeClient.setTimeOffset(0);
    Serial.println("Packetn sent with payload: " + String(packet.payload) + "size" + String(sizeof(packet)) + "checksum" + String(packet.checksum));
    return true;
}

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
    // dht.begin();
        setProcessReceivedDataCallback(userProcessReceivedData);
    lotenInit(destMacList);
    udp.begin(54001); // Local UDP port for receiving (if needed)
}
void userProcessReceivedData(const uint8_t* data, uint16_t length) {
     
  
    Serial.println("User defined received data:");
    if (length != sizeof(hi)) {
        Serial.println("Received data length does not match hi structure size");
        return;
    }
    hi receivedData;
    memcpy(&receivedData, data, sizeof(hi));
  //  Serial.println(timeClient.getEpochTime());
    std::vector<String> keys = {"humid", "temp", "time"};
    std::vector<String> values = {String(receivedData.values[0]), String(receivedData.values[1]), String(receivedData.time)};
    std::vector<uint8_t> data_types = {1, 2, 1}; // Example data types
    String topic = "topic1";
      static uint32_t seq_num = 0;
    if (!create_and_send_packet(seq_num, keys, values, data_types, topic))
    {
        Serial.println("Error creating or sending packet, retrying...");
    }
    //     timeClient.begin();
    // timeClient.setTimeOffset(0);
    // timeClient.update();
long time = 10;
Serial.print("time elapsed: ");
Serial.println(time-receivedData.time);
Serial.print("time now: ");
Serial.println(time);
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
 
    // float humidity = 55.3;
    // float temperature = 66.7;
 

}
void loop()
{


    delay(10); // Pause before the next iteration
}
