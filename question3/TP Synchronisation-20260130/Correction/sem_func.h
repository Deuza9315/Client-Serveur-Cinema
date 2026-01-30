
#include <sys/ipc.h>
#include <sys/sem.h>


int init_verrou(void);
void prendre_verrou(void);
void rendre_verrou(void);
