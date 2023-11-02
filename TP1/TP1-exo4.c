#include <stdio.h>
#include <stdlib.h>

int main()
{
    if(fork()==0)
    {
        // Fils
        printf("Liste du sossier : \n");
        execl("/bin/ls","ls","-l", NULL);
    }
    else
    {
        // Père
        wait(NULL);
        if(fork()==0)
        {
            // Fils 2
            printf("Liste des processus en cours : \n");
            execl("/bin/ps","ps","-la", NULL);
        }
        else
        {
            wait(NULL);
            printf("Chemin courant : \n");
            execl("/bin/pwd","pwd", NULL);
        }

    }
    printf("Bonjour\n");
    return 0;
}
