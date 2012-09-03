$ "$TOG: dtfile.msg /main/11 1999/09/16 09:44:10 mgreess $"  
$ *************************************<+>*************************************  
$ *****************************************************************************  
$ **  
$ **  File:        dtfile.msg  
$ **  
$ **  Project:     Common Desktop Environment dtfile  
$ **  
$ **  Description:  
$ **  -----------  
$ **  This file is the source for the message catalog for dtfile  
$ **  Any additional messages are to be added to this file by hand,  
$ **  and the associated index number placed in the code.  
$ **  
$ *****************************************************************************  
$ **  
$ **     (c) Copyright 1990, 1991, 1992 Hewlett-Packard Company  
$ **     All Rights reserved  
$ **  
$ **  
$ *****************************************************************************  
$ *************************************<+>*************************************  
$ *************************************<L>*************************************  
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------  
$ * Comment lines begin with a $ except $set which indicates the start of a  
$ * new set.  
$ *  
$ * Do not delete any comments; you may add comments for your use.  
$ *  
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------  
$ *************************************<L>*************************************  
$ *****************************************************************************  
$   
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****  
$   
$ There may be three types of messages in this file:  
$   
$ 1. Messages that appear in dialogs or are displayed to the user.  
$   
$     These messages are the default and they should ALL BE LOCALIZED.  
$     Note that these messages do NOT have any identification (see the  
$     comments for type 2 and 3 below).  
$   
$ 2. Messages that only appear in the DT error log file ($HOME?.dt/errorlog).  
$   
$     The localization of these messages is OPTIONAL.  These messages are  
$     identified by the following:  
$   
$     MESSAGES xx-yy WILL ONLY APPEAR IN THE DT ERRORLOG FILE  
$   
$ 3. Messages that should not be localized.  
$   
$     These messages are identified by the following:  
$   
$     DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy  
$   
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****  
$   
$ ********** PROLOGUE ****************  
$  Date    Name               Note  
$ ======== =================  ===============================================  
$ 03/25/93 H. Amro            removed the guilty $ from message 30  
$   
$   
$   
$ *****************************************************************************  
$quote "

$set 2
$ ****** Module: ChangeDir.c ****(developer comment only)********  
$ Messages 3 and 10 are titles for the Change Folder dialog, Message 3 is  
$   used when the user has specified a title for dtfile windows  
$ Messages 2 and 4 are labels for the Change Folder dialog  
$ Message 1 is the title for Change Folder errors  
$ Messages 7, 8, 9, 11, and 12 are Change Folder error messages  

1 "Erreur de Changement de dossier"

2 "Nouveau dossier:"

3 "Changer de dossier"

4 "Hôte:"

7 "Le nom de dossier suivant n'est pas valide:\n\n\
%s:%s"

8 "Vous ne pouvez pas passer à ce dossier. Vous êtes\n\
autorisé à visualiser seulement les dossiers sous\n\
le dossier en cours. Vous ne pouvez pas préciser\n\
un chemin absolu vers le nouveau dossier."

9 "Cette vue est en mode restreint. Vous ne pouvez pas\n\
passer au dossier spécifié car il n'est pas dans le\n\
chemin relatif du dossier restreint.\n"

10 "Changement de dossier"

11 "Impossible d'obtenir le nouveau nom de dossier."

12 "Entrez le nom du dossier s'il vous plait."

13 "Aller à l'erreur"

14 "Dossier de destination:"

15 "Aller à"

16 "Nom du système:"

17 "Gestionnaire de fichiers - Aller à"

18 "Le nom du dossier de destination est manquant.\n\
   Entrez un nom de dossier ou sélectionner un dossier dans la liste."

19 "Le nom de dossier suivant est incorrect.\n\n\
%s"

20 "Vous ne pouvez pas passer au dossier\n\n\
%s\n\n\
Il s'agit d'un dossier restreint que vous n'êtes\n\
pas autorisé à visualiser."

$set 3        Redefine set#

$ ****** Module: ChangeDirP.c ****(developer comment only)********  
$ Messages 2-6 are labels for the folder Status Line  
$ Message 3 is required for SUN systems  
$ Message 2-9 are obsolete  

2 "%d Fichiers %d Cachés"

3 "%1$d Fichiers %2$d Cachés"

4 "%3d Fichiers ..."

5 "Lecture en cours ..."

6 "%d Caché(s)"

7 "%d Eléments %d Masqués"

8 "%1$d Eléments %2$d Masqués"

9 "%3d Eléments ..."

10 "%d Elément(s)"

11 "%d Elément(s)..."

12 "%d Elément(s) %d Masqué(s)"

$set 4        Redefine set#


$set 5        Redefine set#


$set 6        Redefine set#


$set 7        Redefine set#


$set 8        Redefine set#


$set 9        Redefine set#

$ ****** Module: File.c ****(developer comment only)********  
$ Messages 6 and 7 are the title and text for an action error message  
$ Messages 8 and 9 are rename error messages  
$ Messages 11-19 are the title and text for warning messages displayed  
$   when an object/set of objects is moved/copied/linked to a folder  
$   containing object(s) with the same name  

6 "Erreur d'action"

7 "Aucune action définie pour %s.\n"

8 "Il y a déjà un fichier de ce nom dans l'espace de\n\
travail. Choisissez un nom différent."

9 "Un fichier de ce nom existe déjà.\n\
Choisissez un nom différent."

11 "Déplacement/Copie/Liaison - Avertissement"
$ Messages 12-15 deal with multiple files/folders  

12 "Les fichiers ou dossiers\n\
suivants existent déjà dans\n\
%s:"

13 "\n\
Si vous effectuez la copie, ces fichiers seront écrasés.\n\
          Voulez-vous continuer ?\n"

14 "\n\
Si vous effectuez le déplacement, ces fichiers seront écrasés.\n\
          Voulez-vous continuer ?\n"

15 "\n\
Si vous effectuez la liaison, ces fichiers seront écrasés.\n\
          Voulez-vous continuer ?\n"
$ Messages 16&17 deal with a single file/folder  

16 "Un objet appelé\n\
%s \n\
existe déjà dans le dossier\n\
%s"

17 "Un dossier appelé\n\
%s\n\
existe déjà dans le dossier\n\
%s"
$ Labels in rename/replace and merge/replace dialogs  

20 "Que souhaitez-vous faire ?"

21 "Remplacer l'objet existant"

22 "Renommer l'objet existant en:"

24 "Fusionner le contenu des deux dossiers"

25 "Remplacer le dossier existant"

27 "Ok"

28 "Annuler"

29 "Aide"

30 "Annuler déplacement"

31 "Annuler copie"

32 "Annuler lien"
$ Window title for overwrite dialogs  

33 "Gestionnaire de fichiers - Avertissement Déplacement/Copie/Lien"

34 "Gestionnaire de fichiers - Avertissement déplacement"

35 "Gestionnaire de fichiers - Avertissement copie"

36 "Gestionnaire de fichiers - Avertissement lien"

37 "Renommer objet - Erreur"

38 "%s\n\
existe déjà: choisissez un autre nom."

39 "%s\n\
ne peut pas être renommé en\n\
%s\n\
%s"

40 "Remplacement objet - Erreur"

$ 41 OBS- "L'icône de ce type de données est déjà affichée."

51 "copie en cours"

52 "déplacement en cours"

53 "lien en cours"

54 "changement en cours"

56 "Remplacer les objets existants"

57 "Renommer les objets existants en leur ajoutant un numéro"

58 "copie effectuée"

59 "déplacement effectué"

60 "lien établi"

61 "modification effectuée"

68 "1 objet %s."

69 "%d objets %s."

70 "%s\n\
%d objets possèdent le même nom que des objets du dossier\n\
%s"

71 "%s\n\
1 objet possède le même nom qu'un objet du dossier\n\
%s"

72 "Copier"

73 "Déplacer"

74 "Lier"

75 "Opération"

76 "copier"

77 "déplacer"

78 "lier"

79 "opération"

80 "%s annulé - le dossier n'était pas %s.\n\n\
Le dossier à remplacer n'a pas pu être supprimé car un objet\n\
de ce dossier ne peut pas être supprimé.\n\
Certains objets de ce dossier ont pu être supprimés\n\
avant que l'opération %s ne soit interrompue."

81 "%s\n\n\
Vous ne disposez probablement pas des droits d'accès en écriture\n\
pour l'objet qui n'a pas pu être déplacé.\n\
Modifiez vos droits ou renommez manuellement le dossier sur lequel\n\
l'opération %s est lancée."

82 "%s annulé\n\n\
%s\n\
ne peut pas être supprimé.\n\n\
Vous ne disposez probablement pas des droits d'accès en\n\
écriture pour ce fichier. Modifiez vos droits ou renommez\n\
manuellement le fichier sur lequel l'opération\n\
%s est lancée."

