$ $XConsortium: dthelpprint.msg /main/3 1995/11/08 12:27:26 rswiston $
$ *************************************<+>*************************************  
$ *****************************************************************************  
$ **  
$ **  File:        dthelpprint.msg  
$ **  
$ **  Project:     CDE 1.0 Help  
$ **  
$ **  Description:  
$ **  -----------  
$ **  This file is the source for the message catalog for dthelpprint  
$ **  
$ **  
$ *****************************************************************************  
$ **  
$ **     (c) Copyright 1993, 1994 Hewlett-Packard Company 
$ **     (c) Copyright 1993, 1994 International Business Machines Corp. 
$ **     (c) Copyright 1993, 1994 Sun Microsystems, Inc. 
$ **     (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell,$ Inc.  
$ **         All Rights reserved  
$ **  
$ **  
$ **     There are two types of messages in this file:  
$ **  
$ **     1) Messages that appear in printed output  
$ **        These messages are the default and they should all be localized.  
$ **        These messages are marked with LOCALIZE THESE MESSAGES.  
$ **  
$ **     2) Messages that should not be localized.  
$ **        These messages are marked with DO NOT LOCALIZE THESE MESSAGES.  
$ **  
$ *****************************************************************************  
$ **  
$ **  
$ **  ------------------------- MODIFICATION RECORD --------------------------  
$ *   Major Mods, 7/11/94 for dthelpprint  
$ *  
$ **  ----------------------- MODIFICATION RECORD END ------------------------  
$ *****************************************************************************  
$ *************************************<+>*************************************  
$ Use the double quote char around all messages  
$quote "


$set 2        Redefine set#

$ *****  Module: PrintUtil.c *****  
$   
$set 3
$ *****  Module: PrintTopics.c *****  
$   
$ ** LOCALIZE THESE MESSAGES **  

1 "%s Erreur: Le type d'aide est par sujet, mais aucun volume d'aide n'est indiqué.\n"

2 "%s Erreur: impossible de localiser le volume d'aide %s\n"

3 "%s Erreur: erreur lors du traitement du volume d'aide %s\n"

4 "%s Erreur: l'allocation de la mémoire n'a pas abouti\n"

5 "%s Erreur: impossible d'obtenir des informations sur le sujet:\n\
volume %s, ID_emplacement %s\n"

6 "%s Erreur: impossible d'ouvrir le fichier temporaire %s\n"
$ String used to replace $SECTNUM when printing the index section  

10 "Index"
$ String used to replace $SECTNUM when printing the table of contents section  

11 "Table des matières"
$ ** DO NOT LOCALIZE THESE MESSAGES **  
$ index entry, page number  

20 "%s, %d\n"
$ index subentry, page number  

21 "     %s, %d\n"
$ location ID associated with the index in the TOC maintained by dthelpprint  

30 "__INDEX-GENERE"
$ string used by can't get a topic title associated with an index subentry  

31 ""

$set 4        Redefine set#

$ *****  Module: PrintManStrFile.c *****  
$   
$ ** LOCALIZE THESE MESSAGES **  

1 "%s Erreur: le type d'aide porte sur la chaine, mais aucune donnée de chaîne n'est indiquée.\n"

2 "%s Erreur: le type d'aide est chaîne dynamique, mais aucune donnée de chaîne n'est indiquée.\n"

3 "%s Erreur: le type d'aide est page man, mais aucune page man n'est indiquée.\n"

4 "%s Erreur: le type d'aide est fichier, mais aucun fichier d'aide n'est indiqué.\n"

5 "%s Erreur: l'allocation de mémoire n'a pas abouti\n"

$set 5        Redefine set#

$ *****  Module: Initialize.c *****  
$   
$ ** LOCALIZE THESE MESSAGES **  
$ Messages 1 to 29: dthelpprint usage message  
$ **DO NOT LOCALIZE** the command line option names (e.g. -copies).  
$ **DO LOCALIZE** the option argument (e.g. number) and description.  

1 "dthelpprint - Programme d'impression pour aide\n\n"

2 "Syntaxe: dthelpprint [options]\n"

3 "Options de contrôle d'impression:\n"

4 "\t-printer nom_imprimante  imprimante à utiliser\n"

5 "\t-copies nombre        nombre de copies à imprimer\n"

6 "\t-outputFile nom_fichier  écrire la sortie dans ce fichier\n"

7 "\t-paperSize taille     adapter l'impression au format de papier indiqué\n"
$ Do NOT localize the paper size names help_papersize_xxx  

8 "\t\tsize = { help_papersize_letter|help_papersize_legal|\n"

9 "\t\t         help_papersize_executive|help_papersize_a4|help_papersize_b5}\n"

10 "\t-display nom_écran  écran à partir duquel obtenir les ressources\n"

11 "\t-name                 nom de programme à utiliser pour obtenir les ressources\n"

12 "\t-class                nom de classe à utiliser pour l'obtention des ressources\n"

13 "\t-xrm chaîne_ressource   ressources supplémentaires\n"

14 "Options de contrôle d'impression:\n"

15 "\t-helpType type        type de données d'aide\n"

16 "\t\ttype = 0 (volume d'aide), 1 (chaîne), 2 (page man), 3 (fichier d'aide)\n"

17 "\t-helpVolume volume    chemin d'accès complet du fichier de volume d'aide\n"

18 "\t-locationId emplacement  nom du sujet d'aide dans le volume\n"

19 "\t-all                  imprime les sujets, la table des matières et l'index du volume\n"

20 "\t-sub                  impression des ID_emplacement et des sujets annexes\n"

21 "\t-one                  impression de l'ID_emplacement\n"

22 "\t-toc                  impression du volume d'aide de table des matières\n"

23 "\t-index                Impression du volume d'aide index\n"

24 "\t-frontMatter          impression du volume d'aide en-tête\n"

25 "\t-manPage nom_pageman  nom de la page man\n"

26 "\t-stringData chaîne    Texte d'aide à imprimer\n"

27 "\t-helpFile nom_fichier    fichier contenant le texte d'aide\n"

28 "\t-jobTitle titre       chaîne de titre pour le travail d'impression\n"

29 "\t-topicTitle titre     chaîne de titre pour le texte d'aide\n"
$ Warning and error messages  
$ Do NOT localize the default size name help_papersize_letter  

40 "%s Avertissement: format de papier incorrect '%s'.  help_papersize_letter utilisé.\n"

41 "%s Avertissement: Format de papier ou taille/largeur manquante.  help_papersize_letter utilisé.\n"

42 "%s Avertissement: Impossible d'ouvrir l'écran %s\n"

45 "Erreur: impossible d'allouer la mémoire pour le fichier temporaire\n"
$ ** DO NOT LOCALIZE THESE MESSAGES **  
$ arg order: directory prefix processid filecnt suffix  

50 "%1$s/%2$s%3$d_%4$d%5$s"

$set 6        Redefine set#

$ *****  Module: Main.c *****  
$   
$ ** LOCALIZE THESE MESSAGES **  

1 "%s Erreur: Type d'aide incorrect %d.\n"
