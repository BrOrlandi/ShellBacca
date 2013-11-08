#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STRING_H
#define STRING_H
#include <string.h>
#endif

#ifndef MAX_LEN 
#define MAX_LEN 1024 //maior tamanho aceito de input
#endif

typedef struct{
    char str[MAX_LEN];
    int size;
}String;

String *CriarString(void);
int putString(String *s,char x);
int remString(String *s);
void clearString(String *s);
void StringCopy(String *s1, String *s2);
void StringCopyChar(String *s1, char *s2);
int StringLength(String *s);
