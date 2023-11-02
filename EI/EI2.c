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
void remplirtab(int *tab, int taille)
{
    for(int i=0 ; i< taille ; i++)
    {
        tab[i] = rand() % 100 ; // Valeurs alétoires entre 0 et 100
    }
}
void affichertab(int *tab, int taille)
{
    for(int i=0 ; i< taille ; i++)
    {
        printf("%4d", tab[i]);
    }
    printf("\n");
}
#define CLEMEM1 100
#define CLEMEM2 200
#define CLEMEM3 300
#define CLESEM1 30
#define CLESEM2 40
#define CLESEM3 50
#define N 10


int main()
{
    /**
        Création des IPC : MP et Sémaphores
    **/
    int *tab1 = creerMemoirePartagee(CLEMEM1, N*30);
    int *tab2 = tab1+10 ; //creerMemoirePartagee(CLEMEM2, N);
    int *resultat = tab1 + 20 ; //creerMemoirePartagee(CLEMEM3, N);

    //printf("%p - %p - %p\n", tab1, tab2, resultat);
    Init(CLESEM1,0); // celui du père
    Init(CLESEM2,0); // celui du fils1
    Init(CLESEM3,0); // celui du fils2
    int pid1, pid2 ;
    if((pid1=fork())==0)
    {

        // Fils 1
        while(1)
        {
            P(CLESEM2); // Attendre que le père remplisse tab1 et tab2
            //printf("Fils 1 : faire la somme des 2 premieres moities :\n");
            for(int i=0; i<N/2; i++)
            {
                resultat[i] = tab1[i] + tab2[i] ;
            }
            V(CLESEM1); // Relance le père
        }
    }
    else
    {
        if((pid2=fork())==0)
        {
            // Fils 2
            while(1)
            {
                P(CLESEM3); // Attendre que le père remplisse tab1 et tab2
                //printf("Fils 2 : faire la somme des 2 dernieres moities :\n");
                for(int i=N/2; i<N; i++)
                {
                    resultat[i] = tab1[i] + tab2[i] ;
                }
                //affichertab(resultat,N);
                V(CLESEM1); // Relance le père
            }
        }
        else
        {
            // Pere
            int mat[N][N];
            srand(getpid());
            for(int i=0; i<N; i++)
            {
                // Remplissage de tab1 et tab2
                // printf("Pere etape 1 : remplissage de tab1 et tab2\n");
                remplirtab(tab1,N);
                remplirtab(tab2,N);
                affichertab(tab1,N);
                affichertab(tab2,N);
                // Relance de Fils1 et Fils2
                V(CLESEM2);
                V(CLESEM3);
                // Attendre que Fils1 et Fils2 terminent leurs sommes
                P(CLESEM1);
                P(CLESEM1);
                // Remplir une ligne de la matrice
                //printf("Pere etape 2 : copie de resultat dans mat\n");
                for(int j=0; j<N; j++)
                {
                    mat[i][j] = resultat[j];
                }
                affichertab(resultat,N);

            }
            // Arrete les 2 processus fils
            kill(pid1,SIGINT);
            kill(pid2,SIGINT);
            // Destruction des IPCs
            while(waitpid(-1,0,0)>0) ;
            Detruire(CLESEM1);
            Detruire(CLESEM2);
            Detruire(CLESEM3);
            detruireMem(CLEMEM1,tab1);
            detruireMem(CLEMEM2,tab2);
            detruireMem(CLEMEM3,resultat);
            // Affichage de la matrice
            printf("Voici la matrice :\n");
            for(int i=0; i<N; i++)
            {
                for(int j=0; j<N; j++)
                {
                    printf("%4d",mat[i][j]);
                }
                printf("\n");
            }
        }
    }

    return 0;
}
