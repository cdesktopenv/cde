$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        DtPrint.msg
$ **
$ **  Project:     DT
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for the Dt Print
$ **  library.
$ **
$ **
$ *****************************************************************************
$ **
$ **  (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994 Novell, Inc.
$ **
$ *****************************************************************************
$ **
$ **
$ **  $XConsortium: DtPrint.msg /main/10 1996/11/19 18:29:02 rws $
$ *****************************************************************************
$ *****************************************************************************

$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There MAY be three types of messages in this file:
$ 
$  1. Messages that appear in dialogs or are displayed to the user.
$ 
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$ 
$  2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$  3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ******************************************************************************

$quote "


$set 1
$ *************************************************************
$ ** 							     ** 
$ ** PrintSetupBox messages and labels 			     **
$ ** 							     ** 
$ *************************************************************
$ Warning messages
$ Note: do not localize the chars %s
$ 
$quote

1 Configuration de la ressource workAreaLocation incorrecte.

$ Label values
$ 
2 Imprimantes supplémentaires...
3 Imprimer dans le fichier :
4 Sélectionner le fichier...
5 Annuler
6 Exemplaires :
7 Aide
8 Imprimer
9 Imprimante :
10 Configuration...
11 Imprimante :

$ Warning messages
$ Note: do not localize the chars %s
12 Le nombre de copies est hors limites.
13 Le mode de configuration de l'impression est incorrect.
14 Le mode de configuration de l'impression ne peut pas être modifié.

$ Label values
$ 
15 Info...

$ Warning messages
$ Note: do not localize the chars %s
16 Imprimante incorrecte
17 L'imprimante %s ne se trouve pas sur le serveur d'imprimante %s
18 Impossible de se connecter au serveur d'imprimante %s
19 Le serveur X %s ne prend pas l'impression en charge
20 Impossible de définir une imprimante par défaut
21 Impossible de trouver un serveur d'impression X print pour %s
22 Pas de nom d'imprimante dans l'identificateur : %s
23 La destination de l'impression est incorrecte.
24 Conversion du texte composé impossible.
25 Pas d'argument nécessaire pour la conversion de la ressource.

$ Label values
$ 
26 Sélectionner l'erreur d'imprimante

$ Warning messages
$ Note: do not localize the chars %s
27 Imprimantes introuvables.

$ Label values
$ 
28 pas de description
29 Sélectionner le fichier
30 Information de l'imprimante
31 Format:
32 Modèle de l'imprimante :
33 Imprimantes:
34 Imprimantes supplémentaires
35 Imprimer sur l'imprimante
36 Nom du fichier:
37 Configuration de l'impression - Aide

$set 2
$ *************************************************************
$ ** 							     ** 
$ ** _DtPrint Frame GUI messages			     **
$ ** 							     ** 
$ *************************************************************
$ 
$quote "

$ Label values for Margin Frame
$ 
1 "Marges"
2 "Du haut :"
3 "De droite :"
4 "Du bas :"
5 "De gauche :"
$ 
$ Label values for Header/Footer Frame
$ 
6 "En-têtes et pieds de page"
7 "Supérieur gauche :"
8 "Supérieur droit :"
9 "Inférieur gauche :"
10 "Inférieur droit :"

$set 3
$ *************************************************************
$ ** 							     ** 
$ ** Dt Print Dialog Manager Job Options GUI messages        **
$ ** 							     ** 
$ *************************************************************
$ 
$quote

$ Label values for Margin Frame
1 Envoyer le courrier 
2 Titre de la page bannière :
3 Imprimer les options de commande :
