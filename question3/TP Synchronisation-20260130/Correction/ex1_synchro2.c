
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "sem_func.h"

int nb_places=5;

void * reservation(void *p)
{
	int j;
	int *n;
	
	n=(int *) p;
	
	for(j=0; j<10; j++)
	{	
		printf("\n[Proc. Reservation-%d] Debut réservation place.\n", *n);
		prendre_verrou();
		if(nb_places>0)
		{
			sleep(2);
			nb_places=nb_places-1;
			printf("[Proc. Reservation-%d] Place réservée.\n", *n);
		}
		else printf("[Proc. Reservation-%d] Aucune place disponible!\n", *n);

		printf("[Proc. Reservation-%d] Fin réservation.\n", *n);
		fflush(stdout);
		// rendre_verrou();
	
		sleep(2);
	}
}

void * consultation(void *arg)
{
	int i;
	
	for(i=0; i<10; i++)
	{		
		prendre_verrou();
		printf("\n[Proc. Consultation] Debut consultation places.\n");
		printf("[Proc. Consultation] Il reste %d places.\n", nb_places);
	
		printf("[Proc. Consultation] Fin consultation.\n");
		fflush(stdout);
		
		// rendre_verrou();
		
		sleep(2);
	}
}

void main(void)
{
	int k, val, val2, ret;
	pthread_t num_thread[3];

	ret=init_verrou();
	
	if(ret==-1) exit(-1);
  
	if(pthread_create(&num_thread[0], NULL, &consultation, NULL) == -1)
	{
		perror("pb pthread_create\n");
	}

	val=1;
	if(pthread_create(&num_thread[1], NULL, &reservation, &val) == -1)
	{
		perror("pb pthread_create\n");
	}
	
	val2=2;
	if(pthread_create(&num_thread[2], NULL, &reservation, &val2) == -1)
	{
		perror("pb pthread_create\n");
	}
	
	for(k=0; k<3; k++)
	{
		if(num_thread[k]>0) pthread_join(num_thread[k], NULL);
	}
  
	exit(0);
}
