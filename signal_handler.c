#include "signal_handler.h"
void sahandler(int signo)
{
    if (signo == SIGINT)//se sinal recebido for interromper terminal
        kill (0, SIGKILL);//destrói processo cujo PID é 0, ou seja, o filho

    if (signo == SIGTSTP)//se sinal recebido for parar terminal
        kill (0, SIGSTOP);//pára processo cujo PID é 0, ou seja, o filho
}
