#include "signal_handler.h"
void sa_sigactionn(int signo, siginfo_t *siginfo, void *context)
{
    if (signo == SIGCHLD)//caso o filho seja destruído(pois setei a flag SA_NOCLDSTOP para ignorar quando o filho é suspenso ou continua)
    {
        if (siginfo->si_code == CLD_STOPPED)//processo filho foi suspenso
            alterarEstadoUltimoProcesso(STOPPED);//atera estado do processo atual para suspenso

        else if (siginfo->si_code == CLD_EXITED || siginfo->si_code == CLD_KILLED || siginfo->si_code == CLD_DUMPED)//processo filho foi destruído de alguma forma
        {
            pid_t pid;
            char entrou = 0;

            while ((pid = waitpid((pid_t)(-1), 0, WNOHANG)) > 0)//remove processos zumbis. Processos que fecham em background(clicando no 'x' no canto superior direito do programa) sempre viram zumbis porque não tem wait para esperá-los. Contudo, mesmo processos que rodam em foreground e estão sendo "esperados", às vezes viram zumbis(pelos meus testes). Então este while cuida disso
            {
                entrou = 1;
                removerProcesso(pid);//remove processo que acabou de ser destruído, da lista de processos
            }

            if (entrou == 0)//se processo não virou zumbi, ele também sempre é o atual, então não precisa saber o pid. Remove abaixo
                removerProcessoAtual();
        }
    }

    else if (signo == SIGHUP)//se detectou sinal de fechar terminal
    {
        destruirListaProcessos();//destrói todos os processos
        kill(getpid(), SIGKILL);//destrói shell
    }

}

void IgnoreSignals()
{
    struct sigaction mysigact;//cria uma struct sigaction
    mysigact.sa_handler = SIG_IGN;//ignora os sinais
    sigaction(SIGINT, &mysigact, NULL);//ignora SIGINT
    sigaction(SIGTSTP,&mysigact, NULL);//ignora SIGTSTP
}

void DefaultSignals()
{
    struct sigaction mysigact;//cria uma struct sigaction
    mysigact.sa_handler = SIG_DFL;//sinais vão apresentar comportamento padrão
    sigaction(SIGINT, &mysigact, NULL);
    sigaction(SIGTSTP,&mysigact, NULL);
}

void SIGCHLD_SIGHUP()
{
    struct sigaction mysigact;//cria uma struct sigaction
    mysigact.sa_sigaction = sa_sigactionn;//para tratar do SIGCHLD
    mysigact.sa_flags = SA_SIGINFO;//para usar sa_sigaction e não sa_handler
    sigaction(SIGCHLD,&mysigact,NULL);//cuida de SIGCHLD
    sigaction(SIGHUP, &mysigact, NULL);//cuida de SIGHUP
}
