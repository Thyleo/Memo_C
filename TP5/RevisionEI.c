#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#define MEM1 10
#define MEM2 20
#define SEM_P 30
#define SEM_F1 31
#define SEM_F2 32
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
int *creerMemoirePartagee(key_t cle, int taille){
	int num = shmget ( cle, taille * sizeof(int), IPC_CREAT|0666);
	int *ptr = (int*) shmat ( num, NULL, 0);
	return ptr;
}
void detruireMem(key_t cle, int *ptr){
	shmdt (ptr) ;
	int num = shmget ( cle, 0, 0666);
	shmctl ( num, IPC_RMID, NULL);
}
/**
 *  Initialise un tableau avec N valeurs aléatoires entre 1 et 10
 * */
void initTab(int *tab){
    for(int i=0;i<N; i++){
        tab[i] = rand()%10 +1;
    }
}
/**
 *  retourne la somme des valeurs du tableau tab
 * */
int somme(int *tab){
    int s = 0;
    for(int i=0;i<N; i++){
        s+=tab[i];
    }
    return s;
}
/**
 *  retourne le produit des valeurs du tableau tab
 * */
int produit(int *tab){
    int p = 1;
    for(int i=0;i<N; i++){
        p*=tab[i];
    }
    return p;
}
/**
 *  affiche les valeurs du tableau tab
 * */
void affiche(int *tab){
    for(int i=0;i<N; i++){
        printf("%10d", tab[i]);
    }
    printf("\n");
}
int *T;
int *sp;
/**
 *  affiche les valeurs du tableau tab
 * */
void handlerSIGINT(int sig){
    printf("Fin processus\n");
    detruireMem(MEM1, T);
    detruireMem(MEM2, sp);
    Detruire(SEM_F1);
    Detruire(SEM_F2);
    Detruire(SEM_P);
    
    exit(0);
}
void main(){
    // Capturer le signal SIGINT
    signal(SIGINT, handlerSIGINT);
    // Création des mémoires partagées
    T = creerMemoirePartagee(MEM1, N); // Tableau partagé par les 3 processus
    sp = creerMemoirePartagee(MEM2, 2); // sp[0] donne la somme, sp[1] donne le produit
    // Création de 3 sémaphores privés
    Init(SEM_F1,0);
    Init(SEM_F2, 0);
    Init(SEM_P, 0);
    
    int pid1, pid2;
    pid1=fork();
    if(pid1==0){
        // Fils 1
        while(1){
            // Attend le Père
            P(SEM_F1);
            // Calcul de la somme 
            sp[0] = somme(T);
            // Réveiller le Père
            V(SEM_P);
        }
    }
    else{
        pid2=fork();
        if(pid2==0){
            // Fils 2
            while(1){
                // Attend le Père
                P(SEM_F2);
                // Calcul de la somme 
                sp[1] = produit(T);
                // Réveiller le Père
                V(SEM_P);
            }
        }
        else{
            // Père
            int tab[N];
            for(int i=0;i<N; i++){
                // Initialiser le tableau T
                initTab(T);
                // Relance Fils1 et Fils2
                V(SEM_F1);
                V(SEM_F2);
                // Attend le Fils1 et le Fils2
                P(SEM_P);
                P(SEM_P);
                tab[i] = 14 + N * sp[0] + sp[1]/N ;
            }
            // Tue les Fils1 et Fils2
            affiche(tab);
            kill(pid1, SIGINT);
            kill(pid2, SIGINT);
        }
    }
}