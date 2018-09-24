$ $XConsortium: dtksh.msg /main/3 1995/12/08 09:23:38 rswiston $
$quote "

$set 1

1 "DtkshCvtStringToPixel: La couleur '%s' n'est pas définie."

$set 5        Redefine set#


1 "Les paramètres de ressources indiqués sont trop nombreux ; '%s' n'est pas pris en compte."

2 "Spécification de ressource incorrecte. La forme admise est 'nom:valeur' : %s"

3 "Le séparateur de ressource est ':'. Or, vous avez utilisé '=' : %s"

4 "La boîte à outils a déjà été initialisée."

5 "Syntaxe: XtInitialize variable nom_application classe_application [arg. ...]"

6 "Syntaxe: %s variable nom classe parent [arg:val ...]"

7 "Syntaxe: XtCreateApplicationShell variable nom classe [arg:val ...]"

8 "Syntaxe: XtPopup widget GrabNone|GrabNonexclusive|GrabExclusive"

9 "Le type d'appropriation '%s' est inconnu ; utilisation de 'GrabNone'."

10 "Syntaxe: XtIsShell widget"

11 "Syntaxe: XtMapWidget widget"

12 "Syntaxe: XtUnmapWidget widget"

13 "Syntaxe: %s variable widget"

14 "Syntaxe: XtScreen variable widget"

15 "Syntaxe: XtWindow variable widget"

16 "Syntaxe: XtCallCallbacks widget fonction_appelée"

17 "Syntaxe: XtHasCallbacks variable widget fonction_appelée"

18 "Syntaxe: XtAddCallback widget fonction_appelée commande_ksh"

19 "Syntaxe: XtRemoveCallback widget fonction_appelée commande_ksh"

20 "Le descripteur de fonction appelée suivant est incorrect: %s"

21 "La fonction appelée indiquée n'est pas enregistrée."

22 "Syntaxe: XtAddEventHandler widget masque masque_impossible command_ksh"

23 "Syntaxe: XtRemoveEventHandler widget masque masque_impossible commande_ksh"

24 "Le descripteur d'événement indiqué n'est pas enregistré."

25 "Syntaxe: XtGetValues widget ressource:variable ..."

26 "La forme du paramètre de ressource suivant est incorrecte: %s."

27 "Syntaxe: XtSetValues widget arg:val ..."

28 "Syntaxe: XtAddWorkProc variable commande"

29 "Syntaxe: XtRemoveWorkProc ID_proc_travail"

30 "Un nombre hexadécimal doit être indiqué pour le paramètre ID_proc_travail: %s"

31 "Syntaxe: XtAddTimeOut variable millisecondes commande"

32 "La valeur indiquée pour le paramètre millisecondes doit être supérieure à zéro."

33 "Syntaxe: XtRemoveTimeOut ID_intervalle"

34 "Un nombre hexadécimal doit être indiqué pour le paramètre ID_intervalle: %s"

35 "Syntaxe: %s widget ..."

36 "Syntaxe: XBell affichage volume"

37 "Syntaxe: %s variable écran"

38 "Le paramètre d'écran est incorrect: %s"

39 "Syntaxe: XDefineCursor affichage fenêtre ID_curseur"

40 "Le paramètre ID_curseur est incorrect: %s"

41 "Syntaxe: XUndefineCursor affichage fenêtre"

42 "Syntaxe: XtRemoveAllCallbacks widget fonction_appelée"

43 "Syntaxe: XTextWidth variable nom_police chaîne"

44 "Syntaxe: %s affichage zone_dessin [arg. ...]"

45 "Le paramètre zone_dessin est incorrect: %s"

46 "Le paramètre cible est incorrect: %s"

47 "Nom de fonction graphique inconnu: %s"

48 "Style de ligne inconnu: %s"

49 "Option de dessin inconnue: %s"

50 "Des points n'ont pas été pris en compte."

