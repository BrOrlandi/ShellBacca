#ifndef INTERNAL_COMMANDS_H_INCLUDED
#define INTERNAL_COMMANDS_H_INCLUDED

#include "parse.h"
#include "processes_handler.h"
#include "signal_handler.h"

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h> // para perror
#endif

#ifndef UNISTD_H
#define UNISTD_H
#include <unistd.h> //para constantes do dup2
#endif

#ifndef SYSTYPES_H
#define SYSTYPES_H
#include <sys/types.h>
#endif

#ifndef SYSWAIT_H
#define SYSWAIT_H
#include <sys/wait.h>//para wait
#endif

#ifndef SIGNAL_H
#define SIGNAL_H
#include <signal.h>
#endif

#ifndef MAX_LEN
#define MAX_LEN 1024 //maior tamanho aceito de input
#endif

char internal_command(char* line, char executa);//comandos internos.Retorna 1 se for exit, 2 se for cd, 3 se for bg, 4 se for fg, -5 se for jobs, 0 se não for um comando interno
void cd(char* line, int argindex);//comando CD. Esse argindex é simplesmente o começo dos argumentos de CD. Normalmente seria 2(C = 0, D = 1), mas pode ter um monte de espaço antes
void jobs(char* line);//comando jobs. O processo atual é o último inserido, a não ser que o comando fg mude isso. Mostra processos suspensos e rodando em background.
void bg(char* line, char executa);//comando bg.
void fg(char* line, char executa);//comando fg. Retorna 0 se der tudo certo, -1 se processo corrente não tiver filho(pipe), -2 se não tiver jobs pra mostrar, -3 se não tiver job com id passado

void printaEstado(char status);//função auxiliar de jobs, recebe status e imprime uma string correspondente na saida padrao
void printaID_STATUS_PATH(int id, char* stringID, char status, char* path);//função auxiliar de jobs, recebe os argumentos para imprimir id, status e path
void printaID_PID_STATUS_PATH(int id, pid_t pid, char* stringID, char status, char* path);//função auxiliar de jobs

__attribute__((weak)) int setenv(const char *name, const char *value, int overwrite);
#endif // INTERNAL_COMMANDS_H_INCLUDED
