import socket
import random
import time

TCP_IP = '192.168.1.94'
TCP_PORT = 6666



s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))


def rev_bytes(r,g,b):
    return (hex(int('{:08b}'.format(g)[::-1], 2))[2:].zfill(2)) + '' + (hex(int('{:08b}'.format(r)[::-1], 2))[2:].zfill(2)) + '' + (hex(int('{:08b}'.format(b)[::-1], 2))[2:].zfill(2))


max = 255

rmax = 3
gmax = 3
bmax = 3

sleep = 1

for x in range(0, 10000):

    for i in range(0,256):

        b = bin(i)[2:].zfill(8)
        msg = '10';

        for n in range(0, 8):
            c = b[n]

            if c == '1':
                msg += rev_bytes(random.randint(1,rmax),random.randint(1,gmax),random.randint(1,bmax))
            else:
                msg += '000000'

        print msg
        s.send(msg.decode('hex'))
        time.sleep(sleep)

s.close()
