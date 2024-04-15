import inputs
import asyncio
import websockets
import json
import time

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

# WebSocket server settings
websocket_server = "ws://192.168.178.127:8080/control?token=token&robotid=1&type=client"  # Change this to your WebSocket server address


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
    async with websockets.connect(websocket_server) as websocket:
        print("Connected to WebSocket server")
        last_send_time = time.time()  # Record the time of the last event transmission
        
        # Infinite loop to read Xbox controller events and send them via WebSocket
        while True:
            print("ge")
            events = inputs.get_gamepad()


            print("....")
            for event in events:
                if event.code == 'ABS_X':
                    CONTROL.x =   int(-0.5*event.state/(2*32768) * 255 * 2)
                if event.code == 'ABS_Y':
                    CONTROL.y =   int(event.state/(2*32768) * 255* 2)
                if event.code == 'ABS_Z':
                    CONTROL.m =   int(event.state)

            print("kwd")

 
            print(CONTROL.toJson())

            # Send joystick data as JSON string via WebSocket
            current_time = time.time()
            elapsed_time = current_time - last_send_time  # Calculate the time elapsed since the last event transmission
            
            

            # If 10 milliseconds have elapsed since the last transmission, send joystick events
            if elapsed_time >= 0.01:
                await websocket.send(json.dumps(CONTROL.toJson()))
                print("Sent joystick events")
                last_send_time = current_time  # Update the time of the last event transmission
            
            # Otherwise, sleep until 10 milliseconds have elapsed since the last transmission
            else:
                await asyncio.sleep(0.01 - elapsed_time)
                print("Slept for", 0.01 - elapsed_time, "seconds")

asyncio.run(send_joystick_events())
