$ *************************************<+>*************************************   
$ *****************************************************************************   
$ **   
$ **  File:        dthelp.msg   
$ **   
$ **  Project:     CDE 1.0 Help   
$ **   
$ **  Description:   
$ **  -----------   
$ **  This file is the source for the message catalog for DtHelp    
$ **   
$ **   
$ *****************************************************************************   
$ **   
$ **     (c) Copyright 1990 Hewlett-Packard Company   
$ **         All Rights reserved   
$ **   
$ **   
$ **     There are two types of messages in this file:   
$ **   
$ **     1) Messages that appear in dialogs or are displayed to the user.   
$ **        These messages are the default and they should ALL BE LOCALIZED.   
$ **        Note that these messages do NOT have any identification.   
$ **   
$ **     2) Messages that should not be localized.   
$ **        These messages are marked with DO NOT TRANSLATE THESE MESSAGES.   
$ **   
$ *****************************************************************************   
$ **   
$ **   
$ **  ------------------------- MODIFICATION RECORD --------------------------   
$ *   Major Mods, 3/26/92 in preperation for the NLS package release on the    
$ *               27th.  (Mike Wilson).   
$ *   
$ *   Major Mods, 6/10/94 in prep for Cde Message Catalog Freeze   
$ *   
$ **  ----------------------- MODIFICATION RECORD END ------------------------   
$ *****************************************************************************   
$ *************************************<+>*************************************   
$quote "

$set 2
$ *****  Module: HelpDialog.c  and a few from HelpQuickD.c *****   
$    
$ These Messages appear in the pulldown menus for Cde Help General Help   
$ dialog widget.   
$    
$ They come in pairs where the first is the menu name (e.g. 1 Print...) and the   
$ second is the mnemonic (e.g. 2 P).     
$    
$ Messages 1 through 6 are the pulldown menus for the File menu.   

1 "Imprimer..."

2 "I"

3 "Fermer"

4 "F"

5 "Nouvelle fenêtre..."

6 "N"
$ Messages 7 through 8 are the pulldown menus for the Edit menu.   

7 "Copier"

8 "C "
$ Messages 9 through 10 are the pulldown menus for the Search menu.   

9 "Index..."

10 "I"
$ Messages 11 through 16 are the pulldown menus for the Navigate menu.   

11 "Sujet initial"

12 "S"

13 "Revenir"

14 "R"

15 "Historique..."

16 "H"
$ Messages 17 through 28 are the pulldown menus for the Help menu.   

17 "Généralités - Aide sur l'aide"

18 "G"

19 "Table des matières"

20 "T"

21 "Tâches"

22 "T"

23 "Référence"

24 "R"

25 "Souris et clavier"

26 "S"

27 "A propos de l'aide"

28 "A"
$    
$ Messages 29 through 36 are the menu labels and mnemonics for for the    
$ Help Dialog.   

29 "Fichier"

30 "F"

31 "Rechercher"

32 "R"

33 "Naviguer"

34 "N"

35 "Aide"

36 "A"

37 "Editer"

38 "E"
$    
$    
$ >>>>> Skip #39 through #49   
$    
$ Messages 50 through 55 are used to display formatting errors within the   
$ help dialog and quick help dialog widgets.   
$    

50 "Mise en forme des données de la chaîne impossible."

51 "Mise en forme des données dynamiques de la chaîne impossible."

52 "Mise en forme de la page de manuel impossible: elle n'existe pas ou elle est altérée."

53 "Mise en forme des données du fichier texte impossible: le fichier n'existe pas ou il est altéré."

54 "Mise en forme du sujet d'aide impossible."

55 "L'ID d'emplacement n'existe pas:"
$    
$ Messages 56 and 67 are displayed when a hypertext link type is selected but   
$ not supported within the current application.   
$    

56 "Cette application n'accepte pas le lien d'hypertexte sélectionné."

57 "Cette application n'accepte pas les liens avec les pages de manuel."
$    
$ Messages 58 through 62  are used when bad or null help volumes or    
$ locationId's are used.   
$    

58 "L'aide en ligne demandée n'est pas installée ou n'est pas dans le chemin d'aide adéquat. Pour plus de détails sur l'installation de l'aide en ligne, reportez-vous à la documentation sur le produit.\n"

59 "Aucun ID d'emplacement spécifié."

