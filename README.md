# C-socketProgramming-groupChat
## Cahier Des Charges

### 1. Objectif du projet
Développer une application de dialogue au clavier utilisant des sockets en C pour la communication entre un client et un serveur, via le protocole TCP.

### 2. Application choisie
**Dialogue au clavier** : Une application permettant à un utilisateur de communiquer avec tout les utilisateur connectés au meme serveur via des messages textuels échangés au clavier.

### 3. Type de serveur
**Serveur en parallèle** : Le serveur traitera les requêtes des clients de manière concurrente en utilisant le multithreading.

### 4. Mode de communication
**TCP (Transmission Control Protocol) - Orienté connexion** : 
- **Justification** : 
  - **Fiabilité** : TCP garantit la livraison des messages dans l'ordre et sans perte.
  - **Connexion persistante** : Maintenir une connexion ouverte permet de gérer efficacement la communication bidirectionnelle entre le client et le serveur.

### 5. Explication du choix du serveur en parallèle
Les requêtes des clients sont traitées en parallèle, chaque client étant géré dans un thread distinct après l'acceptation de sa connexion. Cette approche permet de gérer efficacement les opérations bloquantes comme `getline()`, `accept()`, et `recv()`, en assurant que chaque client est servi sans retarder les autres. Cela améliore la réactivité du serveur et permet de traiter plusieurs requêtes simultanément.

### 6. Langage de programmation
**C** : Le projet sera développé en langage C pour tirer parti de ses performances et de son contrôle bas-niveau sur les sockets.

### 7. Spécifications techniques
- **Système d'exploitation cible** : Linux (mais le code devrait être portable sur d'autres systèmes Unix-like avec des modifications mineures).
- **Fonctionnalités du client** :
  - Établir une connexion avec le serveur.
  - Envoyer des messages textuels au serveur.
  - Recevoir des réponses du serveur.
- **Fonctionnalités du serveur** :
  - Accepter les connexions entrantes des clients.
  - Créer un thread pour chqaue connection entrantes/acceptées .
  - Lire et traiter les messages des clients.
  - Diffuser les messages aux autres clients connectés.
  - Gérer les déconnexions de manière propre et sécurisée.
  
### 8. Documentation et tests
- **Documentation** : Fournir une documentation détaillée du code et des instructions pour la compilation et l'exécution de l'application.
- **Tests** : TODO

### 9. Difficultés rencontrées

Lors du développement de l'application, on a rencontrés plusieurs défis, notamment la gestion des descripteurs de fichiers à travers les processus forkés (fils).

## Problème : 
Chaque processus enfant hérite des descripteurs de fichiers du parent. Si un descripteur est modifié après le fork du premier enfant, les enfants suivants voient le descripteur mis à jour, mais pas le premier enfant, causant des incohérences. Par consequence on n'aura pas l'access aux sockets d'autre fils qui ont été forkés derniermenets.

## Approche :
- On etait obliger de repenser l'approche initiale plutôt que de forcer le partage du même descripteur entre tous les processus enfants.
- On a decidé d'utiliser des threads au lieu de processus pour un partage efficace des ressources.

## Solution :*
Utilisation du multithreading, permettant à tous les threads d'accéder aux mêmes ressources et résolvant ainsi les problèmes de synchronisation.


### 10. Instructions de compilation et d'exécution
Pour compiler et exécuter l'application, suivez les étapes ci-dessous :

1. **Compilation** :
   ```sh
    make client
    make serveur
    ``` 
1. **Exécution** :
- Pour démarrer le serveur :
    ``` 
    ./serveur
    ```
- Pour démarrer un client :
    ``` 
    ./client
    ```


### 11. Conclusion
Ce projet vise à développer une application robuste et fiable pour le dialogue au clavier via des sockets en C, en mettant l'accent sur la simplicité de l'architecture et la performance de l'application. Le serveur traite les requêtes des clients de manière concurrente en utilisant le multithreading, ce qui permet de gérer efficacement les opérations bloquantes et d'améliorer la réactivité. Des fonctions d'aide pour valider les erreurs et accélérer le processus de développement ont été utilisées ([fon.c](fon.c)), fournies par le professeur de TP réseaux Pascal Sicard.