51 "Syntaxe: XCopyArea affichage source cible sourceX sourceY largeur hauteur cibleX cibleY [arg. ...]"

52 "Impossible de convertir le type de ressource '%s' en 'chaîne'"

53 "Impossible de convertir le type de ressource 'chaîne' au type '%s'"

54 "Syntaxe: XtAddInput variable [-r] descripteur_fich commande_ksh"

56 "Syntaxe: XtRemoveInput ID_entrée"

57 "Le paramètre ID_entrée doit être un nombre hexadécimal: %s"

59 "Syntaxe: XFlush affichage"

60 "Syntaxe: XSync affichage suppression"

61 "Syntaxe: XRaiseWindow affichage fenêtre"

62 "Syntaxe: %s widget [True|False]"

63 "Syntaxe: %s widget translations"

64 "Syntaxe: XtUninstallTranslations widget"

65 "Syntaxe: XtParent variable widget"

66 "Syntaxe: XtLastTimestampProcessed variable affichage"

67 "Syntaxe: DtSessionRestorePath widget variable_chemin fich_sauvegarde"

68 "Syntaxe: DtSessionSavePath widget variable_chemin variable_fichier"

69 "Syntaxe: DtShellIsIconified widget"

70 "Syntaxe: DtSetStartupCommand widget commande"

71 "Syntaxe: DtSetIconifyHint widget booléen"

73 "Le paramètre fenêtre_root est incorrect: %s"

74 "Syntaxe: DtWsmSetCurrentWorkspace widget atome"

75 "L'atome de l'espace de travail est incorrect: %s"

78 "Syntaxe: DtWsmSetWorkspacesOccupied affichage fenêtre Liste_espaces_travail"

79 "Syntaxe: DtGetHourGlassCursor variable affichage"

80 "Syntaxe: DtWsmAddCurrentWorkspaceCallback variable widget commande_ksh"

81 "Syntaxe: DtWsmRemoveWorkspaceCallback identificateur"

82 "Syntaxe: XtNameToWidget variable widget_référence noms"

83 "Syntaxe: DtDbLoad"

84 "Syntaxe: DtDbReloadNotify commande_ksh"

85 "Syntaxe: DtActionExists nom_action"

86 "Syntaxe: DtActionLabel variable nom_action"

87 "Syntaxe: DtActionDescription variable nom_action"

88 "Syntaxe: DtActionInvoke widget nom_action param_terminal hôte_exéc rép_contexte indic_util commande_ksh [\"FILE\" nom_fichier] ..."

89 "Syntaxe: DtDtsLoadDataTypes"

90 "Syntaxe: DtDtsFileToDataType variable nom_fichier"

91 "Syntaxe: DtDtsFileToAttributeValue variable nom_fichier nom_attribut"

92 "Syntaxe: DtDtsFileToAttributeList variable nom_fichier"

93 "Syntaxe: DtDtsDataTypeToAttributeValue variable type_données nom_attribut nom_option"

94 "Syntaxe: DtDtsDataTypeToAttributeList variable type_données nom_options"

95 "Syntaxe: DtDtsFindAttribute variable nom valeur"

96 "Syntaxe: DtDtsDataTypeNames variable"

97 "Syntaxe: DtDtsSetDataType variable nom_fichier type_données annulation"

98 "Syntaxe: DtDtsDataTypeIsAction type_données"

99 "Syntaxe: ouvert_ttdt variable état variable2 nom_outil fournisseur version lancement_envoi"

100 "Syntaxe: gest_entrée_tttk ID_processus source ID"

101 "Le paramètre source doit être un entier: %s"

102 "Le paramètre ID doit être un nombre hexadécimal: %s"

103 "Syntaxe: fern_ttdt état ID_processus nouv_ID_processus arrêt_envoi"

104 "Syntaxe: jonction_sessions_ttdt variable état ID_session Desc_widget_shell jonction"

105 "Syntaxe: sortie_session_ttdt état ID_session modèles_session sortie"