83 "%s annulé - Aucun des objets n'a subi l'opération %s.\n\n\
 L'objet suivant n'a pas pu être automatiquement renommé\n\
%s\n\n\
%d objets ont été renommés avant que l'opération %s ne soit arrêtée."

84 "%s\n\n\
Vous ne possédez probablement pas les droits d'accès en écriture pour\n\
l'objet désigné ci-dessus. Modifiez vos droits d'accès ou renommez \n\
manuellement l'objet sur lequel l'opération %s est lancée."

85 "%s annulé - Aucun des objets n'a subi l'opération %s.\n\n\
 L'objet suivant n'a pas pu être automatiquement renommé\n\
%s\n\n\
1 objet a été renommé avant que l'opération %s ne soit arrêtée."

86 "%s annulée - aucun des objets n'a subi l'opération %s.\n\n\
Un dossier à remplacer n'a pas pu être supprimé.\n\
   Dossier: %s\n\n\
Certains objets à l'intérieur du dossier ont été supprimés\n\
avant que l'opération %s ne soit arrêtée.\n\n\
%d autres objets ont été supprimés.\n\n\
Vous ne disposez probablement pas des droits d'accès en écriture pour\n\
le dossier ci-dessus. Modifiez vos droits d'accès ou renommez manuellement\n\
le dossier sur lequel l'opération %s est lancée."

87 "%s annulé - aucun des objets n'a subi l'opération %s.\n\n\
Un dossier à remplacer n'a pas pu être supprimé.\n\
   Dossier: %s\n\n\
Certains objets de ce dossier ont pu être supprimés avant que\n\
l'opération %s ne soit arrêtée.\n\n\
1 autre objet a été supprimé.\n\n\
Vous ne devez probablement pas disposer des droits d'accès en écriture\n\
pour le dossier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le dossier sur lequel l'opération %s est lancée."

88 "%s annulé - aucun des objets n'a subi l'opération %s.\n\n\
Un fichier à remplacer n'a pas pu être supprimé.\n\
    Fichier: %s\n\n\
%d autres objets ont été supprimés.\n\n\
Vous ne disposez probablement pas des droits d'accès en écriture\n\
pour le fichier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le fichier sur lequel l'opération %s est lancée."

89 "%s annulé - aucun des objets n'a subi l'opération %s.\n\n\
Un fichier à remplacer n'a pas pu être supprimé.\n\
    Fichier: %s\n\n\
1 autre objet a été supprimé.\n\n\
Vous ne disposez probablement pas des droits d'accès en écriture\n\
pour le fichier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le fichier sur lequel l'opération %s est lancée."

90 "Conflit de noms.\n\
  Cet objet se trouve sur l'espace de travail.\n\
  Un autre objet posséde un nom identique à celui que\n\
  vous essayez d'entrer.\n\
  Choisissez un autre nom."

91 "Arrêt copie"

92 "Arrêt lien"

93 "Arrêt déplacement"

94 "Erreur"

95 "Déplacement annulé - le dossier n'a pas été déplacé.\n\n\
L'un des objets du dossier n'ayant pas pu être supprimé,\n\
celui-ci n'a pas pu être remplacé.\n\
Certains objets du dossier ont pu être supprimés\n\
avant que le déplacement ne soit interrompu.\n\n\
Vous ne disposez probablement pas des droits d'accès en écriture\n\
sur l'objet qui n'a pas pu être supprimé.\n\
Modifiez vos droits d'accès ou renommez manuellement\n\
le dossier à déplacer."

96 "Copie annulée - le dossier n'a pas été copié.\n\n\
L'un des objets du dossier n'ayant pas pu être \n\
supprimé, celui-ci n'a pas pu être copié.\n\
Certains objets du dossier ont pu être supprimés avant que\n\
la copie ne soit interrompue.\n\n\
Vous ne disposez probablement pas des droits d'accès en\n\
écriture sur l'objets qui n'a pas pu être supprimé.\n\
Modifiez vos droits d'accès ou renommez manuellement\n\
le dossier à copier."

97 "Liaison annulée - le dossier n'a pas été lié.\n\n\
L'un des objets du dossier n'ayant pas pu être supprimé,\n\
celui-ci n'a pas pu être remplacé.\n\
Certains objets du dossier ont pu être supprimés avant\n\
que la liaison ne soit interrompue.\n\n\
Vous ne disposez probablement des droits d'accès en écriture\n\
sur l'objet qui n'a pas pu être supprimé.\n\
Modifiez vos droits d'accès ou renommez manuellement\n\
le dossier à lier."

98 "Opération annulée - le dossier n'a pas été modifié.\n\n\
L'un des objets du dossier n'ayant pas pu être supprimé,\n\
celui-ci n'a pas pu être modifié.\n\
Certains objets du dossier ont pu être supprimés avant\n\
que l'opération ne soit interrompue.\n\n\
Vous ne disposez probablement pas des droits d'accès en écriture\n\
sur l'objet qui n'a pas pu être supprimé.\n\
Modifiez les droits d'accès ou renommez manuellement\n\
le dossier à modifier."

99 "Déplacement annulé.\n\n\
%s\n\
n'a pas pu être supprimé.\n\n\
Vous ne disposez probablement pas des droits d'accès\n\
en écriture sur le fichier.\n\
Modifiez vos droits d'accès ou renommez manuellement\n\
le fichier à déplacer."

100 "Copie annulée.\n\n\
%s\n\
n'a pas pu être supprimé.\n\n\
Vous ne disposez probablement pas des droits d'accès\n\
en écriture sur le fichier.\n\
Modifiez vos droits d'accès ou renommez manuellement le\n\
fichier à copier."

101 "Liaison annulée.\n\n\
%s\n\
n'a pas pu être supprimé.\n\n\
Vous ne disposez probablement pas des droits d'accès\n\
en écriture sur le fichier.\n\
Modifiez vos droits d'accès ou renommez manuellement\n\
le fichier à lier."

102 "Opération annulée.\n\n\
%s\n\
n'a pas pu être supprimé.\n\n\
Vous ne disposez probablement pas des droits d'accès en\n\
écriture sur le fichier.\n\
Modifiez vos droits d'accès ou renommez manuellement\n\
le fichier à modifier."

103 "Déplacement annulé - aucun objet n'a été déplacé.\n\n\
L'objet suivant n'a pas pu être renommé automatiquement:\n\
%s\n\n\
%d objets ont été renommés avant que le déplacement ne soit interrompu.\n\n\
Vous ne disposez probablement pas des droits d'accès en écriture\n\
sur l'objet ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement l'objet à déplacer."

104 "Copie annulée - aucun objet n'a été copié.\n\n\
L'objet suivant n'a pas pu être renommé automatiquement:\n\
%s\n\n\
%d objets ont été renommés avant que la copie ne soit interrompue.\n\n\
Vous ne disposez probablement des droits d'accès en écriture sur\n\
l'objet ci-desssus. Modifiez vos droits d'accès ou renommez\n\
manuellement l'objet à copier."

105 "Liaison annulée - aucun des objets n'a été lié.\n\n\
L'objet suivant n'a pas pu être renommé automatiquement:\n\
%s\n\n\
%d objets ont été renommés avant l'interruption de la liaison.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture \n\
sur l'objet ci-dessus. Modifiez les droits d'accès ou renommez\n\
manuellement l'objet à lier."

106 "Opération annulée - aucun objet n'a été modifié.\n\n\
L'objet suivant n'a pas pu être renommé automatiquement:\n\
%s\n\n\
%d objets ont été renommés avant l'interruption de l'opération.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur l'objet ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement l'objet à modifier."

107 "Déplacement annulé - aucun objet n'a été déplacé.\n\n\
L'objet suivant n'a pas pu être renommé automatiquement:\n\
%s\n\n\
1 objet a été renommé avant l'interruption du déplacement.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur l'objet ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement l'objet à déplacer."

108 "Copie annulée - aucun objet n'a été copié.\n\n\
L'objet suivant n'a pas pu être renommé automatiquement:\n\
%s\n\n\
1 objet a été renommé avant l'interruption de la copie.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur l'objet ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement l'objet à copier."

109 "Liaison annulée - aucun objet n'a été lié.\n\n\
L'objet suivant n'a pas pu être renommé automatiquement:\n\
%s\n\n\
1 objet a été renommé avant l'interruption de la liaison.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur l'objet ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement l'objet à lier."

110 "Opération annulée - aucun objet n'a été modifié.\n\n\
L'objet suivant n'a pas pu être renommé automatiquement:\n\
%s\n\n\
1 objet a été renommé avant l'interruption du processus.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur l'objet ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement l'objet à modifier."

