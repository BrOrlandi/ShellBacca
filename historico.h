//Lista duplamente encadeada de strings de linha de comando

#include <stdlib.h>
#include <string.h>

#define LINHA 1024

typedef struct bloco{
    char info[LINHA];
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



