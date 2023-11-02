#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define CLE_MEM 65
#define S1 30
#define S2 40

#define N 20
int *ptr;
int num_mem, n;
void
handler (int sig)
{

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

void remplirtab(int *tab, int taille){
    srand(getpid());
    for(int i=0 ; i< taille ; i++){
        tab[i] = rand() % 1000 ; // Valeurs alétoires entre 0 et 100
    }
}
void affichertab(int *tab, int taille){
    srand(getpid());
    for(int i=0 ; i< taille ; i++){
       printf("%4d", tab[i]);
    }
    printf("\n");
}
int indPetit(int *tab, int taille){
    int indP = 0 ;
    for(int i=1 ; i< taille ; i++){
        if(tab[i] < tab [indP]){
            indP = i ;
        }
    }
    return indP;
}
int indGrand(int *tab, int taille){
    int indG = 0 ;
    for(int i=1 ; i< taille ; i++){
        if(tab[i] > tab [indG]){
            indG = i ;
        }
    }
    return indG;
}
main ()
{
    Init(S1, 0);
    Init(S2, 0) ;
  signal (SIGUSR1, handler);
  num_mem = shmget (CLE_MEM, sizeof (int) * (N+1), IPC_CREAT | 0666);
  ptr = (int *) shmat (num_mem, 0, 0);
  remplirtab(ptr, N);
 // affichertab(ptr, N);
  ptr[N] = 100 ;
  int pid = fork ();
  if (pid == 0)
    {
      num_mem = shmget (CLE_MEM, sizeof (int) * (N+1), 0666);
      ptr = (int *) shmat (num_mem, 0, 0);

      // Répéter

      while(ptr[N] != -1 ){
          // 1- Recherche de l'indice du plus petit élément dans
          // la première moitié (N/2) du tableau
          // pause();
          int petit = indPetit(ptr, N/2);
          // Mettre cet indice dans la nième case (parce que le père doit utiliser la valeur)
          ptr[N] = petit ;
          // 2- Se mettre en pause pour donner la main au père
            V(S2);
            P(S1);
         // kill(getppid(), SIGUSR1);

      }
      // Jusqu'à ?????
    }
  else
    {
        // Répéter
        //sleep(1);
        while(1){
              // 1- Recherche de l'indice du plus grand élément dans
              // la deuxième moitié (N/2) du tableau
             int grand = indGrand( ptr + N/2, N/2) ; // donne l'indice à partir de la moitié du tableau
             grand = grand + N/2 ; // le vrai indice (à partir du début du tableau)
             // 2- Attente le signal du fils
              //kill(pid, SIGUSR1);
              V(S1);
              P(S2);
             //pause();
              // 3- Faire éventuellement une permutation
             if(ptr[grand] > ptr[ ptr[N] ]){
                 int temp = ptr[grand];
                 ptr[grand ] = ptr[ptr[N]];
                 ptr[ptr[N]] = temp;
                 affichertab(ptr, N);
             }
             else {
                 ptr[N] = -1 ;
                 printf("FIN\n");
                 break;
             }
          // 4- Donner la main au fils
          //

        }
      // Jusqu'à ?????
       // kill(pid, SIGINT);
        wait(NULL);
    affichertab(ptr, N);
    }

  shmdt (ptr);
  shmctl (num_mem, IPC_RMID, NULL);
  Detruire(S1);
  Detruire(S2);
}


