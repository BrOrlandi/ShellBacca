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
            //printf("argindex %d, end %d, command %s, size %d\n", argindex, end, command, strlen(command));
            argindex += end;//vai pra próximo ponto de procura de comando

            if (strcmp(command, "..") == 0)//volta um diretório
            {
                char* pch;

                pch = strrchr(path,'/');//procura última barra
                int pos = pch - path;

                if (pos == 0)//se diretório anterior for raiz. Isso pode parecer desnecessário, mas não funciona fazer simplesmente path[pos+1] quando retorna diretório várias vezes em um comando
                    path[1] = '\0';//troca por fim de string
                else
                    path[pos] = '\0';//troca por fim de string

                if (chdir(path) != 0)//se não conseguir voltar um diretório
                     perror("Nao foi possivel voltar um diretorio");
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

            else//avançar um diretório
            {
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
            }

            free(command);
        }
    }
}

void jobs(char* line)
{
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

void printaEstado(char status)
{
    switch(status)//imprime estado do processo
    {
        case 1:printf("BACKGROUND");break;
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
