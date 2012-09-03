$ $TOG: dticon.msg /main/6 1999/09/17 12:19:13 mgreess $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dticon.msg
$ **
$ **  Project:     HP DT Icon Editor
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dticon
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1992, 1993, 1994 Hewlett-Packard Company
$ **     (c) Copyright 1993, 1994 International Business Machines Corp.
$ **     (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **     (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
$ **         Novell, Inc.
$ **
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **

$set 2
$ *****************************************************************************
$ ** _DtMessage set for fileIODialog.c
$ **
$ ** messages 10-16 are for the FileSelectionBox widget (default labels)
$ *****************************************************************************

2 Editor delle icone - File I/O
4 Editor delle icone - File I/O
6 Editor delle icone - Salvataggio come
8 Editor delle icone - Apertura file
10 Aggiornare
12 Directory
14 File
16 Aprire il file
17 Aprire il file
18 Aggiornare
20 SENZA NOME
22 Aprire
24 Salvare

$set 4
$ *****************************************************************************
$ ** _DtMessage set for newIconDialog.c
$ **
$ *****************************************************************************

2 newIconDialog
4 Editor delle icone - Ridimensionamento
6 OK
8 Annullare
10 Aiuto
12 Nuova larghezza:
14 Nuova altezza:

$set 6
$ *****************************************************************************
$ ** _DtMessage set for queryDialog.c
$ **
$ *****************************************************************************

2 Editor delle icone - Avvertenza
6 OK
$ ** DO NOT LOCALIZE - the user should never see this message
8 Do ya really wanna?
10 Annullare

$set 8
$ *****************************************************************************
$ ** _DtMessage set for stdErrDialog.c
$ **
$ *****************************************************************************

2 Editor delle icone - Errore
4 Continuare
$ 
$ "-NEW"
6 OK
$ 
$ "-NEW"
8 "Could not connect to ToolTalk:\n%s\nExiting ..."

$set 10
$ *****************************************************************************
$ ** _DtMessage set for utils.c
$ **
$ *****************************************************************************

$ ** DO NOT LOCALIZE messages 2-48, they appear in the errorlog only
2 Impossibile inizializzare l'icona del pulsante per il cerchio
4 Impossibile inizializzare l'icona del pulsante per il cerchio pieno
6 Impossibile inizializzare l'icona del pulsante per il poligono aperto
8 Impossibile inizializzare l'icona del pulsante per il poligono chiuso
10 Impossibile inizializzare l'icona del pulsante per il poligono chiuso pieno
12 Impossibile inizializzare l'icona del pulsante per l'ellisse
14 Impossibile inizializzare l'icona del pulsante per l'ellisse piena
16 Impossibile inizializzare l'icona del pulsante per la gomma
18 Impossibile inizializzare l'icona del pulsante per la linea
20 Impossibile inizializzare l'icona del pulsante per lo strumento di riempimento
22 Impossibile inizializzare l'icona del pulsante per il punto
24 Impossibile inizializzare l'icona del pulsante per il rettangolo
26 Impossibile inizializzare l'icona del pulsante per il rettangolo pieno
28 Impossibile inizializzare l'icona del pulsante per lo strumento di selezione
30 Impossibile analizzare il colore statico n.
32 Impossibile allocare il colore statico n.
38 Memoria esaurita(0)
40 Memoria esaurita(1)
42 Memoria esaurita(2)
44 Memoria esaurita(3)
46 Memoria esaurita(4)
48 Memoria esaurita(5)

50 Impossibile inizializzare la memorizzazione dell'icona dell'applicazione
52 Uso non ammesso del parametro '-f'
54 Uso non ammesso del parametro '-i'
56 Uso non ammesso del parametro '-session'
58 Formato non valido
60 Cattura dell'immagine dello schermo non riuscita
62 Operazione non riuscita.\nLa mappa dei colori è piena
64 Parametro della dimensione non valido

66 (SENZA NOME)

$set 12
$ *****************************************************************************
$ ** _DtMessage set for dtIconShell.c
$ **
$ *****************************************************************************

1 Editor delle icone
2 2 Colori
3 Editor delle icone - (SENZA NOME)
4 Benvenuti nell'Editor delle icone.
6 Riempimento
8 Colori statici
10 Colori dinamici
12 Grigi statici
14 Colore del primo piano
16 Colore dello sfondo
18 Ombreggiatura superiore
20 Ombreggiatura inferiore
22 Selezione
24 Trasparente

$ ** message 26-198 are menu items with corresponding mnemonics
$ ** (for set grouping information, see the comment at the end-of-file)
26 N
28 Nuovo...
30 A
32 Aprire...
34 S
36 Salvare
38 c
40 Salvare come...
42 C
44 Chiudere
46 F
48 File
50 A
52 Annullare
54 T
56 Tagliare area
58 C
60 Copiare area
62 I
64 Incollare area
68 s
70 Verso sinistra
72 d
74 Verso destra
76 R
78 Ruotare area
82 V
84 Verticalmente
86 O
88 Orizzontalmente
90 p
92 Capovolgere area
94 s
96 Riprodurre in scala area
98 d
100 Ridimensionare icona...
102 g
104 Aggiungere punto focale
106 l
108 Cancellare punto focale
110 m
112 Catturare immagine dello schermo
114 n
116 Cancellare icona
118 E
120 Editare
122 G
124 Griglia visibile
128 B
130 XBM
134 P
136 XPM
138 F
140 Formato di uscita
144 2x
146 3x
148 4x
150 5x
152 6x
154 8x
156 10x
158 12x
160 I
162 Ingrandimento
164 O
166 Opzioni
168 P
170 Panoramica...
172 A
174 Attività...
176 R
178 Riferimenti...
180 S
182 Sull'elemento
188 U
190 Uso dell'aiuto...
192 I
194 Informazioni sull'Editor delle icone...
196 A
198 Aiuto

$ ** message 200-are used in the "instruction" line for current drawing mode
200 Modalità corrente:
202 (pieno)
204 Matita
206 Riempimento
208 Linea
210 Poligono aperto
212 Rettangolo
214 Poligono chiuso
216 Cerchio
218 Ellisse
220 Gomma
222 Selezione
224 Non valida

$set 14
$ *****************************************************************************
$ ** _DtMessage set for help.c
$ **
$ *****************************************************************************

2 Editor delle icone - Aiuto
4 Selezionare un elemento\nall'interno dell'Editor delle icone.

$set 16
$ *****************************************************************************
$ ** _DtMessage set for process.c
$ **
$ *****************************************************************************

2 Il file è inaccessibile\no contiene dati non validi
4 Impossibile scrivere i dati nel file
6 Questa funzione non è ancora disponibile
8 Non sono disponibili\nimmagini precedenti
10 La larghezza e/o l'altezza\nspecificate non sono valide
12 Non è stata selezionata nessuna area
14 Non era stata tagliata o\ncopiata nessuna area
16 L'icona corrente non è stata salvata.\n\nLe modifiche verranno perse.
20 Controllare le autorizzazioni del file
22 Impossibile ripristinare la sessione dal file
24 Impossibile salvare la sessione in un file
25 Il file esiste già.\n\nSovrascrivere il file?


$set 18
$ *****************************************************************************
$ ** _DtMessage set for event.c
$ **
$ *****************************************************************************
2 Grandezza dell'imagine trascinata= %dx%d

$ *****************************************************************************
$ ** End of _DtMessage Catalog File
$ ** (Comments only below this line)
$ *****************************************************************************
$ **
$ *****************************************************************************
$ ** Unique mnemonic groupings from set 12
$ *****************************************************************************
$ **
$ ** Unique Set 1:  (Main Menu Bar)
$ **
$ **   46 F
$ **   48 File
$ **
$ **   118 E
$ **   120 Edit
$ **
$ **   164 O
$ **   166 Options
$ **
$ **   196 H
$ **   198 Help
$ **
$ **
$ ** Unique Set 2:  (File Menu)
$ **
$ **   26 N
$ **   28 New...
$ **   30 O
$ **   32 Open...
$ **   34 S
$ **   36 Save
$ **   38 A
$ **   40 Save As...
$ **   42 x
$ **   44 Exit
$ **
$ **
$ ** Unique Set 3:  (Edit Menu)
$ **
$ **   50 U
$ **   52 Undo
$ **   54 t
$ **   56 Cut Area
$ **   58 C
$ **   60 Copy Area
$ **   62 P
$ **   64 Paste Area
$ **
$ **        Unique Set 3a:  (Rotate Sub-Menu)
$ **        68 L
$ **        70 Left
$ **        72 R
$ **        74 Right
$ **   76 R
$ **   78 Rotate Area
$ **
$ **        Unique Set 3b:  (Flip Sub-Menu)
$ **        82 V
$ **        84 Vertical
$ **        86 H
$ **        88 Horizontal
$ **   90 F
$ **   92 Flip Area
$ **   94 S
$ **   96 Scale Area
$ **   98 z
$ **   100 Resize Icon...
$ **   102 A
$ **   104 Add Hot Spot
$ **   106 D
$ **   108 Delete Hot Spot
$ **   110 G
$ **   112 Grab Screen Image
$ **   114 e
$ **   116 Clear Icon
$ **
$ **
$ ** Unique Set 4:  (Options Menu)
$ **
$ **   122 V
$ **   124 Visible Grid
$ **
$ **        Unique Set 4a:  (Output Sub-Menu)
$ **        128 B
$ **        130 XBM
$ **        134 P
$ **        136 XPM
$ **   138 O
$ **   140 Output Format
$ **   160 M
$ **   162 Magnification
$ **
$ **
$ ** Unique Set 5:  (Help Menu)
$ **
$ **   168 I
$ **   170 Introduction...
$ **   172 T
$ **   174 Tasks...
$ **   176 R
$ **   178 Reference...
$ **   180 O
$ **   182 On Item
$ **   188 U
$ **   190 Using Help...
$ **   192 V
$ **   194 Version...
$ **
