#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED

#include "parse.h"
#include "signal_handler.h"
#include "processes_handler.h"
#include "internal_commands.h"

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h> // para perror
#endif

#ifndef SYSTYPES_H
#define SYSTYPES_H
#include <sys/types.h>//para wait
#endif

#ifndef SYSWAIT_H
#define SYSWAIT_H
#include <sys/wait.h>//para wait
#endif

#ifndef UNISTD_H
#define UNISTD_H
#include <unistd.h> //para constantes do dup2
#endif

#ifndef FCNTL_H
#define FCNTL_H
#include <fcntl.h>//para constantes do comando open
#endif

#ifndef MAX_LEN
#define MAX_LEN 1024 //maior tamanho aceito de input
#endif

void execute_external_command(char* line);//executa comando externo
void internal_external(char* command, char executa);//executa comando interno ou externo
int input_redirection(char* line);//faz redirecionamento de entrada, se tiver.
void output_redirection(char* line);//faz redirecionamento de saída, se tiver
void execute_commands(char* line, char* command);//executa comandos
void free_Mem(char* prg_tok[]);//libera memória
void print_prg_tok(char* prg_tok[], int n);//para debug, imprime todos os argumentos
void execute_program(char* program);//executa programa
void AnalyseCommand(char* command, char isPipe);//analisa comando, e executa o programa se encontrar, e faz os redirecionamentos de entrada e saída, se tiver
void read_from_pipe(int* fd);//redireciona o descritor de arquivo stdin para apontar pra região de leitura do pipe
void write_to_pipe(int* fd);//redireciona o descritor de arquivo stout para apontar pra região de escrita do pipe
void create_pipe(int* fd, pid_t* pid);//passa os descritores de entrada e saída do pipe, respectivamente, para fd[0] e fd[1] e forka o processo
char foregroundORbackground(char* line);//Verifica se é pra rodar comando em background ou foreground. Retorna 1 pra background, 0 pra foreground

#endif // SHELL_H_INCLUDED