106 "Le paramètre modèles_session est incorrect: %s"

107 "Syntaxe: événement_fichier_ttdt état op modèles envoi"

108 "Syntaxe: sortie_fichier_ttdt état modèles sortie"

109 "Syntaxe: modif_ttdt chemin_accès portée délai"

110 "Syntaxe: %s état chemin_accès portée délai"

112 "Syntaxe: pointeur_erreur_tt variable état_tt"

113 "Syntaxe: %s état msg"

114 "Syntaxe: %s état msg état_msg chaîne_état_msg suppression"

115 "Syntaxe: jonction_fichiers_ttdt variable état chemin_accès portée jonction commande_ksh"

116 "Syntaxe: fich_rés_fich_tt variable état nom_fichier"

117 "Syntaxe: fich_fich_réseau_tt variable état nom_fich_réseau"

118 "Syntaxe: fich_rés_fich_hôte_tt variable état hôte nom_fichier"

119 "Syntaxe: fich_fich_rés_hôte_tt variable état hôte nom_fich_rés"

120 "Syntaxe: XtIsSubclass widget classe"

121 "%s n'est pas un nom de classe de widget valide."

122 "Syntaxe: XtClass variable widget"

$set 6        Redefine set#


1 "DtkshCvtWindowToString: La taille de la valeur source est incorrecte."

2 "DtkshCvtHexIntToString: La taille de la valeur source est incorrecte."

3 "DtkshCvtIntToString: La taille de la valeur source est incorrecte."

4 "DtkshCvtBooleanToString: La taille de la valeur source est incorrecte."

5 "DtkshCvtStringToWidget: La taille de la valeur source est incorrecte."

6 "DtkshCvtStringToWidget: Le widget '%s' n'existe pas."

7 "DtkshCvtStringToCallback: La taille de la valeur source est incorrecte."

8 "DtkshCvtCallbackToString: La taille de la valeur source est incorrecte."

9 "DtkshCvtWidgetToString: La taille de la valeur source est incorrecte."

10 "DtkshCvtWidgetToString: Impossible de trouver un nom pour le widget."

11 "DtkshCvtWidgetClassToString: La taille de la valeur source est incorrecte."

12 "DtkshCvtWidgetClassToString: Classe de widget inconnue."

13 "DtkshCvtStringToWidgetClass: Classe de widget inconnue."

14 "DtkshCvtScreenToString: La taille de la valeur source est incorrecte."

$set 11        Redefine set#


1 "La structure '%s' ne contient pas la zone '%s'."

2 "DtkshCvtCallbackToString: Dépassement de capacité d'une mémoire tampon de conversion interne."

3 "Le hachage de la ressource '%s' dans la classe de widget '%s' n'a pas abouti."

4 "Aucun nom de fonction n'a été indiqué."

5 "L'exécution de la commande est impossible car la boîte à outils n'a pas été initialisée."

6 "La création du widget '%s' n'a pas abouti."

7 "Le paramètre affichage est incorrect: %s"

8 "L'atome de propriété suivant est incorrect: %s"

9 "La ressource suivante n'est pas définie pour le widget '%s': %s"

10 "La position indiquée est incorrecte: %s"

11 "La police indiquée est incorrecte: %s"

12 "Le descripteur de widget '%s' n'est pas associé à un widget existant."

13 "Le widget doit être de type 'command'."

14 "Le widget doit être de type 'mainWindow'."

15 "Le widget doit être de type 'scale'."

16 "Le widget doit être de type 'scrollBar'."

17 "Il doit s'agir d'un widget ou d'un gadget de type 'toggleButton'."

18 "Le paramètre de fenêtre est incorrect: %s"

19 "Impossible d'allouer la mémoire requise ; abandon."

20 "Impossible de trouver le widget père."

21 "Impossible de localiser le symbole '%s'."

22 "Impossible de localiser le type '%s'."

