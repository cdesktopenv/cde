$ $TOG: dtpad.msg /main/21 1999/09/15 14:20:56 mgreess $
$ ***********************************<+>*************************************
$ ***************************************************************************
$ **
$ **  File:        dtpad.msg
$ **
$ **  Project:     DT Text Editor client (sometimes called dtpad)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtpad
$ **
$ **
$ ***************************************************************************
$ **
$ **     (c) Copyright 1992, 1993 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ ***************************************************************************
$ **
$ ***************************************************************************
$ **  (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994 Novell, Inc.
$ ***************************************************************************
$ **
$quote "

$set 2
$ **********************************************
$ **  General messages used in multiple files **
$ **********************************************
$ Label used for all "Help" buttons in all DIALOGS.
$ It is not used for the Help menu.

1 "Aide"

$quote

$ Unrecognized arguments
2 dtpad: option illégale: %s 
$ Extra arguments
3 dtpad: argument supplémentaire: %s


$set 4        Redefine set#

$ *****************************************************************
$ **                                                    fileCB.c **
$ ** These are the strings which change between the Open dialog  **
$ ** and the Include dialog.  These two operations use the same  **
$ ** Motif dialog, and simply change these two labels.           **
$ *****************************************************************
$ Title of the Open dialog

$quote "
1 "Ouvrir un fichier"
$ label for the File text field in the file selection box
$ ***UNUSED***\tThis label is no longer used and need not be localized.

2 "Nom du fichier à ouvrir:"
$ Title of the Include dialog

3 "Inclure un fichier"
$ Label for the File text field in the file selection box
$ ***UNUSED***\tThis label is no longer used and need not be localized.

4 "Nom du fichier à inclure:"
$ Error when printing a file and a temporary copy can not be saved.

5 "Impossible de créer un fichier temporaire."

$set 5        Redefine set#

$ ***************************************************************************
$ **                                                               dtpad.c **
$ **                                                             fileDlg.c **
$ **                                                                       **
$ ** Messages and strings used in dialogs for all operations involving the **
$ ** "File" menu items.                                                    **
$ ***************************************************************************
$ These compose the dialog which appears if the user attempts to exit, open
$ a different file, drop a file, or start a "New" session with unsaved
$ changes to the current file.
$ This is used to build: "Save changes to FILENAME?"

1 "Sauvegarder les modifications dans %s ?"
$ This is used as-is, whenever the current file is unnamed.

2 "Le fichier en cours n'a pas de nom.\n\
Voulez-vous le sauvegarder ?"
$ This is used to build a message like "Do you want to save the current
$ copy of FILENAME before you Edit NEWFILENAME?" when a new file is droped
$ on the edit window.
$ ***UNUSED***\tCurrently, dropped files are INSERTED in the text rather than
$ \topened for editing so this message is no longer used and need
$ \tnot be localized.

3 "Voulez-vous sauvegarder la\n\
version en cours de %s\n\
avant d'éditer %s ?"
$ Used when about to write over an existing file.
$ The "%s" is replaced by the file name.

5 "Le fichier %s existe déjà.\n\
Voulez-vous remplacer son contenu par celui\n\
de la présente session d'édition ?"
$ Title of "file already exists" dialog.  Also used for other "Warning" type
$ dialogs.

6 "Attention"
$ Title of "Save As" dialog.

7 "Sauvegarder sous"
$ Labels for add/don't-add newlines radio buttons in the Save As dialog
$ ***UNUSED***\tThese labels are no longer used and need not be localized.
$ \tThey've been replaced by messages 44 and 45

8 "Ajout de caractères de nouvelle ligne à la fin des lignes\n\
avec retour automatique."

9 "Aucun ajout de caractères de nouvelle ligne. Seuls les changements\n\
de ligne effectués à l'aide de [Retour] seront conservés."
$ Labels for the Save Warning dialog. Also used in the confirmation, or
$ Editor Return dialog.

14 "Non"

15 "Oui"
$ Title of the dialog - used when user selects "New", "Open", or "Exit" with
$ unsaved changes in the text.

16 "Sauvegarder ?"
$ Title of general error dialog

17 "Erreur"
$ Title of the resource-specified "confirmation" dialog
$ This dialog is generally specified by programs such as mailers, which
$ want to know if the user wants to send the message.
$ There is no default message for the dialog, since it is _only_ posted if
$ the message string is specified via a resource (dtpad*confirmationString:)
$ This message is only used if the "dtpad*mainTitle:" resource is not set.
$ If this resource _is_ set, the the mainTitle is also used as the title
$ for the exit confirmation dialog.
$ DO NOT LOCALIZE - the user should never see this message

18 "Confirmation de sortie"
$ Title for general information dialog. e.g. Find dialog can't find word in
$ file.

20 "Informations"
$ Default file name in Save As dialog box.

21 "SANS TITRE"
$ Error message when a file can not be saved.  The "%1$s" gets replaced by
$ the file name and the "%2$s" gets replaced by the reason why the file could
$ not be saved (messages 31-39).

30 "Impossible de sauvegarder %1$s.\n\
%2$s"
$ Messages 31-39 are reasons why a file could not be saved.
$ Messages 34-39 are substitued in message 30 for "%2$s".

31 "Impossible de sauvegarder le fichier.\n\
Nom de fichier non indiqué."

34 "Ce fichier n'est pas accessible en écriture."

35 "Le fichier est un répertoire."

36 "Le fichier est une unité spéciale à caractères."

37 "Le fichier est une unité en mode bloc."
$ ***UNUSED***\tThis message should never appear since currently there is
$ \tonly one supported file format - text.  And this type is
$ \thard coded into dtpad.

38 "Format de fichier incorrect."
$ This is a "catch all" when the file can not be saved and none of the above
$ errors apply.

39 "Raison inconnue - Espace disque peut-être insuffisant."
$ This is displayed when a ToolTalk edit request on a buffer is received,
$ a Save operation is done and there are unsaved changes.

40 "Sauvegarder les modifications?"
$ This is displayed when a ToolTalk edit request on a file or buffer is
$ received with a document name specified in the request, a Save operation
$ is done and there are unsaved changes.  The "%s" in the message is
$ replaced by the document name (instead of the filename).

41 "Sauvegarder les modifications apportées à \"%s\"?"
$ Title of "Copy As" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 108 of set 11.
$ ***UNUSED***\tThis title is no longer used and need not be localized.

42 "Copier sous"
$ Title of "Copy To File" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 110 of set 11.

43 "Copier dans fichier"
$ Labels for add/don't-add newlines radio buttons in the SaveAs and Save dialogs

44 "Ajouter caractères nouvelle ligne à la fin des lignes\n\
avec retour à la ligne automatique."

45 "Ne pas ajouter de nouvelles lignes. Seuls les passages à la ligne\n\
à l'aide de la touche Entrée seront préservés."
$ 
$ New message as of 5/31/95 (i.e. new since Sample Implementation). This
$ new message is in response to bug DTS CISlx19530.
$ 
$quote
46 Le fichier n'est pas accessible en écriture ou le chemin au fichier\nn'existe pas.

$ 
$ "-NEW"
$ 
47 "Operation failed due to insufficient memory.\nTry increasing swap space."

$ 
$ "-NEW"
$ 
48 "Format operation failed."


$set 6        Redefine set#

$ ************************************************************************
$ **                                                           fileIo.c **
$ **                                                                    **
$ ** Error messages generated when there are problems reading a file.   **
$ ************************************************************************

$quote "
1 "Ce fichier est déjà ouvert pour lecture."

2 "Ce fichier n'existe pas."

3 "Impossible d'éditer un répertoire."

4 "Impossible d'éditer un fichier unité spécial à caractères."

5 "Impossible d'éditer un fichier unité en mode bloc."

$quote
6 Zéros imbriqués supprimés du fichier.

7 Impossible de charger le fichier (mémoire insuffisante).

8 Impossible d'accéder au fichier (impossible de déterminer son état).

9 Impossible de lire le fichier (sans doute pas d'autorisation en lecture).
$ Message placed in the status line message area based on the -viewOnly  
$ resource and whether or not the file being edited is read only  

$quote "
10 "Visualiser uniquement."

11 "Lecture seulement."

$quote
12 Impossible de lire le fichier (raison inconnue).


$set 7        Redefine set#

$ ************************************
$ **                         main.c **
$ **                        dtpad.c **
$ ** Prefix string for all dialogs. **
$ ************************************
$ bogus?? blank string gotten but otherwise unused in main()
$ DO NOT LOCALIZE string 1

$quote "
1 ""
$ The name used in the title bar of every window.
$ Be sure to end this message with a space character.

2 "Editeur de texte - "
$ Default file name used in the main window title.
$ ***UNUSED***\tThis mesage has been replaced by message 6 of this set and
$ \tneed not be localized.

3 " (sans nom)"
$ Terminal error message displayed to stdout when the desk top services
$ can not be initialized (via DtInitialize())

4 "Impossible d'initialiser les fonctions de bureau ... Sortie de l'éditeur texte."
$ Terminal error message displayed to stdout when a connection to the
$ display can not be opend (via XtDisplay())

5 "Impossible d'ouvrir l'écran ... sortie de l'éditeur de texte."
$ Default file name used in the main window title.
$ This should correspond to the default file name used in the "Save As"
$ dialog box - specified in message 21 of set 5.

6 "(SANS TITRE)"


$ 
$ New messages (7,8) since CDE/SI - error gets displayed when user tries to run
$ dtpad -server when and dtpad server process is already running on the
$ $DISPLAY.
$quote
7 ERREUR: Un serveur dtpad sur %s\n\
utilise déjà cet écran.
8 ERREUR: Un serveur dtpad utilise\n\
 déjà cet écran.
$ 
$ Message 9 is also new since CDE/SI - new as of 5/22/95.  It is a usage message
$ for dtpad. Don't translate the '-OPTION' just the text explaining the
$ option.
$ 
9 '%s' est une option incorrecte.\n\
Syntaxe: %s...\n\
   -saveOnClose\n\
\tDtpad sauvegarde automatiquement et silencieusement le texte en cours à\n\
\tla fermeture de l'Editeur de texte si des modifications ont été effectuées.\n\n\
   -missingFileWarning\n\
\tAffiche un avertissement lorsque le fichier indiqué\n\
\tn'existe pas ou est inaccessible.\n\n\
   -noReadOnlyWarning\n\
\tDésactive l'affichage d'un avertissement lorsque l'utilisateur\n\
\tne dispose pas d'une autorisation d'accès en écriture pour le fichier indiqué.\n\n\
   -noNameChange\n\
\tIndique que le nom de fichier par défaut associé au\n\
\ttexte courant ne doit pas être modifié lorsque le texte est\n\
\tsauvegardé sous un nom différent de celui de son ouverture.\n\n\
   -viewOnly\n\
\tInterdit l'édition du texte dans la fenêtre d'édition,\n\
\tprincipalement en transformant l'Editeur de texte en Afficheur.\n\n\
   -statusLine\n\
\tAffiche une ligne d'état au bas de la fenêtre d'édition. La\n\
\tligne d'état affiche le numéro de la ligne du texte sur laquelle\n\
\tse trouve actuellement le curseur.\n\n\
   -wrapToFit\n\
\tActive le mode retour à la ligne.\n\n\
   -workspaceList WorkspaceList\n\
\tAffiche la fenêtre d'édition pour l'invocation courante de\n\
\tl'Editeur de texte dans l'espace ou les espaces de travail spécifiés.\n\n\
   -session SessionFile\n

10 \tRestaure les fenêtres et paramètres de l'Editeur de texte\n\
\ten vigueur lors d'une précédente fermeture de CDE. Toutes les\n\
\tautres options de la ligne de commande sont ignorées lorsque\n\
\tcette option est indiquée.\n\n\
   -standAlone\n\
\tOblige l'invocation courante de l'Editeur de texte à\n\
\teffectuer son propre traitement de texte dans sa fenêtre,\n\
\tindépendamment du serveur de l'Editeur de texte.\n\n\
   -noBlocking\n\
\tAchève le traitement du demandeur de l'Editeur de texte dès\n\
\tque le serveur de l'Editeur de texte détermine qu'il peut gérer\n\
\tla demande d'édition.\n\n\
   -server\n\
\tForce le démarrage d'un serveur d'Editeur de texte (s'il n'y en\n\
\ta pas déjà un qui fonctionne) pour traiter toutes les demandes\n\
\td'édition ultérieures pour l'écran.\n\n\
   -exitOnLastClose\n\
\tIndique que le traitement du serveur de l'Editeur de texte\n\
\tdoit s'arrêter à la fermeture de la dernière fenêtre d'édition de\n\
\tl'écran. Ne doit pas être utilisée avec l'option -server car elle ne\n\
\ts'applique qu'au traitement du serveur.\n\n\
Voir la page dtpad de l'aide pour plus de détails sur ces options.\n\n
$ 

$ "NEW-"
11 "Exiting text editor ..."
$ "NEW-"
12 "Running in standalone mode ..."
$ "NEW-"
13 "Could not connect to ToolTalk service:\n%s\n%s\n"

$set 10        Redefine set#

$ ************************************************************************
$ **                                                          session.c **
$ ** THESE MESSAGES ARE _ONLY_ WRITTEN TO THE ERRORLOG FILE.
$ **
$ ************************************************************************
$ Strings used to build error messages issued when the session save
$ file cannot be read or written.

$quote "
1 "Vérifiez les autorisations associées au fichier."

2 "Le fichier %s est le fichier de sauvegarde prévu pour votre session."

$set 11        Redefine set#

$ ***********************************************************
$ **                                              dtpad.c **
$ **                                                       **
$ ** These are all of the menu button labels and mnemonics **
$ ***********************************************************
$ ***UNUSED*** \tMessages 1-14 are no longer used and need not be localized.
$ \tAll of the mnemonics and labels relating to the "Help" menu
$ \thave been replaced by messages 81-96.
$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu

1 "A"

2 "Aide"
$ mnemonic and label for the "Overview" item

3 "i"

4 "Généralités..."
$ mnemonic and label for the "Tasks" item

5 "T"

6 "Tâches..."
$ nmemonic and label for the "Table of Contents" item are message nos. 71 & 72
$ mnemonic and label for the "Reference" item

7 "R"

8 "Référence..."
$ mnemonic and label for the "On Item" item

9 "u"

10 "Sur l'élément"
$ mnemonic and label for "Using Help" item

11 "d"

12 "Aide sur l'aide..."
$ mnemonic and label for "About Text Editor" item

13 "t"

14 "A propos de l'éditeur de texte"
$ *
$ * Mnemonics and labels for the "File" pulldown menu and items.
$ *
$ mnemonic and label for the "File" pulldown menu

15 "F"

16 "Fichier"
$ mnemonic and label for the "New" item

17 "N"

18 "Nouveau"
$ mnemonic and label for the "Open" item

19 "O"

20 "Ouvrir..."
$ mnemonic and label for the "Include" item

21 "I"

22 "Inclure..."
$ mnemonic and label for the "Save" item
$ NOTE: The "Save" label can be either Message 24 or message 73

23 "S"

24 "Sauvegarder"
$ mnemonic and label for the "Save As" item

25 "a"

26 "Sauvegarder sous..."
$ mnemonic and label for the "Print" item

27 "p"

28 "Imprimer..."
$ mnemonic and label for the "Close" item

29 "F"

30 "Fermer"
$ *
$ * Mnemonics and labels for the "Edit" pulldown menu and items.
$ *
$ mnemonic and label for the "Edit" pulldown menu

31 "E"

32 "Editer"
$ mnemonic and label for the "Undo" menu item

33 "A"

34 "Annuler"
$ mnemonic and label for the "Cut" menu item

35 "C"

36 "Couper"
$ mnemonic and label for the "Copy" menu item

37 "o"

38 "Copier"
$ mnemonic and label for the "Paste" menu item

39 "l"

40 "Coller"
$ mnemonic and label for the "Clear" menu item

41 "E"

42 "Effacer"
$ mnemonic and label for the "Delete" menu item

43 "S"

44 "Supprimer"
$ ***UNUSED***\tMessages 45-50 are no longer used and need not be localized.
$ \tThey have been replaced by messages 101-106 of this set.
$ mnemonic and label for the "Select All" menu item
$ 45 A
$ 46 Select All
$ mnemonic and label for the "Find/Change" menu item
$ 47 F
$ 48 Find/Change...
$ mnemonic and label for the Check Spelling menu item
$ 49 S
$ 50 Check Spelling...
$ *
$ * Mnemonics and labels for the "Format" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu

51 "m"

52 "Format"
$ mnemonic and label for the "Settings..." menu item

53 "P"

54 "Paramètres..."
$ mnemonic and label for the "Paragraph" menu item

55 "g"

56 "Paragraphe"
$ mnemonic and label for the "All" menu item

57 "o"

58 "Tous"
$ *
$ * Mnemonics and labels for the "Options" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu

59 "O"

60 "Options"
$ mnemonic and label for the "Overstrike" toggle button

61 "B"

62 "Barré"
$ mnemonic and label for the "Word Wrap" toggle button
$ ***UNUSED***\tThis label and mnemonic are no longer used and need not be
$ \tlocalized.  They've been replaced by messages 111 and 112.

63 "R"

64 "Retour ligne"
$ mnemonic and label for the "Status Line" toggle button

65 "L"

66 "Ligne d'état"
$ *
$ * Late additions.
$ *
$ nmemonic and label for the "Table of Contents" item of the "Help" menu
$ ***UNUSED*** \tThis nmemonic and label are no longer used and need not be
$ \tlocalized.  They've been replaced by messages 87 and 88.

71 "T"

72 "Table des matières..."
$ label for the "Save" item in the "File" pulldown menu
$ NOTE: This label is used in place of message 24 when text has been
$       changed but has not been saved

73 "Sauvegarder (requis)"
$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu

81 "d"

82 "Aide"
$ mnemonic and label for the "Overview" item

83 "n"

84 "Généralités"
$ mnemonic and label for the "Tasks" item

85 "c"

86 "Tâches"
$ nmemonic and label for the "Table of Contents" item

87 "m"

88 "Table des matières"
$ mnemonic and label for the "Reference" item

89 "R"

90 "Référence"
$ mnemonic and label for the "On Item" item

91 "S"

92 "Sur l'élément"
$ mnemonic and label for "Using Help" item

93 "A"

94 "Aide sur l'aide"
$ mnemonic and label for "About Text Editor" item

95 "A"

96 "A propos de l'éditeur de texte"
$ *
$ * Changes for "Edit" pulldown menu messages 45-50.
$ *
$ mnemonic and label for the "Select All" menu item

101 "s"

102 "Tout sélectionner"
$ mnemonic and label for the "Find/Change" menu item

103 "R"

104 "Rechercher/Modifier..."
$ mnemonic and label for the Check Spelling menu item

105 "O"

106 "Orthographe..."
$ mnemonic and label for the "Copy As" File menu item
$ (should match the dialog title specified in message 42 of set 5)
$ ***UNUSED***\tThis label is no longer used and need not be localized.

107 "s"

108 "Copie sous..."
$ mnemonic and label for the "Copy To File" File menu item
$ (should match the dialog title specified in message 42 of set 5)

109 "F"

110 "Copie dans fichier..."
$ mnemonic and label for the "Wrap To Fit" toggle button

111 "R"

112 "Retour à la ligne automatique"

$ Label for the "Print" item
113 Imprimer

$ ***********************************************************
$ **                                              helpCB.c **
$ **                                                       **
$ ** Messages and strings associated with online help.     **
$ **                                                       **
$ ***********************************************************

$set 12        Redefine set#

$ Message used for "Help - On Item" if the user clicks outside application

1 "Vous devez sélectionner un objet dans l'Editeur de texte."
$ ***********************************************************
$ **                                        ttMsgSupport.c **
$ **                                                       **
$ ** Messages and strings associated with ToolTalk.        **
$ **                                                       **
$ ***********************************************************

$set 13        Redefine set#


1 "Echec de la sauvegarde du point de sécurité."

2 "Echec de la réponse du point de sécurité."

$ more errors/warnings can
$ be detected while reading in a file via ToolTalk.
$quote
3 Unable to set contents of Text Editor (unknown reason).


$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SETS 14 - 15 WERE ADDED TO SUPPORT CDEnext PRINTING
$ **
$ **	The following set was added to support CDEnext printing based on
$ **	the Xp Print Server.
$ **
$ ****************************************************************************
$ ****************************************************************************

$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SET 14
$ **
$ ****************************************************************************
$ ****************************************************************************
$set 14
$ 
$	Messages 1 - 11 are used in printJob.
$ 
$	This is warning message is displayed when the user has provided
$	a margin specifier in the PrintSetup dialog which cannot be parsed.
$	
$quote "

1 "La syntaxe d'un des indicateurs de la marge \n est incorrecte : \n %s \n %s \n %s \n %s \nVoulez-vous utiliser la valeur par défaut ?"
$ 
$	Messages 2 - 7 are messages for errors which can occur in the
$	process of loading a dtpad document file for printing.	
$ 
2 "Le fichier n'existe pas."
3 "Le fichier spécifié est un répertoire."
4 "Erreur de type de fichier."
5 "Le fichier contient des caractères NULL."
6 "Impossible de charger le fichier (mémoire insuffisante)."
7 "Pas d'autorisation de lecture pour ce fichier"

$	Messages 8 - 11 are format strings used to construct the header
$	and footer strings which appear in the printing output.
$ 
$	Message 8 is the format string used to make a Date header/footer.
$	Message 9 is the format string used to make a DocName header/footer.
$	Message 10 is the format string used to make a Page# header/footer.
$	Message 11 is the format string used to make a UserName header/footer.
8 "Date:  %s"
9 "Document:  %s"
10 "Page %d de %d"
11 "Document pour :  %s"

$ 
$	Messages 12 - 13 are used in printSetup to construct the
$	app-specific printing options for DtMail which will be included
$	in the DtPrintSetupBox.
$ 
$	Message 12 is the string labeling the field in the GUI which
$	contains the name of the document.
$	Message 13 is the label used on the Toggle Button to indicate that
$	lines which would extend beyond the edge of the printed page be 
$	"wrapped" to the next line.
12 "Nom du document : "
13 "Retour à la ligne"


$	Messages 14 - 15 are status messages for the PrintToFile function.
$ 
$	Message 14 indicates that the PrintToFile function was unsuccessful.
$	Message 15 indicates that the PrintToFile function was successful.
$       NOTE:  'Print to File' should be translated as in DtPrint.msg
$               Set 1, Message 3.
14 "Imprimer au fichier' a réussi."
15 "'Imprimer au fichier' terminé."

$          Messages 16 - 19 are obsolete, and have been replaced by 25
$ 
$	Messages 16 - 19 are status messages for the popup PDM function.
$ 
$	Message 16 indicates that there was no pdm deamon running.
$	Message 17 indicates that the pdm deamon was unable to start a pdm.
$	Message 18 indicates that the pdm couldnt connect to the video display.
$	Message 19 indicates that the pdm couldnt connect to the print display.
16 "Pas de PDM dans l'environnement."
17 "Impossible de lancer le PDM."
18 "Pas de connexion autorisée entre PDM et l'écran Vidéo."
19 " Pas de connexion autorisée entre PDM et l'écran d'Impression."

$	Messages 20 - 22 are status messages displayed while printing
$ 
$	Message 20 indicates that the user has asked to close the editor
$		   window.  Dtpad is prevented from doing so until the
$		   active print jobs have completed.
$	Message 21 status message displayed when the print job is started.
$	Message 22 status message displayed as the print job progresses.
20 "Fermeture imminente :  attendez la fin de la tâche..."
21 "Impression %s en cours ..."
22 "Impression %s: page %d de %d ..."

$       Message 23 is used in the construction of a title string for the
$                  Print Setup dialog.
23 "Configuration de l'impression - "

$	Message 24 is used to alert the user to the fact that the X Print
$		   Server cannot complete the print request due to a lack
$		   of resources.
24 "Le serveur d'impression X manque temporairement de ressources."

$       Messages 25 is used to handle errors from the PDM
25	"Erreur du Gestionnaire d'impression (PDM) - configuration a échoué."

$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SET 15
$ **
$ ****************************************************************************
$ ****************************************************************************
$ 
$set 15
$ 
$	Messages 1-5 are used in printSetup.c in the pulldown menu for the
$	header/footer options in the print setup dialog.
$ 
$	Message 1 specifies that the page header/footer field be empty.
$	Message 2 put the Date in the page header/footer field.
$	Message 3 put the Document Name in the page header/footer field.
$	Message 4 put the Page Nmbr msg header in the page header/footer field.
$	Message 5 put the User Name msg header in the page header/footer field.
1 "Vide"
2 "Date"
3 "Document "
4 "Page Numéro"
5 "Nom de l'utilisateur"

$	Messages 6-9 are used to construct error messages informing users
$	that they have input an invalid print page margin specification for 
$ 
6 "La valeur de l'indicateur de la marge du haut est incorrecte :  "
7 " La valeur de l'indicateur de la marge de droite est incorrecte :  "
8 " La valeur de l'indicateur de la marge du bas est incorrecte :  "
9 " La valeur de l'indicateur de la marge de gauche est incorrecte :  "
