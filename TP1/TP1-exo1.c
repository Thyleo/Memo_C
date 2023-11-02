#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{
    int i, pid, status;
    if((pid=fork())==0)
    {
        // Fils
        // sleep(20);
        printf("Je suis le fils : mon pid est %d, celui du pere %d : \n", getpid(), getppid());
    }
    else
    {
        // Pere

        //sleep(20);
        printf("Je suis le pere : mon pid est %d, celui du pere %d : \n", getpid(), pid);
    }
    return 0;
}
