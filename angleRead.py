import time
import subprocess
from subprocess import PIPE
import copy

pos = None
posold = None
while(1):
	f = open('xwiimote/angle.txt', 'r', encoding='UTF-8')
	txt = f.read()
	f.close()
	print(txt)
	pos = txt.split(',')
	if(len(pos) > 1 and pos != posold):
		#shell = '/Servo/Servo ' + pos[0] + ' ' + pos[1]
		shell = '/home/pi/plism-vr-robot/Servo/Servo ' + pos[0] + ' ' + pos[1]
		#print('mp_x = ' + pos[0] + ', mp_y = ' + pos[1])
		#shell = 'pwd'
		proc = subprocess.run(shell, shell=True, stdout=PIPE, stderr=PIPE, text=True)
	print(proc)
	posold = copy.deepcopy(pos)
	time.sleep(0.1)
