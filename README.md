# C-socketProgramming-groupChat
## Cahier Des Charges

### 1. Objectif du projet
Développer une application de dialogue au clavier utilisant des sockets en C pour la communication entre un client et un serveur.

### 2. Application choisie
**Dialogue au clavier** : Une application permettant à un utilisateur de communiquer avec un serveur via des messages textuels échangés au clavier.

### 3. Type de serveur
**Serveur itératif** : Le serveur traitera les requêtes des clients de manière séquentielle, une par une.

### 4. Mode de communication
**TCP (Transmission Control Protocol) - Orienté connexion** : 
- **Justification** : 
  - **Fiabilité** : TCP garantit la livraison des messages dans l'ordre et sans perte.
  - **Connexion persistante** : Maintenir une connexion ouverte permet de gérer efficacement la communication bidirectionnelle entre le client et le serveur.

### 5. Explication du choix du serveur itératif
Les requêtes des clients sont traitées séquentiellement, ce qui simplifie la gestion des connexions et évite les complications liées à la concurrence et au blocage. Cette approche est adéquate lorsque le traitement des messages est rapide, garantissant ainsi une latence minimale pour chaque requête.

### 6. Langage de programmation
**C** : Le projet sera développé en langage C pour tirer parti de ses performances et de son contrôle bas-niveau sur les sockets.

### 7. Spécifications techniques
- **Système d'exploitation cible** : Linux (mais le code devrait être portable sur d'autres systèmes Unix-like avec des modifications mineures).
- **Fonctionnalités du client** :
  - Établir une connexion avec le serveur.
  - Envoyer des messages textuels au serveur.
  - Recevoir des réponses du serveur.
  - Gérer les interruptions de connexion et tenter de se reconnecter.
- **Fonctionnalités du serveur** :
  - Accepter les connexions entrantes des clients.
  - Lire et traiter les messages des clients.
  - Envoyer des réponses appropriées aux clients.
  - Gérer les déconnexions de manière propre et sécurisée.
  
### 8. Contraintes et considérations
- **Performance** : Assurer une latence minimale dans le traitement des messages.
- **Sécurité** : Mettre en place des mesures de sécurité pour éviter les attaques courantes sur les réseaux, telles que les attaques par déni de service (DoS).
- **Scalabilité** : Prévoir la possibilité d'améliorer l'architecture du serveur pour passer à un modèle multithreadé ou multiprocessus si les besoins en capacité augmentent.

### 9. Documentation et tests
- **Documentation** : Fournir une documentation détaillée du code et des instructions pour la compilation et l'exécution de l'application.
- **Tests** : Mettre en place une suite de tests pour vérifier la robustesse et la fiabilité de l'application, incluant des tests unitaires et des tests de stress.

### 10. Délais et livrables
- **Phase 1** : Conception et documentation (2 semaines)
- **Phase 2** : Développement du client et du serveur (4 semaines)
- **Phase 3** : Tests et débogage (2 semaines)
- **Phase 4** : Livraison et déploiement (1 semaine)

### 11. Conclusion
Ce projet vise à développer une application robuste et fiable pour le dialogue au clavier via des sockets en C, en mettant l'accent sur la simplicité de l'architecture et la performance de l'application. Des fonctions d'aide pour valider les erreurs et accélérer le processus de développement ont été utilisées ([fon.c](fon.c)), fournies par le prof de TP reseaux Pascal Sicard.