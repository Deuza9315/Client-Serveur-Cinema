#include "protocole.h"
#include <pthread.h>
#include <unistd.h>

// --- Instanciation du mutex (déclaré dans protocole.h) ---
pthread_mutex_t mutex_spectacles = PTHREAD_MUTEX_INITIALIZER;

// --- Données des spectacles ---
typedef struct { int id; char nom[50]; int stock; } Spectacle;
Spectacle specs[] = {
    {0, "Le Roi Lion", 100},
    {1, "Molière", 50},
    {2, "Starmania", 10}
};
#define NB_SPECTACLES (sizeof(specs) / sizeof(Spectacle))

// --- ID de la file global ---
int file_id_global;

// --- Fonction unique exécutée par un thread détaché pour chaque requête ---
void* thread_traitement_requete(void* arg) {
    // On récupère le message copié dans le tas
    MessageRequete* req_ptr = (MessageRequete*) arg;
    MessageRequete req = *req_ptr; // Copie locale
    free(req_ptr); // On libère la mémoire allouée par le main

    MessageReponse res;
    
    printf("\n[Thread %lu] Prise en charge requête Type %ld...\n", pthread_self(), req.type);

    // --- LOGIQUE METIER ---
    
    if (req.type == TYPE_REQ_CONSULTATION) {
        res.type = TYPE_REP_CONSULTATION;
        
        // Lecture seule, mais pour être propre on protège aussi (ou on utilise un RWLock)
        // Ici Mutex simple comme demandé
        pthread_mutex_lock(&mutex_spectacles);
        strcpy(res.texte, "\n--- SPECTACLES ---\n");
        for (int i = 0; i < NB_SPECTACLES; i++) {
            char ligne[100];
            sprintf(ligne, "[%d] %s : %d places\n", specs[i].id, specs[i].nom, specs[i].stock);
            strcat(res.texte, ligne);
        }
        pthread_mutex_unlock(&mutex_spectacles);
        
    } 
    else if (req.type == TYPE_REQ_RESERVATION) {
        res.type = TYPE_REP_RESERVATION;
        
        pthread_mutex_lock(&mutex_spectacles); // EXCLUSION MUTUELLE OBLIGATOIRE
        
        if (req.id_spec >= 0 && req.id_spec < NB_SPECTACLES) {
            if (specs[req.id_spec].stock >= req.nb_places) {
                specs[req.id_spec].stock -= req.nb_places;
                sprintf(res.texte, "OK ! %d places réservées pour %s.", req.nb_places, specs[req.id_spec].nom);
            } else {
                sprintf(res.texte, "ECHEC : Pas assez de places (Reste: %d).", specs[req.id_spec].stock);
            }
        } else {
            strcpy(res.texte, "ECHEC : ID Invalide.");
        }
        
        pthread_mutex_unlock(&mutex_spectacles);
    }
    
    // --- ENVOI REPONSE ---
    if (msgsnd(file_id_global, &res, sizeof(res) - sizeof(long), 0) == -1) {
        perror("Err msgsnd worker");
    } else {
        printf("[Thread %lu] Réponse envoyée (Type %ld).\n", pthread_self(), res.type);
    }

    return NULL;
}

int main() {
    file_id_global = msgget(CLE_FILE, 0666 | IPC_CREAT);
    if (file_id_global == -1) { perror("msgget"); return 1; }
    
    printf("=== SERVEUR Q4 (Dynamic Threads) EN MARCHE ===\n");

    while (1) {
        // Allocation dynamique pour passer les données au thread
        MessageRequete* req_recue = malloc(sizeof(MessageRequete));
        if (!req_recue) { perror("malloc"); continue; }

        // --- RECEPTION ---
        // On écoute tout message de type <= 9 (Donc REQ_CONSULT(1), REQ_RESERV(2) et QUIT(5))
        // Mais on ignore les Types REP (10, 11) pour ne pas lire ses propres réponses !
        if (msgrcv(file_id_global, req_recue, sizeof(MessageRequete) - sizeof(long), -9, 0) == -1) {
            perror("msgrcv");
            free(req_recue);
            continue;
        }

        if (req_recue->type == TYPE_QUITTER_SERVEUR) {
            printf("Ordre d'arrêt reçu.\n");
            free(req_recue);
            break;
        }

        // --- CREATION THREAD DYNAMIQUE ---
        pthread_t th;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // Thread détaché (libère ses ressources seul)
        
        if (pthread_create(&th, &attr, thread_traitement_requete, req_recue) != 0) {
            perror("pthread_create");
            free(req_recue);
        } else {
            // Le thread gérera le free(req_recue)
            pthread_attr_destroy(&attr);
        }
    }

    msgctl(file_id_global, IPC_RMID, NULL);
    pthread_mutex_destroy(&mutex_spectacles);
    printf("Serveur terminé.\n");
    return 0;
}