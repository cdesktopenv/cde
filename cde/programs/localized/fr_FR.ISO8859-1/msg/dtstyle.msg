$ "$XConsortium: dtstyle.msg /main/9 1996/10/19 15:27:29 rws $"  
$ *************************************<+>*************************************  
$ *****************************************************************************  
$ **  
$ **  File:        dtstyle.msg  
$ **  
$ **  Project:     DT Style Manager  
$ **  
$ **  Description:  
$ **  -----------  
$ **  This file is the source for the message catalog for dtstyle  
$ **  
$ **  
$ *****************************************************************************  
$ **  
$ **     (c) Copyright 1990 Hewlett-Packard Company  
$ **     All Rights reserved  
$ **  
$ **  
$ *****************************************************************************  
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
$ 2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).  
$   
$     The localization of these messages is OPTIONAL.  These messages are  
$     identified by the following:  
$   
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE  
$   
$ 3. Messages that should not be localized.    
$   
$     These messages are identified by the following:  
$   
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz  
$   
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****  
$   
$ ******************************************************************************  
$ *************************************<L>*************************************  
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------  
$ * Comment lines begin with a $ except $set which indicates the start of a   
$ * new set.  
$ *  
$ * Do not delete any comments; you may add comments for your use.  
$ *  
$ *   
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------  
$ *************************************<L>*************************************  
$quote "

$set 2
$ Message 1 from set 2 was deleted from the message catalog  
$ _DtMessage 2 is the title of all Information dialog boxes  

2 "Avertissement"
$ _DtMessage 3 is the title of all Error dialog boxes  

3 "Erreur"
$ _DtMessage 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

4 "Cannot communicate with the session\n\
manager... Exiting."
$  MESSAGES 5-6 IN SET 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

5 "Style Manager is already running,\n\
second attempt aborted."

6 "An X Toolkit error occurred...\n\
Exiting."
$ _DtMessage 7 is the title of all Help dialog boxes  

7 "Aide"
$ _DtMessage 8 is an error message that appears when you are using Itemh Help  
$ and select on something outside of the Style Manager  

8 "Vous devez sélectionner un objet\n\
dans le Gestionnaire de configuration."
$ _DtMessage 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

9 "Could not obtain screen saver information\n\
from the session manager. Start up settings\n\
may be incorrect."

$set 4        Redefine set#

$ _DtMessage set 4 is for the Style Manager main window  
$ Messages 6,8-26,34,39 from set 4 were deleted from the message catalog  
$ _DtMessage 1 is the title of the main dtstyle window  

1 "Gestionnaire de configuration"
$ mnemonic and label for the File menubar item.  

2 "F"

3 "Fichier"
$ mnemonic and label for the Help menubar item.  

4 "A"

5 "Aide"
$ _DtMessage 7 is the label for the Exit button in the File pulldown  
$ the mnenomic for the Exit button is set 4 _DtMessage 40  

7 "Quitter"
$ Messages 27-33,35  are the labels on the buttons in the dtstyle main window.  

27 "Couleur"

28 "Police"

29 "Fond"

30 "Clavier"

31 "Souris"

32 "Bip"

33 "Ecran"

35 "Lancement"
$  MESSAGE 36 IN SET 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

36 "No Dtstyle restore information available for current session"
$ _DtMessage 37 is the label on the dtstyle icon  

37 "Configuration"
$ Message 38 is the label on the Window button in the dtstyle main window.  

38 "Fenêtre"
$ _DtMessage 40 is replaced with message 53  
$ 40 E  
$ *  
$ * mnemonic and label for the Help menu items.  
$ *   
$ Messages 41-42 have been replaced with messages 54-55  
$ Overview... menu item  
$ 41 I  
$ 42 Introduction...  
$ Tasks... menu item   

43 "T"
$ Message 44 has been replaced with messages 56  
$ 44 Tasks...  
$ Reference... menu item  
$ 45 R  
$ Message 46 has been replaced with messages 57  
$ 46 Reference...  
$ On Item menu item  
$ Keybaord menu item  

47 "u"

48 "Sur l'élément"
$ Using Help... menu item  

