#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include "softPwmModified.h"
#include "colorControl.h"

#define RANGE		255
#define MIN_VALUE    10
#define MAX_VALUE    50
#define	NUM_LEDS	  3

int ledMap[NUM_LEDS] = { 12, 13, 14 };

void ledInit(void)
{
    int i;

    wiringPiSetup();
    
    softPwmModifiedInit(RANGE);

    for (i = 0; i < NUM_LEDS; ++i)
    {
        softPwmModifiedCreate(ledMap[i], 0);
    }
}

void setLed(unsigned int ledIndex, unsigned char value)
{
    unsigned char pwmValue = (RANGE * value) / 255;
    
    softPwmModifiedWrite(ledMap[ledIndex], pwmValue);
}

void setColor(unsigned int color)
{
    setLed(RED_LED, (color & 0xff0000) >> 16);
    setLed(GREEN_LED, (color & 0xff00) >> 8);
    setLed(BLUE_LED, (color & 0xff));
}

void loopColor(void)
{
    int i, j;

    for (;;)
    {
        for (i = 0; i < NUM_LEDS; ++i)
        {
            for (j = MIN_VALUE; j <= MAX_VALUE; ++j)
            {
                softPwmModifiedWrite(ledMap[i], j);
                delay(10);
            }
        }
        delay(1000);
        for (i = 0; i < NUM_LEDS; ++i)
        {
            for (j = MAX_VALUE; j >= MIN_VALUE; --j)
            {
                softPwmModifiedWrite(ledMap[i], j);
                delay(10);
            }
        }
        delay(1000);
    }
}




