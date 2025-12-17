#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define MAX_SPECTACLES 10
#define MAX_NAME_LEN 50
#define MAX_BUFFER 256

typedef enum {
    REQ_CONSULTATION = 1,
    REQ_RESERVATION = 2,
    REQ_LISTE = 3
} TypeRequete;

typedef struct {
    int id;
    char titre[MAX_NAME_LEN];
    int places_restantes;
} Spectacle;

typedef struct {
    TypeRequete type;
    int id_spectacle;
    int nb_places;
} Requete;

typedef struct {
    int status;
    char message[MAX_BUFFER];
    int val_retour;
    Spectacle liste[MAX_SPECTACLES];
} Reponse;

Spectacle table_spectacles[MAX_SPECTACLES];
int nb_spectacles = 0;

void init_spectacles() {
    table_spectacles[0] = (Spectacle){0, "Le Roi Lion", 100};
    table_spectacles[1] = (Spectacle){1, "Notre Dame de Paris", 50};
    table_spectacles[2] = (Spectacle){2, "Moliere", 10};
    table_spectacles[3] = (Spectacle){3, "Starmania", 0};
    nb_spectacles = 4;
}

void run_server(int fd_read, int fd_write) {
    Requete req;
    Reponse res;

    init_spectacles();
    printf("[SERVEUR] Démarré (PID %d). En attente de requêtes...\n", getpid());

    while (read(fd_read, &req, sizeof(Requete)) > 0) {
        memset(&res, 0, sizeof(Reponse));
        res.status = 0;

        if (req.type == REQ_LISTE) {
            res.status = 1;
            memcpy(res.liste, table_spectacles, sizeof(table_spectacles));
        } 
        else if (req.id_spectacle >= 0 && req.id_spectacle < nb_spectacles) {
            Spectacle *s = &table_spectacles[req.id_spectacle];
            
            if (req.type == REQ_CONSULTATION) {
                res.status = 1;
                res.val_retour = s->places_restantes;
                sprintf(res.message, "%d places pour '%s'", s->places_restantes, s->titre);
            } 
            else if (req.type == REQ_RESERVATION) {
                if (s->places_restantes >= req.nb_places) {
                    s->places_restantes -= req.nb_places;
                    res.status = 1;
                    sprintf(res.message, "Réservé: %d places pour '%s'", req.nb_places, s->titre);
                } else {
                    sprintf(res.message, "Echec: seulement %d places restantes", s->places_restantes);
                }
            }
        } else {
            strcpy(res.message, "ID Spectacle invalide");
        }

        write(fd_write, &res, sizeof(Reponse));
    }

    printf("[SERVEUR] Client déconnecté. Arrêt du serveur.\n");
    close(fd_read);
    close(fd_write);
    exit(0);
}

void run_client(int fd_read, int fd_write) {
    Requete req;
    Reponse res;

    printf("[CLIENT] Démarré (PID %d).\n", getpid());

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Liste des spectacles\n");
        printf("2. Consulter places\n");
        printf("3. Réserver places\n");
        printf("0. Quitter\n");
        printf("Choix : ");
        
        int choix;
        if (scanf("%d", &choix) != 1) break;
        if (choix == 0) break;

        memset(&req, 0, sizeof(Requete));

        if (choix == 1) req.type = REQ_LISTE;
        else if (choix == 2) {
            req.type = REQ_CONSULTATION;
            printf("ID Spectacle : ");
            scanf("%d", &req.id_spectacle);
        }
        else if (choix == 3) {
            req.type = REQ_RESERVATION;
            printf("ID Spectacle : ");
            scanf("%d", &req.id_spectacle);
            printf("Nombre de places : ");
            scanf("%d", &req.nb_places);
        }
        else continue;

        write(fd_write, &req, sizeof(Requete));

        read(fd_read, &res, sizeof(Reponse));

        printf("\n>> REPONSE : ");
        if (req.type == REQ_LISTE) {
            printf("\n");
            for(int i=0; i<MAX_SPECTACLES; i++) {
                 if(strlen(res.liste[i].titre) > 0)
                    printf("   [%d] %s : %d places\n", res.liste[i].id, res.liste[i].titre, res.liste[i].places_restantes);
            }
        } else {
            printf("%s\n", res.message);
        }
    }

    printf("[CLIENT] Fermeture.\n");
    close(fd_read);
    close(fd_write);
    exit(0);
}

int main() {
    int pipe_req[2];
    int pipe_res[2];

    if (pipe(pipe_req) == -1 || pipe(pipe_res) == -1) {
        perror("Erreur création pipes");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Erreur fork");
        return 1;
    }

    if (pid == 0) {
        close(pipe_req[0]);
        close(pipe_res[1]);

        run_client(pipe_res[0], pipe_req[1]);
        
    } else {
        close(pipe_req[1]);
        close(pipe_res[0]);

        run_server(pipe_req[0], pipe_res[1]);

        wait(NULL);
    }

    return 0;
}