49 "i"
$ Message 50 has been replaced with messages 58  
$ 50 Using Help...   
$ Messages 51-52 have been replaced with messages 59-60  
$ Overview menu item  
$ 51 V  
$ 52 Version...  
$ Message 53 is replaced with message 40  

53 "x"
$ Messages 54-55 are replacements for messages 41-42  

54 "v"

55 "Généralités"
$ Message 56 is a replacement for messages 44  

56 "Tâches"
$ Message 57 is a replacement for messages 46  

57 "Référence"
$ Message 58 is a replacement for messages 50  

58 "Utiliser l'aide"
$ Messages 59-60 are replacements for messages 51-52  

59 "A"

60 "A propos du Gestionnaire de configuration"

$set 5        Redefine set#

$ _DtMessage set 5 is for the Font dialog  
$ Messages 1-6, 7-16 from set 5 were deleted from the message catalog  
$ _DtMessage 17 is the label of the titlebox of the preview font area  

17 "Prévisualiser"
$ _DtMessage 18 appears in a Notice dialog after the user selects a new font  
$ and presses Ok - when the resource *WriteXrdbImmediate is True  

18 "Les modifications apportées aux polices seront\n\
prises en compte dans certaines applications\n\
au prochain lancement. \n\
Dans les autres applications (Gestionnaire \n\
de fichiers, par ex.), les modifications\n\
n'apparaîtront pas à moins que vous ne \n\
quittiez le bureau et vous reconnectiez."
$ _DtMessage 19 appears in a Notice dialog after the user selects a new font  
$ and presses Ok - when the resource *WriteXrdbImmediate is False  

19 "La police sélectionnée sera utilisée\n\
lorsque vous relancerez cette session."
$ _DtMessage 20 and 21 is the text that initially appears in the Font Preview   
$ area displaying sample user and system fonts.  

20 "AaBbCcDdEeFfGg0123456789"

21 "AaBbCcDdEeFfGg0123456789"
$ _DtMessage 22 is the label of the titlebox containing the list of font sizes  

22 "Taille"
$ _DtMessage 23 is the title of the Fonts dialog  

23 "Police"
$ _DtMessage 24 is an informative message used by the Fonts dialog  

24 "La police utilisM-ie actuellement sur le Bureau n'est pas disponible\n\
dans la liste Taille. Si vous sM-ilectionnez et appliquez une nouvelle\n\
police, vous ne pourrez pas revenir M-` la police en cours M-` partir de\n\
la boM-nte de dialogue Gestionnaire de configuration - Police."

$set 6        Redefine set#

$ _DtMessage set 6 is for the Beep dialog  
$ _DtMessage 1,6 from set 6 have been deleted from the message catalog  
$ Messages 2 through 5 are the default buttons and labels of the scales.    

2 "Défaut"

3 "Volume"

4 "Tonalité"

5 "Durée"
$ _DtMessage 7 is the title of the Beep dialog box  

7 "Bip"

$set 7        Redefine set#

$ _DtMessage set 7 is for the Startup dialog  
$ Messages 1-2 from set 7 have been deleted from the message catalog  
$ Messages 3 through 9 are labels of the buttons and text visible in the   
$ Session Startup dialog box.   

3 "A la connexion:"

4 "Reprendre session en cours"

5 "Revenir à la session initiale"

$ Message 6 is obsolete - DO NOT localize message #6 from this set
$ 6 "OBS- Choisir à la déconnexion"

7 "Boîte de confirmation de déconnexion:"

8 "En fonction"

9 "Hors fonction"

10 "Enregistrer la session initiale..."
$ _DtMessage 11 is a warning message when the user pushes Set Home Session...  
$ Notice the spaces before Continue .. the spaces make it centered.  

11 "Votre session initiale sera remplacée\n\
par la session en cours.\n\n\
                  Voulez-vous continuer ?"
$ _DtMessage 12 is the title of the warning dialog box when Set Home Session is   
$ pushed.   

12 "Attention"
$ _DtMessage 13 is an error message  