60 "Aucun volume d'aide spécifié."

61 "Volume d'aide:"

62 "ID d'emplacement:"
$    
$ >>>>> Skip #63 through #69   
$    
$    
$ Message 70 is used when a null value for DtNtopicTitle is used.   
$    

70 "Ce nom de sujet d'aide n'existe pas."
$    
$ New messages added after early msg cat freeze; not in order   

80 "Le type d'aide indiqué est incorrect."

$set 3        Redefine set#

$ ****  Module: History.c ****   
$ These messages are used as labels for the History Dialog.   

1 "Utilitaire d'affichage de l'historique - Aide"

2 "Volumes d'aide consultés:"

3 "Fermer"

4 "Aide"

5 "Fichiers d'aide"

6 "Pages de manuel"

7 "Messages d'aide"

8 "Sujets d'aide consultés:"

9 "Fichiers d'aide consultés:"

10 "Pages de manuel consultées:"

11 "Messages d'aide consultés:"

$set 4        Redefine set#

$ *****  Module: Print.c  *****   
$ These messages are used as labels for the Print Dialog.   
$ 1: dialog title   

1 "Impression - Aide"
$ 10-12: standard printer dialog labels   

10 "Imprimante:"

11 "Exemplaires:"

12 "Titre de la banderole:"
$ 20-25: paper size label and sizes   

20 "Format du papier:"

21 "Letter (215,9 x 279,4 mm)"

22 "Legal (215,9 x 355,6 mm)"

23 "Executive (181,1 x 266,7 mm)"

24 "A4 (210 x 297 mm)"

25 "B5 (182 x 257 mm)"
$ 30-33: help volume-specific printing options   

30 "Sujets à imprimer"

31 "Sujet en cours"

32 "Sujets en cours et sous-sujets"

33 "Table des matières et index"

34 "Tous les sujets du volume"
$ 40-42: buttons at bottom of dialog   

40 "Imprimer"

41 "Annulation"

42 "Aide"
$ These messages are used to describe the kind of content to be printed   
$ 50 "Help Volume: "   

50 "Volume d'aide: "

51 "Message d'aide"

52 "Fichier d'aide"
$ 53 "Manual Page: "   

53 "Page de manuel: "

54 "Message d'erreur"
$ Don't translate message 60 unless it is not an empty string   
$ This is the default value that goes in the Printer field   
$ 60 " "   

60 ""
$ Don't translate message 61   
$ This is the default value that goes in the Copies field   

61 "1"

$set 5        Redefine set#

$ *****  Module: GlobSearch.c  *****   
$ Message 1 is the label used to label the dialog   

1 "Recherche dans l'index - Aide"

2 "Rechercher"

3 "En cours "
$ DO NOT TRANSLATE MESSAGE 4   
$ " (%s)"   

4 "(%s)"

5 "Tous les volumes"

6 "Sélection"

7 "Sélectionner les volumes..."

8 "Afficher"

9 "Index complet"

10 "Entrées comportant:"

11 "Aller à"

12 "Lancer la recherche"

13 "Continuer la recherche"

14 "Arrêter la recherche"

15 "Fermer"

16 "Aide"

17 "Recherche en cours... Volumes détectés: %d"

18 "Recherche en cours... Volumes restants: %d"

19 "Aucune entrée d'index détectée."

20 "Index complet"

21 "Entrées comportant \"%s\""

22 "Aucun volume sélectionné"

23 "Sélectionnez les options de recherche ci-dessus."
$ Do NOT translate messages 25-31   
$ " "   

25 ""

26 "+"

27 "-"
$ "  +"   

28 "+"
$ "  -"   

29 "-"
$ 30 "       "   

30 ""
$ 31 "         "   

31 ""
$ DO translate messages 32 and above   
$ kind of topic messages   

32 "Message d'aide"

33 "Fichier d'aide"

34 "Page de manuel"

35 "Format inconnu"
$ specialized title of Search Volume Selection dialog   

40 "Sélection d'un volume de recherche - Aide"
$ additional messages (late addition caused this ordering)   

50 "Aucune entrée d'index détectée. Lancez la recherche sur tous les volumes."

51 "Aucune entrée d'index détectée. Lancez la recherche sur un autre mot."

60 "("

61 ")"

$set 6        Redefine set#

