#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define N 10
#define cleSemProd 50
#define cleSemCons 60
#define cleMutex 70
#define cleMem1 80
#define cleMem2 90
#define MUTEX 100
#define MUTEX2 200
char *T ;
int *ip ; // indice utilisé par les producteurs
int *ic ; // indice utilisé par les consommateurs
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
char *creerMemoirePartageeChar(key_t cle, int taille)
{
	int num = shmget ( cle, taille * sizeof(char), IPC_CREAT|0666);
	char *ptr = (char*) shmat ( num, NULL, 0);
	return ptr;
}

void detruireMemChar(key_t cle, char *ptr)
{
	shmdt (ptr) ;
	shmctl ( cle, IPC_RMID, NULL);
}

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

int lire(char *ch, int n)
{
    int taille = rand()%n+1;
    for(int i=0;i<taille;i++)
        ch[i]='A' + rand()%26;
    ch[taille]='\0';
    return taille;
}
void deposer(char *ch, int m, int ip)
{
    for(int i=0;i<m;i++)
    {
        T[(ip+i)%N]=ch[i];
    }
}
char retirer(int ic)
{
    return T[ic];
}
void traiter(char c)
{
    printf("%c\n",c);
}

void producteur(int  num)
{
    int  m;
    char ch[N];
       
    while(1)
    {
        m=lire(ch, N);
        printf("producteur %d --> ch=%s\n",num,ch);
        // Appel de P (cleSemProd)
        // Prendre m jetons du sémaphore cleSemProd
        for(int i=0;i<m;i++)
            P(cleSemProd);
        deposer(ch, m, *ip);
        P(MUTEX);
        *ip=(*ip+m)%N; // Section critique 
        V(MUTEX);
        // Remettre m jetons dans le sémaphore cleSemCons
        for(int i=0;i<m;i++)
            V(cleSemCons);
         sleep(1);
    }
}
void consommateur(int num)
{
    // Consommateur
    char c ;
    while(1)
    {
        // Appel de P (cleSemCons)
        P(cleSemCons);
        c=retirer (*ic) ;
        P(MUTEX2);
        *ic = (*ic + 1 )%N ; // Section critique
        V(MUTEX2);
        // Appel de V (cleSemProd)
        V(cleSemProd);
        printf("%d --> ", num);
        traiter(c) ;
    }
}
int main()
{
    // Initialisation des sémaphores
    Init(cleSemCons, 0);
    Init(cleSemProd, N);
    Init(cleMutex,1);
    Init(MUTEX, 1);
    Init(MUTEX2, 1);
    T = creerMemoirePartageeChar(cleMem1, N);
    ip = creerMemoirePartagee(cleMem2, 2);
    *ip=0;
    ic =ip+1; // pointe la 2ème case du tableau
    *ic=0;
    int pid;


    if((pid=fork())==0)
    {
        // Producteur
        srand(getpid());
        T = creerMemoirePartageeChar(cleMem1, N);
        ip = creerMemoirePartagee(cleMem2, 2);
        producteur(1);
    }
    else
    {
        if(fork()==0)
        {
            // Producteur 2
            srand(getpid());
             T = creerMemoirePartageeChar(cleMem1, N);
             ip = creerMemoirePartagee(cleMem2, 2);
            producteur(2);
        }
        else
        {
             if(fork()==0)
            {
                // Producteur 3
                srand(getpid());
                 T = creerMemoirePartageeChar(cleMem1, N);
                 ip = creerMemoirePartagee(cleMem2, 2);
                producteur(3);
            }
            else
            {
                if(fork()==0)
                {
                    // Consommateur 1
                    ip = creerMemoirePartagee(cleMem2, 2);
                    ic = ip+1;
                    consommateur(1);
                }
                else
                {
                    // Consommateur 2
                    ip = creerMemoirePartagee(cleMem2, 2);
                    ic = ip+1;
                    consommateur(2);
                }
            }
        }
    }

    return 0;
}
