#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i;
    if(fork()==0)
    {
        int fact=1;
        for(i=1;i<6;i++)
            fact*=i;
        exit(fact);
    }
    else
    {
        // Père
        int res;
        wait(&res);
        res=WEXITSTATUS(res);
        for(i=6;i<11;i++)
            res*=i;
        printf("fact(10)=%d\n",res);
    }
    return 0;
}