$ *****  Module: HelpAccess.c  *****   
$ These messages are used for the Help on Help dialog in Cde Help widgets.   

1 "Aide sur l'aide"

2 "Imprimer..."

3 "Fermer"

4 "Aide sur l'aide non disponible pour cette application."

5 "Revenir"

6 "Aide ..."

$set 7        Redefine set#

$ *****  Module: Main.c & Util.c & (dthelpview CDE 1.0  *****   
$ These messages are used by the dthelpview program.  They need to be    
$ localized for dthelpview, however, they are not directly used by libDtHelp   
$ Messages 1 through 7 are command line usage response messages.   
$ Do NOT translate the option name (e.g. -helpVolume).  DO translate   
$ the argument descriptor name (e.g. <Help Volume File>)   

1 "Syntaxe: %s...\n"

2 "\t-helpVolume <Fichier volume aide>\n"

3 "\t-locationId  <ID>\n"

5 "\t-file  <Fichier texte ASCII>\n"

6 "\t-man\n"

7 "\t-manPage <Page de manuel Unix>\n\n\n"
$ man page dialog   

11 "Page de manuel"

12 "Page de manuel:"

13 "Afficher page de manuel"
$ button labels   

16 "Quitter"

17 "Fermer"

18 "Revenir"

19 "Imprimer..."
$ title of helpview window   

20 "Utilitaire d'affichage de l'aide"

$set 8        Redefine set#

$ *****  Module: HelpUtil.c  *****   
$ Message 1 is the label used in the definition link dialog box    
$ (e.g. Quick help close button)   

2 "Fermer"

3 "Erreur interne: Saisie de la souris impossible.\n\
Echec de DtHelpReturnSelectedWidget.\n"

4 "Erreur interne: Saisie du clavier impossible.\n\
Echec de DtHelpReturnSelectedWidget().\n"

5 "Erreur - Aide"
$ Messages 10 and up were added after message catalogs were taken   
$ for translation.  They are not in any order or chosen location.   

10 "Exécuter"

11 "Ne pas exécuter"

12 "Le volume d'aide a tenté d'exécuter un alias de commande.\n\
Or, celui-ci ('%s') n'est pas défini."

13 "Le volume d'aide a tenté d'exécuter une commande. Or, pour des raisons\n\
de sécurité, la fonction d'exécution automatique des commandes est \n\
désactivée. La commande est la suivante : %s"

14 " Le volume d'aide a tenté d'exécuter une commande en tant qu'utilisateur root\n\
mais le répertoire personnel de \"%s\"  ($HOME) ne correspond pas au répertoire \n\
personnel de l'utilisateur root. Ceci peut conduire à l'exécution de commandes \n\
non anticipées.\n\
\n\
La commande est la suivante :  \"%s\"\n\
\n\
Remarque : à l'avenir, pour éviter cela :\n\
 exécutez \"su - root\"  plutôt que \"su root\".\n"
$ End Message set 8   

$set 9        Redefine set#

$ *****  Module: PathArea.c  *****   
$ These messages are used in the general Help Dialog Widget.   
$ Message 1 is the label used to label Topic Tree Area     

1 "Volume: "
$ Messages 2 though 5 are used in the Button Box area of our general    
$ help dialog.   

2 "Revenir"

3 "Historique..."

4 "Index..."

5 "Niveau supérieur"

$set 10        Redefine set#

$ *****  Module: VolSelect.c  *****   
$ Message 1 is the default label used to label the volume selection dialog.   

1 "Sélection d'un volume - Aide"

2 "Volumes d'aide"

3 "Sélectionnez un ou plusieurs volumes d'aide."

4 "Fermer"

5 "Aide"

$set 11        Redefine set#

$ *****  Module: HelpQuickD.c  *****   
$ These messages are used as labels for the quick help dialog.   

1 "Fermer"

2 "A suivre..."

3 "Revenir"

4 "Imprimer..."

5 "Aide..."

$set 99        Redefine set#

$    DO NOT TRANSLATE THESE MESSAGES   
$    DO NOT CHANGE THESE MESSAGES   
$    DO NOT LOCALIZE THESE MESSAGES   
$    These messages are used for the version information.   

1 "@(#)version_goes_here"

2 "\n\
@(#)Message catalog source $XConsortium: DtHelp.msg /main/5 1996/10/19 15:25:53 rws $"
