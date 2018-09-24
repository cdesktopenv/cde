$ $TOG: SUNW_TOOLTALK.msg /main/7 1999/09/16 13:44:06 mgreess $
$quote "

$set 1

3 "Echec de clnt_create pour rpc.ttdbserverd sur %s.%s"

4 "Erreur: rpc.ttdbserverd ne s'exécute pas sur %s."

6 "La version du serveur (%s) ne correspond pas à celle des tables de bases de données (%s). Exécutez la version %s du serveur de base de données."

7 "La tentative d'ouverture de la table de base de données %s:%s a échoué."

14 "Impossible de supprimer la propriété %s de la fenêtre root X faisant référence à cette session."

15 "La version d'adresse est %d. Or, la seule version admise est %d (adresse: <%s>)."

17 "Impossible d'analyser le nom d'écran X: \"%s\"."

18 "Impossible de mettre en attente un message pour le fichier \"%s\" (erreur interne %d)."

19 "Impossible de supprimer la session altérée <%s> de la liste des processus intéressés par le fichier \"%s\" (erreur interne %d)."

20 "Impossible de supprimer la session <%s> de la liste des processus intéressés par le fichier \"%s\" (erreur interne %d)."

21 "Erreur de syntaxe tttracefile à la ligne:\n\
%s"

22 "Impossible de traiter tttracefile dans $%s."

23 "Impossible d'exécuter la trace si l'UID réel ne correspond pas à l'UID effectif."

24 "Requête acceptée."

25 "Document vide, fichier inexistant"

26 "Impossible d'obtenir l'ID du message. Vous ne pourrez peut-être pas annuler l'opération (tt_message_arg_val())."

27 "Impossible d'obtenir le titre. Le document ne portera pas de titre (tt_message_arg_val())."

28 "Décalage opnum inconnu."

$set 2


2 "Signatures non identifiables pour le ptype %s -"

3 "Pas de transport orienté connexion."

4 "Un client ToolTalk devant extraire un message s'est interrompu avant d'être identifié comme tel."

5 "La connexion avec le client ToolTalk s'est interrompue alors que son identification comme devant extraire un message était en cours."

6 "Une autre session en cours d'exécution a été détectée (hôte=%s, pid=%d)."

7 "Impossible d'établir la communication avec la session spécifiée (hôte=%s, pid=%d) ; celle-ci est considérée comme arrêtée."

8 "Impossible d'analyser l'ID de session indiqué ; remplacement par l'ID par défaut."

9 "%s est une base de données de types version %d ; celle-ci ne peut lire que les versions %d et précédentes."

10 "Impossible de décoder les types de la base de données : %s ; celle-ci est peut-être altérée."

11 "%s a été écrasé."

12 "L'attribut inconnu <%s> de la signature ToolTalk est ignoré..."

13 "Echec de ttce2xdr (état=%d) ; les types de la base de données \"utilisateur\" Classing Engine ne seront pas convertis..."

14 "Enregistrement ISAM en double"

15 "Le fichier ISAM n'est pas ouvert."

16 "L'argument ISAM est incorrect."

17 "Le descripteur clé ISAM est incorrect."

18 "Les fichiers ISAM ouverts sont trop nombreux."

19 "Le format de fichier ISAM est incorrect."

20 "L'accès ISAM n'est pas exclusif."

21 "L'enregistrement ISAM est verrouillé."

22 "La clé ISAM existe déjà."

23 "La clé ISAM est une clé principale."

24 "Il s'agit de la fin ou du début du fichier ISAM."

25 "Enregistrement ISAM introuvable."

26 "Pas d'enregistrement ISAM en cours."

27 "Le fichier ISAM est verrouillé."

28 "Le nom de fichier ISAM est trop long."

29 "ISAM ne peut pas allouer de mémoire."

30 "Temps d'inactivité ISAM."

31 "Connexion ISAM TCP/IP interrompue."

32 "Impossible d'établir la connexion avec le serveur ISAM."

33 "Impossible d'importer les données ISAM."

34 "Pas de démon SAM local."

35 "Erreur interne ISAM fatale."

36 "Pas de correspondance environnement local ISAM /LANG."

$set 3


2 "Fils (%d) interrompu suite au signal %d%s."

3 " (cliché de fichier core)"

4 "Le fils (%d) n'a été ni interrompu ni signalé."

5 "Impossible d'agrandir les clients en raison de %m."

6 "Aucune étendue à gérer. Utilisez -c, -p, -d ou définissez $DISPLAY."

7 "Les options -S et -c sont incompatibles."

8 "Le processus ttsession fils s'est interrompu (état %d)."

9 "Le processus ttsession fils s'est interrompu suite au signal %d%s."

10 " (cliché de fichier core)"

11 "Le processus ttsession fils n'a été ni interrompu ni signalé."

12 "Lancement"

13 "Les types ont été relus."

14 "Erreur dans les types ; conservation des anciens types."

15 "Pas de définition pour $OPENWINHOME."

16 "\n\
Syntaxe: ttsession [-a unix|des][-d écran][-spStvhNX"

17 "][-c commande]\n\
 -c [commande]\tLance une session dans l'arborescence de processus et y exécute la\n\
\t\tcommande, en utilisant les options qui suivent (par défaut: $SHELL).\n\
 -p\t\tLance une session dans l'arborescence de processus et affiche son ID.\n\
 -d écran\tLance une session X sur l'écran indiqué.\n\n\
 -a unix|des\tDéfinit le niveau de vérification d'identité.\n\
 -s\t\tMode silencieux: aucun message ne s'affiche.\n\
 -S\t\tPas d'appel système fork en arrière-plan.\n\
 -N\t\tAugmente le nombre de clients autorisés.\n\
 -t\t\tActive la trace des messages.\n\
 -X\t\tUtilisation des bases de données XDR pour les types statiques (défaut).\n"

18 " -E\t\tUtilisation de Classing Engine pour les types statiques.\n"

19 "\n\
 -v\t\tAffiche le niveau de version.\n\
 -h\t\tAffiche ce message.\n\n\
Interface de signalisation:\n\
 kill -USR1 PID_ttsession\tActive ou désactive la trace des messages.\n\
 kill -USR2 PID_ttsession\tRelit les types statiques."

20 "Abandon en cours"

$set 4


2 "Syntaxe:\n\
tt_type_comp [-s] [-d base_données] [-mM] fichier_source\n\
tt_type_comp [-s] [-d base_données] -r type ...\n\
-M\tInclut les types source dans la base de données, sans mettre à jour les types existants.\n\
-m\tMême fonction, avec mise à jour des types existants (option par défaut).\n\
-r\tSupprime les types source de la base de données indiquée.\n\
-d base_données\tBase de données à laquelle la commande doit être appliquée: utilisateur (valeur par défaut), système ou réseau.\n\
-G\tRécupère les positions inutilisées sur le serveur de base de données ToolTalk.\n\n\
tt_type_comp [-sE] -p|O|P [-d base_données]\n\
tt_type_comp [-s]  -p|O|P fich_compilé\n\
-O\tAffiche sur la sortie standard les noms des otypes lus.\n\
-P\tAffiche sur la sortie standard les noms des ptypes lus.\n\
-p\tAffiche sur la sortie standard les types ToolTalk lus.\n\
-E\tUtilise les bases de données Classing Engine au lieu des bases de données XDR.\n\
-d base_données\tBase de données à partir de laquelle la lecture doit être effectuée.\n\n\
tt_type_comp [-s] -x [-o fich_compilé] fich_source\n\
-x\tCompile les types à partir du fich_source (ou de l'entrée standard, si le fichier est \"-\").\n\
-o\tEcrit les types compilés dans fich_compilé (ou sur la sortie standard, si le fichier est \"-\").\n\
\tPar défaut: fich_source.xdr ou \"types.xdr\" si la source est l'entrée standard.\n\n\
tt_type_comp [-hv]\n\
-v\tAffiche le niveau de version.\n\
-h\tAffiche ce message.\n\
-s\tN'affiche aucun message d'état.\n\n\
Les options cpp suivantes sont transmises:\n\
        -undef -Dname -Idirectory -Uname -Ydirectory"

3 "Erreur de sémantique dans le fichier de types."

4 "Le fichier de types ToolTalk est incorrect."

5 "Impossible de lire les types ToolTalk à partir de la base de données Classing Engine."

6 "L'inclusion des tables Classing Engine n'est plus prise en charge."

7 "Impossible de lire les types dans la base de données %s (incompatibilité de versions)."

8 "Impossible de lire les types dans la base de données %s."

9 "Pas de définition pour $OPENWINHOME."

10 "Impossible d'initialiser la base de données %s pour l'écriture."

11 "Impossible de supprimer la définition précédente de %s."

12 "Remplacement en cours"

13 "Ecriture en cours"

14 "Impossible d'ajouter la définition de %s."

15 "Impossible de supprimer la définition précédente de %s."

16 "Remplacement en cours"

17 "Ecriture en cours"

18 "Impossible d'ajouter la définition de %s."

19 "Incompatibilité de versions dans les types compilés."

20 "Impossible de lire les types dans la base de données."

21 "Pas de définition pour $OPENWINHOME."

22 "Impossible de lire les types dans la base de données %s (incompatibilité de versions)."

23 "Impossible de lire les types dans la base de données %s."

24 "Pas de définition pour $OPENWINHOME."

25 "Suppression en cours de l'otype %s.\n"

26 "Suppression en cours du ptype %s.\n"

