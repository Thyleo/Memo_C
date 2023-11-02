#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

/* problème de syracuse avec pipe */

int main(int argc , char *argv[]){
    if(argc<2)
        exit(0);

    int val = atoi(argv[1]);
    int p1[2], p2[2] ;
    pipe(p1); pipe(p2);
    // Mettre la valeur dans l'un des 2 pipes
    if(val%2==0)
        write(p2[1], &val, sizeof(int));
    else
        write(p1[1], &val, sizeof(int));
    int pid = fork();
    if(pid==0){
        // Fils
        close(p1[1]); close(p2[0]); // fermeture descripteurs inutile
        while(read(p1[0],&val, sizeof(int))!=0) {
            printf("Fils -> %d\n", val);
            if(val==1)
                break;
            val = val*3 + 1 ;
            write(p2[1], &val, sizeof(int));
        }
        close(p1[0]); close (p2[1]);
    }
    else{
        close(p1[0]); close(p2[1]); // fermeture descripteur d'écriture
        while(read(p2[0], &val, sizeof(int))!=0){
            while(val%2==0){
                printf("Pere -> %d\n", val);
                val/=2;
            }
            write(p1[1], &val, sizeof(int));
        }
        close(p1[1]); close(p2[0]);
    }

}
