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
#include "highResTimer.h"

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

#define	PULSE_TIME	10000

typedef struct
{
    int pin;
    int value;
    pthread_t thread;
    unsigned char *valueList;
} SoftPwmDataType;

static SoftPwmDataType pwmDatas[MAX_PINS];
static SoftPwmDataType *pwmDataList[MAX_PINS];
static int pwmDataCount = 0;
static int spwmRange = 1;

/*
 * softPwmThread:
 *	Thread to do the actual PWM output
 *********************************************************************************
 */

static void softPwmThread(int arg)
{
    int index;
    static int cycle = 0;

    for (index = 0; index < pwmDataCount; index++)
    {
        digitalWrite(pwmDataList[index]->pin, (pwmDataList[index]->valueList)[cycle]);            
    }
    
    cycle = (cycle + 1) % spwmRange;
}

static void assignListValues(SoftPwmDataType *pwmData)
{
    int onCount = pwmData->value;
    int offCount = spwmRange - onCount;
    int index;

    for (index = 0; index < onCount; index++)
    {
        pwmData->valueList[index] = 1;
    }
    for (; index < spwmRange; index++)
    {
        pwmData->valueList[index] = 0;
    }
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
        else if (value > spwmRange)
        {
            value = spwmRange;
        }

        pwmDatas[pin].value = value;
        assignListValues(&pwmDatas[pin]);
    }
}

int softPwmModifiedInit(int pwmRange)
{
    if (pwmRange <= 0)
    {
        return -1;
    }

    spwmRange = pwmRange;
    
    createHighResHandler(softPwmThread, PULSE_TIME);
    
    return 0;
}



/*
 * softPwmCreate:
 *	Create a new softPWM thread.
 *********************************************************************************
 */

int softPwmModifiedCreate(int pin, int initialValue)
{
    // TODO check if init is called
    if (pin >= MAX_PINS)
    {
        return -1;
    }

    // TODO search for the list and check if it is already created
    
    digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);

    pwmDatas[pin].value = initialValue;
    pwmDatas[pin].pin = pin;
    pwmDatas[pin].valueList = malloc(spwmRange);
    
    assignListValues(&pwmDatas[pin]);
    
    pwmDataList[pwmDataCount] = &pwmDatas[pin];
    pwmDataCount++;    

    return 0;
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
        // TODO search for the list and remove it
    }
}