27 "Sortie écrite dans %s.\n"

28 "Base de données incorrecte: %s."

29 "Indiquez une option parmi les suivantes: -O -P -m -M -p -r -x."

30 "Le prétraitement n'a pas été effectué (%s: %s)."

31 "Cycle d'héritage otype comprenant %s."

32 "L'ancêtre %s de l'otype %s n'existe pas."

33 "Le ptype %s n'existe pas, mais a été nommé par l'otype de mise en oeuvre %s."

34 "Plusieurs identificateurs sont définis dans l'otype %s."

35 "La signature héritée de l'otype %s n'existe pas dans l'otype père."

36 "Deux ptypes, %s et %s, ont le même identificateur."

37 "L'ancêtre %s de l'otype %s n'existe pas."

38 "Hiérarchie d'héritage d'otype cyclique -\n\
  {"

39 "Définition en double pour l'otype %s."

40 "Définition en double pour le ptype %s."

$set 5


3 "Violation de sécurité: Un appel RPC a demandé l'ouverture d'un fichier qui n'est pas une base de données ToolTalk."

4 "Les données écrites à l'aide d'un rpc.ttdbserverd version 1.0.2 ou antérieure après avoir utilisé un rpc.ttdbserverd version 1.1 ou suivante sont ignorées."

5 "Les données écrites à l'aide d'un rpc.ttdbserverd version 1.0.2 ou antérieure après avoir utilisé un rpc.ttdbserverd version 1.1 ou suivante sont ignorées."

6 "La version du rpc.ttdbserverd (%s) ne correspond pas à celle des tables de base de données (%s). Installez la version %s ou suivante."

7 "Syntaxe:\n\
rpc.ttdbserverd [-S] [-n] [-m DTMOUNTPOINT]\n\
rpc.ttdbserverd [-S] [-v]"

$ 8 NEW-
8 "Unable to start garbage collection thread. thr_create()\n"
$ 9 NEW-
9 "Unable to fork() for garbage collection.\n"

$set 6


2 "Syntaxe:\n\
ttdbck [-f fichier] [-k clé_obj [-t type] [-bx] \n\
[-impa] [-IZ] [-F nouv_fichier] [-T nouv_type] [points_montage]\n"

3 "ttdbck: Si vous spécifiez une option de réparation [-FTZ], vous devez également\n\
indiquer une option de sélection [-fkt] ou de diagnostic [-bx].\n"

4 "Incompatibilité de versions dans les types compilés."

5 "Impossible de lire les types dans la base de données."

6 "ttdbck: Indiquez ttdbck -I.\n"

7 "Sélection par nom de fichier: %s\n"

8 "Sélection par clé d'objid:"

9 "Sélection par type: %s\n"

10 "Des entités incorrectes ont été détectées.\n"

11 "Des références à des entités inexistantes ont été détectées.\n"

12 "Affich. ID\n"

13 "Affich. données requises\n"

14 "Affich. caractéristiques et valeurs\n"

15 "Appel de la fonction NetISAM isrepair() avant inspection\n"

16 "Réparation par définition du type: %s\n"

17 "Réparation par suppression\n"

18 "Débogage niveau affichage %d\n"

19 "Réparation par définition du fichier: %s\n"

20 "Erreur: Pas de fichier pour la spécification.\n"

21 "Erreur: Pas de type pour la spécification.\n"

22 "Erreur: \"%s\" n'est pas un otype installé.\n"

23 "Erreur: Plusieurs spécifications pour la propriété du type.\n"

24 "Erreur: "

25 "ttdbck: Aucune erreur détectée.\n"

26 "Erreur: "

$set 7


2 "Syntaxe: %s {ctx}[fhpPv[v]] [fichier_tar] chemin ...\n\
         %s {ctx}fL[hpPRv[v]] fichier_tar_tt chemin ...\n\
         %s -v\n\
         %s -h\n"

3 "\tc       Crée un fichier archive.\n\
\tt       Affiche le contenu d'un fichier archive.\n\
\tx       Extrait à partir d'un fichier archive.\n\
\tf       Utilise l'argument <fichier_tar> comme fichier archive.\n\
\th       Suit les liens symboliques.\n\
\tL       N'appelle pas tar(1).\n\
\tp       Conserve les modes des fichiers.\n\
\tP       (root) Ne conserve pas les caractéristiques (heure de modif.,\n\
\t        propriétaire, etc.) des fichiers.\n\
\tR       N'effectue pas de récursion dans les répertoires.\n\
\tv       Sortie prolixe.\n\
\tvv      Sortie très prolixe.\n\
\t-v      Affiche le niveau de version et sort.\n\
\t-h[elp] Affiche ce message.\n"

4 "%s: Impossible de lire le type d'objet à partir du flot d'archive.\n"

5 "%s: Impossible de lire la version d'archive à partir du flot.\n"

6 "%s: Version d'archive détectée: %d ; version attendue: %d.\n"

7 "%s: Un type d'objet inconnu (%d) a été détecté dans l'archive.\n"

8 "%s: Impossible de lire le type d'objet à partir du flot d'archive.\n"

9 "%s: Impossible de lire la version d'archive à partir du flot d'archive.\n"

10 "%s: Version d'archive détectée: %d ; version attendue: %d.\n"

11 "%s: Un type d'objet inconnu (%d) a été détecté dans l'archive.\n"

$set 8


2 "%s: Impossible de supprimer les objets ToolTalk de %s (%s).\n"

3 "%s: Impossible de déplacer les objets ToolTalk de \"%s\" dans \"%s\" (%s).\n"

4 "%s: Pas de déplacement des objets ToolTalk de:\n"

5 "Syntaxe: %s [-] [-fL] chemin1 chemin2\n\
         %s [-] [-fL] chemin1 [chemin2 ...] rép.\n\
         %s -v\n\
         %s -h\n"

6 "\t-L      N'exécute pas la commande mv(1)\n\
\t-v      Affiche le niveau de version et sort\n\
\t-h      Affiche ce message\n"

7 "%s: Impossible de supprimer les objets ToolTalk de %s (%s).\n"

8 "Syntaxe: %s [-] [-%s] %s ...\n\
         %s -v\n\
         %s -h\n"

9 "fichier"

10 "rép"

11 "\t-L      N'exécute pas %s(1).\n\
\t-v      Affiche le niveau de version et sort.\n\
\t-h[elp] Affiche ce message.\n"

12 "%s: Impossible de supprimer les objets ToolTalk de %s (%s).\n"

13 "Syntaxe: %s [-pL] fichier1 fichier2\n\
         %s [-prRL] chemin1 [chemin2 ...] rép.\n\
         %s -v\n\
         %s -h\n"

14 "\t-L      N'exécute pas la commande cp(1).\n\
\t-v      Affiche le niveau de version et sort.\n\
\t-h      Affiche ce message.\n"

$set 9


2 "Syntaxe: %s [-0FCa][-o fich_sortie] [-S session | commande [options]]\n\
       %s [-e script | -f fich_script][-S session | commande [options]]\n\
 -0\t\tDésactive la trace des messages pour la session ou exécute la commande\n\
\t\tsans tracer les messages (seule la trace d'API est effectuée).\n\
 -F\t\tSuit les processus fils ayant fait l'objet d'un appel fork ou lancés\n\
\t\tdans la session par ttsession(1).\n\
 -C\t\tPas de trace des appels API ToolTalk.\n\
 -a\t\tAffiche les attributs, les arguments et le contexte des messages\n\
\t\ttracés. Par défaut, ces informations s'affichent sur une seule ligne.\n\
 -e script\tLit les paramètres tttracefile(4) à partir du script.\n\
 -f fich_script\tLit les paramètres tttracefile(4) à partir du script.\n\
\t\t\"-\": entrée standard.\n\
 -o fich_sortie\tSortie. \"-\": sortie standard. Par défaut, la sortie standard\n\
\t\test utilisée pour la trace de session et l'erreur standard (de tttrace), pour\n\
\t\tla trace des commandes.\n\
 -S session\tSession à tracer. Par défaut, voir tt_default_session().\n\
 commande\tClient ToolTalk à appeler et à tracer.\n"

3 "%s: La session <%s> ne prend pas en charge Session_Trace ; utilisez kill -USR1 (voir ttsession(1)).\n"

$set 10


2 "Le paramètre Tt_address inconnu transmis à Tts_address::add() est ignoré.\n"

3 "Le paramètre Tt_address inconnu transmis à Tts_address::remove() est ignoré.\n"

4 "L'adresse inconnue transmise à tts_address::tts_set() est ignorée.\n"

5 "L'adresse inconnue transmise à tts_address::tts_unset() est ignorée.\n"

6 "L'adresse inconnue lue par tts_address::load() est ignorée.\n"

7 "Tts_arglist::Tts_arglist - erreur lors de la lecture de l'argument - \n\
\t"

8 "Tts_arglist::load - Type d'argument inconnu détecté - utilisation de BARG.\n"

9 "Classe de résumé Tts_arg::assign (Tt_pattern) appelée - Opération ignorée.\n"

10 "Classe de résumé Tts_arg::assign(Tt_message) appelée - Opération ignorée.\n"

11 "Classe de résumé Tts_arg::generate appelée - Opération ignorée.\n"

12 "Classe de résumé Tts_arg::save appelée - Opération ignorée.\n"

13 "Classe de résumé Tts_arg::load appelée - Opération ignorée.\n"

14 "Mode incorrect tts_arg::mode_to_chars - Utilisation de TT_INOUT.\n"

