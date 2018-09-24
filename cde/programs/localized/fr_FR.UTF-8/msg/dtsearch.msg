$ $XConsortium: dtsearch.msg /main/7 1996/10/30 17:35:51 cde-ibm $
$ 
$ (c) Copyright 1996 Digital Equipment Corporation.
$ (c) Copyright 1996 Hewlett-Packard Company.
$ (c) Copyright 1996 International Business Machines Corp.
$ (c) Copyright 1996 Sun Microsystems, Inc.
$ (c) Copyright 1996 Novell, Inc. 
$ (c) Copyright 1996 FUJITSU LIMITED.
$ (c) Copyright 1996 Hitachi.

$   COMPONENT_NAME: austext
$ 
$   FUNCTIONS: database
$ 
$   ORIGINS: 27
$ 
$   IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$   combined with the aggregated modules for this product)
$   OBJECT CODE ONLY SOURCE MATERIALS
$ 
$   (C) COPYRIGHT International Business Machines Corp. 1994,1995
$   All Rights Reserved
$   US Government Users Restricted Rights - Use, duplication or
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.

$ ********************** DTSEARCH.MSG ************************
$ $Id: dtsearch.msg /main/7 1996/10/30 17:35:51 cde-ibm $
$ December 1994.
$ Messages specific to DtSearch.
$ Symbolic msg sets replaced with numbers for portability.
$ Basically there is a msgset for each module.

$ $Log$
$ Revision 2.15  1996/04/10  22:54:07  miker
$ Revision 2.14  1996/04/10  19:47:18  miker
$ Revision 2.13  1996/03/20  19:32:13  miker
$ Revision 2.12  1996/03/13  22:53:07  miker
$ Revision 2.11  1996/03/05  16:04:30  miker
$ Revision 2.10  1996/02/01  17:16:48  miker
$ Revision 2.9  1995/12/27  21:17:19  miker
$ Revision 2.8  1995/12/27  16:04:46  miker
$ Revision 2.7  1995/12/07  23:26:31  miker
$ Revision 2.6  1995/12/01  16:10:25  miker
$ Revision 2.5  1995/10/26  16:21:21  miker
$ Revision 2.4  1995/10/20  21:30:42  miker
$ Revision 2.3  1995/10/19  20:52:30  miker
$ Revision 2.2  1995/10/03  21:37:17  miker
$ Revision 2.1  1995/09/22  17:54:34  miker

$quote "

$ General comments about all messages in this file:
$ - Any message that begins with a %1$s or %s is using that variable
$   to represent the name of the executable which is causing the
$   message to be output.
$ - Any variables specified in Usage messages should be translated
$   consistently.

$ ****************************************************
$ set 1 formerly set MS_misc, used many places
$set 1

1 "<null>"
$ message 2 can be translated to match local conventions.  It is used
$ in output files for information.
2 "%d,%m,%Y/,%H:%M:%S"
3 "*** %1$sMémoire épuisée à %2$s demande %3$lu octets ! ***\n"
4 "%1$s Version %2$s.\n"
$ message 6 refers to any value which is null when printing information.
6 "<inconnue>"
8 "%1$s Il ne reste aucune base de données correcte."
9 "%1$s Argument '%2$s' de la ligne de commande incorrect.\n"
10 "%1$s L' argument '%2$s' de la ligne de commande inconnu est ignoré.\n"
11 "%1$s Nom de la base de données '%2$s 'est incorrect.\n"
$ In message 13, do not translate "dtsrcreate." It is an executable name.
13 "%1$s Pas d'enregistrement BD dans la base de données '%2$s'.\n\
  La cause en est une initialisation incorrecte de la base de données (run dtsrcreate)."
14 "%1$s La version schéma de programme '%2$s' est incompatible avec\n\
la version '%4$s' de la base de données'%3$s'.\n"
15 "%1$s Reçu un signal d'abandon %2$d.\n"
16 "<pas de variable d'environnement>"
17 "<pas de daemon de serveur>"
18 "%1$s Nom de la base de données n'est pas spécifié.\n"
19 " ,\t\n"
21 "%1$s Code de sortie = %2$d.\n"
$ Message 22 can be translated.  It represents a human-readable date like
$ "Thursday, Jan 5, 1996, 6:30 pm".  The "%5$I" is 12 hour clock time, but
$ it could be changed to "%H" for 24 hour clock time.
22 "%A %d %b %Y, %H:%M"
23 "%1$s: Version %2$s.  Run %3$s.\n"
$ Message 24 should not translate ".fzk".
24 "%1$s: répertoire de travail courant = '%2$s',fichier .fzk = '%3$s'\n"
25 "%1$s Abandon dû à une erreur du programme.\n"
27 "%1$s: %2$s: Format de fichier incorrect."
96 "%s Liste des correspondances est vide."
301 "%1$s le texte d'entrée est %2$s."
$ Msg 362 is a generic system message.  The format is
$ "<program name> <filename>: <system error>" where <system error>
$ is a system errno which will already be translated.
362 "%1$s %2$s: %3$s"
$ Msg 400 is like "<Program name>: <file name>, line <line number>:..."
400 "%1$s: %2$s, ligne %3$ld : caractères incorrects dans le mot '%4$s'."
$ Msg 423 is like "<Program name> Word <word> in <file name> is a duplicate."
423 "%1$s Le mot '%2$s' dans %3$s est en double."
1596 "%1$s %2$s Impossible d'ouvrir le fichier audit %3$s: %4$s"

$ ****************************************************
$ set 2 formerly set MS_ausapi (ausapi.*)
$set 2

1 "%1$s Le nom de la base de données est incorrect ou n'est pas disponible '%2$s'."
$ Msg 7 should not translate "dbnames" or "dbcount."
7 "%s dbnames spécifié mais pas dbcount."
20 "%1$s type_recherche incorrect '%2$c'."
30 "%s Requête Null. Aucune recherche effectuée."
$ Msg 37 should not translate "DtSearchInit."  It is a function name.
37 "%s La première fonction API appelée doit être DtSearchInit().\n"
40 "%1$s Cleartext (Supprimer texte) est Null . Pas de surbrillance."
$ Msg 115 must have the date formatted in the way specified.
115 "%1$s '%2$s' est une chaîne de date incorrecte ou incomplête.\n\
Le format correct est '[yy]yy [mm [dd]]'."
$ Msg 187 must not translate "LOGNAME."  It is an environment variable.
187 "%1$s L'Id de l'utilisateur et la variable d'environnement LOGNAME sont absentes."
198 "%s La date 'Avant' est égale ou dépasse la date 'Après'.\n\
  Les enregistrements seraient perdus."
216 "\n%1$s %2$s Caught signal %3$d.\n"
294 "%1$s Erreur du programme dans DtSearchResultsToText."
437 "%s La requête contient une chaîne de navigateur incorrecte."
492 "%1$s Il n'existe pas de liste de correspondances en cours."
498 "%1$s Args incorrects pour DtSearchResultsToText."
621 "%1$s %2$s a déjà été initialisé."
806 "%1$s Requête insuffisante ou options de recherche\n\
incompatibles avec la base de données '%2$s' pour lancer la recherche."
$ Msg 1072 "stem" is the root of a word.
1072 "%1$s Erreur de programme : Nombre de racines (%2$d) est supérieur au maximum (%3$d)."
1342 "%1$sErreur du moteur de recherche %2$d pour mettre la requête en surbrillance\n\
pour la base de données '%3$s', nombre des correspondances=%4$ld, type de recherche='%5$c', texte='%6$.30s'"


$ ****************************************************
$ set 5 formerly set MS_chandel, used in dtsrhan.c
$set 5

3 "Le fichier de sortie '%1$s' existe déjà.\n"
$ Msg 4 needs to use the same 2 characters from messages 4 and 5.
$ The "q" is not needed because it is assumed that any character
$ other than "a" or "o" means to quit the process.
4 "Annexer, écraser ou quitter? [a,o,q] "
$ Msg 5 is the shortcut key for "a" or Append from message 4.
5 "a"
$ Msg 6 is the shortcut key for "o" or overwrite from message 4.
6 "é"
7 "Impossible d'ouvrir le fichier de sortie '%1$s'.\n"
11 "\nErreur - impossible d'ouvrir le fichier profil '%1$s'.\n"
12 "Erreur ligne %1$d: identificateur incorrect '%2$s'.\n"
13 "Erreur ligne %1$d -valeur de l'identificateur '%2$s' manquante.\n"
14 "Error ligne %1$d - valeur incorrecte ou zéro du jeton '%2$s'.\njeton offensif : %3$s.\n"
$ Msg 15 should not translate "ANY."
15 "Erreur ligne %1$d - pour l'identificateur '%2$s', la colonne est définie à ANY\n\
  mais la chaîne de signature d'identification est absente.\n"
