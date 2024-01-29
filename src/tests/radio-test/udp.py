import random
import socket
from time import sleep
import time
import struct

UDP_IP = "10.0.0.100"
UDP_PORT = 42069

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP

start = round(time.time() * 1000)

def genPacket():
    id = 10
    len = 4 * 6
    timestamp = round(time.time() * 1000) - start
    data = [random.random()*1000 for i in range(6)]

    sum1 = 0
    sum2 = 0

    sum1 = sum1 + id
    sum2 = sum2 + sum1
    sum1 = sum1 + len
    sum2 = sum2 + sum1

    for i in range(4):
        sum1 = sum1 + bytearray(struct.pack("i", timestamp))[i];
        sum2 = sum2 + sum1;

    for i in range(len):
        sum1 = sum1 + bytearray(struct.pack("f", data[int(i/4)]))[i%4];
        sum2 = sum2 + sum1;

    sum2 %= 256
    sum1 %= 256

    packet =[]
    packet.extend(bytearray(struct.pack("B", id)))
    packet.extend(bytearray(struct.pack("B", len)))
    packet.extend(bytearray(struct.pack("i", timestamp)))
    packet.extend(bytearray(struct.pack("B", sum2)))
    packet.extend(bytearray(struct.pack("B", sum1)))
    for d in data:
        packet.extend(bytearray(struct.pack("f", d)))

    return packet

print([ "0x%02x" % b for b in genPacket()])

num = 1000000
starttime = time.time()
lasttime = time.time()
interval = 6000 * 10**(-6)
for i in range(num):
    while(time.time()-lasttime < interval):
        pass
    lasttime = time.time()
    sock.sendto(bytearray(genPacket()), (UDP_IP, UDP_PORT))

rate = num * len(bytearray(genPacket())) * 8/((time.time() - starttime))
print(rate)

