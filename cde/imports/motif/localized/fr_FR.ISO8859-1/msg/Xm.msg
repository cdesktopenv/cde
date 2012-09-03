$ $XConsortium: Xm.msg /main/8 1996/10/19 15:28:23 rws $
$ 
$  @OSF_COPYRIGHT@
$  COPYRIGHT NOTICE
$  Copyright (c) 1990, 1991, 1992, 1993 Open Software Foundation, Inc.
$  ALL RIGHTS RESERVED (MOTIF). See the file named COPYRIGHT.MOTIF for
$  the full copyright text.
$ 

$ This message catalog contains messages issued by Motif toolkit library.
$ Consult X-Window systems, OSF/MOTIF Programmer's Guide and OSF/MOTIF
$ Programmer's Reference Manual for technical terms if you have any
$ doubts about their meanings.
$ Do not translate variables such as %s, %d %s ... etc.
$ Do not translate Motif resource name, such as XmNlabelType.
$ Special terms with capital letter(s) should not be translated.
$ "False" and "True" are keywords. Do not translate.
 
$quote " 


$set MS_ArrowButton

MSG_ArrowButton_1000 "Le sens de la flèche est incorrect."
$ MSG_ArrowButton_1000 message is obsolete - DO NOT localize this message.

$set MS_BulletinB

MSG_BulletinB_1000 "La politique de redimensionnement est incorrecte."
$ MSG_BulletinB_1000 message is obsolete - DO NOT localize this message.

$ MSG_BulletinB_0001 "Le style de boîte de dialogue est incorrect."
MSG_BulletinB_0001 "Le style de la boîte de dialogue doit être XmDIALOG_MODELESS."

MSG_BulletinB_1001 "Le type d'ombrage est incorrect."
$ MSG_BulletinB_1001 message is obsolete - DO NOT localize this message.

MSG_BulletinB_1002 "La liste de polices est vide (pas de liste par défaut dans VendorShell)."
$ MSG_BulletinB_1002 message is obsolete - DO NOT localize this message.


$set MS_CascadeB

MSG_CascadeB_1000 "XmCascadeButton doit avoir le type\n\
approprié de parent XmRowColumnWidgetClass."
$ MSG_CascadeB_1000 message is obsolete - DO NOT localize this message.

$ MSG_CascadeB_0001 "Seuls les XmMENU_PULLDOWN XmRowColumnWidgets peuvent être des sous-menus."
MSG_CascadeB_0001 "Seuls les widgets XmRowColumn du type XmMENU_PULLDOWN peuvent être des submenus."
$ please do not translate submenus. 

MSG_CascadeB_0002 "La valeur de XmNmappingDelay doit être supérieure ou égale à 0."

MSG_CascadeB_0000 "XmCascadeButton[Gadget] doit avoir un parent XmRowColumn \n\
dont la valeur de XmNrowColumnType est XmMENU_PULLDOWN, XmMENU_POPUP, \n\
XmMENU_BAR ou XmMENU_OPTION."

MSG_CascadeB_0003 "XtGrabPointer n'a pas abouti."

MSG_RowColText_0024 "XtGrabKeyboard n'a pas abouti."


$set MS_Command

MSG_Command_0000 "Le type de boîte de dialogue doit être XmDIALOG_COMMAND."

MSG_Command_0001 "Le type de fils est incorrect ; il\n\
n'existe pas pour le widget Command."
$ please do not translate Command.

MSG_Command_0002 "XmString NULL ou vide."

MSG_Command_0003 " XmString NULL ou vide passée à XmCommandAppendValue."

MSG_Command_0004 "La valeur de XmNmustMatch est toujours False pour le widget Command."
$ please do not translate False or Command. 

MSG_Command_0005 "La valeur de XmNhistoryMaxItems doit être un entier positif supérieur à zéro."


$set MS_CutPaste

MSG_CutPaste_0000 "XmClipboardStartCopy() doit être appelé avant XmClipboardCopy()."

MSG_CutPaste_0001 "XmClipboardStartCopy() doit être appelé avant XmClipboardEndCopy()."

MSG_CutPaste_0002 "Formats trop nombreux dans XmClipboardCopy()."

MSG_CutPaste_0003 "ClipboardBadDataType"
$ please do not translate ClipboardBadDataType.

MSG_CutPaste_0004 "type de données incorrect"

MSG_CutPaste_0005 "ClipboardCorrupt"
$ please do not translate ClipboardCorrupt.

MSG_CutPaste_0006 "erreur interne - structure de données corrompue"

MSG_CutPaste_0007 "ClipboardBadFormat"
$ please do not translate ClipboardBadFormat

MSG_CutPaste_0008 "Erreur - la longueur du format enregistré doit être 8, 16 ou 32 "

MSG_CutPaste_0009 "Erreur - le nom du format enregistré ne doit pas être nul"


$set MS_DialogS

MSG_DialogS_0000 "Le widget DialogShell ne prend en charge qu'un fils de RectObj."
$ please do not translate DialogShell and RectObj.

MSG_DialogS_1000 "Les gadgets ne sont pas autorisés dans le shell."
$ MSG_DialogS_1000 message is obsolete - DO NOT localize this message.


$set MS_DrawingA

MSG_DrawingA_1001 "La largeur ou la hauteur de marge ne peut pas être négative."
$ MSG_DrawingA_1001 message is obsolete - DO NOT localize this message.

MSG_DrawingA_1002 "La politique de redimensionnement est incorrecte."
$ MSG_DrawingA_1002 message is obsolete - DO NOT localize this message.


$set MS_Form

MSG_Form_0000 "La base de la fraction ne peut pas être égale à zéro."

MSG_Form_1000 "Type de connexion incorrect."
$ MSG_Form_1000 message is obsolete - DO NOT localize this message.

MSG_Form_1001 "Impossible de définir les contraintes d'un widget non redimensionnable."
$ MSG_Form_1001 message is obsolete - DO NOT localize this message.

MSG_Form_1002 "Le widget Attachment ne doit pas être nul."
$ MSG_Form_1002 message is obsolete - DO NOT localize this message.
$ please do not translate Attachment

MSG_Form_0002 "Dépendance circulaire des enfants dans le widget Form.\n\
Vérifiez les connexions circulaires entre les enfants."
$ please do not translate Form. 

MSG_Form_1006 "Un bord est connecté à un widget, mais aucun widget n'a été specifié."
$ MSG_Form_1006 message is obsolete - DO NOT localize this message.

MSG_Form_0003 "L'alignement du bord est abandonné après 10000 itérations.\n\
Vérifiez les contraintes contradictoires des enfants de ce widget Form."
$ please do not translate Form.

