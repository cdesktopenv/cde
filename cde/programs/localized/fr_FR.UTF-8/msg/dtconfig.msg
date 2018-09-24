$ $XConsortium: dtconfig.msg /main/6 1996/10/19 15:26:26 rws $
$ ***************************************************************************** 
$ ** 
$ **  File:        dtconfig.msg 
$ ** 
$ **  Project:     CDE Session Management (dtconfig) 
$ ** 
$ **  Description: 
$ **  ----------- 
$ **  This file is the source for the message catalog for dtconfig 
$ ** 
$ **  Message set 10: IBM 
$ **  Message set 20: Sun 
$ ** 
$ ***************************************************************************** 
$ ***************************************************************************** 
$ **  Message set 10: For IBM platforms 
$ ***************************************************************************** 

$quote "

$set 10


1 "ne peut être utilisé que par le super-utilisateur.\n"
$  

2 "L'environnement du Bureau AIXwindows est maintenant défini comme interface \n\
utilisateur par défaut. Cette interface apparaît lors de la connexion de tous\n\
les utilisateurs du système. Pour que cette modification soit prise en compte,\n\
vous devez arrêter, puis redémarrer le système.\n"
$  

3 "La ligne de commande est maintenant définie comme interface \n\
utilisateur par défaut. Cette interface apparaît lors de la \n\
connexion de tous les utilisateurs du système. Pour que cette \n\
modification soit prise en compte, vous devez arrêter, puis \n\
redémarrer le système.\n"

4 "L'environnement du Bureau AIXwindows est déjà défini comme interface \n\
utilisateur par défaut.\n"
$  

5 "La ligne de commande est déjà définie comme interface utilisateur par défaut.\n"
$  

6 "Syntaxe: %s\n\
  -e (démarrage automatique de dtlogin activé)\n\
  -d (démarrage automatique de dtlogin désactivé)\n\
  -kill (arrêt de dtlogin)\n\
  -reset (réinitialisation de dtlogin, relecture des fichiers de configuration)\n"
$  

7 "Trop d'arguments \n"
$  

8 "Un argument est requis\n"
$  

9 "Impossible de désactiver xdm ; consultez les pages man de xdm\n\
et faites une nouvelle tentative.\n\
 "
$  

10 "Dtlogin n'est pas en cours d'exécution.\n"
$ ***************************************************************************** 
$ **  Message set 20: For Sun Platforms 
$ ***************************************************************************** 

$set 20

$  

1 "Utilitaide de configuration du Bureau CDE"
$  

2 "%s -d     (démarrage automatique désactivé)"

3 "%s -e     (démarrage automatique activé)"

4 "%s -kill  (arrêt de dtlogin)"

5 "%s -reset (réinitialisation de dtlogin)"

6 "%s -p     (mise à jour de l'action d'impression)"
$  
$ In above messages 2 through 6, only translate words between 
$ left "(" and right "). 
$  

7 "%s: Ce script doit être exécuté en tant que root."

8 "Syntaxe: Trop d'arguments"

9 "Syntaxe: Un argument est requis"

10 "Echec de l'option."

11 "Impossible d'exécuter %s."

12 "Echec de la désactivation."

13 "Impossible de supprimer %s."

14 "terminé"

15 "Le démarrage automatique du Bureau est désactivé."

16 "Echec de la désactivation."

17 "%s est introuvable."

18 "Impossible de créer %s."

19 "Impossible de créer %s."

20 "Le démarrage automatique du Bureau est activé."

21 "dtlogin est arrêté (commande kill)."

22 "Les ressources de configuration de dtlogin ont été rechargées."

23 "La mise à jour de l'action d'impression est terminée."

24 "Syntaxe:"
$ 
25 " %s -inetd    (inetd.conf /usr/dt daemons)"
26 " %s -inetd.ow (inetd.conf /usr/openwin daemons)"
$ 
$	In above messages 25 and 26, only translate words between
$	left "(" and right ").
$ 
27 "A la prochaine initialisation, les éléments suivants seront exécutés à partir de /usr/dt/bin"
$ 
28 "    rpc.ttdbserverd         (ToolTalk)"
29 "    rpc.cmsd                (Gestionnaire d'agenda)"
30 "    dtspcd                  (Contrôle de sous procédure)"
$ 
$  Keep column allignment on messages 28 thru 30.  
$  Only translate what's beteen ( )'s
$ 
31 " A la prochaine initialisation, les éléments suivants seront exécutés à partir de /usr/openwin/bin"
$ 
32 "    rpc.ttdbserverd         (ToolTalk)"
33 "    rpc.cmsd                (Gestionnaire d'agenda)"
$ 
$  Keep column allignment on messages 32 thru 33.  
$  Only translate what's beteen ( )'s
$ 
