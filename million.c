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
#include "io_file.h"

#define TAILLE_MESSAGE 16
#define BUFFER_SIZE 16

void gestionTube()
{
	if(mkfifo("/tmp/tube", 0777) != 0)
	{
		printf("Echec du mkfifo erreur lors de la création du processus\n");
		exit(1);
	}
	
	int fd = open("/tmp/tube", O_RDWR);
	
	if(fd < 0)	
		perror("Problème avec open ");
	
	if(!fork())
	{
		if(write(fd, "Hello World!", TAILLE_MESSAGE) < 0)	
			perror("Problème avec write ");
			
		printf("Le processus fils a écrit le message dans le tube !\n");
	}
	
	else
	{
		wait(NULL);
		char lecture[TAILLE_MESSAGE];
		read(fd, lecture, TAILLE_MESSAGE);
		printf("Le processus père reçoit le message : %s\n\n",lecture);
		close(fd);
		unlink("/tmp/tube");
	}
}

//Fichier configuration
void config()
{
	
}

//Lecture du tube
void serveur()
{
	
}

//Lecture du tube
void client()
{
	
}

void q1Named() 
{
	int fd;
	char buf [BUFFER_SIZE];

	mkfifo ("/tmp/tube", 0600);

	if (fork ()) 
	{
		fd = open ("/tmp/tube", O_WRONLY);

		strcpy (buf, "Here is Brian!");

		write (fd, buf, BUFFER_SIZE);

		close (fd);

		wait (NULL);
	} 
	
	else 
	{
		fd = open ("/tmp/tube", O_RDONLY);

		read (fd, buf, BUFFER_SIZE);

		printf ("%s\n", buf);

		close (fd);

		exit (0);
	}

	unlink ("/tmp/tube");
}

void q2Named () {
	int fd, number1, number2;
	char buf [BUFFER_SIZE];

	mkfifo ("/tmp/tube", 0600);

	if (fork()) 
	{
		fd = open ("/tmp/tube", O_WRONLY);

		number1 = rand () % 1000;
		number2 = rand () % 10;

		write (fd, &number1, sizeof (int) );
		write (fd, &number2, sizeof (int) );

		close (fd);

		wait (NULL);
	} 
	
	else 
	{
		fd = open ("/tmp/tube", O_RDONLY);

		read (fd, &number1, sizeof (int) );
		read (fd, &number2, sizeof (int) );

		printf ("(%d, %d)\n", number1, number2);

		close (fd);

		exit (0);
	}

	unlink ("/tmp/tube");
}

int main(int argc, char* argv[]){
    
    //doit y avoir plus de trois arguments minimum 
    if (argc < 3)
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
    
    exit(0);
}
