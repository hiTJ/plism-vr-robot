
//
// gcc -Wall -r -o servo_lib.o servo_lib.c 
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "servo_lib.h"

// GPIO PIN
#define SERVO_PIN_H 18 
#define SERVO_PIN_V 19

#define DEG_TO_V(deg) (70 + deg/2)
#define POS_MIN(deg,pos_min) ((deg < pos_min) ? pos_min : deg ) 
#define POS_MAX(deg,pos_max) ((deg > pos_max) ? pos_max : deg ) 

static int init_gpio(int gpio);

int servo_init(void)
{
  init_gpio(SERVO_PIN_H);
  return init_gpio(SERVO_PIN_V);
}

static int init_gpio(int gpio)
{
  if (wiringPiSetupGpio() == -1) {
     printf("cannot setup gpio.");
    return 1;
  }
  pinMode(gpio, PWM_OUTPUT);
  pwmSetMode(PWM_MODE_MS);
  pwmSetClock(400);
  pwmSetRange(1024);

  // move to initial position
  // pwmWrite(gpio, DEG_TO_V(0));
  return 0;
}

int servo_set_v(int deg)
{
  deg = POS_MIN(deg, -16);
  deg = POS_MAX(deg, 55);

  pwmWrite(SERVO_PIN_V, DEG_TO_V(deg));
  return 0;
}

int servo_set_h(int deg)
{
  deg = POS_MIN(deg, -90);
  deg = POS_MAX(deg, 90);

  pwmWrite(SERVO_PIN_H, DEG_TO_V(deg));
  return 0;
}

