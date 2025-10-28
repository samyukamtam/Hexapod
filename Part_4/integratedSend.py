import socket
import serial
import time

RASPBERRY_PI_IP = '192.168.57.206'  
PORT = 5000


SERIAL_PORT = '/dev/cu.usbserial-WX5UBC3' 
BAUD_RATE = 115200

def connect_serial():
    while True:
        try:
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
            print(f"Connected to serial port {SERIAL_PORT} at {BAUD_RATE} baud.")
            return ser
        except serial.SerialException as e:
            print("Serial connect error:", e)
            print("Retrying serial connection in 2 seconds...")
            time.sleep(2)

def connect_socket():
    while True:
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((RASPBERRY_PI_IP, PORT))
            print(f"Connected to Raspberry Pi at {RASPBERRY_PI_IP}:{PORT}")
            return client_socket
        except Exception as e:
            print("Socket connect error:", e)
            print("Retrying socket connection in 2 seconds...")
            time.sleep(2)

def main_loop(ser, client_socket):
    try:
        while True:
            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print("Received from Propeller:", line)
                    client_socket.send(line.encode())
                    print("Sent to Raspberry Pi:", line)
            time.sleep(0.1)
    except (serial.SerialException, socket.error) as e:
        print("Error during communication:", e)
        ser.close()
        client_socket.close()
        print("Closed connections. Reinitializing...")
        return False

if __name__ == "__main__":
    while True:
        ser = connect_serial()
        client_socket = connect_socket()
        if main_loop(ser, client_socket) is False:
            continue
