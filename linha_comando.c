#include "linha_comando.h"


void LinhaDeComando(char *linha, Historico *historico)
{
    
    char x; // char lido
    int key; // verifica se apertou seta direcional
    String *linhaString = CriarString();
    noh *hPointer;
    
    hPointer = NavegarHistorico(historico);
    do
    {
        x = GetChar();
        key = isValidChar(x);
        if(key == 1)
        {
            if(putString(linhaString,x) >= 0)
            {
                printf("%c",x);
            }
        }
        else if(key == ARROW_UP)
        {
            //printf("ARROW_UP");
            char hist[MAX_LEN];
            if(MoverParaCima(&hPointer,hist))
            {
                BackSpace(StringLength(linhaString));
                StringCopyChar(linhaString,hist);
                printf("%s",hist);
            }
        }
        else if(key == ARROW_DOWN)
        {
            //printf("ARROW_DOWN");
            char hist[MAX_LEN];
            if(MoverParaBaixo(&hPointer,hist))
            {
                BackSpace(StringLength(linhaString));
                StringCopyChar(linhaString,hist);
                printf("%s",hist);
            }
        }
        else if(key == ARROW_LEFT)
        {
            //printf("ARROW_LEFT");
        }
        else if(key == ARROW_RIGHT)
        {
            //printf("ARROW_RIGHT");
        }
        else if(x == BACKSPACE)
        {
            if(remString(linhaString) == 1)
            {
                BackSpace(1);
            }
        }
    }while(x != 10); // apertou enter
    if(strcmp(linhaString->str,"") != 0)
    {
        InserirHistorico(historico, linhaString->str);     
    }
    strcpy(linha,linhaString->str);
    printf("\n");
    free(linhaString);
}

int isValidChar(char x)
{
    if(x >= 32 && x <= 126)
    {
        return 1;
    }
    else if(x == 27)
    {
        char c1,c2;
        if(ApertouTecla())
        {
            c1 = GetChar();
        }
        if(ApertouTecla())
        {
            c2 = GetChar();
        }
        if(c1 == 91)
        {
            if(c2 == 65)
            {
                return ARROW_UP;
            }
            else if(c2 == 66)
            {
                return ARROW_DOWN;
            }
            else if(c2 == 67)
            {
                return ARROW_RIGHT;
            }
            else if(c2 == 68)
            {
                return ARROW_LEFT;
            }
        }
    }
    return 0;
}

void BackSpace(int n)
{
    int i;
    for(i=0;i<n;i++)
    {
        printf("%c%c%c",8,32,8);
    }
}