13 "Cette session a été lancée à partir d'un fichier de lancement\n\
du système X Window. Les paramètres de lancement ne sont pas\n\
valides car la session ne peut pas être restaurée."
$ _DtMessage 14 is the title of the Session Startup dialog box.  

14 "Lancement"
$ _DtMessage 15 is an error message  

15 "Le Gestionnaire de sessions n'est pas en fonction.\n\
Les paramètres de lancement ne sont pas valides."

$ Message 16 is used in the Sessin Startup dialog.  It is the
$   string that appears to right of the Startup dialog icon IF
$   the user selects a display-specfic session at the login screen.
16 "Session spécifique de l'écran :"

$set 8        Redefine set#

$ All of the messages from set 8 have been deleted from the message catalog  

$set 9        Redefine set#

$ _DtMessage set 9 is for the Mouse dialog  
$ Message 7 from set 9 have been deleted from the message catalog  
$ Messages 1-4 are labels of toggle buttons in the Mouse Dialog.   

1 "Gaucher"

2 "Droitier"

3 "Configuration de la souris:"

4 "Bouton 2:"
$ _DtMessage 5 is the label of default button.  

5 "Défaut"
$ Messages 6,8,9 are labels of the scales.  

6 "Double-clic"

8 "Accélération"

9 "Seuil"
$  MESSAGE 10 IN SET 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

10 "ERROR: setting pointer mapping\n"

11 "Transférer"

12 "Ajuster"
$ _DtMessage 13 appears in a Notice dialog after the user selects the   
$ left-handed or right-handed toggle  

13 "Lorsque vous sélectionnez OK, les fonctions des\n\
boutons droit et gauche sont permutées. Pour\n\
connaître la configuration en cours (boutons 1\n\
et 3), consultez le graphique représenté dans\n\
la fenêtre Souris (Test Double-clic)."
$ _DtMessage 14 appears in a Notice dialog after the user selects the   
$ transfer toggle to change the function of the middle mouse button.  

14 "Si vous sélectionnez OK, la fonction 'Transférer' (déplacement)\n\
sera affectée au bouton 2 lors de la prochaine session."
$ double-click time and presses Ok (and the resource   
$ *WriteXrdbImmediate is True)  

15 "La nouvelle définition du double-clic entrera en \n\
vigueur lorsque vous relancerez les applications."
$ _DtMessage 16 appears in a Notice dialog after the user selects a new   
$ double-click time and presses Ok (and the resource   
$ *WriteXrdbImmediate is False)  

16 "La nouvelle définition du double-clic entrera\n\
en vigueur à la prochaine session."
$ _DtMessage 17 is the text below the mouse icon. Notice the two lines.  

17 "Test\n\
Double-clic"
$ _DtMessage 18 is the Mouse Dialog title  

18 "Souris"
$ _DtMessage 19 appears in a Notice dialog after the user selects the   
$ adjust toggle to change the function of the middle mouse button.  

19 "Si vous sélectionnez OK, la fonction 'Ajuster' sera\n\
affectée au bouton 2 lors de la prochaine session.\n\
Cette fonction s'applique au texte et aux sélections\n\
dans les listes. Pour les actions de déplacement,\n\
utilisez le bouton 1."

$set 10        Redefine set#

$ _DtMessage set 10 is for the SaveRestore functionality  
$ Messages 2,4 from set 10 have been deleted from the message catalog  
$  MESSAGE 1,3 IN SET 10 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  
$ Messages 1,3 make up one error message.  Put together, they say   
$ "<filename> <_DtMessage 2> <_DtMessage 1>  

1 "Vérifiez les autorisations associées au fichier."

3 "%s est le fichier qui aurait été utilisé pour sauvegarder votre session.\n"

$set 11        Redefine set#

$ _DtMessage set 11 is for the Backdrop dialog  
$  MESSAGE 1 IN SET 11 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  
$ _DtMessage 1 - could not open a backdrop directory  

1 "Could not open directory %s. "
$ _DtMessage 2 - error opening the backdrop files  

2 "The backdrop portion of the Style Manager\n\
will not operate because there are no backdrop\n\
files available. Check $HOME/.dt/errorlog."
$ _DtMessage 12 is the title of the Backdrop dialog box  

12 "Fond"

