#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int createHighResHandler(HANDLERFUNC sa_handler, int timerNSec)
{
    struct itimerspec timerSpec, timerSpecOld;
    struct sigaction action;
    struct sigevent sevent;
    timer_t timer1;

    /* SIGALRM for printing time */
    memset (&action, 0, sizeof (struct sigaction));
    action.sa_handler = sa_handler;
    if (sigaction (SIGALRM, &action, NULL) == -1)
        perror ("sigaction");

    /* for program completion */
    memset (&sevent, 0, sizeof (struct sigevent));
    sevent.sigev_notify = SIGEV_SIGNAL;
    sevent.sigev_signo = SIGRTMIN;

    if (timer_create (CLOCK_MONOTONIC, NULL, &timer1) == -1)
        perror ("timer_create");


    timerSpec.it_interval.tv_sec = timerNSec / 1000000000;
    timerSpec.it_interval.tv_nsec = timerNSec % 1000000000;  /* 100 us */
    timerSpec.it_value.tv_sec = timerNSec / 1000000000;
    timerSpec.it_value.tv_nsec = timerNSec % 1000000000;     /* 100 us */
    if (timer_settime (timer1, 0, &timerSpec, &timerSpecOld) == -1)
        perror ("timer_settime");
}
