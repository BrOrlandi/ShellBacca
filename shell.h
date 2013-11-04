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
void free_Mem(char* prg_tok[]);//libera memória
void cd(char* line, int argindex);//comando CD. Esse argindex é simplesmente o começo dos argumentos de CD. Normalmente seria 2(C = 0, D = 1), mas pode ter um monte de espaço antes
void print_prg_tok(char* prg_tok[], int n);//para debug, imprime todos os argumentos
void execute_program(char* program);//executa programa
void AnalyseCommand(char* command);//analisa comando, e executa o programa se encontrar, e faz os redirecionamentos de entrada e saída, se tiver
void read_from_pipe(int* fd);//redireciona o descritor de arquivo stdin para apontar pra região de leitura do pipe
void write_to_pipe(int* fd);//redireciona o descritor de arquivo stout para apontar pra região de escrita do pipe
void create_pipe(int* fd, pid_t* pid);//passa os descritores de entrada e saída do pipe, respectivamente, para fd[0] e fd[1] e forka o processo

__attribute__((weak)) int setenv(const char *name, const char *value, int overwrite);

#endif // SHELL_H_INCLUDED
