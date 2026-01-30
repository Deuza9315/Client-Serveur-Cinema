
/******************************************************************/
/* Processus B */
/******************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
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
  mkfifo ("fictub2", mode);
  
  /* ouverture des tubes */
  tub1 = open("fictub1", O_RDONLY);
  tub2 = open("fictub2", O_WRONLY);

  /* ecriture dans le tube */
  write(tub2, "hello, je suis le processus B", 29);

  /* lecture dans le tube */
  read(tub1, chaine, 29);
  chaine[30]='\0';
  printf("(Proc B) la chaine lue est : %s\n", chaine);

  /* fermeture des tubes */
  close(tub1);
  close(tub2);
  
  exit(0);
}

