import time
import servo
import copy

servo.init()

pos = [0,0]
posold = [0,0]
while(1):
	f = open('xwiimote/angle.txt', 'r', encoding='UTF-8')
	txt = f.read()
	f.close()
	print(txt)
	pos = txt.split(',')
	if(len(pos) > 1 and (pos[0] != posold[0] or pos[1] != posold[1])):
		print('move to',pos[0],pos[1])
		servo.set_h(int(pos[0]))
		servo.set_v(int(pos[1]))
	posold = copy.deepcopy(pos)
	time.sleep(0.1)
