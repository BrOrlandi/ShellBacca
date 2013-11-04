#include "historico.h"
#include "LinuxUtils.h"
#include "stringcmd.h"
#include <stdio.h>
#include <stdlib.h>

#define LINHA 1024

#define ARROW_UP 11
#define ARROW_DOWN 12
#define ARROW_RIGHT 13
#define ARROW_LEFT 14
#define BACKSPACE 127

void LinhaDeComando(char *linha, Historico *historico);
int isValidChar(char x);
void BackSpace(int n);

