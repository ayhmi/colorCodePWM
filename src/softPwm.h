#ifndef SOFTPWM_H
#define SOFTPWM_H

#define RED_LED 0
#define GREEN_LED 1
#define BLUE_LED 2

void setLed(unsigned int ledIndex, unsigned char value);
void loopColor(void);

#endif