15 "Mode incorrect tts_arg::chars_to_mode - Utilisation de TT_INOUT.\n"

16 "La catégorie tt_category inconnue transmise à Tts_category::set() est ignorée.\n"

17 "Tts_category::generate() a demandé un message - Requête ignorée.\n"

18 "La catégorie inconnue lue par tts_category::load() est ignorée.\n"

19 "La tt_class inconnue transmise à Tts_class::add() est ignorée.\n"

20 "La tt_class inconnue transmise à Tts_class::remove() est ignorée.\n"

21 "La classe inconnue transmise à tts_class::tts_set() est ignorée.\n"

22 "La classe inconnue transmise à tts_class::tts_unset() est ignorée.\n"

23 "La classe inconnue lue par tts_class::load() est ignorée.\n"

24 "Tts_connection::universal_callback - Pas de modèle pour la forme.\n"

25 "Tts_cntxtlist::Tts_cntxtlist - Erreur lors de la lecture du contexte - \n\
\t"

26 "Tts_contextlist::load - Type cntxt inconnu détecté - Utilisation de BCNTXT.\n"

27 "Classe de résumé tts_context::assign (Tt_pattern) appelée - Opération ignorée.\n"

28 "Classe de résumé tts_context::assign (Tt_pattern) appelée - Opération ignorée.\n"

29 "Classe de résumé tts_context::generate appelée - Opération ignorée.\n"

30 "Classe de résumé tts_context::save appelée - Opération ignorée.\n"

31 "Classe de résumé tts_context::load appelée - Opération ignorée.\n"

32 "La Tt_disposition inconnue transmise à tts_disposition::add() est ignorée.\n"

33 "La Tt_disposition inconnue transmise à tts_disposition::remove() est ignorée.\n"

34 "La disposition inconnue transmise à tts_disposition::tts_set() est ignorée.\n"

35 "La disposition inconnue transmise à tts_disposition::tts_unset() est ignorée.\n"

36 "La disposition inconnue lue par tts_disposition::load() est ignorée.\n"

37 "%s appelé\n"

38 "Erreur ToolTalk: %s\n\
        Origine: Fichier source %s à la ligne %d\n"

39 "Avertissement: %s\n\
        Origine: Fichier source %s à la ligne %d\n"

40 "Tts_message_molding::send - Tentative de transmisseion d'un modèle de message non initialisé.\n"

41 "Tt_scope inconnu transmis à tts_scope::add() ignoré\n"

42 "tt_scope inconnu transmis à tts_scope::remove() ignoré\n"

43 "L'étendue inconnue transmise à tts_scope::tts_set() est ignorée.\n"

44 "L'étendue inconnue transmise à tts_scope::tts_unset() est ignorée.\n"

45 "L'étendue inconnue lue par tts_scope::load() est ignorée.\n"

46 "tt_state inconnu transmis à tts_state::add() ignoré\n"

47 "tt_state inconnu transmis à tts_state::remove() ignoré\n"

48 "L'nstruction inconnue transmise à tts_state::tts_set() est ignorée.\n"

49 "L'instruction inconnue transmise à tts_state::tts_unset() est ignorée.\n"

50 "Tts_state::generate() a demandé un message - Requête ignorée.\n"

51 "L'instruction inconnue lue par tts_state::load() est ignorée.\n"

82 "Obtention de l'aide TTSnoop"

83 "---------------------------"

84 "Pour accéder à l'aide sur les boutons et les paramètres:"

85 "Amenez le pointeur de la souris sur le bouton ou le paramètre"

86 "et appuyez sur la touche d'aide. Pour afficher les appels API utilisés"

87 "par ttsnoop, indiquez l'option -t lors de l'appel de ttsnoop."

88 ""

89 "Utilisation de TTSnoop"

90 "----------------------"

91 "Pour visualiser les messages, sélectionnez l'option Lancer."

92 ""

93 "Pour désactiver l'affichage des messages, sélectionnez l'option Arrêt."

94 ""

95 "Pour définir le type de message à afficher:"

96 "      1) Sélectionnez le bouton Modèles."

97 "      2) Indiquez le modèle voulu."

98 "      3) Sélectionnez le bouton Appliquer."

99 ""

100 "Pour mettre en évidence les informations affichées:"

101 "      1) Sélectionnez le bouton Afficher."

102 "      2) Choisissez les éléments à mettre en évidence."

103 "      3) Sélectionnez le bouton Appliquer."

104 ""

105 "Pour envoyer un message:"

106 "      1) Sélectionnez le bouton Messages."

107 "      2) Rédigez le message."

108 "      3) Sélectionnez le bouton Envoyer."

109 ""

110 "Pour stocker un message:"

111 "      1) Sélectionnez le bouton Message."

112 "      2) Rédigez le message."

113 "      3) Sélectionnez le bouton Ajouter."

114 ""

115 "Pour envoyer un message stocké:"

116 "      Sélectionnez le message à partir du menu Envoyer."

117 ""

118 "Pour supprimer les messages de la fenêtre, sélectionnez le bouton Effacer."

119 ""

120 "ToolTalk - Généralités"

121 "----------------------"

122 "Avant tout envoi ou réception de messages orientés objet"

123 "par l'intermédiaire de ToolTalk, un processus doit"

124 "s'inscrire auprès du programme de transmission, afin"

125 "de fournir les informations suivantes à ToolTalk:"

126 ""

127 "      Le type de processus (ptype),"

128 "      qui permet au programme de transmission"

129 "      d'adresser au processus les messages"

130 "      générés par ce ptype."

131 ""

132 "      Les sessions dans lesquelles le processus est impliqué."

133 ""

134 "      Les documents traités par le processus."

135 "      Les messages relatifs aux objets de"

136 "      ces documents sont alors transmis"

137 "      au processus."

138 ""

139 "Généralement, cette inscription modifie"

140 "les modèles générés. Les appels API de"

141 "manipulation de modèles pourraient être utilisés"

142 "pour visualiser ou gérer les messages orientés"

143 "objet, mais les méthodes décrites ici sont plus"

144 "rapides, étant donné qu'elles utilisent les"

145 "informations indiquées dans les définitions de types."

146 ""

147 "Pour ouvrir une session, utilisez tt_session_join."

148 "Lors de cette opération, vous devez indiquer l'ID"

149 "de la session à ouvrir."

150 ""

151 "Une fois que votre programme ne requiert plus les fonctions"

152 "ToolTalk, mettez fin à la session à l'aide de tt_session_quit."

153 ""

154 "Lorsque votre programme charge un fichier, il utilise"

155 "tt_file_join pour se joindre au groupe de processus"

156 "intéressés par ce fichier. Une fois que celui-ci n'est"

157 "plus nécessaire, le programme utilise tt_file_quit()"

158 "pour quitter le groupe. Certains programmes peuvent ouvrir"

159 "plusieurs fichiers simultanément, d'autres peuvent n'en ouvrir qu'un."

160 "Utilisation de TTSnoop"

161 "----------------------"

162 "Pour visualiser les messages, sélectionnez l'option Lancer."

163 "Pour désactiver l'affichage des messages, sélectionnez l'option Arrêter."

164 ""

165 "Pour définir le type de message à afficher:"

166 "        1) Sélectionnez le bouton Modèles."

167 "        2) Indiquez le modèle voulu."

168 "        3) Sélectionnez le bouton Appliquer."

169 ""

170 "Pour mettre en évidence les informations affichées:"

171 "        1) Sélectionnez le bouton Afficher."

172 "        2) Choisissez les éléments à mettre en évidence."

173 "        3) Sélectionnez le bouton Appliquer."

174 ""

175 "Pour envoyer un message:"

176 "        1) Sélectionnez le bouton Messages."

177 "        2) Rédigez le message."

178 "        3) Sélectionnez le bouton Envoyer."

179 ""

180 "Pour stocker un message:"

181 "        1) Sélectionnez le bouton Message."

182 "        2) Rédigez le message."

183 "        3) Sélectionnez le bouton Ajouter."

184 ""

185 "Pour envoyer un message stocké:"

186 "        Sélectionnez le message à partir du menu Envoyer."

187 ""

188 "Pour supprimer les messages de la fenêtre, sélectionnez l'option Effacer."

189 ""

190 "ToolTalk - Généralités"

191 "----------------------"

192 "Avant tout envoi ou réception de messages orientés objet par"

193 "l'intermédiaire de ToolTalk, un processus doit s'inscrire auprès du programme"

194 "de transmission, afin de fournir les informations suivantes à ToolTalk:"

195 ""

196 "        Le type de processus (ptype), qui permet"

197 "        au programme de transmission d'adresser au"

198 "        processus les messages générés par ce ptype."

199 ""

200 "        Les sessions dans lesquelles le processus est impliqué."

201 ""

202 "        Les documents traités par ce processus. "

203 "        Les messages relatifs aux objets de ces documents"

204 "        sont alors transmis au processus."

205 ""

206 "Généralement, cette inscription modifie les modèles"

207 "générés. Les appels API de manipulation de modèles pourraient"

208 "être utilisés pour visualiser ou gérer les messages orientés objet,"

209 "mais les méthodes décrites ici sont plus rapides, étant donné qu'elles"

210 "utilisent les informations indiquées dans les définitions de types."

211 ""

212 "Pour ouvrir une session, utilisez tt_session_join. Lors de cette"

213 "opération, vous devez indiquer l'ID de la session à ouvrir."

214 ""