16 "Erreur ligne %1$d - valeur du jeton offensif  %3$s de l'identificateur '%2$s'\n\
est incorrecte ou zéro.\n"
18 "Erreur ligne %1$d - valeur manquante de l'identificateur '%2$s'\n"
23 "Ligne d'avertissement %1$d: Séparateur redéfini.\n"
25 "Erreur ligne %1$d: séparateur n'est pas défini en tant que 'haut ' ou 'bas'.\n"
26 "Erreur ligne %1$d: jeton incorrect '%1$s'.\n"
27 "Erreur ligne %1$d: jeton incorrect '%2$s'.\n"
$ Msg 34 needs to match the 'all' from msgs 36 and 37.
34 "tous"
$ Msg 35 needs to match the 'none' from msgs 36 and 37.
35 "aucun"
$ Msg 36 needs 'all' to match msg 34 and 'none' to match msg 35.
36 "Erreur ligne %1$d: le mode de l'image doit être 'tous' ou 'aucun' -'%2$s' inconnu.\n"
$ Msg 37 needs "all" to match msg 34 and "none" to match msg 35.
37 "Erreur ligne %1$d: le mode du texte doit être 'tous' ou 'aucun' - '%2$s' inconnu.\n"
38 "Ligne d'avertissement %1$d: Caractère-clé redéfini.\n"
39 "Erreur ligne %1$d: Caractère-clé incorrect:'%2$c'.\n"
40 "Ligne d'avertissement %1$d - zone-clé redéfinie.\n"
43 "Erreur ligne %1$d - Identificateur de clé '%2$s' incorrect.\n"
$ Msg 45 should not translate "discard."
45 "Erreur ligne %1$d: option inconnue de 'discard': '%2$s'.\n"
$ Msg 46 should not translate "delblanklines."
46 "Erreur ligne %1$d: option inconnue de 'delblanklines': '%2$s'.\n"
47 "Erreur ligne %1$d: option inconnue pour le résumé : '%2$s'\n"
48 "Erreur ligne %1$d -type d'identificateur inconnu '%2$s'.\n"
49 "Erreur - séparateur n'est pas défini.\n"
50 "Erreur - caractère de type clé n'est jamais défini.\n"
51 "Erreur - clé jamais définie.\n"
52 "Erreur - séparateur défini par '%1$s' introuvable.\n"
53 "Erreur - séparateur défini par '%1$s' fait référence à une ligne dans l'enregistrement.\n\
   Puisque le séparateur définit les lignes physiques\n\
  il ne peut pas en être une.\n"
54 "Erreur - dans la zone '%1$s', aucun identificateur de lignes ne correspond à '%2$s'.\n"
55 "Erreur - la liste inclure/exclure de la zone comprenait\n\
la zone '%1$s' qui n'a pas été définie.\n"
56 "Erreur - la liste inclure/exclure de l'image comprenait \n\
  la ligne '%1$s' qui n'a pas été définie.\n"
57 "Erreur - la liste inclure/exclure du texte comprenait \n\
  la ligne '%1$s' qui n'a pas été définie.\n"
58 "Erreur - la définition de la clé fait état de la zone '%1$s'\n\
 qui n'a pas été définie.\n"
59 "Erreur - la définition de l'abstrait fait état de la zone '%1$s'\n\
 qui n'a pas été définie.\n"
68 "Avertissement - les zones nécessaires à la clé sont introuvables.\n\
  suppression de l'enregistrement %1$ld qui commence:\n\
  %2$s\n"
69 "Avertissement - l'enregistrement %1$ld classé insignifiant, suppression en cours...\n\
  enregistrement commençait : %2$.60s\n"
70 " Impossible d'ouvrir le fichier d'entrée '%1$s'.\n"
$ Msg 71 should not translate ".fzk."
71 "\n\
SYNTAXE: %1$s [options] <profile> <infile> [<outfile>]\n\
  -m         Arrête tout sauf les messages d'erreur.\n\
  -w<N>      Change la largeur de l'écran cible en <N>.\n\
  -oo        Pré-approuve l'écrasement du fichier de sortie.\n\
  -oa        Pré-approuve l'annexion du fichier de sortie.\n\
  <profile>  Fichier d'entrée contenant les profils des enregistrements\n\
             à traiter.\n\
  <infile>   Fichier d'entrée contenant les enregistrements actuels .\n\
  <outfile>  Nom de fichier de sortie, format .fzk (%2$s).\n\n"
72 "Largeur d'écran spécifiée incorrecte.\n"
75 "'%1$s' est un mode d'entrée incorrect.\n"
76 "Argument de la ligne de commande inconnu '%1$s'.\n"
77 "nom obligatoire du fichier de profil manquant.\n"
78 "Nom du fichier d'entrée obligatoire manquant.\n"
79 "Erreur - si stdin est utilisé comme entrée, le nom du fichier de sortie est obligatoire !\n"
80 "les fichiers profil et d'entrée ont le même nom '%1$s'.\n"
81 " les fichiers de sortie et d'entrée ont le même nom:'%1$s'.\n"
82 " les fichiers profil et de sortie ont le même nom:'%1$s'.\n"
83 "        Fichier profil : %1$s\n"
$ Msg 84 should not translate "stdin."
84 "          Fichier d'entrée: stdin\n"
85 "          Fichier d'entrée: %1$s\n"
$ Msg 86 should not translate "stdout."
86 "           Fichier de sortie: stdout\n"
87 "           Fichier de sortie: %1$s\n"
88 "%1$s. %2$s %3$s Text Filter.\n"
89 "\n  Chaque point est %ld enregistrements...\n"
90 "\nErreur de fermeture du fichier de sortie - disque plein ?\n"
91 "Abandon dû à des erreurs dans le fichier profil.\n"
92 "\n%1$s: Fin normale.  %2$ld enregistrements traités.  Code de sortie = 0.\n"
93 "Erreur ligne %1$d -la chaîne '%2$s' n'est pas entre doubles guillements.\n"
110 "Avertissement ligne %d - zone de date redéfinie\n"
111 "Erreur ligne %1$d - identificateur '%2$s' incorrect pour la date.\n"
115 "%1$s Dates de l'objet par défaut seront '%2$s'.\n"
116 "Erreur -la date fait référence à une zone non définie '%s'.\n"
124 "%1$s impossible d'écrire dans le fichier de sortie '%2$s':\n  %3$s\n"
$ Msg 125 should not be translated.
125 "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC"
130 "%1$s Impossible de charger les fichiers de traitement de languague :\n%2$s\n"
133 "\n\
Avertissement - '%1$s' est une date incorrecte.\n\
  Utilisant la date '%2$s' comme numéro d'enregistrement %3$ld qui a commencé : %4$.30s\n"


$ ****************************************************
$ set 6 formerly set MS_ve
$set 6

26 "dummy_workproc() appelé.\n"
142 "%1$s Erreur du client : Enregistrement demandé a une adr. de base de données \n\
  incorrecte %2$ld (%3$d:%4$ld, x'%5$08.8lx') pour la base de données '%6$s'."
156 "%1$s Table de compression de données incompatible avec la base de données '%2$s'.\n\
  Base de données compressée avec %3$ld, le moteur de décompression est %4$ld.\n"
157 "DTSRVE157 Erreur du programme client : Adresse de la base de données est Null dans usrblk.dba."
167 "%1$s La base de données '%2$s' est vide."
178 "%1$s La  version '%3$s' de la base de données '%2$s' est incompatible\n\
  avec la version du moteur '%4$s'."
183 "%1$s Impossible d'ouvrir le fichier sémaphore des notes de l'utilisateur '%2$s': %3$s.\n"
185 "%1$s La taille maximale des mots %3$d de la base de données '%2$s' est trop courte."
199 "%1$s Impossible d'obtenir le sémaphore des notes de l'utilisateur '%2$s' en %3$d essais.\n"
230 "%1$s Impossible d'ouvrir le fichier de sauvegarde des notes de l'utilisateur '%2$s': %3$s."
231 "%1$sLa base de données '%2$s' a une taille de résumé incorrecte %3$d."
251 "%1$s La base de données '%2$s' est corrompue :\n\
Le nombre d'enregistrements ne correspond pas au nombre d'adresses.\n"
$ Msgs 310 and 311 can be appended to msg 309 to produce output.
$ For example, one message might be
$ "<Programname> User notes disabled for entire Engine."
309 "%1$s Les notes de l'utilisateur sont désactivées "
$ Msg 310 can be appended to msg 309 to produce output.
310 "dans tout le moteur."
$ Msg 311 can be appended to msg 309 to produce output.
311 "dans la base de données '%1$s'."
$ Msg 332 can be translated to local timestamp.  It currently outputs
$ something like "1996/03/04 at 16:40 CDT"
332 "%d %m %Y/d à %H:%M %Z"
333 "\n <Note de l'utilisateur annexée par '%1$s' on %2$s>\n"
398 "DTSRVE398 NULL query string."

$ ****************************************************
$ set 7 formerly set MS_uiutil, no longer used in uiutil/dtsrutil.c.

$ ****************************************************
$ set 8 formerly set MS_loadocf, for module ocf.c.

$ All words in set 8 which are capitalized should not be translated.
$ They represent keywords in a configuration file.  Words like PATH,
$ SEARCH, MAXHITS, FILEIO, KEYTYPE, FILEPATH, KEYTYPES, KEYTYPE,
$ UPPERKEYS, FALSE, and HOME.

$set 8

