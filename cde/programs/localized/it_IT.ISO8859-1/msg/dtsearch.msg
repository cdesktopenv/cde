$ $XConsortium: dtsearch.msg /main/9 1996/12/06 13:51:13 rws $
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
$ $Id: dtsearch.msg /main/9 1996/12/06 13:51:13 rws $
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

1 "<nullo>"
$ message 2 can be translated to match local conventions.  It is used
$ in output files for information.
2 "%d/%m/%Y,%H:%M:%S"
3 "*** %1$sMemoria esaurita a %2$s con una richiesta di %3$lu byte! ***\n"
4 "%1$s Versione %2$s.\n"
$ message 6 refers to any value which is null when printing information.
6 "<sconosciuto>"
8 "%1$s Non c'è un database valido."
9 "%1$s Argomento non valido nella riga di comando '%2$s'.\n"
10 "%1$s L'argomento sconosciuto è stato ignorato '%2$s'.\n"
11 "%1$s Il nome del database non è valido '%2$s'.\n"
$ In message 13, do not translate "dtsrcreate." It is an executable name.
13 "%1$s Nessun record DB nel database '%2$s'.\n\
  Controllare che il database sia inizializzato (eseguire dtsrcreate)."
14 "%1$s Schema versione '%2$s' non è compatibile con\n\
 il database '%3$s' versione '%4$s'.\n"
15 "%1$s Ricevuto segnale di terminazione %2$d.\n"
16 "<nessuna variabile d'ambiente>"
17 "<nessun serverdaemon>"
18 "%1$s Il nome del database non è stato specificato.\n"
19 " ,\t\n"
21 "%1$s Codice di uscita = %2$d.\n"
$ Message 22 can be translated.  It represents a human-readable date like
$ "Thursday, Jan 5, 1996, 6:30 pm".  The "%5$I" is 12 hour clock time, but
$ it could be changed to "%H" for 24 hour clock time.
22 "%A, %b %d %Y, %H:%M"
23 "%1$s: Versione %2$s.  Eseguito %3$s.\n"
$ Message 24 should not translate ".fzk".
24 "%1$s: directory corrente = '%2$s', file .fzk = '%3$s'\n"
25 "%1$s Errore programma. Terminazione.\n"
27 "%1$s: %2$s: Il formato del file non è valido."
96 "%s L'elenco delle ricorrenze è vuoto."
301 "%1$s Il testo immesso è  %2$s."
$ Msg 362 is a generic system message.  The format is
$ "<program name> <filename>: <system error>" where <system error>
$ is a system errno which will already be translated.
362 "%1$s %2$s: %3$s"
$ Msg 400 is like "<Program name>: <file name>, line <line number>:..."
400 "%1$s: %2$s, riga %3$ld: La parola '%4$s'contiene caratteri non validi."
$ Msg 423 is like "<Program name> Word <word> in <file name> is a duplicate."
423 "%1$s La parola'%2$s' è duplicata nel file %3$s."
1596 "%1$s %2$s Impossibile aprire il file di revisione %3$s: %4$s"

$ ****************************************************
$ set 2 formerly set MS_ausapi (ausapi.*)
$set 2

1 "%1$s Database incorretto o non valido '%2$s'."
$ Msg 7 should not translate "dbnames" or "dbcount."
7 "%s dbnames sono stati specificati. Il valore dbcount non è stato specificato."
20 "%1$s search_type non è valido '%2$c'."
30 "%s Richiesta vuota.  Nessuna ricerca eseguita."
$ Msg 37 should not translate "DtSearchInit."  It is a function name.
37 "%s La prima chiamata di funzione API deve essere DtSearchInit().\n"
40 "%1$s Cleartext nullo.  Nessuna evidenziazione."
$ Msg 115 must have the date formatted in the way specified.
115 "%1$s '%2$s' è una stringa della data non valida o non completa.\n\
Il formato corretto è '[yy]yy [mm [dd]]'."
$ Msg 187 must not translate "LOGNAME."  It is an environment variable.
187 "%1$s Le variabili d'ambiente ID d'utente e LOGNAME sono mancanti."
198 "%s Il valore 'Before' è posteriore o uguale al valore 'After'.\n\
  Non verrà restitutito alcun record."
216 "\n%1$s %2$s Segnale ricevuto %3$d.\n"
294 "%1$s Errore di programma in DtSearchResultsToText."
437 "%s La richiesta contiene una stringa 'navigator' non valida."
492 "%1$s Non c'è un elenco delle ricorrenze trovate."
498 "%1$s Argomenti non validi per DtSearchResultsToText."
621 "%1$s %2$s è già stato inizializzato."
806 "%1$s Impossibile iniziare la ricerca. La richiesta non è sufficiente oppure\n\
le opzioni specificate per la ricerca non sono compatibili con il database '%2$s."
$ Msg 1072 "stem" is the root of a word.
1072 "%1$s Errore di programma: Numero di radici (%2$d) supera il valore massimo (%3$d)."
1342 "%1$s Errore del motore di ricerca %2$d per la richiesta di evidenziazione\n\
nel database '%3$s', numero di ricorrenze=%4$ld, tipo di ricerca='%5$c', testo='%6$.30s'"


$ ****************************************************
$ set 5 formerly set MS_chandel, used in dtsrhan.c
$set 5

3 "Il file di output '%1$s' esiste già.\n"
$ Msg 4 needs to use the same 2 characters from messages 4 and 5.
$ The "q" is not needed because it is assumed that any character
$ other than "a" or "o" means to quit the process.
4 "Accodare, sovrascrivere, o uscire? [a,s,u] "
$ Msg 5 is the shortcut key for "a" or Append from message 4.
5 "a"
$ Msg 6 is the shortcut key for "o" or overwrite from message 4.
6 "s"
7 "Impossibile aprire il file di output '%1$s'.\n"
11 "\nErrore - impossibile aprire il file profilo '%1$s'.\n"
12 "Errore riga %1$d: identificatore non valido '%2$s'.\n"
13 "Errore riga %1$d - valore mancante per l'identificatore '%2$s'.\n"
14 "Errore riga %1$d - valore non valido o zero per '%2$s'.\ntoken non ammesso: %3$s.\n"
$ Msg 15 should not translate "ANY."
15 "Errore riga %1$d - per l'identificatore '%2$s', la colonna è\n\
   stata impostata ad ANY ma non si trova la stringa della specifica di identificazione.\n"
16 "Errore riga %1$d - valore non valido o zero per l'identificatore '%2$s'\n\
  token non ammesso: %3$s.\n"
18 "Errore riga %1$d - valore mancante per l'identificatore '%2$s'.\n"
23 "Avvertimento riga %1$d: il delimitatore è stato ridefinito.\n"
25 "Errore riga %1$d: il delimitatore non è stato impostato a 'top' o a 'bottom'.\n"
26 "Errore riga %1$d: token non valido '%1$s'.\n"
27 "Errore riga %1$d: token non valido '%2$s'.\n"
$ Msg 34 needs to match the 'all' from msgs 36 and 37.
34 "tutto"
$ Msg 35 needs to match the 'none' from msgs 36 and 37.
35 "nessuno"
$ Msg 36 needs 'all' to match msg 34 and 'none' to match msg 35.
36 "Errore riga %1$d: modalità dell'immagine deve essere 'tutto' o 'nessuno' -'%2$s' non è riconosciuto.\n"
$ Msg 37 needs "all" to match msg 34 and "none" to match msg 35.
37 "Errore riga %1$d: modalità del testo deve essere 'tutto' o 'nessuno' - '%2$s' non è riconosciuto.\n"
38 "Avvertimento riga %1$d: il carattere chiave è stato ridefinito.\n"
39 "Errore riga %1$d: carattere chiave non valido:'%2$c'.\n"
40 "Avvertimento riga %1$d - il campo chiave è stato ridefinito.\n"
43 "Errore riga %1$d - l'identificatore per la chiave '%2$s' non è valido.\n"
$ Msg 45 should not translate "discard."
45 "Errore riga %1$d: opzione sconociuta per 'discard': '%2$s'.\n"
$ Msg 46 should not translate "delblanklines."
46 "Errore riga %1$d: opzione sconosciuta per 'delblanklines': '%2$s'.\n"
47 "Errore riga %1$d: opzione sconosciuta per 'abstract':'%2$s'\n"
48 "Errore riga %1$d -tipo di identificatore sconosciuto '%2$s'.\n"
49 "Errore - delimitatore non definito.\n"
50 "Errore - il carattere tipo-di-chiave non è stato definito.\n"
51 "Errore - chiave non definita.\n"
52 "Errore - Il delimitatore definito '%1$s' non è stato trovato.\n"
53 "Errore - Il delimitatore definito '%1$s' punta ad una riga fisica nel record.\n\
   Poiché il delimitatore definisce le righe fisiche\n\
  non è possibile fare riferimento al delimitatore in questo modo.\n"
54 "Errore - per il campo '%1$s', nessun identificatore di riga corrisponde a '%2$s'.\n"
55 "Errore - l'elenco includere/escludere è incluso nel\n\
  campo '%1$s' che non è stato definito.\n"