111 "Déplacement annulé - aucun objet n'a été déplacé.\n\n\
Un dossier à remplacer n'a pas pu être supprimé.\n\
   Dossier: %s\n\n\
Certains objets du dossier ont pu être supprimés\n\
avant l'interruption du déplacement.\n\n\
%d autres objets ont été supprimés.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le dossier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le dossier à déplacer."

112 "Copie annulée - aucun objet n'a été copié.\n\n\
Un dossier à remplacer n'a pas pu être supprimé.\n\
   Dossier: %s\n\n\
Certains objets du dossier ont pu être supprimés\n\
avant l'interruption de la copie.\n\n\
%d autres objets ont été supprimés.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le dossier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le dossier à copier."

113 "Liaison annnulée - aucun objet n'a été lié.\n\n\
Un dossier à remplacer n'a pas pu être supprimé.\n\
   Dossier: %s\n\n\
Certains objets du dossier ont été supprimés\n\
avant l'interruption de la liaison.\n\n\
%d autres objets ont été supprimés.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le dossier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le dossier à lier."

114 "Opération annulée - aucun objet n'a été modifié.\n\n\
Un dossier à remplacer n'a pas pu être supprimé.\n\
   Dossier: %s\n\n\
Certains objets du dossier ont pu être supprimés avant\n\
l'interruption de l'opération.\n\n\
%d autres objets ont été supprimés.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le dossier ci-dessus. Redéfinissez vos droits d'accès ou \n\
renommez manuellement le dossier à modifier."

115 "Déplacement annulé - aucun objet n'a été déplacé.\n\n\
Un dossier à remplacer n'a pas pu être supprimé.\n\
   Dossier: %s\n\n\
Certains objets ont pu être supprimés avant l'interruption\n\
du déplacement.\n\n\
1 autre objet a été supprimé.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le dossier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le dossier à déplacer."

116 "Copie annulée - aucun objet n'a été copié.\n\n\
Un dossier à remplacer n'a pas pu être supprimé. \n\
   Dossier: %s\n\n\
Certains objets de ce dossier ont pu être supprimés\n\
avant l'interruption de la copie.\n\n\
1 autre objet a été supprimé.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le dossier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le dossier à copier."

117 "Liaison annulée - aucun objet n'a été lié.\n\n\
Un dossier à remplacer n'a pas pu être supprimé.\n\
   Dossier: %s\n\n\
Certains objets du dossier ont pu être supprimés avant\n\
l'interruption de la liaison.\n\n\
1 autre objet a été supprimé.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le dossier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le dossier à lier."

118 "Opération annulée - aucun objet n'a été modifié.\n\n\
Un dossier à remplacer n'a pas pu être supprimé.\n\
   Dossier: %s\n\n\
Certains objets de ce dossier ont pu être supprimés\n\
avant l'interruption de l'opération.\n\n\
1 autre objet a été supprimé.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le dossier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le dossier à modifier."

119 "Déplacement annulé - aucun objet n'a été déplacé.\n\n\
Un fichier à remplacer n'a pas pu être supprimé.\n\
    Fichier: %s\n\n\
%d autres objets ont été supprimés.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le fichier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le fichier à déplacer."

120 "Copie annulée - aucun objet n'a été copié.\n\n\
Un fichier à remplacer n'a pas pu être supprimé.\n\
    Fichier: %s\n\n\
%d autres objets ont été supprimés.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le fichier ci-desssu. Modifiez vos droits d'accès ou renommez\n\
manuellement le fichier à copier."

121 "Liaison annulée - aucun objet n'a été lié.\n\n\
Un fichier à remplacer n'a pas pu être supprimé.\n\
    Fichier: %s\n\n\
%d autres objets ont été supprimés.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le fichier ci-desssu. Modifiez vos droits d'accès ou renommez\n\
manuellement le fichier à lier."

122 "Opération annulée - aucun objet n'a été modifié.\n\n\
Un fichier à remplacer n'a pas pu être supprimé.\n\
    Fichier: %s\n\n\
%d autres objets ont été supprimés.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le fichier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le fichier à modifier."

123 "Déplacement annulé - aucun objet n'a été déplacé.\n\n\
Un fichier à remplacer n'a pas pu être supprimé.\n\
\ Fichier: %s\n\n\
1 autre objet a été supprimé.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le fichier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le fichier à déplacer."

124 "Copie annulée - aucun objet n'a été copié.\n\n\
Un fichier à remplacer n'a pas pu être supprimé.\n\
    Fichier: %s\n\n\
1 autre objet a été supprimé.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le fichier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le fichier à copier."

125 "Liaison annulée - aucun objet n'a été lié.\n\n\
Un fichier à remplacer n'a pas pu être supprimé.\n\
    Fichier: %s\n\n\
1 autre objet a été supprimé.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le fichier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le fichier à lier."

126 "Opération annulée - aucun objet n'a été modifié.\n\n\
Un fichier à remplacer n'a pas pu être supprimé.\n\
    Fichier: %s\n\n\
1 autre objet a été supprimé.\n\n\
Vous ne disposez probablement pas du droit d'accès en écriture\n\
sur le fichier ci-dessus. Modifiez vos droits d'accès ou renommez\n\
manuellement le fichier à modifier."

127 "1 objet déplacé."

128 "%d objets déplacés."

129 "1 objet copié."

130 "%d objects copiés."

131 "1 objet lié."

132 "%d objets liés."

133 "1 objet modifié."

134 "%d objets modifiés."

135 "Impossible de déplacer l'objet %s sur lui-même."

136 "Impossible de copier l'objet %s sur lui-même."

137 "Impossible de lier l'objet %s à lui-même."

138 "Impossible de renommer le(s) objet(s) suivant(s):\n\n\
%s\n\n\
Vous n'êtes probablement pas autorisé\n\
à renommer ce(s) objet(s)."

139 "Impossible de remplacer le(s) objet(s) suivant(s):\n\n\
%s\n\n\
Vous n'êtes probablement pas autorisé à\n\
remplacer ce(s) objet(s)."
140 "Renommer le nouvel objet à : "
141 "Renommer les nouveaux objets en leur annexant un nombre: "

$set 10        Redefine set#

$ ****** Module: FileDialog.c ****(developer comment only)********  
$ Messages 1, 2, 20, and 21 are titles for error dialogs  
$ Message 3 is an error message for dialogs requiring new file names  
$ Messages 5 and 27 are titles for the Copy File dialog, Message 5 is used  
$   when the user has specified a title for dtfile windows  
$ Messages 6-9 are labels for the File Copy, New Dir, and New File dialogs  
$ Messages 10-13 and 25 are error messages for the file copy operation  
$ Messages 14 and 28 are titles for the New Folder dialog, Message 14 is  
$   used when the user has specified a title for dtfile windows  
$ Message 16 is a label for the New Folder dialog  
$ Messages 22-24 are error messages for the New File/Folder operations  
$ Messages 26 and 29 are titles for the New File dialog, Message 26 is used  
$   when the user has specified a title for dtfile windows  
$ Messages 30 and 31 are titles for the Move File dialog, Message 30 is used  
$   when the user has specified a title for dtfile windows  
$ Messages 32 and 33 are titles for the Link File dialog, Message 32 is used  
$   when the user has specified a title for dtfile windows  

1 "Erreur d'attribution du nouveau nom de fichier"

2 "Erreur de copie de fichier"

3 "Les espaces sont interdits dans un nom de fichier ou de dossier."

5 "Copier un fichier"

6 "Nom de fichier actuel:"

7 "Nouveau nom de fichier:"

8 "Type de fichier"

9 "Afficher le type de fichier"

10 "Le fichier sélectionné à l'origine n'existe plus."

11 "Il est interdit d'entrer des blancs comme nom de fichier ou de dossier."

13 "Nom de fichier ou de dossier non valide."

14 "Nouveau dossier"

16 "Nom du nouveau dossier:"

20 "Erreur de création de dossier"

21 "Erreur de création de fichier"

22 "Impossible de créer un fichier ou un dossier sans nom."

23 "Nom de dossier non valide."

24 "Nom de fichier non valide."

25 "Vous ne pouvez pas utiliser la commande Copie pour copier\n\
un fichier dans l'espace de travail. Vous devez utiliser\n\
la commande Afficher dans l'espace de travail du menu Fichier,\n\
ou poser le fichier dans l'espace de travail."

26 "Nouveau fichier"

27 "Gestionnaire de fichiers - Copie de fichier"

28 "Gestionnaire de fichiers - Nouveau dossier"

29 "Gestionnaire de fichiers - Nouveau fichier"

30 "Déplacer un fichier"

31 "Gestionnaire de fichiers - Déplacement de fichier"

32 "Lier un fichier"

33 "Gestionnaire de fichiers - Lien entre fichiers"