MSG_Form_0004 "Le widget de connexion doit avoir le même père que le widget."
$ please do not translate Attachment
$ MSG_Form_0004 message is obsolete - DO NOT localize this message.


$set MS_Frame

MSG_Frame_1000 "Un seul fils peut être inséré dans un Frame widget."
$ please do not translate Frame.
$ MSG_Frame_1000 message is obsolete - DO NOT localize this message.

MSG_Frame_1001 "La largeur de marge est incorrecte."
$ MSG_Frame_1001 message is obsolete - DO NOT localize this message.

MSG_Frame_1002 "La hauteur de marge est incorrecte."
$ MSG_Frame_1002 message is obsolete - DO NOT localize this message.


$set MS_Gadget

MSG_Gadget_1000 "L'épaisseur de mise en évidence est incorrecte."
$ MSG_Gadget_1000 message is obsolete - DO NOT localize this message.

MSG_Gadget_1001 "Le type d'unité est incorrect."
$ MSG_Gadget_1001 message is obsolete - DO NOT localize this message.

MSG_Gadget_1002 "L'épaisseur d'ombrage est incorrecte."
$ MSG_Gadget_1002 message is obsolete - DO NOT localize this message.

MSG_Gadget_1003 "La ressource pixmap doit être définie."
$ MSG_Gadget_1003 message is obsolete - DO NOT localize this message.

MSG_Gadget_0000 "Impossible de modifier XmNlayoutDirection après l'initialisation."


$set MS_Label

MSG_Label_1001 "XmNlabeType est incorrect."
$ MSG_Label_1001 message is obsolete - DO NOT localize this message.

MSG_Label_1002 "Valeur incorrecte dans XmNalignment."
$ MSG_Label_1002 message is obsolete - DO NOT localize this message.

MSG_Label_1003 "Valeur incorrecte dans XmNstringDirection."
$ MSG_Label_1003 message is obsolete - DO NOT localize this message.

MSG_Label_0003 "XmNlabelString doit être Compound String."
$ please do not translate Compound String.

MSG_Label_0004 "XmNacceleratorText doit être Compound String."
$ please do not translate Compound String.


$set MS_List

MSG_List_0000 "La valeur modifiée de XmNvisibleItemCount doit être d'au moins 1."

MSG_List_1001 "La politique de sélection est incorrecte."
$ MSG_List_1001 message is obsolete - DO NOT localize this message.

MSG_List_1002 "La politique de dimensionnement est incorrecte."
$ MSG_List_1002 message is obsolete - DO NOT localize this message.

MSG_List_1003 "La politique d'affichage de la barre de défilement est incorrecte."
$ MSG_List_1003 message is obsolete - DO NOT localize this message.

MSG_List_1004 "Le sens de la chaîne est incorrect."
$ MSG_List_1004 message is obsolete - DO NOT localize this message.

MSG_List_0005 "Impossible de modifier XmNlistSizePolicy après l'initialisation."

MSG_List_0006 "La valeur de XmNitemCount modifié doit être positive ou zéro."
$ non-negative means zero or positive.

MSG_List_1007 "La police nulle de SetValues est ignorée."
$ MSG_List_1007 message is obsolete - DO NOT localize this message.
$ please do not translate SetValues

MSG_List_0007 "Les éléments à supprimer n'existent pas sur la liste."

MSG_List_1009 "Pas de barre de défilement horizontal définie."
$ MSG_List_1009 message is obsolete - DO NOT localize this message.

MSG_List_1010 "La définition de marge est incorrecte."
$ MSG_List_1010 message is obsolete - DO NOT localize this message.

MSG_List_0008 "La valeur de XmNlistSpacing ne doit pas être négative."

MSG_List_0009 "Impossible de définir XmNitems à la valeur NULL quand la valeur XmNitemCount est positive."

MSG_List_0010 "La valeur de XmNselectedItemCount ne peut pas être négative."

MSG_List_0011 "Impossible de définir XmNselectedItems à la valeur NULL quand la valeur de XmNselectedItemCount \
est positive."

MSG_List_0012 "XmNtopItemPosition ne peut pas être négative."

MSG_List_0013 "XmNitems et XmNitemCount ne sont pas compatibles."

MSG_List_1017 "Impossible de conserver le mode ajout en sélection multiple."
$ MSG_List_1017 message is obsolete - DO NOT localize this message.

MSG_List_0014 "XmNselectedPositionCount ne peut pas être négative."

MSG_List_0015 "Impossible de définir la valeur NULL pour XmNselectedPositions quand la valeur de  XmNselectedPositionCount \
est positive."


$set MS_MainW

MSG_MainW_0000 "Impossible de définir la valeur nulle pour MenuBar."
$ please do not translate MenuBar.

MSG_MainW_0001 "Impossible de définir la valeur nulle pour la CommandWindow."
$ please do not translate CommandWindow.

MSG_MainW_1003 "La valeur de marge négative est ignorée."
$ MSG_MainW_1003 message is obsolete - DO NOT localize this message.


$set MS_MenuShell

MSG_MenuShell_0000 "Les widgets MenuShell n'acceptent que des enfants XmRowColumn."
$ please do not translate MenuShell.

MSG_MenuShell_0001 "Tentative de gestion d'un menu déroulant qui n'est pas connecté \n\
à un bouton Cascade."

MSG_MenuShell_0002 "XmPopup exige une sous classe de shellWidgetClass"
$ please do not translate shellWidgetClass.

MSG_MenuShell_0003 "XmPopdown exige une sous classe de shellWidgetClass "
$ please do not translate shellWidgetClass.

MSG_MenuShell_0004 "XtMenuPopup exige un seul argument."

MSG_MenuShell_0005 "XtMenuPopup ne prend en charge que des événements ButtonPress, KeyPress ou EnterNotify."
$ please do not translate ButtonPress, KeyPress or EnterNotify.

MSG_MenuShell_0006 "Widget instantané \"%s\" introuvable dans XtMenuPopup."

MSG_MenuShell_0007 " Widget instantané \"%s\" introuvable dans XtMenuPopdown."

MSG_MenuShell_0008 "XtMenuPopdown appelé par plus d'un argument."

MSG_MenuShell_0009 "Impossible de modifier XmNlayoutDirection après l'initialisation."


$set MS_MessageB

MSG_MessageB_1001 "Le type de boîte de dialogue est incorrect."
$ MSG_MessageB_1001 message is obsolete - DO NOT localize this message.