$ Msg 1 is "<Programname><filename>: <keyword> keyword is obsolete."
1 "%1$s%2$s: %3$s mot-clé est périmé."
$ Msg 140 "(missing)" refers to a token which is expected to be found but
$ is missing.  The token is from an input string of words being indexed.
140 "(absent)"
$ Msgs 142-168 are combined into "<token>: <error>" where msg 142 or a
$ default file name can be <token> and msgs 143, 165, 166, and 168
$ can be <error>.
142 "le reste de la ligne"
143 "est vide"
165 "id inconnu"
166 "spécifié précédemment"
168 "spécification du chemin absente"
184 "%1$s %2$s: Il y a plus de 8 caractères dans le nom de la base de données '%3$.16s'.\n"
$ Msg 221 should not translate "PATH."
221 "%1$s %2$s: Base de données '%3$s', pas de PATH spécifié, instruction ignorée.\n"
$ Msg 275 should not translate "SEARCH."
275 "%1$s %2$s: valeur incorrecte de SEARCH ignorée."
$ Msg 304 should not translate "MAXHITS."
304 "%1$s %2$s: Base de données '%3$s', valeur de MAXHITS incorrecte.\n"
352 "%1$s %2$s: Valeur de %3$s incorrecte, ligne ignorée.\n"
365 "%1$s %2$s: La valeur %3$s est manquante."
414 "%1$s Impossible d'ouvrir le fichier de configuration '%2$s':\n  %3$s."
457 "'%1$c' Enregistrements"
477 "%1$s %2$s: Non de la base de données absent après le mot-clé '%3$s'.\n"
495 "%1$s %2$s: '%3$.16s' est un mot-clé incorrect. Ligne %4$d ignorée.\n"
$ Msg 607 should not translate "FILEIO."
607 "%1$s %2$s: Valeur de FILEIO incorrecte. Pas de droits en lecture et en écriture du fichier de l'utilisateur."
610 "%1$s %2$s: %3$s value is '%4$s'. Doit être un entier positif. Ligne ignorée."
$ Msg 684 should not translate "KEYTYPE."
684 "%1$s %2$s: Base de données '%3$s', Caractère KEYTYPE incorrect '%4$c'."
732 "%1$s %2$s: Base de données '%3$s', Pas plus de %4$d keytypes autorisés.\n"
$ Msg 903 should not translate "FILEPATH."
903 "%1$s %2$s: FILEPATH '%3$s' %4$s. Ligne ignorée."
929 "%1$sVous avez ouvert l'ancien fichier de configuration du style '%2$s'."
$ Msg 986 should not translate "KEYTYPES."
986 "%1$s %2$s: KEYTYPES absent de la base de données '%3$s'."
$ Msg 1002 should not translate "KEYTYPE."
1002 "%1$s %2$s: Base de données '%3$s', Caractère KEYTYPE '%4$c' en double.\n"
1007 "%1$s%2$s: La chaîne vide %3$s ignorée."
$ Msg 1011 should not translate "KEYTYPE," "UPPERKEYS," or "FALSE."
1011 "%1$s %2$s: Base de données '%3$s': Les enregistrements avec le caractère minuscule\n\
  KEYTYPE '%4$c' seront inaccessibles.\n\
  Définir UPPERKEYS = FALSE si vous utilisez des minuscules."
1404 "%1$s Etat du fichier de configuration '%2$s': %3$s n'est pas disponible."
$ Msg 1643 should not translate "DTSROCFPATH," "HOME," "/etc/inittab,"
$ or "/etc/inetd.conf."
1643 "%1$sImpossible d'ouvrir ou de trouver\n\
le fichier de configuration '%2$s'\n\
dans aucun des emplacements suivants :\n\
  répertoire DTSROCFPATH =     %3$s,\n\
  répertoire de travail en cours = %4$s,\n\
  répertoire HOME =            %5$s,\n\
  répertoire /etc/inittab =    %6$s,\n\
  répertoire conf./etc/inetd. = %7$s."
1869 "%1$s %2$s: Le fichier de configuration doit spécifier au moins une base de données."

$ ****************************************************
$ set 9 formerly set MS_oeinit, used in oeinit.c and opendblk.c.
$set 9

$ Msg 99 should not translate "open_dblk".  It is a function name.
99 "%s Erreur de programmation : entrée incorrecte pour open_dblk()."
137 "%1$s La version %2$s de l'interface de l'utilisateur et la version %3$s du moteur sont incompatibles."
202 "%s L'initialisation a échoué à cause d'erreurs dans le fichier de configuration."
265 "%s Il ne reste aucune base de données correcte."
266 "%1$s L'initialisation a échoué à cause d'erreurs dans la base de données,\n\
  la langue ou des fichiers qui s'y rapportent."
317 "%1$s Impossible d'ouvrir le fichier de la base de données '%2$s'\n\
  Erreur %3$d = %4$s\n\
  %5$s supprime '%6$s' de la liste des bases de données disponibles."
318 "\n  Pour y remédier, spécifiez une valeur correcte PATH\n\
  de la base de données dans le fichier de configuration d'emplacement."
1404 "%1$s Supprime la base de données '%2$s' de la liste des bases de données disponibles parce que l'état du fichier %3$s: %4$s n'est pas disponible."

$ ****************************************************
$ set 10 formerly set MS_oe, used in oe.c and dbchange.c.
$set 10
48 "OE48 Requête abandonnée : la base de données '%1$s' n'est pas disponible à cette location."
71 "OE71 %1$s a expiré."
$ Msg 76 should not translate "dblk".  It is a structure name.
76 "OE76 Erreur de programme: Impossible de trouver dblk de la base de données '%1$s'."
217 "OE217 Erreur de programme: texte d'enregistrement trop long."
367 "OE367: Erreur de l'interface utilistaeur.  %1$d est un code de requête incorrect.\n"
429 "OE429 Impossible d'ouvrir le fichier de suppression '%1$s': %2$s. Requête  ignorée."
440 "OE440 Pas d'enregistrements de types de clé sélectionnés dans la base de données '%1$s'."
490 "OE490 Le moteur %1$s est désactivé de manière permanente."
516 "OE516 Un maximum de %1$d caractères est autorisé dans chaque chaîne."
523 "OE523 Requête refusée : La première requête doit être l'initialisation du moteur."
753 "%1$s Catégories sémantiques non définies pour la base de données '%2$s'."
1300 "%1$s Impossible d'accéder à l'état du fichier de la base de données\n  '%2$s': %3$s"
1313 "%1$s *** REQUETE ANNULEE *** Le moteur %2$s est réinitialisé\n\
  dû à la modification du fichier %3$s, causée probablement par la mise à jour\n\
  d'une ou de plusieurs bases de données."


$ ****************************************************
$ set 11 formerly set MS_cravel, used in dtsrload.c.
$set 11
1 "\n\
SYNTAXE : %1$s -d<dbname> [options] <infile>\n\
       Les extensions de noms de fichiers par défaut peuvent être écrasées.\n\
  -d<dbname>  nom de base de données de 1à 8 caractères, y compris le préfixe de chemin en option.\n\
              Extensions de noms de fichiers automatiquement annexées.\n\
  -t<etxstr>  chaîne marquant la fin d'un document texte. Par défaut '\\f\\n'.\n\
  -c          Initialise le compte des enregistrements de bases de données en les comptant.\n\
  -p<N>       Imprime un point à chaque enregistrement<N> (valeur par défaut %2$d).\n\
  -h<N>       Change la taille de table de hachage d'enregistrements d'id en double de %3$ld à <N>.\n\
              -h0 indique qu'il n'y a pas de doubles, ne les recherchez pas.\n\
  -e<path>    Non de fichier de chemin de la table code huffman (valeur par défaut %4$s).\n\
  <infile>    Entrée du nom de fichier [chemin].  Extension par défaut %5%s.\n"
2 "%1$s: Code de sortie = %2$d\n"
13 "%1$s Vérificateur d'id en double désactivé.\n"
14 "\n%1$s Argument '%2$s' de la ligne de commande inconnu.\n"
15 "\nNom obligatoire du fichier d'entrée absent \07"
16 "\naPas de nom de base de données spécifié (-d argument).\07"
17 "%1$s Initialisation du nombre total d'enregistrements dans la base de données effectuée en comptant...\n"
18 "%1$s: version'%2$s' du schéma = %3$s, nombre d'enregistrement = %4$ld, dernière tranche =%5$ld.\n"
19 "%1$s: Nombre total d'enregistrements de la base de données = %2$ld, dernière tranche = %3$ld.\n"
20 "%1$s %2$ld enregistrements traités dans %3$ldm %4$lds, (%5$ld%%).\n\
  %6$ld  en double, %7$ld nouveaux, %8$ld mis à jour.\n"
29 "%1$s Impossible d'obtenir l'état de %2$s: %3$s\n"
30 "%1$s Fichier %2$s est vide.\n"
31 "%1$s: Chaque point = %2$d enregistrements traités \n"
33 "%1$s Nombre d'enregistrements supprimés %2$ld:  Clé trop longue :\n  '%3$s'.\n"
34 "%1$s Mémoire épuisée !  Configurez arg. -h à un nombre plus petit,\n\
  ou réduisez le nombre d'enregistrements d'entrées."
35 "%1$s: Enregistrement en double numéro %2$ld '%3$s' supprimé.\n"
246 "\n%1$s '%2$s' est un nom de dictionnaire/chemin incorrect.\n"
522 "%s Cette version de %s ne prend pas ne charge les bases de données sémantiques.\n"
$ Msg 579 should not translate ".fzk."
579 "%1$s Enregistrement numéro %2$ld supprimé : Format de fichier.fzk incorrect.\n"
582 "%1$sArgument de point de progression incorrect '%2$s' ignoré\n"
666 "%1$sErreur de programmation : Etat inconnu = %2$d.\n"
717 "%1$s Abandon : texte non compressé n'est pas pris en charge.\n"
844 "%1$sAbandon après %2$ld avertissements.\n\
  Le nombre d'avertissements est indiqué sur la ligne de commande.\n"
926 "%1$s Arg -m ignoré : %2$s: %3$s.\n"
927 "%1$sL'enregistrement numéro %2$ld supprimé : Premier caractère (keytype) de la clé\n\
  '%3$s' n'est pas alphanumérique.\n"
1048 "%1$sLes objets de la base de données sont inchangés parce que le fichier d'entrée est vide.\n"
1086 "%1$s L'enregistrement '%2$s' a un format de date incorrect-utilisation de run date.\n"
1153 "%1$s L'id courante de la table de compression de données dans '%2$s' est %3$ld.\n\
  La base de données '%4$s' a été compressée précédemment avec la table %5$ld.\n"
1215 "%1$s Avertissement. L'enregistrement '%2$s' n'a pas de texte.\n"


$ ****************************************************
$ set 12 formerly set MS_initausd, used in dtsrcreate.c.
$set 12