34 "Dossier de destination: "

35 "Nom d'objet:"

36 "Afficher icône"

37 "Objet sélectionné:"

38 "Nom de la copie:"

39 "Erreur de modification de nom"

40 "Cet objet est une action.\n\
   Les libellés d'icônes d'action ne peuvent pas être directement renommées."

41 "L'icône correcte de ce type de données est déjà affichée."

42 "Déplacer objet"

43 "Gestionnaire de fichiers - Déplacement d'objet"

44 "Copier objet"

45 "Gestionnaire de fichiers - Copie d'objet"

46 "Lier objet"

47 "Gestionnaire de fichiers - Liaison d'objet"

48 "Nouveau nom d'objet:"

$set 11        Redefine set#

$ ****** Module: FileManip.c ****(developer comment only)********  
$ Message 6 is the title for an error dialog  
$ Messages 9, 14, 16, 28, and 30-36 are file manipulation error messages  

6 "Erreur de manipulation de fichier"

9 "Vous ne pouvez pas poser un fichier\n\
de la corbeille vers l'espace de travail."

14 "Impossible de localiser les dossiers."

16 "Un dossier ne peut pas être copié sur lui-même.\n\
%s"

28 "%s est introuvable."

$ 29 OBS- "Afficher également (facultatif):"

30 "Impossible de déplacer ou de renommer le dossier %s.\n\
Vous devez fermer tous les dossiers ou sous-dossiers\n\
affichés pour pouvoir déplacer ou renommer ce dossier."

31 "Erreur à la lecture de %s."

32 "Impossible de renommer %s."

33 "Ouverture impossible de %s."

34 "Vous n'avez pas l'autorisation de déplacer %s. Vous\n\
pouvez toutefois copier le fichier, en le faisant glisser\n\
tout en maintenant la touche Ctrl enfoncée."

35 "Impossible de copier un dossier sur lui-même."

36 "Impossible de créer un processus fils."

37 "Vous ne pouvez pas poser de fichiers de\n\
la corbeille sur l'espace de travail.\n\
Pour retirer un objet de la corbeille,\n\
   -Sélectez l'objet et\n\
   -Choisissez l'action 'Redéposer'\n\
à partir du menu fichier ou du menu contextuel de l'objet.\n\
Vous pouvez ensuite faire glisser l'objet hors du Gestionnaire de fichiers\n\
et le placer sur le fond de l'espace de travail."

38 "Vous ne disposez pas des droits pour déplacer %s\n\
Toutefois, vous pouvez copier l'objet.\n\
Pour copier un objet:\n\
  - Maintenez enfoncée la touche <Ctrl> et\n\
  - faire glisser l'objet à l'aide de la souris.\n\
Ou\n\
  - utilisez 'Copier vers' dans le menu 'Sélectionné' de la barre de menus."

39 "Impossible de créer le processus enfant.\n\
   Le nombre maximal de processus pour ce système est atteint.\n\
   Arrêtez certains processus ou programmes en cours puis\n\
   relancez cette fonction."

40 "Les objets source et destination sont les mêmes.\n\
%s"

41 "Le dossier\n\
%s\n\
n'existe pas."

42 "Il n'existe pas d'espace disponible\n\
   sur l'unité cible de la copie."

43 "Il n'existe pas d'espace disponible\n\
   sur l'unité cible du déplacement."

44 "Aucune opération de déplacement n'a été lancée sur\n\
   l'objet %s. Vous ne pouvez déplacer un objet dans le dossier dans lequel\n\
   il se trouve. Vous devez entrer une destination différente (nom de dossier)\n\
\   avant de pouvoir déplacer l'objet."

45 "Aucune opération de copie n'a été lancée sur\n\
   l'objet %s. Vous devez changer le dossier de destination ou le nom de copie\n\
\   avant que la copie ne soit créée."

46 "Aucune opération de lien n'a été lancée sur l'objet %s.\n\
   Vous devez changer le dossier de destination ou le nom de copie avant que\n\
   le lien ne soit créé."

47 "Impossible de déplacer un dossier sur lui-même."

48 "Impossible de copier les objets suivants dans le dossier \"%s\":\n\n\
%s\n\n\
Vous ne disposez probablement pas du droit d'accès\n\
en écriture sur le dossier cible."

49 "Impossible de déplacer les objets suivants dans le dossier \"%s\":\n\n\
%s\n\n\
Vous ne disposez probablement pas du droit d'accès\n\
en écriture sur le dossier cible."

50 "Impossible de déplacer objets suivants dans le dossier \"%s\":\n\n\
%s\n\n\
Vous n'êtes probablement pas autorisé à \n\
déplacer l'objet source."
$        
$ Messages 51 - 56 are new since CDE/SI.          
$        

51 "Impossible de copier le fichier ou le dossier,\n\
sous peine de dépasser le quota du disque cible\n\
de la copie."

52 "Impossible de déplacer le fichier ou le dossier,\n\
sous peine de dépasser le quota du disque cible\n\
du déplacement."

53 "La copie du fichier ou du dossier n'a pas\n\
abouti en raison de l'erreur interne\n\
suivante:"

54 "Le déplacement du fichier ou du dossier n'a\n\
pas abouti en raison de l'erreur interne\n\
suivante:"

55 "Faites appel à l'administrateur système."

56 "(Inconnu(es)):"

$quote

57 Vous n'êtes pas autorisé à déplacer \n\
le fichier%s\n\
Droit d'accès en écriture obligatoire.

58 Process Create Error
59 Cannot create a child process to delete the dropped files.

$set 12        Redefine set#

$ ****** Module: FileMgr.c ****(developer comment only)********  
$ Message 3 is a dialog title  
$ Messages 4-6 are movement warning or error messages  
$ Message 7 is the title for the folder popup menus  

$quote "
3 "Déplacer un/des fichier(s)"

4 "L'objet %s est dans le dossier %s.\n\
Vous avez demandé son déplacement dans le dossier %s.\n\
Confirmez-vous l'opération ?"

5 "L'objet %1$s est dans le dossier %2$s.\n\
Vous avez demandé son déplacement dans le dossier %3$s.\n\
Confirmez-vous l'opération ?"

6 "Au moins l'un des fichiers à déplacer vers %s ne se\n\
trouve pas dans le même dossier que les autres.\n\
Confirmez-vous l'opération ?"

7 "Gestionnaire de fichiers"

18 "Le dossier %s est vide."

19 "Le dossier %s ne contient pas de sous-répertoires."

$set 13        Redefine set#

$ ****** Module: Filter.c ****(developer comment only)********  
$ Messages 11 and 21-22 are button labels for the Modify Filter List dialog  
$ Messages 19 and 24 are titles for the Link File dialog, Message 19 is used  
$   when the user has specified a title for dtfile windows  
$ Message 23 is a text field label for the Modify Filter List dialog  
$ Message 25 is a label for the Modify Filter List dialog  

11 "Par défaut"

19 "Définir les options de filtrage"

21 "Tout sélectionner"

22 "Tout désélectionner"

23 "Chaîne de filtrage (facultatif):"

24 "Gestionnaire de fichiers - Définition des options de filtrage"

25 "Sélectionner les types de fichiers à cacher:"

26 "Masquer également (facultatif):"

27 "Affiché"

28 "Masqué"

29 "Afficher aussi (facultatif):"

30 "Sélectionnez les types de données à masquer."

31 "Sélectionnez les types de données à:"

$set 14        Redefine set#


$set 15        Redefine set#

$ ****** Module: Find.c ****(developer comment only)********  
$ Message 1 is the title for an error dialog  
$ Messages 14 and 15 are button labels for the Find Files dialog  
$ Messages 23, 26, and 27 are Find Files error messages  
$ Messages 29 and 41 are titles for the Find Files dialog, Message 29 is used  
$   when the user has specified a title for dtfile windows  
$ Messages 30-40 and 42 are labels for the Find Files dialog  

1 "Erreur de recherche de fichier"

14 "Lancer"

15 "Arrêter"

$ 18 OBS- "Le dossier %s est vide."

$ 19 OBS- "Le dossier %s ne contient pas de sous-répertoire."

23 "Données de recherche non valides.\n\n\
Vous devez préciser un dossier de recherche."

26 "Données de recherche non valides.\n\n\
Le fichier sélectionné n'existe plus."

27 "Aucun fichier ne correspond aux données de recherche."

29 "Recherche de fichiers ou de dossiers"

30 "Complétez une ou plusieurs zones indiquant les fichiers à rechercher:"

31 "Nom du fichier ou du dossier:"

32 "Contenu:"
$ Messages 33-35 are used on hpux systems  

33 "Oui"

34 "Non"

35 "Suivre liens:"

36 "Ouvrir dans une nouvelle fenêtre"