MSG_MessageB_1002 "Le type de bouton par défaut est incorrect."
$ MSG_MessageB_1002 message is obsolete - DO NOT localize this message.

MSG_MessageB_1003 "Le type d'alignement est incorrect."
$ MSG_MessageB_1003 message is obsolete - DO NOT localize this message.

MSG_MessageB_0003 "Type d'enfant incorrect ; cet enfant n'appartient pas au widget."

MSG_MessageB_0004 "Impossible de modifier le bouton Annuler."

MSG_MessageB_1006 "Utilisez XmNdefaultButtonType pour définir le bouton par défaut MessageBox."
$ MSG_MessageB_1006 message is obsolete - DO NOT localize this message.


$set MS_PanedW

MSG_PanedW_0000 "La valeur minimale doit être supérieure à 0."

MSG_PanedW_0001 "La valeur maximale doit être supérieure à 0."

MSG_PanedW_0002 "La valeur minimale doit être inférieure à la valeur maximale."

MSG_PanedW_0003 "Les contraintes ne permettent pas d'effectuer le dimensionnement approprié."
$ MSG_PanedW_0003 message is obsolete - DO NOT localize this message.

MSG_PanedW_0004 "Pas assez de paramètres dans la manette de réglage de rappel."

MSG_PanedW_0005 "Paramètre incorrect dans la manette de réglage de rappel."


$set MS_PWidget

MSG_PWI_1001 "La valeur de fontList n'est pas définie."
$ MSG_PWI_1001 message is obsolete - DO NOT localize this message.
$ please do not translate fontList.


$set MS_Protocols

MSG_Protocols_0000 "Widget doit être VendorShell"
$ please do not translate VendorShell.

MSG_Protocols_0001 "Le gestionnaire de protocole existe déjà."

MSG_Protocols_0002 "Trop de protocoles pour le widget ;\n\
La limite est de 32."


$set MS_PushB

MSG_PushB_1001 "Mémoire insuffisante."
$ MSG_PushB_1001 message is obsolete - DO NOT localize this message.


$set MS_RowColumn

$ In this set, do not translate the word that starts with "XmN" or "Xm". 

MSG_RowColumn_1001 "Tentative de définition de la largeur à 0.\n\
La valeur par défaut (16) est utilisée."
$ MSG_RowColumn_1001 message is obsolete - DO NOT localize this message.

MSG_RowColumn_0000 "Tentative de définition de la largeur à zéro ignorée "

MSG_RowColumn_1003 "Tentative de définition de la hauteur à 0.\n\
La valeur par défaut (16) est utilisée."
$ MSG_RowColumn_1003 message is obsolete - DO NOT localize this message.

MSG_RowColumn_0001 "Tentative de définiition de la hauteur à zéro ignorée "

MSG_RowColumn_0002 "Le Widget XmNhelpWidget est NULL parce qu'il n'est pas utilisé par les menus instantanés."

MSG_RowColumn_0003 "Le Widget XmNhelpWidget est NULL parce qu'il n'est pas utilisé par les menus déroulants."


MSG_RowColumn_0004 "Le Widget XmNhelpWidget est NULL parce qu'il n'est pas utilisé par les menus d'options."


MSG_RowColumn_0005 " Le Widget XmNhelpWidget est NULL parce qu'il n'est pas utilisé par les zones de travail."

MSG_RowColumn_1009 "La valeur de XmNrowColumnType est inconnue ;\n\
WorkArea est utilisée."
$ MSG_RowColumn_1009 message is obsolete - DO NOT localize this message.

MSG_RowColumn_0007 "La hiérarchie du Widget est incorrecte pour ce XmNrowColumnType :\n\
utilisation de XmWORK_AREA par défaut."

MSG_RowColumn_0008 "Tentative de modification de XmNrowColumnType après l'initialisation ignorée"

MSG_RowColumn_1012 "La valeur de XmNorientation est inconnue ;\n\
utilisation de la valeur par défaut."
$ MSG_RowColumn_1012 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1013 "Tentative de définition d'une valeur inconnue pour\n\
XmNorientation. Cette valeur est ignorée."
$ MSG_RowColumn_1013 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1014 "La valeur de XmNpacking est inconnue ;\n\
utilisation de la valeur par défaut."
$ MSG_RowColumn_1014 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1015 "Tentative de définition d'une valeur inconnue pour XmNpacking.\n\
Cette valeur est ignorée."
$ MSG_RowColumn_1015 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1016 "La valeur de XmNentryAlignment est inconnue ;\n\
utilisation de la valeur par défaut."
$ MSG_RowColumn_1016 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1017 "Tentative de définition d'une valeur inconnue pour XmNentryAlignment.\n\
Cette valeur est ignorée."
$ MSG_RowColumn_1017 message is obsolete - DO NOT localize this message.

MSG_RowColumn_0015 "Tentative de définition de XmNisHomogenous à la valeur FALSE pour un widget RowColumn du type \
XmMENU_BAR ignorée."
$ please do not translate FALSE and RowColumn.

MSG_RowColumn_0016 "Tentative de modification de XmNentryClass pour un widget RowColumn du type \
XmMENU_BAR ignorée."
$ please do not translate RowColumn.

MSG_RowColumn_0017 "Tentative de modification de XmNwhichButton via XtSetValues pour un widget RowColumn \
du type XmMENU_PULLDOWN ignorée."
$ please do not translate RowColumn.

MSG_RowColumn_0018 "Tentative de modification de XmNmenuPost via XtSetValues pour un widget RowColumn widget \
du type XmMENU_PULLDOWN ignorée."
$ please do not translate RowColumn.

MSG_RowColumn_0019 "Tentative de définition d'une valeur illégale pour XmNmenuPost ignorée."

MSG_RowColumn_0020 "Tentative de modification de XmNshadowThickness pour un widget RowColumn pas du type \
XmMENU_PULLDOWN ou XmMENU_POPUP ignorée."
$ please do not translate RowColumn.

MSG_RowColumn_1024 "Tentative de modification de XmNorientation pour un\n\
widget RowColumn de type XmMENU_OPTION.\n\
La nouvelle valeur est ignorée."
$ MSG_RowColumn_1024 message is obsolete - DO NOT localize this message.

MSG_RowColumn_0022 "Tentative d'ajout d'un type d'enfant incorrect à un widget de menu (c'est-à-dire XmRowColumn) "

MSG_RowColumn_0023 "Tentative d'ajout d'un type d'enfant incorrect à un widget RowColumn homogène"

