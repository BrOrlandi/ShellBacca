#ifndef PROCESSES_HANDLER_H_INCLUDED
#define PROCESSES_HANDLER_H_INCLUDED

#ifndef SYSTYPES_H
#define SYSTYPES_H
#include <sys/types.h>//para pid_t
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STRING_H
#define STRING_H
#include <string.h>
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h> // para perror
#endif

#ifndef MAX_LEN
#define MAX_LEN 1024 //maior tamanho aceito de input
#endif

#define FOREGROUND 0
#define BACKGROUND 1
#define STOPPED 2

typedef struct
{
    pid_t pid;//pid do processo
    int id;//id do processo. Isso é pro comando fg e bg, que não utilizam o pid
    char status;//estado do processo
    char path[MAX_LEN];//armazena comando, com caminho e tudo
} Processo;

typedef struct block
{
    Processo processo;
    struct block *ant, *prox;
} no;//cada no tem um ponteiro pro nó anterior e pro próximo, e cada nó contém um processo

typedef struct
{
    no *atual, *inicio, *fim;//lista de processos tem ponteiro pro processo atual e pro fim da lista de processos. O inicio serve só para imprimir os processos
} Processos;

extern Processos ps;//lista de processos. Infelizmente mais uma variável global teve de ser usada, culpa do handler do sigaction que não permite mais de um argumento

void inicializarListaProcessos();//para inicializar lista de processos. Sempre deve ser executada antes de qualquer outra
void destruirListaProcessos();//destrói lista de processos
void inserirProcesso (int pid, int status, char path[]);//insere processo em lista de processos
char alterarEstadoProcesso(int id, char status);//altera estado do processo com id para status. Processo com id também passa a ser o atual. Retorna -1 se processo com id especificado não for encontrado, -2 se processo encontrado já tiver estado do parâmetro, 0 se encontrou processo e atribuiu estado
void alterarEstadoUltimoProcesso(char status);//altera estado do último processo
void imprimirProcessos();//imprime lista de processos, para debug somente
void removerProcesso(pid_t pid);//remove processo atual
void removerProcessoAtual();//remove o processo atual.

#endif // PROCESSES_HANDLER_H_INCLUDED
