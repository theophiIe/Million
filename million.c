#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

//include de la bibliothéque du TD3
#include "se_fichier.h"

// Création d'un tableau d'entier à partir du fichier de configuration
// \param[out]	tailleMax	taille du tableau
// \param[in]	chemin		chemin d'accès au fichier de configuration	
// \return					un tableau d'entier
int *config(int *tailleMax, char *chemin)
{
	int i;
	
	SE_FICHIER fic = SE_ouverture (chemin, O_RDONLY);

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

int testArgClient(int argc, char* argv[])
{
	for(int x = 2; x < argc; x++)
	{
		if(atoi(argv[x]) <= 0)
		{
			printf("Erreur d'argument Le client n'a pas était retenue pour \n\n");
			return 3;
		}		
	}
	
	return 0;
}
// ecriture des numéros joué par le client dans le tube
// \param[in]	chemin		chemin d'acces du tube
// \param[in]	argc		nombre d'argument 
// \param[in]	argv		tableau d'argument qui contient les numéros à écrire
// \return					-1 en cas d'erreur,
//							0 si aucun problème,
//							2 si le tube n'existe pas
int clientEcriture(const char *chemin, int argc, char* argv[])
{	
	SE_FICHIER tube = SE_ouverture (chemin, O_WRONLY);

	if (tube.descripteur == -1)
		return 2;
	
	printf("Client écriture : Les numéros joués sont : ");
	
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

// Lecture du résultat envoyé depuis serveur par le tube 
// \param[in]	chemin		chemin d'acces du tube
// \return					-1 en cas d'erreur,
//							 0 le client n'a pas trouvé tout les numéros,
//							 1 le cient a trouvé tout les numéros
int clientLecture(const char *chemin)
{	
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
		printf("Client lecture : Vous avez %d bon numéro\n", i);
		
		if(SE_lectureEntier(tube, &i) == -1)
			return -1;
		
		printf("\t\t Vos gains s'élève à : %d\n\n", i);
	}
	
	if(SE_lectureEntier(tube, &i) == -1)
			return -1;
	
	SE_fermeture (tube);
	
	
	if(bonNum == i)
		return 1;
	
	return 0;
}

// Gestion des fonction de lecture et d'eciture du tube pour client
// \param[in]	chemin		chemin d'acces du tube
// \param[in]	argc		nombre d'argument 
// \param[in]	argv		tableau d'argument qui contient les numéros à écrire
// \return					-1 en cas d'erreur,
//							 0 le client n'a pas trouvé tout les numéros,
//							 1 le cient a trouvé tout les numéros
//							 2 si le tube n'existe pas
int client(const char *chemin, int argc, char* argv[])
{
	sleep(1);
	
	if(testArgClient(argc, argv) == 3)
		return 3;
	
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

// Le seveur créer le tube et lit son contenue 
// \param[in]	chemin		chemin d'acces du tube
// \param[in]	tab			tableau avec les informations du fichier .cfg
// \param[out]	gain		nombre de numéro gagnant
// \return					-1 en cas d'erreur,
//							 0 tout c'est bien passé
int serveurLecture(const char *chemin, int *tab, int *gain)
{
	mkfifo ("/tmp/tube", 0600);
	
	int i;
	int numWin = 0;
	int nbreNum = tab[0];
	
	SE_FICHIER tube = SE_ouverture(chemin, O_RDONLY);

	if (tube.descripteur == -1)
		return -1;
	
	printf("Serveur lecture : Le joueur a joué les numéros : ");
	
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
	
	printf("\n\t\t  Le joueur à trouvé %d bon numéro\n", numWin);
	*gain = numWin;
	
	return 0;
}

int wrTubeServ(SE_FICHIER tube, int i)
{
	if(SE_ecritureEntier(tube, i) == -1)
	{
		printf("Une erreur d'écriture a eu lieu\n");
		return -1;
	}
	
	if(SE_ecritureCaractere (tube, ' ') == -1)
	{
		printf("Une erreur d'écriture a eu lieu\n");
		return -1;
	}
	
	return 0;
}

// Le seveur créer le tube et lit son contenue 
// \param[in]	chemin		chemin d'acces du tube
// \param[in]	tab			tableau avec les informations du fichier .cfg
// \param[out]	gain		nombre de numéro gagnant
// \return					-1 en cas d'erreur,
//							 0 tout c'est bien passé
int serveurEcriture(const char *chemin, int *tab, int tailleMax ,int gain)
{
	printf("Serveur écriture : Transmistions des données vers le client\n");
	mkfifo (chemin, 0600);
	
	SE_FICHIER tube = SE_ouverture(chemin, O_WRONLY);

	if (tube.descripteur == -1)
		return -1;
		
	if(gain == 0)
	{
		if(wrTubeServ(tube, 0) == -1)
			return -1;
	}
	
	else
	{
		for(int x = 0; x < 2; x++)
		{
			if(wrTubeServ(tube, tab[tailleMax-(2*gain)+x]) == -1)
				return -1;
		}
	}
	
	if(wrTubeServ(tube, tab[0]) == -1)
		return -1;
	
	SE_fermeture(tube);
	
	return gain;
}

// Gestion des fonctions serveurs 
// \param[in]	chemin		chemin d'acces du tube
// \param[in]	tab			tableau avec les informations du fichier .cfg
// \param[in]	tailleMax	nombre de numéro gagnant
void serveur(const char *chemin, int *tab, int tailleMax)
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
	
	SE_suppression (chemin);
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
