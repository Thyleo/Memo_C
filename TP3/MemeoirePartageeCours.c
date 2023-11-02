#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#define CLE_MEM 60
int *ptr ;
int num_mem, n;
int main()
{
    n=1;

    if (fork()==0)
    {
        num_mem = shmget(CLE_MEM, sizeof(int), IPC_CREAT | 0666);
        ptr = (int*) shmat(num_mem, 0, 0);
        *ptr=n+(2*n)+(3*n);
    }
    else
    {
        num_mem = shmget(CLE_MEM, sizeof(int), IPC_CREAT | 0666);
        ptr = (int*) shmat(num_mem, 0, 0);
        n++;
        wait(NULL);
        printf("Le résultat est %d\n",n+*ptr);
    }
    shmdt(ptr);
    shmctl(num_mem, IPC_RMID, NULL);
}