MSG_RowColumn_0025 "La tentative de modification de XmNisHomogeneous pour un widget\n\
RowColumn de type XmMENU_OPTION est ignorée."
$ please do not translate RowColumn.

MSG_RowColumn_0026 "Il est déconseillé d'activer le détachement sur un volet de menu partagé "

MSG_RowColumn_0027 "Caractère mnémonique illégal ; Impossible de convertir X KEYSYM en code clé"
$ please do not translate X KEYSYM.


$set MS_Scale

MSG_Scale_0000 "La valeur d'échelle minimale est supérieure ou\n\
égale à la valeur d'échelle maximale."

MSG_Scale_0001 "La valeur d'échelle indiquée est inférieure à\n\
la valeur d'échelle minimale."

MSG_Scale_0002 "La valeur d'échelle indiquée est supérieure à\n\
la valeur d'échelle maximale."

MSG_Scale_1004 "L'orientation est incorrecte."
$ MSG_Scale_1004 message is obsolete - DO NOT localize this message.

MSG_ScaleScrBar_0004 Incorrect processing direction.

MSG_Scale_0005 "Invalid highlight thickness."
$ MSG_Scale_0005 message is obsolete - DO NOT localize this message.

MSG_Scale_0006 "La valeur de XmNscaleMultiple est incorrecte (supérieure à max - min)."

MSG_Scale_0007 "La valeur de XmNscaleMultiple est incorrecte; inférieure à zéro."

MSG_Scale_0008 "(Maximum - minimum) ne peut pas être supérieur à INT_MAX\n\
\t divisé par 2. La valeur minimale a été définie à 0. Il est possible\n\
\t que la valeur maximale ait été définie à (INT_MAX/2)."

MSG_Scale_0009 "La valeur de XmNshowValue est incorrecte"


$set MS_ScrollBar

MSG_ScrollBar_0000 "La valeur minimale de la barre de défilement est\n\
supérieure ou égale à la valeur maximale."

MSG_ScrollBar_0001 "La taille indiquée pour la règle est inférieure à 1."

MSG_ScrollBar_0002 "La valeur indiquée pour la barre de défilement est\n\
inférieure à la valeur minimale autorisée."

MSG_ScrollBar_0003 "La valeur indiquée pour la barre de défilement est supérieure\n\
à la valeur maximale moins la taille de la règle."

MSG_ScrollBar_1005 "L'orientation est incorrecte."
$ MSG_ScrollBar_1005 message is obsolete - DO NOT localize this message.

MSG_ScrollBar_1006 "Le sens de traitement est incorrect."
$ MSG_ScrollBar_1006 message is obsolete - DO NOT localize this message.

MSG_ScrollBar_0004 "L'incrément de la barre de défilement est inférieur à 1."

MSG_ScrollBar_0005 "L'incrément de page de la barre de défilement est inférieur à 1."

MSG_ScrollBar_0006 "Le délai initial de la barre de défilement est inférieur à 1."

MSG_ScrollBar_0007 "Le délai de répétition de la barre de défilement est inférieur à 1."

MSG_ScrollBar_1011 "Erreur liée au gestionnaire de contexte ; la définition\n\
des arrière-plans de barres de défilement est impossible."
$ MSG_ScrollBar_1011 message is obsolete - DO NOT localize this message.

MSG_ScrollBar_1012 "Erreur liée au gestionnaire de contexte ; la définition\n\
         des avant-plans de barres de défilement est impossible."
$ MSG_ScrollBar_1012 message is obsolete - DO NOT localize this message.

MSG_ScrollBar_0008 "La taille de règle indiquée est supérieure à la valeur maximale\n\
de la barre de défilement moins sa valeur minimale."


$set MS_ScrolledW

MSG_ScrolledW_1001 "La politique d'affichage de la barre de défilement est incorrecte."
$ MSG_ScrolledW_1001 message is obsolete - DO NOT localize this message.

MSG_ScrolledW_1002 "La politique de défilement est incorrecte."
$ MSG_ScrolledW_1002 message is obsolete - DO NOT localize this message.

MSG_ScrolledW_1003 "La politique visuelle est incorrecte."
$ MSG_ScrolledW_1003 message is obsolete - DO NOT localize this message.

MSG_ScrolledW_1004 "La politique de positionnement est incorrecte."
$ MSG_ScrolledW_1004 message is obsolete - DO NOT localize this message.

MSG_ScrolledW_0004 "Impossible de modifier la politique de défilement après l'initialisation."

MSG_ScrolledW_0005 "Impossible de modifier la politique visuelle après l'initialisation."

MSG_ScrolledW_0006 "Impossible de définir une politique de défilement AS_NEEDED\n\
lorsque la politique visuelle est VARIABLE."
$ please do not translate AS_NEEDED or VARIABLE.

MSG_ScrolledW_0007 "Impossible de modifier le widget de barre de défilement en mode AUTOMATIC."
$ please do not translate AUTOMATIC.

MSG_ScrolledW_0008 "Impossible de modifier la fenêtre de cadrage."

MSG_ScrolledW_0009 "Impossible de définir une politique visuelle CONSTANT en mode APPLICATION_DEFINED."
$ please do not translate CONSTANT or APPLICATION_DEFINED.

MSG_ScrollVis_0000 "Les paramètres incorrects sont passés à la fonction XmScrollVisible."


$set MS_SelectioB

MSG_SelectioB_1001 "Le type de boîte de dialogue est incorrect."
$ MSG_SelectioB_1001 message is obsolete - DO NOT localize this message.

MSG_SelectioB_0001 "Impossible de modifier le type de boîte de dialogue."
$ please do not translate Dialog.

MSG_SelectioB_1003 "Un seul enfant de zone de travail est autorisé."
$ MSG_SelectioB_1003 message is obsolete - DO NOT localize this message.

MSG_SelectioB_0002 "Le widget ne prend pas en charge ce type d'enfant."


$set MS_Separator

MSG_Separator_1001 "Le type de séparateur est incorrect."
$ MSG_Separator_1001 message is obsolete - DO NOT localize this message.

MSG_Separator_1002 "L'orientation est incorrecte."
$ MSG_Separator_1002 message is obsolete - DO NOT localize this message.


$set MS_Text

MSG_Text_0000 "Le texte source incorrect est ignoré."

MSG_Text_1003 "Le mode d'édition est incorrect."
$ MSG_Text_1003 message is obsolete - DO NOT localize this message.

MSG_Text_0002 "Le widget de texte peut être édité ; la valeur de XmNtraversalOn doit être True."
$ please do not translate XmNtraversalOn and True.
$ MSG_Text_0002 message is obsolete - DO NOT localize this message.