215 "Une fois que votre programme ne requiert plus les fonctions ToolTalk,"

216 "mettez fin à la session à l'aide de tt_session_quit."

217 ""

218 "Lorsque votre programme charge un fichier, il utilise tt_file_join"

219 "pour se joindre au groupe de processus intéressés par ce fichier."

220 "Une fois que celui-ci n'est plus nécessaire, le programme utilise tt_file_quit() pour"

221 "quitter le groupe. Certains programmes peuvent ouvrir plusieurs fichiers"

222 "simultanément, d'autres peuvent n'en ouvrir qu'un."

223 "Lancer/Arrêter la réception et l'affichage des messages."

224 "Effacer la fenêtre de messages."

225 "Afficher"

226 "--------"

227 "Lorsqu'un message destiné à votre processus est transmis, un descripteur"

228 "de fichier est activé. Selon la structure du programme, vous pouvez"

229 "faire en sorte que les outils de développement du système de fenêtres appellent"

230 "une fonction lorsque le descripteur est activé, ou inclure ce dernier"

231 "dans un appel select(3) délimitant un certain nombre de descripteurs de fichiers."

232 "Dans un cas comme dans l'autre, une fois le descripteur activé,"

233 "tt_message_receive() est appelé, afin d'obtenir un identificateur pour le message"

234 "contenant le message en entrée. Les appels tt_message_*() permettent d'examiner"

235 "les attributs du message, afin de décider de l'action à entreprendre."

236 "Pour identifier les réponses aux messages que vous avez envoyés, comparez"

237 "les identificateurs, en indiquant les informations appropriées dans"

238 "le message à l'aide de l'appel tt_message_user_set(), ou en appliquant"

239 "des fonctions spécifiques aux messages et aux modèles à l'aide"

240 "des appels tt_message_callback_add et tt_pattern_callback_add."

241 "Messages"

242 "--------"

243 "Avant d'envoyer un message, vous devez tout d'abord l'allouer, puis indiquer"

244 "les informations appropriées. Les réponses apparaissent dans le même message."

245 "Une fois l'opération terminée, libérez l'espace utilisé pour le message. Dans le"

246 "cas de requêtes exigeant le retour de valeurs, cette opération intervient"

247 "après la copie des valeurs. Pour les notifications, elle est effectuée après l'envoi."

248 ""

249 "Pour allouer le nouveau message, utilisez tt_message_create(). Un"

250 "\"identificateur\" ou un \"pointeur opaque\" est retourné ; utilisez-le, lors"

251 "des appels suivants, pour faire référence à ce message."

252 ""

253 "Pour rédiger le message, utilisez les appels tt_message_*_set() ;"

254 "il en existe un pour chaque attribut du message."

255 ""

256 "Vous devez remplir les zones Class, Address, Op et Args."

257 ""

258 "Pour envoyer le message, utilisez tt_message_send()."

259 "Modèles"

260 "-------"

261 "Les messages n'étant pas adressés de façon explicite à un destinataire donné,"

262 "une méthode de \"mise en correspondance\" est utilisée"

263 "pour identifier le destinataire. La description des messages est enregistrée par"

264 "les programmes intéressés, puis utilisée par ToolTalk pour identifier le destinataire"

265 "du message. Les messages que le programme gère et ceux qu'il"

266 "affiche appartiennent à des groupes de modèles différents."

267 "Les programmes souhaitant recevoir des messages se font connaître"

268 "dynamiquement lors de l'exécution ou statiquement lors de l'installation."

269 "Dans le premier cas, les messages sont comparés à un groupe de modèles."

270 "Si un message correspond à un modèle, le programme ayant enregistré"

271 "ce dernier peut recevoir le message."

272 ""

273 "Avant de recevoir des messages par l'intermédiaire de ToolTalk, un processus"

274 "doit s'inscrire auprès du programme de transmission des messages, afin de fournir"

275 "des modèles à ToolTalk. Celui-ci transmet alors au processus les messages"

276 "correspondant aux modèles. Ceux-ci peuvent être créés de deux manières:"

277 ""

278 "Statique, par l'intermédiaire d'un ptype. Un processus définit son ptype auprès du "

279 "programme de transmission de messages. ToolTalk génère alors des modèles à partir de chaque signature"

280 "du ptype. Les modèles générés peuvent être modifiés par ouverture et fermeture"

281 "de sessions et de fichiers, comme lors de la transmission des messages orientés objet."

282 ""

283 "Dynamique: Un processus peut créer des modèles \"en temps réel\" et les enregistrer"

284 "à l'aide de ToolTalk. Cette fonction peut notamment être utilisée dans le cadre"

285 "d'un programme de consignation qui examine les messages générés et les"

286 "affiche dans une fenêtre (pour les protocoles de messages de débogage,"

287 "par exemple)."

288 ""

289 "Cette application n'utilise que les modèles dynamiques."

290 ""

291 "Avant d'enregistrer un modèle, vous devez tout d'abord l'allouer, puis indiquer"

292 "les informations appropriées. Une fois l'opération terminée, c'est-à-dire lorsque"

293 "vous n'avez plus besoin des messages associés au modèle, libérez"

294 "l'espace utilisé pour ce dernier."

295 ""

296 "Pour allouer le nouveau modèle, utilisez tt_pattern_create(). Un"

297 "\"identificateur\" ou \"pointeur opaque\" est retourné ; utilisez-le,"

298 "lors des appels suivants, pour faire référence à ce modèle."

299 ""

300 "Pour définir le modèle, utilisez les appels tt_pattern_*_add() ;"

301 "il en existe un pour chaque attribut du modèle. Plusieurs valeurs peuvent"

302 "être indiquées pour chaque attribut du modèle. Un attribut de modèle"

303 "correspond à un attribut de message si une valeur du modèle correspond"

304 "à celle du message."

305 ""

306 "Cette application utilise des paramètres non exclusifs. Les valeurs d'attributs"

307 "doivent être séparées par des espaces."

308 ""

309 "Pour faire correspondre les messages sans prendre en compte la valeur d'un"

310 "attribut donné, n'indiquez pas l'attribut pour le modèle."

311 ""

312 "Si aucun attribut n'est indiqué pour le modèle, ToolTalk considère que la"

313 "correspondance est établie pour l'attribut du message. Moins vous indiquez"

314 "d'attributs de modèle, plus vous êtes susceptible de recevoir des messages."

315 ""

316 "Si plusieurs valeurs sont indiquées pour un attribut de modèle, au moins une"

317 "valeur doit correspondre à celle de l'attribut du message. Si tel n'est pas"

318 "le cas, ToolTalk ne considère pas l'application comme destinataire potentiel."

319 ""

320 "Les attributs Category et Scope doivent être définis."

321 ""

322 "Une fois le modèle terminé, enregistrez-le à l'aide de"

323 "tt_pattern_register(), puis ouvrez les sessions ou les fichiers voulus"

324 "à l'aide du bouton Appliquer."

325 ""

326 "L'objectif principal de cette opération étant la mise à jour des modèles déjà"

327 "enregistrés, ceux que vous enregistrez après l'ouverture ne sont pas mis à"

328 "jour. Vous devez soit enregistrer tous les modèles avant l'ouverture, soit"

329 "réouvrir les sessions et les fichiers après l'enregistrement des nouveaux modèles."

330 "Ceci s'applique également aux modèles enregistrés de façon implicite, par ouverture de ptypes."

331 "L'ajout de messages à l'aide de la fenêtre Messages entraîne l'augmentation du nombre"

332 "d'options du menu associé au bouton Messages. Pour envoyer un messages, sélectionnez-le."

333 "Address:"

334 "Etant donné qu'il existe de nombreux types de programmes, auxquels de nombreux"

335 "utilisateurs ont accès, l'expéditeur d'un message est généralement dans"

336 "l'impossibilité d'identifier précisément son destinataire."

337 "Il indique un nom d'opération correspondant au contenu du message et, "

338 "éventuellement, un objet auquel celui-ci doit être adressé ou"

339 "le type d'objet (otype)."

340 ""

341 "Utilisez l'objet ou l'otype pour les messages orientés objet. Si l'adresse"

342 "est un identificateur, indiquez-le."

343 ""

344 "        TT_PROCEDURE: Op et Args."

345 ""

346 "        TT_OBJECT:    Object, Op et Args."

347 ""

348 "        TT_HANDLER:   Handler, Op et Args."

349 ""

350 "        TT_OTYPE:     Otype, Op et Args."

351 "Handler:"

352 "Si vous connaissez le PID de l'identificateur, vous pouvez lui adresser"

353 "les messages directement. Généralement, un processus lance une requête"

354 "générale, extrait l'attribut Handler de la réponse, puis adresse les messages"

355 "suivants à cet identificateur. Ainsi, deux processus peuvent communiquer"

356 "à l'aide de la transmission de messages à diffusion générale, "

357 "et échanger des informations."

358 "Handler_Ptype:"

359 "Si vous connaissez le ptype du processus qui va gérer le message, indiquez-le"

360 "ici. Cette information figure dans la définition du ptype, qui indique"

361 "les détails relatifs au protocole."

362 "Object:"

363 "Indiquez l'ID de l'objet auquel le message doit être adressé."

364 "OType:"

365 "Type d'objet."

366 "Op:"

367 "Indiquez l'opération qui décrit la notification ou la requête en cours."

368 "Pour connaître le nom de l'opération, consultez la définition "

369 "d'otype de l'objet cible."

370 "Opnum:"

371 "Numéro d'opération, qui permet d'identifier les opérations portant le même"

