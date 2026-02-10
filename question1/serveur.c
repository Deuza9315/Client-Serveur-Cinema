#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define MAX_SPECTACLES 10
#define MAX_NAME_LEN 50
#define MAX_BUFFER 256

// TES STRUCTURES (INCHANGÉES)
typedef enum { REQ_CONSULTATION = 1, REQ_RESERVATION = 2, REQ_LISTE = 3 } TypeRequete;
typedef struct { int id; char titre[MAX_NAME_LEN]; int places_restantes; } Spectacle;
typedef struct { TypeRequete type; int id_spectacle; int nb_places; } Requete;
typedef struct { int status; char message[MAX_BUFFER]; int val_retour; Spectacle liste[MAX_SPECTACLES]; } Reponse;

Spectacle table_spectacles[MAX_SPECTACLES];
int nb_spectacles = 0;

void init_spectacles() {
    table_spectacles[0] = (Spectacle){0, "Le Roi Lion", 100};
    table_spectacles[1] = (Spectacle){1, "Notre Dame de Paris", 50};
    table_spectacles[2] = (Spectacle){2, "Moliere", 10};
    table_spectacles[3] = (Spectacle){3, "Starmania", 0};
    nb_spectacles = 4;
}

int main() {
    Requete req;
    Reponse res;

    init_spectacles();

    // Création des tubes nommés
    mkfifo("fifo_req", 0666);
    mkfifo("fifo_res", 0666);

    printf("[SERVEUR] Démarré. En attente de requêtes...\n");

    while (1) {
        // Ouverture des tubes
        int fd_read = open("fifo_req", O_RDONLY);
        int fd_write = open("fifo_res", O_WRONLY);

        if (read(fd_read, &req, sizeof(Requete)) > 0) {
            // TON CODE DE LOGIQUE (COPIÉ-COLLÉ)
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
            // FIN DE TA LOGIQUE
            
            write(fd_write, &res, sizeof(Reponse));
        }
        close(fd_read);
        close(fd_write);
    }
    return 0;
}
