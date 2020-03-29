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
int config()
{
	SE_FICHIER fic;
	char c;
	char tab[99];
	int i = 0;
	
	fic = SE_ouverture ("lottery.cfg", O_RDONLY);

	if (fic.descripteur == -1)
		return -1;

	while (SE_lectureCaractere (fic, &c) > 0)
	{
		tab[i] = c;
		printf ("%c", c);
		i++;
	}
	
	for(int x = 0; x<i; x++)
	{
		printf("%c", tab[x]);
	}
	
	printf("%d",i);
	
	SE_fermeture (fic);
	
	return 0;
	
	/*
	//Tableau 2D
	int ** tableau2d = malloc(nblignes*sizeof(int));

	for(int i = 0, i < nblignes; ++i)

	{
		tableau2d[i] = malloc(nbcolonnes*sizeof(int));
	}

	//apres tableau2d[x][y] pour acces aux elements
	//la suppression ce fait de la meme facom

	for(int i = 0, i < nblignes; ++i)
	{
		free(tableau2d[i]);
	}

	free(tableau2d);
	*/
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
	
	for(int cmpt = 0; SE_lectureEntier (tube, &i) > 0; cmpt++)
	{
		tab[cmpt] = i;
	}
	
	SE_fermeture (tube);

	return 0;
	
	
	//~ SE_FICHIER file;
	//~ file = SE_ouverture (path, O_RDONLY);
	/*

		fd = open ("/tmp/tube", O_RDONLY);

		read (fd, buf, BUFFER_SIZE);

		printf ("%s\n", buf);

		close (fd);
 
	 //int fd;
    printf("%d args:\n",argc);
    for(int i = 2; i < argc; i++) printf("%d ",atoi(argv[i]));
    printf("\n");
    */
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
	
	/*
		fd = open ("/tmp/tube", O_WRONLY);

		strcpy (buf, "Here is Brian!");
		write (fd, buf, BUFFER_SIZE);
		close (fd);
		 
	*/
}

int main(int argc, char* argv[]){
    
    //doit y avoir plus de trois arguments minimum 
    /*if (argc < 3)
    {
        printf("Syntaxe :\n");
    }
    
    else 
    {
        if(strcmp(argv[1], "serveur") == 0) 
            serveur();
        
        else if(strcmp(argv[1], "client") == 0) 
            client();
    }
    */
    
   
    for(int i=2; i<argc; i++)
    {
		printf("argument : %s\n", argv[i]);
	}
    
    config();
    
    exit(0);
}

/*
SE_lectureEntier(cfgFile, &j);
  // j = 4 
SE_lectureEntier(cfgFile, &gain[j-1]);
  // gain[3] = (GAIN POUR 4 CHIFFRES)
*/


//~ if (SE_lectureEntier(cfgFile, &nbrGagnants) == -1)
 //~ PRINT_ON_ERR("Lecture de nbrGagnants")


//~ tabGagnants = malloc(nbrGagnants * sizeof(int));
//~ tabGain     = malloc(nbrGagnants * sizeof(int));

//~ for (int i = 0; i < nbrGagnants; ++i) {
	//~ if (SE_lectureEntier(cfgFile, &tabGagnants[i]) == -1)
		 //~ PRINT_ON_ERR("Lecture des numéros gagnants")


//~ int fd = open ("/tmp/tube", O_WRONLY);
    //~ write(fd, &argc, sizeof(int)); // sends count of chosen numbers
    //~ for(int i = 0; i < argc; i++) write(fd, &argv[i], sizeof(int));//sends chosen numbers
    //~ close(fd);

//~ write(fd, &entier, sizeof(int));

//~ for(int i = 0; i < size_c; i++){
        //~ read(fd, &table[i], sizeof(int));
        //~ printf("%d ",table[i]);
    //~ }
    //~ printf("\n");

int serveur(const char *chemin, int *tab, int tailleMax)
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
		}
	}
	
	SE_fermeture (tube);
	unlink (chemin);
	
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
	
	mkfifo ("/tmp/tube", 0600);
	
	tube = SE_ouverture(chemin, O_WRONLY);

	if (tube.descripteur == -1)
		return -1;
		
	if(numWin == 0)
	{
		if(SE_ecritureEntier(tube, 0) == -1)
		{
			printf("Une erreur d'écriture a eu lieu\n");
			return -1;
		}
	}
	
	else
	{
		for(int x = 0; x<2; x++)
		{
			if(SE_ecritureEntier(tube, tab[tailleMax-(2*numWin)+x]) == -1)
			{
				printf("Une erreur d'écriture a eu lieu\n");
				return -1;
			}
		}
	}
	
	SE_fermeture (tube);
		
	return 0;
}

int client(const char *chemin, int argc, char* argv[])
{
	SE_FICHIER tube;
	int i;
	tube = SE_ouverture (chemin, O_WRONLY);

	if (tube.descripteur == -1)
		return -1;
	
	//On commence à 2 car on ne veux pas lire ./million client
	for(int x = 2; x<argc; x++)
	{
		if(SE_ecritureEntier(tube, atoi(argv[x])) == -1)
		{
			printf("Une erreur d'écriture a eu lieu\n");
			return -1;
		}
	}
	SE_fermeture (tube);
	
	tube = SE_ouverture(chemin, O_RDONLY);
	
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
	unlink (chemin);
		
	return 0;
}

int main(int argc, char* argv[]){
	//Comparer le nombre d'argument avec le nombre de numéros de la lottery 
	int *tab;
	int tailleMax;
    
    tab = config(&tailleMax);
    
    mkfifo ("/tmp/tube", 0600);
	
	if(!strcmp(argv[1], "client"))
	{
		//Utilisisation de la fonction client
		client("/tmp/tube", argc, argv);
	}
	
	else if(!strcmp(argv[1], "server"))
	{
		//Utilisisation de la fonction serveur
		serveur("/tmp/tube", tab, tailleMax);
		free(tab);
	}
	
	else
		printf("L'argument : %s n'est pas reconnue\n", argv[1]);
	
    for(int i=2; i<argc; i++)
    {
		printf("argument : %s\n", argv[i]);
	}
	
    exit(0);
}

//~ int serveurLectureTest(const char *chemin)
//~ {
	//~ int i;
	//~ int nbreNum = 4;
	
	//~ SE_FICHIER tube = SE_ouverture(chemin, O_RDONLY);

	//~ if (tube.descripteur == -1)
		//~ return -1;
	
	//~ printf("Le joueur a joué les numéros : ");
	
	//~ for(int cmpt = 0; cmpt < nbreNum; cmpt++)
	//~ {
		
		//~ if(SE_lectureEntier(tube, &i) == -1)
			//~ return -1;

		//~ printf("%d ", i);
	//~ }
	
	//~ SE_fermeture (tube);
	//~ SE_suppression (chemin);
	
	//~ printf("\nFin de traitement du serveur\n");
	
	//~ char str[256];
	//~ int fd =-1;
	
	//~ if ( (fd = open(chemin, O_RDONLY)) == -1)
	//~ {
		//~ perror("Erreur open\n");
		//~ return -1;
	//~ }
	
	//~ read(fd, str, 256);
	//~ printf("message reçu : %s\n", str);
	//~ unlink(chemin);
	
	//~ return 0;
//~ }
