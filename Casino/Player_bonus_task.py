import binascii
import socket
import struct
import sys


# Create a socket object
s = socket.socket()

# Define the port on which you want to connect
port = 12345

# connect to the server on local computer
s.connect(('127.0.0.1', port))

packer = struct.Struct('I')
unpacker = struct.Struct('I I I')
# Casino sends data about number of rounds to each player
for i in range(1):
    rounds_packed = s.recv(packer.size)
    rounds_data = packer.unpack(rounds_packed)
    rounds = rounds_data[0]


for i in range(rounds):

    # Card values sent by server are received and the maximum of them is sent back to server
    data = s.recv(unpacker.size)
    if not data:
        break

    unpacked_data = unpacker.unpack(data)

    max_val = max(unpacked_data[0], unpacked_data[1], unpacked_data[2])

    send_data = packer.pack(max_val)
    s.sendall(send_data)
# Another loop is run to receive results of game
while True:
    msg = s.recv(1024)
    if not msg:
        break

    print(msg.decode("utf-8"))


s.close()
# close the connection
