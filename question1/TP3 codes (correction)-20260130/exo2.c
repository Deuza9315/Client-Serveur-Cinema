
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define P 2

void lecture_grandeurs(int *g)
{
  g[0]=rand()%5000;
  g[1]=rand()%5000;
  g[2]=rand()%5000;
  g[3]=rand()%5000;
  g[4]=rand()%5000;
}

int main(int argc, char **argv)
{
  /* déclarations de variables */
  pid_t pid;
  int i;
  int grandeurs[5]; /* tableau de 5 entiers qui contient les grandeurs lues par PA */
  int tube[2];
  char gd1[5], gd2[5], gd3[5], gd4[5], gd5[5], tmp[5][5];
  
  srand(time(NULL));
  i=0;
  
  /* mise en place des moyens de communication et création des processus */
  pipe(tube);
  
  pid=fork();
  
  if(pid == 0)
  { // Processus PB
    close(tube[1]);
    
    while(i < 10)
    {
      read(tube[0], tmp[0], 5);
      read(tube[0], tmp[1], 5);
      read(tube[0], tmp[2], 5);
      read(tube[0], tmp[3], 5);
      read(tube[0], tmp[4], 5);
      
      printf("Valeurs recues par processus B : %s, %s, %s, %s, %s\n", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);
      
      i=i+1;
    }
  }
  else
  { // Processus PA
    close(tube[0]);
    while(i < 100) /* il y a encore des mesures à faire */
    {
      /* lire les grandeurs */
      lecture_grandeurs(grandeurs);
      
      /* écrire vers le fils */
      sprintf(gd1, "%d", grandeurs[0]);
      sprintf(gd2, "%d", grandeurs[1]);
      sprintf(gd3, "%d", grandeurs[2]);
      sprintf(gd4, "%d", grandeurs[3]);      
      sprintf(gd5, "%d", grandeurs[4]);
      
      printf("Valeurs relevees par Processus A : %d, %d, %d, %d, %d\n", grandeurs[0], grandeurs[1], grandeurs[2], grandeurs[3], grandeurs[4]);
      
      write(tube[1], gd1, 5);
      write(tube[1], gd2, 5);
      write(tube[1], gd3, 5);
      write(tube[1], gd4, 5);
      write(tube[1], gd5, 5);
      
      i=i+1;
      sleep(P) ; /* dormir durant P unités de temps*/
    }
    
    wait(NULL);
  }
  
  exit(0);
}
