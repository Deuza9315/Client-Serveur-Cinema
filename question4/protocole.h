#ifndef PROTOCOLE_H
#define PROTOCOLE_H

#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> // Ajout pour les mutex et threads

// Clé unique pour la file de messages (même qu'avant)
#define CLE_FILE 12345

// --- NOUVEAUX CODES POUR DIRIGER LES MESSAGES ---
// --- NOUVEAUX CODES POUR DIRIGER LES MESSAGES ---
#define TYPE_REQ_CONSULTATION 1 
#define TYPE_REQ_RESERVATION  2 
#define TYPE_QUITTER_SERVEUR  5 

#define TYPE_REP_CONSULTATION 10 
#define TYPE_REP_RESERVATION  11

// Structure du message envoyé par le CLIENT (avec les mêmes champs qu'avant)
typedef struct {
    long type;      // Type du message (TYPE_REQ_CONSULTATION ou TYPE_REQ_RESERVATION)
    int action;     // Pas strictement nécessaire ici car le type fait déjà le tri, mais on le garde pour compatibilité ou actions futures
    int id_spec;    // ID du spectacle
    int nb_places;  // Nombre de places
} MessageRequete;

// Structure du message renvoyé par le SERVEUR (ou les threads)
typedef struct {
    long type;      // Type du message (TYPE_REP_CONSULTATION ou TYPE_REP_RESERVATION)
    char texte[512]; // Texte à afficher
} MessageReponse;

// --- MUTEX POUR PROTÉGER LES DONNÉES PARTAGÉES (le tableau des spectacles) ---
// Déclaré ici pour être accessible partout
extern pthread_mutex_t mutex_spectacles;

#endif