$ $TOG: dtcreate.msg /main/4 1999/09/17 17:23:37 mgreess $
$ %Z%%M%        %I%  %W% %G% %U%  
$   
$   COMPONENT_NAME: dtcreate  
$   
$   FUNCTIONS: none  
$   
$   ORIGINS: 27  
$   
$   
$   (C) COPYRIGHT International Business Machines Corp. 1993  
$   All Rights Reserved  
$   Licensed Materials - Property of IBM  
$   US Government Users Restricted Rights - Use, duplication or  
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.  
$   
$ **   Description:  
$ **   ------------  
$ **   This is the source message catalog file for dtcreate.  
$quote "

$set 2
$ *****************************************************************************  
$ **  
$ ** Message set for menu options on main window  
$ **  
$ *****************************************************************************  

10 "Fichier"

11 "F"

15 "Ouvrir..."

16 "O"

20 "Nouveau"

21 "N"

25 "Sauvegarder"

26 "S"

30 "Quitter"

31 "Q"

35 "Options"

36 "O"

40 "Affich. fonctions étendues"

41 "A"

42 "Afficher icônes"

43 "i"

44 "Couleur"

46 "C"

47 "Monochrome"

48 "M"

49 "Affich. icônes monochromes"

50 "m"

53 "Aide"

54 "A"

60 "Généralités"

61 "G"

65 "Table des matières"

66 "T"

70 "Tâches"

71 "c"

75 "Référence"

76 "R"

80 "Aide sur l'aide"

81 "i"

85 "A propos de l'application"

86 "p"

$set 3        Redefine set#

$ *****************************************************************************  
$ **  
$ ** Message set for  main Create Action window  
$ **  
$ *****************************************************************************  

10 "Créer une action"

12 "Nom (libellé de l'icône):"

13 "Icônes d'action:"

20 "Commande exécutée lors du double-clic:"

21 "Commande exécutée lors du double-clic:"

22 "Commande exécutée à l'ouverture de l'action:"

30 "Aide sur l'icône:"

32 "Type de fenêtre:"

34 "Graphique (X-Window)"

36 "Terminal (fermeture auto.)"

38 "Terminal (fermeture manuelle)"

40 "Pas de sortie"

50 "Avancée"

52 "De base"

$set 4        Redefine set#

$ *****************************************************************************  
$ **  
$ ** Message set for  main Create Action window advanced function area  
$ **  
$ *****************************************************************************  

10 "En option"

11 "A l'ouverture de l'action, demander aux utilisateurs:"

12 "Types de données utilisant l'action:"

13 "Ajouter..."

14 "Supprimer"

15 "Editer..."

16 "Types de données pouvant être posés:"

17 "Tous"

18 "Liste ci-dessus seulement"

$set 5        Redefine set#

$ *****************************************************************************  
$ **  
$ ** Error and Information message set for  main Create Action window  
$ **  
$ *****************************************************************************  

10 "Le nom de l'action est manquant: indiquez-le\n\
dans la zone Nom (libellé de l'icône)."
$   

15 "La commande à exécuter est manquante:\n\
indiquez-la dans la zone Commande\n\
exécutée à l'ouverture de l'action."
$   

20 "La liste Types de données utilisant l'action est\n\
vide. Or, vous avez sélectionné Liste ci-dessus\n\
seulement dans la zone Types de données pouvant être\n\
posés. Modifiez votre sélection en Tous types de données\n\
ou ajoutez au moins un type de données à la liste."
$   

25 "Une action et un fichier de définition sont déjà\n\
associés à ce nom d'action.\n\
Vous pouvez annuler la sauvegarde et modifier le nom\n\
de l'action ou remplacer le fichier de définition\n\
existant. Confirmez-vous le remplacement ?"
$   

30 "La zone Commande exécutée à l'ouverture de l'action\n\
ne comporte aucun nom de variable. Indiquez-en un\n\
correspondant au libellé spécifié dans la zone\n\
A l'ouverture de l'action, demander aux utilisateurs:.\n\
Les noms admis sont: $*, $1, $2, ..., $9."
$   

35 "Les modifications n'ont pas été sauvegardées.\n\
Voulez-vous effectuer la sauvegarde avant de sortir ?"
$   

40 "Les modifications n'ont pas été sauvegardées.\n\
Voulez-vous sauvegarder cette action avant\n\
d'en créer une autre ?"
$   

45 "Les modifications n'ont pas été sauvegardées.\n\
Voulez-vous effectuer la sauvegarde avant de\n\
charger un autre fichier de définition d'action ?"
$   

