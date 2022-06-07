import serial
import bluetooth


DEVICE = 'COM9'
BAUD_RATE = 115200

data =[0xAA, 0x02, 0x11, 0x22, 0xEE]

# Connect to the device
s = serial.Serial(DEVICE, BAUD_RATE)
# print('Connect to', DEVICE)

# # Send data
# s.write(bytearray(data))

# # Receive data
# data = s.read(8)
# print(data)