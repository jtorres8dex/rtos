import socket
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import datetime

# Set up TCP server parameters
HOST = '0.0.0.0'  # Listen on all available network interfaces
PORT = 65432      # Same port as used in the RPi script

# Initialize lists to store data
roll_data = []
pitch_data = []
yaw_data = []
time_data = []

# Set up the figure and axes for real-time plotting
fig, (ax_roll, ax_pitch, ax_yaw) = plt.subplots(3, 1, figsize=(10, 8))
fig.suptitle("Real-Time IMU Data: Roll, Pitch, Yaw")

def update_plot(frame):
    # Clear previous data from the plots
    ax_roll.cla()
    ax_pitch.cla()
    ax_yaw.cla()

    # Plot each axis
    ax_roll.plot(time_data, roll_data, label="Roll", color="b")
    ax_pitch.plot(time_data, pitch_data, label="Pitch", color="g")
    ax_yaw.plot(time_data, yaw_data, label="Yaw", color="r")

    # Set labels
    ax_roll.set_ylabel("Roll (°)")
    ax_pitch.set_ylabel("Pitch (°)")
    ax_yaw.set_ylabel("Yaw (°)")
    ax_yaw.set_xlabel("Time (s)")

    # Set titles
    ax_roll.set_title("Roll")
    ax_pitch.set_title("Pitch")
    ax_yaw.set_title("Yaw")

    # Display legends
    ax_roll.legend()
    ax_pitch.legend()
    ax_yaw.legend()

# Function to handle incoming data
def receive_data():
    print("safdfs")
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((HOST, PORT))
        server_socket.listen()
        print(f"Listening for connections on {HOST}:{PORT}...")

        conn, addr = server_socket.accept()
        print(f"Connected by {addr}")
        with conn:
            start_time = None
            while True:
                data = conn.recv(1024).decode('utf-8')
                if not data:
                    break
                
                # Parse the incoming data (assuming it's in "Roll,Pitch,Yaw" format)
                try:
                    roll, pitch, yaw = map(float, data.split(","))
                    
                    # Update time data for real-time plotting
                    if start_time is None:
                        start_time = datetime.datetime.now()
                    elapsed_time = (datetime.datetime.now() - start_time).total_seconds()
                    
                    # Append data to the lists
                    roll_data.append(roll)
                    pitch_data.append(pitch)
                    yaw_data.append(yaw)
                    time_data.append(elapsed_time)

                    # Keep the lists to a reasonable length (e.g., last 100 points)
                    if len(time_data) > 100:
                        roll_data.pop(0)
                        pitch_data.pop(0)
                        yaw_data.pop(0)
                        time_data.pop(0)

                except ValueError:
                    print("Received invalid data format. Expected 'Roll,Pitch,Yaw'")
                    continue

# Run receive_data in the background so the plot can update continuously
receive_data()
# Start real-time plotting with matplotlib's FuncAnimation
ani = FuncAnimation(fig, update_plot, interval=100, save_count=1000)
plt.tight_layout()
plt.show()