$ Msg 3 should not translate "dtsearch.dbd."
3 "\n\
SYNTAXE : %1$s [options] <dbname>\n\
       Crée et initialise des fichiers de bases de données DtSearch/AusText.\n\
  -q       pas d'impression des messages d'information.\n\
  -o       permission d'écraser la base de données qui existe déjà.\n\
  -a<N>    Configuration de la taille du résumé maximale à <N> (défaut par type).\n\
  -d<dir>  Emplacement ddu fichier dtsearch.dbd s'il n'est pas dans le répertoire <dbname>.\n\
  -wn<N>   Modification de la taille minimale d'un mot à <N>.  Défaut %2$d.\n\
  -wx<N> Modification de la taille maximale d'un mot à <N>.  Défaut pour langage.\n\
  ---------- Choix du type de la base de données ----------\n\
  -fd      Type DtSearch. Pas de documents, seulement des références de documents\n\
           dans les résumés (par défaut).\n\
  -fa      Type AusText.  Documents stockés dans le référentiel du serveur principal.\n\
  ------------ langues prises en charge ------------\n\
  -l<N>    Configurer le nombre de langues à <N>. Par défaut est 0. Les valeurs prises en charge:\n\
           0 Anglais-ASCII\n\
           1 Anglais-Latin1\n\
           2 Espagnol\n\
           3 Français\n\
           4 Italien\n\
           5 Allemand\n\
           6 Japonais-autoknj\n\
           7 Japonais-knjlist\n\
  <dbname> Préfix du chemin en option, puis le nom de la base de données\n\
           de 1 à 8 caractères. Ne spécifiez pas 'austext' ou 'dtsearch'.\n"

5 "DTSRCREATE Taille minimale du mot %1$d supérieure à sa taille maximale %2$d.\n"
6 "DTSRCREATE Taille minimale du mot changée à %1$d.\n"
8 "DTSRCREATE Taille maximale du mot changée à to %1$d.\n"
9 "DTSRCREATE Spécifier des petites tailles de mots minimales peut entraîner des modifications extensives du fichier d'arrêt\n\
  pour éviter une augmentation importante des \n\
  conditions de stockage de l'index\n"
10 "DTSRCREATE Spécifier des grandes tailles maximales de mot peut entrainer une augmentation importante\n\
  des conditions de stockage.\n"
$ Msg 12 needs option "y" for "yes" to be translated the same as in msg 15.
$ Option "n" does not need a translation because the code assumes anything
$ other than the translated "y" option means "no."
12 "\nFichier '%s' existe déjà.\n\
Permission de l'écraser ainsi que les autres fichiers de la base de données ? [o,n] "
$ Msg 13 should not translate "load_custom_language."
13 "%s Avertissement ! vous avez spécifié une langue personnalisable, non prise en charge.\n\
  Vous devrez fournir vos propres chargeurs de langues au moment de l'exécution\n\
  dans la fonction d'utilisateur 'load_custom_language' pour avoir accès à cette base de données.\07\n"

14 "\n\
Le traitement sémantique%s n'est possible que pour les bases de données de langue anglaise.\n\07"
$ Msg 15 needs "y" to match the "yes" option in msg 12.
15 "o"
24 "DTSRCREATE Initialisation réussie de la base de données '%1$s'.\n"
98 "DTSRCREATE %1$s Id de table de compression Huffman incorrecte."
186 "DTSRCREATE186 Impossible de renommer '%1$s' to '%2$s':\n  %3$s\n"
205 "DTSRCREATE205 Impossible de modifier les permissions de '%1$s': %2$s\n"
213 "%1$s Impossible d'ouvrir '%2$s':\n  %3$s.\07\n"
214 "%1$s Impossible de copier '%2$s' to '%3$s':\n  %4$s\n"
230 "DTSRCREATE230 Impossible d'ouvrir la base de données '%1$s'.\n"
239 "DTSRCREATE239 Impossible d'initialiser la base de données '%1$s'.\n"
244 "DTSRCREATE244 Impossible de supprimer '%1$s':\n  %2$s\n"
326 "DTSRCREATE326 Impossible de modifier les permissions de '%1$s':\n  %2$s\n\
  Tous les fichiers de la base de données doivent avoir des droits en lecture et en écriture.\n\
  Peut être impossible d'ouvrir ou d'initialiser les fichiers de la base de données.\n"
327 "  Cependant, cela n'est utile que si tous les fichiers de la base de données\n\
  ont des autorisations en lecture et en écriture."
433 "%1$s a agrandi la taille abstraite maximale à %2$hd.\n"
509 "DTSRCREATE509 Impossible d'initialiser l'enregistrement d'en-tête de la base de données.\n"

$ ****************************************************
$ set 13 formerly set MS_vista.
$ Msg numbers 1-100 and 900-1000 match
$ user_error[] and system_error[] respectively in dberr.h.
$ Used in dberr.c, ausdopen.c, and opendblk.c.

$ Msgs 1-100 and 900-1000 in set 13 are generated during database
$ input/output.  They are all fatal errors and should be treated as
$ standalone messages.  They are not combined with other messages to
$ produce errors.  The messages come from a third party vendor, so we
$ do not intend to modify the vendor code and make complete sentences
$ out of them.

$set 13

$ For msgs 1-100, see comments at top of set 13.
1 "Base de données non ouverte"
2 "groupe incorrect"
3 "enregistrement incorrect"
4 "base de données incorrecte"
5 "nom de zone incorrect"
6 " db_adresse incorrect"
7 "pas d'enregistrement en cours"
8 "groupe n'a pas de propriétaire courant"
9 "groupe n'a pas de membre courant"
10 "valeur de la clé exigée"
11 "valeur du verrou incorrecte"
12 "l'enregistrement est propriétaire d'un groupe vide"
13 "l'enregistrement est propriétaire d'un groupe"
14 "membre déjà possédé"
15 "zone est une clé composée"
16 "l'enregistrement n'est pas connecté au groupe"
17 "zone n'est pas une clé correcte"
18 "l'enregistrement pas le propriétaire légitime du groupe"
19 "l'enregistrement pas membre légitime du groupe"
20 "erreur dans d_setpages (base de données ouverte ou paramètres incorrects)"
21 "fichier de dictionnaire incompatible"
22 "tentative illégale de supression d'un enregistrement système"
23 "illegal attempt to lock locked set/record"
24 " tentative illégale de blocage d'un goupe/ enregistrement non verrouillé"
25 "id de transaction non fournie"
26 "transaction déjà active"
27 "transaction n'est pas active"
28 "impossible de commencer la transaction à cause de fichiers verrouillés"
29 "impossible de libérer les verrous dans la transaction"
30 "trop de pages modifiées dans la base de données"
31 "impossible de mettre la base de données à jour en hors de la transaction"
32 "accès exclusif demandé"
33 "verrous en écriture non autorisés sur les fichiers statiques"
34 "Id utilisateur non spécifié ou en double"
35 "nom de la base de données, du chemin ou du fichier trop long"
36 "un numéro de fichier incorrect envoyé à d_renfile"
37 "zone n'est pas une zone de clé facultative"
38 "la zone n'est pas définie dans le type d'enregistrement courant"
39 "enregistrement/zone est/a une clé composée"
40 "enregistrement ou numéro de groupe incorrect"
41 "enregistrement/groupe pas heurodaté"
42 " DBUSERIDincorrect (ne contient pas d'alphanumérique)"
43 "NetBIOS n'est pas installé la machine"
44 "nombre incorrect d'éléments dans DBDPATH"
45 " nombre incorrect d'éléments dans DBFPATH"
46 "pas de type d'enregistrement courant"
47 "chaîne de tri de table de pays incorrecte"
48 "base de données n'est pas fermée"
$ Msg 301 should not translate "errno."
301 "\n*** Système I/O errno %1$d = %2$s\n"
302 "\n\
*** La cause habituelle de cette erreur est l'absence d'un fichier en lecture seule\n\
*** dans la base données ou le dépassement de la limite \n\
*** du nombre de fichiers ouverts."
303 "(emplacement non spécifié)"
$ Msg 304 should not translate "db_VISTA."
304 "\n*** db_VISTA auto réparation en cours...\n"
$ Msg 311 should not translate "db_status."
311 "*** Erreur de base de données dans %1$s, db_status = %2$d: %3$n"
312 "Erreur de programmation."
313 "Erreur inconnue."
315 "Base de données n'est pas disponible parce que d'autres utilisateurs l'utilisent."
348 "%1$s Nom de base de données incorrect '%2$s'."
378 "%1$s Impossible d'ouvrir la chaîne de nom de base de données suivante :\n  '%2$s'"
379 "%1$s Impossible d'ouvrir la base de données '%2$s':\n  %3$s."

$ For msgs 900-1000, see comments at top of set 13.
900 "Plus d'espace dans le fichier"
901 "erreur de système"
902 "erreur de page"
$ Msg 903 should not translate "dio."
903 "pas de fichier de travail dans dio"
904 "allocation de mémoire insuffisante"
905 "erreur d'ouverture du fichier"
906 "impossible d'accéder au fichier taf de la base de données"
907 "erreur du fichier taf/log de la base de données"
908 "verrous de la base de données incohérents"
909 "limite d'enregistrement de fichier dépassée"
910 "inncohérence du fichier de clés"
911 "la limite du nombre maximal d'utilisteurs simultanés dépassée"
912 "recherche d'erreur dans le fichier de base de données"
913 "Fichier spécifié incorrect"
914 "erreur de lecture dans un fichier de débordement de la base de données"
915 "erreur de la synchronisation du gestionnaire de verrou"
916 "vérification débogage interrompue"
917 "erreur des communications du réseau"
918 "auto-réparation en cours"
919 " erreur d'écriture dans un fichier de débordement de la base de données "
920 "pas de gestionnaire de verrou installé"
921 "DBUSERID est en cours d'utilisation"
922 "Le gestionnaire de verrou est occupé"


$ ****************************************************
$ set 14 formerly set MS_cborodin, used in dtsrindex.c.
$set 14

