import os
import inputs
import asyncio
import paho.mqtt.client as mqtt
import json
import time
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


# Function to filter Xbox controller events
def is_xbox_controller(event):
    if (event.ev_type == "Absolute" and event.code.startswith("ABS_X")):
        return True
    elif (event.ev_type == "Absolute" and event.code.startswith("ABS_Y")):
        return True
    elif (event.ev_type == "Absolute" and event.code.startswith("ABS_Z")):
        return True
    return False

# Function to process Xbox controller events
def process_event(event):
    return {"code": event.code, "value": event.state}


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


def send_message(client, x, y, relay):
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


# Control Class
class Control:
    def __init__(self):
        self.x = 0
        self.y = 0
        self.m = 0
        self.t = 0

    def toJson(self):
        return {
            "x": self.x,
            "y": self.y,
            "m": self.m,
            "t": self.t
        }

CONTROL = Control()


async def send_joystick_events():
    # Connect to WebSocket server


    print("Connected to MQTT server")
    last_send_time = time.time()  # Record the time of the last event transmission
    
    # Infinite loop to read Xbox controller events and send them via WebSocket
    while True:

        events = inputs.get_gamepad()

        for event in events:
            if event.code == 'ABS_X':
                CONTROL.x =   90 + int(90*event.state/(32768))
            if event.code == 'ABS_Y':
                CONTROL.y =   90 - int(90*event.state/(32768)/2)
            if event.code == 'ABS_Z':
                CONTROL.m =   int(event.state/255)


        print(CONTROL.toJson())

        # Send joystick data as JSON string via WebSocket
        current_time = time.time()
        elapsed_time = current_time - last_send_time  # Calculate the time elapsed since the last event transmission
        
        
        # If 10 milliseconds have elapsed since the last transmission, send joystick events
        if elapsed_time >= 0.01:

            send_message(client, CONTROL.x, CONTROL.y, CONTROL.m)
            print("Sent joystick events")
            asyncio.sleep(0.01)
            last_send_time = current_time  # Update the time of the last event transmission
            

        # Otherwise, sleep until 10 milliseconds have elapsed since the last transmission
        else:
            await asyncio.sleep(0.01 - elapsed_time)
            print("Slept for", 0.01 - elapsed_time, "seconds")

asyncio.run(send_joystick_events())
