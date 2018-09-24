$ $XConsortium: dtcopy.msg /main/5 1996/11/19 18:29:23 rws $
$ *****************************************************************************  
$ **  
$ **  File:        dtcopy.msg  
$ **  
$ **  Project:     CDE File Manager  
$ **  
$ *****************************************************************************  
$quote "

$set 2
$ ****** Module: main_dtcopy.c ******  

3 "fichier"

4 "fichier(s)"

5 "sous-dossier"

6 "sous-dossier(s)"

7 "Erreur(s): aucune"

8 "Erreur(s): %d"

10 "Syntaxe: %s [options ...] source cible\n"

11 "Gestionnaire de fichiers - Déplacer un dossier"

12 "Gestionnaire de fichiers - Copier un répertoire"

13 "%s n'a(ont) pas été supprimé(s) suite à des erreurs."

14 "Opération terminée."

15 "Annulé(s) dans"
$ ** Message 20 - 32  
$ ** LongFileOpNames ... message number = op + 20.  

20 "traitement de"

21 "ouverture de"

22 "ouverture de"

23 "ouverture de"

24 "lecture de lien"

25 "suppression de "

26 "copie de"

27 "création du répertoire"

28 "copie de lien"

29 "création de lien"

30 "en cours"

31 "est une copie"

32 "est un lien"

33 "renommer"

$set 3        Redefine set#

$ ****** Module: copydialog.c ******  

1 "Reprendre"

2 "Suspendre"

3 "Déplacer:"

4 "Copier:"

5 "Dans dossier:"

7 "Cette action va annuler l'opération de copie. Tous les\n\
fichiers copiés resteront dans le dossier de destination."

10 "Cette action va annuler le déplacement. Tous les fichiers\n\
déplacés resteront dans le dossier de destination."

11 "Copier dossier - Avertissement"

12 "Déplacer dossier - Avertissement"

13 "Annuler copie"

14 "Continuer copie"

15 "Annuler déplacement"

16 "Continuer déplacement"

17 "Déplacé"

18 "Copié"

19 Impossible de déplacer le dossier sur lui-même\n\
%s\n\n

20 Impossible de copier le dossier sur lui-même\n\
%s\n\n


$set 4        Redefine set#

$ ****** Module: errordialog.c ******  

1 "Erreur(s) %s\n\
%s"

2 "Pause si erreur"

3 "Copie de dossier - Erreur"

4 "Déplacement de dossier - Erreur"

5 "Déplacement de dossier - Erreur"

6 "Copie d'objet - Erreur"

7 "Suppression d'objet (corbeille) - Erreur"

8 "Vous n'êtes pas autorisé à mettre l'objet\n\n\
    %s\n\n\
dans la corbeille.\n\n\
Pour vous accorder l'autorisation d'accéder en lecture \n\
à l'objet, utilisez l'option Modifier les droits d'accès\n\
du menu instantané de l'objet ou du menu Sélectionné(s).\n\n\n\
Remarque: Si l'objet est un dossier, vous devez \n\
également avoir accès en lecture à tous les objets\n\
qu'il contient pour pouvoir le mettre dans la corbeille."

9 Impossible d'ouvrir %s


$set 5        Redefine set#

$ ****** Module: overwrtdialog.c ******  

1 "Un fichier appelé\n\
%s\n\
existe déjà dans le dossier\n\
%s"

2 "Déplacement de dossier - Ecraser le fichier"

3 "Remplacer le fichier existant"

4 "Modifier le nom du fichier existant en:"

5 "Modifier le nom du nouvel objet en:"

6 "Ignorer ce fichier"

7 "Appliquer cette action aux conflits de noms"

8 "Action:"

9 "Copie de dossier - Remplacer le fichier"

10 "%s\n\
existe déjà: choisissez un autre nom."

11 "Remplacement de fichier - Erreur"

12 "%s\n\
ne peut pas être renommé en\n\
%s\n\
%s"

$set 6        Redefine set#

$ ****** Module: utils.c ******  
$ ** Button labels.  

1 "OK"

2 "Suspendre"

3 "Annuler"

4 "Aide"

5 "Copie de répertoire - Aide"

6 "Continuer"

7 "Annuler la copie"

8 "Annuler le déplacement"
