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

char foregroundORbackground(char* line)
{
    int start, end;//começo e fim do '&'
    char* expression = NULL;//expressão encontrada, que é '&' ou '&[^>]'(& seguido de algo que não é >, senão seria redirecionamento de erro)

    if (expression_delimeters(line, "&($|[^>])", &start, &end) == 0)//se é pra rodar em background
    {
        get_expression(line, start, 0, end - start, &expression);
        line[start] = ' ';//troca por um simples espaço. As expressões regulares cuidam dos espaços, + o trim_leading_whitespace
        return BACKGROUND;//rodar em background
    }

    else
        return FOREGROUND;//rodar em foreground mesmo
}

void execute_external_command(char* line)
{
    char back_fore = foregroundORbackground(line);//armazena 0 se for foreground, 1 se for background
    pid_t pid = fork();
    int status;//para wait

    if (pid < 0)
        perror ("Fork");

    else if (pid > 0)//processo pai
    {
        setpgid(pid, pid);//seta id de grupo de processo do processo filho para seu própio pid, pra quando receber sinais SIGTTIN e SIGTTOUT, suspender
        tcsetpgrp(STDIN_FILENO, pid);//seta controle do terminal pra novo grupo de processos, com o processo filho

        inserirProcesso(pid, back_fore, line);//insere processo na lista de processos

        if (ps.atual->processo.status == FOREGROUND)//se for pra rodar em foreground
            waitpid(pid, &status, WUNTRACED);//espera filho terminar. Somente wait() por algum motivo não funciona quando aperta ctrl+c


        //printf("pid %d\n", pid);
        tcsetpgrp(STDIN_FILENO, getpgid(0));//processo pai retoma controle do terminal

    }

    else//filho
    {
        DefaultSignals();//sinais SIGTSTP e SIGINT vão apresentar comportamento padrão no processo filho
        setpgid(0, 0);//seta id de grupo de processo do processo filho para seu própio pid, pra quando receber sinais SIGTTIN e SIGTTOUT, suspender. Já fiz isso no pai, mas fiz aqui de novo porque o filho poderia passar pelo exec antes do pai setar o grupo
        //tcsetpgrp(STDIN_FILENO, pid);//seta controle do terminal pra novo grupo de processos, com o processo filho
        AnalyseCommand(line, 0);//executa comando
    }
}

void execute_commands(char* line, char* command)
{
    char back_fore = foregroundORbackground(command);//armazena 0 se for foreground, 1 se for background
    pid_t pid = fork();
    int status;//para wait

    if (pid < 0)
        perror ("Fork");

    else if (pid > 0)//processo pai
    {
        setpgid(pid, pid);//seta id de grupo de processo do processo filho para seu própio pid, pra quando receber sinais SIGTTIN e SIGTTOUT, suspender
        tcsetpgrp(STDIN_FILENO, pid);//seta controle do terminal pra novo grupo de processos, com o processo filho

        inserirProcesso(pid, back_fore, command);//insere processo na lista de processos

        if (ps.atual->processo.status == FOREGROUND)//se for pra rodar em foreground
            waitpid(pid, &status, WUNTRACED);//espera filho terminar. Somente wait() por algum motivo não funciona quando aperta ctrl+c

        tcsetpgrp(STDIN_FILENO, getpgid(0));//processo pai retoma controle do terminal

    }

    else//filho
    {
        DefaultSignals();//sinais SIGTSTP e SIGINT vão apresentar comportamento padrão no processo filho
        setpgid(0, 0);//seta id de grupo de processo do processo filho para seu própio pid, pra quando receber sinais SIGTTIN e SIGTTOUT, suspender. Já fiz isso no pai, mas fiz aqui de novo porque o filho poderia passar pelo exec antes do pai setar o grupo

        int startLine = 0, startExpr, end;//startLine é o começo da procura da expressão seguinte, startExpr é o começo da expressão encontrada, e end é o fim da expressão encontrada
        char* PreviousCommand = NULL;//comando anterior

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
                internal_external(PreviousCommand, 0);//executa comando, cuja saída vai pro região de escrita do pipe
            }

            else
               read_from_pipe(fd);//descritor stdin aponta pra região de leitura do pipe
        }

        internal_external(command, 0);//executa último comando
    }
}

void internal_external(char* command, char executa)
{
    char int_command = internal_command(command, executa);//para tentar executar comando interno
    if (int_command == 0)//não foi executado comando interno
        AnalyseCommand(command, 1);//executa comando externo, cuja saída vai pro região de escrita do pipe
    else
    {
        free(command);//libera comando anterior
        exit(1);//precisa sair do filho, se foi executado comando interno não retorna normalmente
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

void AnalyseCommand(char* command, char isPipe)
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
    if (isPipe == 1)
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