$set 12        Redefine set#

$ _DtMessage set 12 is for the Screen dialog  
$ Message 2 from set 12 has been deleted from the message catalog  
$ _DtMessage 1 is the title of the Screen dialog box.   

1 "Ecran"
$ Messages 3-14 are the labels and buttons for the Screen dialog  

3 "Par défaut"

4 "Economiseur d'écran"

5 "Lancer l'économiseur"

6 "Délai pour l'économiseur"

7 "minutes"

8 "Verrouillage de l'écran"
$ Message 9 has been removed   

10 "En fonction"

11 "Hors fonction"

12 "Economiseur d'écran:"

13 "Verrouillage de l'écran:"

14 "Lancer le verrouillage"

15 "Effacement automatique écran"

16 "Veilleuse écran:"

17 "Démarrage mise en veille"

18 "Verrouillage panneau avant"
$ Message 19 has been replaced by message 21  

19 "Utiliser arrière-plan verrouillage"

20 "Temps par arrière-plan"
$ Message 21 is a replacement for message 19  

21 "Utiliser les arrière-plans pour verrouillage"

22 "Verrouillage transparent"

$set 13        Redefine set#

$ _DtMessage set 13 is for the Keyboard dialog  
$ _DtMessage 1,5 from set 13 have been deleted from the message catalog  
$ _DtMessage 2 through 4 are labels of scales, buttons, and text visible in the  
$ Keyboard dialog box.  

2 "Défaut"

3 "Répétition automatique"

4 "Volume du clic"
$ _DtMessage 6 is the title of the Keyboard dialog box  

6 "Clavier"

$set 14        Redefine set#

$ _DtMessage set 14 is for the Color dialog  
$ _DtMessage 1-4,9,14-15,17,19,20,22,24 from set 14 have been deleted from the message catalog  
$ Messages 5 through 8 are labels and buttons visible in the Color dialog  

5 "Palettes"

6 "Ajouter..."

7 "Supprimer..."

8 "Modifier..."
$ _DtMessage 10 is the title above the text area in the Add Palette dialog box  

10 "Nouveau nom de palette:"
$ _DtMessage 11 is the title of the Add Palette dialog box.  

11 "Ajouter la palette"
$ Messages 12 through 14 are error messages.  

12 "Le nom de la palette ne peut pas contenir\n\
les caractères suivants:\n\n\
<espace> * : ( ) [ ] { } < > ! | \" / \"$ LOCALIZERS: FOR 8 AND 16 BIT LANGUAGES YOU MAY WANT TO CHANGE THE MESSAGE "
$ TO SAY THE PALETTE NAME MUST BE 10 BYTES OR LESS  

13 "Le nom de la palette est limité\n\
à 10 caractères.\n"
$ _DtMessage 16 is the title of the Delete Palette dialog box.  See screen 4a (2).  

16 "Supprimer la palette"
$ _DtMessage 18 appears in an information dialog.  

18 "Impossible de supprimer la dernière palette.\n"
$ _DtMessage 21 is an error message  

21 "Attention"
$ _DtMessage 23 appears in an information dialog  

23 "Supprimer la palette '%s' ?\n"
$ _DtMessage 25 is an error message that appears when you press the color  
$ button and the color server is not running for some reason.  

25 "La fonction couleur du Gestionnaire de configuration\n\
ne sera pas en vigueur car le serveur des couleurs\n\
n'est pas en cours d'exécution. Vérifiez $HOME/.dt/errorlog."
$ _DtMessage 26 can be caused by setting the resource *useColorObj to False  
$ and then running dtstyle and trying to post the Color Dialog.  

26 "La fonction couleur du Gestionnaire de configuration\n\
ne sera pas en vigueur car la ressource\n\
'useColorObj' est définie à False."
$ _DtMessage 27 is for the new ColorUse dialog when you press ok.  

27 "La nouvelle couleur prendra effet\n\
à la prochaine session."
$ _DtMessage 28 is a warning message that appears on Black and White systems  

28 "La palette sélectionnée prendra effet\n\
à la prochaine session."
$ _DtMessage 29 appears in a warning dialog  

