import socket
import binascii
import struct
import sys

#Client socket is created
s = socket.socket()

port = 12345
#Client socket is connected to server
s.connect(('127.0.0.1',port))
n = input("Enter number of players")
m = input("Enter number of rounds")
m = int(m)
n = int(n)
while m%n == 0:
	m = input("Invalid number of rounds enter any other number")
	m = int(m)

data = (n,m)

packer = struct.Struct('I I')
packed_data = packer.pack(*data)
#Client sends the data of number of players and number of rounds to casino
s.sendall(packed_data)
#A loop is run to receive game results
while True:
	data = s.recv(1024)
	if not data:
		break
	print(data.decode("utf-8"))

	


s.close()
