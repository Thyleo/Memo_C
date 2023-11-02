#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define CLE_MEM 65
#define N 20
int *ptr;
int num_mem, n;
void
handler (int sig)
{

}
void remplirtab(int *tab, int taille){
    srand(getpid());
    for(int i=0 ; i< taille ; i++){
        tab[i] = rand() % 100 ; // Valeurs alétoires entre 0 et 100
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