56 "Errore - l'elenco includere/escludere immagine contiene la\n\
  riga '%1$s' che non è stata definita.\n"
57 "Errore - l'elenco includere/escludere testo contiene la\n\
  riga '%1$s' che non è stata definita.\n"
58 "Errore - la definizione della chiave punta al campo '%1$s'\n\
  che non è stato definito.\n"
59 "Errore - la definizione del riassunto punta al campo '%1$s'\n\
  che non è stato definito.\n"
68 "Avvertimento - non si trovano i campi necessari alla chiave.\n\
  Eliminazione del record %1$ld che inizia:\n\
  %2$s\n"
69 "Avvertimento - record %1$ld senza significato,\n\
  eliminazione in corso...\n\
  Il record inizia con: %2$.60s\n"
70 " Impossibile aprire il file di input '%1$s'.\n"
$ Msg 71 should not translate ".fzk."
71 "\n\
SINTASSI: %1$s [opzioni] <profilo> <input> [<output>]\n\
  -m         Visualizza solo i messaggi di errore.\n\
  -w<N>      Cambia la larghezza dello schermo in <N>.\n\
  -oo        Autorizza la sovrascrittura del file di output.\n\
  -oa        Autorizza l'aggiunta al file di output.\n\
  <profilo>  File di input che contiene il profilo dei record\n\
             da elaborare.\n\
  <input>   File di input che contiene i record effettivi.\n\
  <output>  File di output, formato .fzk (%2$s).\n\n"
72 "È stato specificato un valore di Larghezza schermo non valido.\n"
75 "'%1$s' non è ammesso nel modo di output.\n"
76 "Argomento sconosciuto nella riga comandi '%1$s'.\n"
77 "Non è stato specificato il nome del file profilo.\n"
78 "Non è stato specificato il nome del file di input.\n"
79 "Errore - se stdin viene utilizzato come input, occorre specificare un file di output.\n"
80 "Il file profilo ed il file di input hanno lo stesso nome: '%1$s'.\n"
81 "Il file di input ed il file di output hanno lo stesso nome: '%1$s'.\n"
82 "Il file profilo ed il file di output hanno lo stesso nome: '%1$s'.\n"
83 "        File profilo: %1$s\n"
$ Msg 84 should not translate "stdin."
84 "          File di input: stdin\n"
85 "          File di input: %1$s\n"
$ Msg 86 should not translate "stdout."
86 "         File di output: stdout\n"
87 "         File di output: %1$s\n"
88 "%1$s. %2$s %3$s Filtro per il testo.\n"
89 "\n  Ogni puntino corrisponde a %ld record...\n"
90 "\nErrore di chiusura del file di ouput - disco pieno?\n"
91 "Terminazione causata dagli errori nel file profilo.\n"
92 "\n%1$s: Completamento normale.  %2$ld record elaborati.  Codice di uscita = 0.\n"
93 "Errore riga %1$d - '%2$s' la stringa deve essere racchiusa tra virgolette.\n"
110 "Avvertimento riga %d - il campo data è stato ridefinito.\n"
111 "Errore riga %1$d - identificatore della data '%2$s' non è valido.\n"
115 "%1$s Le date predefinite degli oggetti saranno '%2$s'.\n"
116 "Errore - La data punta ad un campo non definito '%s'.\n"
124 "%1$s Impossibile scrivere nel file di output '%2$s':\n  %3$s\n"
$ Msg 125 should not be translated.
125 "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC"
130 "%1$s Impossibile caricare i file di elaborazione lingue:\n%2$s\n"
133 "\n\
Avvertimento - '%1$s' La data specificata non è valida.\n\
  Utilizzando la data '%2$s' per il record numero %3$ld che inizia: %4$.30s\n"


$ ****************************************************
$ set 6 formerly set MS_ve
$set 6

26 "dummy_workproc() chiamato.\n"
142 "%1$s Errore di client: È stato richiesto un record con indirizzo\n\
  %2$ld (%3$d:%4$ld, x'%5$08.8lx') che non è valido per il database '%6$s'."
156 "%1$s Tabella di compressione dati incompatibile con il database '%2$s'.\n\
  Il database è stato compresso con %3$ld, il motore di decompressione è %4$ld.\n"
157 "DTSRVE157 Errore di programma client: Indirizzo di database è Null in usrblk.dba."
167 "%1$s Il database '%2$s' è vuoto."
178 "%1$s Il database '%2$s' versione '%3$s' non è compatibile\n\
  con il motore della versione '%4$s'."
183 "%1$s Impossibile aprire il file semaforo delle note di utente '%2$s': %3$s.\n"
185 "%1$s Database '%2$s' la dimensione massima di parole %3$d è troppo corta."
199 "%1$s Impossibile ottenere il semaforo delle note '%2$s' dopo %3$d tentativi.\n"
230 "%1$s Impossibile aprire il file di backup '%2$s': %3$s."
231 "%1$sDatabase '%2$s': dimensione dei riassunti non è valida %3$d."
251 "%1$s Database '%2$s' danneggiato:\n\
Il numero di record non corrisponde al numero di indirizzi.\n"
$ Msgs 310 and 311 can be appended to msg 309 to produce output.
$ For example, one message might be
$ "<Programname> User notes disabled for entire Engine."
309 "%1$s Le note di utente sono disattivate"
$ Msg 310 can be appended to msg 309 to produce output.
310 "nel motore intero."
$ Msg 311 can be appended to msg 309 to produce output.
311 "nel database '%1$s'."
$ Msg 332 can be translated to local timestamp.  It currently outputs
$ something like "1996/03/04 at 16:40 CDT"
332 "%d %m %Y - %H:%M"
333 "\n <Nota di utente accodata da '%1$s' il %2$s>\n"
398 "DTSRVE398 la stringa da ricercare è NULL."

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
1 "%1$s%2$s: La parola chiave %3$s è obsoleta."
$ Msg 140 "(missing)" refers to a token which is expected to be found but
$ is missing.  The token is from an input string of words being indexed.
140 "(mancante)"
$ Msgs 142-168 are combined into "<token>: <error>" where msg 142 or a
$ default file name can be <token> and msgs 143, 165, 166, and 168
$ can be <error>.
142 "il resto della riga"
143 "è vuoto"
165 "ID sconosciuto"
166 "specificato precedentemente"
168 "la specificazione del percorso è mancante"
184 "%1$s %2$s: Il nome del database supera gli 8 caratteri %3$.16s'.\n"
$ Msg 221 should not translate "PATH."
221 "%1$s %2$s: Database '%3$s', Nessun PATH specificato, istruzione ignorata.\n"
$ Msg 275 should not translate "SEARCH."
275 "%1$s %2$s: Valore SEARCH non è valido ed è stato ignorato."
$ Msg 304 should not translate "MAXHITS."
304 "%1$s %2$s: Database '%3$s', valore MAXHITS non è valido.\n"
352 "%1$s %2$s: Valore %3$s non valido, la riga è stata ignorata.\n"
365 "%1$s %2$s: valore %3$s mancante."
414 "%1$s Impossibile aprire il file di configurazione '%2$s':\n  %3$s."
457 "'%1$c' Record"
477 "%1$s %2$s: Nome del database è mancante dopo la parola chiave '%3$s'.\n"
495 "%1$s %2$s: '%3$.16s' è una parola chiave non corretta. Riga %4$d ignorata.\n"
$ Msg 607 should not translate "FILEIO."
607 "%1$s %2$s: Valore FILEIO non corretto. Non c'è autorizzazione di scrittura/lettura per il file d'utente."
610 "%1$s %2$s: valore %3$s è '%4$s' e deve essere un numero intero positivo. Riga ignorata."
$ Msg 684 should not translate "KEYTYPE."
684 "%1$s %2$s: Database '%3$s', carattere KEYTYPE non valido '%4$c'."
732 "%1$s %2$s: Database '%3$s', Non sono ammessi più di %4$d keytype.\n"
$ Msg 903 should not translate "FILEPATH."
903 "%1$s %2$s: FILEPATH '%3$s' %4$s. Riga ignorata."
929 "%1$s File di configurazione '%2$s' è del vecchio stile."
$ Msg 986 should not translate "KEYTYPES."
986 "%1$s %2$s: KEYTYPES mancante per il database '%3$s'."
$ Msg 1002 should not translate "KEYTYPE."
1002 "%1$s %2$s: Database '%3$s', carattere KEYTYPE duplicato '%4$c'.\n"
1007 "%1$s%2$s: La stringa vuota %3$s è stata ignorata."
$ Msg 1011 should not translate "KEYTYPE," "UPPERKEYS," or "FALSE."
1011 "%1$s %2$s: Database '%3$s': Non sarà possibile accedere ai record\n\
  che contengono il carattere KEYTYPE in minuscolo '%4$c'.\n\
  Occorre impostare UPPERKEYS = FALSE per poter usare i caratteri in minuscolo."
