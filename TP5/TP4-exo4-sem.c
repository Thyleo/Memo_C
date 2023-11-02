#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


#define CLE_MEM 200
#define CLE_MEM2 210
#define CLE_SEM1 300
#define CLE_SEM2 350
#define N 10
void Init (key_t cle, int V) {
  int Num = semget (cle, 0, 0);
  if (Num==-1) Num = semget (cle, 1, 0777|IPC_CREAT);
  semctl (Num, 0, SETVAL, V);

}
void Detruire (key_t cle) {
  int Num = semget (cle, 0, 0);
  semctl (Num, 0, IPC_RMID, 0);
}
void P (key_t cle) {
  int Num = semget (cle, 0, 0);
  struct sembuf TabOp;
  TabOp.sem_num = 0;
  TabOp.sem_op  = -1;
  TabOp.sem_flg = SEM_UNDO;     // bloquant
  int R = semop (Num, &TabOp, 1);
  if (R==-1) perror ("P");
}
void V (key_t cle) {
  int Num = semget (cle, 0, 0);
  struct sembuf TabOp;
  TabOp.sem_num = 0;
  TabOp.sem_op  = 1;
  TabOp.sem_flg = SEM_UNDO;
  int R = semop (Num, &TabOp, 1);
  if (R==-1) perror ("V");
}
void handler(int sig){
    
}
/**
 *  Remplir le tableau avec des valeurs aléatoires
 */
void remplirTab(int *t, int s){
    srand(getpid());
    for(int i=0; i<s; i++){
        t[i] = rand()%100;
    }
}
/**
 *  Affiche le tableau 
 */
void afficheTab(int *t, int s){
    for(int i=0; i<s; i++){
        printf("%4d",t[i]);
    }
    printf("\n");
}
/**
 *  Donne l'indice du plus petit élément du tableau
 */
int indPetit(int *t, int s){
    int p = 0;
    for(int i=0;i<s;i++){
        if(t[i]<t[p]){
            p=i;
        }
    }
    return p;
}
/**
 *  Donne l'indice du plus grand élément du tableau
 */
int indGrand(int *t, int s){
    int g = 0;
    for(int i=1;i<s;i++){
        if(t[i]>t[g]){
            g=i;
        }
    }
    return g;
}

void main ()
{
    signal(SIGUSR1, handler);
    Init(CLE_SEM1, 0);
    Init(CLE_SEM2, 0);
    
    int *tab, *attente;
    int num_mem, num_mem2, n;
    // Création d'un tableau partagé de N+1 éléments
    num_mem = shmget (CLE_MEM, sizeof (int) * (N+1), IPC_CREAT | 0666);
    tab = (int *) shmat (num_mem, 0, 0);
    num_mem2 = shmget (CLE_MEM2, sizeof (int) , IPC_CREAT | 0666);
    attente = (int *) shmat (num_mem2, 0, 0);
    
    *attente=0;

    remplirTab(tab, N);
    
    int pid = fork ();
    if (pid == 0)
    {
        // Fils
        num_mem = shmget (CLE_MEM, sizeof (int)* (N+1), IPC_CREAT | 0666);
        tab = (int *) shmat (num_mem, 0, 0);
        num_mem2 = shmget (CLE_MEM2, sizeof (int) , IPC_CREAT | 0666);
        attente = (int *) shmat (num_mem2, 0, 0);
        
        // repeter
        while(*attente!=2 ){
            printf("Fils->");
            // 1- chercher l'indice du plus petit élément
            int ipetit = indPetit(tab, N/2);
            // 2- mettre cet indice dans la dernière case du tableau
            tab[N] = ipetit ;
            // 3- attendre jusqu'à relance par le père
            //*attente = 0 ; // Pour relancer le père
            V(CLE_SEM2);
            //while(*attente!=1 && *attente!=2) ; // Attente
            P(CLE_SEM1);
        }
        // Jusqu'à 
    }
  else
    {
        // Le père
       // repeter
        while(*attente !=2){
            printf("Pere->");
            afficheTab(tab, N);
            // 1- chercher l'indice du plus grand élément
            int igrand = indGrand(tab + N/2, N/2) + (N/2);
            // 2- Attendre la fin de la recherche du Fils
            //while(*attente!=0) ; 
            P(CLE_SEM2);
            // 3- Permutaion si c'est possible
            if(tab[igrand] > tab[ tab[N] ]){
                int temp = tab[igrand];
                tab[igrand] =  tab[ tab[N] ] ;
                tab[ tab[N] ] = temp;
                // 4- Relance du fils 
                //*attente = 1;
                V(CLE_SEM1);
            }
            else
            {
                V(CLE_SEM1);
                *attente = 2 ;
            }
            
        }
        // Jusqu'à 
	//kill(pid, SIGINT);
	    
        wait (NULL);
        Detruire(CLE_SEM1);
        Detruire(CLE_SEM2);
    }
}


