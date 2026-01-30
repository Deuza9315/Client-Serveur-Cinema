
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "sem_func.h"

int nb_places=5;

void reservation(int *p)
{
	int j;
	
	for(j=0; j<10; j++)
	{
		// ajout fonction prendre_verrou()
		prendre_verrou();
	
		printf("\n[Proc. Reservation-%d] Debut réservation place.\n", *p);	
		if(nb_places>0)
		{
			sleep(2);
			nb_places=nb_places-1;
			printf("[Proc. Reservation-%d] Place réservée.\n", *p);
		}
		else printf("[Proc. Reservation-%d] Aucune place disponible!\n", *p);

		printf("[Proc. Reservation-%d] Fin réservation.\n", *p);
		fflush(stdout);
	
		// ajout fonction rendre_verrou()
		rendre_verrou();
		sleep(2);
	}
}

void consultation(void)
{
	int i;
	
	for(i=0; i<10; i++)
	{
		// ajout fonction prendre_verrou()
		prendre_verrou();
		
		printf("\n[Proc. Consultation] Debut consultation places.\n");
		printf("[Proc. Consultation] Il reste %d places.\n", nb_places);
	
		printf("[Proc. Consultation] Fin consultation.\n");
		fflush(stdout);
	
		// ajout fonction rendre_verrou()
		rendre_verrou();
		sleep(2);
	}
}

void main(void)
{
	int k, val, val2, ret;
	pthread_t num_thread[3];

	ret=init_verrou();
	
	if(ret==-1) exit(-1);
  
	if(pthread_create(&num_thread[0], NULL, (void *(*)())consultation, NULL) == -1)
	{
		perror("pb pthread_create\n");
	}

	val=1;
	if(pthread_create(&num_thread[1], NULL, (void *(*)())reservation, &val) == -1)
	{
		perror("pb pthread_create\n");
	}
	
	// Ajout thread reservation
	val2=2;
	if(pthread_create(&num_thread[2], NULL, (void *(*)())reservation, &val2) == -1)
	{
		perror("pb pthread_create\n");
	}
	
	for(k=0; k<3; k++)
	{
		if(num_thread[k]>0) pthread_join(num_thread[k], NULL);
	}
  
	exit(0);
}
