
//
// gcc -Wall -o Servo Servo.c servo_lib.o -lwiringPi
//

#include <stdio.h>
#include <stdlib.h>
#include "servo_lib.h"

int main(int argc, char *argv[])
{
  if(argc == 3)
  {
    int h = atoi(argv[1]);
    int v = atoi(argv[2]);

    printf("moveto h=%d, v=%d\n",h,v);
    servo_init();
    servo_set_h(h);
    servo_set_v(v);
  }
  else
  {
    printf("Usage: Servo h v\n");
  }
  return 0;
}