1404 "%1$s Lo stato del file di configurazione '%2$s': %3$s non è disponibile."
$ Msg 1643 should not translate "DTSROCFPATH," "HOME," "/etc/inittab,"
$ or "/etc/inetd.conf."
1643 "%1$sImpossibile trovare o aprire il file di\n\
 configurazione '%2$s' nelle directory seguenti:\n\
  directory DTSROCFPATH =     %3$s,\n\
  directory corrente = %4$s,\n\
  directory iniziale (HOME) =            %5$s,\n\
  directory /etc/inittab  =    %6$s,\n\
  directory /etc/inetd.conf  = %7$s."
1869 "%1$s %2$s: Il file di configurazione deve specificare almeno un database."

$ ****************************************************
$ set 9 formerly set MS_oeinit, used in oeinit.c and opendblk.c.
$set 9

$ Msg 99 should not translate "open_dblk".  It is a function name.
99 "%s Errore di programma: Input non valido per open_dblk()."
137 "%1$s L'interfaccia utente della versione %2$s ed il motore della versione %3$s non sono compatibili."
202 "%s Inizializzazione fallita: errori nel file di configurazione."
265 "%s Non c'è un database valido."
266 "%1$s Inizializzazione fallita: errori nei file del database,\n\
  della lingua o nei file assocciati ad essi."
317 "%1$s Impossibile aprire il file database '%2$s'\n\
  Errore %3$d = %4$s\n\
  %5$s sta eliminando '%6$s' dall'elenco dei database disponibili."
318 "\n  Per correggere il problema, specificare un PATH valido\n\
  nel file di configurazione database PATH."
1404 "%1$s Eliminazione del database '%2$s' dall'elenco dei database disponibili: Lo stato del file %3$s: %4$s non è disponibile."

$ ****************************************************
$ set 10 formerly set MS_oe, used in oe.c and dbchange.c.
$set 10
48 "OE48 Richiesta abbandonata: il database '%1$s' non è disponibile in questa installazione."
71 "OE71 %1$s è scaduto."
$ Msg 76 should not translate "dblk".  It is a structure name.
76 "OE76 Errore di programma: Impossibile trovare dblk per il database '%1$s'."
217 "OE217 Errore di programma: il testo del record è troppo lungo."
367 "OE367: Errore dell'interfaccia utente.  %1$d non è un codice di richiesta valido.\n"
429 "OE429 Impossibile aprire il file di cancellazioni '%1$s': %2$s. Richiesta ignorata."
440 "OE440 Nessun keytype di record è stato selezionato nel database '%1$s'."
490 "OE490 %1$s Il motore è stato disattivato in modo permanente."
516 "OE516 La stringa di ricerca può contenere un massimo di %1$d caratteri."
523 "OE523 Richiesta negata: La prima richiesta deve essere l'inizializzazione del motore."
753 "%1$s Categorie semantiche non sono definite per il database '%2$s'."
1300 "%1$s Impossibile accedere allo stato del file database\n  '%2$s': %3$s"
1313 "%1$s *** RICHIESTA ANNULLATA *** %2$s La reinizializzazione del motore\n\
  è stata causata da modifiche apportate al file %3$s, probabilmente causate\n\
  dall'aggiornamento di uno o più database."


$ ****************************************************
$ set 11 formerly set MS_cravel, used in dtsrload.c.
$set 11
1 "\n\
SINTASSI: %1$s -d<nome_db> [opzioni] <input>\n\
       È possibile sovrascrivere l'estensione del nome file predefinita.\n\
  -d<nome_db>   Nome database di 1 a 8 caratteri. Includere prefisso di percorso\n\
                facoltativo. L'estensione verrà accodata al nome automaticamente.\n\
  -t<f_stringa> Fine della stringa che delimita il documento di testo. Predefinito '\\f\\n'.\n\
  -c            Conta i record per inizializzare il conteggio record del database.\n\
  -p<N>         Stampa un puntino ogni <N> record (predefinito %2$d).\n\
  -h<N>         Cambia la dimensione della tabella hash delle id duplicate\n\
		da %3$ld in <N>.\n\
               -h0 significa nessuna id duplicata, non cercarle.\n\
  -e<percorso>  Nome percorso della tabella di codifica huffman (predefinito %4$s).\n\
  <input>       Nome/percorso del file di input. Estensione predefinita %5%s.\n"
2 "%1$s: Codice di uscita = %2$d\n"
13 "%1$s Controllo delle id duplicate è stato disattivato.\n"
14 "\n%1$s Argomento sconosciuto '%2$s'.\n"
15 "\nSpecificare il nome del file di input.\a"
16 "\nNessun nome di database specificato (argomento -d).\a"
17 "%1$s Inizializzaione il conteggio totale di record. Conteggio in corso...\n"
18 "%1$s: '%2$s' versione schema = %3$s, conteggio record = %4$ld, ultimo slot = %5$ld.\n"
19 "%1$s: Numero totale di record nel database = %2$ld, ultimo slot = %3$ld.\n"
20 "%1$s %2$ld record elaborati in %3$ldm %4$lds, (%5$ld%%).\n\
  %6$ld duplicati, %7$ld nuovi, %8$ld aggiornati.\n"
29 "%1$s Impossibile accedere allo stato di %2$s: %3$s\n"
30 "%1$s Il file %2$s è vuoto.\n"
31 "%1$s: Ogni puntino corrisponde a %2$d record elaborati.\n"
33 "%1$s Record numero %2$ld eliminato:  Chiave troppo lunga:\n  '%3$s'.\n"
34 "%1$s Memoria esaurita!  Impostare l'argomento -h ad un numero inferiore,\n\
  o ridurre il numero di record di input."
35 "%1$s: Il record duplicato numero %2$ld '%3$s' è stato eliminato.\n"
246 "\n%1$s '%2$s' non è un nome di dizionario/percorso valido.\n"
522 "%s Questa versione di %s non supporta i database semantici.\n"
$ Msg 579 should not translate ".fzk."
579 "%1$s Record numero %2$ld eliminato:  Formato di file .fzk non valido.\n"
582 "%1$s L'argomento del puntino '%2$s' non è valido ed è stato ignorato.\n"
666 "%1$s Errore di programma: stato sconsciuto = %2$d.\n"
717 "%1$s Terminazione: il testo non compresso non è supportato.\n"
844 "%1$s Terminazione dopo %2$ld avvisi.\n\
  Il numero di avvisi è specifciato nella riga di comando.\n"
926 "%1$s Arg -m ignorato: %2$s: %3$s.\n"
927 "%1$s Record numero %2$ld eliminato:  Il primo carattere (keytype) della chiave\n\
  '%3$s' non è alfanumerico.\n"
1048 "%1$s Oggetti database non sono stati modificati perché  il file di input è vuoto.\n"
1086 "%1$s Record '%2$s' ha un formato data non valido--si userà la data 'run date'.\n"
1153 "%1$s L'id della tabella di compressione dati in '%2$s' è %3$ld.\n\
  Il database '%4$s' è stato compresso precedentemente con la tabella %5$ld.\n"
1215 "%1$s Avviso.  Record '%2$s' non contiene testo.\n"


$ ****************************************************
$ set 12 formerly set MS_initausd, used in dtsrcreate.c.
$set 12

$ Msg 3 should not translate "dtsearch.dbd."
3 "\n\
SINTASSI: %1$s [opzioni] <nome_db>\n\
       Crea e inizializza i file database DtSearch/AusText.\n\
  -q       Non stampare i messaggi informativi.\n\
  -o       Sovrascrive un database esistente.\n\
  -a<N>    Imposta il valore massimo del riassunto a <N> (predef. per tipo).\n\
  -d<dir>  Directory che contiene dtsearch.dbd (se non è la directory <nome_db>).\n\
  -wn<N>   Imposta la dimensione minima diparole a <N>. Valore predefinito %2$d.\n\
  -wx<N>   Imposta la dimensione mass. di parole a <N>. Predefinito per lingua.\n\
  ---------- Tipo di database ----------\n\
  -fd      Tipo di DtSearch.  Nessun documento, solo riferimenti nei riassunti.\n\
  -fa      Tipo AusText.  Documenti sono archiviati nel server centrale.\n\
  ------------ Lingue disponibili ------------\n\
  -l<N>    Impostare la lingua a <N>.  Valore predefinito: 0. Sono disponibili:\n\
            0 Inglese-ASCII\n\
            1 Inglese-Latin1\n\
            2 Spagnolo\n\
            3 Francese\n\
            4 Italiano\n\
            5 Tedesco\n\
            6 Giapponese-autoknj\n\
            7 Giapponese-knjlist\n\
  <nome_db> Percorso (facoltativo) e nome database di 1 - 8 caratteri.\n\
            Non specificare 'austext' o 'dtsearch'.\n"

5 "DTSRCREATE Il valore minimo %1$d è maggiore del valore massimo di dimensione parole %2$d.\n"
6 "DTSRCREATE Il valore minimo è stato cambiato in %1$d.\n"
8 "DTSRCREATE Il valore massimo è stato cambiato in %1$d.\n"
9 "DTSRCREATE La specifica di grandezza parole piccole può richiedere lunghi\n\
  aggiornamenti del file stopword per la prevenzione di forti aumenti\n\
  di requisiti nell'indice.\n"
