#include "protocole.h"

typedef struct { int id; char nom[50]; int stock; } Spectacle;

int main() {
    int file_id = msgget(CLE_FILE, 0666 | IPC_CREAT);

    Spectacle specs[3] = {
        {0, "Le Roi Lion", 100},
        {1, "Moliere", 50},
        {2, "Starmania", 10}
    };

    MessageRequete req;
    MessageReponse res;

    printf("Serveur pret... (ID File : %d)\n", file_id);

    while (1) {
        msgrcv(file_id, &req, sizeof(req) - sizeof(long), TYPE_REQ, 0);
        res.type = TYPE_RES;

        if (req.action == 0) break;

        // --- ACTION 1 : LISTE (On affiche maintenant le stock) ---
        if (req.action == 1) { 
            sprintf(res.texte, "[0] %s: %d places, [1] %s: %d places, [2] %s: %d places",
                    specs[0].nom, specs[0].stock, 
                    specs[1].nom, specs[1].stock, 
                    specs[2].nom, specs[2].stock);
        } 
        
        // --- ACTION 2 : RESERVATION (Plus de logs serveur et client) ---
        else if (req.action == 2) {
            // Sécurité pour ne pas déborder du tableau
            if (req.id_spec >= 0 && req.id_spec < 3) {
                if (specs[req.id_spec].stock >= req.nb_places) {
                    specs[req.id_spec].stock -= req.nb_places;
                    
                    // Message pour le client
                    sprintf(res.texte, "Reservation OK ! Il reste %d places pour %s.", 
                            specs[req.id_spec].stock, specs[req.id_spec].nom);
                    
                    // Log complet pour le serveur
                    printf("VENTE : ID %d | Spectacle : %s | Places vendues : %d | Nouveau stock : %d\n", 
                           req.id_spec, specs[req.id_spec].nom, req.nb_places, specs[req.id_spec].stock);
                } else {
                    sprintf(res.texte, "Erreur : Plus assez de places (Reste : %d)", specs[req.id_spec].stock);
                }
            } else {
                strcpy(res.texte, "ID spectacle inconnu !");
            }
        }

        msgsnd(file_id, &res, sizeof(res) - sizeof(long), 0);
    }

    // Supprimer la file
    if (msgctl(file_id, IPC_RMID, NULL) == -1) {
        perror("Erreur lors de la suppression de la file");
    }
    return 0;
}

