#include "internal_commands.h"

char internal_command(char* line)
{
    if (exist_expression(line, "^ *exit *$") == 0)
        return 1;//exit, retorna 1

    else
    {
        int end;//primeira posição depois de comando interno, como "cd"

        if (expression_delimeters2(line, "^ *cd($| )", &end) == 0)//se for só cd e terminar, ou for cd seguido de um espaço, é o comando cd
        {
            cd(line, end);//executa comando cd
            return 2;//retorna 2
        }

        else if (expression_delimeters2(line, "^ *jobs($| )", &end) == 0)//ver explicação de cd
        {
            jobs(line + end);//executa comando jobs
            return 3;//retorna 3
        }

        else if (expression_delimeters2(line, "^ *bg($| )", &end) == 0)
        {
            bg(line + end);//executa comando bg
            return 4;//retorna 4
        }

        else if (expression_delimeters2(line, "^ *fg($| )", &end) == 0)
        {
            fg(line + end);//execute comando fg
            return 5;
        }
    }

    return 0;//se não foi e nenhum comando interno, retorna 0
}

void cd(char* line, int argindex)
{
    char path[MAX_LEN];//para armazenar caminho de diretório atual
    getcwd(path,sizeof(path));//pega diretório atual

    if (exist_expression(line + argindex, "^ *$") == 0)//é o CD sozinho, voltar ao HOME
    {
        char* HOME = getenv("HOME");

        if (chdir(HOME) != 0)
            perror("Nao foi possivel voltar pra home");
        else
        {
            setenv("OLDPWD", path, 1);//seta OLDPWD
            setenv("PWD", HOME, 1);//seta PWD
        }

    }

    else if (exist_expression(line + argindex, "^ *-") == 0)//é CD -, que fica alternando
    {
        char* OLDPWD = getenv("OLDPWD");

        if (OLDPWD != NULL)//se OLDPWD foi setado
        {
            chdir(OLDPWD);//muda pra diretorio antigo
            setenv("OLDPWD", path, 1);//seta OLDPWD
            setenv("PWD", OLDPWD, 1);//seta PWD
        }
        else
            fprintf(stderr, "A variavel de ambiente OLDPWD nao foi setada ainda\n");
    }

    else if (exist_expression(line + argindex, "^ */") == 0)//voltar ao diretorio raiz
    {
        if (chdir("/") != 0)//se não conseguir ir ao diretorio raiz
            perror("Nao foi possivel ir ao diretorio raiz");
        else
        {
            setenv("OLDPWD", path, 1);//seta OLDPWD
            setenv("PWD", "/", 1);//seta PWD
        }

    }

    else//se encontrou argumentos
    {

        int startExpr, end;//início e fim da expressão encontrada, respectivamente
        char* command = NULL;//diretório ou '..' ou '~' atual
        char flag = 0;//para ficar alternando entre diretório atual e anterior
        char OLDPWD[MAX_LEN];//para armazenar diretório antigo
        strcpy(OLDPWD, path);//diretório antigo armazena atual inicialmente
        while (expression_delimeters(line + argindex, "[^/ ]([^/]*[^/ ])?", &startExpr, &end) == 0)//enquanto encontrar diretórios ou '..' ou '~'
        {
            get_expression3(line + argindex, startExpr, end - startExpr, &command);//pega comando atual
            argindex += end;//vai pra próximo ponto de procura de comando

            if (strcmp(command, "~") == 0)//ir pro diretório home
            {
                char* HOME = getenv("HOME");
                strcpy(path, HOME);

                if (chdir(HOME) != 0)//muda pra home
                    perror("Nao foi possivel voltar pra home");

                else
                {
                    if (flag == 0)
                    {
                        setenv("OLDPWD", OLDPWD, 1);//seta OLDPWD
                        flag = 1;
                    }

                    setenv("PWD", path, 1);//seta PWD
                }
            }

            else
            {
                strcat(path, "/");//concatena barra
                strcat(path, command);//concatena argumento

                if (chdir(path) != 0)//se não conseguir avançar um diretório
                    perror("Nao foi possivel avancar um diretorio");
                else
                {
                    if (flag == 0)
                    {
                        setenv("OLDPWD", OLDPWD, 1);//seta OLDPWD
                        flag = 1;
                    }

                    setenv("PWD", path, 1);//seta PWD
                }
            }

            free(command);
        }
    }
}

void jobs(char* line)
{
    if (ps.fim == NULL)//se não tiver processo, retorna
    {
        fprintf(stderr, "sem jobs para mostrar\n");
        return;
    }

    no *n = ps.inicio;
    if (exist_expression(line, "^ *$") == 0)//é o comando jobs sem argumentos
    {
        while (n != NULL)
        {
            if (n == ps.atual)
                printaID_STATUS_PATH(n->processo.id, "[%d]+   ",n->processo.status, n->processo.path);
            else
                printaID_STATUS_PATH(n->processo.id, "[%d]    ",n->processo.status, n->processo.path);

            n = n->prox;
        }
    }

    else if (exist_expression(line, "^ *-p") == 0)//comando jobs com argumento -p. Imprime os pids dos processos
    {
        while (n != NULL)
        {
            printf("%d\n", (int)n->processo.pid);
            n = n->prox;
        }
    }

    else if (exist_expression(line, "^ *-l") == 0)//comando jobs com argumento -p. Imprime tudo que o primeiro caso imprime, + os pids
    {
        while (n != NULL)
        {
            if (n == ps.atual)
                printaID_PID_STATUS_PATH(n->processo.id, n->processo.pid, "[%d]+   ",n->processo.status, n->processo.path);
            else
                printaID_PID_STATUS_PATH(n->processo.id, n->processo.pid, "[%d]    ",n->processo.status, n->processo.path);

            n = n->prox;
        }
    }

    else
        fprintf(stderr, "opcao invalida, apenas -l e -p sao aceitos\n");
}

