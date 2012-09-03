$ $TOG: dtcreate.msg /main/4 1999/09/17 17:24:05 mgreess $
$ %Z%%M%        %I%  %W% %G% %U%
$ 
$   COMPONENT_NAME: dtcreate
$ 
$   FUNCTIONS: none
$ 
$   ORIGINS: 27
$ 
$ 
$   (C) COPYRIGHT International Business Machines Corp. 1993
$   All Rights Reserved
$   Licensed Materials - Property of IBM
$   US Government Users Restricted Rights - Use, duplication or
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ **   Description:
$ **   ------------
$ **   This is the source message catalog file for dtcreate.

$set 2
$ *****************************************************************************
$ **
$ ** Message set for menu options on main window
$ **
$ *****************************************************************************
10 File
11 F
15 Aprire...
16 A
20 Nuovo
21 N
25 Salvare
26 C
30 Chiudere
31 U
35 Opzioni
36 O
40 Mostrare funzioni avanzate
41 f
42 Visualizzazione icone
43 i
44 A colori
46 C
47 Monocromatica
48 M
49 Icone monocromatiche
50 M
53 Aiuto
54 A
60 Panoramica
61 P
65 Indice
66 I
70 Attività
71 A
75 Riferimenti
76 R
80 Uso dell'aiuto
81 U
85 Informazioni su Creare azione
86 C

$set 3
$ *****************************************************************************
$ **
$ ** Message set for  main Create Action window
$ **
$ *****************************************************************************
10 Creare azione
12 Nome dell'azione (nome dell'icona):
13 Icone dell'azione:
20 Comando da eseguire al doppio clic sull'icona:
21 Comando da eseguire al doppio clic:
22 Comando da eseguire al doppio clic sull'icona:
30 Testo di aiuto per l'azione:
32 Tipo di finestra:
34 Grafica (finestra X)
36 Terminale (chiusura automatica)
38 Terminale (chiusura manuale)
40 Nessun output
50 Funzioni avanzate
52 Funzioni di base

$set 4
$ *****************************************************************************
$ **
$ ** Message set for  main Create Action window advanced function area
$ **
$ *****************************************************************************
10 Opzionale
11 All'avvio dell'azione, chiedere all'utente:
12 Tipi di dati che utilizzano l'azione:
13 Aggiungere...
14 Cancellare
15 Editare...
16 Tipi di dati accettati:
17 Tutti i tipi di dati
18 Solo i tipi nella lista

$set 5
$ *****************************************************************************
$ **
$ ** Error and Information message set for  main Create Action window
$ **
$ *****************************************************************************
10 Il nome dell'azione non è stato specificato.\n\
Inserire un nome nel campo 'Nome dell'azione'.
$ 
15 Il comando da eseguire non è stato specificato.\n\
Inserire un comando nel campo\n\
'Comando da eseguire al doppio clic sull'icona'.
$ 
20 La lista 'Tipi di dati che utilizzano l'azione' è vuota.\n\
Nel campo 'Tipi di dati accettati' è stata\n\
selezionata l'opzione 'Solo i tipi nella lista'.\n\
Cambiare la selezione in 'Tutti i tipi di dati'\n\
o aggiungere almeno un tipo di dati alla lista.
$ 
25 Esiste già un file che definisce un'azione e\n\
un tipo di dati con il nome specificato.\n\
È possibile annullare il salvataggio e cambiare il nome dell'azione,\n\
oppure sovrascrivere il file di definizione esistente.\n\
Si desidera sovrascrivere il file di definizione esistente?
$ 
30 Nel campo 'Comando da eseguire al doppio clic sull'icona'\n\
manca il nome di una variabile.\n\
Aggiungere una variabile all'etichetta inserita nel\n\
campo 'All'avvio dell'azione, chiedere all'utente'.\n\
Le variabili ammesse sono $*, $1, $2, ..., $9.
$ 
35 Le modifiche non sono state salvate.\n\
Si desidera salvare le modifiche prima di uscire?
$ 
40 Le modifiche correnti non sono state salvate.\n\
Si desidera salvare l'azione corrente prima di azzerare\n\
i campi per una nuova azione?
$ 
45 Le modifiche correnti non sono state salvate.\n\
Si desidera salvare le modifiche correnti prima di azzerare\n\
i campi per caricare un altro file di definizione?
$ 
50 Il nome dell'azione contiene uno o più caratteri\n\
non ammessi.\n\
I caratteri da non utilizzare per i nomi sono i seguenti:\n\
  / \ " ' ;  (  ) ~ ! $ { } < > # @ | &
