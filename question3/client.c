#include "protocole.h"

int main() {
    // 1. Récupération de la file existante
    int file_id = msgget(CLE_FILE, 0666);
    if (file_id == -1) { 
        printf("Le serveur n'est pas lancé !\n"); 
        return 1; 
    }

    printf("=== CLIENT CONNECTÉ ===\n");

    MessageRequete req;
    MessageReponse res;

    while (1) {
        printf("\n1. Voir liste (Consultation)\n2. Réserver\n0. Quitter\nChoix : ");
        if (scanf("%d", &req.action) != 1) { // On lit l'action choisie
            // Gérer les erreurs de scanf pour éviter les boucles infinies
            while (getchar() != '\n'); // Vide le buffer d'entrée
            continue;
        }

        // --- Le client envoie le bon type de message en fonction de l'action ---
        if (req.action == 0) { // Quitter le client ET demander au serveur de s'arrêter
            req.type = TYPE_QUITTER_SERVEUR; 
            msgsnd(file_id, &req, sizeof(req) - sizeof(long), 0);
            break;
        }
        else if (req.action == 1) { // Consultation
            req.type = TYPE_REQ_CONSULTATION;
            req.id_spec = -1; // Pas besoin d'ID pour la liste complète
            req.nb_places = 0; // Pas de places pour la consultation
        }
        else if (req.action == 2) { // Réservation
            req.type = TYPE_REQ_RESERVATION;
            printf("ID du spectacle : ");
            if (scanf("%d", &req.id_spec) != 1) {
                while (getchar() != '\n');
                continue;
            }
            printf("Nombre de places : ");
            if (scanf("%d", &req.nb_places) != 1) {
                while (getchar() != '\n');
                continue;
            }
        } else {
            printf("Choix invalide. Veuillez réessayer.\n");
            continue; // Retourne au début de la boucle
        }

        // 2. Envoi de la requête
        msgsnd(file_id, &req, sizeof(req) - sizeof(long), 0);
        printf("Requête envoyée (Type %ld).\n", req.type);

        // 3. Attente de la réponse du BON type
        // On attend la réponse correspondant au type de requête envoyé
        if (req.action == 1) { // Si c'était une consultation
            msgrcv(file_id, &res, sizeof(res) - sizeof(long), TYPE_REP_CONSULTATION, 0);
        }
        else if (req.action == 2) { // Si c'était une réservation
            msgrcv(file_id, &res, sizeof(res) - sizeof(long), TYPE_REP_RESERVATION, 0);
        }

        // Affichage du résultat reçu du serveur
        printf("--- RÉPONSE DU SERVEUR ---\n%s\n", res.texte);
    }

    printf("Fermeture du client.\n");
    return 0;
}