372 "nom mais associées à des types d'arguments différents, et de"

373 "simplifier la répartition \"interne\" d'un client (en appelant"

374 "la procédure interne correspondant à un message donné)."

375 "Scope:"

376 "Utilisez une session étendue si vous souhaitez recevoir les messages provenant"

377 "d'autres processus s'exécutant dans la même session. Utilisez un fichier étendu si"

378 "vous voulez recevoir les messages relatifs à un fichier."

379 ""

380 "      TT_SESSION:          Réception des messages provenant d'autres processus"

381 "                           dans la même session."

382 ""

383 "      TT_FILE:             Réception de messages sur le fichier ouvert."

384 ""

385 "      TT_BOTH:             Réception de messages sur un fichier et la"

386 "                           session."

387 ""

388 "      TT_FILE_IN_SESSION:  Réception de messages relatifs au fichier ouvert"

389 "                           au cours de la session."

390 "Session:"

391 "Dans le cas d'une session étendue, indiquez la session (tt_default_session() "

392 "par défaut."

393 "File:"

394 "Dans le cas d'un fichier étendu, indiquez le nom du fichier."

395 "Category:"

396 "Utilisez la catégorie observe (affichage) pour consulter les messages, et la"

397 "catégorie handle (identificateur) si vous souhaitez être le seul identificateur du message."

398 ""

399 "    TT_OBSERVE: Les processus \"observateurs\" utilisent des copies"

400 "                du message. Un nombre illimité de processus peut visualiser"

401 "                un message. Même s'il s'agit d'une requête, le processus"

402 "                ne peut pas renvoyer de valeur à l'expéditeur. Généralement, "

403 "                les actions effectuées par les processus affectent"

404 "                uniquement l'affichage interactif des données sous-jacentes."

405 ""

406 "    TT_HANDLE:  L'action exécutée par le processus \"gestionnaire\" dépend du message."

407 "                Un seul processus gère n'importe quel message donné."

408 "                S'il s'agit d'une requête, le processus gestionnaire"

409 "                est celui qui renvoie des valeurs. Généralement,"

410 "                les actions entreprises par un gestionnaire affectent"

411 "                les données permanentes du programme."

412 "Class:"

413 "Utilisez cette requête pour les messages qui renvoient des valeurs, lorsque vous"

414 "voulez être averti lors de l'identification ou de la mise en attente d'un message,"

415 "ou lorsqu'un processus destiné à gérer la requête est lancé. La notification"

416 "est destinée aux messages qui communiquent des événements aux autres processus."

417 ""

418 "    TT_NOTICE:   Les notices sont des messages qui indiquent qu'un"

419 "                 événement s'est produit. Les programmes expéditeurs"

420 "                 n'attendent pas de réponse. Les messages ne sont"

421 "                 fournis qu'à titre d'information."

422 ""

423 "     TT_REQUEST: Les requêtes sont des messages qui demandent à un"

424 "                 programme d'exécuter une action. Généralement, le programme"

425 "                 demandeur attend une valeur de retour en réponse"

426 "                 à la requête. Même lorsqu'aucune valeur n'est retournée,"

427 "                 le programme qui traite la requête envoie une réponse"

428 "                 qui indique si elle a abouti ou échoué."

429 "Disposition:"

430 "Indique l'action à entreprendre lorsque le message ne peut être géré par aucun"

431 "processus en cours: Queue si le message doit être mis en attente jusqu'à ce qu'un"

432 "processus Handler_Ptype soit lancé ; Start si un processus Handler_Ptype"

433 "doit être lancé."

434 ""

435 "    TT_QUEUE: Met le message en attente jusqu'à ce qu'un processus du"

436 "              type approprié le reçoive."

437 ""

438 "    TT_START: Tente de lancer un processus du type approprié,"

439 "              si nécessaire."

440 ""

441 "Les valeurs de tt_disposition peuvent être cumulées, de sorte que"

442 "TT_QUEUE+TT_START indique à la fois la mise en attente du message et le lancement"

443 "d'un processus. Cette fonction peut être utile en cas d'échec du lancement,"

444 "(ou si celui-ci est annulé par l'utilisateur), afin d'assurer le traitement du message"

445 "dès qu'un processus approprié est lancé."

446 "State:"

447 "Les valeurs possibles de l'attribut State sont les"

448 "suivantes:"

449 ""

450 "    TT_CREATED:  Le message a été créé mais pas envoyé. Seul son expéditeur"

451 "                 peut le visualiser."

452 ""

453 "    TT_SENT:     Le message a été envoyé mais pas identifié."

454 ""

455 "    TT_HANDLED:  Le message a été traité, les valeurs de retour sont correctes."

456 ""

457 "    TT_FAILED:   Le message n'a pas pu être transmis à un gestionnaire."

458 ""

459 "    TT_QUEUED:   Le message a été mis en attente pour transmission ultérieure."

460 ""

461 "    TT_STARTED:  Tentative de lancement d'un processus destiné à gérer le message."

462 ""

463 "    TT_REJECTED: Le message a été refusé par un gestionnaire potentiel."

464 "                 Seul le processus ayant refusé le message peut le visualiser."

465 "                 ToolTalk modifie l'état en TT_SENT avant"

466 "                 d'adresser le message à un autre gestionnaire potentiel."

467 "Status:"

468 "Numéro unique indiquant le résultat du message retourné par le processus"

469 "répondant. Ce numéro doit être supérieur à TT_ERR_LAST (2047)."

470 "Status string:"

471 "Description du statut du message."

472 "Sender:"

473 "Identifie le processus qui envoie le message."

474 "Sender ptype:"

475 "Type du processus qui envoie le message."

476 "Uid:"

477 "ID utilisateur effectif du processus qui envoie le message."

478 "Gid:"

479 "ID groupe effectif du processus qui envoie le message."

480 "Mode:"

481 "Mode d'un argument de message. Les valeurs possibles sont"

482 "les suivantes:"

483 ""

484 "    TT_IN:    L'argument est écrit par l'expéditeur et lu par "

485 "              le gestionnaire et les observateurs."

486 ""

487 "    TT_OUT:   L'argument est écrit par le gestionnaire et lu par"

488 "              l'expéditeur et les processus attendant une réponse."

489 ""

490 "    TT_INOUT: L'argument est écrit et lu par l'expéditeur, le"

491 "              gestionnaire et les processus observateurs."

492 "Type:"

493 "Chaîne indiquant le type de message. Les valeurs courantes sont "

494 "\"string\" et \"int\", mais les développeurs peuvent en définir d'autres."

495 "Value:"

496 "Données réelles de la valeur dépendant du type."

497 "Lorsque vous sélectionnez le bouton Appliquer, les valeurs des paramètres"

498 "indiqués s'affichent pour chaque message reçu."

499 "Les valeurs d'affichage sont réinitialisées, en fonction de celle définies"

500 "lors de la dernière sélection du bouton Appliquer ou de la valeur initiale."

501 "Edit Receive Contexts:"

502 "Vous pouvez ajouter, supprimer ou modifier les contextes à recevoir."

503 "Mise en correspondance"

504 "----------------------"

505 "Les messages n'étant pas adressés de façon explicite à un destinataire donné,"

506 "une méthode de \"mise en correspondance\" est utilisée pour identifier"

507 "le destinataires. La description des messages est enregistrée par les"

508 "programmes intéressés, puis utilisée par ToolTalk pour identifier le destinataire"

509 "du message. Les messages que le programme gère et ceux qu'il"

510 "affiche appartiennent à des groupes de modèles différents."

511 "Les programmes souhaitant recevoir des messages se font connaître"

512 "dynamiquement lors de l'exécution ou statiquement lors de"

513 "l'installation. Dans le premier cas, les messages sont comparés"

514 "à un groupe de modèles. Si un message correspond à un modèle,"

515 "le programme ayant enregistré ce dernier peut recevoir"

516 "le message."

517 ""

518 "Avant de recevoir des messages par l'intermédiaire de ToolTalk, un processus"

519 "doit s'inscrire auprès du programme de transmission des messages, afin de fournir"

520 "des modèles à ToolTalk. Celui-ci transmet alors au processus les messages"

521 "correspondant aux modèles. Ceux-ci peuvent être créés de deux manières:"

522 ""

523 "Statique, par l'intermédiaire d'un ptype. Un processus définit son ptype auprès du"

524 "programme de transmission de messages. ToolTalk génère alors des modèles à partir de chaque signature"

525 "du ptype. Les modèles générés peuvent être modifiés par ouverture et fermeture"

526 "de sessions et de fichiers, comme lors de la transmission de messages orientés objet."

527 ""

528 "Dynamique: Un processus peut créer des modèles \"en temps réel\" et les enregistrer"

529 "à l'aide de ToolTalk. Cette fonction peut notamment être utilisée dans le cadre"

530 "d'un programme de consignation qui examine les messages générés et les"

531 "affiche dans une fenêtre (pour les protocoles de messages de débogage,"

532 "par exemple)."

533 ""

534 "Cette application n'utilise que les modèles dynamiques."

535 ""

536 "Avant d'enregistrer un modèle, vous devez tout d'abord l'allouer, puis indiquer"

537 "les informations appropriées. Une fois l'opération terminée, c'est-à-dire lorsque"

538 "vous n'avez plus besoin des messages associés au modèle, libérez"

539 "l'espace utilisé pour ce dernier."

540 ""

541 "Pour allouer le nouveau modèle, utilisez tt_pattern_create(). Un"