23 "Impossible d'analyser la déclaration '%s' ; utilisation de 'unsigned long'."

24 "Type de fils inconnu: %s."

25 "Option inconnue: %s."

26 "Syntaxe: %s widget position"

27 "Syntaxe: %s widget heure"

28 "Syntaxe: %s widget"

29 "dtksh - Erreur"

30 "dtksh - Avertissement"

31 "Syntaxe: %s affichage fenêtre"

32 "Syntaxe: %s affichage fenêtre variable"

33 "Syntaxe: %s affichage fenêtre_root variable"

34 "Le paramètre délai est incorrect: %s."

35 "Le paramètre msg est incorrect: %s."

36 "Le paramètre modèles est incorrect: %s."

$set 14        Redefine set#


1 "Une erreur liée à la table de hachage interne s'est produite lors de l'initialisation de la classe de widget ; abandon."

2 "La classe de widget '%s' n'existe pas."

3 "L'identificateur '%s' n'est pas un descripteur de widget correct."

4 "Classe de widget incorrecte."

5 "Syntaxe: DtLoadWidget nom_classe_widget nom_enreg_classe_widget"

6 "L'enregistrement de classe de widget '%s' est introuvable."

7 "Une erreur liée à la table de hachage interne s'est produite lors de l'initialisation de la classe de widget '%s'."

8 "\n\
%sRESSOURCES POUR %s%s%s:\n"

9 "CONTRAINTE "

10 "R"

11 "M"

12 "S"

13 "%-15s %-6s %-6s %-18s %-6s %s\n"

14 "VARIABLE ENV    CLASSE MERE DESCRIPT.            NOM ETAT\n"

15 "\n\
Syntaxe:\tDtWidgetInfo [descript_widget]\n\
\tDtWidgetInfo -r <descript_widget|nom_classe>\n\
\tDtWidgetInfo -R <descript_widget|nom_classe>\n\
\tDtWidgetInfo -c [nom_classe]\n\
\tDtWidgetInfo -h [descript_widget]"

$set 15        Redefine set#


1 "Impossible d'initialiser la boîte à outils."

2 "La ressource suivante ne peut pas être définie lors de la création\n\
du widget. Une fois le widget créé, utilisez XtSetValues: %s"

3 "Syntaxe: %s variable parent nom [argument:valeur ...]"

4 "Syntaxe: DtHelpReturnSelectedWidgetId variable widget variable"

5 "Syntaxe: DtHelpSetCatalogName nom_catalogue"

6 "Syntaxe: DtHelpQuickDialogGetChild variable widget_quickHelp fils"

7 "Le widget doit être de type 'quickHelp'."

8 "Le widget doit être de type 'list'."

9 "Syntaxe: %s widget position élément"

10 "Syntaxe: %s widget position liste_éléments"

11 "Syntaxe: %s widget élément"

12 "Syntaxe: XmListDeleteItemsPos widget nombre position"

13 "Syntaxe: XmListDeleteItems widget liste_éléments"

14 "Syntaxe: XmListDeletePositions widget liste_positions"

15 "Syntaxe: XmListGetKbdItemPos variable widget"

16 "Syntaxe: XmListItemExists widget élément"

17 "Syntaxe: XmListItemPos variable widget élément"

18 "Syntaxe: XmListPosSelected widget position"

19 "Syntaxe: XmListPosToBounds widget position variable variable variable variable"

20 "Syntaxe: %s widget position option_notification"

21 "Syntaxe: %s widget élément option_notification"

22 "Syntaxe: XmListSetAddMode widget booléen"

23 "Syntaxe: XmListSetKbdItemPos widget position"

24 "Syntaxe: XmMainWindowSetAreas fenêtre_principale menu commande défil_hor défil_vert travail"

25 "Le descripteur 'fenêtre_principale' est NULL."

26 "Syntaxe: %s variable fenêtre_principale"

27 "Syntaxe: XmProcessTraversal widget sens"

