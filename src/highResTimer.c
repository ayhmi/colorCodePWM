/*   hires_timer.c: 100 microsecond timer example program   */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

void print_time (void);

timer_t timer1;

int main (int argc, char **argv)
{
    struct itimerspec timerSpec, timerSpecOld;
    struct sigaction action;
    struct sigevent sevent;

    /* SIGALRM for printing time */
    memset (&action, 0, sizeof (struct sigaction));
    action.sa_handler = print_time;
    if (sigaction (SIGALRM, &action, NULL) == -1)
        perror ("sigaction");

    /* for program completion */
    memset (&sevent, 0, sizeof (struct sigevent));
    sevent.sigev_notify = SIGEV_SIGNAL;
    sevent.sigev_signo = SIGRTMIN;

    if (timer_create (CLOCK_MONOTONIC, NULL, &timer1) == -1)
        perror ("timer_create");


    timerSpec.it_interval.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = 100000;  /* 100 us */
    timerSpec.it_value.tv_sec = 0;
    timerSpec.it_value.tv_nsec = 100000;     /* 100 us */
    if (timer_settime (timer1, 0, &timerSpec, &timerSpecOld) == -1)
        perror ("timer_settime");
    
    while(1);

    exit (EXIT_SUCCESS);
}

void print_time (void)
{   
    struct timespec tp;
    char buffer [80];

    if (clock_gettime (CLOCK_MONOTONIC, &tp) == -1)
        perror ("clock_gettime");

    sprintf (buffer, "%ld s %ld ns overrun = %d\n", tp.tv_sec,
                      tp.tv_nsec, timer_getoverrun (timer1));
    write (STDOUT_FILENO, buffer, strlen (buffer));
}