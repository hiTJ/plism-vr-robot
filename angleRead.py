import time
import subprocess
from subprocess import PIPE

while(1):
	f = open('xwiimote/angle.txt', 'r', encoding='UTF-8')
	txt = f.read()
	f.close()
	print(txt)
	pos = txt.split(',')
	shell = '~/Survo/Servo' + pos[0] + ' ' + pos[1]
	proc = subprocess.run(shell, shell=True, stdout=PIPE, stderr=PIPE, text=True)
	print(proc)
	time.sleep(1)