void bg(char* line)
{
    int end;//para armazenar fim do número

    if (ps.fim == NULL)//se não tiver processo, retorna
    {
        fprintf(stderr, "sem jobs para mostrar\n");
        return;
    }


    if (exist_expression(line, "^ *$") == 0)//é o comando bg sem argumentos
    {
        if (ps.atual->processo.status == BACKGROUND)
            fprintf(stderr, "processo atual ja esta em background\n");
        else
        {
            ps.atual->processo.status = BACKGROUND;//muda estado do processo para background
            kill (ps.atual->processo.pid, SIGCONT);//Estava suspenso, continua processo em background
            printf("processo atual colocado em background\n");
        }
    }

    else if (expression_delimeters2(line, "^ *[0-9]+", &end) == 0)//é o comando bg com o id do processo como argumento
    {
        int id;//id do processo(versão inteira, vai armazenar com atoi)
        char* jid = NULL;//id do processo(versão string, porque vou extrair do comando)
        char retorno;//armazena retorno da função
        get_expression2(line, end, &jid);//pega o número de id do processo
        id = atoi(jid);//armazena id do processo
        free(jid);//libera memória
        retorno = alterarEstadoProcesso(id, BACKGROUND);//altera estado do processo com id para BACKGROUND, processo com id também vira processo atual

        if (retorno == -1)
            fprintf(stderr, "Nao tem job com esse id\n");
        else if (retorno == -2)
            fprintf(stderr, "job %d ja esta em background\n", id);
        else
        {
            kill (ps.atual->processo.pid, SIGCONT);//Estava suspenso, continua processo em background
            printf("job %d colocado em background\n", id);
        }
    }

    else
        fprintf(stderr, "digite 'bg' ou 'bg n', onde n é um natural maior que 0\n");
}

void fg(char* line)
{
    int end, status;//para armazenar fim do número, status para wait

    if (ps.fim == NULL)//se não tiver processo, retorna
    {
        fprintf(stderr, "sem jobs para mostrar\n");
        return;
    }

    if (exist_expression(line, "^ *$") == 0)//é o comando fg sem argumentos
    {
        DefaultSignals();//sinais SIGTSTP e SIGINT vão apresentar comportamento padrão no processo filho
        tcsetpgrp(STDIN_FILENO, ps.atual->processo.pid);//seta controle do terminal pra novo grupo de processos, com o processo filho
        ps.atual->processo.status = FOREGROUND;//muda estado do processo para foreground
        kill (ps.atual->processo.pid, SIGCONT);//Estava suspenso, continua processo em foreground
        printf("processo atual colocado em foreground\n");
        while (waitpid(ps.atual->processo.pid, &status, WUNTRACED) == -1);//espera comando em foreground terminar. Precisa desse while(poderia ser um if, o wait executaria duas vezes, mas o while é + compacto) porque pode dar o erro EINTR, em que o SIGCHLD é capturado porque o job estava suspenso e voltou em foreground
        tcsetpgrp(STDIN_FILENO, getpgid(0));//processo pai retoma controle do terminal
    }

    else if (expression_delimeters2(line, "^ *[0-9]+", &end) == 0)//é o comando fg com o id do processo como argumento
    {
        int id;//id do processo(versão inteira, vai armazenar com atoi)
        char* jid = NULL;//id do processo(versão string, porque vou extrair do comando)
        char retorno;//armazena retorno da função
        get_expression2(line, end, &jid);//pega o número de id do processo
        id = atoi(jid);//armazena id do processo
        free(jid);//libera memória
        retorno = alterarEstadoProcesso(id, FOREGROUND);//altera estado do processo com id para FOREGROUND, processo com id também vira processo atual

        if (retorno == -1)
            fprintf(stderr, "Nao tem job com esse id\n");

        else
        {
            DefaultSignals();//sinais SIGTSTP e SIGINT vão apresentar comportamento padrão no processo filho
            tcsetpgrp(STDIN_FILENO, ps.atual->processo.pid);//seta controle do terminal pra novo grupo de processos, com o processo filho
            kill (ps.atual->processo.pid, SIGCONT);//Estava suspenso, continua processo em foreground
            printf("job %d colocado em foreground\n", id);
            while (waitpid(ps.atual->processo.pid, &status, WUNTRACED) == -1);//espera comando em foreground terminar. Precisa desse while(poderia ser um if, o wait executaria duas vezes, mas o while é + compacto) porque pode dar o erro EINTR, em que o SIGCHLD é capturado porque o job estava suspenso e voltou em foreground
            tcsetpgrp(STDIN_FILENO, getpgid(0));//processo pai retoma controle do terminal
        }
    }

    else
        fprintf(stderr, "digite 'fg' ou 'fg n', onde n é um natural maior que 0\n");
}

void printaEstado(char status)
{
    switch(status)//imprime estado do processo
    {
        case 1:printf("RUNNING");break;
        case 2:printf("STOPPED");break;
    }
}

void printaID_STATUS_PATH(int id, char* stringID, char status, char* path)
{
    printf(stringID, id);//imprime id do processo
    printaEstado(status);//imprime status do processo
    printf("    %s\n", path);
}

void printaID_PID_STATUS_PATH(int id, pid_t pid, char* stringID, char status, char* path)
{
    printf(stringID, id);//imprime id do processo
    printf("%d  ", (int)pid);//imprime pid do processo
    printaEstado(status);//imprime status do processo
    printf("    %s\n", path);
}