10 "DTSRCREATE La specifica di grandezza parole larghe può aumentare molto\n\
  i requisiti di registrazione.\n"
$ Msg 12 needs option "y" for "yes" to be translated the same as in msg 15.
$ Option "n" does not need a translation because the code assumes anything
$ other than the translated "y" option means "no."
12 "\nFile '%s' esiste già .\n\
Si desidera sovrascrivere questo file e gli altri file di database? [s,n] "
$ Msg 13 should not translate "load_custom_language."
13 "%s Avviso! È stata specificata una lingua personalizzata che non è supportata.\n\
  Per accedere al database sarà necessario fornire al tempo di esecuzione un programma\n\
  di carico lingue nella funzione 'load_custom_language'.\a\n"

14 "\n\
%s L'elaborazione semantica è solo disponibile per i database di lingua inglese.\n\a"
$ Msg 15 needs "y" to match the "yes" option in msg 12.
15 "s"
24 "DTSRCREATE Database '%1$s' è stato inizializzato.\n"
98 "DTSRCREATE %1$s id della tabella di compressione Huffman non valida."
186 "DTSRCREATE186 Impossibile cambiare il nome di '%1$s' in '%2$s':\n  %3$s\n"
205 "DTSRCREATE205 Impossibile cambiare le autorizzazioni per '%1$s': %2$s\n"
213 "%1$s Impossibile aprire '%2$s':\n  %3$s.\7\n"
214 "%1$s Impossibile copiare '%2$s' in '%3$s':\n  %4$s\n"
230 "DTSRCREATE230 Non è stato possibile aprire il database '%1$s'.\n"
239 "DTSRCREATE239 Non è stato possibile inizializzare il database '%1$s'.\n"
244 "DTSRCREATE244 Impossibile togliere '%1$s':\n  %2$s\n"
326 "DTSRCREATE326 Impossibile cambiare le autorizzazioni di '%1$s':\n  %2$s\n\
  Tutti i file database devono avere l'autorizzazione di scrittura e di lettura.\n\
  Può essere impossibile aprire o inizializzare i file del database.\n"
327 "  Potrebbe essere inutilizabile se tutti i file di database non hanno\n\
  le autorizzazioni di scrittura e lettura.\n"
433 "%1$s Il valore massimo per la dimensione dei riassunti è stato aumentato a %2$hd.\n"
509 "DTSRCREATE509 Impossibile inizializzare il record d'intestazione.\n"

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
1 "database non aperto"
2 "set non valido"
3 "record non valido"
4 "database non valido"
5 "nome campo non valido"
6 "db_address non valido"
7 "nessun record corrente"
8 "set senza proprietario corrente"
9 "set senza membro corrente"
10 "valore chiave obligatorio"
11 "valore blocco non valido"
12 "record è proprietario di set non vuoti"
13 "record è un membro di set"
14 "membro ha già un proprietario"
15 "campo è una chiave composta"
16 "record non è collegato al set"
17 "campo non è una chiave valida"
18 "record non è proprietario valido del set"
19 "record non è membro valido del set"
20 "errore in d_setpages (database aperto o parametro non valido)"
21 "file dizionario non compatibile"
22 "tentativo non corretto di cancellare record di sistema"
23 "tentativo di bloccare un set/record già bloccato"
24 "tentativo di accedere ad un set/record sbloccato"
25 "id della transazione non è stata fornita"
26 "transazione già attiva"
27 "transazione non attiva"
28 "impossibile iniziare la transazione, file bloccati"
29 "impossibile liberare i blocchi nella transazione"
30 "troppe pagine modificate nel database"
31 "impossibile aggiornare il database fuori della transazione"
32 "è necessario l'accesso esclusivo"
33 "impossibile bloccare la scrittura di un file statico"
34 "id utente duplicata o non specificata"
35 "nome del database, del percorso o del file troppo lungo"
36 "Numero di file non valido è stato passato a d_renfile"
37 "campo non è un campo di chiave facoltativo"
38 "il campo non è definito nel tipo di record corrente"
39 "record/campo ha/in una chiave composta"
40 "numero di record o di set non valido"
41 "record/set senza ora-data"
42 "DBUSERID non ammesso (carattere non alfanumerico)"
43 "NetBIOS non è stato installato su questo computer"
44 "numero di elementi non corretto in DBDPATH"
45 "numero di elementi non corretto in DBFPATH"
46 "nessun tipo di record corrente"
47 "stringa  per l'ordinamento 'country' non valida"
48 "database non chiuso"
$ Msg 301 should not translate "errno."
301 "\n*** Sistema I/O errno %1$d = %2$s\n"
302 "\n\
*** The usual cause for this kind of error is a missing\n\
*** or read-only database file, or some system limit\n\
*** on the number of open files has been exceeded."
303 "(posizione non specificata)"
$ Msg 304 should not translate "db_VISTA."
304 "\n*** ripristino db_VISTA in corso...\n"
$ Msg 311 should not translate "db_status."
311 "*** Errore database a %1$s, db_status = %2$d: %3$n"
312 "Errore di programma."
313 "Errore sconosciuto."
315 "Il database non è disponibile: è già in uso da altri utenti."
348 "%1$s Nome del database incorretto '%2$s'."
378 "%1$s Impossibile aprire la stringa del nome:\n  '%2$s'"
379 "%1$s Impossibile aprire il database '%2$s':\n  %3$s."

$ For msgs 900-1000, see comments at top of set 13.
900 "spazio esaurito nel file"
901 "errore di sistema"
902 "errore di pagina"
$ Msg 903 should not translate "dio."
903 "nessun file di lavoro in dio"
904 "impossibile allocare memoria sufficiente"
905 "errore durante l'apertura del file"
906 "impossibile accedere al file taf del database"
907 "errore del file taf/log del database"
908 "blocchi database non coerenti"
909 "limite dei record superato"
910 "incoerenza del file chiave"
911 "il numero massimo di utenti è stato superato"
912 "errore di ricerca nel file database"
913 "il file specificato non è valido"
914 "errore durante la lettura del file database/overflow"
915 "errore di sincronizzazione nella gestione dei blocchi"
916 "debug check interrupt"
917 "errore di comunicazione della rete"
918 "recupero automatico in corso"
919 "errore durante la scrittura al file database/overflow"
920 "la Gestione blocchi non è stata installata"
921 "DBUSERID è già  in uso"
922 "la gestione blocchi è occupata"


$ ****************************************************
$ set 14 formerly set MS_cborodin, used in dtsrindex.c.
$set 14

1 "%1$s.  Esecuzione di %2$s.\n"
2 "%1$s: avvertenze rilevate."
$ Msg 6 should not be translated.
6 "put_addrs_2_dtbs_addr_file() nitems=%1$d, batchsz=%2$ld\n"
$ Msg 9 should not translate "d99."
9 "\n\
\n%1$s File d99 danneggiato per la parola '%2$s',\n\
  indirizzo database %3$ld alla posizione nel file %4$ld => bitvector[%5$ld],\n\
  l'allocazione massima bitvector = %6$ld.\n"
15 "%1$s input contiene meno di 2 byte.\n"
17 "\n\
SINTASSI: %1$s -d<nome_db> [opzioni] <input>\n\
       È possibile sovrascrivere l'estensione predefinita.\n\
 -d<nome_db>   Nome database di 1 a 8 caratteri.\n\
               Includere prefisso di percorso facoltativo.\n\
 -t<f_stringa> Fine della stringa che delimita il documento di testo. Predefinito '\\f\\n'.\n\
 -r<N>         Cambia il numero di record per puntino nel Pass 1 da %2$d in <N>.\n\
 -b<N>         Cambia grandezza batch da %3$ld in <N>.\n\
 -c<N>         Cambia 'paging' del database da %4$ld 1K pagine in <N> 1K pagine.\n\
               <N> >= 16 alla potenza di 2. All'inizio tentare solo piccoli cambiamenti.\n\
 -i<N>         Cambia grandezza del buffer di (i)nput dal default %5$d in <N>.\n\
 -h<N>         Cambia grandezza della tabella hash dei id record duplicati da %6$ld in <N>.\n\
               -h0 indica che non esistono duplicati: non eseguire un controllo.\n\
 <input>       [percorso]nome del file di input. Estensione predefinita = %7$s.\n"
21 "\n\
\n%s '%1$s' record supera il limite del conteggio parole.\n\
  Numero di record %2$ld > maxdba %3$ld, dba=%4$ld, sld00=%5$d, offs=%6$d.\n"
22 "\n\
\n%1$s '%2$s' record in database (dba=%3$ld)\n\
  supera l'allocazione bitvector (%4$ld >= %5$ld).\n"
33 "\n%1$s Numero di record in arrivo superato: %2$d.\n\
  Ciò causerà un errore 'Out of Paging Space'in Pass 2 e produrrà\n\
  un database non valido. Dividere il file in arrivo per ridurre\n\
  il numero di record oppure usare l'opzione -b e cominciare\n\
  di nuovo.\n"
