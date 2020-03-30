#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

//include de la biblio
#include "se_fichier.h"

//Fichier configuration
int *config(int *tailleMax, char *chemin)
{
	SE_FICHIER fic;
	int i;
	
	fic = SE_ouverture (chemin, O_RDONLY);

	SE_lectureEntier(fic, &i);
	int taille = (i*3)+1; // i correspond au nombre de numéro le fois 3 correspond aux numeros gagnants + le nombre de numéros gagnant + les gains 
	int *tab = malloc (sizeof (int) * taille);
	
	tab[0] = i;
	
	for (int x = 1; x<taille; x++)
	{
		SE_lectureEntier(fic, &i);
		tab[x] = i;
	}
	
	SE_fermeture (fic);
	*tailleMax = taille;
	
	return tab;
}


int clientEcriture(const char *chemin, int argc, char* argv[])
{	
	printf("Client écriture : \n");
	SE_FICHIER tube = SE_ouverture (chemin, O_WRONLY);

	if (tube.descripteur == -1)
		return 2;
	
	printf("les numéros joués sont : ");
	
	//On commence à 2 car on ne veux pas lire ./million client
	for(int x = 2; x < argc; x++)
	{
		printf("%d ", atoi(argv[x]));
		
		if(SE_ecritureEntier(tube, atoi(argv[x])) == -1)
		{
			printf("Une erreur d'écriture a eu lieu\n");
			return -1;
		}
		
		if(SE_ecritureCaractere (tube, ' ') == -1)
		{
			printf("Une erreur d'écriture a eu lieu\n");
			return -1;
		}
	}
	
	printf("\n");
	SE_fermeture (tube);
	
	return 0;
}

int clientLecture(const char *chemin)
{	
	printf("Client lecture : \n");
	int i;
	int bonNum;
	SE_FICHIER tube = SE_ouverture(chemin, O_RDONLY);
	
	if (tube.descripteur == -1)
		return -1;
	
	if(SE_lectureEntier(tube, &i) == -1)
		return -1;
	
	bonNum = i;
	
	if(i == 0)
		printf("Vous avez perdu aucun bon numéro\n\n");
		
	else
	{
		printf("Vous avez %d bon numéro\n", i);
		
		if(SE_lectureEntier(tube, &i) == -1)
			return -1;
		
		printf("Vos gains s'élève à : %d\n\n", i);
	}
	
	if(SE_lectureEntier(tube, &i) == -1)
			return -1;
	
	
	
	SE_fermeture (tube);
	SE_suppression (chemin);
	
	if(bonNum == i)
		return 1;
	
	return 0;
}

int client(const char *chemin, int argc, char* argv[])
{
	sleep(1);
	switch(clientEcriture(chemin, argc, argv))
	{
		case -1 :
			printf("Une erreur est survenu\n");
			return -1;
		
		case 0 :
			sleep(1);
			return clientLecture(chemin);
			
		case 2 :
			printf("There is no ongoing lottery\n");
			return 2;
			
		default :
			break;
	}
	
	return 0;
}

int serveurLecture(const char *chemin, int *tab, int *gain)
{
	printf("Serveur lecture: \n");
	mkfifo ("/tmp/tube", 0600);
	
	int i;
	int numWin = 0;
	int nbreNum = tab[0];
	
	SE_FICHIER tube = SE_ouverture(chemin, O_RDONLY);

	if (tube.descripteur == -1)
		return -1;
	
	printf("Le joueur a joué les numéros : ");
	
	for(int cmpt = 0; cmpt < nbreNum; cmpt++)
	{
		if(SE_lectureEntier(tube, &i) == -1)
			return -1;
		
		printf("%d ", i);
		
		for(int x = 1; x <= nbreNum; x++)
		{
			if(i == tab[x])
				numWin++;
		}
	}
	
	SE_fermeture (tube);
	SE_suppression (chemin);
	
	printf("\nLe joueur à trouvé %d bon numéro\n\n", numWin);
	*gain = numWin;
	
	return 0;
}

int serveurEcriture(const char *chemin, int *tab, int tailleMax ,int gain)
{
	printf("Serveur écriture: \n");
	mkfifo (chemin, 0600);
	
	SE_FICHIER tube = SE_ouverture(chemin, O_WRONLY);

	if (tube.descripteur == -1)
		return -1;
		
	if(gain == 0)
	{
		if(SE_ecritureEntier(tube, 0) == -1)
		{
			printf("Une erreur d'écriture a eu lieu\n");
			return -1;
		}
		
		if(SE_ecritureCaractere (tube, ' ') == -1)
		{
			printf("Une erreur d'écriture a eu lieu\n");
			return -1;
		}
	}
	
	else
	{
		for(int x = 0; x < 2; x++)
		{
			if(SE_ecritureEntier(tube, tab[tailleMax-(2*gain)+x]) == -1)
			{
				printf("Une erreur d'écriture a eu lieu\n");
				return -1;
			}
			
			if(SE_ecritureCaractere (tube, ' ') == -1)
			{
				printf("Une erreur d'écriture a eu lieu\n");
				return -1;
			}
		}
	}
	
	if(SE_ecritureEntier(tube, tab[0]) == -1)
	{
		printf("Une erreur d'écriture a eu lieu\n");
		return -1;
	}
	
	if(SE_ecritureCaractere (tube, ' ') == -1)
	{
		printf("Une erreur d'écriture a eu lieu\n");
		return -1;
	}
	
	SE_fermeture (tube);
	
	return gain;
}

int serveur(const char *chemin, int *tab, int tailleMax)
{
	int gain;
	
	while(1)
	{
		while(1)
		{	
			if(serveurLecture(chemin, tab, &gain) != -1)
				break;
		}
	
		if(serveurEcriture(chemin, tab, tailleMax, gain) == tab[0])
			break;
		sleep(1);
	}
	
	return 0;
}

int main(int argc, char* argv[])
{	
	if(!strcmp(argv[1], "client"))
	{
		printf("Exécution de client\n");
		return client("/tmp/tube", argc, argv);
	}
	
	else if(!strcmp(argv[1], "server"))
	{
		int *tab;
		int tailleMax;
    
		tab = config(&tailleMax, argv[2]);
		
		printf("Exécution de serveur\n");
		serveur("/tmp/tube", tab, tailleMax);
		free(tab);
	}
	
	else
		printf("L'argument : %s n'est pas reconnue\n", argv[1]);
	
    exit(0);
}
