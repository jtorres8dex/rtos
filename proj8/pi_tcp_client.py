# rpi_data_sender.py
import socket
import serial
import time

# Serial setup for Arduino connection
serial_port = '/dev/ttyUSB0'  # Or /dev/ttyACM0 depending on your setup
baud_rate = 9600

# TCP setup for WiFi communication
HOST = '192.168.1.100'  # Replace with your laptop's IP address
PORT = 65432            # Same port as the server

# Initialize Serial and TCP connections
ser = serial.Serial(serial_port, baud_rate, timeout=1)
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST, PORT))
print(f"Connected to {HOST}:{PORT}")

try:
    while True:
        if ser.in_waiting > 0:
            data = ser.readline().decode('utf-8').strip()  # Read data from Arduino
            if data:
                print("Sending:", data)
                client_socket.sendall(data.encode('utf-8'))  # Send over WiFi
                time.sleep(0.1)  # Small delay to avoid flooding
except KeyboardInterrupt:
    print("Interrupted by user")
finally:
    ser.close()
    client_socket.close()