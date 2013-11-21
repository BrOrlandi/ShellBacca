#include "signal_handler.h"
pid_t signal_handler_pid;

void sahandler(int signo)
{
    if (signo == SIGCHLD)//caso o filho seja destruído(pois setei a flag SA_NOCLDSTOP para ignorar quando o filho é suspenso ou continua)
        removerProcesso();//remove processo atual

    else if (signo == SIGHUP)//se detectou sinal de fechar terminal
        kill (0, SIGKILL);//destrói processo pai e filho. Eu precisei fazer isso porque por algum motivo se eu suspender o processo filho, ele não é destruído junto com o pai normalmente

    else if (signo == SIGINT)//se sinal recebido for interromper terminal
        kill (signal_handler_pid, SIGKILL);//destrói processo filho

    else if (signo == SIGTSTP)//se sinal recebido for parar terminal
    {
        kill (signal_handler_pid, SIGSTOP);//suspende processo filho
        alterarEstadoUltimoProcesso(STOPPED);//atera estado do processo atual para suspenso
    }

}

void SendSignalsToChild()
{
    struct sigaction mysigact;//cria uma struct sigaction
    mysigact.sa_handler = sahandler;//passa função que trata os sinais
    sigaction(SIGINT, &mysigact, NULL);//cuida de SIGINT
    sigaction(SIGTSTP,&mysigact, NULL);//cuida de SIGTSTP
    sigaction(SIGHUP, &mysigact, NULL);//cuida de SIGHUP
}

void IgnoreSignals()
{
    struct sigaction mysigact;//cria uma struct sigaction
    mysigact.sa_handler = SIG_IGN;//ignora os sinais
    sigaction(SIGINT, &mysigact, NULL);//cuida de SIGINT
    sigaction(SIGTSTP,&mysigact, NULL);//cuida de SIGTSTP
    mysigact.sa_handler = sahandler;//para tratar do SIGCHLD
    mysigact.sa_flags = SA_NOCLDSTOP;//para que trate somente o caso em que filho é destruído.
    sigaction(SIGCHLD,&mysigact,NULL);//cuida de SIGCHLD*/
}
