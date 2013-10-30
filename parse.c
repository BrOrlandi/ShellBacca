#include "parse.h"

int expression_delimeters(char* line, char* pattern, int* start, int* end)
{
    regex_t regex;
    regmatch_t matchptr[1];
    int reti;

    regcomp(&regex, pattern, REG_EXTENDED);//compila expressão regular
    reti = regexec(&regex, line, 1, matchptr, 0);//executa expressão regular

    if(reti != 0)//se não encontrou expressão
        return reti;

    *start = matchptr[0].rm_so;//onde começa expressão
    *end = matchptr[0].rm_eo;//esse rm_eo é a última posição do último caracter da expressão + 1. Ou seja, o tamanho da string inteira em que se fez a busca, não o tamanho da expressão encontrada

    return reti;//deu tudo certo, vai retornar 0

}

int expression_delimeters2(char* line, char* pattern, int* end)
{
    regex_t regex;
    regmatch_t matchptr[1];
    int reti;

    regcomp(&regex, pattern, REG_EXTENDED);//compila expressão regular
    reti = regexec(&regex, line, 1, matchptr, 0);//executa expressão regular

    if(reti != 0)//se não encontrou expressão
        return reti;

    *end = matchptr[0].rm_eo;//esse rm_eo é a última posição do último caracter da expressão + 1. Ou seja, o tamanho da string inteira em que se fez a busca, não o tamanho da expressão encontrada

    return reti;//deu tudo certo, vai retornar 0

}

void get_expression(char* line, int start, int ignore, int size, char** expression)
{
    *expression = malloc(size + 1 - ignore);//um espaço a mais pro '\0'
    strncpy(*expression, line + start + ignore, size - ignore);//ignore para caso de redirecionamento
    expression[0][size - ignore] = '\0';//ultima posição recebe '\0'
    trim_leading_whitespace(*expression);//tira os espaços do começo
}

void get_expression2(char* line, int size, char** expression)
{
    *expression = malloc(size + 1);//um espaço a mais pro '\0'
    strncpy(*expression, line, size);
    expression[0][size] = '\0';//ultima posição recebe '\0'
    trim_leading_whitespace(*expression);//tira os espaços do começo
}

int count_expression(char* line, char* pattern)
{
    regex_t regex;//pra compilar e executar expressão regular
    regmatch_t matchptr[1];//para armazenar o começo e fim das expressões regulares

    regcomp(&regex, pattern, REG_EXTENDED);//compila expressão regular
    int offset = 0;
    int i = 0;
    while(!regexec(&regex, line + offset, 1, matchptr, 0))//enquanto encontrou expressão
    {
        ++i;//vai contando total de expressões
        offset += matchptr[0].rm_eo;//esse rm_eo é a última posição do último caracter da expressão + 1. Ou seja, o tamanho da string inteira em que se fez a busca, não o tamanho da expressão encontrada
    }

    return i;//retorna total de expressoes encontradas em String
}

void trim_leading_whitespace(char* string)//retira os espaços do começo da string(os do fim as expressões regulares resolvem)
{
    int i = 0;

    if (string[i] != ' ')//não tem espaço no começo, então sai
        return;

    while(string[i] == ' ')
        ++i;

    int size = strlen(string) - i + 1;//mais um pro '\0'
    strncpy(string, string + i, size - 1);//copia a string sem os espaços do começo
    string[size - 1] = '\0';//finaliza com um '\0'
}

void tokenize(char* cmd, char* cmd_tok[], char* token)
{
    char* pch;
    pch = strtok (cmd, token);
    int i = 0;
    while (pch != NULL)//vai pegando os argumentos
    {
        cmd_tok[i] = malloc(strlen(pch) + 1);
        strcpy(cmd_tok[i], pch);
        pch = strtok (NULL, token);
        ++i;
    }

    cmd_tok[i] = NULL;
}

int exist_expression(char* line, char* pattern)
{
    regex_t regex;
    regcomp(&regex, pattern, REG_EXTENDED);//compila expressão regular
    return regexec(&regex, line, 0, NULL, 0);//se deu tudo certo, vai retornar 0, senão vai retornar diferente de 0
}