1 "%1$s. exécutez %2$s.\n"
2 "%1$s: Des avertissements ont eu lieu."
$ Msg 6 should not be translated.
6 "put_addrs_2_dtbs_addr_file() nitems=%1$d, batchsz=%2$ld\n"
$ Msg 9 should not translate "d99."
9 "\n\
\n%1$s a corrompu le fichier d99 pour le mot '%2$s',\n\
  adresse de la base de données %3$ld @ emplacement du fichier %4$ld => bitvector[%5$ld],\n\
  mais l'allocation bitvecteur maximale = %6$ld.\n"
15 "%1$s moins de 2 octets en entrée.\n"
17 "\n\
SYNTAXE : %1$s -d<dbname> [options] <infile>\n\
       Listed default file name extensions can be overridden.\n\
  -d<dbname>  nom de la base de données 1 - 8 caractères, y compris préfixe du chemin facultatif.\n\
  -t<etxstr>  Fin de la chaîne de séparation du document texte.Par défaut '\\f\\n'.\n\
  -r<N>       Change Pass 1 enregistrement-par-point de %2$d à <N>.\n\
  -b<N>       Change taille max batch de %3$ld à <N>.\n\
  -c<N>       Change cache pagination de la base de données de %4$ld 1K pages à <N> 1K pages.\n\
              <N> >= 16 by powers of 2.  Au départ n'effectuez que des petits changements.\n\
  -i<N>       Change taille tampon d'entrée de défaut  %5$d à <N>.\n\
  -h<N>       Change taille de la table Hash des id d'enregistrements en double de %6$ld à <N>.\n\
              -h0 indique qu'il n'y a pas de doubles, ne les recherchez pas.\n\
  <infile>    Nom du fichier [chemin]d'entrée. Extension par défaut %7$s.\n"
21 "\n\
\n%s '%1$s' enregistrement dépasse la limite du compte des mots.\n\
 Nombre d'enregistrements %2$ld > maxdba %3$ld, dba=%4$ld, sld00=%5$d, offs=%6$d.\n"
22 "\n\
\n%1$s '%2$s' enregistrement dans la base de données (dba=%3$ld)\n\
  dépasse l'allocation bitvecteur (%4$ld >= %5$ld).\n"
33 "\n%1$s Nombre d'enregistrements prévus a dépassé %2$d.\n\
  Une erreur \"espace de pagination épuisé\" en résulte dans Pass 2 \n\
  et la corruption de la base de données. Divisez le fichier pour en réduire le nombre de motsn\
  ou utilisez l'option -b option et relancez l'application.\n"
40 "Enregistrements en double %1$ld ignorés"
41 "Pas d'enregistrements en double"
164 "\n%1$s: %2$s Abandon dû au signel %3$d. base de données %4$s\n\
  peut être corrompue. Restaurez la base de données de sauvegarde.\n"
206 "%1$s: Code de sortie = %2$d, Temps total écoulé %3$ldm %4$lds.\n"
$ Msg 280 should not be translated.
280 "\n%1$s num_addrs_for_word (%2$ld) >= batchsz (%3$ld).\n"
288 "%1$s Abandon. Pas de mots dans le fichier d'entrée %2$s.\n"
374 "\n%1$s Mémoire épuisée ! n\
  Divisez le fichier en plusieurs fichiers plus petits et réessayez.\n"
539 "%1$s Vérificateur d'id d'enregistrements en double est désactivé.\n"
550 "%1$s '%2$s' est un nom de base de données/chemin incorrect.\n"
558 "%1$s Taille du tampon d'entrée incorrecte '%2$s'.\n"
567 "%1$s Argument de la ligne de commande '%2$s' inconnu.\n"
577 "%1$s Arg. '%2$s'incorrect. Utilisation de la valeur par défaut -r%3$d.\n"
580 "%1$s Nom obligatoire de fichier d'entrée absent.\n"
589 "%1$s Pas de nom de base de données spécifié (argument -d).\07\n"
595 "%1$s Argument de taille batch '%2$s incorrect.\n"
600 "%1$sTaille du cache changée à %2$d.\n"
659 "\n%1$s: %2$s supprime l'enregistrement clé trop longue :\n  '%3$s'.\n"
713 "\n%1$s Le mot '%2$s' se trouve dans %3$ld enregistrements,\n\
  mais il n'y a que %4$ld enregistrements dans la base de données!\n\
  (Ceci peut être un bon exemple pour la liste d'arrêt).\n"
727 "\n%1$s Nombre d'enregistrements d'adr, de bd (%2$ld)\n\
dépasse l'allocation bitvector (%3$ld).\n"
$ Msg 776 should not translate "d99."
776 "%1$s Panne d'écriture du fichier d99 : %2$s\n"
$ Msg 848 should not translate "d99."
848 "\n%1$s Impossible de libérer %2$ld octets (%3$ld dba's) dans le fichier d99 \n\
  at offset %4$ld.  Nombre d'adresses lues (code retour) = %5$ld.\n"
849 "\n%1$s: Nombre de mots %2$ld, %3$.0f%% terminé.  Est %4$lum %5$02lus à la fin.\n"
$ Msg 875 should not translate "fseek" or "d99."
875 "\n%1$s Impossible de fseek  le fichier d99  to offset %2$ld.\n"
1068 "%1$s Impossible d'ouvrir le nouveau fichier d'index inversé '%2$s': %3$s\n"
1083 "%1$s Impossible de lire le fichier d'entrée '%2$s': %3$s\n"
1097 "%1$s Abandon dû à des erreurs de chargement de fichiers de langues.\n"
1108 "%1$s: commençant Pass 1, lecture des enregistrements de '%2$s'.\n\
  Chaque point = %3$d enregistrements.\n"
$ Msg 1129 should not translate ".fzk."
1129 "%1$s: %2$s Format du fichier.fzk incorrect.\n"
1168 "%1$s: %2$s supprimé '%3$s', clé n'est pas dans la base de données.\n"
1190 "\n%1$s: Nombre d'enregistrements%2$lu, %3$.0f%% terminé.  Est %4$lum %5$02lus fin du  Pass 1.\n"
1225 "%1$s: Pass 1 complété en %2$lum %3$lus,a lu %4$lu enregistrements.\n\
  %5$s, analysé %6$lu mots.\n"
1233 "%1$s: Commence Pass 2: mise à jour de la base de données et de traversal de l'index batch.\n\
  Chaque point = %2$d mots.\n"
1246 "%1$s: Pass 2 terminé en %2$lum %3$lus, mis à jour %4$lu mots.\n"
1402 "%1$s: Supprimé l'enregistrement en double numéro %2$lu '%3$s'.\n"


$ ****************************************************
$ set 15 formerly set MS_delsfx, now MS_lang, used in lang.c and jpn.c
$set 15

$ Msg 4 should not translate "dblk."  It is a structure name.
4 "%s dblk non initialisé.\n"
$ Msg 5 should not translate "parg."
5 "%s Erreur du programme: parg ne contient ni fichier ni chaîne.\n"

8 "\n\
%1$s '%2$.*s...' est supérieur\n\
à la taille maximale d'un mot de la base de données '%3$s'."

10 "%s Erreur de programme : Etat de l'analyse inconnu.\n"
12 "%1$s '%2$.120s' n'est pas un mot correct de la base de données '%3$s'."

17 "\n\
%1$s '%2$s' est inférieur\n\
à la taille minimale d'un mot de la base de données '%3$s'."

19 "%1$s Le mot '%2$s'n'est pas indexé dans la base de données '%3$s'."
20 "%1$s Erreur de programme : Etat j inconnu %2$d.\n"
21 "%1$s '%2$.*s' n'est pas un mot japonais correct"
23 "%1$s Le mot '%2$s' à la ligne %3$ld est trop court."
24 "%1$s Pas de mots japonais dans le fichier de mots '%2$s'."
25 "%1$s Le chemin de la base de données '%2$s' est trop long : '%3$s'."
31 "%s Le fichier des suffixes de racines n'a pas été chargé.\n"
$ Msg 41 should not translate "Paice."
41 "%1$s %2$s: La règle Paice incorrecte commençant à '%3$s'."
$ Msg 42 should not translate "Paice."
42 "%1$s %2$s: Règles Paice en désordre commençant à '%3$s'."
45 "%s Mémoire épuisée.\n"
50 "Anglais-ASCII"
$ Msg 51 refers to the Latin 1 codeset.
51 "AnglaisLatin1"
52 "Espagnol"
53 "Français"
54 "Italien"
55 "Allemand"
56 "Japonais-comp"
57 "Japonais-.knj"


$ ****************************************************
$ set 16 formerly set MS_vewords, now MS_boolsrch, used in boolsrch.c.
$set 16

$ Msg 2 is like "<program name> Part of ... term <word> which is
$ not in database <database name>."
2 "\n\
%1$s une partie de l'opération de collocation comprend\n\
un mot.'%2$s' qui n'est pas dans la base de données '%3$s'."
$ Msg 6, variable 3 is the database name.
6 "\n\
%1$s Erreur de base de données.  Le mot '%2$s' est\n\
listé dans la base de données '%3$s' mais n'a pas d'enregistrement d'index."
$ Msg , variable 3 is the database name.
11 "%1$s Le mot '%2$s' n'est pas dans la base de données '%3$s'."
$ Msg 14 should not translate "WHITLIM."  The message is like
$ "<Program name> <word in query> has more than <some number> hits."
14 "\n\
%1$s '%2$s' a plus de %3$ld correspondances.\n\
Supprimez-le de la requête ou augmentez la valeur WHITLIM\n\
dans le fichier de configuration du moteur de recherche."
15 "\n\
%s Cette requête demande l'extraction de tout document de la base de données\n\
qui ne contient aucun des mots recherchés.\n\
Ce type de recherche peut avoir une durée\n\
anormalement longue."

