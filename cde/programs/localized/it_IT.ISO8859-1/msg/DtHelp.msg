$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthelp.msg
$ **
$ **  Project:     CDE 1.0 Help
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for DtHelp 
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **         All Rights reserved
$ **
$ **
$ **     There are two types of messages in this file:
$ **
$ **     1) Messages that appear in dialogs or are displayed to the user.
$ **        These messages are the default and they should ALL BE LOCALIZED.
$ **        Note that these messages do NOT have any identification.
$ **
$ **     2) Messages that should not be localized.
$ **        These messages are marked with DO NOT TRANSLATE THESE MESSAGES.
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *   Major Mods, 3/26/92 in preperation for the NLS package release on the 
$ *               27th.  (Mike Wilson).
$ *
$ *   Major Mods, 6/10/94 in prep for Cde Message Catalog Freeze
$ *
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$set 2
$ *****  Module: HelpDialog.c  and a few from HelpQuickD.c *****
$ 
$ These Messages appear in the pulldown menus for Cde Help General Help
$ dialog widget.
$ 
$ They come in pairs where the first is the menu name (e.g. 1 Print...) and the
$ second is the mnemonic (e.g. 2 P).  
$ 
$ Messages 1 through 6 are the pulldown menus for the File menu.
1 Stampare...
2 S
3 Chiudere
4 C
5 Nuova finestra...
6 f
$ Messages 7 through 8 are the pulldown menus for the Edit menu.
7 Copiare
8 C 
$ Messages 9 through 10 are the pulldown menus for the Search menu.
9 Indice analitico...
10 I
$ Messages 11 through 16 are the pulldown menus for the Navigate menu.
11 Argomento iniziale
12 A
13 Ripercorrere indietro
14 R
15 Registrazione cronologica...
16 c
$ Messages 17 through 28 are the pulldown menus for the Help menu.
17 Panoramica - Uso dell'aiuto
18 P
19 Indice
20 I
21 Attività
22 A
23 Riferimenti
24 R
25 Mouse e tastiera
26 M
27 Informazioni sull'aiuto
28 f
$ 
$ Messages 29 through 36 are the menu labels and mnemonics for for the 
$ Help Dialog.
29 File
30 F
31 Ricercare
32 R
33 Sfogliare
34 S
35 Aiuto
36 A
37 Editare
38 E
$ 
$ 
$ >>>>> Skip #39 through #49
$ 
$ Messages 50 through 55 are used to display formatting errors within the
$ help dialog and quick help dialog widgets.
$ 
50 Impossibile formattare i dati della stringa.
51 Impossibile formattare i dati della stringa dinamica.
52 Impossibile formattare la pagina di spiegazioni. La pagina richiesta \
non è presente o è danneggiata.
53 Impossibile formattare i dati del file di testo. Il file richiesto \
non è presente o è danneggiato.
54 Impossibile formattare l'argomento dell'aiuto.
55 ID di posizione inesistente:
$ 
$ Messages 56 and 67 are displayed when a hypertext link type is selected but
$ not supported within the current application.
$ 
56 Il collegamento ipertestuale richiesto non è supportato in questa applicazione.
57 I collegamenti alle pagine di spiegazioni non sono supportati da questa applicazione.
$ 
$ Messages 58 through 62  are used when bad or null help volumes or 
$ locationId's are used.
$ 
58 L'aiuto online richiesto non è installato o non si trova nel percorso di ricerca corretto. Per informazioni sull'installazione dell'aiuto online, consultare la documentazione del prodotto.\n
59 Non è stato specificato l'ID di posizione.
60 Non è stato specificato il volume di aiuto.
61 Volume di aiuto:
62 ID di posizione:
$ 
$ >>>>> Skip #63 through #69
$ 
$ 
$ Message 70 is used when a null value for DtNtopicTitle is used.
$ 
70 L'argomento specificato non esiste.
$ 
$ New messages added after early msg cat freeze; not in order
80 Il tipo di aiuto specificato non è valido.



$set 3
$ ****  Module: History.c ****
$ These messages are used as labels for the History Dialog.
1 Aiuto - Registrazione cronologica
2 Volumi consultati:
3 Chiudere
4 Aiuto
5 File di aiuto
6 Pagine di spiegazioni
7 Messaggi di aiuto
8 Argomenti consultati:
9 File consultati:
10 Pagine di spiegazioni consultate:
11 Messaggi consultati:


$set 4
$ *****  Module: Print.c  *****
$ These messages are used as labels for the Print Dialog.
$ 1: dialog title
1 Aiuto - Stampa
$ 10-12: standard printer dialog labels
10 Stampante:
11 Copie:
12 Titolo pagina di testa:
$ 20-25: paper size label and sizes
20 Formato del foglio:
21 Lettera 8,5 x 11"
22 Legale 8,5 x 14"
23 Esecutivo 7,25 x 10,5"
24 A4 210 x 297 mm
25 B5 182 x 257 mm
$ 30-33: help volume-specific printing options
30 Argomenti da stampare
31 Argomento corrente
32 Argomento corrente e sottoinsiemi
33 Indice e indice analitico
34 Tutti gli argomenti del volume

$ 40-42: buttons at bottom of dialog
40 Stampare
41 Annullare
42 Aiuto

$ These messages are used to describe the kind of content to be printed
$ 50 "Help Volume: "
50 Volume di aiuto: 
51 Messaggio di aiuto
52 File di aiuto

$ 53 "Manual Page: "
53 Pagina di spiegazioni: 
54 Messaggio di errore

$ $ Don't translate message 60 unless it is not an empty string
$ This is the default value that goes in the Printer field
$ 60 " "
$quote "
60 ""
$quote

