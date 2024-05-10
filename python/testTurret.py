import paho.mqtt.client as mqtt
import json
import random
import time
import os

from dotenv import load_dotenv

# Load environment variables from .env file
load_dotenv()


# MQTT broker settings
broker_address = os.getenv("MQTT_BROKER_ADDRESS")
broker_port = int(os.getenv("MQTT_BROKER_PORT"))
username = os.getenv("MQTT_USERNAME")
password = os.getenv("MQTT_PASSWORD")
topic = "turret/control"

version = '5' # or '3' 
mytransport = 'websockets' # or 'tcp'

# Define callback function for when the client connects to the broker
def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker with result code "+str(rc))
    # Subscribe to the "turret/status" topic when connected
    client.subscribe("turret/status")

# Define callback function for when a message is received
def on_message(client, userdata, message):
   # print("Message received on topic:", message.topic)
    print("Message:", message.payload.decode())
    pass


def send_message(x, y, relay):
    # Create JSON message
    message = {
        "x": x,
        "y": y,
        "r": relay
    }

    # Convert message to JSON format
    json_message = json.dumps(message)

    # Publish JSON message to the control topic
    client.publish("turret/control", json_message)

    print("Message sent:", json_message)


# Create MQTT client instance
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, client_id="myPy")

# Set username and password for MQTT broker
client.username_pw_set(username, password)


# Set the callback functions
client.on_message = on_message
client.on_connect = on_connect


# Connect to MQTT broker
client.connect(broker_address, broker_port)
client.loop_start()

# Subscribe to the "turret/status" topic

relay = 0
try:
    while True:
        # Get user input for x, y, and relay values
        x = int(input("Enter value for x (0-180): "))
        y = int(input("Enter value for y (0-180): "))
        #relay = int(input("Enter value for relay (0 or 1): "))

#        x = 0
 #       y = 0

        if relay == 1:
            relay = 0
        else:
            relay = 1

        #relay = 0
        time.sleep(1.3)

        # Validate user input
        if 0 <= x <= 180 and 0 <= y <= 180 and relay in [0, 1]:
            send_message(x, y, relay)
        else:
            print("Invalid input. Please enter values within the specified range.")

        if relay == 1:
            time.sleep(0.3)
        else:
            time.sleep(0.5)

except KeyboardInterrupt:
    print("Exiting...")
    client.disconnect()