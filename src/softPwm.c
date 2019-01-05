#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <softPwm.h>

#define RANGE		100
#define MIN_VALUE    10
#define MAX_VALUE    50
#define	NUM_LEDS	  3

int ledMap[NUM_LEDS] = { 12, 13, 14 };

void ledInit(void)
{
    int i;

    wiringPiSetup();

    for (i = 0; i < NUM_LEDS; ++i)
    {
        softPwmCreate(ledMap[i], 0, RANGE);
    }
}

void setLed(unsigned int ledIndex, unsigned char value)
{
    unsigned char pwmValue = (100 * value) / 255;
    
    softPwmWrite(ledMap[ledIndex], pwmValue);
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
                softPwmWrite(ledMap[i], j);
                delay(10);
            }
        }
        delay(1000);
        for (i = 0; i < NUM_LEDS; ++i)
        {
            for (j = MAX_VALUE; j >= MIN_VALUE; --j)
            {
                softPwmWrite(ledMap[i], j);
                delay(10);
            }
        }
        delay(1000);
    }
}




