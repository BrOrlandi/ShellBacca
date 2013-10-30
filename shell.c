#include "parse.h"
#include "shell.h"

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
        char flag = 1;//=1 é concatenação, =0 é sobrescrita
        int fd;//descritor de arquivo

        if ( (start != 0 && line[start - 1] != '>') || start == 0)//se não tinha outro '>' atrás, então é sobrescrita
            flag = 0;

        get_expression(line, start, 1, end - start, &output);//a expressão regular é dessa forma justamente para pegar apenas um output redirection, não mais.

        if (flag == 1)//se foi encontrado concatenacao
            fd = open(output, O_WRONLY|O_APPEND|O_CREAT, 0644);

        else//foi encontrado sobrescrita
        {
            remove(output);//remove arquivo, se existir. Senão dá "erro", mas não me interessa.
            fd = open(output, O_WRONLY|O_CREAT, 0644);//abre arquivo
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
    int pid = fork(), status;

    if (pid < 0)
        perror ("Fork");

    else if (pid > 0)//processo pai
        wait (&status);//espera pelo filho

    else//filho
    {
        char* command = NULL;//para capturar comando externo
        int start, end;//começo e fim do comando externo
        char flag = 0;//flag para indicar que encontrou comando externo

        if (expression_delimeters2(line, "^( *[^<>| ]+)+", &end) == 0)//se encontrou comando externo no começo da linha
        {
            get_expression2(line, end, &command);
            flag = 1;
        }

        else if (expression_delimeters(line, "[^<>| ] ( *[^<>| ]+)+", &start, &end) == 0)//se encontrou comando externo em outro lugar
        {
            get_expression(line, start, 1, end - start, &command);
            flag = 1;
        }

        if (flag == 0)//se não encontrou comando externo retorna
            return;

        if (input_redirection(line) != 0)//faz redirecionamento de entrada, se tiver. Se arquivo não existir, retorna
            exit(1);

        output_redirection(line);//faz redirecionamento de saída, se tiver.

        int n = count_expression(command, " [^ ]") + 1;//conta total de argumentos(é o total de espaços seguidos de não espaços + 1)
        char* cmd_tok[n + 1];//vetor de strings, com o comando mais argumentos(o comando é um dos argumentos). + 1 porque o último é NULL
        tokenize(command, cmd_tok, " ");//armazena argumentos
        //print_cmd_tok(cmd_tok, n);
        if (execvp (cmd_tok[0], cmd_tok) < 0)//executa exec e verifica se deu erro ao mesmo tempo
            perror ("Execvp");

        free_Mem(cmd_tok);//se exec falhar, libera memória. Isso é necessário, senão temos um memory leak.
        exit(1);//se exec falhar, sai do processo filho
    }
}

void free_Mem(char* cmd_tok[])
{
    int i = 0;
    while(cmd_tok[i] != NULL)
    {
        free(cmd_tok[i]);
        ++i;
    }
}

void cd(char* line, int argindex)
{
    int end;
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
        expression_delimeters2(line + argindex, ".*[^ ]", &end);
        char* args = NULL;//vetor de argumentos. Esses argumentos podem ser ".." ou um diretório, ou "~"

        get_expression2(line + argindex, end, &args);//armazena argumentos

        int n = count_expression(args, "/.") + 1;//conta total de argumentos(é o total de "/" seguido de qualquer coisa + 1)

        char* args_tok[n + 1];//vetor de strings, com os path ou ".." ou "~". + 1 porque o último argumento recebe null
        tokenize(args, args_tok, "/");//armazena argumentos

        int i = 0;//para caminhar pelos argumentos
        //print_cmd_tok(args_tok, n);

        char flag = 0;
        char OLDPWD[MAX_LEN];
        strcpy(OLDPWD, path);

        while (args_tok[i] != NULL)
        {
            if (strcmp(args_tok[i], "..") == 0)//volta um diretório
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
                if (strcmp(args_tok[i], "~") == 0)//ir pro diretório home
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
                    strcat(path, args_tok[i]);//concatena argumento

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

            ++i;
        }

        free_Mem(args_tok);//libera memória
    }
}

void print_cmd_tok(char* cmd_tok[], int n)
{
    int i = 0;

    printf("total de argumentos: %d\n", n);
    while(cmd_tok[i] != NULL)
    {
        printf("arg[%d]: %s len: %d\n", i, cmd_tok[i], (int)strlen(cmd_tok[i]));
        ++i;
    }
}