28 "Sens de passage inconnu: %s."

29 "Syntaxe: XmInternAtom variable affichage nom existant_seulement"

30 "Syntaxe: XmGetAtomName variable affichage atome"

31 "L'atome indiqué est incorrect: %s."

32 "Syntaxe: XmGetColors widget arrière-plan avant-plan ombrage_sup ombrage_inf sélection"

33 "Pixel d'arrière-plan incorrect: %s"

34 "Syntaxe: XmUpdateDisplay widget"

35 "Syntaxe: %s widget protocole [protocole ...]"

36 "L'atome indiqué est incorrect: %s."

37 "Syntaxe: XmAddWMProtocolCallback widget protocole commande_ksh"

38 "Syntaxe: XmRemoveWMProtocolCallback widget protocole commande_ksh"

39 "Syntaxe: XmMenuPosition menu événement"

40 "L'événement indiqué est incorrect: %s"

41 "Syntaxe: XmCommandAppendValue widget_command chaîne"

42 "Syntaxe: XmCommandError widget_command message_erreur"

43 "Syntaxe: XmCommandSetValue widget_command commande"

44 "Syntaxe: XmCommandGetChild variable widget_command fils"

45 "Syntaxe: XmMessageBoxGetChild variable widget_command fils"

46 "Le widget doit être de type 'messageBox'."

47 "Syntaxe: XmFileSelectionBoxGetChild variable widget fils"

48 "Le widget doit être de type 'boîte de sélection de fichier'."

49 "Syntaxe: XmSelectionBoxGetChild variable widget fils"

50 "Le widget doit être de type 'boîte de sélection'."

51 "Syntaxe: XmScaleGetValue wdget_scale variable"

52 "Syntaxe: XmScaleSetValue widget_scale valeur"

53 "La valeur d'échelle indiquée est incorrecte: %s."

54 "Syntaxe: XmScrollBarGetValues barre_défil variable variable variable variable"

55 "Syntaxe: XmScrollBarSetValues barre_défil valeur taille_règle incrément incrément_page notification"

56 "La valeur indiquée est incorrecte: %s."

57 "La taille indiquée pour la règle est incorrecte: %s."

58 "L'incrément indiqué est incorrect: %s."

59 "L'incrément de page indiqué est incorrect: %s."

60 "Syntaxe: XmScrollVisible fenêtre_défil widget marge_gauche_droite marge_sup_inf"

61 "Le widget doit être de type 'scrolledWindow'."

62 "Le widget à rendre visible n'existe pas."

63 "La valeur de marge gauche/droite indiquée est incorrecte: %s."

64 "La valeur de marge inférieure/supérieure indiquée est incorrecte: %s."

65 "Syntaxe: %s widget état notification"

66 "Syntaxe: ouverture_cat variable nom_cat"

67 "Syntaxe: fermeture_cat ID_cat"

68 "Syntaxe: catgets variable ID_cat Num_set Num_msg Msg_dft"

69 "Le widget doit être de type 'text' ou 'textField'."

70 "Syntaxe: %s variable widget"

71 "Syntaxe: %s widget booléen"

72 "Le paramètre indiqué est incorrect: %s."

73 "Syntaxe: %s widget lignes"

74 "Syntaxe: %s widget long_maxi"

75 "Syntaxe: %s widget chaîne"

76 "Syntaxe: %s widget variable variable"

77 "Syntaxe: %s widget position chaîne"

78 "Syntaxe: %s widget position variable variable"

79 "Syntaxe: %s widget position_source position_cible chaîne"

80 "La position source indiquée est incorrecte: %s."

81 "La position cible indiquée est incorrecte: %s."

82 "Syntaxe: %s widget première_position dernière_position heure"

83 "La première position indiquée est incorrecte: %s."

84 "La dernière position indiquée est incorrecte: %s."

85 "L'heure indiquée est incorrecte: %s."

86 "Syntaxe: %s variable widget x y"