37 "Afficher dans l'espace de travail"

38 "Fichiers repérés (par contenu):"

39 "Fichiers trouvés (par nom et contenu):"

40 "Fichiers trouvés (par nom):"

41 "Gestionnaire de fichiers - Recherche de dossiers ou de répertoires"

42 "Dossier de recherche:"

43 "Erreur de recherche"

44 "L'argument de recherche de dossier est manquant.\n\
Entrez le nom du dossier à partir duquel la recherche doit commencer."

45 "Le fichier sélectionné n'existe plus.\n\n\
Un utilisateur a supprimé le fichier après la fin du processus de recherche."

46 "Recherche"

47 "Remplissez une ou plusieurs zones pour indiquer les éléments à rechercher:"

48 "Ouvrir dossier"

49 "Gestionnaire de fichiers - Recherche"

50 "Le nom de recherche du dossier ou l'argument de contenu du dossier\n\
est manquant. Entrez le nom du dossier à partir duquel la recherche\n\
doit commencer ou entrez la chaîne à rechercher."

$set 16        Redefine set#


$set 17        Redefine set#


$set 18        Redefine set#

$ ****** Module: Main.c ****(developer comment only)********  
$ Message 1 is a blank line  
$ Messages 2, 6, 17-21 and 27-28 are error messages which will ONLY APPEAR  
$   in the DT ERRORLOG FILE  
$ above no longer true for message 2 - it will be displayed.
$ Messages 16 and 32 are titles for the Save Settings dialog, Message 16 is  
$   used when the user has specified a title for dtfile windows  
$ Message 22 is the title for an error dialog  
$ Messages 25 and 26 are usage strings  
$ Messages 29 and 30 are text for the Save Settings dialog  
$ Messages 34 and 35 are drag/drop error messages  
$ DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGE 1  

1 ""
$ MESSAGE 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

2 "Impossible de créer le dossier ou les sous-dossiers .dt."
$ MESSAGE 6 WILL ONLY APPEAR IN THE DT ERRORLOG FILE, and stderr  

6 "Erreur de X Toolkit... Sortie.\n"

16 "Sauvegarder comme options par défaut"
$ MESSSAGES 17-21 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

17 "Impossible de créer un fichier dans lequel stocker les options par défaut"

18 "Impossible d'ouvrir le fichier de session."

19 "La spécification de dossier\n\
\%s ne correspond pas à un dossier."

20 "La spécification de dossier\n\
\%s n'existe pas."

21 "Spécification de dossier %s:%s non valide."

22 "Erreur de déplacement"

23 "\n\
Syntaxe: %s...\n\n\
   -noview\n\n\
\tDtfile est exécuté en mode serveur. \n\n\
   -session nom_de_fichier\n\n\
\tDtfile est exécuté avec le fichier de session indiqué dans\n\
\tparamètre nom_de_fichier.\n\n\
   -folder dossier[,dossier,dossier]\n\
   -dir dossier[,dossier,dossier]\n\n\
\tDtfile affiche une fenêtre pour chaque dossier indiqué dans\n\
\tle paramètre Dossier\n\
\tVous pouvez indiquer une liste de noms de dossiers séparés\n\
\tpar des espaces, sous la forme 'chemin'.\n\n\
   -title Titre\n\n\
\tDtfile utilise la chaîne indiquée comme titre\n\
\tpour ses fenêtres.\n\n\
   -help_volume volume d'aide\n\n\
\tDtfile utilise le volume d'aide indiqué.\n\n\
   -restricted\n\n\
\tDtfile n'affiche pas les dossiers situés au-dessus du dossier\n\
\trestreint (dossier indiqué avec l'option -dir). Si cette option\n\
\tn'est pas spécifiée, le dossier restreint est le dossier en\n\
\tcours de l'utilisateur.\n\n\
   -grid on/off\n\n\
\ton     = Les fichiers sont affichés sous forme de grille.\n\
\toff    = Les fichiers sont affichés dans un ordre aléatoire.\n\n\
   -tree on/off\n\n\
\ton     = Les fichiers sont affichés en mode dossier unique.\n"

24 "\toff    = Les fichiers sont affichés en mode arborescent.\n\n\
   -tree_files /never/always/choose\n\n\
\tnever  = Les deux états du mode arborescent sont: partiellement étendu\n\
\tou réduit.\n\
\talways = Les deux états du mode arborescent sont: étendu ou réduit.\n\
\tchoose = Les trois états du mode arborescent sont: partiellement étendu,\n\
\tétendu ou réduit.\n\n\
   -order alphabetical/file_type/date/size\n\n\
\tLes fichiers sont affichés dans l'ordre indiqué: alphabétique,\n\
\tpar type, date ou taille.\n\n\
   -view no_icon/large_icon/small_icon/attributes\n\n\
\tLes fichiers sont affichés dans le format indiqué: texte seulement,\n\
\ttexte et icônes normales, texte et icônes réduites ou avec attributs.\n\n\
   -direction ascending/descending\n\n\
\tLes fichiers sont affichés dans le sens indiqué (croissant\n\
\tou décroissant).\n\
-large_icon_width <size>\n\n\
   -large_icon_height <taille>\n\n\
   -small_icon_width  <taille>\n\n\
   -small_icon_height <taille>\n\n\
       Taille de la zone d'affichage dans le Gestionnaire de fichiers.\n\
       Les images dépassant cette taille seront tronquées.\n\
       La taille de zone d'affichage par défaut est de 38 (grand) ou de 24 (petit).\n\n"

26 "\n\
Syntaxe (mode restreint): %s...\n\n\
   -folder dossier[,dossier,dossier]\n\
   -dir dossier[,dossier,dossier]\n\n\
\t'dossier' se trouvant sous le dossier\n\
\thome des utilisateurs et/ou incluant ce dernier.\n\n"
$ MESSSAGES 27-28 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

27 "Impossible de récupérer le fichier des paramètres par défaut \n\
pour home, utilisation de default.\n"

28 "Unable to recover the saved default tool settings file, will use default.\n"
$ Messages 29-30 are the strings displayed in a question dialog  

29 "Voulez-vous sauvegarder les dimensions de la fenêtre du Gestionnaire\n\
de fichiers, les préférences et les paramètres de filtrage ?\n\n\
Ces valeurs seront utilisées pour toutes les fenêtres du\n\
Gestionnaire de fichiers lancées à partir du tableau de bord."

30 "Voulez-vous sauvegarder les dimensions de la fenêtre du Gestionnaire de\n\
fichiers restreint, les préférences et les paramètres de filtrage ?\n\n\
Ces valeurs seront utilisées pour toutes les fenêtres du Gestionnaire\n\
de fichiers restreint lancées à partir du tableau de bord."

32 "Gestionnaire de fichiers - Sauvegarder comme options par défaut"

34 "La copie et le lien de fichiers ou de\n\
dossiers vers la corbeille sont impossibles."
$ 35 You can't copy or link a file or 
$ \

$ folder out of the Trash Can.  

35 "Sauvegarder, dans le Gestionnaire d'applications, la vue,\n\
la taille de fenêtre, les préférences et les options de filtre ?\n\n\
Ces valeurs seront utilisées pour chaque nouvelle fenêtre\n\
du Gestionnaire d'applications lancée à partir du tableau de bord."

36 "Vous ne pouvez pas copier ou lier un fichier ou un dossier à partir\n\
de la corbeille. Sortez l'objet de la corbeille et placez-le dans\n\
le Gestionnaire de fichiers.\n\
Vous pouvez alors effectuer la copie ou le lien."

37 "Vous ne pouvez pas copier ou lier un fichier ou un dossier à partir\n\
de la corbeille. Sortez l'objet de la corbeille et placez-le dans le\n\
Gestionnaire de fichiers.\n\
Vous pouvez alors effectuer la copie ou le lien."

38 "Spécification de dossier %s incorrecte."

$quote

$ MESSSAGE 39 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
39 Impossible de lancer deuxième session du Gestionnaire de fichiers \n\
car il est déjà ouvert sur le bureau. Pour lancer une nouvelle session \n\
du Gestionnaire de fichiers, fermez toutes ses fenêtres en cours \n\
et retirez tous ses objets de leurs espaces de travail.

$ 
$ "-NEW"
40 "Could not connect to ToolTalk:\n%s\nExiting ..."


$set 19        Redefine set#


$set 20        Redefine set#

$ ****** Module: Menu.c ****(developer comment only)********  
$ Messages 1-8, 123 and 9 are the text and mnemonics for the dtfile menu bar  
$ Messages 10/11, 128/129, 124/125, 121/122, 126/127, 16/17, 18/19, 84/85,  
$   90/91, 22/23, 117/118, are the text and mnemonics for the Fill   
$   pulldown menu   
$ Messages 10/11, 30/33, 92/93, 34/37, 38/41, 42/94, and 97/98 are the text  
$   and mnemonics for the Folder pulldown menu  
$ Messages 46/47, 99/100, 48/51, 52/55, 101/102, 103/104, 119/120, and   
$   62/63 are the text and mnemonics for the View pulldown menu  
$ Messages 105-116 are the text and mnemonics for the Help pulldown menu  

