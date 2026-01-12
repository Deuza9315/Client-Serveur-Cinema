# Client-Serveur Cinema
Par RAYANE ADAM SAYF

Ce projet est une application client-serveur de réservation de places de cinéma utilisant les files de messages (IPC Message Queues) en C.

### Fonctionnalités
- Consultation de la liste des films et du stock disponible.
- Réservation de places avec mise à jour en temps réel du stock.
- Gestion des erreurs (ID inconnu, stock insuffisant).
- Communication asynchrone via IPC.

## Structure du projet

- `protocole.h` : Définition des structures de messages.
- `client.c` : Interface utilisateur pour consulter les films et réserver.
- `serveur.c` : Gestion des stocks et des requêtes.

Le protocole utilise une file de messages système unique identifiée par la clé `12345`.

## Compilation

Utilisez le Makefile fourni :

```bash
make
```

## Nettoyage

Pour supprimer les exécutables et les fichiers objets :

```bash
make clean
```

## Utilisation

1. Lancez le serveur : `./serveur`
2. Lancez le client : `./client`

## Debugging

En cas de problème avec les files de messages, vous pouvez utiliser :

- `ipcs -q` : Pour lister les files actives.
- `ipcrm -q <id>` : Pour supprimer manuellement une file si nécessaire.

<!-- update 13 -->
<!-- update 14 -->
<!-- update 15 -->