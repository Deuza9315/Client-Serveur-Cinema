#include "protocole.h"

int main() {
    // Connexion à la file
    int file_id = msgget(CLE_FILE, 0666);
    if (file_id == -1) {
        perror("Erreur : Impossible de se connecter à la file de messages. Assurez-vous que le serveur est lancé.");
        return 1;
    }

    // Instanciation des messages
    MessageRequete req;
    MessageReponse res;

    while (1) {
        printf("\n1.Liste 2.Reserver 0.Quitter : ");
        scanf("%d", &req.action);

        req.type = TYPE_REQ; // On prépare le type

        if (req.action == 0) {
            // Envoyer (Pointeur &req) et quitter
            msgsnd(file_id, &req, sizeof(req) - sizeof(long), 0);
            break;
        }

        if (req.action == 2) {
            printf("ID : "); scanf("%d", &req.id_spec);
            printf("Places : "); scanf("%d", &req.nb_places);
        }

        // Envoyer la demande (Pointeur &req)
        if (msgsnd(file_id, &req, sizeof(req) - sizeof(long), 0) == -1) {
            perror("Erreur lors de l'envoi du message");
            break;
        }

        // Recevoir la réponse (Pointeur &res)
        if (msgrcv(file_id, &res, sizeof(res) - sizeof(long), TYPE_RES, 0) == -1) {
            perror("Erreur lors de la réception du message");
            break;
        }

        // Afficher le texte reçu
        printf("Reponse : %s\n", res.texte);
    }

    return 0;
}
