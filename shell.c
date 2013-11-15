#include "shell.h"

void execute_program(char* program)
{
    int n = count_expression(program, " [^ ]") + 1;//conta total de argumentos(é o total de espaços seguidos de não espaços + 1)
    char* prg_tok[n + 1];//vetor de strings, com o programa mais argumentos(o programa é um dos argumentos). + 1 porque o último é NULL
    tokenize(program, prg_tok, " ");//armazena argumentos
    //print_prg_tok(prg_tok, n);
    if (execvp (prg_tok[0], prg_tok) < 0)//executa exec e verifica se deu erro ao mesmo tempo
        perror ("Execvp");

    free_Mem(prg_tok);//se exec falhar, libera memória. Isso é necessário, senão temos um memory leak.
    free(program);//libera programa
    exit(1);//se exec falhar, sai do processo filho
}

char internal_command(char* line)
{
    if (exist_expression(line, "^ *exit *$") == 0)
        return 1;//exit, retorna 1

    else
    {
        int end;//primeira posição depois de "cd" ou "cd "
        if (expression_delimeters2(line, "^ *cd($| )", &end) == 0)//se for só cd e terminar, ou for cd seguido de um espaço, é o comando cd
        {
            cd(line, end);//executa comando cd
            return 2;//retorna 2
        }
    }

    return 0;//se não foi e nenhum comando interno, retorna 0
}

int input_redirection(char* line)
{
    int start, end;//para armazenar começo e fim da expressão encontrada

    if (expression_delimeters(line, "< *[^<>| ]+", &start, &end) == 0)//se encontrou redirecionamento de entrada
    {
        char* input = NULL;

        get_expression(line, start, 1, end - start, &input);//a expressão regular é dessa forma justamente para pegar apenas um input redirection, não mais.

        int fd = open(input, O_RDONLY, 0);//descritor de arquivo

        if (fd < 0)
        {
            perror("Failed to Open File");
            return 1;
        }


        if (dup2(fd, STDIN_FILENO) < 0)//duplica descritor de arquivo, para STDIN_FILENO apontar para o arquivo aberto
        {
            perror("Failed to duplicate File Descriptor");
            return 2;
        }


        close(fd);//não precisa de dois descritores apontando pro mesmo arquivo, então fecha este.

        free(input);//já abriu o arquivo, já tem o descritor apontando pra ele, então libero a string
    }

    return 0;
}

void output_redirection(char* line)
{
    int start, end;//para armazenar começo e fim da expressão encontrada

    if (expression_delimeters(line, "> *[^<>| ]+", &start, &end) == 0)//se encontrou redirecionamento de saída
    {
        char* output = NULL;
        char flag = 0;//=0 é sobrescrita, =1 é concatenação, =2 é sobrescrita e redirecionamento de erro
        int fd;//descritor de arquivo

        if (start != 0)
        {
            if (line[start - 1] == '>')
                flag = 1;

            else if (line[start - 1] == '&')
                flag = 2;

        }

        get_expression(line, start, 1, end - start, &output);//a expressão regular é dessa forma justamente para pegar apenas um output redirection, não mais.

        if (flag == 1)//se foi encontrado concatenacao
            fd = open(output, O_WRONLY|O_APPEND|O_CREAT, 0644);

        else//foi encontrado sobrescrita ou sobrescrita e redirecionamento de erro
        {
            remove(output);//remove arquivo, se existir. Senão dá "erro", mas não me interessa.
            fd = open(output, O_WRONLY|O_CREAT, 0644);//abre arquivo
            if (flag == 2)//se for sobrescrita e redirecionamento de erro
            {
                if (dup2(fd, STDERR_FILENO) < 0)//duplica descritor de arquivo, para STDERR_FILENO apontar para o arquivo aberto
                    perror("Failed to duplicate File Descriptor");
            }
        }

        if (fd < 0)
            perror("Failed to Open File");

        if (dup2(fd, STDOUT_FILENO) < 0)//duplica descritor de arquivo, para STDOUT_FILENO apontar para o arquivo aberto
            perror("Failed to duplicate File Descriptor");

        close(fd);//não precisa de dois descritores apontando pro mesmo arquivo, então fecha este.

        free(output);//já abriu o arquivo, já tem o descritor apontando pra ele, então libero a string
    }
}


