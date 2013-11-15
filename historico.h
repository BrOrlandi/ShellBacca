//Lista duplamente encadeada de strings de linha de comando
#ifndef HISTORICO_H_INCLUDED
#define HISTORICO_H_INCLUDED

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STRING_H
#define STRING_H
#include <string.h>
#endif

#ifndef MAX_LEN
#define MAX_LEN 1024 //maior tamanho aceito de input
#endif

typedef struct bloco{
    char info[MAX_LEN];
    struct bloco *ant,*prox;
}noh;

typedef struct{
    noh *ini,*fim;
}Historico;

Historico *CriarHistorico(void);
void InserirHistorico(Historico *lista, char *novo);
int IsEmptyHistorico(Historico *h);
noh *NavegarHistorico(Historico *h);
int MoverParaCima(noh **p, char *str);
int MoverParaBaixo(noh **p, char *str);
#endif//HISTORICO_H_INCLUDED