40 "%1$ld record duplicati sono stati ignorati"
41 "Non è stato trovato nessun record duplicato."
164 "\n%1$s: %2$s Terminazione a causa di segnale %3$d.  Database %4$s\n\
  può essere danneggiato.  Ripristinare il database di backup.\n"
206 "%1$s: Codice di uscita = %2$d, Tempo trascorso %3$ldm %4$lds.\n"
$ Msg 280 should not be translated.
280 "\n%1$s num_addrs_for_word (%2$ld) >= batchsz (%3$ld).\n"
288 "%1$s Terminazione. Il file di input non contiene parole %2$s.\n"
374 "\n%1$s Memoria esaurita!\n\
  Dividere il file in arrivo e riprovare.\n"
539 "%1$s Controllo delle id duplicate è disattivato.\n"
550 "%1$s '%2$s' non è un nome di database/percorso valido.\n"
558 "%1$s Dimensione buffer di input non valida '%2$s'.\n"
567 "%1$s Argomento sconosciuto nella riga di comando '%2$s'.\n"
577 "%1$s Argomento non valido '%2$s'.  Si userà  il valore predefinito -r%3$d.\n"
580 "%1$s Specificare il nome del file di input.\n"
589 "%1$s Non è stato specificato nessun nome di database (-d argomento).\a\n"
595 "%1$s Argomento di dimensione batch non valido '%2$s'.\n"
600 "%1$sLa dimensione della cache è stata cambiata in %2$d.\n"
659 "\n%1$s: %2$s Eliminazione del record, la chiave eccede la lunghezza massima consentita:\n  '%3$s'.\n"
713 "\n%1$s La parola '%2$s' compare in %3$ld record\n\
   ma nel database esistono solo %4$ld record!\n\
  (Ottimo esempio per il file stopword.).\n"
727 "\n%1$s Il numero di record dbaddr errati (%2$ld)\n\
eccede l'allocazione bitvector (%3$ld).\n"
$ Msg 776 should not translate "d99."
776 "%1$s Errore scrittura del file d99: %2$s\n"
$ Msg 848 should not translate "d99."
848 "\n%1$s Impossibile leggere %2$ld bytes (%3$ld dba) del file d99\n\
  all'offset %4$ld.  Numero di dba letti (codice di ritorno) = %5$ld.\n"
849 "\n%1$s: Numero di parole %2$ld, %3$.0f%% completate.\n\
   Prognostico: %4$lum %5$02lus per finire.\n"
$ Msg 875 should not translate "fseek" or "d99."
875 "\n%1$s Impossibile fseek file d99 per offset %2$ld.\n"
1068 "%1$s Impossibile aprire il nuovo file dell'indice invertito '%2$s': %3$s\n"
1083 "%1$s Impossibile leggere il file di input '%2$s': %3$s\n"
1097 "%1$s Terminazione causata dal caricamento dei file di lingua.\n"
1108 "%1$s: Iniziando Pass 1, lettura dei record da '%2$s'.\n\
   Ogni puntino = %3$d record.\n"
$ Msg 1129 should not translate ".fzk."
1129 "%1$s: %2$s Formato di file .fzk non valido.\n"
1168 "%1$s: %2$s '%3$s' eliminato, chiave non si trova nel database.\n"
1190 "\n%1$s: Record numero %2$lu, %3$.0f%% completato.\n\
   Prognostico: %4$lum %5$02lus per completare Pass 1.\n"
1225 "%1$s: Pass 1 completato in %2$lum %3$lus, %4$lu record letti.\n\
  %5$s, %6$lu parole analizzate.\n"
1233 "%1$s: Iniziando Pass 2: aggiornamento del database e traversal batch dell'indice.\n\
  Ogni puntino = %2$d parole.\n"
1246 "%1$s: Pass 2 completato in %2$lum %3$lus, %4$lu parole aggiornate.\n"
1402 "%1$s: Eliminato il record duplicato numero %2$lu '%3$s'.\n"


$ ****************************************************
$ set 15 formerly set MS_delsfx, now MS_lang, used in lang.c and jpn.c
$set 15

$ Msg 4 should not translate "dblk."  It is a structure name.
4 "%s dblk non è stato inizializzato.\n"
$ Msg 5 should not translate "parg."
5 "%s Errore del programma: parg non contiene un file o una stringa.\n"

8 "\n\
%1$s '%2$.*s...' supera la grandezza massima\n\
di parole nel database '%3$s'."

10 "%s Errore del programma: stato dell'analisi sconosciuto.\n"
12 "%1$s '%2$.120s' non è una parola valida nel database. '%3$s'."

17 "\n\
%1$s '%2$s' è minore del valore minimo\n\
consentito per parole nel database '%3$s'."

19 "%1$s La parola '%2$s' non è inserita nel '%3$s'."
20 "%1$s Errore del progamma: jstate sconosciuto %2$d.\n"
21 "%1$s '%2$.*s' non è una parola giapponese valida."
23 "%1$s Parola '%2$s' nella riga %3$ld è troppo corta."
24 "%1$s Il file di parole '%2$s' non contiene parole giapponesi."
25 "%1$s Database '%2$s' percorso troppo lungo: '%3$s'."
31 "%s Il file dei suffissi non è stato caricato.\n"
$ Msg 41 should not translate "Paice."
41 "%1$s %2$s: Paice Rule non valido dalla posizione '%3$s'."
$ Msg 42 should not translate "Paice."
42 "%1$s %2$s: Paice Rules non ordinati dalla posizione '%3$s'."
45 "%s Memoria esaurita.\n"
50 "Inglese-ASCII"
$ Msg 51 refers to the Latin 1 codeset.
51 "Inglese-Latin1"
52 "Spagnolo"
53 "Francese"
54 "Italiano"
55 "Tedesco"
56 "Giapponese-comp"
57 "Giapponese-.knj"


$ ****************************************************
$ set 16 formerly set MS_vewords, now MS_boolsrch, used in boolsrch.c.
$set 16

$ Msg 2 is like "<program name> Part of ... term <word> which is
$ not in database <database name>."
2 "\n\
%1$s Una parte dell'operazione di collocazione include\n\
il termine '%2$s' che non si trova nel database '%3$s'."
$ Msg 6, variable 3 is the database name.
6 "\n\
%1$s Errore del database.  La parola '%2$s' è\n\
elencata nel '%3$s' senza un record dell'indice."
$ Msg , variable 3 is the database name.
11 "%1$s La parola '%2$s' non è stata trovata nel database '%3$s'."
$ Msg 14 should not translate "WHITLIM."  The message is like
$ "<Program name> <word in query> has more than <some number> hits."
14 "\n\
%1$s Si trovano più di %3$ld ricorrenze della parola '%2$s'.\n\
Togliere la parola dalla richiesta oppure modificare il valore WHITLIM\n\
nel file di configurazione del motore di ricerca."
15 "\n\
%s È stato richiesto di controllare tutti i documenti del\n\
database che non contengono nessuna delle parole richieste.\n\
Questa ricerca potrebbe richiedere un lungo tempo."

$ Msg 28 should not translate "d99."  Variable 2 is the database name.
28 "%1$s Errore di lettura Database in %2$s.d99."
$ Msg 32 should not translate "d99."  The message is formed using either
$ msg 33 or 34 as variable 2, and it is like "<program name> Database
$ Error: Word <word> in database <database name> has invalid d99 record
$ number <number>."
32 "\n\
%1$s Errore di database: %2$s '%3$s'\n\
nel database '%4$s' ha un numero di record d99 non valido %5$ld."
$ Msg 33 is used for variable 2 of message 32.
33 "Parola"
$ Msg 34 is used for variable 2 of message 32.
34 "Radice di"
$ Msg 35 should not translate "stemct," and "pmsz."
35 "%1$s Errore di programma: stemct=%2$d pmsz=%3$d\n"

$ ****************************************************
$ set 17 formerly set MS_vestatis
$set 17

104 "Impossibile staccare la memoria condivisa."
107 "Qsort stack overflow.\n"
110 "Impossibile togliere la memoria condivisa."
115 "Non c'e memoria condivisa disponibile"
310 "%1$s Memoria esaurita. Sono necessari  %2$ld byte.\n"
314 "Memoria esaurita.\n"
421 "Numero totale di ricorrenze = %1$ld. Le ricorrenze oltre il valore\n\
  massimo specificato verranno ignorate."
$ Msg 437 should not be translated.
437 "%1$s db_status = %2$d, dba = %3$d:%4$ld (x'%5$08.8lx'), vistano = %6$d.\n"
504 "Impossibile allocare cur_ditto.\n"
677 "Il campo Oggetto della ricerca è vuoto."
990 "Ordinamento in corso. Attendere..."


$ ****************************************************
$ set 23 formerly set MS_dbrec

$ Set 23 has several words in parenthesis which should not be translated.
$ For example, msg 3: "objkey,"  msg 5: "fzkeysz," msg 12: "abstrsz," 
$ msg 24: "minwordsz," msg 26: "maxwordsz," ...  The single words in
$ parenthesis should not be translated.
$ Msgs like 10, where the entire text is in parentheses should be translated.

$set 23