MSG_TextIn_0000 "Impossible de trouver la position tout en essayant de déplacer la ligne précédente."

MSG_TextOut_0000 "Le nombre de lignes doit être supérieur à 0."

MSG_TextF_0002 "XmFontListInitFontContext n'a pas abouti."
$ please do not translate XmFontListInitFontContext.

MSG_TextF_0003 "XmFontListGetNextFont n'a pas abouti."
$ please do not translate XmFontListGetNextFont.

MSG_TextF_0004 " Le caractère '%s' n'est pas pris en charge par la police. Spprimé."

MSG_TextFWcs_0000 " Le caractère '%s' n'est pas pris en charge par la police. Supprimé."

MSG_TextFWcs_0001 "Cannot use multibyte locale without a fontset.  Value discarded."
$ MSG_TextFWcs_0001 message is obsolete - DO NOT localize this message.

$set MS_TextF

MSG_TextF_0000 "La position du curseur doit être supérieure ou égale à 0."

MSG_TextF_0001 "Le nombre de colonnes doit être supérieur à 0."

MSG_TextF_0005 "La valeur de XmNtraversalOn doit toujours être True."
$ please do not translate XmNtraversalOn and True.
$ MSG_TextF_0005 message is obsolete - DO NOT localize this message.

MSG_TextF_0006 "Le nombre de colonnes doit être supérieur ou égal à 0."


$set MS_ToggleB

MSG_ToggleB_1001 "Le type d'indicateur doit être XmONE_OF_MANY ou XmN_OF_MANY."
$ MSG_ToggleB_1001 message is obsolete - DO NOT localize this message.


$set MS_Traversal

MSG_Traversal_1001 "La valeur indiquée pour navigation_type est incorrecte."
$ MSG_Traversal_1001 message is obsolete - DO NOT localize this message.

MSG_Traversal_1002 "Ancienne valeur incorrecte pour navigation_type."
$ MSG_Traversal_1002 message is obsolete - DO NOT localize this message.

MSG_Traversal_1003 "Paramètres incorrects pour l'amorce de traversée."
$ MSG_Traversal_1003 message is obsolete - DO NOT localize this message.

MSG_Traversal_1004 "Paramètres incorrects pour la tentative de passage à la nouvelle tabulation."
$ MSG_Traversal_1004 message is obsolete - DO NOT localize this message.

MSG_Traversal_1005 "startWidget ne figure pas dans la liste fille."
$ MSG_Traversal_1005 message is obsolete - DO NOT localize this message.
$ please do not translate startWidget

MSG_Traversal_1006 "Paramètres incorrects pour TraverseToChild."
$ MSG_Traversal_1006 message is obsolete - DO NOT localize this message.
$ please do not translate TraverseToChild.


$set MS_Vendor

MSG_Vendor_0000 "Valeur incorrecte pour XmNdeleteResponse"

MSG_Vendor_1002 "La valeur de XmNpreeditType est incorrecte ; utilisation de OverTheSpot."
$ MSG_Vendor_1002 message is obsolete - DO NOT localize this message.

MSG_Vendor_0001 "La valeur de XmNinputPolicy est incorrecte."

MSG_Vendor_0002 "Impossible de modifier XmNlayoutDirection."

MSG_Vendor_0003 "Erreur fatale : \n\
_Impossible d'utiliser XmGetDefaultDisplay avant VendorS.Initialize, revenir à NULL"


$set MS_VendorE

MSG_VendorE_1001 "FetchUnitType: classe de widget incorrecte."
$ MSG_VendorE_1001 message is obsolete - DO NOT localize this message.

MSG_VendorE_0000 "String to no-op conversion needs no extra arguments"
$ MSG_VendorE_0000 message is obsolete - DO NOT localize this message.

MSG_VendorE_0005 "FetchUnitType a été appelé sans référence à un widget."
$ please do not translate FetchUnitType.
$ MSG_VendorE_0005 message is obsolete - DO NOT localize this message.


$set MS_VirtKeys

MSG_VirtKey_1001 "L'initialisation des liens virtuels n'a pas été lancée."
$ MSG_VirtKey_1001 message is obsolete - DO NOT localize this message.


$set MS_Visual

MSG_Visual_0000 "La couleur de _XmAccessColorData demandée est incorrecte."

MSG_Visual_0001 "Impossible d'allouer une entrée de carte des couleurs pour le fond par défaut."

MSG_Visual_0002 "Impossible d'analyser la spécification de la couleur du fond par défaut."


$set MS_XmIm

MSG_XmIm_0000 "Impossible d'ouvrir la méthode d'entrée avec XLookupString."

MSG_XmIm_1002 "Impossible de créer l'objet Méthode d'entrée."
$ MSG_XmIm_1002 message is obsolete - DO NOT localize this message.

MSG_XmIm_1003 "XmIMFocus appelé avec un widget nul."
$ MSG_XmIm_1003 message is obsolete - DO NOT localize this message.

MSG_XmIm_1004 "XmIMMove appelé avec la cible Méthode d'entrée."
$ MSG_XmIm_1004 message is obsolete - DO NOT localize this message.

  
$set MS_Resources
  
MSG_Resource_0001 "OK"
$ "OK" as in "Go ahead"/"Confirmed to proceed as instructed"/"Fine"

MSG_Resource_0002 "Annuler"
$ ... as in "Never mind"/"abort this operation"/"Stop current request"

MSG_Resource_0003 "Sélection"
$ ... as in "choosing from a list of items"

MSG_Resource_0004 "Appliquer"
$ ... as in "To do something with the things selected"

MSG_Resource_0005 "Aide"
$ ... as in "Need more guidance on how to deal with this dialog box"

MSG_Resource_0006 "Filtre"
$ ... the noun "Filter", as in "a sifter/a filter/a qualifier to screen out"
$ files with

MSG_Resource_0007 "Fichiers"
$ ... as in "a list of the files"

MSG_Resource_0008 "Répertoires"
$ ... as in "a list of the directories" 

MSG_Resource_0009 "Eléments"
$ ... as in "a list of things to choose from"

MSG_Resource_0010 "Filtrer"
$ ... the verb "Filter", as in "To screen out some files"
$ Note this has a slightly different semantics from MSG_Res_1006.
$ In English, it is the same word, but MSG_Res_1006 means "a sifter", and
$ MSG_Res_1010 means "Push this button to filter out some files"
$ MSG_Res_1006 is a label indicating "entry below is qualifier to filter out" 
$ files with

MSG_Resource_0011 "Répertoire"

