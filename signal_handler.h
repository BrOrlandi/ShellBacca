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

#ifndef UNISTD_H
#define UNISTD_H
#include <unistd.h>
#endif

#ifndef SIGNAL_H
#define SIGNAL_H
#include <signal.h>
#endif

#ifndef SYSWAIT_H
#define SYSWAIT_H
#include <sys/wait.h>//para wait
#endif

void sa_sigactionn(int, siginfo_t *, void *);//função que vai cuidar dos sinais recebidos pelo pai
void IgnoreSignals();//pai ignora sinais SIGINT e SIGTSTP
void DefaultSignals();//para sinais gerarem comportamento padrão nos filhos
void SIGCHLD_SIGHUP();//pai seta controle dos sinais SIGCHLD e SIGHUP
#endif // SIGNAL_HANDLER_H_INCLUDED
