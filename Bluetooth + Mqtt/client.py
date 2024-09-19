import paho.mqtt.client as mqtt
import time
import ntplib
from time import ctime

def get_ntp_time(server='pool.ntp.org'):
  try:
        # Create an NTP client
        client = ntplib.NTPClient()

        # Send a request to the NTP server
        response = client.request(server)

        # Get the current time from the response (time since Unix epoch)
        current_time_since_epoch = response.tx_time

        return current_time_since_epoch

  except Exception as e:
        print(f"An error occurred: {e}")
        return None
# MQTT broker settings
mqtt_server = "139.59.246.168"
mqtt_port = 1883
mqtt_topic = "test/topic"

# Global variables to keep track of the last message time
last_time = None

# Callback when a message is received
def on_message(client, userdata, msg):
    global last_time
    data = msg.payload.decode()
    print(f"Received message: {data}")

    # Extract the current time from the message
    # Assuming the time is the last part of the message separated by a comma
    parts = data.split(',')
    if len(parts) >= 3:
        current_time = int(parts[-1])
        # print(f"Current Time: {current_time}")
        
        # Calculate elapsed time
        if last_time is not None:
            elapsed_time = - current_time + get_ntp_time()
            print(f"Time elapsed : {elapsed_time} seconds")
        
        # Update the last time
        last_time = current_time
    else:
        print("Message format is incorrect")

# Callback when the client connects to the broker
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe(mqtt_topic)

# Create an MQTT client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Connect to the MQTT broker
client.connect(mqtt_server, mqtt_port, 60)

# Loop forever, waiting for messages
client.loop_forever()
