#include "shell.h"
#include "internal_commands.h"
#include "linha_comando.h"

int main()
{
    SIGCHLD_SIGHUP();//seta handler pra esses dois sinais

    int loop = 1;
    char line[MAX_LEN], cwd[MAX_LEN];

    Historico *historico = CriarHistorico();
    inicializarListaProcessos();//inicializa lista de processos

    int startExpr, end;//startExpr é o começo da expressão encontrada, e end é o fim da expressão encontrada
    char* command = NULL;//comando digitado
    
    IgnoreSignals();//ignora sinais SIGINT e SIGTSTP
    chewbacca();
    while(loop)
    {
      IgnoreSignals();//ignora sinais SIGINT e SIGTSTP
      printf("%s$: ", getcwd(cwd, sizeof(cwd)));

      LinhaDeComando(line, historico);

      if (strlen(line) > MAX_LEN - 1)
      {
          fprintf(stderr, "Comando muito grande, ultrapassou os 1024 bytes de buffer\n");
          loop = 0;
      }

      else
      {
          if (strchr(line, '|') == NULL)//se não encontrou simbolo de pipe
          {
              char int_command = internal_command(line, 1);//tenta executar comando interno, esse 1 é pro comando bg/fg
              if (int_command == 1)//é o exit
                loop = 0;
              else if (int_command == 0)//se não foi executado comando interno
              {
                  execute_external_command(line);
              }

          }

          else//tem o símbolo de pipe
          {
              if (expression_delimeters(line, "[^| ]([^|]*[^| ])?", &startExpr, &end) == 0)//se encontrou um comando
              {
                  get_expression3(line, startExpr, end - startExpr, &command);//captura o comando
                  execute_commands(line + end, command);//executa comandos
              }
          }
      }

    }

    return EXIT_SUCCESS;
}

