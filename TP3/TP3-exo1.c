#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define LECTURE  0
#define ECRITURE 1
int main()
{
    int p[2];
    char car ='a' ;
    pipe(p);

    if(fork()==0){
        // Fermeture des descripteurs inutiles
        close(p[LECTURE]);
        while((car=getchar())!='$'){
            if(isalpha(car)){
                write(p[ECRITURE],&car, sizeof(char));
            }
        }
        close(p[ECRITURE]);
    }
    else{
        close(p[ECRITURE]);
        while(read(p[LECTURE], &car, sizeof(char))!=0){
            putchar(toupper(car));
            //putchar('\n');
            fflush(stdout);
        }
        close(p[LECTURE]);
    }
    return 0;
}
