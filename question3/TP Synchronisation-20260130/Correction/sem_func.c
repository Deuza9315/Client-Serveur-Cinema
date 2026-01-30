
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "sem_func.h"

int semid;
struct sembuf operation;


int init_verrou(void)
{
  semid = semget(22, 1, IPC_CREAT|IPC_EXCL|0600);
  
  if(semid==-1)
  {
    printf("\nErreur de creation verrou!\n");
    return -1;
  }
  else
  {
    semctl(semid, 0, SETVAL, 1);
    return 0;
  }
}

void prendre_verrou(void)
{
  /* opération P */
  operation.sem_num= 0;
  operation.sem_op = -1;
  operation.sem_flg= 0;
  
  semop(semid, &operation, 1);
}

void rendre_verrou(void)
{
  /* opérationV */
  operation.sem_num= 0;
  operation.sem_op= 1;
  operation.sem_flg= 0;
  
  semop(semid, &operation, 1);
}
