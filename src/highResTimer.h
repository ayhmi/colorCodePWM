#ifndef HIGHRESTIMER_H
#define HIGHRESTIMER_H

#ifdef __cplusplus
extern "C" {
#endif

int createHighResHandler(void (*sa_handler)(int), int timerNSec);

#ifdef __cplusplus
}
#endif
#endif