MSG_Resource_1012 ">"
$ command-line prompt in the Command widget. 


$set MS_BaseClass

MSG_BaseClass_0000 "Contexte introuvable pour l'extension."

MSG_BaseClass_0001 "_XmPopWidgetExtData; extension introuvable avec XFindContext."

MSG_BaseClass_0002 "La routine XmFreeWidgetExtData n'est pas prise en charge."
$ MSG_BaseClass_0002 message is obsolete - DO NOT localize this message.

MSG_GetSecRes_0000 "getLabelSecResData: mémoire insuffisante."
$ please do not translate getLabelSecResData.
$ MSG_GetSecRes_0000 message is obsolete - DO NOT localize this message.


$set MS_Display

MSG_Display_0001 "Création de plusieurs XmDisplays pour le même écran X.\n\
\tSeul le premier XmDisplay créé pour un écran X donné peut être\n\
\tutilisé dans les appels de XmGetXmDisplay."
$ please do not translate XmDisplay or XmGetXmDisplay

MSG_Display_0002 "TOP_LEVEL_LEAVE a été reçu alors qu'aucun DragContext n'était actif."
$ please do not translate TOP_LEVEL_LEAVE or DragContext

MSG_Display_0003 "Seule une sous-classe de XmDisplay peut être attribuée à XmDisplay."
$ please do not translate XmDisplay


$set MS_DragBS

MSG_DragBS_0000 "_MOTIF_DRAG_WINDOW a été supprimé."
$ please do not translate _MOTIF_DRAG_WINDOW

MSG_DragBS_0001 "Les niveaux de version des protocoles sont incompatibles."

MSG_DragBS_0002 "Impossible d'ouvrir l'écran."

MSG_DragBS_0003 "La table d'atomes est vide."

MSG_DragBS_0004 "La table de cibles est vide."

MSG_DragBS_0005 "La propriété de la table de cibles est incohérente."

MSG_DragBS_0006 "L'index de la table cible est incorrect."


$set MS_DragC

MSG_DragC_0001 "GenerateCallback n'attend pas XmCR_DROP_SITE_ENTER comme condition."
$ please do not translate GenerateCallback or XmCR_DROP_SITE_ENTER

MSG_DragC_0002 "Sélection incorrecte dans DropConvertCallback."
$ please do not translate DropConvertCallback

MSG_DragC_0003 "La sélection à poser à été perdue."

MSG_DragC_0004 "XGrabPointer n'a pas abouti."

MSG_DragC_0005 "ExternalNotifyHandler: la condition d'appel n'est pas recevable."
$ please do not translate ExternalNotifyHandler

MSG_DragC_0006 "XmDragStart doit être appelé en appuyant sur un bouton ou par un événement de déplacement."
$ please do not translate XmDragStart


$set MS_DragICC

MSG_DragICC_0000 "Type de message relatif à une opération de glisser-poser inconnu."

MSG_DragICC_0001 "Les niveaux des versions du protocole sont incompatibles."


$set MS_DragIcon

MSG_DragIcon_0000 "Aucune géometrie indiquée pour le pixmap dragIcon."
$ please do not translate dragIcon

MSG_DragIcon_0001 "dragIcon a été créé sans pixmap."
$ please do not translate dragIcon

MSG_DragIcon_0002 "String to Bitmap converter needs Screen argument"
$ MSG_DragIcon_0002 message is obsolete - DO NOT localize this message.


$set MS_DragOverS

MSG_DragOverS_0000 "Profondeurs incompatibles."

MSG_DragOverS_0001 "Connexion d'icône inconnue."

MSG_DragOverS_0002 "Etat d'opération \"glisser\" inconnue."

MSG_DragOverS_0003 "XmNblendModel inconnu."
$ please do not translate XmNblendModel 


$set MS_DragUnder

MSG_DragUnder_0000 "Impossible d'obtenir la géométrie de fenêtre dropSite."

MSG_DragUnder_0001 "animationPixmapDepth incorrect."
$ please do not translate animationPixmapDepth


$set MS_DropSMgr

MSG_DropSMgr_0001 "Impossible de créer des zones de pose filles de zones de pose simples."

MSG_DropSMgr_0002 "Réception d'événements de mouvement sans contexte pour l'opération de \"glisser\"."

MSG_DropSMgr_0003 "Réception d'une modification sans contexte actif pour l'opération de \"glisser\"."

MSG_DropSMgr_0004 "Création d'un site de pose sans procédure correspondante."

MSG_DropSMgr_0005 "Impossible de définir les rectangles ou les numéros de rectangles de sites de pose composés."

MSG_DropSMgr_0006 "Enregistrement hors séquence d'un widget en\n\
\ttant que site de pose. Les ancêtres doivent être\n\
\tenregistrés avant leurs descendants."

MSG_DropSMgr_0007 "Impossible d'enregistrer plusieurs fois un widget en tant que site de pose."

MSG_DropSMgr_0008 "Le type de DropSite ne peut être défini que lors de la création."
$ please do not translate DropSite

MSG_DropSMgr_0009 "Impossible de modifier les rectangles d'un site de pose composé."

MSG_DropSMgr_0010 "Impossible d'enregistrer Shell en tant que site de pose."

MSG_DropSMgrI_0001 "Impossible d'enregistrer un site de pose descendant d'un site simple."

MSG_DropSMgrI_0002 "Impossible de créer une liste de fils non contiguë pour un site de pose composé."

MSG_DropSMgrI_0003 "%s n'est pas un site de pose fils de %s."


$set MS_GeoUtils

MSG_GeoUtils_0000 "Failure of geometry request to \"almost\" reply"
$ please do not translate "almost"
$ MSG_GeoUtils_0000 message is obsolete - DO NOT localize this message.

MSG_GeoUtils_0001 "Invalid layout of children found"
$ MSG_GeoUtils_0001 message is obsolete - DO NOT localize this message.

MSG_GeoUtils_0002 "Invalid order found in XmSelectionBox"
$ MSG_GeoUtils_0002 message is obsolete - DO NOT localize this message.


$set MS_Region

MSG_Region_0000 "Erreur liée à la mémoire."


$set MS_RepType

MSG_RepType_0001 "L'ID de type de représentation est incorrect."

MSG_RepType_0002 "Valeur incorrecte (%d) pour le type de réponse XmR%s."
$ please do not translate XmR%s

MSG_RepType_0000 "Inverser la conversion de "


$set MS_ResConvert

MSG_ResConvert_0000 "FetchUnitType: classe de widget incorrecte."
$ please do not translate FetchUnitType.
$ MSG_ResConvert_0000 message is obsolete - DO NOT localize this message.