87 "La position x indiquée est incorrecte: %s."

88 "La position y indiquée est incorrecte: %s."

89 "Syntaxe: %s widget droite gauche mode"

90 "La position de gauche indiquée est incorrecte: %s."

91 "La position de droite indiquée est incorrecte: %s."

92 "Syntaxe: %s widget départ chaîne sens variable"

93 "La position de départ indiquée est incorrecte: %s."

94 "Syntaxe: XmListGetSelectedPos variable widget"

95 "Syntaxe: XmListGetMatchPos variable widget élément"

96 "Syntaxe: XmOptionLabelGadget variable widget"

97 "Syntaxe: XmOptionButtonGadget variable widget"

98 "Syntaxe: XmGetVisibility variable widget"

99 "Syntaxe: XmGetTearOffControl variable widget"

100 "Syntaxe: XmGetTabGroup variable widget"

101 "Syntaxe: XmGetPostedFromWidget variable widget"

102 "Syntaxe: XmGetFocusWidget variable widget"

103 "Syntaxe: XmFileSelectionDoSearch widget masque_répertoire"

$set 16        Redefine set#


1 "DtkshCvtNamedValueToString: Aucune correspondance trouvée."

2 "DtkshCvtStringToNamedValue: Impossible de convertir la chaîne '%s'."
$ The following messages are for ksh93 itself.  The message numbers must
$ match those specified in the MsgStr array allmsgs in
$ ksh93/src/cmd/ksh93/sh/init.c, as must the set number.

$set 25        Redefine set#


1 "Effectué(es)"

3 "En cours d'exécution"

4 "[-n] [arg...]"

5 "[arg...]"

6 "[rép] [liste]"

7 "[travail...]"

8 "[n]"

9 "[nom [chemin_accès] ]"

10 "[nom]"

11 "[sup] [base]"

12 "expr..."

13 "format [arg...]"

14 "est une fonction."

15 "est un mot clé."

16 "est interne au shell."

17 "est une fonction exportée."

18 "est une fonction indéfinie."

19 "nom [arg...]"

20 "chaîne_opt nom [args...]"

21 "secondes"

23 "${HOME:-.}/.profile"

24 "%c: L'expression contient un caractère incorrect: %s ."

25 "%c: Le format d'identificateur est inconnu."

26 "%d-%d: Plage incorrecte."

27 "%d: Version de script binaire incorrecte."

28 "%s est un alias pour "

29 "%s est un alias exporté pour "

30 "%s est manquant."

31 "%s: base inconnue."

32 "%s: ':' attendu pour l'opérateur '?'."

33 "%s: Ambigu."

34 "%s: Les arguments doivent être des ID de processus ou des numéros de %travaux."

35 "%s: alias introuvable.\n"

36 "%s: Erreur de syntaxe arithmétique."

37 "%s: L'affectation nécessite une lvalue."

38 "%s: Numéro d'unité de fichier incorrect."

39 "%s: Format incorrect."

40 "%s: Numéro incorrect."

41 "%s: Option(s) incorrecte(s)."

42 "%s: Remplacement incorrect."

43 "%s: trap incorrect."

44 "%s: Création impossible."

45 "%s: Exécution impossible."

46 "%s: Ouverture impossible."

47 "%s: Division par zéro."

48 "%s: Exception de domaine."

49 "%s: Echec de %s."

50 "%s: Le fichier existe déjà."

51 "%s: Nom de fonction incorrect."

52 "%s: Nom d'alias incorrect."

53 "%s: Fonction de discipline incorrecte."

54 "%s: Nom d'exportation incorrect."

55 "%s: Nom de fonction incorrect."

56 "%s: Nom incorrect."

57 "%s: Expression régulière incorrecte."

58 "%s: Auto-référence incorrecte."

59 "%s: Utilisation incorrecte de :"

60 "%s: Nom de variable incorrect."

61 "%s n'est pas un identificateur."

