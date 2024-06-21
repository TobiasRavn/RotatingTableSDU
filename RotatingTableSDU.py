import socket
from time import sleep

class RotatingTableSDU:
    def __init__(self, ip_address, port):
        self.ip_address = ip_address
        self.port = port
        self.socket = None

    def connect(self):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((self.ip_address, self.port))

    def disconnect(self):
        self.socket.close()

    def rotate_to_angle(self, angle):
        if angle < 0 or angle > 360:
            raise ValueError('Angle must be within the range 0-360 degrees')
        self.socket.send(('goToAngle:' + str(angle)).encode())
        response = self.socket.recv(64).decode()
        if response.startswith('actualAngle:'):
            actual_angle = float(response[12:])
            return actual_angle
        else:
            return None

    def home(self, home_angle : float = 0.0):
        self.socket.send(('home:'+str(home_angle)).encode())
        response = self.socket.recv(64).decode()
        if response.startswith('homeAngle:'):
            actual_home_angle = float(response[10:])
            return actual_home_angle
        else:
            return None
        
    def get_angle(self):
        self.socket.send(('getAngle').encode())
        response = self.socket.recv(64).decode()
        if response.startswith('currentAngle:'):
            current_angle = float(response[13:])
            return current_angle
        else:
            return None
        
    def powerOn(self):
        self.socket.send(('powerOn').encode())
        response = self.socket.recv(64).decode()
        if response.startswith('poweredOn'):
            return True
        else:
            return None
        
    def powerOff(self):
        self.socket.send(('powerOff').encode())
        response = self.socket.recv(64).decode()
        if response.startswith('poweredOff'):
            return True
        else:
            return None

# Example of usage:
if __name__ == '__main__':
    rotating_table = RotatingTableSDU('192.168.0.122', 80)
    rotating_table.connect()
    print('Rotating table is connected')

    actual_angle = rotating_table.rotate_to_angle(180)
    print('Actual angle:', actual_angle)

    powerOn = rotating_table.powerOn()
    print('Power on:', powerOn)

    home_angle = rotating_table.home(0)
    print('Home angle:', home_angle)

    current_angle = rotating_table.get_angle()
    print('Current angle:', current_angle)

    actual_angle = rotating_table.rotate_to_angle(180)
    print('Actual angle:', actual_angle)

    powerOff = rotating_table.powerOff()
    print('Power off:', powerOff)

    current_angle = rotating_table.get_angle()
    print('Current angle:', current_angle)

    rotating_table.disconnect()
    print('Rotating table is disconnected')