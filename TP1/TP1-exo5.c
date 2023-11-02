#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
/** Il faut lancer le programme avec un argument comme : ls ou ps **/
void handlerSIGUSR1(int sig)
{

}

int main(int argc, char *argv[]){
	int pid[5];
	signal(SIGUSR1, handlerSIGUSR1);
	for(int i=0;i<5;i++){
		char commande[100]="/bin/";
		strcat(commande,argv[1]);
		if((pid[i]=fork())==0){
			pause();
			// Exécuter la commande passée en argument
			execv(commande,argv+1);			
			//exit(0);
		}
	}
	// pere 
	getchar();
	// Ecriture des pid des fils
	for(int i=0;i<5;i++)
		printf("%d\n", pid[i]);
	// Envoi de SIGUSR1 à tous les fils
	for(int i=0;i<5;i++)
		kill(pid[i], SIGUSR1);
	// Attente de la fin de tous les fils
	int i=0;
	while((pid[i++]=waitpid(-1,NULL,0))>0);
	// Ecriture des pid des fils après leur terminaison
	for(int i=0;i<5;i++)
		printf("%6d", pid[i]);
	printf("\n");
}
