import time

while(1):
	f = open('xwiimote/angle.txt', 'r', encoding='UTF-8')
	print(f.read())
	f.close()
	time.sleep(1)