1 "---------- Valori di sistema per il database '%1$s' ----------\n"
2 "Il numero della versione dello Schema (version) è '%1$s'.\n"
$ Msg 3 should not translate "sizeof" or "objkey."
3 "Dimensione massima della chiave dell'oggettto (sizeof(objkey)) è %1$ld byte.\n"
$ Msg 4 "Huge" means very, very large.
4 "Le chiavi facoltative 'grandissime' sono state abilitate.\n"
5 "Il numero massimo di categorie semantiche (fzkeysz) è %1$d.\n"
10 "  (L'accesso semantico agli oggetti è disattivato.)."
12 "La lunghezza massima della stringa riassunto (abstrsz) è %1$d.\n"
14 "  (I riassunti non vengono utilizzati in questo database)."
20 "I riassunti sono %1$s compressi.\n"
22 "L'analisi di lingua è numero %1$d, %2$s.\n"
24 "La lunghezza minima delle parole (minwordsz) è %1$d.\n"
26 "La lunghezza massima delle parole (maxwordsz) è %1$d.\n"
30 "Numero di slot .d00 per oggetto (recslots) è %1$d.\n"
36 "  (Numero massimo di oggetti database è %1$ld).\n"
40 "L'id di tabella di compressione Huffman (hufid) è %1$ld.\n"
42 "  (La compressione non è abilitata in questo database.)."
44 "  (La tabella di compressione non è stata definita ancora)."
50 "L'accesso ai dati può  variare da oggetto in oggetto."
54 "Il motore non può accedere direttamente agli oggetti dati."
56 "Gli oggetti dati sono archiviati come testo compresso."
60 "Solo i rifermenti server agli oggetti sono archiviati come testo compresso."
64 "Solo i riferimenti server agli oggetti sono archiviati come testo compresso."
68 "Le chiavi di oggetto sono riferimenti dal server agli oggetti."
72 "Le chiavi di oggetto sono riferimenti dal client agli oggetti."
74 "Riferimenti agli oggetti dal server sono archiviati nelle chiavi 'grandissime'."
80 "Riferimenti agli oggetti dal server archiviati nei riassunti."
86 "Riferimenti agli oggetti dal client sono archiviati nei riassunti."
90 "Errore: Il significato del valore or_dbaccess (%1$hd) è sconosciuto.\n"
100 "Il testo compresso del tipo 'Repository' è %1$scompressed.\n"
110 "Il testo compresso del tipo 'Repository' non è usato in questo database."
120 "I parametri database (dbflags) sono 0x%1$lx:\n"
$ Msgs 124 and 125 are used in the variable in messages 130 and 140.
$ For example: "Inverted index INCLUDES words exactly as parsed."
124 "INCLUDE"
125 "ESCLUDE"
$ Msgs 126 and 127 are used in the variable in messages 160, 162, 164.
$ For example, "Use of optional 'huge' keys is ENABLED."
126 "ATTIVATO"
127 "DISATTIVATO"
$ Msg 130 uses msgs 124 and 125 in the variable.
130 "  L'indice invertito %1$s le parole esattamente come sono analizzate.\n"
$ Msg 140 uses msgs 124 and 125 in the variable.
140 "  L'indice invertito %1$s le radici delle parole.\n"
$ Msg 160 uses msgs 126 and 127 in the variable.
160 "  L'uso delle chiavi facoltative 'grandissime' è %1$s.\n"
$ Msg 162 uses msgs 126 and 127 in the variable.
162 "  Marcare-per-cancellare è %1$s.\n"
$ Msg 164 uses msgs 126 and 127 in the variable.
164 "  L'uso delle note utente è %1$s.\n"
$ Msg 170 is followed by either msg 172 or msg 174.
$ For example, "Text characters are a SINGLE byte wide."
170 "  I caratteri di testo sono di %1$s.\n"
172 "DUE byte"
174 "UN byte"
200 "Il numero corrente di oggetti database (reccount) è %1$ld.\n"
210 "Il numero dell'ultimo slot utilizzato (maxdba) è %1$ld.\n"
310 "\n\
SINTASSI: %1$s <nome_db>\n"
330 "Impossibile aprire il database %1$s'.\n%2$s\n"
340 "Nessun record dbrec nel database '%1$s'.\n"
350 "Impossibile leggere il record dbrec nel database '%1$s'.\n%2$s\n"

$ ****************************************************
$ set MS_dtsrkdump is set 25.  5/20/96.  dtsrkdump.c
$set 25

1 "%s Errore di programma. Terminazione.\a\n"
$ Msg 2 should not be translated
2 "%1$c dba=%2$d:%3$-7ld ofs=%4$-9ld adr=%5$-6ld fre=%6$ld\n"
$ Msg 3 is like "<program name> <RCS revision number>, engine
$ <DtSearch version>.  <time stamp>."
3 "%1$s %2$s, motore %3$s.  %4$s.\n"
$ Msg 4, option "-p" can include decimal points -- this is handled by
$ using sscanf, so if sscanf can read numbers with commas in them, then
$ the "-p" option will process that correctly.  atof is used on what
$ sscanf reads in.
4 "\n\
SINTASSI: %1$s -o|w|ow [-v] [-t<N> | -p<N>] <nome_db>\n\
       Legge i file chiave DtSearch e stampa un riassunto.\n\
  -o        Chiavi da analizzare sono del tipo OBJECT.\n\
  -w        Chiavi da analizzare sono WORDS dell'indice invertito.\n\
  -v        Modo VERBOSE, elenca ogni chiave.\n\
  -t<N>     Soglia.  Imposta le opzioni 'w' e 'v' ed elenca solo le parole\n\
            con un indirizzo  >= <N>.  Se <N> = 1, elenca tutte le parole.\n\
  -p<N>     Altra soglia.  L'uso è uguale a -t solo che <N> è una percentuale\n\
            del database intero. (<N> può includere un punto decimale).\n\
            Per esempio -p99.9 stampa ogni parola occorrente in 99.9%%\n\
            o più dei record. Ottimo modo di trovare candidati per il file stopword.\n\
            Se 'w' e 'v' sono impostati senza alcun valore di soglia,\n\
            il predefinito = -t%2$d.\n\
  <nome_db> Nome database di 1 a 8 caratteri.\n\
            Include prefisso di percorso facoltativo.\n"
5 "%1$s Percentuale non valida %2$lf.\a\n"
53 "%1$s Valore di soglia non valido.\a\n"
55 "%1$s Argomento sconosciuto nella riga di comando '%2$c'.\a\n"
56 "%1$s Specificare il nome del database.\a\n"
57 "%1$s È necessario specificare -o oppure -w.\a\n"
58 "%1$s Nome database non valido '%2$s'.\a\n"
$ Msg 60 should not be translated.
60 "%1$s: '%2$s' reccount=%3$ld maxdba=%4$ld recslots=%5$hd minw=%6$hd maxw=%7$hd\n"
70 "%1$s Elencherà solo parole che occorrono in %2$ld o più record.\n"
80 "%1$s: Elencando tutte le parole nel database.\n"
$ Msg 100 should not be translated.
100 "dba x%1$08lx, %2$6ld\n"
$ Msg 110 is like "in <directory name>"
110 " in %1$s"
120 "Riepilogo Oggetti per '%1$s'%2$s:\n"
130 "Numero di oggetti per Keytypes:"
160 "TOTALE oggetti = %1$ld\n"
$ Msg 170 should not translate "DBA."
170 "DBA Largest Object DBA  = %1$ld\n"
180 "%1$s: * Le parole segnalate da un asterisco compaiono in ogni record.\n"
200 "Riepilogo Parole per '%1$s'%2$s:\n"
210 "TOTALE parole = %1$ld\n"

$ ****************************************************
$ set MS_dtsrclean is set 26.  5/21/96.  dtsrclean.c
$set 26

1 "\n\
SINTASSI: %1$s [opzioni] <nome_db> <nuovo_percorso>\n\
       Comprime lo spazio d99 non utilizzato e convalida i collegamenti d00-d99.\n\
  -p<N>     Stampa un puntino ogni <N> record (predefinito = %lu).\n\
            Stampa un messaggio relativo ogni %d puntini.\n\
  -oy       Autorizza la sovrapposizione dei file d99/d2<N> già esistenti.\n\
  -on       Forza l'uscita se il nuovo percorso include file d99/d2<N> già esistenti.\n\
  -v        Convalida i collegamenti d99 e d00, corregge un file d99 danneggiato e\n\
            assicura l'accuratezza del conteggio record. Usare anche -c0 per correggere\n\
            un database intero.\n\
  -v<nomef> Uso uguale a -v ma scrive tutti i record d00 che non sono specificati\n\
            da d99 al file fname in un formato appropriato per l'eventuale\n\
            estrazione in formato di file .fzk.\n\
  -c<N>     Esce se si trovano più di <N> collegamenti danneggiati o incompleti.\n\
            Predefinito = %d. Il limite viene disattivato da  -c0.\n\
  <nome_db> Nome database di 1 a 8 caratteri. Indica i vecchi file d99/d2<N>\n\
            da aggiornare. I file si trovano in una directory locale.\n\
<nuovo_percorso> Specifica la posizione dei nuovi file d99/d2<N>. Se il primo\n\
            carattere non è una barra inclinata, il percorso è relativo alla\n\
            directory locale.\n\
CODICI DI USCITA:\n\
     0: Operazione riuscita.  1: Avviso.  2: Lavoro non iniziato.\n\
  3-49: Operazione incompleta, Lavoro finito prematuramente,\n\
        vecchi file sono ok, nuovi file non sono utilizzabili.\n\
 50-99: Errore irreversibile, il vecchio database può essere danneggiato.\n\
  100+: Ctrl-C, kill, e tutti gli altri segnali d'interruzione provocano la\n\
        terminazione dell'operazione. I nuovi file possono essere non utilizzabili.\n\
  Segnale = codice di uscita - 100.\n"
