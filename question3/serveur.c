#include "protocole.h"

// --- Instanciation du mutex (déclaré dans protocole.h) ---
pthread_mutex_t mutex_spectacles = PTHREAD_MUTEX_INITIALIZER;

// --- Données des spectacles (Globales pour être accessibles par tous les threads) ---
typedef struct { int id; char nom[50]; int stock; } Spectacle;
Spectacle specs[] = {
    {0, "Le Roi Lion", 100},
    {1, "Molière", 50},
    {2, "Starmania", 10}
};
// On garde la taille pour faciliter les boucles
#define NB_SPECTACLES (sizeof(specs) / sizeof(Spectacle))

// --- Variable pour stocker l'ID de la file de messages (Globale pour les threads) ---
int file_id_global; 

// --- Fonction exécutée par le thread de Consultation ---
void* thread_consultation(void* arg) {
    MessageRequete req;
    MessageReponse res;

    printf("Thread Consultation lancé...\n");

    while (1) {
        // msgrcv : attend un message de type REQ_CONSULTATION (bloquant)
        msgrcv(file_id_global, &req, sizeof(req) - sizeof(long), TYPE_REQ_CONSULTATION, 0);

        printf("[Consultation] Requête reçue.\n");

        res.type = TYPE_REP_CONSULTATION; // Type de réponse pour ce thread

        // --- SECTION CRITIQUE : Accès aux données partagées ---
        pthread_mutex_lock(&mutex_spectacles); // Verrouiller le mutex avant de lire
        // Préparation de la réponse avec la liste des spectacles
        strcpy(res.texte, "\n--- SPECTACLES ---\n");
        for (int i = 0; i < NB_SPECTACLES; i++) {
            char ligne[100];
            sprintf(ligne, "[%d] %s : %d places\n", specs[i].id, specs[i].nom, specs[i].stock);
            strcat(res.texte, ligne);
        }
        pthread_mutex_unlock(&mutex_spectacles); // Déverrouiller le mutex après la lecture
        // --- FIN SECTION CRITIQUE ---

        // msgsnd : envoie la réponse spécifique de consultation
        msgsnd(file_id_global, &res, sizeof(res) - sizeof(long), 0);
        printf("[Consultation] Réponse envoyée.\n");
    }
    return NULL;
}

// --- Fonction exécutée par le thread de Réservation ---
void* thread_reservation(void* arg) {
    MessageRequete req;
    MessageReponse res;

    printf("Thread Réservation lancé...\n");

    while (1) {
        // msgrcv : attend un message de type REQ_RESERVATION (bloquant)
        msgrcv(file_id_global, &req, sizeof(req) - sizeof(long), TYPE_REQ_RESERVATION, 0);

        printf("[Réservation] Requête reçue : ID %d, Places %d\n", req.id_spec, req.nb_places);

        res.type = TYPE_REP_RESERVATION; // Type de réponse pour ce thread

        // --- SECTION CRITIQUE : Accès aux données partagées ---
        pthread_mutex_lock(&mutex_spectacles); // Verrouiller le mutex avant de modifier

        if (req.id_spec >= 0 && req.id_spec < NB_SPECTACLES) {
            if (specs[req.id_spec].stock >= req.nb_places) {
                specs[req.id_spec].stock -= req.nb_places; // Modification du stock
                sprintf(res.texte, "OK ! %d places réservées pour %s (Reste: %d).", req.nb_places, specs[req.id_spec].nom, specs[req.id_spec].stock);
                printf("[Réservation] Succès : %d places pour %s. Nouveau stock: %d\n", req.nb_places, specs[req.id_spec].nom, specs[req.id_spec].stock);
            } else {
                sprintf(res.texte, "ERREUR : Pas assez de places pour %s (Reste: %d).", specs[req.id_spec].nom, specs[req.id_spec].stock);
                printf("[Réservation] Échec : Stock insuffisant pour %s.\n", specs[req.id_spec].nom);
            }
        } else {
            sprintf(res.texte, "ERREUR : ID spectacle invalide.");
            printf("[Réservation] Échec : ID spectacle %d invalide.\n", req.id_spec);
        }
        pthread_mutex_unlock(&mutex_spectacles); // Déverrouiller le mutex après la modification
        // --- FIN SECTION CRITIQUE ---

        // msgsnd : envoie la réponse spécifique de réservation
        msgsnd(file_id_global, &res, sizeof(res) - sizeof(long), 0);
        printf("[Réservation] Réponse envoyée.\n");
    }
    return NULL;
}


int main() {
    pthread_t id_thread_consultation, id_thread_reservation; // Instanciation des ID de threads

    // 1. Création de la file de messages et stockage de son ID globalement
    file_id_global = msgget(CLE_FILE, 0666 | IPC_CREAT);
    if (file_id_global == -1) { perror("Erreur msgget serveur"); return 1; }

    printf("=== SERVEUR MULTI-THREADÉ EN MARCHE (File ID: %d) ===\n", file_id_global);

    // 2. Initialisation du mutex (même si PTHREAD_MUTEX_INITIALIZER l'a déjà fait, c'est bonne pratique)
    // pthread_mutex_init(&mutex_spectacles, NULL); 

    // 3. Création des deux threads
    // pthread_create : lance la fonction dans un nouveau thread
    pthread_create(&id_thread_consultation, NULL, thread_consultation, NULL);
    pthread_create(&id_thread_reservation, NULL, thread_reservation, NULL);

    // Le thread principal (main) du serveur attend un message de "quitter"
    // Ce message n'est pas traité par les threads de consultation/réservation
    MessageRequete req_quitter;
    msgrcv(file_id_global, &req_quitter, sizeof(req_quitter) - sizeof(long), TYPE_QUITTER_SERVEUR, 0);
    printf("Message de QUIT_SERVEUR reçu. Arrêt des threads et du serveur...\n");

    // Pour une fermeture propre, il faudrait envoyer des messages d'arrêt aux threads,
    // ou utiliser des variables de condition/flags, mais pour la simplicité de l'exercice,
    // on va juste terminer le main et laisser les threads s'arrêter "brutalement".
    // En vrai, il faudrait faire : pthread_cancel(id_thread_consultation); etc.

    // On attend la fin des threads (pour une application robuste, mais pas nécessaire si le main quitte)
    // pthread_join(id_thread_consultation, NULL);
    // pthread_join(id_thread_reservation, NULL);

    // 4. Destruction du mutex
    pthread_mutex_destroy(&mutex_spectacles);

    // 5. Nettoyage de la file de messages
    msgctl(file_id_global, IPC_RMID, NULL);
    printf("Serveur fermé.\n");
    return 0;
}