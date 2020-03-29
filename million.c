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
int *config(int *tailleMax)
{
	SE_FICHIER fic;
	int i;
	
	fic = SE_ouverture ("lottery.cfg", O_RDONLY);

	if (fic.descripteur == -1)
		return -1;

	SE_lectureEntier(fic, &i);
	int taille = (i*3)+1; // i correspond au nombre de numéro le fois 3 correspond aux numeros gagnants + le nombre de numéros gagnant + les gains 
	int *tab = malloc (sizeof (int) * taille);
	
	tab[0] = i;
	
	for (int x = 1; x<taille; x++)
	{
		SE_lectureEntier(fic, &i);
		tab[x] = i;
	}
	
	//~ for (int x=0; x<taille; x++)
		//~ printf("%d\n", tab[x]);
	
	SE_fermeture (fic);
	*tailleMax = taille;
	return tab;
}

//Lecture du tube
//Faire une decomposition des fonctions avec une fonction client fonction principale
//Une fonction ecritureClient
//Une fonction lectureClient ou resultat client
int clientEcriture(const char *chemin, int argc, char* argv[])
{	
	mkfifo ("/tmp/tube", 0600);
	printf("les numéros joués sont : ");
	
	SE_FICHIER tube = SE_ouverture (chemin, O_WRONLY);

	if (tube.descripteur == -1)
		return -1;
	
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
	int i;
	
	SE_FICHIER tube = SE_ouverture(chemin, O_RDONLY);
	
	if (tube.descripteur == -1)
		return -1;
	
	SE_lectureEntier(tube, &i);
	
	if(i == 0)
		printf("Vous avez perdu aucun bon numéro\n");
		
	else
	{
		printf("Vous avez %d bon numéro\n", i);
		SE_lectureEntier(tube, &i);
		printf("Vos gains s'élève à : %d\n", i);
	}
	
	SE_fermeture (tube);
	SE_suppression (chemin);
		
	return 0;
}

int client(const char *chemin, int argc, char* argv[])
{
	clientEcriture(chemin, argc, argv);
	//clientLecture(chemin);
	
	return 0;
}

//Lecture du tube
//Faire un fonction principa serveur
//une fonction lectureServeur
//une fonction ecriture serveur pour transmettre le resultat du jeu aux clients
//Fair en sorte que la fonction lit en boucle le tube jusqu'a avoir un gagant
int serveurLecture(const char *chemin, int *tab, int *gain)
{
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
	
	printf("le joueur à trouvé %d bon numéro : \n", numWin);
	*gain = numWin;
	
	return 0;
}

//Recup num win
int serveurEcriture(const char *chemin, int *tab, int tailleMax ,int gain)
{
	mkfifo (chemin, 0600);
	
	SE_FICHIER tube;
	
	tube = SE_ouverture(chemin, O_WRONLY);

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
		for(int x = 0; x<2; x++)
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
	
	SE_fermeture (tube);
		
	return 0;
}

int serveur(const char *chemin, int *tab, int tailleMax)
{
	int gain;
	
	serveurLecture(chemin, tab, &gain);
	//serveurEcriture(chemin, tab, tailleMax, gain);
	
	return 0;
}

int main(int argc, char* argv[]){
	int *tab;
	int tailleMax;
    
    tab = config(&tailleMax);
    
	if(!strcmp(argv[1], "client"))
	{
		printf("client\n");
		client("/tmp/tube", argc, argv);
	}
	
	else if(!strcmp(argv[1], "server"))
	{
		printf("serveur\n");
		serveur("/tmp/tube", tab, tailleMax);
	}
	
	else
		printf("L'argument : %s n'est pas reconnue\n", argv[1]);
	
	free(tab);
    
    exit(0);
}