$ 
55 Impossibile creare il file:
$ 
60 Impossibile aprire il file:
$ 
65 Il file seguente non può essere caricato perché non è\n\
stato originariamente creato con lo strumento Creare azione:
$ 
66 Il file non può essere caricato perché è stato modificato con\n\
uno strumento diverso da Creare azione. È possibile che il file\n\
sia stato modificato con un editor di testo o altri strumenti.
$ 
67 Lo strumento Creare azione può editare solo i file che\n\
siano stati creati ed editati con questo strumento.
$ 
68 Impossibile caricare il file:
$ 
$  The following messages are all part of the same message dialog
70 La nuova azione
71 è stata inserita nella cartella iniziale.
73 È stato creato un file di definizione dell'azione di nome:
$ 
80 Impossibile trovare tutte le dimensioni dell'icona\n\
selezionata.
$ 
85 Impossibile avviare l'Editor delle icone.
$ 
90 Impossibile analizzare il file:
$ 
95 Il file seguente non è stato trovato nel percorso di\n\
ricerca delle icone.
$ 
98 L'icona selezionata non si trova nel percorso di ricerca delle icone.\n\
Per visualizzare correttamente l'icona nell'ambiente del desktop,\n\
occorre spostarla in una directory inclusa nel percorso di ricerca\n\
delle icone.\n\
Scegliere 'OK' se si desidera usare questa icona.\n\
Scegliere 'Annullare' se si desidera selezionare un'altra icona.
$ 
$ ONLY TRANSLATE  "Usage"  and  "filename"
110 Sintassi: dtcreate [<nomefile>] [-?]
$ 
120 Non è stato selezionato un tipo di dati dalla lista\n\
'Tipi di dati che utilizzano l'azione'.
125 Selezionare il tipo di dati che si desidera Cancellare.
130 Selezionare il tipo di dati che si desidera Editare.
$ 
150 Il lavoro corrente non è stato salvato.
152 Non salvare
154 Annullare la chiusura
$ 
160 Impossibile salvare la sessione nel file:
162 Impossibile ripristinare la sessione dal file:
$ 
$ TRANSLATORS: DO NOT TRANSLATE the word "HOME" in the following message
170 La variabile d'ambiente HOME non è stata impostata.\n\
Per poter salvare questa azione è necessario che\n\
la variabile HOME sia stata impostata.

$set 6
$ *****************************************************************************
$ **
$ ** General message set for dialogs
$ **
$ *****************************************************************************
10 Ok
11 Applicare
12 Annullare
13 Aiuto
14 Ripristinare
18 Ricerca di un set di icone
19 Ricercare icone...
20 Editare icona...
31 Creare azione - Errore
41 Creare azione - Conferma
46 Creare azione - Informazioni
51 Creare azione - Domanda

$set 7
$ *****************************************************************************
$ **
$ ** Message set for Add Datatype and Edit Datatype dialogs
$ **
$ *****************************************************************************
10 Aggiunta di un tipo di dati
11 Editazione del tipo di dati
14 Nome del tipo di dati:
15 Caratteristiche di identificazione:
16 Editare...
17 Testo di aiuto per il tipo di dati:
18 Icone del tipo di dati:
21 Comandi
22 Comando di apertura per il tipo di dati:
23 Comando di stampa per il tipo di dati:
30 Aprire
31 Stampare

$set 8
$ *****************************************************************************
$ **
$ ** Error and Information message set for
$ **    Add Datatype and Edit Datatype dialogs
$ **
$ *****************************************************************************
10 Non è stato specificato il tipo di dati.\n\
Inserire un nome nel campo 'Nome del tipo di dati'.
$ 
15 Non sono state specificate le caratteristiche di identificazione.\n\
Per poter creare il tipo di dati è necessario specificare queste\n\
caratteristiche. Per inserire le informazioni richieste, selezionare il\n\
pulsante Editare vicino alla lista Caratteristiche di identificazione.
$ 
20 Il campo 'Nome del tipo di dati' contiene\n\
uno o più caratteri non ammessi.\n\
I caratteri da non utilizzare per i nomi sono i seguenti:\n\
  / \ " ' ;  (  ) ~ ! $ { } < > # @ | &
