#ifndef SIGNAL_HANDLER_H_INCLUDED
#define SIGNAL_HANDLER_H_INCLUDED

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

extern pid_t signal_handler_pid;//pid do processo filho
extern int signal_handler_stopped;//número de processos suspensos, inicialmente é 0

void sahandler(int);//função que vai cuidar dos sinais recebidos
void SendSignalsToChild();
void IgnoreSignals();
#endif // SIGNAL_HANDLER_H_INCLUDED
