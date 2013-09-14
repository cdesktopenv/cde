$ $TOG: dtterm.msg /main/6 1999/09/17 13:29:01 mgreess $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtterm.msg
$ **
$ **  Project:     Common Desktop Environment dtterm
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtterm
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ *************************************<+>*************************************

$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

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
$ 2. Messages that only appear in the DT error log file ($HOME?.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$     MESSAGES xx-yy WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$     DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ********** PROLOGUE ****************
$  Date    Name               Note
$ ======== =================  ===============================================
$ 03/15/94 B. May             Initial rev
$ 
$ *****************************************************************************

$set 1
$ ****** Module: DtTermMain.c 
$ Message 1 is the window menu addition - do not translate f.separator or
$ f.send_msg.  It will be used as the format string for a sprintf() command,
$ so it is necessary to double up on any percent signs ('%') that you
$ want to keep in the string.
$ Message 2 is an error message

1 \
	Sep             f.separator\n\
	Abilitare/Disabilitare\\ barra\\ \\dei \\menu  f.send_msg %d\
	
2 %s: L'opzione -S non può essere usata insieme alle opzioni -server o -serverid\n

3 Terminale

$set 2
$ ****** Module: DtTermSyntax.c 
$ These are the usage messages. 
$ DO NOT TRANSLATE Messages 37 or 41 ("End-of-list")

1 -/+132				abilita/disabilita il modo 80<->132 colonne
2 -/+aw				abilita/disabilita ritorno a capo automatico
3 -bg colore			imposta il colore di sfondo
4 -/+bs				abilita/disabilita lo sfondo del Terminale nel colore selezionato
5 -display nomedisplay		contatta il server delle finestre X specificato
6 -e comando arg		esegue il comando specificato
7 -fb font			usa il font grassetto
8 -fg colore			usa il colore di primo piano specificato
9 -fn font			usa il font nella forma normale
10 -geometry geom			specifica la dimensione (in caratteri) e la posizione della finestra
11 -help				stampa questo messaggio
12 -/+iconic			avvia/non avvia il terminale in forma di icona
13 -/+j				abilita/disabilita lo scorrimento di più righe
14 -/+kshMode			abilita/disabilita il modo ksh
15 -/+l				abilita/disabilita la creazione di file di log
16 -lf nomefile			specifica il nome per il file di log
17 -/+ls				abilita/disabilita la shell di login
18 -/+map				abilita/disabilita la mappa della finestra nell'output pty
19 -/+mb				abilita/disabilita l'avvertimento per il margine
20 -ms colore			specifica il colore del puntatore
21 -n stringa			specifica il nome dell'icona
22 -name stringa		specifica le stringhe per client, icona e titolo
23 -nb distanza			distanza dal margine destro per l'azionamento del segnale acustico
24 -/+rw				abilita/disabilita il ritorno alla riga precedente
25 -/+sb				abilita/disabilita la barra di scorrimento
26 -/+sf				abilita/disabilita i tasti funzione SUN
27 -sl numero[s]			specifica il numero di righe [schermi] da salvare per lo scorrimento
28 -ti nome			specifica la stringa usata per l'identificazione programmatica
29 -title stringa			specifica la stringa per il titolo della finestra
30 -tm stringa			specifica parole chiave e caratteri per il modo terminale
31 -tn nome			specifica il nome della variabile di ambiente TERM
32 -/+vb				abilita/disabilita l'avvertimento visivo
33 -xrm stringarisorse		specifica le risorse aggiuntive
34 -C				abilita la modalità console
35 -Sxxd				abilita la modalità slave su "ttyxx" descrittore del file "d"
36 -Sxxx.d			abilita la modalità slave su "ttyxxx" descrittore del file "d"
37 End-Of-List
38 L'opzione -e, se specificata, deve apparire alla fine della riga comandi,
39 diversamente verrà avviata la shell predefinita dell'utente. Le opzioni
40 precedute da un segno più (+) ripristinano l'impostazione predefinita.
41 End-Of-List
47 %s: errore nell'opzione "%s"\r\n\n
48 Sintassi:  %s
49 \r\n   
50  [%s]
51 \r\n\nPer una descrizione completa delle opzioni, digitare "%s -help".\r\n\n
52 Sintassi:\n
53 \t%s [-opzioni ...] [-e comando arg]\n\n
54 dove le opzioni disponibili sono:\n

$set 3
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 MEMORIA PIENA\nPremere OK per liberare spazio

$set 4
$ ****** Module: TermPrim.c
$ This is the title of a dialog box
$ 1 OBS- Dtterm - Avvertenza

$ This is the title of a dialog box
2 Terminale - Avvertenza

$set 5
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 MEMORIA PIENA\nPremere OK per liberare spazio

$set 6
$ ****** Module: TermView.c
$ DO NOT TRANSLATE Message 1
$ Messages 2 and 3 are titles for dialog boxes.
1 80x24 132x24
$ 2 OBS- Dtterm - Pagina di spiegazioni
$ 3 OBS- Dtterm - Aiuto

$ Messages 4 and 5 are titles for dialog boxes.
4 Terminale - Pagina di spiegazioni

5 Terminale - Aiuto

$set 7
$ ****** Module: TermViewGlobalDialog.c
$ These are the user-visible labels in the Global Options dialog box.
$ #1 is the title. 
$ The rest are resource types and values - look at the dialog box and 
$ documentation for full specs. 
$ 
$ DO NOT TRANSLATE MESSAGE 15
$ 
1 Dtterm - Opzioni globali
2 OK
3 Annullare
4 Aiuto
5 globali
6 Controllo del cursore
7 Rettangolo
8 Trattino
9 Forma del cursore
10 Abilitato
11 Disabilitato
12 Lampeggio del cursore
13 Velocità di lampeggio (millisecondi)
14 Invisibile
15 blinkRate
16 Controllo dei colori
17 Colori invertiti
18 Colori normali
19 Sfondo/caratteri
20 Controllo dello scorrimento
21 Disabilitato
22 Abilitato
23 Scorrimento continuo
24 Controllo dei segnalatori
25 Acustico
26 Visivo
27 Tipo di segnale 
28 Abilitata
29 Disabilitata
30 Segnalazione del margine
31 Distanza dal margine 
32 Applicare
33 Ripristinare

34 Terminale - Opzioni globali

$set 8
$ ****** Module: TermViewMenu.c
$ These are the menu bar entries and menu pane choices.
$ NOTE TO TRANSLATORS: The Single letter entries are the menu mnemonic
$ for the entry above them (i.e. message 11 "F" is the mnemonic for
$ message 10 "Font Size"). 
$ The items with a "+" in them are the accellerators for an item.
$ The single-character items are the mnemonics for a menu item, where the
$ text of the menu item is specified in the previous message
$ Do not translate messages 24 and 28.
1 Predefinite
2 Dimensioni della finestra
3 D
4 -iso8859-1
5 %d punti
6 %.1f punti
7 %.1f punti
8 %.2f punti
9 Predefinita
10 Dimensione font
11 f

$   Window menu
12 Nuova
13 N
14 Stampare
15 S
16 Stampare...
17 t
18 Chiudere
19 C
20 Finestra
21 F

$   Edit menu
22 Copiare
23 C
24 Ctrl osfInsert
25 Ctrl+Insert
26 Incollare
27 I
28 Shift osfInsert
29 Shift+Insert
30 Editare
31 E

$ Option menu

32 Barra dei menu
33 m
34 Barra di scorrimento
35 B
36 Globali...
37 G
38 Terminale...
39 T
40 Ripristino soft
41 s
42 Ripristino hard
43 h
44 Ripristinare
45 R
46 Opzioni
47 O

$ Old Help menu (no longer used for CDE)

48 Panoramica
49 P
50 Attività
51 A
52 Riferimenti
53 R
54 Sull'elemento
55 S
56 Uso dell'aiuto
57 U
58 Informazioni su dtterm
59 I
60 Aiuto
61 A

62 Alt+F4

63 dtterm

$ Current CDE Help menu

64 Panoramica
65 P
66 Indice analitico
67 d
68 Indice
69 n
70 Attività
71 A
72 Riferimenti
73 R
74 Tastiera
75 T
76 Uso dell'aiuto
77 U
78 Informazioni su dtterm
79 I
80 Aiuto
81 A

82 Informazioni sul Terminale

83 Informazioni sul Terminale

$set 9
$ ****** Module: TermTerminalDialog.c
$ These are the messages for the terminal options dialog
1 Dtterm - Opzioni terminale
2 OK
3 Annullare
4 Aiuto
5 Controllo della tastiera
6 Normale
7 Applicazione
8 Modalità tasti cursore
9 Numerica
10 Applicazione
11 Modalità tastierino numerico
12 Solo Return
13 Return/Line Feed
14 Sequenza di avanzamento riga
15 Attivazione permanente
16 Attivazione non permanente
17 Tasti funzione dell'utente
18 Controllo dello schermo
19 Abilitata
20 Disabilitata
21 Commutazione a 132 colonne
22 Ritorno a capo automatico
23 Ritorno a riga precedente
24 Applicare
25 Ripristinare

26 Terminale - Opzioni

$set 10
$ ****** Module: sunDtTermServer.c
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Terminal - Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
$ "-NEW"
3 OK