62 "%s est accessible en lecture seulement."

63 "%s: Libellé non mis en oeuvre."

64 "%s: Limite dépassée."

65 "%s: Des symboles plus nombreux étaient attendus."

66 "%s: Pas de père."

67 "%s: Pas de nom de référence."

68 "%s: Introuvable."

69 "%s: Non mis(e) en oeuvre."

70 "%s: Les opérandes sont de types incompatibles."

71 "%s: Exception de dépassement de capacité."

72 "%s: Paramètre non défini."

73 "%s: Paramètre nul ou non défini."

74 "%s: Recursion trop profonde."

75 "%s: Une variable de référence ne peut pas être un tableau."

76 "%s requiert un chemin d'accès."

77 "%s: Restreint."

78 "%s: Exception de singularité."

79 "%s: Indice hors limites."

80 "%s: Parenthèse manquante."

81 "%s: Fonction inconnue."

82 "%s: Environnement local inconnu."

83 "%s: Opérateur inconnu."

84 "%s: Nom de signal inconnu."

85 "%s: Entraînerait une boucle."

86 "(coredump)"

87 "-c requiert un argument."

88 "-e - requiert un argument unique."

89 "/vpix"

90 "<commande inconnue>"

91 "ACE#?F#?H:[nom]L#?R#?Z#?fi#?[base]lnprtux [nom=[valeur]...]"

92 "AE#?F#?HL#?R#?Z#?fi#?[base]lnprtux [nom=[valeur]...]"

93 "Abandon"

94 "Ad:[delim]prst#[délai]u#[num_fich] [nom...]"

95 "Appel d'un signal d'alarme"

96 "Spécification de noeud root incorrecte."

97 "Appel système incorrect."

98 "Tube interrompu"

99 "Erreur de bus"

100 "Impossible de lancer la gestion des travaux."

101 "Paramètres en cours de l'option"

102 "Signal DIL"

103 "Fils arrêté"

104 "DircabefhkmnpstuvxCI:[fichier]o:?[option]A:[nom] [arg...]"

105 "DircabefhkmnpstuvxCo:?[option]A:[nom] [arg...]"

106 "Interruption EMT"

107 "Limite de temps CPU dépassée."

108 "Limite de taille de fichier dépassée."

109 "Exception de calcul en virgule flottante"

110 "HSacdfmnstv [limite]"

111 "Déconnexion"

112 "Signal d'E-S"

113 "Instruction interdite"

114 "Interruption"

115 "Arrêté"

116 "LP [rép] [modif]"

117 "Erreur de mémoire"

118 "Processus de migration"

119 "Pas de gestion des travaux"

120 "Interruption téléphone"

121 "Alarme d'appel sélectif "

122 "Coupure d'alimentation"

123 "Définition du profil de l'alarme"

124 "Quitter"

125 "Perte de ressources"

126 "Retour au pilote de tty précédent..."

127 "S [masque]"

129 "Libellé de sécurité modifié"

130 "Interruption de prise"

131 "Test sonore terminé"

132 "Fin (signal)"

133 "Fin (entrée de tty)"

134 "Suite du processus interrompu"

135 "Arrêté"

136 "Fin (sortie de tty)"

137 "Passage au nouveau pilote de tty..."

138 "Arrêt imminent du système"

139 "Interrompu"

140 "Trace/Interruption BPT"

141 "Version inconnue"

142 "Entrez 'exit' pour mettre fin à ce shell."

143 "Signal utilisateur 1"

144 "Signal utilisateur 2"

145 "Version non définie"

146 "Signal de l'horloge virtuelle"

147 "Modification de taille de la fenêtre"

148 "Des travaux sont en cours d'exécution."

149 "Vous avez arrêté des travaux."

150 "[_[:alpha:]]*([_[:alnum:]])"

151 "\n\
@(#)Version 12/28/93\0\n"

152 "\n\
@(#)Version M-12/28/93\0\n"