542 "\"identificateur\" ou \"pointeur opaque\" est retourné ; utilisez-le,"

543 "lors des appels suivants, pour faire référence à ce modèle."

544 ""

545 "Pour définir le modèle, utilisez les appels tt_pattern_*_add() ;"

546 "il en existe un pour chaque attribut du modèle. Plusieurs valeurs peuvent"

547 "être indiquées pour chaque attribut du modèle. Un attribut de modèle"

548 "correspond à un attribut de message si une valeur du modèle correspond"

549 "à celle du message."

550 ""

551 "Cette application utilise des paramètres non exclusifs. Les valeurs d'attributs"

552 "doivent être séparées par des espaces."

553 ""

554 "Pour faire correspondre les messages sans prendre en compte la valeur d'un"

555 "attribut donné, n'indiquez pas l'attribut pour le modèle."

556 ""

557 "Si aucun attribut n'est indiqué pour le modèle, ToolTalk considère que la"

558 "correspondance est établie pour l'attribut du message. Moins vous indiquez"

559 "d'attributs de modèles, plus vous êtes susceptible de recevoir des messages."

560 ""

561 "Si plusieurs valeurs sont indiquées pour un attribut de modèle, au moins une"

562 "valeur doit correspondre à celle de l'attribut du message. Si tel n'est pas"

563 "le cas, ToolTalk ne considère pas l'application comme destinataire potentiel."

564 ""

565 "Les attributs Category et Scope doivent être définis."

566 ""

567 "Une fois le modèle terminé, enregistrez-le à l'aide de"

568 "tt_pattern_register(), puis ouvrez les sessions ou les fichiers voulus"

569 "à l'aide du bouton Appliquer."

570 ""

571 "L'objectif principal de cette opération étant la mise à jour des modèles déjà"

572 "enregistrés, ceux que vous enregistrez après l'ouverture ne sont pas mis à"

573 "jour. Vous devez soit enregistrer tous les modèles avant l'ouverture, soit"

574 "réouvrir les sessions et les fichiers après l'enregistrement des nouveaux modèles."

575 "Ceci s'applique également aux modèles enregistrés de façon implicite,"

576 "par ouverture de ptypes."

577 "Address:"

578 "Etant donné qu'il existe de nombreux types de programmes, auxquels de nombreux"

579 "utilisateurs ont accès, l'expéditeur d'un message est généralement dans"

580 "l'impossibilité d'identifier précisément son destinataire."

581 "Il indique un nom d'opération correspondant au contenu du message et,"

582 "éventuellement, un objet auquel celui-ci doit être adressé ou"

583 "le type d'objet (otype)."

584 ""

585 "Utilisez l'objet ou l'otype pour les messages orientés objet. Si l'adresse"

586 "est un identificateur, indiquez-le."

587 ""

588 "    TT_PROCEDURE: Op et Args."

589 ""

590 "    TT_OBJECT:    Object, Op et Args."

591 ""

592 "    TT_HANDLER:   Handler, Op et Args."

593 ""

594 "    TT_OTYPE:     Otype, Op et Args."

595 "Object:"

596 "Indiquez l'ID de l'objet auquel le message doit être adressé."

597 "OType:"

598 "Type d'objet."

599 "Op:"

600 "Indiquez l'opération qui décrit la notification ou la requête en cours."

601 "Pour connaître le nom de l'opération, consultez la définition"

602 "d'otype de l'objet cible."

603 "Scope:"

604 "Utilisez une session étendue si vous souhaitez recevoir les messages provenant"

605 "d'autres processus s'exécutant dans la même session. Utilisez un fichier étendu si"

606 "vous voulez recevoir les messages relatifs à un fichier."

607 ""

608 "    TT_SESSION:          Réception des messages provenant d'autres processus"

609 "                         dans la même session."

610 ""

611 "    TT_FILE:             Réception de messages sur le fichier ouvert."

612 ""

613 "    TT_BOTH:             Réception de messages sur un fichier et la session."

614 ""

615 "    TT_FILE_IN_SESSION:  Réception de messages relatifs au fichier ouvert"

616 "                         au cours de la session."

617 "File:"

618 "Dans le cas d'un fichier étendu, indiquez le nom du fichier."

619 "Session:"

620 "Dans le cas d'une session étendue, indiquez la session (tt_default_session()"

621 "par défaut."

622 "Category:"

623 "Utilisez la catégorie observe (affichage) pour consulter les messages, et la"

624 "catégorie handle (identificateur) si vous souhaitez être le seul identificateur du message."

625 ""

626 "    TT_OBSERVE: Les processus observateurs utilisent des copies"

627 "                du message. Un nombre illimité de processus peut visualiser"

628 "                un message. Même s'il s'agit d'une requête, le processus"

629 "                ne peut pas renvoyer de valeur à l'expéditeur. Généralement, "

630 "                les actions effectuées par le processus affectent"

631 "                uniquement l'affichage interactif des données sous-jacentes."

632 ""

633 "    TT_HANDLE:  L'action exécutée par le processus gestionnaire dépend du message."

634 "                Un seul processus gère n'importe quel message donné."

635 "                S'il s'agit d'une requête, le processus gestionnaire"

636 "                est celui qui renvoie des valeurs. Généralement,"

637 "                les actions entreprises par un gestionnaire affectent"

638 "                les données permanentes du programme."

639 "Class:"

640 "Utilisez cette requête pour les messages qui renvoient des valeurs, lorsque vous"

641 "voulez être averti lors de l'identification ou de la mise en attente d'un message,"

642 "ou lorsqu'un processus destiné à gérer la requête est lancé. La notification"

643 "est destinée aux messages qui communiquent des événements aux autres processus."

644 ""

645 "    TT_NOTICE:  Les notices sont des messages qui indiquent qu'un"

646 "                événement s'est produit. Les programmes expéditeurs"

647 "                n'attendent pas de réponse. Les messages ne sont"

648 "                fournis qu'à titre d'information."

649 ""

650 "    TT_REQUEST: Les requêtes sont des messages qui demandent à un"

651 "                programme d'exécuter une action. Généralement, le programme"

652 "                attend une valeur de retour en réponse"

653 "                à la requête. Même lorsqu'aucune valeur n'est retournée,"

654 "                le programme qui traite la requête envoie une réponse"

655 "                qui indique si elle a abouti ou échoué."

656 "State:"

657 "Les valeurs possibles de l'attribut State sont les"

658 "suivantes:"

659 ""

660 "    TT_CREATED:  Le message a été créé mais pas envoyé. Seul son expéditeur"

661 "                 peut le visualiser."

662 ""

663 "    TT_SENT:     Le message a été envoyé mais pas identifié."

664 ""

665 "    TT_HANDLED:  Le message a été traité, les valeurs de retour sont correctes."

666 ""

667 "    TT_FAILED:   Le message n'a pas pu être transmis à un gestionnaire."

668 ""

669 "    TT_QUEUED:   Le message a été mis en attente pour transmission ultérieure."

670 ""

671 "    TT_STARTED:  Tentative de lancement d'un processus destiné à gérer le message."

672 ""

673 "    TT_REJECTED: Le message a été rejeté par un gestionnaire potentiel."

674 "                 Seul le processus ayant rejeté le message peut le visualiser."

675 "                 ToolTalk modifie l'état en TT_SENT avant"

676 "                 d'adresser le message à un autre gestionnaire potentiel."

677 "Disposition:"

678 "Indique l'action à entreprendre lorsque le message ne peut être géré par aucun"

679 "processus en cours: Queue si le message doit être mis en attente jusqu'à ce qu'un"

680 "processus Handler_Ptype soit lancé ; Start si un processus Handler_Ptype"

681 "doit être lancé."

682 ""

683 "    TT_QUEUE: Met le message en attente jusqu'à ce qu'un processus du"

684 "              type approprié le reçoive."

685 ""

686 "    TT_START: Tente de lancer un processus du type approprié,"

687 "              si nécessaire."

688 ""

689 "Les valeurs de tt_disposition peuvent être cumulées, de sorte que"

690 "TT_QUEUE+TT_START indique à la fois la mise en attente du message et le lancement"

691 "d'un processus. Cette fonction peut être utile en cas d'échec du lancement,"

692 "(ou si celui-ci est annulé par l'utilisateur), afin d'assurer le traitement du message"

693 "dès qu'un processus approprié est lancé."

694 "Sender:"

695 "Identifie le processus qui envoie le message."

696 "Sender ptype:"

697 "Type du processus qui envoie le message."

698 "Args:"

699 "Indiquez les arguments spécifiques de l'opération. Utilisez tt_message_arg_add"

700 "pour ajouter les arguments. Pour chacun d'entre eux, vous devez indiquer le"

701 "mode (in, out ou inout), le type et, si le mode est in ou inout, la valeur."

702 "Mode:"

703 "Indique le mode d'un argument de message. Les valeurs possibles sont"

704 "les suivantes:"

705 ""

706 "    TT_IN:    L'argument est écrit par l'expéditeur et lu par"

707 "              le gestionnaire et les observateurs."

708 ""

709 "    TT_OUT:   L'argument est écrit par le gestionnaire et lu par"

710 "              l'expéditeur et les processus attendant une réponse."

711 ""

712 "    TT_INOUT: L'argument est écrit et lu par l'expéditeur, le"

713 "              gestionnaire et les processus observateurs."

714 "Type:"

715 "Chaîne indiquant le type de message. Les valeurs courantes sont \"string\" et \"int\","

