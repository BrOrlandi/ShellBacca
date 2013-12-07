#include "processes_handler.h"
Processos ps;

void inicializarListaProcessos ()
{
    ps.fim = ps.inicio = ps.atual = NULL;
}

void destruirListaProcessos ()
{
    no *n;
    n = ps.fim;
    while (n != NULL)
    {
        kill (n->processo.pid, SIGKILL);//mata processo
        ps.fim = ps.fim->ant;
        free (n);
        n = ps.fim;
    }
}

void imprimirProcessos ()
{
    no *n;
    n = ps.inicio;

    while (n != NULL)
    {
        printf ("pid: %d, status ", n->processo.pid);
        switch(n->processo.status)
        {
            case 1:printf("BACKGROUND");break;
            case 2:printf("STOPPED");break;
        }

        printf(", path: %s\n", n->processo.path);
        n = n->prox;
    }
}

void inserirProcesso (int pid, int status, char path[])
{
    no *n = (no*) malloc (sizeof(no));
    if (n == NULL)
    {
        fprintf(stderr, "Nao foi possivel alocar um novo no para armazenar o processo\n");
        exit(1);
    }

    if (ps.inicio == NULL)//se for primeiro processo
    {
        ps.inicio = ps.fim = n;
        n->ant = NULL;
        n->prox = NULL;
        n->processo.id = 1;//primeiro processo recebe id = 1
    }

    else//sempre insere no final
    {
        ps.fim->prox = n;
        n->ant = ps.fim;
        ps.fim = n;
        n->prox = NULL;
        n->processo.id = n->ant->processo.id + 1;//os próximos processos recebem id anterior + 1
    }

    n->processo.pid = pid;//armazena pid do processo
    n->processo.status = status;//armazena seu estado
    strcpy(n->processo.path, path);//armazena seu caminho completo
    ps.atual = n;//processo atual é o que acabou de ser inserido
}

char alterarEstadoProcesso(int id, char status)
{
    no* atual = ps.atual;

    while (ps.atual != NULL && ps.atual->processo.id < id)
        ps.atual = ps.atual->prox;

    while (ps.atual != NULL && ps.atual->processo.id > id)
        ps.atual = ps.atual->ant;

    if (ps.atual == NULL)
    {
        ps.atual = atual;//volta processo atual pro original
        return -1;
    }

    else if (ps.atual->processo.status == status)//se processo atual já estiver com estado do parâmetro
    {
        ps.atual = atual;//volta processo atual pro original
        return -2;
    }

    ps.atual->processo.status = status;//processo atual recebe novo estado
    return 0;
}

void alterarEstadoUltimoProcesso(char status)
{
    ps.atual->processo.status = status;
}

void removerProcesso(pid_t pid)
{
    no* n = ps.atual;//recebe processo atual

    while (n->processo.pid < pid)
        n = n->prox;

    while (n->processo.pid > pid)
        n = n->ant;

    if (n->ant == NULL && n->prox == NULL)//só tinha um processo
        ps.atual = ps.inicio = ps.fim = NULL;

    else if (n->ant == NULL)//remover primeiro processo, mas não é único
    {

        n->prox->ant = NULL;
        ps.inicio = n->prox;

        if (n == ps.atual)//se n for o processo atual
            ps.atual = n->prox;
    }


    else if (n->prox == NULL)//remover último processo, mas não é único
    {
        n->ant->prox = NULL;
        ps.fim = n->ant;

        if (n == ps.atual)//se n for o processo atual
            ps.atual = n->ant;
    }

    else//remove algum processo que não é primeiro nem último
    {
        n->ant->prox = n->prox;
        n->prox->ant = n->ant;

        if (n == ps.atual)//se n for o processo atual
            ps.atual = n->prox;
    }

    free(n);

}

void removerProcessoAtual()
{
    no* atual = ps.atual;

    if (ps.atual->ant == NULL && ps.atual->prox == NULL)//só tinha um processo
        ps.atual = ps.inicio = ps.fim = NULL;

    else if (ps.atual->ant == NULL)//remover primeiro processo, mas não é único
    {
        ps.atual = ps.inicio = ps.atual->prox;
        ps.atual->ant = NULL;
    }


    else if (ps.atual->prox == NULL)//remover último processo, mas não é único
    {
        ps.atual = ps.fim = ps.atual->ant;
        ps.atual->prox = NULL;
    }

    else//remove algum processo que não é primeiro nem último
    {
        ps.atual->ant->prox = ps.atual->prox;
        ps.atual->prox->ant = ps.atual->ant;
        ps.atual = ps.atual->prox;
    }

    free(atual);
}
