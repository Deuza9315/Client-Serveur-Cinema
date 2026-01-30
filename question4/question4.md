# Question 4

Nous désirons augmenter le nombre de requêtes simultanément par le serveur. Pour cela, plutôt que d'utiliser deux threads (un par type de requêtes) le serveur devra créer un nouveau thread pour traiter chaque nouvelle requête reçue (que ça soit une requête de consultation ou de réservation) et envoyer la réponse au client.

D'autre part, on veut éviter tout problème de cohérence au niveau de la base stockant les réservations lors de l'exécution de multiple requêtes simultanées. Pour cela, nous voulons effectuer un contrôle d'accès en exclusion mutuelle à la base des réservations entre les threads traitant les requêtes des clients.

Modifiez le code du serveur pour créer dynamiquement un thread pour toute nouvelle requête (en considérant que les programmes client et serveur sont sur une même machine).
Modifiez le code su serveur afin de mettre en place un verrou permettant de garantir un accès en exclusion mutuelle à la base des réservations.
Prenez soin de documenter votre code.