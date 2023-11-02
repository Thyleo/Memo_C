#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int *creerMemoirePartagee(key_t cle, int taille)
{
    int num = shmget ( cle, taille * sizeof(int), IPC_CREAT|0666);
    int *ptr = (int*) shmat ( num, NULL, 0);
    return ptr;
}

void detruireMem(key_t cle, int *ptr)
{
    shmdt (ptr) ;
    int num = shmget ( cle, 0, 0666);
    shmctl ( num, IPC_RMID, NULL);
}

void Init (key_t cle, int V)
{
    int Num = semget (cle, 0, 0);

    if (Num==-1) Num = semget (cle, 1, 0777|IPC_CREAT);
    semctl (Num, 0, SETVAL, V);

}

void Detruire (key_t cle)
{
    int Num = semget (cle, 0, 0);

    semctl (Num, 0, IPC_RMID, 0);
}

void P (key_t cle)
{
    int Num = semget (cle, 0, 0);

    struct sembuf TabOp;
    TabOp.sem_num = 0;
    TabOp.sem_op  = -1;
    TabOp.sem_flg = SEM_UNDO;     // bloquant

    int R = semop (Num, &TabOp, 1);
    if (R==-1) perror ("P");
}


void V (key_t cle)
{
    int Num = semget (cle, 0, 0);

    struct sembuf TabOp;
    TabOp.sem_num = 0;
    TabOp.sem_op  = 1;
    TabOp.sem_flg = SEM_UNDO;

    int R = semop (Num, &TabOp, 1);
    if (R==-1) perror ("V");
}

#define CLEMEM 20
#define CLESEM1 30
#define CLESEM2 40
#define N 50

int main()
{
    int *tab = creerMemoirePartagee(CLEMEM, N);
    tab[0]=3;
    for(int i=1; i<N; i++)
    {
        tab[i]=tab[i-1]+2;
    }
    Init(CLESEM1,0);
    Init(CLESEM2,0);
    int indFin ;

    if(fork()==0)
    {
        // FILS
        indFin = 2;
        while(indFin<99)
        {
            // Passage au père
            V(CLESEM2);
            P(CLESEM1);
            for(int i=0; i<indFin; i++)
            {
                if(tab[i]!=0 && tab[indFin]%tab[i]==0)
                {
                    tab[indFin]=0;
                    break;
                }
            }
            indFin+=2;
            // Afficher tableau
            printf("FILS : ");
            for(int i=0; i<N; i++)
            {
                if(tab[i]!=0)
                    printf("%4d",tab[i]);
            }
            printf("\n");

        }
    }
    else
    {
// PERE
        indFin = 1;
        while(indFin<99)
        {
            // Passage au père
            V(CLESEM1);
            P(CLESEM2);
            for(int i=0; i<indFin; i++)
            {
                if(tab[i]!=0 && tab[indFin]%tab[i]==0)
                {
                    tab[indFin]=0;
                    break;
                }
            }
            indFin+=2;
            // Afficher tableau
             printf("PERE : ");
            for(int i=0; i<N; i++)
            {
                if(tab[i]!=0)
                    printf("%4d",tab[i]);
            }
            printf("\n");

        }
        while(waitpid(-1,0,0)>0);
        detruireMem(CLEMEM,tab);
        Detruire(CLESEM1);
        Detruire(CLESEM2);
    }

    return 0;
}