MSG_ResConvert_0001 "La liste par défaut n'est pas définie correctement ! Quitter..."

MSG_ResConvert_0002 "Deux points absents devant la chaîne de police \"%s\"; le reste des polices de la liste ne sont pas analysées."

MSG_ResConvert_0003 "Le séparateur du repère \"%s\" est incorrect ; le reste des polices de la liste ne sont pas analysées."


MSG_ResConvert_0004 "Unmatched quotation marks in string \"%s\"; any remaining fonts in list unparsed"
$ MSG_ResConvert_0004 message is obsolete - DO NOT localize this message.

MSG_ResConvert_0005 "Les guillemets du repère \"%s\" sont incompatibles; le reste des polices de la liste ne sont pas analysées."


MSG_ResConvert_0006 "valeur null du repère null obtenu lors de la conversion au type %s; le reste des polices de la liste ne sont pas analysées."


MSG_ResConvert_0007 "Impossible de convertir XmString en Compound Text"

MSG_ResConvert_0008 "Mémoire insuffisante pour XmbTextListToTextProperty"

MSG_ResConvert_0009 "XmbTextListToTextProperty ne prend pas ne charge l'environnement du pays"

MSG_ResConvert_0010 "XmbTextListToTextProperty a échoué"

MSG_ResConvert_0011 "Impossible de convertir le nom du widget en Widget."
$ please do not translate Widget.

MSG_ResConvert_0012 "Impossible de convertir le texte composé en XmString"

MSG_ResConvert_0013 "Impossible de convertir XmString en texte composé"

MSG_ResConvert_0014 "FetchUnitType called without a widget to reference"
$ please do not translate FetchUnitType.
$ MSG_ResConvert_0014 message is obsolete - DO NOT localize this message

MSG_ResConvert_0015 "FetchDisplayArg called without a widget to reference"
$ please do not translate FetchDisplayArg.
$ MSG_ResConvert_0015 message is obsolete - DO NOT localize this message

MSG_ResConvert_0016 "FetchWidgetArg called without a widget to reference"
$ please do not translate FetchWidgetArg.
$ MSG_ResConvert_0016 message is obsolete - DO NOT localize this message


$set MS_Screen

MSG_Screen_0000 "Ecrans d'icônes incompatibles."

MSG_Screen_0001 "Impossible d'obtenir XmScreen ; XmDisplay est introuvable."


$set MS_ColObj

MSG_ColObj_0001 "Impossible d'allouer la mémoire pour les données d'objets couleur."

MSG_ColObj_0002 "Numéro d'écran incorrect dans la sélection du serveur de couleurs."

 
$set MS_IG
 
MSG_IG_1 "Alignement incorrect."
$ MSG_IG_1 message is obsolete - DO NOT localize this message.

MSG_IG_2 "Comportement incorrect."
$ MSG_IG_2 message is obsolete - DO NOT localize this message.

MSG_IG_3 "Mode de remplissage incorrect."
$ MSG_IG_3 message is obsolete - DO NOT localize this message.

MSG_IG_4 "Position de chaîne ou de pixmap incorrecte."
$ MSG_IG_4 message is obsolete - DO NOT localize this message.

MSG_IG_5 "Largeur ou hauteur de marge incorrecte."
$ MSG_IG_5 message is obsolete - DO NOT localize this message.

MSG_IG_6 "Type d'ombrage incorrect."
$ MSG_IG_6 message is obsolete - DO NOT localize this message.


$set MS_ComboBox

MSG_ComboBox_0000 "Les applications ne peuvent pas ajoputer d'enfants aux widgets XmComboBox."

MSG_ComboBox_0001 "La ressource XmNcomboBoxType ne peut pas être modifiée par XtSetValues."

MSG_ComboBox_0002 "XmFontListGetNextFont failed."
$ MSG_ComboBox_0002 message is obsolete - DO NOT localize this message.

MSG_ComboBox_0003 "XmFontListInitFontContext failed."
$ MSG_ComboBox_0003 message is obsolete - DO NOT localize this message.

MSG_ComboBox_0004 "Le widget interne a été détruit.. Comportement incertain."

MSG_ComboBox_0005 "Le widget interne a été traité incorrectement. Comportement incertain."

MSG_ComboBox_0006 "XmQUICK_NAVIGATE est correct seulement pour ComboBoxes de XmNcomboBoxType XmDROP_DOWN_LIST"

MSG_ComboBox_0007 "Action appelée par un nombre incorrect de paramètres."

MSG_ComboBox_0008 "La routine de l'action a été appelée à partir d'un widget qui n'est pas un descendant de ComboBox"

MSG_ComboBox_0009 "XmComboBoxSelectItem a été appelé par un élé ment qui n'existe pas dans ComboBox."

MSG_ComboBox_0010 "XmComboBoxSetItem a été appelé par un élé ment qui n'existe pas dans ComboBox."

MSG_ComboBox_0011 "XmComboBoxDeletePos a été appelé par une position incorrecte."

MSG_ComboBox_0012 "La routine d'utilitaire XmComboBox a été appelée par un widget incorrect."

MSG_ComboBox_0013 "Les applications ne peuvent pas définir des enfants automatiques de widget XmComboBox."

MSG_ComboBox_0014 "Vous ne pouvez pas modifier XmComboBox positionMode après sa création."
$ please do not translate positionMode.


$set MS_Container

MSG_Container_0000 "L'action a été appelée avec un nombre incorrect de paramètres."

MSG_Container_0001 "XmNdetailColumnHeading et XmNdetailColumnHeadingCount sont incompatibles !"


$set MS_CSText

MSG_CSText_0000 "La hauteur de la marge est incorrecte ; elle doit être égale ou supérieure à 0."
$ MSG_CSText_0000 message is obsolete - DO NOT localize this message.

MSG_CSText_0001 "Invalid margin width; must be greater than or equal to 0."
$ MSG_CSText_0001 message is obsolete - DO NOT localize this message.

MSG_CSText_0002 "Invalid edit mode."
$ MSG_CSText_0002 message is obsolete - DO NOT localize this message.

MSG_CSText_0003 "XmNtraversalOn must always be true."
$ MSG_CSText_0003 message is obsolete - DO NOT localize this message.

MSG_CSText_0004 "Cannot change XmNscrollHorizontal after initialization."
$ MSG_CSText_0004 message is obsolete - DO NOT localize this message.

MSG_CSText_0005 "Cannot change XmNscrollVertical after initialization."
$ MSG_CSText_0005 message is obsolete - DO NOT localize this message.

