
/******************************************************************/
/* Processus A */
/******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void)
{
  char chaine[30];
  mode_t mode;
  int tub1, tub2;
  mode = S_IRUSR | S_IWUSR;

  /*création du tube nommé */
  mkfifo ("fictub1", mode);
 
  /* ouverture des tubes */
  tub1 = open("fictub1", O_WRONLY);
  tub2 = open("fictub2", O_RDONLY);

  /* écriture dans le tube */
  write(tub1, "hello, je suis le processus A", 29);

  /* lecture dans le tube */
  read(tub2, chaine, 29);
  chaine[30]='\0';
  
  printf("(Proc A) la chaine lue est : %s\n", chaine);

  /* fermeture des tubes */
  close(tub1);
  close(tub2);
  
  exit(0);
}

