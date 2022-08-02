l3-projet_serveur

fait par : RIO YOHANN && DIAKHITE MOUSTAPHA GROUPE 2.1
Serveur streaming musical de fichier wav gérant le multi-client. Il se compose de 4 fichiers :

    audioserver.c : Fichier permettant de créer un serveur qui envoie en streaming une musique
    audioclient.c : Fichier permettant de demander une musique au serveur
    lecteur.c : Fichier apportant des fonctions permettant une gestion aisé de la lecture des fichiers wav
    socket.c : Fichier apportant des fonctions permettant une gestion aisé des sockets

Compilation

make

Démarrer le serveur audio

./audioserver

Une fois le serveur mis en marche, on peut commencer à lui envoyer des requêtes. Pour executer un client on
utilise cette commande suivi du nom des filtres possibles:

padsp ./audioclient nomdudomaine nomdufichier.wav FILTRE1 FILTRE2 FILTRE3
Par exemple :
padsp ./audioclient localhost test30s.wav ECHO MONO

Introduction:

Ce programme permet de créer un système client-seveur de streaming musical. Le client envoie le nom de
fichier avec des filtres, et le serveur lui envoie le son en retour. Le serveur ne sait jouer que des fichiers wav. 
le serveur ne peut gérer que les fichiers wav sur 8bit/16bit sur 1 ou 2 channels (les fichiers 32bit provoquant une erreur lors de leur ouverture).
Le serveur gère le multi-client en utilisant un fork dedié pour chaque client. Du coté client on peut demander
au serveur de jouer un fichier wav avec 0,1 ou plusieurs filtres. Les differents filtres disponiles sont MONO VOLUME et ECHO .

implementaion technique :

implementation des sockets:
La gestion des sockets se fait via socket.h, socket.c permettant une gestion plus aisée des sockets.

Protocole d'envoi:
Le protocole d'envoi est le suivant:
Le client envoie le nom du fichier avec un filtre (ou plusieurs filtres).
MONO =0b001, VOLUME = 0b010, ECHO = 0b100. Biensûr les trois filtres  peuvent être en meme temps. Le client envoie
donc ces donnees  au serveur.

Si le client ne reçoit aucune réponse du serveur, l'erreur connection timeout est soulevé.
Le serveur reçoit les données. Si le serveur n'a pas le fichier demandé, il enverra un message d'erreur 404. Si le
serveur est surchargé, il enverra le message d'erreur 503. Enfin si le filtre demandé n'est pas possible pour le
fichier demandé (par exemple on demande du mono pour un fichier de 1channel), alors le serveur enverra
l'erreur 405. Après cette vérification des erreurs, le serveur peut se lancer dans l'envoi des données. Le serveur
enverra tout le fichier jusqu'à atteindre sa fin. Il enverra une socket vide pour signaler la fin de transmission.

Implémentation du multi-client:
L'implémentation du multi-client demande qu'une partiee du programme s'occupe de la reception des requêtes
client tandis qu'une autre partiee s'occupe d'envoyer les données au client. Pour cela, nous utiliserons un
système de fork. À chaque nouvelle requête, nous créerons un fork. Le processus père s'occupera toujours de
recevoir les requêtes tandis que le processus fils enverra les données à au client. Lorsque le processus fils se
termine, il passe en état zombie et envoie un signal à son processus père. Pour pouvoir éliminer ce zombie, le
père doit récupérer le signal de son fils mort. Or le père a un énorme problème pour faire cela. Il est bloqué à
attendre une socket client dans la foncction recvfrom(). Pour que le père puisse récupérer le signal de son fils, il
doit pouvoir sortir de la fonction recvfrom(). recvfrom() doit donc être non bloquant. Pour ce faire, nous
utilisons la fonction select() permettant de mettre un timeout au fonction d'entrée sortie. Un timeout juste
semble être d'une seconde. Ansi chaque seconde, le père vérifie si un de ses fils est mort.
Le serveur peut également supporter qu'un certain nombre de client à la fois. Ici ce nombre est fixé à 3 par
MAX_PID .

Implémentation des filtres:
Les filtres ont été implémenté dans lecteur.h et lecteur.c

Filtre Mono:
Le filtre mono permet de passer d'un fichier ayant 2 channel à un fichier n'ayant plus que 1 channel.
L'implémentation est la suivante :
Un coup on joue un échantillon gauche, un coup on joue l'échantillon droite. Faire la moyenne des 2
échantillons produit un bruit de fond désagréable.
Le filtre mono est uniquement disponible pour les fichiers contenant 2 channels. Il est impossible de le faire
pour les fichiers contenant seulement 1 channel.

Filtre Volume:
Le filtre volume diminue le son. Ainsi dès qu'on appelle la fonction, le volume sera réduitde 20%.
Le filtre volume ne peut  être disponible pour les fichiers 8bit, car l'augmentation d'un signal produit des
distorsions trop forte.

Filtre Echo:
Le filtre echo produit un echo. Pour cela on utilise un buffer (bufecho) qui sauvegarde les précédents
échantillons et on les additionne à l'échantillon actuel.
Ce filtre est disponible uniquement pour les fichiers 16bit.