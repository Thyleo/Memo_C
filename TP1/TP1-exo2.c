#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{
    int pidFils1;
    if((pidFils1=fork())==0)
    {
        // Fils 1 : Afficher multiple de 2
        for(int i=1; i<=5;i++)
            printf("%5d",i*2);
        printf("\n");
        exit(16);
    }
    else
    {
        // Père :
        int etat;
        int pid ;
        pid = wait(&etat); // Bloque le père jusqu'à la terminaison du fils
        etat = WEXITSTATUS(etat);
        printf("pidFils = %5d , pid=%5d, etat = %d\n", pidFils1,pid,etat);
        if(fork()==0)
        {
            // Fils 2 : Afficher multiple de 3
            for(int i=1; i<=5;i++)
                printf("%5d",i*3);
            printf("\n");
        }
        else
        {
            wait(NULL);
            // Père : Afficher multiple de 4
            for(int i=1; i<=5;i++)
                printf("%5d",i*4);
            printf("\n");
        }
    }
    return 0;
}
