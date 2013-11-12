#ifndef SIGNAL_HANDLER_H_INCLUDED
#define SIGNAL_HANDLER_H_INCLUDED

#ifndef SYSTYPES_H
#define SYSTYPES_H
#include <sys/types.h>
#endif

#ifndef SIGNAL_H
#define SIGNAL_H
#include <signal.h>
#endif

void sahandler(int);//função que vai cuidar dos sinais recebidos
#endif // SIGNAL_HANDLER_H_INCLUDED
