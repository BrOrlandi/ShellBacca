#include "historico.h"
#include "LinuxUtils.h"
#include "stringcmd.h"

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef MAX_LEN 
#define MAX_LEN 1024 //maior tamanho aceito de input
#endif

#define ARROW_UP 11
#define ARROW_DOWN 12
#define ARROW_RIGHT 13
#define ARROW_LEFT 14
#define BACKSPACE 127

void LinhaDeComando(char *linha, Historico *historico);
int isValidChar(char x);
void BackSpace(int n);

