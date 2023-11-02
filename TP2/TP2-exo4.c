#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
void handlerPing(int sig){
    printf("Ping\n");
}
void handlerPong(int sig){
    printf("Pong\n");
}
int main()
{
    int pid1, pid2 ;
    signal(SIGUSR1, handlerPing);
    signal(SIGUSR2, handlerPong);
    pid1=fork();
    if(pid1==0){
        // Fils 1
        while(1){
            pause();
        }
    }
    else{
        pid2 = fork();
        if(pid2==0){
            // Fils2
            while(1){
                pause();
            }
        }
        else{
            // Père
            while(1){
                sleep(1);
                kill(pid1, SIGUSR1);
                sleep(1);
                kill(pid2, SIGUSR2);
            }
        }
    }
}