$ $ Don't translate message 61
$ This is the default value that goes in the Copies field
61 1


$set 5 
$ *****  Module: GlobSearch.c  *****
$ Message 1 is the label used to label the dialog
1 Aiuto - Ricerca in indice analitico
2 Ricercare
3 Volume corrente
$ DO NOT TRANSLATE MESSAGE 4; it is no longer used
$ " (%s)"
4  (%s)
5 Tutti i volumi
6 Volumi selezionati
7 Selezionare volumi...
8 Mostrare
9 Indice completo
10 Voci contenenti:
11 Andare a
12 Avviare la ricerca
13 Continuare la ricerca
14 Arrestare la ricerca
15 Chiudere
16 Aiuto
17 Ricerca dei volumi in corso... %d trovati
18 Ricerca in corso... Volumi restanti: %d
19 Nessuna voce di indice trovata
20 Indice completo
21 Voci contenenti "%s"
22 Nessun volume selezionato
23 Selezionare le opzioni per la ricerca
$ Do NOT translate messages 25-31
$quote "
$ " "
25 " "
26 "+"
27 "-"
$ "  +"
28 "  +"
$ "  -"
29 "  -"
$ 30 "       "
30 "       "
$ 31 "         "
31 "         "
$quote
$ DO translate messages 32 and above
$ kind of topic messages
32 Messaggio di aiuto
33 File di aiuto
34 Pagina di spiegazioni
35 Formato sconosciuto
$ specialized title of Search Volume Selection dialog
40 Aiuto - Selezione dei volumi per la ricerca
$ additional messages (late addition caused this ordering)
50 Nessuna voce di indice trovata. Provare con Tutti i volumi.
51 Nessuna voce di indice trovata. Provare con un'altra parola.
$ additional messages (later addition caused this ordering)
$ These messages are the braces that go around the current volume name
$ in the index search dialog.  They are kept in the message catalog
$ in case they need to be localized.
60 (
61 )


$set 6
$ *****  Module: HelpAccess.c  *****
$ These messages are used for the Help on Help dialog in Cde Help widgets.
1 Aiuto sull'aiuto 
2 Stampare...
3 Chiudere
4 L'Aiuto sull'aiuto non è disponibile per questa applicazione!
5 Ripercorrere indietro
6 Aiuto...


$set 7
$ *****  Module: Main.c & Util.c & (dthelpview CDE 1.0  *****
$ These messages are used by the dthelpview program.  They need to be 
$ localized for dthelpview, however, they are not directly used by libDtHelp
$ Messages 1 through 7 are command line usage response messages.
$ Do NOT translate the option name (e.g. -helpVolume).  DO translate
$ the argument descriptor name (e.g. <Help Volume File>)
1 Sintassi: %s...\n
2 \t-helpVolume <File del volume di aiuto>\n
3 \t-locationId  <ID>\n
5 \t-file  <File di testo ASCII>\n
6 \t-man\n
7 \t-manPage <Pagina di spiegazioni Unix>\n\n\n
$ man page dialog
11 Pagina di spiegazioni
12 Pagina di spiegazioni:
13 Mostrare pagina di spiegazioni
$ button labels
16 Uscita
17 Chiudere
18 Ripercorrere indietro
19 Stampare...
$ title of helpview window
20 Visualizzazione dell'aiuto


$set 8
$ *****  Module: HelpUtil.c  *****
$ Message 1 is the label used in the definition link dialog box 
$ (e.g. Quick help close button)
2 Chiudere
3 Errore interno: Impossibile comunicare con il mouse\nDtHelpReturnSelectedWidget interrotto.\n
4 Errore interno: Impossibile comunicare con la tastiera\nDtHelpReturnSelectedWidget() interrotto.\n
5 Errore dell'aiuto 
$ Messages 10 and up were added after message catalogs were taken
$ for translation.  They are not in any order or chosen location.
10 Eseguire comunque
11 Non eseguire
12 Il volume di help ha cercato di eseguire un alias di comando.\nL'alias '%s' non è definito.
13 Il volume di help ha cercato di eseguire un comando.\nPer ragioni di sicurezza, l'esecuzione automatica del comando è stata disabilitata.\nIl comando è:  %s
14 Il volume di help ha cercato di eseguire un comando come utente root, ma la\ndirectory iniziale  "%s"  ($HOME) non corrisponde alla\ndirectory iniziale dell'utente root.  Questa situazione potrebbe causare l'esecuzione di\ncomandi erronei.\n\nIl comando è:  "%s"\n\nNota bene:  per evitare questo problema:\n utilizzare il comando "su - root"  invece di  "su root".\n
$ End Message set 8


$set 9
$ *****  Module: PathArea.c  *****
$ These messages are used in the general Help Dialog Widget.
$ Message 1 is the label used to label Topic Tree Area  
1 Volume: 
$ Messages 2 though 5 are used in the Button Box area of our general 
$ help dialog.
2 Ripercorrere indietro
3 Registrazione cronologica...
4 Indice analitico...
5 Livello più elevato


$set 10 
$ *****  Module: VolSelect.c  *****
$ Message 1 is the default label used to label the volume selection dialog.
1 Aiuto - Selezione dei volumi
2 Volumi di aiuto
3 Selezionare uno o più volumi
4 Chiudere
5 Aiuto


$set 11
$ *****  Module: HelpQuickD.c  *****
$ These messages are used as labels for the quick help dialog.
1 Chiudere
2 Altre informazioni...
3 Ripercorrere indietro
4 Stampare...
5 Aiuto...

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)Message catalog source $XConsortium: DtHelp.msg /main/5 1996/10/19 16:10:24 rws $"

