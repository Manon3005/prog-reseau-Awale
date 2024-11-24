# prog-reseau-Awale

Comment compiler et executer le projet ?

Côté server : se placer dans prog-reseau-Awale/server puis entrer 'make' dans le terminal. Entrer ./bin/server pour lancer le serveur

Côté client : se placer dans prog-reseau-Awale/client puis entrer 'make' dans le terminal. Entrer ./bin/client [adresse ip] [username] pour se connecter au server.

Une fois connecté.e :

Si vous êtes déja enregistré dans la base de données, vous avez simplement à entrer votre mot de passe.
Sinon vous allez devoir choisir un mot de passe et serez enregistré pour les prochaines connexions.

Une fois logué.e :

Un menu s'affiche et vous avez 10 options que nous allons vous détailler :

1 - '[1] Online players list' : Permet d'afficher la liste des joueurs connectés.

2 - '[2] Challenge a player' :  Permet de défier un joueur connecté à une partie d'Awale. Vous ne pouvez défier qu'un joueur à la fois. Si celui-ci accepte votre défi, il revient à vous de choisir si la partie est privée ou publique. Une partie privée ne peut être observée que par les amis des deux joueurs.

3 - '[3] Change your bio' : Vous permet de changer votre biographie. Celle-ci est visible par tous les joueurs même lorque vous êtes déconnectés.

4 - '[4] Read a player's bio' : Vous permet d'entrer un nom de joueur pour voir sa biographie. Vous pouvez continuer ainsi jusqu'à ce que vous entriez 'MENU' pour revenir au menu.

5 - '[5] See your games' archives' : Vous permet de voir l'historique de toutes vos parties avec votre adversaire, la date, et la vainqueur.

6 - '[6] Observe a game' : Vous permet d'entrer un nom de joueur pour observer sa partie. Vous pourrez quitter le mode observateur à tout moment. Attention, vous ne pouvez pas observer une partie privée si le joueur ne figure pas dans la liste de vos amis.

7 - '[7] See your friends' list' : Vous permet d'afficher votre liste d'amis.

8 - '[8] Add a friend' :  Vous permet d'ajouter un joueur à votre liste d'amis.

9 - '[9] Remove a friend' :  Vous permet d'enlever un joueur de votre liste d'amis.

10 - '[10] Current games list' : Vous permet d'afficher la liste de toutes les parties en cours, privées comme publiques.

Vous pouvez aussi à tout instant envoyer un message privé à un joueur en tapant '/chat [nom du destinataire] [message]'


Quelles sont les règles de l'Awalé ?

Le jeu se joue à deux joueur uniquement. Le plateau est divisé en 2 fois 6 cases. Chaque côté représente le camp d'un joueur.
Au début de la partie, 48 graines sont réparties équitablement dans les 12 cases, soit 4 graines par case.
Le jeu se joue chacun son tour. 

A son tour, le joueur choisit une case non vide de son camp et réparti les graines une par une dans le sens des aiguilles d'une montre entre les cases. S'il fait le tour complet, il saute la case d'où proviennent les graines.
Une fois la répartition terminée, si l'un des camps adverses dans lesquels il a disposé une graine en contient strictement 2 ou 3, il récupère les graines de celui le plus éloigné et remonte vers son camp : il récupère alors les camps qui ont 2 ou 3 graines jusqu'à tomber sur un camp adverse qui ne satisfait pas cette règle.

Les graines gagnées sont mises de côté et rapportent un point par graine.

Si votre adversaire n'a plus de graines, vous êtes obligé de choisir une case de sorte qu'à la fin de votre coup votre adversaire puisse jouer. Vous pouvez faire un coup qui récupèrerait théoriquement toutes les graines de l'adversaire mais aucune graine ne sera alors récupérée.

Si vous ne pouvez plus nourir votre adversaire ou qu'il n'est plus possible de capturer des graines ou que l'un des joureur a plus de 25 points, la partie s'arrête et le joueur avec le plus de points remporte la victoire. 