2 "%1$s: %2$s Compressione %3$d%% (circa %4$lu KB) in %5$ld:%6$02ld min:sec.\n"
$ Msg 3 should not translate "d99."  "WORDS" should be translated.
3 "%1$s: Conteggio: %2$ld PAROLE nel %3$s.d99.\n"
4 "%1$s: Nessun collegamento danneggiato.\n"
5 "%1$s: Codice di uscita = %2$d.\n"
$ Msg 6 should not be translated.
6 "  DBA = %1$d:%2$ld (x%3$02x:%4$06lx),  orig addr val = x%5$08lx\n\
  Word='%6$c%7$s' offset=%8$ld addrs=%9$ld free=%10$d\n"
7 "%1$s: Copiando dai vecchi file d2<N> a %2$s...\n"
$ Msg 11 is like "<program name> Version <version number>. Run <date program
$ was run>."
11 "%1$s Versione %2$s.  Eseguito %3$s.\n"
$ Msg 14 should not translate "d99."
14 "%1%s: Vecchi file: '%2$s.d2x, .d99'.\n"
$ Msg 15 should not translate "d99."
15 "%1$s: Nuovi file: '%2$s.d2x, .d99'.\n"
24 "%1$s: Uno o più file nuovi esistono già.\n"
$ Msg 25 should not be translated.
25 "%1$s: curr reccnt=%2$ld, mxdba=%3$ld, sl/rec=%4$ld, tot#adr=%5$ld.\n"
26 "%1$s: Lettura del file d00.  Ogni puntino rappresenta circa %2$ld documenti del database...\n"
$ Msg 27 should translate DOCUMENTS.
27 "%1$s: %2$ld DOCUMENTI confermati in %3$s.d00.\n"
$ Msg 28 should translate DOCUMENTS.
28 "%1$s: %2$ld DOCUMENTI nel %3$s.d00 non = %4$ld archiviati.\n\
  Il conteggio verrà corretto nel nuovo file d00.\n"
29 "%1$s: Compressione in %2$s.  Ogni puntino rappresenta circa %3$lu parole...\n"
$ Msg 45 needs the "y" for "yes" to match msg 46.
45 "    Sovrapporre i file nella nuova directory? [s/n] "
$ Msg 46 must match the "yes" option from msg 45.
46 "s"
66 "%1$s Terminazione dopo segnale di interruzione %2$d.\n"
$ Msg 111 should not translate "d99."
111 "*** %1$s DBA in d99 = -1.  Possibilità di errore nell'area\n\
  di espansione dovuto a valori 'conteggio' incorretti nel file d2x.\n"
159 "%1$s Argomento sconosciuto: '%2$s'.\n"
177 "%1$s Argomento %2$.2s non valido.\n"
193 "%1$s Terminazione dopo %2$ld collegamenti danneggiati.\n"
194 "%1$s %2$s %3$ld collegamenti danneggiati o incompleti.\n"
210 "%1$s Manca l'argomento obbligatorio nome_db.\n"
211 "%1$s Manca l'argomento obbligatorio nuovo_percorso.\n"
212 "%1$s Troppi argomenti.\n"
$ Msg 222 should not translate "d99" or "DBA."
222 "*** %1$s DBA in d99 non in d00, slot > numero mass. di documenti.\n"
229 "%1$s Nome del database non valido '%2$s'.\n"
301 "%1$s L'argomento -m non è necessario.\n"
$ Msg 333 should not translate "d99" or "d00."
333 "*** %1$s DBA in d99 non esiste in d00.\n"
393 "%1$s La directory vecchia e quella nuova sono identiche: '%2$s'.\n"
439 "%1$s Impossibile aprire %2$s: %3$s\n"
440 "%1$s Impossibile accedere allo stato di %2$s: %3$s\n"
$ Msg 444 should not translate "d99."  Do not translate starting with "DBA = ..."
444 "*** d99 non punta su %1$s record d00 '%2$s'.\n\
  DBA = %3$d:%4$ld (x%5$02x:%6$06lx).\n"
463 "%1$s L'argomento nella riga di comando disattiva la sovrapposizione dei file.\n"
$ Msg 465 "Validate" should match the description for the "-v" option in msg 1.
465 "%1$s WARNING: Impossibile allocare la memoria per bitvector.\n\
  Modalità 'Convalidare' disattivata.\n"
489 "%1$s Errore di scrittura in %2$s: %3$s.\n"
499 "%1$s %2$s è vuoto.\n"
517 "%1$s Errore di lettura in %2$s: %3$s.\n"
$ Msg 561 should not translate "DBA," "maxdba," or "dbrec."
561 "%1$s DBA '%2$d:%3$ld' (record numero %4$ld) in d00 eccede il numero totale di indirizzi %5$ld;\n\
  Bit vector overflow causato da maxdba non valido in dbrec: %6$ld\n"
$ Msg 657 is like "<program name> Read error on <filename>: <system errno>."
657 "%1$s Errore di lettura in %2$s: %3$s.\n"
$ Msg 665 is like "<program name> Write error on <filename>: <system errno>."
665 "%1$s Errore di scrittura in %2$s: %3$s.\n"
$ Msg 802 is like "<program name> Unable to open <filename> to output
$ unreferenced d00 records: <system errno>."  Do not translate "d00."
802 "%1$s Impossibile aprire '%2$s' per estrarre i record d00 non specificati:\n  %3$s\n"

$ ****************************************************
$ set MS_dui is set 27.  5/21/96.  dui.c
$set 27
1 "%1$s DtSearch Versione %2$s.\n\
SINTASSI: %3$s [-opzioni] <oggetto>...\n\
       Opzioni che iniziano con lo stesso carattere si escludono a vicenda.\n\
  -ss          Ricerca del tipo S (radici) e non la ricerca predefinita (statistiche).\n\
  -sw          Ricerca del tipo W (parole esatte) e non la ricerca predefinita (statistiche).\n\
  -pa          Stampa i riassunti (predefinito).\n\
  -pk          Stampa le chiave di record.\n\
  -pb          Stampa i riassunti e le chiavi di record.\n\
  -p<N>        Trova e stampare il record numero N dell'elenco ricorrenze.\n\
  -k<C>        Limita la ricerca alle chiave che iniziano con il carattere <C>.\n\
  -d<nome_db>  Nome del database di 1 - 8 caratteri.  Il valore predefinito è\n\
               il primo database nel file di configurazione.\n\
  -m<N>        Numero massimo di ricorrenze. Valore predefinito: %4$d.\n\
  <oggetto>    Insieme di parole da ricercare. Racchiudere il testo tra\n\
               virgolette per includere l'uso di operatori Boole.\n"
4 "Ricerca nel database '%1$s'...\n"
9 "<nessun riassunto disponibile>"
117 "%1$s Argomento non valido nella riga di comando '%2$s'.\a\n"
125 "%1$s L'oggetto della ricerca non è stato specificato.\a\n"
165 "%1$s DtSearch inizializzazione fallita:\n  %2$s\n"
190 "%1$s Non ci sono ricorrenze.\n"
195 "%1$s Ricerca fallita.\a\n"
200 "%1$s Il testo non è disponibile per il documento '%2$s'.\n"
203 "%1$s Impossibile individuare i keytype disponibili:\n  %2$s\n"
224 "%1$s Non si trovano chiavi di record inizianti con '%2$c' nel database '%3$s'.\n"
247 "%1$s ----- Messaggi di motore dopo l'inizializzazione -----\n\%2$s\n"
248 "%1$s ----- Fine dei messaggi di inizializzazione -----\n"

$ ****************************************************
$ set MS_boolpars is set 28.   5/22/96.
$set 28

1 "\n\
%1$s La stringa non è valida per la ricerca con operatori\n\
booleani. Specificare una nuova stringa e riprovare."

2 "%1$s Il campo Oggetto della ricerca è vuoto."

$ Msg 3 should not translate "@".
3 "\n\
%1$s L'operatore @ deve essere posizionato\n\
tra due parole."

$ Msg 4 variable 2 is <database name>.
4 "\n\
%1$s Una o più delle parole specificate non sono archiviate\n\
nel database '%2$s'."

