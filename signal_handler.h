#ifndef SIGNAL_HANDLER_H_INCLUDED
#define SIGNAL_HANDLER_H_INCLUDED

#include "processes_handler.h"

#ifndef SYSTYPES_H
#define SYSTYPES_H
#include <sys/types.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef SIGNAL_H
#define SIGNAL_H
#include <signal.h>
#endif

#ifndef SYSWAIT_H
#define SYSWAIT_H
#include <sys/wait.h>//para wait
#endif

void sahandler(int);//função que vai cuidar dos sinais recebidos
void SendSignalsToChild();
void IgnoreSignals();
#endif // SIGNAL_HANDLER_H_INCLUDED