MSG_CSText_0006 "Cannot change XmNscrollTopSide after initialization."
$ MSG_CSText_0006 message is obsolete - DO NOT localize this message.

MSG_CSText_0007 "Cannot change XmNscrollLeftSide after initialization."
$ MSG_CSText_0007 message is obsolete - DO NOT localize this message.


$set MS_GrabShell

MSG_GrabS_0000 "XmPopup requires a subclass of shellWidgetClass."
$ please do not translate shellWidgetClass.
$ MSG_GrabS_0000 message is obsolete - DO NOT localize this message.


$set MS_Manager

MSG_Manager_0000 "L'enregistrement CompositeClassExtension de la classe de Widget %s est incorrect"
$ please do not translate CompositeClassExtension.

MSG_Manager_0001 "Impossible de modifier XmNlayoutDirection ou XmNstringDirection après l'initialisation"


$set MS_NavigMap

MSG_NavigMap_0000 "_XmNavigate called with invalid direction"
$ MSG_NavigMap_0000 message is obsolete - DO NOT localize this message.


$set MS_Notebook

MSG_Notebook_0000 "La ressource XmNnotebookChildType ne peut pas être définie par la valeur XtSetValues."


$set MS_PixConv

MSG_PixConv_0000 "Le nombre de paramètres   de CvtStringToPixmap est incorrect "
$ please do not translate CvtStringToPixmap.


$set MS_Primitive

MSG_Primitive_0000 "Impossible de modifier XmNlayoutDirection après l'initialisation."


$set MS_ScrollFrameT

MSG_ScrollFrameT_0000 "AssocNavigator exige une caractéristique du navigateur"
$ please do not translate AssocNavigator.

MSG_ScrollFrameT_0001 "DeAssocNavigator exige une caractéristique navigateur"
$ please do not translate DeAssocNavigator.


$set MS_SpinB

MSG_SpinB_0001 "Invalid value for XmNarrowLayout."
$ MSG_SpinB_0001 message is obsolete - DO NOT localize this message.

MSG_SpinB_0002 "XmNminimumValue equals XmNmaximumValue."
$ MSG_SpinB_0002 message is obsolete - DO NOT localize this message.

MSG_SpinB_0003 "Pas d'éléments pour l'enfant XmSTRING."

MSG_SpinB_0004 "La valeur de XmNincrementValue ne peut pas être 0. Utilisez la valeur 1."

MSG_SpinB_0005 "Le sens de la rotation spécifié par XmNincrementValue\n\
a été inversé pour correspondre aux valeurs de\n\
XmNminimumValue et de XmNmaximumValue."

MSG_SpinB_0006 "XmNposition hors limites ; position minimale XmNposition utilisée."

MSG_SpinB_0007 "XmNposition hors limites ; position maximale XmN utilisée."

MSG_SpinB_0008 "La valeur de XmNpositionType est incorrecte. Utilisez la valeur par défaut."

MSG_SpinB_0009 "XmNpositionType resource can only be set at creation time."
$ MSG_SpinB_0009 message is obsolete - DO NOT localize this message.


$set MS_Transfer

MSG_Transfer_0000 "Appeler SelectionCallbackWrapper quand les transferts doivent être terminés "
$ please do not translate SelectionCallbackWrapper.

MSG_Transfer_0001 "Cannot lock the clipboard; aborting transfer"
$ MSG_Transfer_0001 message is obsolete - DO NOT localize this message.

MSG_Transfer_0002 "Le type et le format des données d'appel fournis ne correspondent pas aux données fusionnées."

MSG_Transfer_0003 "L'état de XmConvertCallbackStruct n'est pas XmCONVERT_MERGE."

MSG_Transfer_0004 "CONVERT_MORE n'est pas enore pris en charge."
$ please do not translate CONVERT_MORE.

MSG_Transfer_0005 "Valeur de l'atome incorrecte."

MSG_Transfer_0006 "Avertissement : Tentative de transfert  MULTIPLE lors d'un transfert."
$ please do not translate MULTIPLE.

MSG_Transfer_0007 "Avertissement : Tentative de lancement d'un transfert MULTIPLE alors qu'aucun transfert n'est en cours."
$ please do not translate MULTIPLE.


$set MS_VaSimple

MSG_VaSimple_0000 "La conversion de XtVaTypedArg exige un traitement avec un widget de valeur non null."

MSG_VaSimple_0001 "Impossible de trouver le type de ressource pour la conversion."

MSG_VaSimple_0002 "La conversion du type a échoué."


$set MS_Xm

MSG_Motif_0000 "\nNom : %s\nClasse : %s\n"

MSG_Motif_0001 "L'action a été lancée avec un nombre incorrect de paramètres."


$set MS_XmRenderT

MSG_XmRenderT_0000 "La valeur de XmNtag ne peut pas être NULL. Configurer à chaîne vide."

MSG_XmRenderT_0001 "La valeur Display est NULL. Impossible de charger la police."

MSG_XmRenderT_0002 "Le type de police XmN est incorrect. Impossible de charger la police."

MSG_XmRenderT_0003 "La conversion a échoué. Impossible de charger la police."

MSG_XmRenderT_0004 "XmNfontType défini à XmAS_IS. Impossible de charger la police."

MSG_XmRenderT_0005 "XmNloadModel est XmLOAD_IMMEDIATE mais XmNfont et XmNfontName ne sont pas spécifiés.\n\
Impossible de charger la police."


$set MS_XmSelect

MSG_XmSelect_0000 "Internal error: no selection property context for display"
$ MSG_XmSelect_0000 message is obsolete - DO NOT localize this message.

MSG_XmSelect_0001 "Selection owner returned type INCR property with format != 32"
$ please do not translate INCR.
$ MSG_XmSelect_0001 message is obsolete - DO NOT localize this message.

MSG_XmSelect_0002 "XtGetSelectionRequest called for widget \"%s\" outside of ConvertSelection proc"
$ please do not translate ConvertSelection.
$ MSG_XmSelect_0002 message is obsolete - DO NOT localize this message.


$set MS_XmString

MSG_XmString_0000 "Police introuvable."


$set MS_XmTabList

MSG_XmTabList_0000 "La valeur de tabulation ne peut pas être négative."


$set MS_SSpinB

MSG_SSpinB_0001 "Impossible de définir la ressource XmNtextField."

MSG_SSpinB_0002 "La ressource XmNpositionType ne peut être définie qu'au moment de sa création."

MSG_SSpinB_0003 "L'élément n'existe pas. XmNposition est inchangé."

