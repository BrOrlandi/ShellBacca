#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED

#include "parse.h"
#include <stdio.h>//para perror
#include <sys/types.h>//para wait
#include <sys/wait.h>//para wait
#include <unistd.h>//para constantes do dup2
#include <fcntl.h>//para constantes do comando open

#define MAX_LEN 1025//maior tamanho aceito de input, 1024 + 1 pro '\0'

char internal_command(char* line);//executa comandos internos, se tiver. Retorna 1 se for digitado exit, 2 se for CD, 0 se não for nenhum comando interno
int input_redirection(char* line);//faz redirecionamento de entrada, se tiver.
void output_redirection(char* line);//faz redirecionamento de saída, se tiver
void external_command(char* line);//executa comando externo, se tiver
void free_Mem(char* cmd_tok[]);//libera memória
void cd(char* line, int argindex);//comando CD. Esse argindex é simplesmente o começo dos argumentos de CD. Normalmente seria 2(C = 0, D = 1), mas pode ter um monte de espaço antes
void print_cmd_tok(char* cmd_tok[], int n);//para debug, imprime todos os argumentos

__attribute__((weak)) int setenv(const char *name, const char *value, int overwrite);

#endif // SHELL_H_INCLUDED