void external_command(char* line)
{
    signal_handler_pid = fork();
    int status = 0;

    if (signal_handler_pid < 0)
        perror ("Fork");

    else if (signal_handler_pid > 0)//processo pai
    {
        SendSignalsToChild();//captura sinais e envia pro filho
        waitpid(signal_handler_pid, &status, 0);//espera filho terminar. Somente wait() por algum motivo não funciona quando aperta ctrl+c
    }

    else//filho
    {
        int startLine = 0, startExpr, end;//startLine é o começo da procura da expressão seguinte, startExpr é o começo da expressão encontrada, e end é o fim da expressão encontrada
        char* command = NULL;//comando atual
        char* PreviousCommand = NULL;//comando anterior

        if (expression_delimeters(line + startLine, "[^| ]([^|]*[^| ])?", &startExpr, &end) == 0)//se encontrou um comando
        {
            get_expression3(line + startLine, startExpr, end - startExpr, &command);//captura o comando
            startLine += end;//vai pro próximo ponto de busca
        }

        else//se não encontrou nenhum comando
            exit(1);

        while (expression_delimeters(line + startLine, "[^| ]([^|]*[^| ])?", &startExpr, &end) == 0)//enquanto encontrar comandos entre pipes
        {
            PreviousCommand = command;//comando anterior recebe comando
            get_expression3(line + startLine, startExpr, end - startExpr, &command);//pega comando atual
            startLine += end;//vai pra próximo ponto de procura de comando

            int fd[2];//para armazenar descritores de arquivo da entrada do pipe e saída do pipe, respectivamente
            pid_t pid;

            create_pipe(fd, &pid);//cria pipe

            if (pid < 0)//se deu pau no fork
                perror ("Fork");

            if (pid == 0)//se for processo filho
            {
                write_to_pipe(fd);//descritor stdout aponta pra região de escrita do pipe
                AnalyseCommand(PreviousCommand);//executa comando, cuja saída vai pro região de escrita do pipe
            }

            else
               read_from_pipe(fd);//descritor stdin aponta pra região de leitura do pipe
        }

        AnalyseCommand(command);//analisa último comando

    }
}

void free_Mem(char* prg_tok[])
{
    int i = 0;
    while(prg_tok[i] != NULL)
    {
        free(prg_tok[i]);
        ++i;
    }
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

void print_prg_tok(char* prg_tok[], int n)
{
    int i = 0;

    printf("total de argumentos: %d\n", n);
    while(prg_tok[i] != NULL)
    {
        printf("arg[%d]: %s len: %d\n", i, prg_tok[i], (int)strlen(prg_tok[i]));
        ++i;
    }
}

void AnalyseCommand(char* command)
{
    char* program = NULL;//para capturar programa externo
    int start, end;//começo e fim do programa externo
    char flag = 0;//flag para indicar que encontrou programa externo

    if (expression_delimeters2(command, "^( *[^&<> ]+)+", &end) == 0)//se encontrou programa externo no começo do comando
    {
        get_expression2(command, end, &program);
        flag = 1;
    }

    else if (expression_delimeters(command, "[^<> ] ( *[^<> ]+)+", &start, &end) == 0)//se encontrou programa externo em outro lugar
    {
        get_expression(command, start, 1, end - start, &program);
        flag = 1;
    }

    if (flag == 0)//se não encontrou programa externo retorna
        exit(1);

    if (input_redirection(command) != 0)//faz redirecionamento de entrada, se tiver. Se arquivo não existir, retorna
        exit(1);

    output_redirection(command);//faz redirecionamento de saída, se tiver.
    free(command);//libera comando
    execute_program(program);//executa programa
}

void read_from_pipe(int* fd)
{
    close(fd[1]);//fecha escrever no pipe
    dup2(fd[0], STDIN_FILENO);//stdin aponta pra região de leitura do pipe
    close(fd[0]);//não precisa de dois descritores de arquivo apontando pro mesmo lugar
}

void write_to_pipe(int* fd)
{
    close(fd[0]);//fecha leitura do pipe
    dup2(fd[1], STDOUT_FILENO);//stdout aponta pra região de escrita do pipe
    close(fd[1]);//não precisa de dois descritores de arquivo apontando pro mesmo lugar
}

void create_pipe(int* fd, pid_t* pid)
{
    if (pipe(fd) != 0)//passa os descritores do pipe
        perror("pipe");

    *pid = fork();//tenta dar um fork
}
