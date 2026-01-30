#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int pip[2];

void fonction_handler(void)
{
   printf("signal SIGPIPE recu\n");
   close(pip[0]);
   close(pip[1]);
   
   exit(0);
}

int main(void)
{
  int nb_ecrit;
  int pid;
  
  // Association fonction de traitement au signal SIGPIPE
  signal(SIGPIPE, (__sighandler_t) fonction_handler);
  
  /*  ouverture d'un pipe */
  if(pipe(pip))
  { 
    perror("pipe");
    exit(1);
  }
  
  pid = fork();
  if (pid == 0)
  {
    close(pip[0]);
    close(pip[1]);
    printf("Je suis le fils\n");
    exit(0);
  }
  else
  {
    close(pip[0]);
    while(1){
      if ((nb_ecrit = write(pip[1], "ABC", 3)) == -1)
      {
        perror ("pb write");
        exit(2);
      }
      else
        printf ("retour du write : %d\n", nb_ecrit);
    }
  }
  
  exit(0);
}
