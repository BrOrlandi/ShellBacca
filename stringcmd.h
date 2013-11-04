#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINHA 1024

typedef struct{
    char str[LINHA];
    int size;
}String;

String *CriarString(void);
int putString(String *s,char x);
int remString(String *s);
void clearString(String *s);
void StringCopy(String *s1, String *s2);
void StringCopyChar(String *s1, char *s2);
int StringLength(String *s);