$quote "
1 "Fichier"

2 "F"

3 "Dossier"

4 "D"

5 "Vue"

6 "V"

7 "Sélectionné(s)"

8 "S"

9 "A"

10 "Nouveau..."

11 "N"

16 "Caractéristiques..."

17 "C"

18 "Recherche..."

19 "h"

22 "Afficher la corbeille..."

23 "i"

30 "Mettre à jour"

33 "e"

34 "Passer à..."

37 "s"

38 "Début"

41 "D"
$ NOTE: The mnemonic for Up is Message 94 in this set.  

42 "Père"

46 "Nouveau"

47 "N"

48 "Tout sélectionner"

51 "T"

52 "Tout désélectionner"

55 "o"

62 "Sauvegarder toutes les caractéristiques..."

63 "a"

84 "Afficher dans l'espace de travail"

85 "A"

90 "Mettre dans la corbeille"

91 "M"

92 "Passage accéléré à"

93 "a"
$ NOTE: Message 94 is the mnemonic for the "Up" menu item  

94 "r"

97 "Terminal"

98 "T"

99 "Ranger"

100 "R"

101 "Afficher les fichiers cachés"

102 "i"

103 "Modifier la liste des filtres..."

104 "M"

105 "Généralités"

106 "G"

107 "Tâches"

108 "T"

109 "Référence"

110 "R"

111 "Sur l'élément"

112 "u"

113 "Aide sur l'aide"

114 "i"

115 "A propos du Gestionnaire de fichiers"

116 "A"

117 "Fermer"

118 "F"

119 "Définir les caractéristiques d'une vue..."

120 "D"

121 "Copier..."

122 "o"

123 "Aide"

124 "Déplacer..."

125 "D"

126 "Lier.."

127 "L"

128 "Renommer"

129 "R"

130 "Nouveau dossier..."

131 "N"

132 "Nouveau fichier..."

133 "f"

134 "Aller à..."

135 "A"

136 "Ouvrir corbeille"

137 "Renommer..."

138 "Ouvrir nouvelle vue"

139 "O"

140 "Copier comme lien..."

141 "Définir les options de vue..."

142 "Sauvegarder comme options par défaut..."

143 "S"

144 "Définir les options de filtre..."

145 "i"

146 "Ouvrir terminal"

147 "O"

148 "Table des matières"

149 "m"

150 "Modifier les droits d'accès..."

151 "Mettre à la corbeille"

152 "Déplacer dans..."

153 "Copier dans..."

154 "Copier sous forme de lien..."

155 "Monter"

156 "Afficher objets cachés"

157 "A propos du Gestionnaire d'applications"

$set 21        Redefine set#

$ ****** Module: ModAttr.c ****(developer comment only)********  
$ Messages 1 and 21 are titles for the File Properties dialog, Message 1 is  
$   used when the user has specified a title for dtfile windows  
$ Messages 2-13 and 18 are labels for the File Properties dialog  
$ Message 14 is the output of the file modification date; DO NOT LOCALIZE  
$ Messages 15 and 16 are file properties error messages  
$ Message 17 is the title for an error dialog  
$ Message 19 is the default file owner/group, if real one can't be determined  
$ Messages 22-28 are error messages that can be generated when dtfile  
$   tries to use the dtfile.config configuration file, this file is  
$   used to customize the File Properties dialog  

1 "Droits d'accès du fichier"

2 "Nom de fichier:"

3 "Nom du propriétaire:"

4 "Nom du groupe:"

5 "Autorisations:"

6 "Lecture"

7 "Ecriture"

8 "Exécution"

9 "Propriétaire:"

10 "Groupe:"

11 "Autre:"

12 "Taille (en octets):"

13 "Date de modif.:"
$ DO NOT LOCALIZE MESSAGE 14  

14 "%x %X"

15 "Attribut de fichier non valide.\n\n\
L'utilisateur indiqué n'existe pas."

16 "Attribut de fichier non valide.\n\n\
Le groupe indiqué n'existe pas."

17 "Erreur de caractéristiques"

18 "Est lié à:"

19 "Inconnu"

21 "Gestionnaire de fichiers - Droits d'accès du fichier"

22 "Impossible d'ouvrir le fichier de configuration du Gestionnaire de fichiers:"

24 "Libellé de zone inconnu dans le fichier de configuration du Gestionnaire de fichiers:"

25 "Le fichier de configuration du Gestionnaire de fichiers ne contient pas d'informations sur l'identificateur du système de fichiers."

26 "Impossible de créer un processus pour la boîte de dialogue correspondant au système de fichiers."

27 "Une erreur s'est produite lors de l'exécution de la boîte de dialogue correspondant au système de fichiers."

28 "Tentative d'exécution de:"
$ do not localize "fsDialog"  

29 "Aucune valeur n'est indiquée dans la zone fsDialog du fichier de configuration de dtfile."

30 "Nom:"

31 "Le nom du propriétaire est incorrect.\n\
L'utilisateur indiqué n'existe pas."

32 "Le nom du groupe est incorrect.\n\n\
Aucun groupe d'utilisateur portant ce nom n'a été créé sur le\n\
système.\n\
Utilisez le programme d'administration pour créer un groupe\n\
d'utilisateurs portant ce nom."

33 "Erreur de droits d'accès"

34 "Gestionnaire de fichiers - Droits d'accès"

35 "La boîte de dialogue Permissions associée à l'objet\n\n\
%s/%s\n\n\
a été fermée. En effet, l'objet a été supprimé\n\
du système."

36 "Modification des permissions - Avertissement"

37 "Si vous appliquez les nouvelles permissions au dossier\n\
en cours, vous ne pourrez plus le visualiser. Il sera\n\
fermé automatiquement et vous n'aurez plus la\n\
possibilité de l'ouvrir.\n\n\
Confirmez-vous la modification ?"

38 File Manager Error
39 File Open Error

$set 22        Redefine set#

$ ****** Module: ModAttrP.c ****(developer comment only)********  
$ Messages 1 and 3 are file properties error messages  
$ Message 2 is the title for the properties error dialog  

1 "Impossible d'ouvrir le fichier, opération annulée."

2 "Erreur de caractéristiques"

3 "Impossible de modifier les caractéristiques de %s."

4 "Impossible d'ouvrir l'objet."

5 "Définir l'erreur de droit d'accès"

$set 23        Redefine set#

$ ****** Module: Prefs.c ****(developer comment only)********  
$ Messages 1 and 22 are titles for the Set Preferences dialog, Message 1 is  
$   used when the user has specified a title for dtfile windows  
$ Messages 2-20 and 23-29 are labels for the Set Preferences dialog  

1 "Définir les options de vue"

2 "Vue"

3 "Par nom"

4 "Par nom et icône"

5 "Par nom et icône réduite"

6 "Par nom, date, taille ..."

7 "Afficher"

8 "Par dossier"

9 "Par structure hiérarchique"

10 "Tri"

11 "Par type de fichier"

12 "Alphabétique"

13 "Par date"

14 "Par taille"

15 "Ordre"

16 "Croissant"

17 "Décroissant"

18 "Position"

19 "Grille triée"

20 "Aléatoire"

22 "Gestionnaire de fichiers - Définition des options de vue"

23 "En-têtes"

24 "Chemin d'icônes"

25 "Dossier en cours"

26 "Ligne d'état"

27 "Dossiers seulement"

28 "Dossiers, puis fichiers"

29 "Dossiers et fichiers"

30 "Par défaut"

31 "Représentation"

32 "Par nom seulement"

33 "Par grandes icônes"

34 "Par petites icônes"

35 "Rangées et colonnes"

36 "Chemin texte"

37 "Ligne de message"

$set 24        Redefine set#


$set 25        Redefine set#


$set 26        Redefine set#


$set 27        Redefine set#

$ ****** Module: Trash.c ****(developer comment only)********  
$ Messages 3, 4, and 81 are titles for error warning dialogs  
$ Message 5-8, 56, 58, 78, 82, and 83 are trash error messages  
$ Message 9 is the title of the trash container  
$ Messages 24/26 and 28/30 are the text and mnemonics for the File pulldown  
$   menu  
$ Message 73 is an error dialog title  
$ Messages 74, 75, 79, and 80 are trash warning messages  

3 "Erreur - Corbeille"

4 "Avertissement - Corbeille"

5 "Les fichiers ou dossiers suivants n'ont pas pu être placés dans la corbeille:\n"