50 "Le nom d'action contient un ou plusieurs\n\
caractères incorrects.\n\
Les caractères suivants ne sont pas autorisés:\n\
  / \\ \" ' ;  (  ) ~ ! $ { } < > # @ | &"
$   

55 "Impossible de créer le fichier suivant:"
$   

60 "Impossible d'ouvrir le fichier suivant:"
$   

65 "Le fichier suivant n'a pas pu être chargé car il n'a pas\n\
été crée à l'aide de l'utilitaire de création d'action."
$   

66 "Le fichier n'a pas pu être chargé car il n'a pas été modifié à l'aide\n\
de l'utilitaire de création d'action. Il est possible que cette opération\n\
ait été effectuée par l'intermédiaire d'un éditeur de texte, par exemple."
$   

67 "L'utilitaire de création d'action ne permet de\n\
modifier que les fichiers créés par son intermédiaire."
$   

68 "Impossible de charger le fichier suivant:"
$   
$  The following messages are all part of the same message dialog  

70 "La nouvelle action"

71 "a été placée dans votre dossier personnel."

73 "Le fichier de définition d'action créé s'appelle:"
$   

80 "Impossible de trouver toutes les dimensions de\n\
l'icône sélectionnée."
$   

85 "Impossible de lancer l'éditeur d'icônes."
$   

90 "Impossible d'analyser le fichier suivant:"
$   

95 "Le fichier d'icône suivant est introuvable\n\
dans le chemin de recherche."
$   

98 "L'icône sélectionnée ne figure pas dans le chemin de\n\
recherche. Vous devez la placer dans un répertoire figurant\n\
dans ce chemin, afin qu'elle s'affiche correctement dans\n\
l'environnement de l'espace de travail.\n\
Pour utiliser l'icône, sélectionnez OK.\n\
Pour choisir une icône différente, sélectionnez Annuler."
$   
$ ONLY TRANSLATE  "Usage"  and  "filename"  

110 "Syntaxe: dtcreate [<nom_fichier>] [-?]"
$   

120 "Vous n'avez pas sélectionné de type de données\n\
dans la liste Types de données utilisant l'action."

125 "Sélectionnez le type de données à supprimer."

130 "Sélectionnez le type de données à éditer."
$   

150 "Le travail en cours n'a pas été sauvegardé."

152 "Ne pas sauvegarder"

154 "Annuler fermeture"
$   

160 "Impossible de sauvegarder la session dans le fichier:"

162 "Impossible de restaurer le fichier à partir du fichier:"
$   
$ TRANSLATORS: DO NOT TRANSLATE the word "HOME" in the following message  

170 "La variable d'environnement HOME n'est pas définie.\n\
Cette variable doit être définie pour que l'action\n\
puisse être sauvegardée."

$set 6        Redefine set#

$ *****************************************************************************  
$ **  
$ ** General message set for dialogs  
$ **  
$ *****************************************************************************  

10 "OK"

11 "Appliquer"

12 "Annuler"

13 "Aide"

14 "Effacer"

18 "Rechercher"

19 "Rech. ensemble"

20 "Editer une icône..."

31 "Créer une action - Erreur"

41 "Créer une action - Confirmation"

46 "Créer une action - Informations"

51 "Créer une action - Question"

$set 7        Redefine set#

$ *****************************************************************************  
$ **  
$ ** Message set for Add Datatype and Edit Datatype dialogs  
$ **  
$ *****************************************************************************  

10 "Ajouter un type de données"

11 "Editer un type de données"

14 "Nom de la famille du type de données:"

15 "Caractéristiques:"

16 "Editer..."

17 "Aide sur l'icône de type de données:"

18 "Icônes du type de données:"

21 "Commandes"

22 "Commande d'ouverture du type de données:"

23 "Commande d'impression du type de données:"

30 "Ouvrir"

31 "Imprimer"

$set 8        Redefine set#

$ *****************************************************************************  
$ **  
$ ** Error and Information message set for  
$ **    Add Datatype and Edit Datatype dialogs  
$ **  
$ *****************************************************************************  

10 "Le nom de la famille du type de données est manquant.\n\
Indiquez-le dans la zone correspondante."
$   

15 "Les caractéristiques d'identification sont manquantes:\n\
vous devez les indiquer pour que le type de données\n\
puisse être créé. Pour ce faire, sélectionnez le bouton\n\
Editer en regard de la liste Caractéristiques."
$   

