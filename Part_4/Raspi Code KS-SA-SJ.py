import math
from time import sleep
from adafruit_servokit import ServoKit
import socket

# IP address of Pi ('' means accept connections from any address)
HOST = ''
PORT = 5000


# Create a socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((HOST, PORT))
server_socket.listen(1)
print("Waiting for connection...")


conn, addr = server_socket.accept()
print(f"Connected by {addr}")

driver_1 = ServoKit(channels = 16, address = 0x40)
driver_2 = ServoKit(channels = 16, address = 0x41)

class SpiderObj:
    def __init__(self, a1, a2, a3):
        """
        Initialize the spider object with leg segment lengths and servo angles.
        
        :param a1: Length of the first leg segment (coxa)
        :param a2: Length of the second leg segment (femur)
        :param a3: Length of the third leg segment (tibia)
        """
        self.a1 = a1
        self.a2 = a2
        self.a3 = a3
        self.servo_angle = [0.0] * 18  # Initialize servo angles to zero
        self.position = [[23.0, 0, 0] for _ in range(6)] # Position of Leg End

        self.threshold = 0.25;

    def clamp(self, value, min_value, max_value):
        return max(min_value, min(value, max_value))

    def ik(self, r, theta, h):
        """
        Inverse kinematics: calculate joint angles for a given foot position.
        
        """
        theta1 = 90 + theta
        
        l = math.sqrt(h**2 + (r - self.a1)**2)  
        alpha = math.atan(h/(r - self.a1))

        theta2 = 90 + math.degrees(math.acos(self.clamp((self.a2**2 - self.a3**2 + l**2)/(2*self.a2*l), -1, 1)) - alpha)
        theta3 = 90 - math.degrees(math.acos(self.clamp((l**2 - self.a2**2 - self.a3**2)/(2*self.a2*self.a3), -1, 1)))

        return theta1, theta2, theta3

    def motor_to_ang(self, i, theta):
        self.servo_angle[i] = theta
        if 0 <= i <= 15:
            driver_1.servo[i].angle = theta
        elif 16 <= i <= 17:
            driver_2.servo[i - 14].angle = theta
        pass

    def leg_to_pos(self, i, r, theta, h):
        """
        Convert leg parameters to position in 3D space.
        
        :param r: Radial distance from body center
        :param theta: Angle in the body plane
        :param h: Height from the ground
        """
        self.position[i] = [r, theta, h]

        theta_1, theta_2, theta_3 = self.ik(r, theta, h)
        self.motor_to_ang(i, theta_1)
        self.motor_to_ang(i + 6, theta_2)
        self.motor_to_ang(i + 12, theta_3)
        pass

    def hard_scrunch(self, radius, step_size):
        t = 0
        flag_scrunch = 0
        while flag_scrunch == 0:
            for i in range(0,6):
                self.leg_to_pos(i, self.position[i][0], self.position[i][1], self.position[i][2])
                if abs(self.position[i][0] - radius) < self.threshold:
                    flag_scrunch = 1

                self.position[i][0] = self.position[i][0] + (radius -  self.position[i][0])*step_size
            sleep(0.01)
        pass

    def hard_stand(self, height, step_size):
        t = 0
        flag_stand = 0
        while flag_stand == 0:
            for i in range(0,6):
                self.leg_to_pos(i, self.position[i][0], self.position[i][1], self.position[i][2])
                if abs(self.position[i][2] - height) < self.threshold:
                    flag_stand = 1

                self.position[i][2] = self.position[i][2] + (height -  self.position[i][2])*step_size
            sleep(0.01)
        pass

    def hard_look(self, time, step_size, dirn):
        t = 0
        while t < time:
            for i in range(0,6):
                self.position[i][1] = self.position[i][1] + dirn*step_size
                self.self.leg_to_pos(i, self.position[i][0], self.position[i][1], self.position[i][2])
            t = t + 
            sleep(0.01)
        pass
       
    def hard_forward(self, time, swing, direction, step_size):
        """
        Moving the spider forward for one cycle using tripod gait
        """

        P = [0, 2, 4]
        Q = [1, 3, 5]

        # Move group P to high, half-swing

        # Move group Q to norm, norm

        # Move group P to norm, full-swing

        # Move group Q to high, half-swing

        # Move group P to norm, norm

        # Move group Q to norm, full-swing

        pass

spider = SpiderObj(a1 = 5, a2 = 7, a3 = 11)

"""
    r = 18 is as far from singularities as possible
"""

spider.hard_scrunch(16, 0.1)

sleep(1)

spider.hard_stand(3, 0.1)

sleep(1)

try:
   while True:
       data = conn.recv(1024)
       if not data:
           break
       number = data.decode()
       if number == 14:
           spider.hard_look(5, 0.1, 1)
       if number == 23:
           spider.hard_look(5, 0.1, -1)
       if number == 2:
           for i in range(0,6):
               spider.leg_to_pos(spider.position[i][0], 0.9*spider.position[i][1], spider.position[i][1])
           sleep(0.01)
except Exception as e:
   print("Error:", e)

