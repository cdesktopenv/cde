$ $XConsortium: dthelpprint.msg /main/4 1996/08/11 17:10:21 pascale $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthelpprint.msg
$ **
$ **  Project:     Cde1 Help
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dthelpprint
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1993, 1994 Hewlett-Packard Company
$ ** (c) Copyright 1993, 1994 International Business Machines Corp.
$ ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ ** (c) Copyright 1993, 1994 Unix System Labs, Inc.,a subsidiary of Novell,Inc.
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

$quote " 
$ Use the double quote char around all messages

$set 2
$ *****  Module: PrintUtil.c *****
$ 

$set 3
$ *****  Module: PrintTopics.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
$quote "
1 "%s Errore: helpType è un argomento, ma non è stato specificato un volume di aiuto.\n"
2 "%s Errore: impossibile trovare il volume di aiuto %s\n"
3 "%s Errore: problema nell'elaborazione del volume di aiuto %s\n"
4 "%s Errore: allocazione della memoria non riuscita\n"
5 "%s Errore: impossibile ottenere informazioni sull'argomento:\nvolume %s, locationId %s\n"
6 "%s Errore: impossibile aprire il file temporaneo %s\n"
$ String used to replace $SECTNUM when printing the index section
10 "Indice analitico"
$ String used to replace $SECTNUM when printing the table of contents section
11 "Indice"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ index entry, page number
20 "%s, %d\n"
$ index subentry, page number
21 "     %s, %d\n"
$ location ID associated with the index in the TOC maintained by dthelpprint
30 "__GENERATED-INDEX"
$ string used by can't get a topic title associated with an index subentry
31 ""

$set 4
$ *****  Module: PrintManStrFile.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s Errore: helpType è una stringa, ma non è stata specificata l'opzione stringData.\n"
2 "%s Errore: helpType è una stringa dinamica, ma non è stata specificata l'opzione stringData.\n"
3 "%s Errore: helpType è una pagina di spiegazioni, ma non è stata specificata l'opzione manPage.\n"
4 "%s Errore: helpType è un file, ma non è stata specificata l'opzione helpFile.\n"
5 "%s Errore: allocazione della memoria non riuscita\n"

$set 5
$ *****  Module: Initialize.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
$ Messages 1 to 29: dthelpprint usage message
$ **DO NOT LOCALIZE** the command line option names (e.g. -copies).
$ **DO LOCALIZE** the option argument (e.g. number) and description.
1 "dthelpprint - Programma di stampa per l'aiuto\n\n"
2 "Sintassi: dthelpprint [opzioni]\n"
3 "Opzioni per il controllo delle modalità di stampa:\n"
4 "\t-printer nome_stampante  stampante da usare\n"
5 "\t-copies numero           numero di copie da stampare\n"
6 "\t-outputFile nome_file    file in cui indirizzare l'output\n"
7 "\t-paperSize formato       formato della carta\n"
$ DO NOT LOCALIZE THE PAPER SIZE NAMES help_papersize_xxx
8 "\t\tformato = { help_papersize_letter|help_papersize_legal|\n"
9 "\t\t         help_papersize_executive|help_papersize_a4|help_papersize_b5}\n"
10 "\t-display nome_schermo   schermo da cui vengono richieste le risorse\n"
11 "\t-name                   programma usato nel richiedere le risorse\n"
12 "\t-class                  classe usata nel richiedere le risorse\n"
13 "\t-xrm stringa_risorse    altre risorse\n"
14 "Opzioni per il controllo del contenuto della stampa:\n"
15 "\t-helpType tipo          tipo di dati dell'aiuto\n"
16 "\t\ttipo = 0 (volume), 1 (stringa), 2 (pagina di spiegazioni), 3 (file)\n"
17 "\t-helpVolume volume      percorso completo del file del volume\n"
18 "\t-locationId posizione   nome dell'argomento del volume\n"
19 "\t-all                    stampa tutti gli argomenti e gli indici del volume\n"
20 "\t-sub                    stampa l'argomento locationId e tutti i suoi sottoinsiemi\n"
21 "\t-one                    stampa l'argomento locationId\n"
22 "\t-toc                    stampa l'indice del volume\n"
23 "\t-index                  stampa l'indice analitico del volume\n"
24 "\t-frontMatter            stampa l'intestazione del volume\n"
25 "\t-manPage nomepagina     nome della pagina di spiegazioni\n"
26 "\t-stringData stringa     testo dell'aiuto da stampare\n"
27 "\t-helpFile nomefile      file contenente il testo dell'aiuto\n"
28 "\t-jobTitle titolo        stringa di titolo per il lavoro di stampa\n"
29 "\t-topicTitle titolo      stringa di titolo per il testo dell'aiuto\n"
$ Warning and error messages
$ Do NOT localize the default size name help_papersize_letter
40 "%s Avvertenza: Il formato '%s' non è valido. Verrà usato il formato help_papersize_letter.\n"
41 "%s Avvertenza: Formato, altezza o larghezza non specificate. Verrà usato il formato help_papersize_letter.\n"
42 "%s Avvertenza: Impossibile accedere allo schermo %s\n"
45 "Errore: Impossibile allocare memoria per il file temporaneo\n"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ arg order: directory prefix processid filecnt suffix
50 "%1$s/%2$s%3$d_%4$d%5$s"


$set 6
$ *****  Module: Main.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s Errore: helpType %d non ammesso.\n"