29 "Il existe déjà une palette du nom de '%s'.\n\
Elle sera remplacée par la nouvelle palette.\n\
Voulez-vous continuer ?"
$ _DtMessage 30 is the title of the Color dialog box.  

30 "Couleur"
$ _DtMessages 31-33 are replaced by Dtmessages 41-43  
$ _DtMessage 31-33 are the ColorUse options  
$ 31 High Color  
$ 32 Medium Color  
$ 33 Low Color  

34 "Noir et blanc"

35 "Défaut"
$ Dt Message 36 has been replaced by message 39  
$ _DtMessage 36 is the title of the Color Use dialog  
$ 36 DT Color Use  
$ Dt Message 37 has been replaced by message 40  
$ _DtMessage 37 is the Color Use button in the main Color Dialog   
$ 37 DT Color Use...  
$ _DtMessage 38 is an error message that appears when you press the color  
$ button and there are no palette files to be found  

38 "La fonction couleur du Gestionnaire de configuration\n\
ne sera pas en vigueur car aucun fichier de palettes\n\
n'est disponible. Vérifiez $HOME/.dt/errorlog."
$ DtMessage 39 is a replacement for DtMessage 36  
$ DtMessage 39 is the title of the Color Use dialog  

39 "Nombre de couleurs à utiliser:"
$ DtMessage 40 is a replacement for DtMessage 37  
$ DtMessage 40 is the title of the Color Use dialog  

40 "Nombre de couleurs..."
$ _DtMessages 41-43 are replacements for Dtmessages 31-33  
$ _DtMessage 41-43 are the ColorUse options  

41 "Autres couleurs pour le bureau"

42 "Autres couleurs pour applications"

43 "Totalité des couleurs pour applications"

$set 15        Redefine set#

$ All of the messages from set 15 have been deleted from the message catalog  

$set 16        Redefine set#

$ _DtMessage set 16 is for the Color dialog  
$ _DtMessage 1-10,13-17 from set 16 have been deleted from the message catalog  
$ _DtMessage 11-12 form one error message that appears in an error dialog  

11 "Attention, impossible de supprimer '%s'.\n"

12 "Pour plus de détails, reportez-vous à $HOME/.dt/errorlog."
$  MESSAGE 18-21 IN SET 16 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

18 "Warning, Too many directories listed in the resource paletteDirectories,\n\
Maximum number is %d. "
$ _DtMessage 19 - could not open a palette directory  

19 "Could not open directory %s. "
$ _DtMessage 20 - error opening the palette file  

20 "Could not open %s. "
$ _DtMessage 21 - error reading a palette file  

21 "%s is an invalid palette file."

$set 17        Redefine set#

$ _DtMessage set 17 is for the Modify Color dialog  
$ Messages 1-3,13,16 from set 17 have been deleted from the message catalog  
$ Messages 4 through 12 are labels, buttons, and text visible in the  
$ Modify Color dialog box  

4 "Exemple de couleur"

5 "Ancienne"

6 "Nouvelle"

7 "Saisir couleur"

8 "Editeur de couleur"

9 "Teinte"
$ We recommend that you don't translate R,G,B which stands for Red, Green,  
$ Blue unless there are standard single letter acronyms in your language.  

10 "R"

11 "V"

12 "B"
$  MESSAGE 14-15 IN SET 17 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

14 "Warning, couldn't grab pointer.\n"

15 "Warning, couldn't grab Keyboard.\n"
$ _DtMessage 16 is the title of the Modify Color dialog box.  

16 "Modification des couleurs"

$set 18        Redefine set#

$ _DtMessage set 18 is for the Window dialog  
$ _DtMessage 1 is the title of the Window dialog box  

1 "Fenêtre"
$ Messages 2 through 5 are labels on the Window dialog.  

2 "Défaut"

3 "Comportement des fenêtres"

4 "Icônes des fenêtres"
$ Messages 5-8 have been replaced by messages 13-16   
$ 5 Focus Follows Mouse  
$ 6 Click In Window For Focus  
$ 7 Raise Window With Focus  
$ 8 Opaque Move  

9 "Dans la boîte d'icônes"

