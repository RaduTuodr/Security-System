import serial
import time
import threading

from fastapi import FastAPI
from fastapi.responses import JSONResponse
import uvicorn
from fastapi.middleware.cors import CORSMiddleware




# 1) Update this to match your Arduino's serial port
#    e.g., "COM3" on Windows, or "/dev/ttyACM0" on Linux
SERIAL_PORT = "COM8"

app = FastAPI()
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)
# A global dictionary to hold the Arduino status
machineState = {
    "machineEnabled": False,   # True if "MACHINE ENABLED!"
    "beamBroken": False,       # True if we see "Beam Broken!"
    "lastKeyPressed": None     # e.g. "1", "2", "3"...
}

def read_from_arduino():
    """
    Continuously read lines from Arduino via serial,
    parse interesting messages, and update machineState.
    """
    try:
        # Open the serial connection
        ser = serial.Serial(SERIAL_PORT, 9600, timeout=1)
        time.sleep(2)  # give it a moment to initialize
    except Exception as e:
        print(f"ERROR: Could not open serial port {SERIAL_PORT}: {e}")
        return

    while True:
        line = ser.readline().decode("utf-8").strip()
        if line:
            print("[Arduino]", line)  # for debugging

            # 1) Machine enabled/disabled
            if "MACHINE ENABLED!" in line:
                machineState["machineEnabled"] = True
            elif "MACHINE DISABLED!" in line:
                machineState["machineEnabled"] = False

            # 2) Beam broken
            elif "Beam Broken!" in line:
                machineState["beamBroken"] = True
            # If you want to clear beamBroken back to False
            # when the beam is restored, you'd need the Arduino to print
            # something like "Beam Restored!". Then you could:
            # elif "Beam Restored!" in line:
            #     machineState["beamBroken"] = False

            # 3) Key pressed
            elif "Key Pressed:" in line:
                parts = line.split(":")
                if len(parts) == 2:
                    key = parts[1].strip()
                    machineState["lastKeyPressed"] = key

        time.sleep(0.05)  # small delay

@app.get("/status")
def get_status():
    """
    GET endpoint for the React frontend to retrieve the machine state.
    """
    return JSONResponse(content=machineState)

if __name__ == "__main__":
    # Start a separate thread to read from the Arduino
    reader_thread = threading.Thread(target=read_from_arduino, daemon=True)
    reader_thread.start()

    # Launch the FastAPI server on http://localhost:8000
    uvicorn.run(app, host="0.0.0.0", port=8000)
