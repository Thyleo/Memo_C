#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
int aleaPair()
{
    return (rand()%101-50)*2;
}
int main(int argc, char *argv[])
{
    int p1[2], p2[2], p3[2];
    pipe(p1);pipe(p2);pipe(p3);
    int val;
    if(fork()==0)
    {
        srand(getpid());
        close(p1[0]);close(p2[0]);close(p2[1]);close(p3[0]);close(p3[1]);
        for(int i=0; i<25; i++)
        {
            val=aleaPair();
            //printf("%d\n",val);
            sleep(rand()%3+1);
            write(p1[1],&val,sizeof(int));
        }
        close(p1[1]);
    }
    else
    {
        if(fork()==0)
        {
            srand(getpid());
            close(p1[0]);close(p2[0]);close(p2[1]);close(p3[0]);close(p3[1]);
            for(int i=0; i<25; i++)
            {
                val=aleaPair()+1;
                //printf("%d\n",val);
                sleep(rand()%3+1);
                write(p1[1],&val,sizeof(int));
            }
            close(p1[1]);
        }
        else
        {
            if(fork()==0)
            {
                float moy=0;
                int cpt=0;
                close(p1[0]);close(p1[1]);close(p2[1]);close(p3[0]);close(p3[1]);
                while(read(p2[0], &val, sizeof(int))!=0){
                    cpt++;
                    moy+=val;
                    printf("%3.2f\n", moy/cpt);
                }
                close(p2[0]);
            }
            else
            {
                if(fork()==0)
                {
                    float moy=0;
                    int cpt=0;
                    close(p1[0]);close(p1[1]);close(p2[1]);close(p2[0]);close(p3[1]);
                    while(read(p3[0], &val, sizeof(int))!=0){
                        cpt++;
                        moy+=val;
                        printf("%3.2f\n", moy/cpt);
                    }
                    close(p3[0]);
                }
                else
                {
                    close(p1[1]); close(p2[0]);close(p3[0]);
                    while(read(p1[0], &val, sizeof(int))!=0){
                        if(val>0){
                            write(p2[1], &val, sizeof(int));
                        }
                        else{
                            write(p3[1], &val, sizeof(int));
                        }
                    }
                    close(p1[0]); close(p2[1]);close(p3[1]);
                }
            }
        }
    }
    return 0;
}
