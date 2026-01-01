#ifndef PROTOCOLE_H
#define PROTOCOLE_H

#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>

#define CLE_FILE 12345 // Clé unique pour la file de messages
#define TYPE_REQ 1     // Type de message pour les requêtes client -> serveur
#define TYPE_RES 2     // Type de message pour les réponses serveur -> client

// Structure de message (Modèle pour le client)
typedef struct {
    long type;      
    int action;     
    int id_spec;    
    int nb_places;  
} MessageRequete;

// Structure de message (Modèle pour le serveur)
typedef struct {
    long type;       // Doit être TYPE_RES
    char texte[512]; // Contenu de la réponse
} MessageReponse;

#endif