$ 
25 Esiste già un tipo di dati con il nome specificato.\n\
Specificare un altro nome nel campo 'Nome del tipo di dati'.

$set 9
$ *****************************************************************************
$ **
$ ** Message set for  Identifying Characteristics dialog
$ **
$ *****************************************************************************
10 Caratteristiche di identificazione
11 Includere
12 File
13 Cartelle
14 Con le seguenti caratteristiche:
20 Schema del nome:
21 Autorizzazioni:
22 Leggibile
24 Scrivibile
25 Eseguibile
27 E
28 Contenuto:
29 Schema:
30 Tipo:
31 Stringa
32 Byte (esadecimale)
33 Valore short (decimale)
34 Valore long (decimale)
35 Byte di inizio:
40 Lettura:
41 Scrittura:
42 Esecuzione:
43 Sì
44 No
45 Opzionale

$set 10
$ *****************************************************************************
$ **
$ ** Error and Information message set for Identifying Characteristics dialog
$ **
$ *****************************************************************************
10 Non è stato specificato uno schema per il nome.\n\
Se l'opzione Schema del nome è selezionata, è necessario\n\
inserire una stringa di identificazione.
$ 
15 Non sono state selezionate le autorizzazioni.\n\
Se l'opzione Schema delle autorizzazioni è selezionata,\n\
occorre specificare una o più delle opzioni disponibili.
$ 
20 Non è stato specificato lo schema del contenuto.\n\
Se l'opzione Contenuto è selezionata, occorre inserire\n\
una stringa nel campo Schema.
$ 
25 Non è stato specificato il valore del byte di inizio.\n\
Se l'opzione Contenuto è selezionata, occorre inserire\n\
un valore nel campo 'Byte di inizio'.
$ 
30 Non è stata selezionata alcuna caratteristica di identificazione.\n\
Selezionare una o più caratteristiche\n\
(Schema del nome, Autorizzazioni o Contenuto).
$ 
45 Lo schema del nome contiene uno o più caratteri\n\
non ammessi.\n\
Per lo Schema del nome non si possono utilizzare i\n\
seguenti caratteri:\n\
  / \ " ' ;  (  ) ~ ! $ { } < > # @ | &
$ 
46 Lo Schema del contenuto non corrisponde al formato del\n\
Tipo di contenuto selezionato.\n\
Reinserire lo Schema del contenuto nel formato corretto, oppure\n\
cambiare il Tipo in modo che corrisponda allo Schema del contenuto.
$ 
47 Il campo Byte di inizio contiene uno o più caratteri non ammessi.\n\
Questo campo può accettare solo numeri in formato decimale.
$ 
48 L'uso dello Schema del contenuto può rallentare sensibilmente\n\
le prestazioni della Gestione di file.\n\
Utilizzando lo Schema del contenuto, la Gestione di file deve\n\
esaminare il contenuto di tutti i file di una cartella prima\n\
di aprirla. Questo significa che l'apertura delle cartelle\n\
diventa molto più lenta.\n\
Per mantenere un buon livello di prestazioni, si consiglia\n\
di usare solo lo schema del nome e le autorizzazioni.

$set 11
$ *****************************************************************************
$ **
$ ** Message set for  Icon Selection Dialog
$ **
$ *****************************************************************************
10 Cartelle con icone
11 File delle icone
12 Nome del file dell'icona
20 Caricamento delle icone in corso. Attendere.
30 Cartelle delle icone
50 Il nome del file specificato per l'icona non è valido.\n\
Inserire un nome valido nel campo\n\
'Nome del file dell'icona'.
$ ONLY TRANSLATE  "Empty".   Do not translate the brackets "[" and "]".
60 [Vuoto]

$set 12
$ *****************************************************************************
$ **
$ ** Message set for  Open File Dialog
$ **
$ *****************************************************************************
10 Filtro
11 Cartelle
12 File
13 Selezione
20 Aprire file
25 Creare azione - Apertura di un file


$set 13
$ *****************************************************************************
$ **
$ ** Message set for  DieFromToolTalkError Dialog
$ **
$ *****************************************************************************
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Create Action - Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
$ "-NEW"
3 OK
