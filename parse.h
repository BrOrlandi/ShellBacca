#ifndef PARSE_H_INCLUDED
#define PARSE_H_INCLUDED

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef REGEX_H
#define REGEX_H
#include <regex.h>
#endif

#ifndef STRING_H
#define STRING_H
#include <string.h>
#endif

/*O end é a última posição do último caracter da expressão encontrada + 1. Ou seja, o tamanho da string inteira em que se fez a busca(até encontrar o último caracter), não o tamanho da expressão encontrada
Por exemplo, supondo a string "abcdefgh" e se comece a procurar a partir de b(entao a posicão de b é a 0) pra encontrar "ef", end seria 5*/
/*start é o ponteiro pro começo da expressão encontrada*/

/*função feita para extrair a primeira expressão encontrada dada por pattern em uma linha.*/
int expression_delimeters(char* line, char* pattern, int* start, int* end);
/*Mesmo que a função de cima, mas sem armazenar a posição do começo da expressão encontrada. Usada para casos em que se sabe que o primeiro caracter da expressão é na posição 0*/
int expression_delimeters2(char* line, char* pattern, int* end);
/*função feita para armazenar a expressão encontrada em expression_delimeters. ignore é a quantidade de caracteres que devem ser ignorados do começo da expressão encontrada,
que vai ser retornada em expression. Por exemplo, se temos a expressão >out.txt e queremos pular o '>', basta fazer ignore = 1
IMPORTANTE: sempre dar free em expression depois de usar as funções da famíla get_expression, pois utilizam malloc pra armazenar a expressão*/
void get_expression(char* line, int start, int ignore, int size, char** expression);
/*Mesmo que a função de cima, mas se sabe que o começo da expressão está na posição 0*/
void get_expression2(char* line, int size, char** expression);
/*mesmo que a primeira versão da função, mas sem tirar os espaços do começo da expressão encontrada, e nunca ignorar nada. Usado quando a expressão regular pode cuidar disso sozinha sem prejuizo*/
void get_expression3(char* line, int start, int size, char** expression);
/*a função abaixo simplesmente conta o total de expressões definidas por pattern em uma linha*/
int count_expression(char* line, char* pattern);
/*tira os espaços em branco do começo da string*/
void trim_leading_whitespace(char* string);
/*quebra os argumentos de um comando cmd e armazena em cmd_tok, token é a string q vai ser usada pra quebrar o comando*/
void tokenize(char* cmd, char* cmd_tok[], char* token);
/*Verifica se a expressão dada por pattern existe em line*/
int exist_expression(char* line, char* pattern);

#endif // PARSE_H_INCLUDED
