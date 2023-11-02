#include "ipcsem.h"

/*
    Correction EI Programmation Système
    Q1 - 1  : 1 point
    Q1 - 2  : 1 point
    Q2 - 1  : 1 point
    Q2 - 2  : 1 point
    Q3 - 1  : 0.5 point
	Q3 - 2  : 1 point
	Q3 - 3  : 0.5 point
    Q3 - 4  : 1 point
    Q4 - 1  : 1 point
    Q5      : 5 points ; il s'agit des 2 processus fils
    Q6      : 4 points
    Q7      : 3 points : dans le cas où cela compile sans erreurs et donne des résultats
*/

/* ###################### Variables globales et headers ###################### */

key_t CLE_MEM1=60;
key_t CLE_MEM2=70;
key_t CLE_SEM1=40;
key_t CLE_SEM2=41;
key_t CLE_SEM3=42;
int N=10;
int* tab;
int* result;

int* creerTabPartage(int nb, key_t cle);
void initTab(int* tab);
int somme(int* tab);
int produit(int* tab);

void afficherTab(int* tab, int taille);

/* ###################### Fonctions pour le signal ###################### */
// Q4 - 1 : 1 point
void termine_proc(int signal){
	detruireMem(CLE_MEM1,tab);
	detruireMem(CLE_MEM2, result);
	printf("Fin processus\n");
	exit(0);
}

/* ###################### Fonction main ###################### */
// Q7 : 3 points : dans le cas où le résultat est juste
int main(int argc, char* argv[]){
	srand(time(NULL));
    // Q3 - 1 : 0.5 point
	tab = creerTabPartage(N, CLE_MEM1);
	// Q3 - 2 : 1 point
	result = creerTabPartage(2, CLE_MEM2);

	signal(SIGINT, termine_proc);

    // Q3 - 4 : 1 point
	Init(CLE_SEM1, 0);
	Init(CLE_SEM2, 0);
	Init(CLE_SEM3, 0);

	pid_t pid1=fork();

	if(pid1==0){
		/* Fils 1 */
            // Q5 : 5 points ; il s'agit des 2 processus fils
			while(1){
				P(CLE_SEM1);
				*result=somme(tab);
				printf("Fils 1 : resultat somme : %d\n", *result);
				V(CLE_SEM3);
			}

	}else{
		pid_t pid2=fork();
		if(pid2==0){
			/* Fils 2 */

			while(1){
				P(CLE_SEM2);
				*(result+1)=produit(tab);
				printf("Fils 2 : resultat produit : %d\n", *(result+1));
				V(CLE_SEM3);
			}

		}else{
			/* Père */

			int i;
			int resultat;
			int tab_resultats[10];
			// Q6  : 4 points
			for(i=0; i<10; i++){
				printf("--------------\nLe pere prend la main : TAB = ");
				// Q3 - 3 : 0.5 point
				initTab(tab);
				afficherTab(tab, N);

				V(CLE_SEM1);
				V(CLE_SEM2);

				P(CLE_SEM3);
				P(CLE_SEM3);

				resultat=(14+N*(*result)+*(result+1)/N);
				printf("Pere : Ajout du resultat %d au tableau !\n", resultat);

				tab_resultats[i]=resultat;
			}
			
			kill(pid1, SIGINT);
			kill(pid2, SIGINT);

			printf("Tableau des resulats : ");
			afficherTab(tab_resultats, 10);
		}
	}

	return 0;
}



/* ######################Fonctions###################### */
// Q1 - 1 : 1 point
int* creerTabPartage(int nb, key_t cle){

	int num_mem = shmget(cle, sizeof(int), IPC_CREAT|0666);
	if(num_mem==-1){
		printf("Erreur de creation du segment !\n");
		return NULL;
	}
	int* tab=(int*)shmat(num_mem, NULL, 0);
	return tab;

}
// Q1 - 2 : 1 point
void initTab(int* tab){
	int i;
	for (i = 0; i < N; ++i){
		*(tab+i)=1+rand()%10;
	}
}
// Q2 - 1 : 1 point
int somme(int* tab){
	int i;
	int somme=0;
	for (i = 0; i < N; ++i){
		somme+=*(tab+i);
	}
	return somme;
}
// Q2 - 2 : 1 point
int produit(int* tab){
	int i;
	int produit=1;
	for (i = 0; i < N; ++i){
		produit*=*(tab+i);
	}
	return produit;
}

void afficherTab(int* tab, int taille){
	int i;
	printf("{");
	for (i = 0; i < taille; ++i){
		printf(" %d", *(tab+i));
	}
	printf(" }\n");
}

/* ###################### ###################### */