$ Msg 28 should not translate "d99."  Variable 2 is the database name.
28 "%1$s Erreur de lecture de la base de données dans %2$s.d99."
$ Msg 32 should not translate "d99."  The message is formed using either
$ msg 33 or 34 as variable 2, and it is like "<program name> Database
$ Error: Word <word> in database <database name> has invalid d99 record
$ number <number>."
32 "\n\
%1$s Erreur de base de données : %2$s '%3$s'\n\
dans la base de données  '%4$s' a un nombre d'enregistrement d99 incorrect %5$ld."
$ Msg 33 is used for variable 2 of message 32.
33 "Mot"
$ Msg 34 is used for variable 2 of message 32.
34 "Racine de"
$ Msg 35 should not translate "stemct," and "pmsz."
35 "%1$s Erreur de programme : stemct=%2$d pmsz=%3$d\n"

$ ****************************************************
$ set 17 formerly set MS_vestatis
$set 17

104 "Impossible de détacher la mémoire partagée."
107 "Débordement de la pile Qsort \n"
110 "Impossible de supprimer la mémoire partagée."
115 "Pas de mémoire partagée disponible"
310 "%1$s n'a plus de mémoire. A besoin de %2$ld octets.\n"
314 "Mémoire épuisée.\n"
421 "Nombre total de correspondances = %1$ld. Correspondances supprimées au delà du nombre maximal spécifié."
$ Msg 437 should not be translated.
437 "%1$s db_status = %2$d, dba = %3$d:%4$ld (x'%5$08.8lx'), vistano = %6$d.\n"
504 "Allocation de cur_ditto impossible.\n"
677 "La zone de requêtre est vide."
990 "Processus de tri en cours. Patientez s'il vous plaît."


$ ****************************************************
$ set 23 formerly set MS_dbrec

$ Set 23 has several words in parenthesis which should not be translated.
$ For example, msg 3: "objkey,"  msg 5: "fzkeysz," msg 12: "abstrsz," 
$ msg 24: "minwordsz," msg 26: "maxwordsz," ...  The single words in
$ parenthesis should not be translated.
$ Msgs like 10, where the entire text is in parentheses should be translated.

$set 23

1 "---------- Valeurs de système pour la base de données '%1$s' ----------\n"
2 "Le numéro de la version schéma (version) est '%1$s'.\n"
$ Msg 3 should not translate "sizeof" or "objkey."
3 "Taille maximale de la clé d'objet (sizeof(objkey)) est %1$ld octets.\n"
$ Msg 4 "Huge" means very, very large.
4 "Clés 'Enormes' facultatives activées.\n"
5 "Nombre maximal de catégories sémantiques (fzkeysz) est %1$d.\n"
10 "  (L'accès sémantique aux objets est désactivé)."
12 "Longueur maximale d'une chaîne abstraite (abstrsz) est %1$d.\n"
14 "  (résumés ne sont pas utilisés dans cette base de données)."
20 "Les résumés sont %1$scompressés.\n"
22 "Analyse du langage est numéro %1$d, %2$s.\n"
24 "Longueur minimale d'un mot (minwordsz) est%1$d.\n"
26 "Longueur maximale d'un mot (maxwordsz) est %1$d.\n"
30 "Nombre de .d00 slots par object (recslots) est %1$d.\n"
36 "  (Le nombre maximal d'objets de la base de données est %1$ld).\n"
40 "Id de la table de compression Huffman (hufid) est %1$ld.\n"
42 "  (Compression est désactivée dans cette base de données)."
44 "  (Une table de compression spéficique n'a encore pas été définie)."
50 "L'accessibilité du moteur au données peut varier selon l'objet."
54 "Impossible d'accéder aux objets de donner à partir du moteur."
56 "Les objets de données sont stockés sous forme de texte compressé."
60 "Seuls les fichiers de serveurs font état d'objets stockés sous forme de texte compressé."
64 " Seuls les fichiers de clients font état d'objets stockés sous forme de texte compressé."
68 "Les clés d'objets sont les références de fichier de serveur aux objets."
72 " Les clés d'objets sont les références de fichier de client aux objets."
74 " Les fichiers de serveurs font état d'objets stockés dans les clés 'énormes'."
80 " Les fichiers de serveurs font état d'objets stockés dans les résumés."
86 " Les fichiers de client font état d'objets stockés dans les résumés."
90 "Erreur : signification de la valeur or_dbaccess (%1$hd) inconnue.\n"
100 "Le texte compressé du référentiel est %1$scompressé.\n"
110 "Le texte compressé du référentiel n'est pas utilisé dans cette base de données."
120 "Les commutateurs de base de données (dbflags) sont 0x%1$lx:\n"
$ Msgs 124 and 125 are used in the variable in messages 130 and 140.
$ For example: "Inverted index INCLUDES words exactly as parsed."
124 "INCLUT"
125 "EXCLUT"
$ Msgs 126 and 127 are used in the variable in messages 160, 162, 164.
$ For example, "Use of optional 'huge' keys is ENABLED."
126 "ACTIVEE"
127 "DESACTIVEE"
$ Msg 130 uses msgs 124 and 125 in the variable.
130 "  Index inversé %1$s mots exactement comme ils ont été analysés.\n"
$ Msg 140 uses msgs 124 and 125 in the variable.
140 " Index inversé %1$s racines de mots.\n"
$ Msg 160 uses msgs 126 and 127 in the variable.
160 " Utilisation des clés \"énormes\" facultatives est %1$s.\n"
$ Msg 162 uses msgs 126 and 127 in the variable.
162 "  La marque de suppression est %1$s.\n"
$ Msg 164 uses msgs 126 and 127 in the variable.
164 "  Les notes de l'utilisateur annexables sont %1$s.\n"
$ Msg 170 is followed by either msg 172 or msg 174.
$ For example, "Text characters are a SINGLE byte wide."
170 "  La largeur des caractères du texte est de %1$s.\n"
172 "DEUX octets"
174 "un SEUL octet"
200 "Le nombre actuel d'objets de la base de données (reccount) est %1$ld.\n"
210 "Le numéro du dernier emplacement utilisé (maxdba) est %1$ld.\n"
310 "\n\
SYNTAXE: %1$s <nombd>\n"
330 "Impossible d'ouvrir la base de données '%1$s'.\n%2$s\n"
340 "Pa sd'enregistrement dans la base de données '%1$s'.\n"
350 "Impossible de lire l'enregistrement dbrec dans la base de données '%1$s'.\n%2$s\n"

$ ****************************************************
$ set MS_dtsrkdump is set 25.  5/20/96.  dtsrkdump.c
$set 25

1 "%s Erreur de programme Abandon.\07\n"
$ Msg 2 should not be translated
2 "%1$c dba=%2$d:%3$-7ld ofs=%4$-9ld adr=%5$-6ld fre=%6$ld\n"
$ Msg 3 is like "<program name> <RCS revision number>, engine
$ <DtSearch version>.  <time stamp>."
3 "%1$s %2$s, engine %3$s.  %4$s.\n"
$ Msg 4, option "-p" can include decimal points -- this is handled by
$ using sscanf, so if sscanf can read numbers with commas in them, then
$ the "-p" option will process that correctly.  atof is used on what
$ sscanf reads in.
4 "\n\
SYNTAXE : %1$s -o|w|ow [-v] [-t<N> | -p<N>] <dbname>\n\
       Lit les fichiers clé DtSearch et imprime le rapport.\n\
  -o        Clés examinées sont des clés d'enregistrement OBJECT.\n\
  -w        Clés examinées sont des MOTS de l'index inversé.n\
  -v        mode VERBOSE, présente chaque clé.\n\
  -t<N>     Seuil. Définit les options w et v et  and présente seulement les mots\n\
            avec >= <N> adresses.  Tous les mots seront listés si <N> = 1.\n\
  -p<N>     Autre seuil. Le même que -t sauf <N> si pourcentage\n\
            de toute la base de données (<N> peut inclure une virgule décimale).\n\
            Par exemple -p99.9 imprime tous les mots qui se trouvent\n\
            dans 99.9%% ou plus des enregistrements-excellente façon de trouver des entrées\n\
            pour la liste d'arrêt.\n\
            Si w et vsont définis sans un seuil, la valeur par défaut est -t%2$d.\n\
  <nombde>  nom de la base de donnée de 1 - 8 caractères avec un préfix de chemin facultatif.\n"
5 "%1$s Valeur de pourcentage incorrecte %2$lf.\07\n"
53 "%1$s Valeur du seuil incorrecte.\07\n"
55 "%1$s Argument de la ligne de commande '%2$c' inconnu.\07\n"
56 "%1$s Nom obligatoire de la base de données absent.\07\n"
57 "%1$s Vous devez spécifier soit -o ou -.\07\n"
58 "%1$s Nom de la base de données incorrect '%2$s'.\07\n"
$ Msg 60 should not be translated.
60 "%1$s: '%2$s' reccount=%3$ld maxdba=%4$ld recslots=%5$hd minw=%6$hd maxw=%7$hd\n"
70 "%1$s Ne fera la liste que des mots trouvés dans %2$ld ou plus des enregistrements.\n"
80 "%1$s: Liste tous les mots de la base de données.\n"
$ Msg 100 should not be translated.
100 "dba x%1$08lx, %2$6ld\n"
$ Msg 110 is like "in <directory name>"
110 " dans %1$s"
120 "Récapitulatif Object pour '%1$s'%2$s:\n"
130 "Nombre d'objets Object Count by Keytypes:"
160 "Nombre TOTAL Object = %1$ld\n"
$ Msg 170 should not translate "DBA."
170 "Plus gros object DBA  = %1$ld\n"
180 "%1$s: * Les mots marqués d'une astérisque se trouvent dans tous les enregistrements.\n"
200 "Récapitulatif des mots pour '%1$s'%2$s:\n"
210 "Nombre TOTAL de mots = %1$ld\n"

$ ****************************************************
$ set MS_dtsrclean is set 26.  5/21/96.  dtsrclean.c
$set 26

