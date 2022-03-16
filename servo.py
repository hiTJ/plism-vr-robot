#!/usr/bin/env python3

import wiringpi as w

servo_pin_h = 18
servo_pin_v = 19

# 初期化：角度を設定前に一度だけ呼び出す必要がある


# Servo(MG966R) Spec
servo_clock = 50 # MG966R Spec
servo_pulse_width_min = 0.0005 # sec
servo_pulse_width_max = 0.0025 # sec

# PWM setup 
sys_clock = 19200000 #19.2Mhz(fixed)
pwm_range = 2048 
pwm_clock =  sys_clock / servo_clock / pwm_range

#ローカル関数：設定角度(-90～90)からパルス幅値を計算する。
def deg_to_val(deg) -> int:
	period = 1.0 / (sys_clock / pwm_clock)
	v1 =  servo_pulse_width_min / period
	v2 =  servo_pulse_width_max / period
	val = (v2 - v1) / 180 * (deg + 90) + v1
	return int(val)

def init():
	w.wiringPiSetupGpio()

	# PWM output mode
	w.pinMode(servo_pin_h,2)	# OUTPUT Mode
	w.pinMode(servo_pin_v,2)	# OUTPUT Mode
	# Set frequency
	w.pwmSetMode(0)				# 0Vに指定 
	w.pwmSetRange(pwm_range)	
	w.pwmSetClock(int(pwm_clock))

# ローカル関数
def clip_range(deg,min,max):
	if deg < min:
		deg = min
	elif deg > max:
		deg = max
	else:
		pass
	return deg


# 垂直角度をdeg度に設定する
def set_v(deg):
	# 垂直稼働範囲制限; 16°～55°
	deg = clip_range(deg,-16,50)
	w.pwmWrite(servo_pin_v, deg_to_val(deg))

# 水平角度をdeg度に設定する
def set_h(deg):
	# 垂直稼働範囲; -90°～90°
	deg = clip_range(deg,-90,90)
	w.pwmWrite(servo_pin_h, deg_to_val(deg))



