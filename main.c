#include "shell.h"

int main()
{
    int loop = 1;
    char line[MAX_LEN], cwd[MAX_LEN];
    char int_command;

    while(loop)
    {
      printf("%s$: ", getcwd(cwd, sizeof(cwd)));
      gets(line);

      if (strlen(line) > MAX_LEN - 1)
      {
          fprintf(stderr, "Comando muito grande, ultrapassou os 1024 bytes de buffer\n");
          loop = 0;
      }

      else
      {
          int_command = internal_command(line);

          if (int_command == 0)//se foi executado comando externo
            external_command(line);//executa comando externo

          else if (int_command == 1)//se foi executado exit
            loop = 0;
      }

    }

    return EXIT_SUCCESS;
}