10 "Position sur l'espace de travail"

11 "Passage des fenêtres principales en haut de pile"

12 "Vous devez relancer le Gestionnaire de l'espace de \n\
travail pour que les modifications soient prises\n\
en compte.\n\
Appuyez sur OK pour relancer le Gestionnaire\n\
ou sur Annul. pour restaurer les valeurs initiales."
$ Messages 13-16 are replacements for messages 5-8  

13 "Activation au pointeur"

14 "Activation à la sélection (clic)"

15 "Passage fenêtre premier plan si activée"

16 "Affichage contenu durant déplacement"


$set 19
$ The set 19 contains messages for the I18N dialog

$ 
$ Messsages 1 to 11 are labels displayed in the dialog.
$ 

$quote

$ Message 1 is the title of the Dialog.
1 Gestionnaire de style - localisation

$ Message 2 appears on the button used to reset the defailt values.
2 Par défaut

$ Message 3 appears as the title of the title box containing Input
$ Methods information.
3 Méthode d'entrée

$ Message 4 appears as the title of the title box containing Input
$ Method Start Mode choice.
4 Mode de lancement de la méthode d'entrée

$ Message 5 appears as the title of the title box containing PreeditType
$ information.
5 Type de pré-édition 

$ Message 6 is the label presenting the server host names
6 Hôte serveur :

$ Message 7 is the label presenting the Input Method list
7 Méthode d'entrée

$ Message 8 and 9 are the toggle buttons for the start mode selection:
$ "Ask at login" will cause dtimsstart to present the list of Input
$ Methods at next login; "Resume Current Input Method" will cause
$ dtimsstart to use the selected IM at next login, without asking the
$ user.
8 Demander à la connexion

9 Continuer la méthode d'entrée en cours 

$ Message 10 is the label displayed on the button used to move up a
$ selected element in the preedit Type list (meaning at the previous
$ position).
10 Haut

$ Message 11 is the label displayed on the button used to move down a
$ selected element in the preedit Type list (meaning at the next
$ position).
11 Bas

$ 
$ Messages 20-21 are displayed in an warning dialog when the user
$ hits the OK button.
$ 

$ Message 20 appears when the user modifies the XmNpreeditType
$ resource value, and the resource *writeXrdbImmediate is True.
$ Do NOT localize the word XmNpreeditType.
20 La nouvelle valeur de XmNpreeditType sera appliquée \n\
au démarrage des applications.

$ Message 21 appears when the userselects a new Input Method Server,
$ or modifies the IMS Start Mode, or modifies the XmNpreeditType
$ resource value, and the resource *writeXrdbImmediate is False.
21 Votre sélection sera appliquée\n\
lors de la session suivante.

$ 
$ Messages 30 to 41 appear as errors in Error Dialog.
$ 

$ Do NOT localize the word HOME.
30 La variable d'environnement HOME n'est pas définie.

$ Do NOT localize the word LANG.
31 La variable d'environnement LANG n'est pas définie.

32 Cet environnement de pays n'est pas pris en charge par le bureau.

33 Impossible de créer le fichier de sélection de méthode d'entrée de l'utilisateur.

34 Le fichier de sélection de méthode d'entrée de l'utilisateur est introuvable.

35 Impossible de sauvegarder le fichier de sélection de méthode d'entrée de l'utilisateur

36 Hôte inconnu

$ Do NOT translate dtimsstart.
37 Impossible de trouver dtimsstart.\n\
L'environnement du bureau est peut-être\n\
mal installé.

38 L'exécution éloignée a échoué\n sur l'hôte sélectionné.

39 Impossible d'initialiser l'exécution éloignée.

40 Impossible d'obtenir l'information de la méthode d'entrée à partir de l'hôte sélectionné.

41 Erreur inconnue



$set 99        Redefine set#

$    DO NOT TRANSLATE THESE MESSAGES  
$    DO NOT CHANGE THESE MESSAGES  
$    DO NOT LOCALIZE THESE MESSAGES  
$    These messages are used for the version information.  

$quote "
1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $XConsortium: dtstyle.msg /main/9 1996/10/19 15:27:29 rws $"
