import inputs
import asyncio
import websockets
import json

# Function to filter Xbox controller events
def is_xbox_controller(event):
    return event.ev_type == "Absolute" and event.code.startswith("ABS_X")

# Function to process Xbox controller events
def process_event(event):
    return {"code": event.code, "value": event.state}

# WebSocket server settings
websocket_server = "ws://192.168.178.127:8080/control?token=token&robotid=2&type=robot"  # Change this to your WebSocket server address

async def send_joystick_events():
    # Connect to WebSocket server
    async with websockets.connect(websocket_server) as websocket:
        print("Connected to WebSocket server")

        # Infinite loop to read Xbox controller events and send them via WebSocket
        while True:
            events = inputs.get_gamepad()
            xbox_events = filter(is_xbox_controller, events)
            joystick_data = [process_event(event) for event in xbox_events]

            print(joystick_data)
            # Send joystick data as JSON string via WebSocket
            await websocket.send(json.dumps(joystick_data))

asyncio.run(send_joystick_events())