$ Msg 5 "AND" should be the local equivalent to the Boolean Operator
$ for "AND."  "&" should not be translated.
5 "\n\
%1$s Gli operatori booleani devono essere posizionati tra\n\
parole o tra espressioni.  La sequenza di due parole senza un operatore\n\
verrà interpretata come se fosse l'operatore E (&) tra le parole."


6 "%1$s Manca l'espressione tra parentesi."

$ Msg 5 "NOT" should be the local equivalent to the Boolean Operator
$ for "NOT."  "~" should not be translated.
7 "\n\
%1$s L'operatore NON (~) deve essere posizionato alla\n\
sinistra della parola o dell'espressione associata."

8 "%1$s La stringa non è valida per la ricerca booleana.  Errore di sintassi numero %2$d."

9 "%1$s Troppe voci per la ricerca booleana."

$ Msg 10 variable 2 is <database name>.
10 "%1$s Ricerche del tipo Collocazione non sono disponibili per il database '%2$s'."

$ Msg 11 should not translate "@."
11 "\n\
%1$s L'operatore Collocazione '%2$.*s' non è valido.\n\
Il formato corretto è '@<N>' dove <N> è maggiore di zero."

$ Msg 12 should not translate "@."
12 "\n\
%1$s L'operatore Collocazione (@) non può\n\
essere posizionato tra parole identiche."

13 "%1$s Parola '%2$.*s' non è valida."

14 "%1$s Stringa '%2$.*s' non è una parola singola."

15 "\n\
%1$s La richiesta non restituirà nessun record.\n\
Riformulare la richiesta e riprovare."

16 "\n\
%1$s La richiesta restituirà il database intero.\n\
'%2$s'.  Riformulare la richiesta e riprovare."


$ ****************************************************
$ set 29 symbolically named MS_tomita, used in tomita.c (dtsrdelete)
$set 29

1 "\n\
\n%1$s Interruzione ricevuta %2$d.\n\
  Il programma verrà terminato dopo le cancellazioni in corso.\n"

3 "%1$s: Codice di uscita = %2$d.\n"
4 "%1$s Database '%2$s' non è stato trovato.\n"
5 "Le scelte disponibili sono:"
8 "%1$s Formato di input non valido: %2$.30s...\n"
$ Msg 24 is like <Program name> Database <nome_db>, <record id> not found."
24 "%1$s Database %2$s, '%3$s' non è stato trovato.\n"

25 "\n\
NON CONTINUARE nei casi seguenti:\n\
-> Se il file di input che contiene le id dei record da cancellare non è\n\
      denominato '%1$s'.\n\
-> Se un utente sta accedendo ai database rilevanti.\n\
-> Se non esiste una versione di backup di ciascun file database.\n\n\
Se si è sicuri di voler cancellare, inserire 'y'... "

$ Msg 26 is "<program name> <number of files> read, <number> deleted,
$ <time> elapsed. Database <nome_db>: Current record count = <number>,
$ Batch size = <number of records>."
26 "%1$s %2$ld letti, %3$ld cancellati, tempo trascorso: %4$ldh %5$2ldm %6$2lds.\n\
  Database '%7$s': Numero attuale di record = %8$ld, Dimensione batch = %9$d.\n"
$ Msg 27 is "<program name> <number of records> records read from input file.
$ <number> were deleted and <number> were not found in <time>." 
27 "%1$s %2$ld record letti dal file di input.  %3$ld sono stati cancellati e\n\
  %4$ld non sono stati trovati dopo %5$ld ore, %6$ld minuti, %7$ld secondi,\n"

28 "  un tempo medio di %1$ld minuti, %2$ld secondi per ogni record cancellato.\n"
$ Msg 29 is "<program name>. Run <time stamp>."
29 "%1$s.  Eseguito %2$s.\n"

$ Msg 30 should not translate "-y".
30 "\n\
SINTASSI: %1$s [opzioni]\n\
  -i    Nome file di input.  Se non specificato, %2$s.\n\
  -d[v] Stampa le informazioni debug.\n\
        -dv attiva il debugging 'verbose' (record per record).\n\
  -t<N> Numero massimo di secondi di runtime desiderato.\n\
        Ctrl-C/Break interrompe la cancellazione al record successivo.\n\
  -n<N> Cambia il numero di record di un batch da %5$d in <N>.\n\
  -y    Risponde 'y' automaticamente alle richieste di conferma nel modo Cancellazione.\n\
  -d    Segue le operazioni di cancellazione.\n"

31 "%1$s Argomento sconosciuto '%2$s'.\n"
32 "%1$s Impossibile aprire il file di input '%2$s'.\n"
33 "\n%1$s Impossibile aprire il file di output '%2$s'.\n"
34 "Inizializzazione del motore di ricerca...\n"
36 "Chiusura normale del motore di ricerca.\n"


$ ****************************************************
$ set 30 named MS_huff has messages for hencode.c, hdecode.c, huffcode.c
$set 30

$ Msg 1 variable 2 is <filename>, variable 3 is <system errno>.
1 "%1$s: Impossibile aprire il file codificato huffman '%2$s':\n\
  '%3$s' Codice di uscita = 2.\n"
2 "\n\
SINTASSI: %1$s <nome_file>\n"
3 "%1$s Impossibile aprire il file di input '%2$s.\n"
4 "%1$s Numero totale di caratteri = %2$ld.\n"

10 "%1$s Memoria esaurita.\n"
$ Msg 11 variable 2 is <tree file name>, variable 3 is <system errno>.
11 "%1$s Impossibile aprire il file di struttura '%2$s': %3$s.\n"
$ Msg 12 variable 2 is <tree file name>.  Do not translate "hctree."
12 "%1$s Formato hctree non valido '%2$s'.\n"
$ Msg 13 Fshould not translate "hctree_ids."
13 "%1$s Incoerenza in hctree_ids.\n"
14 "%1$s Impossibile aprire cypherfile '%2$s'.\n"
$ Msg 15 should not translate "EOF."
15 "%1$s EOF non previsto '%2$s'.\n"

30 "183 Stringhe di bit troppo grandi.  È probablile che le variabili letterali siano disattivate\n\
e che il numero di caratteri sia incorretto.\n"
31 "191 Errore di programma: tentativo di costruire\n\
la struttura Huffman Code Tree dopo la creazione del root.\n"
32 "(carattere non ascii, bit impostato)"
33 "366 Formato file non valido per %1$s.\n"
34 "424 Impossibile aprire il file '%1$s' per la scrittura.  Verificare che non sia di sola lettura.\n"
35 "\n\
SINTASSI: huffcode [-l<N> | -l-] [-o] <nome_huff> [<file_input>]\n\
  -l<N>         Specifica il numero di soglia dei variabili letterali. Un carattere\n\
                che compare <= <N> volte verrà codificato con la variabile letterale.\n\
                Huffman. Valore predefinito = -10, codifica letterale solo per i byte di zero.\n\
  -l-           Disattiva la codifica letterale. Questa azione nelle strutture 'sbilanciate' può \n\
                causare codici di stringhe di bit LUNGHISSIMI. Se non si è sicuri che il file di\n\
                sia un file binario 'bilanciato', non usare questa opzione.\n\
  -o            Autorizza la sovrascrittura dei file di decodifica esistenti.\n\
  <nome_huff>   È il prefisso dei file di codice Huffman.\n\
                Se il file di codifica (%1$s) esiste già, il conteggio byte dal file di input\n\
                verrà accodato, altrimenti verrà creato un nuovo file.\n\
                Il file di decodifica (%2$s) è sempre creato all'esecuzione.\n\
  <file_input>  è un file di input contenente i byte da contare.\n\
                Può essere ommesso se il file di codifica esiste già .\n"
36 "'%1$s' non è un argomento valido.\n"
37 "576 Manca il prefisso di codice Huffman.\n"
$ Msg 38 needs to have "y" translated to match msg 39"
38 "Il file di decodifica '%s' esiste già.  Sovrascrivere il file? [s/n]\n"
$ Msg 39 needs "y" to match the "yes" option in msg 38.
39 "s"
40 "HUFFCODE Versione %1$s\n"
41 "Le tabelle Huffman verranno create.\n"
42 "Tabella '%1$s' contiene già %2$ld Kbyte dalle esecuzioni precedenti.\n"
43 "645 Il file di input non è stato specificato e il file di tabella '%s'\n\
non esiste. Impossibile proseguire.\n"
44 "Impossibile aprire o accedere allo stato del file di input '%1$s': %2$s\n"
45 "Il file di input '%1$s' contiene circa %2$ld Kbyte.\n"
46 "\n%1$ld%% completati. Letti %2$2ld Kbyte.  Tempo stimato per completare l'operazione: %3$3ld secondi.\n"
$ Msg 47 variable 2 is a time stamp, variable 2 is from either msg 48 or 49,
$ variables3 and 4 are filenames.
47 "L'indicatore orario sarà '%1$ld'.\n\
%2$s Tabelle di codice Huffman in '%3$s' e '%4$s'...\n"
$ Msg 48 is used in msg 47 as variable 2.
48 "Creazione"
$ Msg 49 is used in msg 47 as variable 2.
49 "Ricostruzione"

$ ********************** DTSEARCH.MSG ************************