716 "mais les développeurs peuvent en définir d'autres."

717 "Value:"

718 "Données réelles de la valeur dépendant du type."

719 "Bouton Ajouter:"

720 "Ajoute un argument ayant le mode, le type et la valeur indiqués."

721 "Bouton Supprimer:"

722 "Supprime l'argument sélectionné."

723 "Bouton Modifier:"

724 "Modifie l'argument sélectionné en fonction des valeurs en cours de Mode, Type"

725 "et Value."

726 "Bouton Appliquer:"

727 "Permet de recevoir les messages correspondant à ce modèle."

728 "Name:"

729 "Nom du contexte."

730 "Value:"

731 "Valeur correspondant au type:"

732 ""

733 "55 pour int (entier), \"fred\" pour char (caractères) ou ^V^L^X pour"

734 "bytes (octets)."

735 "Ajouter contexte:"

736 "Ajoute les valeurs en cours des zones Name et Valeur à la liste"

737 "de contextes."

738 "Supprimer contexte:"

739 "Supprime l'entrée sélectionnée de la liste de contextes."

740 "Modifier contexte:"

741 "Modifie l'entrée sélectionnée en fonction des valeurs des zones Nom et Valeur."

742 "Edit Send Contexts:"

743 "Fenêtre en incrustation permettant de modifier la liste des contextes à envoyer."

744 "Message"

745 "-------"

746 "Avant d'envoyer un message, vous devez tout d'abord l'allouer, puis indiquer"

747 "les informations appropriées. Les réponses apparaissent dans le même message."

748 "Une fois l'opération terminée, libérez l'espace utilisé pour le message. Dans le"

749 "cas de requêtes exigeant le retour de valeurs, cette opération intervient"

750 "après la copie des valeurs. Pour les notifications, elle est effectuée après l'envoi."

751 ""

752 "Pour allouer le nouveau message, utilisez tt_message_create(). Un"

753 "\"identificateur\" ou \"pointeur opaque\" est retourné ; utilisez-le, lors"

754 "des appels suivants, pour faire référence à ce message."

755 ""

756 "Pour rédiger le message, utilisez les appels tt_message_*_set() ;"

757 "il en existe un pour chaque attribut du message."

758 ""

759 "Vous devez remplir les zones Class, Address, Op et Args."

760 ""

761 "Pour envoyer le message, utilisez tt_message_send()."

762 "Adresse:"

763 "Etant donné qu'il existe de nombreux types de programmes, auxquels de nombreux"

764 "utilisateurs ont accès, l'expéditeur d'un message est généralement dans"

765 "l'impossibilité d'identifier précisément son destinataire."

766 "Il indique un nom d'opération correspondant au contenu du message et,"

767 "éventuellement, un objet auquel celui-ci doit être adressé ou"

768 "le type d'objet (otype)."

769 ""

770 "Utilisez l'objet ou l'otype pour les messages orientés objet. Si l'adresse"

771 "est un identificateur, indiquez-le."

772 ""

773 "    TT_PROCEDURE: Op et Args."

774 ""

775 "    TT_OBJECT:    Object, Op et Args."

776 ""

777 "    TT_HANDLER:   Handler, Op et Args."

778 ""

779 "    TT_OTYPE:     Otype, Op et Args."

780 "Handler:"

781 "Si vous connaissez le PID de l'identificateur, vous pouvez lui adresser"

782 "les messages directement. Généralement, un processus lance une requête"

783 "générale, extrait l'attribut Handler de la réponse, puis adresse les messages"

784 "suivants à cet identificateur. Ainsi, deux processus peuvent communiquer"

785 "à l'aide de la transmission de messages à diffusion générale,"

786 "et échanger des informations."

787 "Handler_Ptype:"

788 "Si vous connaissez le ptype du processus qui va gérer le message, indiquez-le"

789 "ici. Cette information figure dans la définition du ptype, qui indique"

790 "les détails relatifs au protocole."

791 "Object:"

792 "Indiquez l'ID de l'objet auquel le message doit être adressé."

793 "OType:"

794 "Type d'objet."

795 "Op:"

796 "Indiquez l'opération qui décrit la notification ou la requête en cours."

797 "Pour connaître le nom de l'opération, consultez la définition"

798 "d'otype de l'objet cible."

799 "Scope:"

800 "Utilisez une session étendue si vous souhaitez recevoir les messages provenant"

801 "d'autres processus s'exécutant dans la même session. Utilisez un fichier étendu si"

802 "vous voulez recevoir les messages relatifs à un fichier."

803 ""

804 "    TT_SESSION:          Réception des messages provenant d'autres processus"

805 "                         dans la même session."

806 ""

807 "    TT_FILE:             Réception des messages sur le fichier ouvert."

808 ""

809 "    TT_BOTH:             Réception des messages sur un fichier et la session."

810 ""

811 "    TT_FILE_IN_SESSION:  Réception des messages relatifs au fichier ouvert"

812 "                         au cours de la session."

813 "Session:"

814 "Dans le cas d'une session étendue, indiquez la session (tt_default_session()"

815 "par défaut."

816 "File:"

817 "Dans le cas d'un fichier étendu, indiquez le nom du fichier."

818 "Class:"

819 "Utilisez cette requête pour les messages qui renvoient des valeurs, lorsque vous"

820 "voulez être averti lors de l'identification ou de la mise en attente d'un message,"

821 "ou lorsqu'un processus destiné à gérer la requête est lancé. La notification"

822 "est destinée aux messages qui communiquent des événements aux autres processus."

823 ""

824 "    TT_NOTICE:  Les notices sont des messages qui indiquent qu'un"

825 "                événement s'est produit. Les programmes expéditeurs"

826 "                n'attendent pas de réponse. Les messages ne sont"

827 "                fournis qu'à titre d'information."

828 ""

829 "    TT_REQUEST: Les requêtes sont des messages qui demandent à un"

830 "                programme d'exécuter une action. Généralement, le programme"

831 "                demandeur attend une valeur de retour en réponse"

832 "                à la requête. Même lorsqu'aucune valeur n'est retournée,"

833 "                le programme qui traite la requête envoie une réponse"

834 "               qui indique si elle a abouti ou échoué."

835 "Disposition:"

836 "Indique l'action à entreprendre lorsque le message ne peut être géré par aucun"

837 "processus en cours: Queue si le message doit être mis en attente jusqu'à ce qu'un"

838 "processus Handler_Ptype soit lancé ; Start si un processus Handler_Ptype"

839 "doit être lancé."

840 ""

841 "    TT_QUEUE: Met le message en attente jusqu'à ce qu'un processus du"

842 "              type approprié le reçoive."

843 ""

844 "    TT_START: Tentative de lancer un processus du type approprié,"

845 "              si nécessaire."

846 ""

847 "Les valeurs de tt_disposition peuvent être cumulées, de sorte que"

848 "TT_QUEUE+TT_START indique à la fois la mise en attente du messages et le lancement"

849 "d'un processus. Cette fonction peut être utile en cas d'échec du lancement,"

850 "(ou celui-ci est annulé par l'utilisateur), afin d'assurer le traitement du message"

851 "dès qu'un processus approprié est lancé."

852 "Sender_Ptype:"

853 "Type du processus qui envoie le message."

854 "Status:"

855 "Numéro unique indiquant le résultat du message retourné par le processus"

856 "répondant. Ce numéro doit être supérieur à TT_ERR_LAST (2047)."

857 "Status string:"

858 "Description du statut du message."

859 "Args:"

860 "Indiquez les arguments spécifiques de l'opération. Utilisez"

861 "tt_message_arg_add pour ajouter les arguments. Pour chacun d'entre eux,"

862 "vous devez indiquer le mode (in, out ou inout), le type et, si le mode est"

863 "in ou inout, la valeur."

864 "Mode:"

865 "Indique le mode d'un argument de message. Les valeurs possibles sont"

866 "les suivantes:"

867 ""

868 "    TT_IN:    L'argument est écrit par l'expéditeur et lu par"

869 "              le gestionnaire et les processus observateurs."

870 ""

871 "    TT_OUT:   L'argument est écrit par le gestionnaire et lu par"

872 "              l'expéditeur et les processus attendant une réponse."

873 ""

874 "    TT_INOUT: L'argument est écrit et lu par l'expéditeur, le"

875 "              gestionnaire et les processus observateurs."

876 "Type:"

877 "Chaîne indiquant le type. Les valeurs courantes sont \"string\" et \"int\","

878 "mais les développeurs peuvent en définir d'autres."

879 "Value:"

880 "Données réelles de la valeur dépendant du type."

881 "Bouton Ajouter:"

882 "Ajoute un argument ayant le mode, le type et la valeur indiqués."

883 "Bouton Supprimer:"

884 "Supprime l'argument sélectionné."

885 "Bouton Modifier:"

886 "Modifie l'argument sélectionné en fonction des valeurs en cours de Mode, Type"

887 "et Value."

888 "Bouton Envoyer:"

889 "Envoie le message et affiche sur la sortie standard les appels Tooltalk"

890 "lancés pour créer et envoyer le message."

891 "Name:"

892 "Nom du contexte."

893 "Value:"

894 "Valeur du contexte (Type)."

895 "Ajouter:"

896 "Enregistre les valeurs indiquées dans les zones de texte en tant que nouveau contexte."

897 "Supprimer:"

898 "Supprime le contexte sélectionné."

899 "Modifier:"

900 "Modifie le contexte sélectionné en fonction des valeurs indiquées"