1 "\n\
SYNTAXE: %1$s [options] <dbname> <newpath>\n\
       Compresse l'espace d99 inutilisé et valide les connexions d00-d99 .\n\
  -p<N>     Points de progression imprimeés tous les <N> enregistrements (par défaut %lu).\n\
            Message entier de progression imprimé tous les %d points.\n\
  -oy       Permet le recouvrement des fichiers pré-existants d99/d2<N> dans le nouveau chemin.\n\
  -on       Force la sortie si les fichiers présexistants d99/d2<N> se trouvent dans le nouveau chemin.\n\
  -v        Valide les connexions d99 and d00, décorrompt le fichier d99 et assure le compte exact\n\
            des enregistrements. Utilise aussi  -c0 pour décorrompre la base de données entière.\n\
  -v<fname> M~me que -v mais écrit aussi tous les enreg. d00 non cités par d99\n\
            dans fname dans un format compatible à l'extraction en format de fichier .fzk.\n\
  -c<N>     quitte s'il il y a plus de <N> connexions corrompues/incomplètes (par défaut %d).\n\
            Limite de corruption désactivée par -c0.\n\
  <dbname> nom de la base de données de 1 - 8 car = les anciens fichiers d99/d2<N> à mettre à jour.\n\
            Fichiers se trouvent dans le répertoire local.\n\
  <newpath> Indique où les nouveaux fichiers d99/d2<N> seront placés.\n\
            Si le premier caractère n'est pas un tiret, le chemin est lié au répertoire local.\n\
CODES DE SORTIE :\n\
  0: Réussite totale.  1: Avertissement.  2: Tâche jamais commencée.\n\
  3-49: tâche terminée prématurément, fichiers anciens ok, nouveaux fichiers inutilisables.\n\
  50-99: Erreur fatale, même l'ancienne base de données peut être corrompue.\n\
  100+: Ctrl-C, détruire et tous les autres signaux d'interruption provoque une fin prématurée,\n\
     nouveaux fichiers sont peut-être inutilisables. Signal = code de sortie - 100.\n"
2 "%1$s: %2$s Compression %3$d%% (environ %4$lu KB) en %5$ld:%6$02ld min:sec.\n"
$ Msg 3 should not translate "d99."  "WORDS" should be translated.
3 "%1$s: a compté %2$ld MOTS dans %3$s.d99.\n"
4 "%1$s: Pas de connexions corrompues.\n"
5 "%1$s: Code de sortie = %2$d.\n"
$ Msg 6 should not be translated.
6 "  DBA = %1$d:%2$ld (x%3$02x:%4$06lx),  orig addr val = x%5$08lx\n\
  Word='%6$c%7$s' offset=%8$ld addrs=%9$ld free=%10$d\n"
7 "%1$s: Copie des fichiers anciens d2<N> à %2$s...\n"
$ Msg 11 is like "<program name> Version <version number>. Run <date program
$ was run>."
11 "%1$s Version %2$s.  Exécuter %3$s.\n"
$ Msg 14 should not translate "d99."
14 "%1%s: Fichiers anciens : '%2$s.d2x, .d99'.\n"
$ Msg 15 should not translate "d99."
15 "%1$s: Nouveaux fichiers : '%2$s.d2x, .d99'.\n"
24 "%1$s: Un ou plusieurs nouveaux fichiers existent déjà.\n"
$ Msg 25 should not be translated.
25 "%1$s: curr reccnt=%2$ld, mxdba=%3$ld, sl/rec=%4$ld, tot#adr=%5$ld.\n"
26 "%1$s: Lecture du fichier d00. Chaque point correspond environ à %2$ld documents de la base de données...\n"
$ Msg 27 should translate DOCUMENTS.
27 "%1$s: A confirmé %2$ld DOCUMENTS dans %3$s.d00.\n"
$ Msg 28 should translate DOCUMENTS.
28 "%1$s: %2$ld DOCUMENTS actuellement dans %3$s.d00 pas = %4$ld stockés.\n\
  Nombre sera rectifé dans le nouveau fichier d00.\n"
29 "%1$s: Compression into %2$s. Chaque point représente environ %3$lu mots...\n"
$ Msg 45 needs the "y" for "yes" to match msg 46.
45 "    Recouvrir les fichiers dans le nouveau répertoire ? [o/n] "
$ Msg 46 must match the "yes" option from msg 45.
46 "o"
66 "%1$s Abandon apès le signal d'interruption %2$d.\n"
$ Msg 111 should not translate "d99."
111 "*** %1$s DBA dans d99 = -1.  Probable overrun into expansion\n\
  area due to incorrect count values in d2x file.\n"
159 "%1$s Argument : '%2$s 'inconnu.\n"
177 "%1$s Argument %2$.2s incorrect.\n"
193 "%1$s Abandon à %2$ld connexions corrompues.\n"
194 "%1$s %2$s %3$ld connexions corrompues/incomplètes.\n"
210 "%1$s Argument obligatoire nombd absent.\n"
211 "%1$s Argument obligatoire du nouveau chemin absent.\n"
212 "%1$s Trop d'arguments.\n"
$ Msg 222 should not translate "d99" or "DBA."
222 "*** %1$s DBA dans d99 pas dans d00, emplacement > nombre maximal de documents.\n"
229 "%1$s Nom de la base de données '%2$s' incorrect.\n"
301 "%1$s L'argument -m  n'est plus nécessaire.\n"
$ Msg 333 should not translate "d99" or "d00."
333 "*** %1$s DBA dans d99 n'existe pas dans d00.\n"
393 "%1$s Les répertoires anciens et nouveaux sont identiques : '%2$s'.\n"
439 "%1$s Impossible d'ouvrir %2$s: %3$s\n"
440 "%1$s Impossible d'accéder à l'état de %2$s: %3$s\n"
$ Msg 444 should not translate "d99."  Do not translate starting with "DBA = ..."
444 "*** %1$s d00 enregistrement '%2$s' n'est pas référencié dans d99.\n\
  DBA = %3$d:%4$ld (x%5$02x:%6$06lx).\n"
463 "%1$s L'argument de la ligne de commande ne permet pas le recouvrement du fichier.\n"
$ Msg 465 "Validate" should match the description for the "-v" option in msg 1.
465 "%1$s AVERTISSEMENT: Allocation de mémoire au vecteur bit impossible.\n\
  Mode 'Validation' est désactivé.\n"
489 "%1$s Erreur d'écriture dans %2$s: %3$s.\n"
499 "%1$s %2$s est vide.\n"
517 "%1$s Erreur de lecture dans %2$s: %3$s.\n"
$ Msg 561 should not translate "DBA," "maxdba," or "dbrec."
561 "%1$s DBA '%2$d:%3$ld' (nombre d'enregistrements %4$ld) dans d00 excède le nombre total d'adresses %5$ld;\n\
  Débordement du vecteur Bit parce que maxdba %6$ld de dbrec est incorrect.\n"
$ Msg 657 is like "<program name> Read error on <filename>: <system errno>."
657 "%1$s Erreur de lecture dans %2$s: %3$s.\n"
$ Msg 665 is like "<program name> Write error on <filename>: <system errno>."
665 "%1$s Erreur d'écriture dans %2$s: %3$s.\n"
$ Msg 802 is like "<program name> Unable to open <filename> to output
$ unreferenced d00 records: <system errno>."  Do not translate "d00."
802 "%1$s Impossible d'ouvrir '%2$s' pour extraire les enregistrements d00 non référenciés:\n  %3$s\n"

$ ****************************************************
$ set MS_dui is set 27.  5/21/96.  dui.c
$set 27
1 "%1$s DtSearch Version %2$s.\n\
SYNTAXE: %3$s [-options] <requête>...\n\
       Options commençant par le même caractère s'excluent mutuellement.\n\
  -ss         Recherche de type S (racines) au lieu d'une recherche statistique par défaut.\n\
  -sw         Recherche de type W (mots exacts) au lieu d'une recherche statistique par défaut.\n\
  -pa         Imprime des résumés (par défaut).\n\
  -pk         Imprime les clés d'enregistrement.\n\
  -pb Imprime des résumés et aussi les clés d'enregistrement.\n\
  -p<N>       Extrait et imprime le <N>ième enregistrement de la liste hitlist.\n\
  -k<C>       Réduit les recherches aux clés commençant par le caractère <C>.\n\
  -d<nombd> nom de base de données  1 - 8 caractères. Par défaut,\n\
              première base de données du fichier de configuration de l'emplacement.\n\
  -m<N>       Nombre maximal de correspondances renvoyées.Par défaut à %4$d.\n\
  <query>     groupe de mots de recherche sur le reste de la ligne de commande.\n\
              La requête peut être encadrée de guillements pour y inclure des opérateurs bouléens.\n"
4 "Recherche de lla base de données '%1$s'..\n"
9 "<aucun résumé disponible>"
117 "%1$s Argument de la ligne de commande '%2$s' incorrect.\07\n"
125 "%1$s Pas de requête indiquée.\07\n"
165 "%1$s Initialisation de DtSearch a échoué :\n  %2$s\n"
190 "%1$s Pas de correspondance.\n"
195 "%1$s Recherche a échoué.\07\n"
200 "%1$s Le texte n'est pas disponible pour le document '%2$s'.\n"
203 "%1$s Impossible de déterminer les types de clés disponibles :\n  %2$s\n"
224 "%1$s Aucune clé d'enregistrement ne commence par '%2$c' dans la base de données '%3$s'.\n"
247 "%1$s ----- Message du moteur après linitialisation -----\n%2$s\n"
248 "%1$s ----- fin des messages d'initialisation du moteur -----\n"

$ ****************************************************
$ set MS_boolpars is set 28.   5/22/96.
$set 28

1 "\n\
%1$s Cette chaîne de recherche est une requête bouléenne\n\
incorrecte. Exprimez-la à nouveau et ré essayez."

