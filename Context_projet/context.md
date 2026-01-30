# Contexte général

On considère une application client-serveur, comprenant un programme serveur et un programme client. Il n’y aura qu’une seule instance du programme serveur, en revanche il pourra y avoir k instances du programme client (un programme pour chaque client désirant se connecter au serveur).

Cette application permet de réserver des places pour un ensemble de spectacles.

Des clients émettent deux types de requêtes à destination du serveur :

- Requête de consultation : permettant de consulter le nombre de places restantes pour un spectacle donné.

- Requête de réservation : permettant de réserver n places pour un spectacle donné.
 

Le serveur de gestion de places de spectacles  est composé de deux parties :

- Partie Consultation : cette partie prend en compte les requêtes de consultation. Pour chacune d’elle, cette fonctionnalité renvoie le nombre de places disponibles pour le spectacle spécifié dans la requête.

- Partie Réservation : cette partie prend en compte les requêtes de réservation. Pour chacune d’elle, cette fonctionnalité effectue la réservation si cela est possible (suffisamment de places disponibles pour la date et le spectacle indiqué). Dans le cas où la réservation a pu être faite, un acquittement de réservation est renvoyé au client et sinon un message d’erreur.

Les informations concernant les spectacles sont stockées dans une table en mémoire centrale du côté du serveur, qui est accessible par les deux parties du serveur (parties Consultation et Réservation).

Une entrée de la table concerne un spectacle et donne les informations suivantes : Intitulé du spectacle, nombre de places restantes. Chaque spectacle est joué une seule fois.

