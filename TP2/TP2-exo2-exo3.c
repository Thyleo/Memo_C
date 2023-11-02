#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
// Définir un handler pour SIGINT
void handlerCtrlC(int sig)
{
    printf("\nVeuiller confirmer votre Ctrl+C \n");
    alarm(3);
    signal(SIGINT, SIG_DFL);
}
// Définir un handler pour SIGALRM
void handlerAlarme(int sig)
{
    signal(SIGINT, handlerCtrlC);
}
int main()
{
    //signal(SIGINT, SIG_IGN); // Ignorer le signal
    signal(SIGINT, handlerCtrlC); // capture du signal SIGINT
    signal(SIGALRM, handlerAlarme); // capture du signal SIGALRM
    while(1)
    {
        printf("coucou\n");
        sleep(1);
    }
    return 0;
}
