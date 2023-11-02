#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#define CLE_MEM 60
int *ptr;
int num_mem, n;
void handler(int sig){
    //printf("Passage oblige !!\n");
}
int main()
{
    signal(SIGUSR1, handler);
	num_mem = shmget(CLE_MEM, sizeof(int), IPC_CREAT | 0666);
	ptr = (int*) shmat(num_mem, 0, 0);
	*ptr=259; 
	int pid = fork();
    if (pid==0)
    {
    	num_mem = shmget(CLE_MEM, sizeof(int),  0666);
    	ptr = (int*) shmat(num_mem, 0, 0);
       while(*ptr!=1){
            if(*ptr%2!=0){ // impair
                printf("%d\n",*ptr);
                *ptr= *ptr*3 + 1 ;
            }
            kill(getppid(), SIGUSR1);
            pause();
        }
    }
    else
    {
        while(*ptr!=1){
            pause();
            while(*ptr%2==0){ // Pair
                printf("%d\n",*ptr);
                *ptr/=2;
            }
            kill(pid, SIGUSR1);
        }
        printf("%d\n",*ptr);
    }
    shmdt(ptr);
    shmctl(num_mem, IPC_RMID, NULL);
}