901 "dans les zones de texte."

902 "Impossible d'ouvrir un tube pour la sortie sieve.\n"

903 "Tts_string_list:generate appelé pour le message - opération ignorée \n"

$set 11


2 "TT_OK\tLa requête a abouti."

3 "TT_WRN_NOTFOUND\tL'objet est introuvable ; il ne peut pas être supprimé."

4 "TT_WRN_STALE_OBJID\tL'attribut d'objet du message a été remplacé: mettez à jour la source de l'ID objet."

5 "TT_WRN_STOPPED\tLa demande a été interrompue par la procédure de filtrage."

6 "TT_WRN_SAME_OBJID\tL'objet déplacé conserve le même ID."

7 "TT_WRN_START_MESSAGE\tCe message a entraîné le lancement du processus ; il doit faire l'objet d'une réponse, même s'il s'agit d'une notice."

8 "TT_WRN_APPFIRST\tCe code ne doit pas être utilisé."

9 "TT_WRN_LAST\tCe code ne doit pas être utilisé."

10 "TT_ERR_CLASS\tLa valeur de tt_class transmise est incorrecte."

11 "TT_ERR_DBAVAIL\tUne base de données requise n'est pas disponible. Il s'agit peut-être d'une indisponibilité provisoire: faites une nouvelle tentative ultérieurement."

12 "TT_ERR_DBEXIST\tUne base de données requise n'existe pas. Elle doit être créée pour que l'action puisse aboutir."

13 "TT_ERR_FILE\tL'objet fichier est introuvable."

14 "TT_ERR_MODE\tLa valeur de tt_mode est incorrecte."

15 "TT_ERR_ACCESS\tUne tentative d'accès non autorisée à un objet ToolTalk a été effectuée."

16 "TT_ERR_NOMP\tAucun processus ttsession n'est en cours d'exécution ; il est probable que tt_open() n'ait pas été appelé. Ce code, retourné par tt_open(), indique que le processus n'a pas pu être lancé, ce qui signifie généralement que ToolTalk n'est pas installé."

17 "TT_ERR_NOTHANDLER\tSeul le gestionnaire du message peut effectuer cette opération."

18 "TT_ERR_NUM\tL'entier indiqué comme valeur est incorrect."

19 "TT_ERR_OBJID\tL'ID objet indiqué ne correspond à aucune spécification d'objet existante."

20 "TT_ERR_OP\tLa syntaxe du nom d'opération indiqué est incorrecte."

21 "TT_ERR_OTYPE\tLa valeur indiquée ne correspond pas à un type d'objet installé."

22 "TT_ERR_ADDRESS\tLa valeur de tt_address indiquée est incorrecte."

23 "TT_ERR_PATH\tL'un des répertoires du chemin d'accès n'existe pas ou ne peut pas être lu."

24 "TT_ERR_POINTER\tLe pointeur opaque (identificateur) indiqué ne correspond pas à un type d'objet correct."

25 "TT_ERR_PROCID\tL'ID processus indiqué est incorrect."

26 "TT_ERR_PROPLEN\tLa valeur de propriété indiquée est trop longue."

27 "TT_ERR_PROPNAME\tLa syntaxe du nom de propriété indiqué est incorrecte."

28 "TT_ERR_PTYPE\tLa valeur indiquée ne correspond pas à un type de processus installé."

29 "TT_ERR_DISPOSITION\tLa valeur de tt_disposition indiquée est incorrecte."

30 "TT_ERR_SCOPE\tLa valeur de tt_scope indiquée est incorrecte."

31 "TT_ERR_SESSION\tL'ID session indiqué ne correspond pas à une session active."

32 "TT_ERR_VTYPE\tLa valeur de type indiquée est incorrecte."

33 "TT_ERR_NO_VALUE\tLe nom et le numéro indiqués ne correspondent à aucune valeur de propriété existante."

34 "TT_ERR_INTERNAL\tErreur interne."

35 "TT_ERR_READONLY\tImpossible de modifier l'attribut."

36 "TT_ERR_NO_MATCH\tAucun identificateur n'a été trouvé pour ce message, et la valeur de disposition n'est ni queue ni start."

37 "TT_ERR_UNIMP\tCette fonction n'est pas mise en oeuvre."

38 "TT_ERR_OVERFLOW\tTrop de messages actifs ; faites une nouvelle tentative ultérieurement."

39 "TT_ERR_PTYPE_START\tLa tentative de lancement de l'instance du ptype a échoué."

40 "TT_ERR_CATEGORY\tAucune catégorie n'est définie pour l'objet modèle."

41 "TT_ERR_DBUPDATE\tLa base de données est incohérente: un autre objet tt_spec_write a d'abord été mis à jour."

42 "TT_ERR_DBFULL\tLa base de données Tooltalk est saturée."

43 "TT_ERR_DBCONSIST\tLa base de données est altérée ou les informations d'accès sont incomplètes (lancez ttdbck)."

44 "TT_ERR_STATE\tLe tt_message est dans un tt_state incorrect pour l'opération demandée."

45 "TT_ERR_NOMEM\tMémoire insuffisante."

46 "TT_ERR_SLOTNAME\tLa syntaxe du nom d'emplacement est incorrecte."

47 "TT_ERR_XDR\tLa longueur du processus XDR proc. est égale à zéro, ou son appel n'a pas abouti."

48 "TT_ERR_NETFILE\tLa syntaxe du nom de fichier netfilename indiquée est incorrecte."

49 "TT_DESKTOP_UNMODIFIED\tCette opération n'est pas disponible pour les entités n'ayant pas été modifiées."

50 "TT_MEDIA_ERR_SIZE\tLa taille indiquée est excessive ou insuffisante."

51 "TT_MEDIA_ERR_FORMAT\tLe format des données ne correspond pas au format prévu."

52 "TT_ERR_APPFIRST\tCe code ne doit pas être utilisé."

53 "TT_ERR_LAST\tCe code ne doit pas être utilisé."

54 "TT_STATUS_LAST\tCe code ne doit pas être utilisé."

55 "Valeur de tt_status inconnue."

56 "TT_DESKTOP_EPERM\tIl ne s'agit pas d'un super-utilisateur."

57 "TT_DESKTOP_ENOENT\tCe fichier ou répertoire n'existe pas."

58 "TT_DESKTOP_EINTR\tAppel système interrompu."

59 "TT_DESKTOP_EIO\tErreur d'E-S."

60 "TT_DESKTOP_EAGAIN\tAucun processus."

61 "TT_DESKTOP_ENOMEM\tespace insuffisant."

62 "TT_DESKTOP_EACCES\tAccès refusé."

63 "TT_DESKTOP_EFAULT\tadresse incorrecte."

64 "TT_DESKTOP_EEXIST\tLe fichier existe déjà."

65 "TT_DESKTOP_ENODEV\tCette unité n'existe pas."

66 "TT_DESKTOP_ENOTDIR\tIl ne s'agit pas d'un répertoire."

67 "TT_DESKTOP_EISDIR\tIl s'agit d'un répertoire."

68 "TT_DESKTOP_EINVAL\tArgument incorrect."

69 "TT_DESKTOP_ENFILE\tSaturation table des fichiers."

70 "TT_DESKTOP_EMFILE\tTrop de fichiers ouverts."

71 "TT_DESKTOP_ETXTBSY\tFichier texte occupé."

72 "TT_DESKTOP_EFBIG\tFichier trop volumineux."

73 "TT_DESKTOP_ENOSPC\tL'unité est saturée."

74 "TT_DESKTOP_EROFS\tSystème de fichiers en lecture seule."

75 "TT_DESKTOP_EMLINK\tLiens trop nombreux."

76 "TT_DESKTOP_EPIPE\tTube interrompu."

77 "TT_DESKTOP_ENOMSG\tAucun message correspondant au type"

78 "TT_DESKTOP_EDEADLK\tBlocage."

79 "TT_DESKTOP_ENODATA\tAucune donnée disponible."

80 "TT_DESKTOP_EPROTO\tErreur de protocole."

81 "TT_DESKTOP_ENOTEMPTY\tRépertoire non vide."

82 "TT_DESKTOP_ETIMEDOUT\tDélai de connexion dépassé."

83 "TT_DESKTOP_EALREADY\tOpération déjà en cours."

84 "TT_DESKTOP_ECANCELED\topération annulée."

85 "TT_DESKTOP_ENOTSUP\tNon pris en charge."

86	"TT_ERR_TOOLATE\tThis must be the first call made into the ToolTalk API and can therefore no longer be performed."

87	"TT_ERR_AUTHORIZATION\tThe user is not authorized to connect to this ToolTalk session."

88	"TT_AUTHFILE_ACCESS\tThe user is not authorized to access the ToolTalk authority file."

89	"TT_AUTHFILE_LOCK\tCannot lock the ToolTalk authority file."

90	"TT_AUTHFILE_LOCK_TIMEOUT\tCannot lock the ToolTalk authority file."

91	"TT_AUTHFILE_UNLOCK\tCannot unlock the ToolTalk authority file."

92	"TT_AUTHFILE_MISSING\tThe ToolTalk authority file is missing."

93	"TT_AUTHFILE_ENTRY_MISSING\tThe ToolTalk authority file is missing an entry for this session."

94	"TT_AUTHFILE_WRITE\tCannot write the ToolTalk authority file."

95	"TT_ERR_VERSION_MISMATCH\tThis client is using a different RPC or program version from the server of this ToolTalk session."
