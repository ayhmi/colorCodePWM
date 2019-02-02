/*
 * softPwm.c:
 *	Provide many channels of software driven PWM.
 *	Copyright (c) 2012-2017 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringPi.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <malloc.h>
#include <pthread.h>

#include "wiringPi.h"
#include "softPwmModified.h"

// MAX_PINS:
//	This is more than the number of Pi pins because we can actually softPwm.
//	Once upon a time I let pins on gpio expanders be softPwm'd, but it's really
//	really not a good thing.

#define	MAX_PINS	64

// The PWM Frequency is derived from the "pulse time" below. Essentially,
//	the frequency is a function of the range and this pulse time.
//	The total period will be range * pulse time in µS, so a pulse time
//	of 100 and a range of 100 gives a period of 100 * 100 = 10,000 µS
//	which is a frequency of 100Hz.
//
//	It's possible to get a higher frequency by lowering the pulse time,
//	however CPU uage will skyrocket as wiringPi uses a hard-loop to time
//	periods under 100µS - this is because the Linux timer calls are just
//	not accurate at all, and have an overhead.
//
//	Another way to increase the frequency is to reduce the range - however
//	that reduces the overall output accuracy...

#define	PULSE_TIME	100

typedef struct
{
    int pin;
    int value;
    int range;
    pthread_t thread;
} SoftPwmDataType;

static volatile SoftPwmDataType pwmDatas[MAX_PINS];

/*
 * softPwmThread:
 *	Thread to do the actual PWM output
 *********************************************************************************
 */

static void *softPwmThread(void *arg)
{
    int pin, mark, space;
    struct sched_param param;

    param.sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_setschedparam(pthread_self(), SCHED_RR, &param);

    pin = *((int *)arg);

    piHiPri (90);

    for (;;)
    {
        mark = pwmDatas[pin].value;
        space = pwmDatas[pin].range - mark;

        if (mark != 0)
        {    
            digitalWrite(pin, HIGH);
        }
        delayMicroseconds(mark * PULSE_TIME);

        if (space != 0)
        {
            digitalWrite(pin, LOW);
        }
        delayMicroseconds(space * PULSE_TIME);
    }

    return NULL;
}


/*
 * softPwmWrite:
 *	Write a PWM value to the given pin
 *********************************************************************************
 */

void softPwmModifiedWrite(int pin, int value)
{
    if (pin < MAX_PINS)
    {
        if (value < 0)
        {
            value = 0;
        }
        else if (value > pwmDatas[pin].range)
        {
            value = pwmDatas[pin].range;
        }

        pwmDatas[pin].value = value;
    }
}


/*
 * softPwmCreate:
 *	Create a new softPWM thread.
 *********************************************************************************
 */

int softPwmModifiedCreate(int pin, int initialValue, int pwmRange)
{
    if (pin >= MAX_PINS)
    {
        return -1;
    }

    if (pwmDatas[pin].range != 0)	// Already running on this pin
    {
        return -1;
    }

    if (pwmRange <= 0)
    {
        return -1;
    }

    digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);

    pwmDatas[pin].value = initialValue;
    pwmDatas[pin].range = pwmRange;
    pwmDatas[pin].pin = pin;

    return pthread_create(&pwmDatas[pin].thread, NULL, softPwmThread, (void *)&pwmDatas[pin].pin);
}


/*
 * softPwmStop:
 *	Stop an existing softPWM thread
 *********************************************************************************
 */

void softPwmModifiedStop(int pin)
{
    if ((pin < MAX_PINS) && (pin >= 0))
    {
        if (pwmDatas[pin].range != 0)
        {
            pthread_cancel(pwmDatas[pin].thread);
            pthread_join(pwmDatas[pin].thread, NULL);
            pwmDatas[pin].range = 0;
            digitalWrite(pin, LOW);
        }
    }
}
