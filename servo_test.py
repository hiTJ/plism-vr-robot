import sys
import time
import servo

servo.init()
param = sys.argv


def set_deg(deg):
	servo.set_h(deg)
	servo.set_v(deg)
	val = servo.deg_to_val(deg)
	print(deg,val)
	time.sleep(0.02)

if len(param) >= 2:
	deg_h = int(param[1])
	deg_v = int(param[2])
	print('moveto',deg_h,deg_v)
	servo.set_h(deg_h)
	servo.set_v(deg_v)
else:
	while(1):
		for deg in range(-90,90,1):
			set_deg(deg)
		for deg in range(90,-90,-1):
			set_deg(deg)


