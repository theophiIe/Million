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
int *config()
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
	
	return tab;
}

//Lecture du tube
//Faire une decomposition des fonctions avec une fonction client fonction principale
//Une fonction ecritureClient
//Une fonction lectureClient ou resultat client
int client(const char *chemin, int argc, char* argv[])
{
	SE_FICHIER tube;
	
	tube = SE_ouverture (chemin, O_WRONLY);

	if (tube.descripteur == -1)
		return -1;
	
	//On commence à 2 car on ne veux pas lire ./million client
	for(int i = 2; i<argc; i++)
	{
		if(SE_ecritureEntier(tube, atoi(argv[i])) == -1)
		{
			printf("Une erreur d'écriture a eu lieu\n");
			return -1;
		}
	}
	SE_fermeture (tube);
	
	return 0;
}

//Lecture du tube
//Faire un fonction principa serveur
//une fonction lectureServeur
//une fonction ecriture serveur pour transmettre le resultat du jeu aux clients
int serveur(const char *chemin, int *tab)
{
	SE_FICHIER tube;
	int i;
	int numWin;
	int nbreNum = tab[0];
	
	tube = SE_ouverture(chemin, O_RDONLY);

	if (tube.descripteur == -1)
		return -1;
	
	for(int cmpt = 0; cmpt < nbreNum; cmpt++)
	{
		SE_lectureEntier(tube, &i);
		
		for(int x = 1; x <= nbreNum; x++)
		{
			if(i == tab[x])
				numWin++;
			printf("TEST\n");
		}
	}
	
	SE_fermeture (tube);
	unlink (chemin);
	
	return 0;
}

int main(int argc, char* argv[]){
	//Comparer le nombre d'argument avec le nombre de numéros de la lottery 
	int *tab;
    tab = config();

    //~ int x =0;
    //~ while(tab[x] != NULL)
    //~ {
		//~ printf("%d\n", tab[x]);
		//~ x++;
    //~ }
    
    printf("%s\n", argv[1]);
    
	if(!strcmp(argv[1], "client"))
	{
		//Utilisisation de la fonction client
		mkfifo ("/tmp/tube", 0600);
		client("/tmp/tube", argc, argv);
	}
	
	else if(!strcmp(argv[1], "server"))
	{
		//Utilisisation de la fonction serveur
		serveur("/tmp/tube", tab);
		free(tab);
	}
	
	else
		printf("L'argument : %s n'est pas reconnue\n", argv[1]);
	
    for(int i=2; i<argc; i++)
    {
		printf("argument : %s\n", argv[i]);
	}
	
	//Parcours du tab

    
    exit(0);
}
