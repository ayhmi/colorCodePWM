#ifndef HIGHRESTIMER_H
#define HIGHRESTIMER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*HANDLERFUNC)(int);

int createHighResHandler(HANDLERFUNC sa_handler, int timerNSec);

#ifdef __cplusplus
}
#endif
#endif
