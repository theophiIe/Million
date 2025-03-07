#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
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
	int num;
	
	SE_FICHIER file = SE_ouverture (chemin, O_RDONLY);

	SE_lectureEntier(file, &num);
	
	//num correspond au nombre de numéro pour la lotterie, 
	//le fois 3 correspond : aux numeros gagnants + le nombre de numéros gagnant + les gains du fichier.cfg,
	//le +1 est le premier chiffre du fichier.cfg
	int taille = (num*3)+1; 
	int *tab = malloc (sizeof (int) * taille);
	
	tab[0] = num;
	
	for (int x = 1; x<taille; x++)
	{
		SE_lectureEntier(file, &num);
		tab[x] = num;
	}
	
	SE_fermeture (file);
	*tailleMax = taille;
	
	return tab;
}

// Vérifie si les numéros fournie par le client sont des nombres positifs différent de zéro
// \param[in]	argc		nombre d'argument 
// \param[in]	argv		tableau avec les arguments
// \return					-1 en cas d'erreur,
//							 0 si aucun problème
int testArgClient(int argc, char* argv[])
{
	for(int x = 2; x < argc; x++)
	{
		if(atoi(argv[x]) <= 0)
		{
			printf("Erreur d'argument : Le client n'a pas un ticket valide \n\n");
			return -1;
		}		
	}
	
	return 0;
}

// Permet d'écrire dans le tube
// \param[in]	tube		tube dans lequel écrire 
// \param[in]	i			nombre à ecrire dans le tube
// \return					-1 en cas d'erreur,
//							0 si aucun problème
int wrTube(SE_FICHIER tube, int i)
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
		
		if(wrTube(tube, atoi(argv[x])) == -1)
			return -1;
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
	{
		printf("Erreur lors de l'ouverture du tube\n");
		return -1;
	}
	
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
	
	if(testArgClient(argc, argv) == -1)
		return -1;
	
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
// \param[out]	nbreNumWin	nombre de numéro gagnant
// \return					-1 en cas d'erreur,
//							 0 tout c'est bien passé
int serveurLecture(const char *chemin, int *tab, int *nbreNumWin)
{
	mkfifo (chemin, 0600);
	
	int i;
	int numWin = 0;
	int nbreNum = tab[0];
	
	SE_FICHIER tube = SE_ouverture(chemin, O_RDONLY);

	if (tube.descripteur == -1)
	{
		printf("Erreur lors de l'ouverture du tube\n");
		return -1;
	}
	
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
	
	*nbreNumWin = numWin;
	
	return 0;
}

// Le seveur créer le tube et lit son contenue 
// \param[in]	chemin		chemin d'acces du tube
// \param[in]	tab			tableau avec les informations du fichier .cfg
// \param[out]	nbreNumWin	nombre de numéro gagnant
// \return					-1 en cas d'erreur,
//							 0 tout c'est bien passé
int serveurEcriture(const char *chemin, int *tab, int tailleMax ,int nbreNumWin)
{
	printf("Serveur écriture : Transmistions des données vers le client\n");
	mkfifo (chemin, 0600);
	
	SE_FICHIER tube = SE_ouverture(chemin, O_WRONLY);

	if (tube.descripteur == -1)
	{
		printf("Erreur lors de l'ouverture du tube\n");
		return -1;
	}
		
	if(nbreNumWin == 0)
	{
		if(wrTube(tube, 0) == -1)
			return -1;
	}
	
	else
	{
		for(int x = 0; x < 2; x++)
		{
			if(wrTube(tube, tab[tailleMax-(2*nbreNumWin)+x]) == -1)
				return -1;
		}
	}
	
	if(wrTube(tube, tab[0]) == -1)
		return -1;
	
	SE_fermeture(tube);
	
	return nbreNumWin;
}

// Gestion des fonctions serveurs 
// \param[in]	chemin		chemin d'acces du tube
// \param[in]	tab			tableau avec les informations du fichier .cfg
// \param[in]	tailleMax	nombre de numéro gagnant
void serveur(const char *chemin, int *tab, int tailleMax)
{
	int nbreNumWin;
	
	while(1)
	{
		while(1)
		{	
			if(serveurLecture(chemin, tab, &nbreNumWin) != -1)
				break;
		}
	
		if(serveurEcriture(chemin, tab, tailleMax, nbreNumWin) == tab[0])
			break;
			
		sleep(1);
	}
	
	SE_suppression (chemin);
}

int main(int argc, char* argv[])
{	
	const char *chemin = "/tmp/tube";
	
	if(!strcmp(argv[1], "client"))
	{
		printf("Exécution du client n°%d\n", (int)getpid());
		return client(chemin, argc, argv);
	}
	
	else if(!strcmp(argv[1], "server"))
	{
		int *tab;
		int tailleMax;
    
		tab = config(&tailleMax, argv[2]);
		
		printf("Exécution du serveur n°%d\n\n", (int)getpid());
		serveur(chemin, tab, tailleMax);
		printf("\nFin d'éxécution du serveur n°%d\n\n", (int)getpid());
		free(tab);
	}
	
	else
		printf("L'argument : %s n'est pas reconnue\n", argv[1]);
	
    exit(0);
}
