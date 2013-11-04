#include "historico.h"

Historico *CriarHistorico(void)
{
    Historico *h;
    h = (Historico *) malloc(sizeof(Historico));
    h->ini = h->fim = NULL;
}

void InserirHistorico(Historico *h, char *novo)
{
    noh *n;
    n = (noh *) malloc(sizeof(noh));
    if(n==NULL)
    {
        exit(1);
    }
    if(IsEmptyHistorico(h))
    {
        h->ini = n;
        noh *b = (noh *) malloc(sizeof(noh));
        strcpy(b->info,"");
        b->ant = n;
        b->prox = NULL;
		n->prox = b;
		n->ant = NULL;	
        h->fim = b;
    }
    else
    {
        noh *p = h->fim;
        h->fim = h->fim->ant;
        free(p);
        
		h->fim->prox = n;
		n->ant = h->fim;
		
		noh *b = (noh *) malloc(sizeof(noh));
		strcpy(b->info,"");
		b->ant = n;
		b->prox = NULL;
		
		n->prox = b;
		h->fim = b;
	}
    strcpy(n->info,novo);
}

int IsEmptyHistorico(Historico *h)
{
    if(h->ini == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

noh *NavegarHistorico(Historico *h)
{
    return h->fim;
}

int MoverParaCima(noh **p, char *str)
{
    if(*p != NULL)
    {
        if((*p)->ant != NULL)
        {
            *p = (*p)->ant;
        }
        strcpy(str,(*p)->info);
        return 1;
    }
    return 0;
}

int MoverParaBaixo(noh **p, char *str)
{
    if(*p != NULL)
    {
        if((*p)->prox != NULL)
        {
            *p = (*p)->prox;
        }
        strcpy(str,(*p)->info);
        return 1;
    }
    return 0;
}
