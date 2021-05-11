import random
import socket
from threading import Thread
from queue import Queue
import binascii
import struct
import sys
#Function to send card values to player and receive maximum of them
def send_and_receive(c,packed_data,q):
	c.sendall(packed_data)
	unpacker = struct.Struct('I')
	data = c.recv(unpacker.size)
	max_val = unpacker.unpack(data)
	q.put(max_val[0])
#Function to send results of game to each player
def send_result(c,i):
	c.send(bytes("Player " + str(i+1) + " wins the game","utf-8"))

def main():
	port = 12345
#Casino socket is created
	s = socket.socket()

	s.bind(('',port))

	s.listen(100)
    
	n = 3
	m = 4
	points = [0]*n
#A list of connections to player sockets is created
	socket_list = []
	for i in range(n):
		play,address = s.accept()
		socket_list.append((play,address))

	for j in range(1,m+1):
#A list of distinct numbers is generated and scaled to range of 13
		list_size = 3*n
		list_num = random.sample(range(1,53),list_size)

		for k in range(list_size):
			list_num[k] = (list_num[k] - 1)%13 + 1
		print("Card values generated (in range of 1 to 13):")
		print(list_num)
#A queue is used to store maximum  card value of each player
#And the queue elements are then stored in a list for further result calculation
		max_val = []
		thrds = []
		queue = Queue(maxsize = n)
		packer = struct.Struct('I I I')
#Card values are packed in tuples of 3 elements and sent to each player
		for k in range(n):
			vals = (list_num[3*k],list_num[3*k+1],list_num[3*k+2])
			packed_data = packer.pack(*vals)
			thrds.append(Thread(target=send_and_receive, args=(socket_list[k][0],packed_data,queue)))
			
		for k in range(n):
			thrds[k].start()
		for k in range(n):
			thrds[k].join()
		for k in range(n):
			max_val.append(queue.get())
#Accordingly winner of each result is found and points are updated
		for k in range(n):
			if max(max_val) == max_val[k]:
				print("Round ",j,": Winner is Player",k+1)
				points[k] = points[k] + 1

#From the points of each player winner of game is found
#and result is sent to each player	
	for k in range(n):
		thrds_result = []
		if max(points) == points[k]:
			print("Player " + str(k+1) +" wins the game")
			for j in range(n):
				thrds_result.append(Thread(target = send_result,args = (socket_list[j][0],k)))
			for j in range(n):
				thrds_result[j].start()
			for j in range(n):
				thrds_result[j].join()

	for j in range(n):
		socket_list[j][0].close()
	s.close()

if __name__ == "__main__":
	main()
