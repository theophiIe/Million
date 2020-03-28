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
	int val;
	
	fic = SE_ouverture ("lottery.cfg", O_RDONLY);

	if (fic.descripteur == -1)
		return -1;

	SE_lectureEntier(fic, &i);
	int taille = (i*3)+1; // i correspond au nombre de numéro le fois 3 correspond aux numeros gagnants + le nombre de numéros gagnant + les gains 
	//int tab[taille];
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
int serveur(const char *chemin, int argc)
{
	SE_FICHIER tube;
	int i;
	
	int tab[argc];
	
	tube = SE_ouverture (chemin, O_RDONLY);

	if (tube.descripteur == -1)
		return -1;
	
	for(int cmpt = 0; SE_lectureEntier(tube, &i) > 0; cmpt++)
	{
		tab[cmpt] = i;
	}
	
	SE_fermeture (tube);

	return 0;
}

int main(int argc, char* argv[]){
	//Comparer le nombre d'argument avec le nombre de numéros de la lottery 
	
	if(argv[1] == "client")
	{
		//Utilisisation de la fonction client
	}
	
	else if(argv[1] == "server")
	{
		//Utilisisation de la fonction serveur
	}
	
	else
		printf("L'argument : %s n'est pas reconnue\n", argv[1]);
	
    for(int i=2; i<argc; i++)
    {
		printf("argument : %s\n", argv[i]);
	}
	
	//Parcours du tab
    int *tab;
    tab = config();

    int x =0;
    while(tab[x] != NULL)
    {
		printf("%d\n", tab[x]);
		x++;
    }
    free(tab);
    
    exit(0);
}
