#include "stringcmd.h"

String *CriarString(void)
{
    String *n;
    n = (String *) malloc(sizeof(String));
    n->str[0] = '\0';
    n->size = 0;
    return n;
}

int putString(String *s,char x)
{
    if(s == NULL)
    {
        return -1;
    }
    else
    {
        if(s->size < MAX_LEN-1)
        {
            s->str[s->size++] = x;
            s->str[s->size] = '\0';
            return s->size;
        }
        else
        {
            return -2;
        }
    }
}

int remString(String *s)
{
    if(s == NULL)
    {
        return -1;
    }
    else
    {
        if(s->size > 0)
        {
            s->str[--s->size] = '\0';
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

void clearString(String *s)
{
    if(s != NULL)
    {
        s->str[0] = '\0';
        s->size = 0;
    }
}

void StringCopy(String *s1, String *s2)
{
    strcpy(s1->str,s2->str);
    s1->size = s2->size;
}

void StringCopyChar(String *s1, char *s2)
{
    strcpy(s1->str,s2);
    s1->size = strlen(s2);
}

int StringLength(String *s)
{
    return s->size;
}
