#include "LinuxUtils.h"

int GetChar(void)
{
    int ch;
    #ifdef WIN32
    ch = getch();
    #else
    fflush(stdin);
    struct termios oldt,newt; // códigos padroes para usar o getch() no linux
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();         // usa o getch() da biblioteca ncurses.h
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    #endif
    return ch;
}

int ApertouTecla(void)
{
    // kbhit no Linux
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
    ungetc(ch, stdin);
    return 1;
    }

    return 0;
}