20 "La zone Nom de la famille de type de données\n\
contient un ou plusieurs caractères incorrects.\n\
Les caractères suivants ne sont pas autorisés:\n\
  / \\ \" ' ;  (  ) ~ ! $ { } < > # @ | &"
$   

25 "Ce nom est déjà utilisé pour une famille de type de\n\
données. Modifiez-le dans la zone Nom de la famille\n\
du type de données."

$set 9        Redefine set#

$ *****************************************************************************  
$ **  
$ ** Message set for  Identifying Characteristics dialog  
$ **  
$ *****************************************************************************  

10 "Caractéristiques"

11 "Tout inclure"

12 "Fichiers"

13 "Dossiers"

14 "correspondant aux critères suivants:"

20 "Modèle de nom:"

21 "Modèle d'autorisations:"

22 "Accessible en lecture"

24 "Acessible en écriture"

25 "Exécutable"

27 "ET"

28 "Contenu"

29 "Modèle:"

30 "Type de données:"

31 "Chaîne"

32 "Octets (hex)"

33 "Courtes (décimales)"

34 "Longues (décimales)"

35 "Octet de départ:"

40 "Lecture:"

41 "Ecriture:"

42 "Exécution:"

43 "En fonction"

44 "Hors fonction"

45 "Indifféremment"

$set 10        Redefine set#

$ *****************************************************************************  
$ **  
$ ** Error and Information message set for Identifying Characteristics dialog  
$ **  
$ *****************************************************************************  

10 "Le modèle de nom est manquant.\n\
Si vous sélectionnez Modèle de nom,\n\
vous devez compléter cette zone."
$   

15 "Les autorisations sont manquantes.\n\
Si vous sélectionnez Modèle d'autorisations,\n\
vous devez sélectionner une ou plusieurs\n\
options sous Modèle d'autorisations."
$   

20 "Le modèle de contenu est manquant. Si vous sélectionnez\n\
Contenu comme caractéristique, la zone Modèle\n\
figurant sous cette option doit être complétée."
$   

25 "La valeur d'octet de début est manquante.\n\
Si Contenu est sélectionné, un nombre\n\
doit être indiqué dans la zone Octet de début."
$   

30 "Vous n'avez sélectionné aucune caractéristique.\n\
Vous devez sélectionner Modèle de nom, Modèle \n\
d'autorisations ou Contenu."
$   

45 "Le modèle de nom contient un ou plusieurs\n\
caractères incorrects.\n\
Les caractères suivants ne sont pas autorisés:\n\
  / \\ \" ' ;  (  ) ~ ! $ { } < > # @ | &"
$   

46 "Le Modèle de contenu n'est pas compatible\n\
avec le format de type de contenu sélectionné.\n\
Corrigez le format ou modifiez le type de sorte\n\
qu'il corresponde au modèle de contenu."
$   

47 "La zone Octet de début contient des caractères incorrects.\n\
Seuls les nombres décimaux sont admis."
$   

48 "Si vous utilisez le modèle de contenu, les performances\n\
du Gestionnaire de fichiers risquent d'être ralenties.\n\
En effet, celui-ci recherche le contenu de tous les\n\
fichiers dans un dossier avant d'ouvrir ce dernier.\n\
L'ouverture des dossiers est donc plus lente. \n\
Il est conseillé d'utiliser uniquement les modèles\n\
de nom et d'autorisations."

$set 11        Redefine set#

$ *****************************************************************************  
$ **  
$ ** Message set for  Icon Selection Dialog  
$ **  
$ *****************************************************************************  

10 "Répertoires"

11 "Fichiers d'icônes"

12 "Entrez un nom de fichier d'icône"

20 "Chargement en cours des icônes. Veuillez patienter."

30 "Dossiers d'icônes"

50 "Le fichier d'icône indiqué est incorrect.\n\
Corrigez-le dans la zone Entrez un nom\n\
de fichier d'icône."
$ ONLY TRANSLATE  "Empty".   Do not translate the brackets "[" and "]".  

60 "[Vide]"

$set 12        Redefine set#

$ *****************************************************************************  
$ **  
$ ** Message set for  Open File Dialog  
$ **  
$ *****************************************************************************  

10 "Filtre"

11 "Répertoires"

12 "Fichiers"

13 "Sélection"

20 "Ouvrir un fichier"

25 "Créer une action - Ouverture"

$set 13
$ *****************************************************************************
$ **
$ ** Message set for  DieFromToolTalkError Dialog
$ **
$ *****************************************************************************
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Create Action - Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
$ "-NEW"
3 OK