6 "Il s'agit de dossiers qui ne sont pas vides ou de\n\
   fichiers pour lesquels vous ne disposez pas du droit\n\
   d'accès en écriture. Voulez-vous continuer ?\n"

7 "Vous ne disposez pas du droit d'accès en écriture sur les\n\
    fichiers suivants. Voulez-vous continuer ?\n"

8 "Les dossiers suivants ne sont pas vides.\n\
   Voulez-vous continuer ?\n"

9 "Corbeille"

24 "Réintégrer"

26 "R"

28 "Broyer"

30 "B"

56 "Les fichiers suivants n'ont pas pu être réintégrés:\n"
$ Message 58 supplements Message 56  

58 "(plus %d fichiers)"

73 "Broyer fichier(s)"

74 "Le fichier suivant est également dans l'espace de travail:\n\
   %s\n\
Si vous l'amenez dans la corbeille, il\n\
sera supprimé de l'espace de travail."

75 "Les fichiers suivants sont également dans l'espace de travail:\n\
   %s\n\
Si vous les amenez dans la corbeille, ils\n\
seront supprimés de l'espace de travail."

78 "Le Gestionnaire de fichiers n'accepte pas les\n\
fichiers ou les dossiers dont les nom comportent\n\
des espaces ; il est donc impossible de les supprimer.\n"

79 "Le fichier %s existe déjà dans votre système\n\
de fichiers dans:\n\
   %s\n\
Si vous l'amenez dans la corbeille, il sera supprimé\n\
du système de fichiers."

80 "Les fichiers de l'espace de travail existent déjà\n\
dans votre système de fichiers dans:\n\
   %s\n\
Si vous les amenez dans la corbeille, ils seront\n\
supprimés du système de fichiers."

81 "Erreur de broyage de fichier/dossier"

82 "Le fichier suivant n'a pas pu être supprimé du système de fichiers:\n"

83 "Impossible d'ouvrir le fichier .trashinfo du répertoire\n\
   $HOME/.dt/Trash. Ce fichier étant nécessaire au fonctionnement\n\
   de la corbeille, vérifiez les droits d'accès qui lui sont\n\
   associés, ainsi qu'au dossier qui le contient.\n"

84 "Les objets broyés sont définitivement supprimés\n\
   du système.\n\n\
   Confirmez-vous le broyage ?\n"

85 "L'objet que vous supprimez est une copie liée à un objet original du\n\
   Gestionnaire de fichiers\n\
  (%s)\n\
   Si vous cliquez sur OK, la copie liée et l'objet original seront supprimés.\n\n\
   Si vous souhaitez supprimer l'objet uniquement,\n\
      1) cliquez sur Annuler dans cette boîte de dialogue\n\
      2) sélectionnez ensuite 'Retirer de l'espace de travail'\n\
         dans le menu contextuel de l'objet."

86 "Les objets que vous supprimez sont des copies liées à des objets originaux du\n\
   Gestionnaire de fichiers\n\
  (%s)\n\
   Si vous cliquez sur OK, les copies liées et les objets originaux seront supprimés.\n\
     \n\
   Si vous souhaitez supprimer les objets uniquement,\n\
      1) cliquez sur Annuler dans cette boîte de dialogue\n\
      2) sélectez ensuite 'Retirer de l'espace de travail'dans le menu\n\
      contextuel de l'objet."

87 "Impossible d'ouvrir l'objet de la corbeille.\n\n\
   Avant d'ouvrir un objet, utilisez l'option 'Redéposer' pour le\n\
   remettre dans le Gestionnaire de fichiers."

88 "Impossible d'écrire dans un fichier temporaire.\n\
   Le système de fichiers est peut-être saturé."

89 "Impossible d'ouvrir le répertoire Corbeille\n\
   %s\n\
   (Authentification, erreur de droit d'accès)\n\n\
   Les opérations sur la corbeille ne seront pas effectuées."

90 "La 'Corbeille' ne peut pas être initialisée.\n\
   Vérifiez les points suivants:\n\
        1)$HOME/.dt/Trash pour les droits d'accès.\n\
        2)Espace disque disponible.\n\
        3)Authentification."
$ 91 is not used  

91 "Une copie de l'objet suivant sur trouve sur le fond\n\
de l'espace de travail:\n\
Si vous déplacez cet objet dans la corbeille, il sera supprimé\n\
dans le Gestionnaire de fichiers et dans l'Espace de travail."
$ 92 is not used  

92 "Une copie des objets suivants se trouve également sur\n\
le fond de l'espace de travail:\n\
Si vous déplacez ces objets dans la corbeille, ils seront supprimés \n\
du Gestionnaire de fichiers et de l'Espace de travail."

93 "Impossible d'accéder au fichier d'informations corbeille:\n\
   %s\n\
   Les opérations de corbeille ne seront pas effectuées.\n\
   Les causes les plus probables sont les suivantes:\n\
     - Authentification du réseau.\n\
     - Espace disque insuffisant.\n\
     - Droits d'accès incorrects dans $HOME/.dt/Trash."

94 "Le Gestionnaire de fichiers ne prend pas en charge lesobjets\n\
dont le nom comporte un espace. Ces objets ne peuvent\n\
donc pas être supprimés.\n\
Procédez comme suit:\n\
  - sélectionnez 'Ouvrir terminal' dans la barre de menus du Gestionnaire de fichiers\n\
  - lancez la commande de retrait supprimer les objets."

95 "Impossible d'accéder au répertoire corbeille:\n\
   %s\n\
   Les opérations de corbeille ne seront pas effectuées.\n\
   Le causes les plus probables sont les suivantes:\n\
     - Authentification de réseau.\n\
     - Espace disque insuffisant\n\
     - Droits d'accès incorrects."

96 "Les objets suivants ne peuvent pas être supprimés du système de fichiers:   \n"

97 "Les objets suivants ne peuvent pas être placés dans la corbeille:     \n"

98 "(Plus %d autre(s) objet(s))"
$ message 99 is not used  

99 "Vous ne disposez pas des droits d'accès en écriture pour le(s) fichier(s) suivant(s):\n\
Souhaitez-vous continuer ?\n"

100 "Le(s) dossier(s) suivant(s) n'est pas/ne sont pas vide(s).\n\
Souhaitez-vous continuer ?\n"

101 "L'objet ou les objets suivant(s) n'a/n'ont pas pu être redéposé(s):\n"

102 "Une copie liée de l'objet suivant se trouve sur le fond de\n\
l'espace de travail:\n\
%s\n\
Si vous déplacez l'objet dans la corbeille, il sera supprimé\n\
du Gestionnaire de fichiers et de l'espace de travail."

103 "Une copie liée des objets suivants se trouvent sur le fond\n\
de l'espace de travail:\n\
%s\n\
Si vous déplacez les objets dans la corbeille, ils seront supprimés\n\
du Gestionnaire de fichiers et de l'espace de travail."

104 "Vous ne disposez pas des droits d'accès en écriture pour le(s) fichier(s) suivant(s):\n\
Souhaitez-vous continuer ?\n"

105 "L'action par défaut de cet objet ne sera pas effectuée dans la corbeille.\n\n\
    Pour exécuter l'action par défaut de cet objet, utilisez\n\
    'Redéposer' pour le réintégrer au Gestionnaire de fichiers\n\
    et exécutez l'opération."

106 "A propos de la corbeille"

107 "Au moins un objet du dossier suivant n'a pas pu être supprimé:\n\n\
%s\n\n\
Vous n'êtes probablement pas autorisé à supprimer\n\
cet objet."

108 "Impossible de supprimer le fichier suivant:\n\n\
%s\n\n\
Vous n'êtes probablement pas autorisé à supprimer\n\
cet objet."

109 "Réintégrer - Avertissement"

110 "Il existe déjà un objet appelé:\n\n\
    %s\n\n\
Pour le remplacer par l'objet de la\n\
corbeille, sélectionnez OK.\n\n\
Pour le conserver, sélectionner Annuler.\n\
Si vous souhaitez conserver les deux objets, renommez\n\
l'objet existant et relancez la commande Réintégrer. "

111 "Réintégrer - Erreur"

112 "Impossible de supprimer le ou les objet(s)\n\
suivant(s). Il ont probablement déjà été\n\
supprimés.\n\n"

113 "Au moins un objet du dossier suivant, ou le dossier\n\
lui-même, ne peut pas être supprimé. Vous n'êtes\n\
probablement pas autorisé à effectuer cette opération.\n\n"

114 "Impossible de supprimer le(s) fichier(s) suivant(s). Vous n'êtes\n\
probablement pas autorisé à effectuer cette opération.\n\n"

115 "Le ou les objet(s) suivant(s), nécessaire(s) au fonctionnement\n\
du Bureau, ne peu(ven)t pas être supprimé(s).\n\n"

