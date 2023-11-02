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

void main(){
    int val=0;
    Init(CLE_SEM1, 0 );
    Init(CLE_SEM2, 0 );
    if(fork()==0){
        // Fils
        printf("Fils->");
        while(val<100){
            val+=2;
            printf("%4d",val);
            if(val%5==0){
                printf("\nFils->");
               //sleep(1);
               V(CLE_SEM1); // Débloque le père
               P(CLE_SEM2); // Se bloque
            }
        }
        printf("\n");
    }
    else{
        // père
        //sleep(1);
        P(CLE_SEM1); // Bloquer le père pour forcer le fils à commencer
        printf("Pere->");
        while(val<100){
            val+=3;
            printf("%4d",val);
            if(val%5==0){
                printf("\nPere->");
                //sleep(1);
                V(CLE_SEM2); // Débloque le fils
                P(CLE_SEM1); // Se bolque
            }
        }
        printf("\n");
        V(CLE_SEM2);V(CLE_SEM2);V(CLE_SEM2);V(CLE_SEM2);V(CLE_SEM2);V(CLE_SEM2); // Débloque le fils
    }
}

