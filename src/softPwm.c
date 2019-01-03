#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <softPwm.h>

#define RANGE		100
#define MAX_VALUE    50
#define	NUM_LEDS	  3

#define RED_LED 0
#define GREEN_LED 1
#define BLUE_LED 2

int ledMap[NUM_LEDS] = { 12, 13, 14 };

#if 0
void setLed(unsigned int ledIndex, unsigned char value)
{
    unsigned char pwmValue = (100 * value) / 255;
    
    softPwmWrite(ledMap[ledIndex], pwmValue);
}
#endif

int main()
{
    int i, j;
    char buf[80];

    wiringPiSetup();

    for (i = 0; i < NUM_LEDS; ++i)
    {
        softPwmCreate(ledMap[i], 0, RANGE);
    }

    for (;;)
    {
        for (i = 0; i < NUM_LEDS; ++i)
        {
            for (j = 0; j <= MAX_VALUE; ++j)
            {
                softPwmWrite(ledMap[i], j);
                delay(10);
            }
        }
        delay(1000);
        for (i = 0; i < NUM_LEDS; ++i)
        {
            for (j = MAX_VALUE; j >= 0; --j)
            {
                softPwmWrite(ledMap[i], j);
                delay(10);
            }
        }
        delay(1000);
    }
}