$set 28        Redefine set#

$ ****** Module: Desktop.c ****(developer comment only)********  
$ Messages 1-3 Workspace error messages  
$ Message 4 is the title for the Workspace popup menus  
$ Messages 5-8 are the labels and mnemonics for the Workspace obj popup menu  

1 "Un fichier de ce nom existe déjà dans l'espace de travail.\n\
Choisissez un nom différent."

2 "Un fichier de ce nom existe déjà dans\n\
le dossier d'origine du fichier.\n\
Choisissez un nom différent."

3 "Un fichier %s existe déjà dans cet espace de travail. Deux fichiers\n\
de nom identique ne peuvent pas coexister dans un même espace de travail.\n\
Pour conserver les deux fichiers, renommez-en un."

4 "Objet Espace de travail"

5 "Supprimer de l'espace de travail"

6 "S"

7 "Ouvrir le dossier parent"

8 "O"

9 "Un objet possédant ce nom existe déjà sur l'espace de travail.\n\
Choisissez un nom différent."

10 "Un objet possédant ce nom existe déjà dans le dossier original\n\
(le dossier d'origine de cet objet).\n\
Choisissez un autre nom."

11 "Un objet appelé %s existe déjà sur le fond de l'espace de travail.\n\
Vous ne pouvez pas placer un objet du même nom sur l'espace de travail.\n\
Pour avoir ces deux objets sur l'espace de travail, renommez l'un d'eux."

12 "Vous avez indiqué un espace de travail incorrect. Par défaut, l'espace\n\
de travail Un est utilisé. Cependant, un objet appelé:\n\
   %s\n\
existe déjà sur le fond de cet espace de travail. Vous ne pouvez pas\n\
y poser un autre objet portant le même nom.\n\
Pour conserver les deux objets, renommez l'un d'entre eux."

13 "L'objet:\n\
   %s\n\
n'existe pas dans le système de fichiers."

$set 29        Redefine set#

$ ****** Module: Help.c ****(developer comment only)********  
$ Message 1 is the title for the dtfile help dialog  
$ Messages 1 and 2 are titles for the Help dialog, Message 2 is  
$   used when the user has specified a title for dtfile windows  
$ Message 3 is the text for help on the "Open New View" action  
$ Message 4 is the text for help on the "Open In Place" action  
$ Messages 5-6 are help error messages  

1 "Gestionnaire de fichiers - Aide"

2 "%s Aide"

3 "L'action %s ouvre le dossier indiqué dans une nouvelle fenêtre du Gestionnaire de fichiers."

4 "L'action %s affiche le dossier indiqué dans la fenêtre en cours du Gestionnaire de fichiers."

5 "Aucune aide disponible pour l'action %s."

6 "Aucune aide disponible pour le type de fichier %s."

7 "L'action `%s` ouvre une nouvelle fenêtre du Gestionnaire de fichiers\n\
sur le dossier indiqué."

8 "L'action `%s` affiche le dossier requis dans la fenêtre\n\
en cours du Gestionnaire de fichiers."

9 "Aide sur la corbeille"

10 "L'aide n'est pas disponible lorsque plusieurs objets sont sélectionnés."

$set 30        Redefine set#

$ ****** Module: SharedMsgs.c ****(developer comment only)********  
$ The following messages are used in more than one place within the  
$ dtfile source.  Unless otherwise specified, they are error messages.  

1 "Lecture impossible à partir de %s."
$ Message 2 is used to format the file attributes in 'by attributes' mode  

2 "%b %d %H:%M:%S %Y"

3 "Impossible de déplacer ou de copier le fichier %s à partir de l'hôte %s.\n\
Il est possible que votre système de fichiers ne soit pas monté sur l'hôte %s."

4 "Impossible de déplacer ou de copier le fichier %1$s à partir de l'hôte %2$s.\n\
Il est possible que votre système de fichiers ne soit pas monté sur l'hôte %3$s."

5 "Impossible de lire %s."

6 "Impossible d'écrire dans le dossier %s."

7 "Impossible de créer %s."

8 "Impossible de supprimer %s."

9 "Un dossier ou un fichier appelé\n\
  %s\n\
  existe déjà et ne peut pas être écrasé.\n\
  Entrez un nom différent."
$ Message 10 is the help topic string for help on a file icon or action  

10 "Description du type de fichier ou de l'action"

11 "Vous ne pouvez pas préciser un chemin dans le nom de fichier."

12 "Informations de recherche non valides.\n\n\
Le dossier suivant n'existe pas ou n'est pas accessible:\n\n\
     %s\n"
$ Messages 13-16 are dialog titles  

13 "%s - Généralités"

14 "%s - Tutoriel"

15 "A propos de %s"

16 "Gestionnaire de fichiers"
$ Message 18 is appended to ".." folders  

18 "(précédent)"
$ Messages 19 and 20 are messages which appear when a user clicks item help  
$ outside the application.  19 is the title, while 20 is the message.  

19 "Aide sur l'élément - Erreur"

20 "Vous devez sélectionner un élément \n\
dans le Gestionnaire de fichiers."
$ Messages 21 and 22 are titles for error dialogs  

21 "Erreur de déplacement de fichier"

22 "Erreur de lien de fichier"

23 "Impossible de créer '%s'\n\n\
Vous ne possédez peut-être pas les droits d'accès\n\
corrects pour les fichiers ou les\n\
dossiers indiqués.\n\n\
Pour visualiser les droits d'accès, sélectionnez les objets,\n\
puis l'option 'Modifier les droits d'accès...'"

24 "Vous devez sélectionner un objet dans le Gestionnaire de fichiers.\n\
L'aide 'Sur l'élément' ne fonctionne qu'au sein de l'application\n\
dans laquelle vous la lancez."

25 "Impossible de créer '%s'\n\n\
Vous ne disposez probablement pas des droits d'accès\n\
pour les fichiers ou les dossiers\n\
concernés.\n\n\
Pour visualiser les droits d'accès, sélectionnez l'objet,\n\
puis 'Modifier les droits d'accès...' à partir de l'option\n\
Sélectionnés ou du menu contextuel."

26 "Vous devez sélectionner un objet dans le Gestionnaire d'applications.\n\
L'aide 'Sur l'élément' ne fonctionne qu'au sein de l'application\n\
dans laquelle vous la lancez."

$set 31        Redefine set#


$set 32        Redefine set#

$ ****** Module: dterror.src ****(developer comment only)********  
$ Message 1 is a title for dterror  

1 "Erreur dt"

2 "Gestionnaire de fichiers - Erreur d'ouverture de répertoire"

$set 33        Redefine set#


1 "Objets multiples"

2 "Dossier en cours"

3 "Déplacement d'objet - Erreur"

4 "Copie d'objet - Erreur"

5 "Liaison d'objet - Erreur"

6 "L'emplacement cible du déplacement:\n\n\
   %s\n\n\
n'existe pas dans le système de fichiers."

7 "L'emplacement cible de la copie:\n\n\
   %s\n\n\
n'existe pas dans le système de fichiers."

8 "L'emplacement cible de la liaison:\n\n\
   %s\n\n\
n'existe pas dans le système de fichiers."

9 "L'emplacement cible du déplacement:\n\n\
   %s\n\n\
n'est pas un dossier."

10 "L'emplacement cible de la copie:\n\n\
   %s\n\n\
n'est pas un dossier."

11 "L'emplacement cible de la liaison:\n\n\
   %s\n\n\
n'est pas un dossier."

12 "L'objet que vous tentez de déplacer:\n\n\
   %s\n\n\
n'existe pas dans le système de fichiers."

13 "L'un des objets que vous tentez de déplacer:\n\n\
   %s\n\n\
n'existe pas dans le système de fichiers.\n\
Aucun objet ne sera déplacé."

14 "L'objet que vous tentez de copier:\n\n\
   %s\n\n\
n'existe pas dans le système de fichiers."

15 "L'un des objets que vous tentez de copier:\n\n\
   %s\n\n\
n'existe pas dans le système de fichiers.\n\
Aucun objet ne sera copié."

16 "L'objet que vous tentez de lier:\n\n\
   %s\n\n\
n'existe pas dans le système de fichiers."

17 "L'un des objets que vous tentez de lier:\n\n\
   %s\n\n\
n'existe pas dans le système de fichiers.\n\
Aucun objet ne sera lié."

$set 99        Redefine set#

$ ****** Version Information ********  
$ DO NOT TRANSLATE THESE MESSAGES  
$ DO NOT CHANGE THESE MESSAGES  
$ DO NOT LOCALIZE THESE MESSAGES  
$ These messages are used for the version information.  

1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $TOG: dtfile.msg /main/11 1999/09/16 09:44:10 mgreess $"
