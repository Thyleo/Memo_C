#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#define CLE_MEM 60
#define MUTEX 55
#define N 1000000
int *ptr;
int num_mem;

int main ()
{
   num_mem = shmget (CLE_MEM, sizeof (int), IPC_CREAT | 0666);
  ptr = (int *) shmat (num_mem, 0, 0);
  *ptr = 0;
  int pid;
  
  pid = fork ();
  if (pid == 0)
    {
        num_mem = shmget (CLE_MEM, sizeof (int), 0666);
        ptr = (int *) shmat (num_mem, 0, 0);
        for(int i=0;i<N ; i++)
    	{
    	    
    	    *ptr = *ptr + 1 ;
    	  
    	}
    }
  else
    {
        for(int i=0;i<N; i++)
    	{
    	   
    	    *ptr = *ptr - 1 ;
    	   
    	}
    	wait(NULL);
    	printf("resultat = %d\n", *ptr);
    }
  shmdt (ptr);
  shmctl (num_mem, IPC_RMID, NULL);
}