2 "%1$sLa zone de requête est vide."

$ Msg 3 should not translate "@".
3 "\n\
%1$s L'opérateur de collocation (@) peut se trouver\n\
seulement entre deux mots."

$ Msg 4 variable 2 is <database name>.
4 "\n\
%1$s un ou plusieurs mots de la requête ne sont pas \n\
stockés dans la base de données '%2$s'."

$ Msg 5 "AND" should be the local equivalent to the Boolean Operator
$ for "AND."  "&" should not be translated.
5 "\n\
%1$s Les opérateurs booléens doivent être placés entre des mots ou\n\
des expressions. Deux mots de suite sans opérateur\n\
sont interprétés comme séparés par \n\
l'opérateur AND (&)."

6 "%1$s L'expression entre parenthèses est absente."

$ Msg 5 "NOT" should be the local equivalent to the Boolean Operator
$ for "NOT."  "~" should not be translated.
7 "\n\
%1$s L'opérateur NOT (~) doit être placé à gauche du mot\n\
ou de l' expression qu'il qualifie."

8 "%1$s Requête bouléenne incorrecte.Erreur de syntaxe numéro %2$d."

9 "%1$s Trop de termes dans la requête bouléenne."

$ Msg 10 variable 2 is <database name>.
10 "%1$sLes recherches en collocation sont impossibles dans cette base de données '%2$s'."

$ Msg 11 should not translate "@."
11 "\n\
%1$s L'opérateur de collocation '%2$.*s' est incorrect.\n\
Le format correct est '@<N>'où <N> est supérieur à zéro."

$ Msg 12 should not translate "@."
12 "\n\
%1$s l'opérateur de collocation (@) n'est pas autorisé \n\
entre deux mots identiques."

13 "%1$s Le mot '%2$.*s' est incorrect."

14 "%1$s La chaîne '%2$.*s' n'est pas un seul mot."

15 "\n\
%1$s Cette requête ne peut pas renvoyer des enregistrements\n\
logiquement. Reformulez-la et réessayez."

16 "\n\
%1$s Cette requête renverra la base de données entier\n\
'%2$s'. Reformulez-la et réessayez."


$ ****************************************************
$ set 29 symbolically named MS_tomita, used in tomita.c (dtsrdelete)
$set 29

1 "\n\
\n%1$s A reçu une interruption %2$d.\n\
  Le programme s'arrêtera après les suppressions en cours.\n"

3 "%1$s: Code de sortie = %2$d.\n"
4 "%1$s Base de données '%2$s' introuvable.\n"
5 "Les chpix sont les suivants :"
8 "%1$s Format d'entrée incorrect : %2$.30s...\n"
$ Msg 24 is like <Program name> Database <dbname>, <record id> not found."
24 "%1$s Base de données  %2$s, '%3$s' introuvable.\n"

25 "\n\
NE PAS CONTINUER sous aucune des circonstances suivantes :\n\
-> Si le fichier d'entrée qui contient les id d'enregistrements à supprimer \n\
      n'est pas nommé '%1$s'.\n\
-> Si certains utilisateurs ont toujours accès à la base de données affectée.\n\
-> Si des fichiers de la base de données n'ont pas été sauvegardés.\n\n\
Si vous êtes sûr de vouloir commnecer la suppression, tapez 'y' maintenant... "

$ Msg 26 is "<program name> <number of files> read, <number> deleted,
$ <time> elapsed. Database <dbname>: Current record count = <number>,
$ Batch size = <number of records>."
26 "%1$s %2$ld lu, %3$ld supprimé, %4$ldh %5$2ldm %6$2lds passé.\n\
  Base de données '%7$s': Nombre actuel d'enregistrements = %8$ld, Taille Batch = %9$d.\n"
$ Msg 27 is "<program name> <number of records> records read from input file.
$ <number> were deleted and <number> were not found in <time>." 
27 "%1$s %2$ld enregistrements lus dans fichier d'entrée.  %3$ld ont été supprimés et\n\
  %4$ld non trouvés en %5$ld heures, %6$ld minutes, %7$ld secondes,\n"

28 "  soit une moyenne de %1$ld minutes, %2$ld secondes par enregiatrement supprimé.\n"
$ Msg 29 is "<program name>. Run <time stamp>."
29 "%1$s. Exécuter %2$s.\n"

$ Msg 30 should not translate "-y".
30 "\n\
SYNTAXE: %1$s [options]\n\
  -i    Nom de fichier d'entrée. Si non spécifié, par défaut à %2$s.\n\
  -d[v] imprime les états de débogage.\n\
        -dv active le débogage verbose (enregistrement par enregistrement).\n\
  -t<N> Nombre maximum de secondes de run time désiré.\n\
        Ctrl-C/Break arrêtera la suppression à l'enregistrement suivant.\n\
  -n<N> Change le nombre d'enregistrements d'un batch de %5$d à <N>.\n\
  -y    Répond automatiquement 'oui à l'invite de confirmation du mode supprimer.\n\
  -d    Suit les opérations de suppression.\n"

31 "%1$sArgument '%2$s' inconnu.\n"
32 "%1$s Impossible d'ouvrir le fichier d'entrée '%2$s'.\n"
33 "\n%1$s Impossible d'ouvrir le fichier de sortie '%2$s'.\n"
34 "Initialisation du moteur de recherche...\n"
36 "Arrêt normal du moteur.\n"


$ ****************************************************
$ set 30 named MS_huff has messages for hencode.c, hdecode.c, huffcode.c
$set 30

$ Msg 1 variable 2 is <filename>, variable 3 is <system errno>.
1 "%1$s: Impossible d'ouvrir le fichier encodé huffman '%2$s': '%3$s' Code de sortie = 2.\n"
2 "\n\
SYNTAXE: %1$s <filename>\n"
3 "%1$s Impossible d'ouvrir le fichier d'entrée '%2$s.\n"
4 "%1$s Nombre total de caractères = %2$ld.\n"

10 "%1$s Mémoire épuisée.\n"
$ Msg 11 variable 2 is <tree file name>, variable 3 is <system errno>.
11 "%1$s Impossible d'ouvrir le fichier d'arbre '%2$s': %3$s.\n"
$ Msg 12 variable 2 is <tree file name>.  Do not translate "hctree."
12 "%1$s format hctree '%2$s' incorrect.\n"
$ Msg 13 should not translate "hctree_ids."
13 "%1$s hctree_ids. incompatible \n"
14 "%1$s Impossible d'ouvrir le fichier cypher '%2$s'.\n"
$ Msg 15 should not translate "EOF."
15 "%1$s EOF inattendue '%2$s'.\n"

30 "183 Chaînes de bits sont trop grandes. Vous avez probablement des litéraux \n\
turned off par le nombre de caractères disproportionné.\n"
31 "191 Erreur de programmation : Tentative de construction d'un arbre code\n\
Huffman apès la création de root persiste.\n"
32 "(caractère nonascii, high bit set)"
33 "366 Format de fichier incorrect pour %1$s.\n"
34 "424 Ouverture en écriture du fichier '%1$s' a échoué. Vérifiez qu'il n'est pas lecture seule.\n"
35 "\n\
SYNTAXE: huffcode [-l<N> | -l-] [-o] <huffname> [<infile>]\n\
  -l<N>      indique le nombre de seuil 'littéral'. Tout caractère qui apparaît\n\
             <= <N> fois sera codé par le littéral Huffman. La valeur par défaut est -l0,\n\
             code littéral seulement pour octets avec des counts of zero.\n\
  -l-       arrête le code littéral. L'arrêt du codage littéral dans des arbres déséquilibrés\n\
             peut conduire à des codes de chaînes de bit TRES,TRES LONGS-Évitez-le sauf si\n\
             l'entrée est un fichier binaire équilibré.\n\
  -o         pré autorise l'écrasement de tout fichier de décodage courant.\n\
  <huffname> est le préfix du nom de fichier des fichiers de Code Huffman.\n\
             Si le fichier de codage existe déjà (%1$s), le nombre d'octets du fichier d'entrée s'y ajoutera\n\
             sinon il en sera créé un nouveau.\n\
             Le fichier de décodage (%2$s) est toujours créé à chauqe exécution.\n\
  <infile>   est un fichier d'entrée conteant des octets à compter.\n\
             Vous pouvez l'omettre s'il existe déjà un fichier de codage.\n"
36 "'%1$s' est un argument incorrect.\n"
37 "576 Préfixe des noms de fichiers de code Huffman absent.\n"
$ Msg 38 needs to have "y" translated to match msg 39"
38 "Fichier de décodage '%s' existe déjà. L'écraser ? [o/n]\n"
$ Msg 39 needs "y" to match the "yes" option in msg 38.
39 "o"
40 "HUFFCODE Version %1$s\n"
41 "Les tables de Code Huffman seront créées.\n"
42 "La table '%1$s' contient déjà %2$ld Ko des exécutions précédentes.\n"
43 "645 Fichier d'entrée n'est pas spécifié et le fichier de la table '%s' \n\
n'existe pas - ne rien faire.\n"
44 "Impossible d'ouvrir le fichier d'entrée '%1$s'ou d'accéder à l'état : %2$s\n"
45 "Le fichier d'entrée '%1$s' contient environ %2$ld Ko.\n"
46 "\n%1$ld%% accompli. %2$2ld Ko lus. Estimation %3$3ld secondes pour terminer.\n"
$ Msg 47 variable 2 is a time stamp, variable 2 is from either msg 48 or 49,
$ variables3 and 4 are filenames.
47 "L'horodateur d'identifacation sera '%1$ld'.\n\
%2$s tables de Code Huffman dans '%3$s' and '%4$s'...\n"
$ Msg 48 is used in msg 47 as variable 2.
48 "Création"
$ Msg 49 is used in msg 47 as variable 2.
49 "Reconstruction"

$ ********************** DTSEARCH.MSG ************************