153 "\n\
réel"

154 "\r\n\
\007Dépassement de délaid'inactivité du shell dans 60 secondes."

155 "un nom..."

156 "a:c [commande [args...] ]"

157 "nom afpv..."

158 "alarm %s %.3f\n"

159 "alarm -r %s +%.3g\n"

160 "Argument attendu"

161 "Répertoire incorrect"

162 "Numéro d'unité de fichier incorrect."

163 "Remplacement incorrect"

164 "Impossible d'accéder aux répertoires pères."

165 "Impossible de créer un tube."

166 "Impossible de créer un fichier temporaire."

167 "Impossible d'exécuter fork."

168 "Impossible d'obtenir %s."

169 "Impossible de définir %s."

170 "Impossible de définir l'alarme."

171 "Condition(s) requise(s)"

172 "dsf:[biblio] [nom...]"

173 "e:[editeur]lnrsN# [premier] [dernier]"

174 "Fin de fichier."

175 "f:[format]nprsu:[num_fich] [arg...]"

176 "nom fnv..."

177 "hist -e \"${VISUAL:-${EDITOR:-vi}}\" "

178 "Impossible d'ouvrir le fichier d'historique."

179 "Syntaxe incorrecte."

180 "Argument de type %c incorrect."

181 "est une version interne au shell de"

182 "est un alias repéré pour "

183 "kill"

184 "Ligne %d: $ non précédé de \\"

185 "Ligne %d: %c entre ${} doit être encadré de guillemets."

186 "Ligne %d: %s Libellé inconnu."

187 "Ligne %d: %s entre [[...]] obsolète ; utilisez ((...))."

188 "Ligne %d: '=' obsolète ; utilisez '=='."

189 "Ligne %d: -a obsolète ; utilisez -e."

190 "Ligne %d: \\ devant %c réservé pour une utilisation ultérieure."

191 "Ligne %d: `...` obsolète ; utilisez $(...)."

192 "Ligne %d: indiquez un caractère d'échappement pour %c afin d'éviter toute ambiguïté."

193 "Ligne %d: libellé %s ignoré."

194 "Ligne %d: indiquez %c entre guillemets pour éviter toute ambiguïté."

195 "Ligne %d: ensemble %s obsolète."

196 "Ligne %d: Des espaces sont requis pour le sous-shell imbriqué."

197 "Ligne %d: indiquez des accolades pour éviter toute ambiguïté avec $id[...]."

198 "Ligne %d: indiquez un espace ou une tabulation pour séparer les opérateurs %c et %c."

199 "ln#[num_signal]s:[nom_signal] signal..."

200 "Shells de connexion setuid/setgid interdits."

201 "mappage"

202 "retour à la ligne"

203 "nlp [travail...]"

204 "Pas de fichier d'historique."

205 "Pas de processus d'interrogation."

206 "Ce travail n'existe pas."

207 "Ce processus n'existe pas."

208 "Non pris en charge."

209 "hors fonction"

210 "en fonction"

211 "Nombre maximal de fichiers ouverts dépassée."

212 "Mémoire saturée."

213 "p [action condition...]"

214 "p [nom[=valeur]...]"

215 "Paramètre non défini."

216 "Autorisation refusée."

217 "Ce processus existe déjà."

218 "ptx [nom=[valeur]...]"

219 "pvV nom [arg]..."

220 "r [nom_variable secondes]"

221 "Erreur de syntaxe à la ligne %d: `%s' %s"

222 "Erreur de syntaxe à la ligne %d: libellé en double %s."

223 "Erreur de syntaxe: `%s' %s"

224 "sys"

225 "Dépassement de délai lors de l'attente d'entrée."

226 "inattendu"

227 "universel inaccessible"

228 "illimité"

229 "sans correspondance"

230 "utilisateur"

231 "versions"

232 "Echec de l'écriture dans %d."

233 "Vous avez du courrier dans $_"

234 "zéro octet"
