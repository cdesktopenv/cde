
$ "$TOG: dtfile.msg /main/10 1999/09/16 09:44:40 mgreess $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtfile.msg
$ **
$ **  Project:     Common Desktop Environment dtfile
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtfile
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990, 1991, 1992 Hewlett-Packard Company
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
$ 03/25/93 H. Amro            removed the guilty $ from message 30
$ 
$ 
$ 
$ *****************************************************************************

$set 2
$ ****** Module: ChangeDir.c ****(developer comment only)********

$ Messages 3 and 10 are titles for the Change Folder dialog, Message 3 is
$   used when the user has specified a title for dtfile windows
$ Messages 2 and 4 are labels for the Change Folder dialog
$ Message 1 is the title for Change Folder errors
$ Messages 7, 8, 9, 11, and 12 are Change Folder error messages

1 Errore di cambiamento della cartella
2 Nuova cartella:
3 Cambiamento della cartella
4 Host:
7 Il nome della cartella seguente non è valido.\n\n%s:%s
8 Impossibile passare alla cartella specificata. Si possono\n\
visualizzare solo le cartelle che si trovano al di sotto \n\
della cartella corrente. Non è possibile specificare un \n\
percorso completo per la nuova cartella.
9 Questa vista è in modalità limitata. Non è possibile\n\
passare alla cartella specificata poiché non si trova \n\
nel percorso relativo della cartella limitata.\n
10 Gestione di file - Cambiamento della cartella
11 Impossibile identificare il nome della nuova cartella.
12 Inserire il nome della cartella.

13 Errore di Andare a 
14 Cartella di destinazione:
15 Andare a
16 Nome del sistema:
17 Gestione di file - Andare a 
18 Il nome della cartella di destinazione non è stato trovato.\n\
   Digitare il nome di una cartella o selezionarne una dall'elenco.

$quote "
19 "Il seguente nome cartella non è valido.\n\n\
%s"

20 "Non è possibile passare alla cartella\n\n\
%s\n\n\
Non è consentito visualizzare questa \n\
cartella poiché è una cartella con\n\
autorizzazioni di accesso limitato."

$set 3
$ ****** Module: ChangeDirP.c ****(developer comment only)********

$ Messages 2-6 are labels for the folder Status Line
$ Message 3 is required for SUN systems
$ Message 2-9 are obsolete

$quote
2 %d File %d Nascosti
3 %1$d File %2$d Nascosti
4 %3d File...
5 Lettura in corso...
6 %d Nascosti

7 %d Elementi %d Nascosti
8 %1$d Elementi %2$d Nascosti
9 %3d Elementi ...



10 %d Elemento(i)
11 %d Elemento(i)... 
12 %d Elementi %d Nascosto/i



$set 4
$set 5
$set 6
$set 7
$set 8



$set 9
$ ****** Module: File.c ****(developer comment only)********

$ Messages 6 and 7 are the title and text for an action error message
$ Messages 8 and 9 are rename error messages
$ Messages 11-19 are the title and text for warning messages displayed
$   when an object/set of objects is moved/copied/linked to a folder
$   containing object(s) with the same name

6 Errore dell'azione 
7 Nessuna azione definita per %s\n

8 Nello Spazio di lavoro è già presente \n\
un file con il nuovo nome specificato.\n\
Scegliere un nome diverso.
9 Esiste già un file con questo nome.\n\
Scegliere un nome diverso.

11 Gestione di file - Avvertenza di Spostamento/Copia/Collegamento
$ Messages 12-15 deal with multiple files/folders
12 I seguenti file (o cartelle)\n\
esistono già in\n\
%s:
13 \nL'operazione di copia sovrascriverà questi file.\n\
          Procedere ugualmente?\n
14 \nL'operazione di spostamento sovrascriverà questi file.\n\
          Procedere ugualmente?\n
15 \nL'operazione di collegamento sovrascriverà questi file.\n\
          Procedere ugualmente?\n

$ Messages 16&17 deal with a single file/folder
16 Esiste già un oggetto di nome:\n%s\nall'interno della cartella:\n%s
17 Esiste già una cartella di nome:\n%s\nall'interno della cartella:\n%s

$ Labels in rename/replace and merge/replace dialogs
20 Scegliere l'operazione desiderata
21 Sostituire l'oggetto esistente
22 Cambiare nome all'oggetto in:
24 Unire il contenuto delle due cartelle
25 Sostituire la cartella esistente

27 Ok
28 Annullare
29 Aiuto
30 Annullare lo spostamento
31 Annullare la copia
32 Annullare il collegamento

$ Window title for overwrite dialogs
33 Gestione di file - Avvertenza di spostamento/copia/collegamento
34 Gestione di file - Avvertenza di spostamento 
35 Gestione di file - Avvertenza di copia 
36 Gestione di file - Avvertenza di collegamento

37 Cambiamento del nome dell'oggetto - Errore
38 %s\nesiste già: scegliere un altro nome.
39 %s\nnon può essere rinominato in\n%s\n%s
40 Sostituzione dell'oggetto - Errore

51 copiare
52 spostare 
53 collegare 
54 modificare 
56 Sostituire gli oggetti esistenti
57 Cambiare nome agli oggetti esistenti aggiungendo un numero al nome
58 copiato/a/i
59 spostato/a/i
60 collegato/a/i
61 modificato/a/i
68 1 oggetto %s.
69 %d oggetti %s.
70 %s\n%d oggetti hanno lo stesso nome degli oggetti nella cartella\n%s
71 %s\n1 oggetto ha lo stesso nome di un oggetto nella cartella\n%s
72 Copia
73 Spostamento
74 Collegamento
75 Operazione
76 copia
77 spostamento
78 collegamento
79 operazione

$ Messages with variables - we are forced to use /a/i/o etc.

80 %s annullato/a - La cartella non è stata %s.\n\n\
Non è stato possibile cancellare la cartella\n\
a causa di un oggetto contenuto al suo interno.\n\
È possibile che altri oggetti all'interno della cartella\n\
siano stati cancellati prima dell'interruzione del processo di %s.


81 %s\n\n\
La causa più probabile di questo problema è la mancanza\n\
di autorizzazioni in scrittura per l'oggetto che non\n\
è stato possibile cancellare.\n\
Modificare le autorizzazioni in scrittura o cambiare\n\
il nome manualmente al file da %s.


82 %s annullato/a\n\n\
%s\n\
non è stato cancellato.\n\n\
La causa più probabile di questo problema\n\
è la mancanza di autorizzazioni in scrittura per il file.\n\
Modificare la autorizzazioni in scrittura o cambiare il nome\n\
manualmente al file da %s.


83 %s annullato/a - Nessuno degli oggetti è stato %s.\n\n\
Non è stato possibile cambiare il nome automaticamente\n\
ai seguenti oggetti:\n\
%s\n\n\
È stato cambiato il nome di %s oggetti prima che il \n\
processo di %s venisse interrotto.


84 %s\n\n\
La causa più probabile di questo problema è la mancanza di\n\
autorizzazioni in scrittura per l'oggetto indicato. \n\
Modificare le autorizzazioni in scrittura o rinominare\n\
manualmente l'oggetto da %s.


85 %s annullato/a - Nessuno degli oggetti è stato %s.\n\n\
Non è stato possibile rinominare automaticamente i seguenti oggetti\n\
%s\n\n\
1 oggetto è stato rinominato prima che il processo di %s venisse interrotto.


86 %s annullato/a - Nessuno degli oggetti è stato %s.\n\n\
Non è stato possibile cancellare una cartella.\n\
   Cartella: %s\n\n\
È possibile che alcuni oggetti di questa cartella siano stati\n\
cancellati prima che il processo di %s venisse interrotto.\n\n\
%d altri oggetti sono stati cancellati.\n\n\
La causa più probabile di questo problema è la mancanza di\n\
autorizzazioni in scrittura per la cartella indicata.\n\
Modificare le autorizzazioni in scrittura o rinominare\n\
manualmente la cartella da %s.


87 %s annullato/a - Nessuno degli oggetti è stato %s.\n\n\
Non è stato possibile cancellare una cartella.\n\
   Cartella: %s\n\n\
È possibile che alcuni oggetti di questa cartella siano stati\n\
cancellati prima che il processo di %s venisse interrotto.\n\n\
1 altro oggetto è stato cancellato.\n\n\
La causa più probabile di questo problema è la mancanza di \n\
autorizzazioni in scrittura per la cartella indicata. \n\
Modificare le autorizzazioni in scrittura o rinominare\n\
manualmente la cartella da %s.


88 %s annullato/a - nessuno degli oggetti è stato %s.\n\n\
Non è stato possibile cancellare un file.\n\
    File: %s\n\n\
    %d altri oggetti sono stati cancellati.\n\n\
La causa più probabile di questo problema è la mancanza di \n\
autorizzazioni in scrittura per il file indicato. \n\
Modificare le autorizzazioni in scrittura o rinominare\n\
manualmente il file da %s.


89 %s annullato/a - Nessuno degli oggetti è stato %s.\n\n\
Non è stato possibile cancellare un file.\n\
    File: %s\n\n\
1 altro oggetto è stato cancellato.\n\n\
La causa più probabile di questo problema è la mancanza di\n\
autorizzazioni in scrittura per il file indicato. \n\
Modificare le autorizzazioni in scrittura o rinominare\n\
manualmente il file da %s.


90 Conflitto di nomi.\n\
  Lo spazio di lavoro in cui compare questo oggetto\n\
  contiene già un altro oggetto con il nome specificato.\n\
  Scegliere un nome diverso.\n\


91 Interrompere la copia
92 Interrompere il collegamento 
93 Interrompere lo spostamento
94 Errore

$quote "
95 "Spostare annullato - la cartella non è stata spostata.\n\n\
Non è stato possibile cancellare la cartella che si \n\
desidera sostituire poiché contiene un oggetto che \n\
non può essere cancellato. E' possibile che altri \n\
oggetti contenuti nella cartella siano già stati \n\
cancellati prima che il processo di spostamento \n\
fosse arrestato.\n\n\
E' molto probabile che non si abbia l'autorizzazione\n\
in scrittura per l'oggetto che non può essere eliminato.\n\
Modificare l'autorizzazione in scrittura o ridenominare \n\
la cartella che si desidera spostare."

96 "Copiare annullato - la cartella non è stata copiata.\n\n\
Non è stato possibile cancellare la cartella che si\n\
desidera sostituire poiché contiene un oggetto che\n\
non può essere cancellato. E' possibile che altri \n\
oggetti contenuti nella cartella siano già stati\n\
cancellati prima che il processo di copia fosse \n\
arrestato.\n\n\
E' molto probabile che non si abbia l'autorizzazione in\n\
scrittura per l'oggetto che non si può eliminare.\n\
Modificare l'autorizzazione in scrittura o ridenominare\n\
la cartella di cui si desidera effettuare la copia."

97 "Collegare annullato - la cartella non è stata collegata.\n\n\
Non è stato possibile cancellare la cartella che si \n\
desidera sostituire poiché contiene un oggetto che \n\
non si può eliminare. E' possibile che altri oggetti\n\
contenuti nella cartella siano già stati cancellati\n\
prima che il processo di collegamento fosse\n\
arrestato.\n\n\
E' molto probabile che non si abbia l'autorizzazione in \n\
scrittura per l'oggetto che non può essere eliminato.\n\
Modificare l'autorizzazione in scrittura o ridenominare \n\
la cartella che si desidera collegare."

98 "Operazione annullata - la cartella non è stata modificata.\n\n\
Non è stato possibile cancellare la cartella che si \n\
desidera sostituire poiché contiene un oggetto che non \n\
può essere cancellato. E' possibile che altri oggetti \n\
contenuti nella cartella siano già stati cancellati \n\
prima che il processo fosse arrestato.\n\n\
E' molto probabile che non si abbia l'autorizzazione \n\
in scrittura per l'oggetto che non può essere eliminato.\n\
Modificare l'autorizzazione in scrittura o ridenominare\n\
la cartella che si desidera modificare."

99 "Spostamento annullato\n\n\
%s\n\
non può essere eliminato.\n\n\
E' molto probabile che non si abbia \n\
l'autorizzazione in scrittura per questo\n\
file. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare il file che si sta spostando."

100 "Copiare annullato\n\n\
%s\n\
non può essere eliminato.\n\n\
E' molto probabile che non si abbia \n\
l'autorizzazione in scrittura per questo file.\n\
Modificare l'autorizzazione in scrittura\n\
oppure ridenominare il file che si sta copiando."

101 "Collegare annullato\n\n\
%s\n\
non può essere eliminato.\n\n\
E' molto probabile che non si abbia \n\
l'autorizzazione in scrittura per questo file.\n\
Modificare l'autorizzazione in scrittura oppure\n\
ridenominare il file che si sta collegando."

102 "Operazione annullata\n\n\
%s\n\
non può essere eliminato.\n\n\
E' molto probabile che non si abbia \n\
l'autorizzazione in scrittura per questo file.\n\
Modificare l'autorizzazione in scrittura o \n\
ridenominare il file che si sta modificando."

103 "Spostare annullato - Non è stato spostato alcun oggetto.\n\n\
Non è stato possibile ridenominare automaticamente il\n\
seguente oggetto\n\
%s\n\n\
Sono stati ridenominati %d oggetti prima che fosse arrestato \n\
il processo di spostamento.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura \n\
per l'oggetto indicato. Modificare l'autorizzazione in scrittura \n\
o ridenominare l'oggetto che si sta spostando."

104 "Copiare annullato - Non è stato copiato alcun oggetto.\n\n\
Non è stato possibile ridenominare automaticamente il \n\
seguente oggetto\n\
%s\n\n\
Sono stati ridenominati %d oggetti prima che fosse arrestato \n\
il processo di copia.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittua\n\
per l'oggetto indicato. Modificare l'autorizzazione in scrittura \n\
o ridenominare l'oggetto che si sta copiando."

105 "Collegare annullato - Non è stato collegato alcun oggetto.\n\n\
Non è stato possibile ridenominare automaticamente il \n\
seguente oggetto\n\
%s\n\n\
Sono stati ridenominati %d oggetti prima che fosse arrestato \n\
il processo di collegamento.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per l'oggetto indicato. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare l'oggetto che si sta collegando."

106 "Operazione annullata - Non è stato modificato alcun oggetto.\n\n\
Non è stato possibile ridenominare automaticamente il \n\
seguente oggetto\n\
%s\n\n\
Sono stati ridenominati automaticamente %d oggetti prima che \n\
fosse arrestato il processo.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura \n\
per l'oggetto indicato. Modificare l'autorizzazione in scrittura \n\
oppure ridenominare l'oggetto che si sta modificando."

107 "Spostare annullato - Non è stato spostato alcun oggetto.\n\n\
Non è stato possibile ridenominare automaticamente il \n\
seguente oggetto\n\
%s\n\n\
E' stato ridenominato 1 oggetto prima che fosse arrestato\n\
il processo.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura \n\
per l'oggetto indicato. Modificare l'autorizzazione in scrittura\n\
o ridenominare l'oggetto che si sta spostando."

108 "Copiare annullato - Non è stato copiato alcun oggetto.\n\n\
Non è stato possibile ridenominare automaticamente il \n\
seguente oggetto\n\
%s\n\n\
E' stato ridenominato 1 oggetto prima che fosse arrestato il \n\
processo di copia.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura \n\
per l'oggetto indicato. Modificare l'autorizzazione in scrittura \n\
oppure ridenominare l'oggetto che si sta copiando."

109 "Collegare annullato - Non è stato collegato alcun oggetto.\n\n\
Non è stato possibile ridenominare automaticamente il\n\
seguente oggetto\n\
%s\n\n\
E' stato ridenominato 1 oggetto prima che fosse arrestato il \n\
processo di collegamento.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura \n\
per l'oggetto indicato. Modificare l'autorizzazione in scrittura \n\
oppure ridenominare l'oggetto che si sta collegando."

110 "Operazione annullata - Non è stato modificato alcun oggetto.\n\n\
Non è stato possibile ridenominare automaticamente il\n\
seguente oggetto\n\
%s\n\n\
E' stato ridenominato 1 oggetto prima che fosse arrestato\n\
il processo.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per l'oggetto indicato. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare l'oggetto che si sta modificando."

111 "Spostare annullato - Non è stato spostato alcun oggetto.\n\n\
Non è stato possibile cancellare la cartella che si desidera\n\
sostituire\n\
   Cartella: %s\n\n\
E' possibile che siano stati cancellati alcuni oggetti contenuti\n\
in questa cartella prima che fosse arrestato il processo.\n\n\
Sono stati cancellati altri %d oggetti.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per la cartella indicata. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare la cartella che si sta spostando."

112 "Copiare annullato - Non è stato copiato alcun oggetto.\n\n\
Non è stato possibile cancellare la cartella che si desidera \n\
sostituire\n\
   Cartella: %s\n\n\
E' possibile che siano stati cancellati alcuni oggetti contenuti \n\
in questa cartella prima che fosse arrestato il processo di copia.\n\n\
Sono stati cancellati altri %d oggetti.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per la cartella indicata. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare la cartella che si sta copiando."

113 "Collegare annullato - Non è stato collegato alcun oggetto.\n\n\
Non è stato possibile cancellare la cartella che si desidera \n\
sostituire\n\
   Cartella: %s\n\n\
E' possibile che siano stati cancellati alcuni oggetti contenuti \n\
in questa cartella prima che fosse arrestato il processo di \n\
collegamento.\n\n\
Sono stati cancellati altri %d oggetti.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per la cartella indicata. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare la cartella che si sta collegando."

114 "Operazione annullata - Non è stato modificato alcun oggetto.\n\n\
Non è stato possibile cancellare la cartella che si desidera\n\
sostituire\n\
   Cartella: %s\n\n\
E' possibile che siano stati cancellati alcuni oggetti contenuti \n\
in questa cartella prima che fosse arrestato il processo.\n\n\
Sono stati cancellati altri %d oggetti.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura \n\
per la cartella indicata. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare la cartella che si sta modificando."

115 "Spostare annullato - Non è stato spostato alcun oggetto.\n\n\
Non è stato possibile cancellare la cartella che si desidera\n\
sostituire\n\
   Cartella: %s\n\n\
E' possibile che siano stati cancellati alcuni oggetti contenuti \n\
in questa cartella prima che fosse arrestato il processo di \n\
spostamento.\n\n\
E' stato cancellato 1 altro oggetto.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per la cartella indicata. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare la cartella che si sta spostando."

116 "Copiare annullato - Non è stato copiato alcun oggetto.\n\n\
Non è stato possibile cancellare la cartella che si desidera \n\
sostituire\n\
   Cartella: %s\n\n\
E' possibile che siano stati cancellati alcuni oggetti contenuti \n\
in questa cartella prima che fosse arrestato il processo di copia.\n\n\
E' stato cancellato 1 altro oggetto.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per la cartella indicata. Modificare l'autorizzazione in scrittura \n\
oppure ridenominare la cartella che si sta copiando."

117 "Collegare annullato - Non è stato collegato alcun oggetto.\n\n\
Non è stato possibile cancellare la cartella che\n\
si desidera sostituire\n\
   Cartella: %s\n\n\
E' possibile che siano stati cancellati alcuni oggetti\n\
contenuti in questa cartella prima che fosse arrestato\n\
il processo di collegamento.\n\n\
E' stato cancellato 1 altro oggetto.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura \n\
per la cartella indicata. Modificare l'autorizzazione in scrittura \n\
oppure ridenominare la cartella che si sta collegando."

118 "Operazione annullata - Non è stato modificato alcun oggetto.\n\n\
Non è stato possibile cancellare la cartella che \n\
si desidera sostituire\n\
   Cartella: %s\n\n\
E' possibile che siano stati cancellati alcuni oggetti\n\
contenuti in questa cartella prima che fosse arrestato\n\
il processo.\n\n\
E' stato cancellato 1 altro oggetto.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per la cartella indicata. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare la cartella che si sta modificando."

119 "Spostare annullato - Non è stato spostato alcun oggetto.\n\n\
Non è stato possibile cancellare il file che si desidera\n\
sostituire\n\
    File: %s\n\n\
Sono stati cancellati %d altri oggetti.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per il file indicato. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare il file che si sta spostando."

120 "Copiare annullato - Non è stato copiato alcun oggetto.\n\n\
Non è stato possibile cancellare il file che si desidera\n\
sostituire.\n\
    File: %s\n\n\
Sono stati cancellati %d altri oggetti.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per il file indicato. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare il file che si sta copiando."

121 "Collegare annullato - Non è stato possibile collegare alcun oggetto.\n\n\
Non è stato possibile cancellare il file che si desidera\n\
sostituire.\n\
    File: %s\n\n\
Sono stati cancellati %d altri oggetti.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per il file indicato. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare il file che si sta collegando."

122 "Operazione annullata - Non è stato modificato alcun oggetto.\n\n\
Non è stato possibile cancellare il file che si desidera\n\
sostituire.\n\
    File: %s\n\n\
Sono stati cancellati %d altri oggetti.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per il file indicato. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare il file che si sta modificando."

123 "Spostare annullato - Non è stato possibile spostare alcun oggetto.\n\n\
Non è stato possibile cancellare il file che si desidera\n\
sostituire.\n\
    File: %s\n\n\
E' stato cancellato 1 altro oggetto.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per il file indicato. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare il file che si sta spostando."

124 "Copiare annullato - Non è stato copiato alcun oggetto.\n\n\
Non è stato possibile cancellare il file che si desidera\n\
sostituire.\n\
    File: %s\n\n\
E' stato cancellato 1 altro oggetto.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per il file indicato. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare il file che si sta copiando."

125 "Collegare annullato - Non è stato collegato alcun oggetto.\n\n\
Non è stato possibile cancellare il file che si desidera\n\
sostituire.\n\
    File: %s\n\n\
E' stato cancellato 1 altro oggetto.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per il file indicato. Modificare l'autorizzazione in scrittura\n\
oppure ridenominare il file che si sta collegando."

126 "Operazione annullata - Non è stato modificato alcun oggetto.\n\n\
Non è stato possibile cancellare il file che si desidera \n\
sostituire\n\
    File: %s\n\n\
E' stato cancellato 1 altro oggetto.\n\n\
E' molto probabile che non si abbia l'autorizzazione in scrittura\n\
per il file indicato. Modificare l'autorizzazione in scrittura \n\
oppure ridenominare il file che si sta modificando."

127 "1 oggetto spostato."

128 "%d oggetti spostati."

129 "1 oggetto copiato."

130 "%d oggetti copiati."

131 "1 oggetto collegato."

132 "%d oggetti collegati."

133 "1 oggetto modificato."

134 "%d oggetti modificati."

135 "Impossibile spostare l'oggetto %s su se stesso."

136 "Impossibile copiare l'oggetto %s su se stesso."

137 "Impossibile collegare l'oggetto %s su se stesso."

138 "Non si è stato possibile ridenominare il seguente\n\
oggetto/i seguenti oggetti\n\n\
%s\n\n\
E' molto probabile che non si abbia l'autorizzazione\n\
per ridenominare tale oggetto/tali oggetti."

139 "Non è stato possibile sostituire il seguente\n\
oggetto/i seguenti oggetti\n\n\
%s\n\n\
E' molto probabile che non si abbia l'autorizzazione\n\
per sostituire tale oggetto/tali oggetti."

140 "Rinominare il nuovo oggetto:"


141 "Aggiunta di numero per rinominare i nuovi oggetti"
$set 10
$ ****** Module: FileDialog.c ****(developer comment only)********

$ Messages 1, 2, 20, and 21 are titles for error dialogs
$ Message 3 is an error message for dialogs requiring new file names
$ Messages 5 and 27 are titles for the Copy File dialog, Message 5 is used
$   when the user has specified a title for dtfile windows
$ Messages 6-9 are labels for the File Copy, New Dir, and New File dialogs
$ Messages 10-13 and 25 are error messages for the file copy operation
$ Messages 14 and 28 are titles for the New Folder dialog, Message 14 is
$   used when the user has specified a title for dtfile windows
$ Message 16 is a label for the New Folder dialog
$ Messages 22-24 are error messages for the New File/Folder operations
$ Messages 26 and 29 are titles for the New File dialog, Message 26 is used
$   when the user has specified a title for dtfile windows
$ Messages 30 and 31 are titles for the Move File dialog, Message 30 is used
$   when the user has specified a title for dtfile windows
$ Messages 32 and 33 are titles for the Link File dialog, Message 32 is used
$   when the user has specified a title for dtfile windows

$quote
1 Errore di cambiamento del nome 
2 Errore di copia del file

3 Non è possibile inserire spazi nel nome di un file o di una cartella.

5 Copia di file

6 Nome corrente del file:
7 Nome del nuovo file:
8 Tipo di file
9 Mostrare tipo di file

10 Il file selezionato originariamente non esiste più.
11 Il nome del file o della cartella deve contenere almeno un carattere.
13 Nome del file o della cartella non valido.

14 Nuova cartella

16 Nome della nuova cartella:

20 Errore di creazione della cartella
21 Errore di creazione del file

22 Impossibile creare un file o una cartella con un nome vuoto.
23 Nome della cartella non valido.
24 Nome del file non valido.

25 Non è possibile usare il comando di copia per copiare\n\
un file sullo spazio di lavoro. È necessario usare il comando\n\
`Collocare nello spazio di lavoro` del menu File, o\n\
rilasciare il file sullo spazio di lavoro.

26 Nuovo file

27 Gestione di file - Copia di file
28 Gestione di file - Nuova cartella
29 Gestione di file - Nuovo file

30 Spostamento di file
31 Gestione di file - Spostamento di file

32 Collegamento di file
33 Gestione di file - Collegamento di file
34 Cartella di destinazione: 
35 Nome dell'oggetto:
36 Mostrare icona

37 Oggetto selezionato: 
38 Nome per la copia:

39 Errore di cambiamento del nome
40 Questo oggetto è un'Azione.\n\
   Non è possibile cambiare il nome direttamente alle icone delle azioni.
41 È già visualizzata l'icona corretta per questo tipo di dati.

$quote "
42 "Spostare Oggetto"

43 "Gestore file - Spostare Oggetto"

44 "Copiare Oggetto"

45 "Gestore file - Copiare Oggetto"

46 "Collegare Oggetto"

47 "Gestore file - Collegare Oggetto"

48 "Nome nuovo oggetto:"


$set 11
$ ****** Module: FileManip.c ****(developer comment only)********

$ Message 6 is the title for an error dialog
$ Messages 9, 14, 16, 28, and 30-36 are file manipulation error messages

$quote
6 Errore di manipolazione del file

9 Non è possibile rilasciare i file dal\n\
Cestino sullo spazio di lavoro.

14 Impossibile individuare la posizione delle cartelle.
16 Impossibile spostare una cartella su se stessa.\n%s
28 Impossibile trovare %s.
30 Impossibile spostare o cambiare il nome alla cartella %s.\n\
Per poter cancellare, spostare o cambiare nome a una cartella \n\
è necessario chiudere tutte le viste della Gestione di file \n\
relative alla cartella o alle sue cartelle secondarie.
31 Errore in lettura su %s
32 Impossibile cambiare il nome a %s
33 Impossibile aprire %s
34 L'utente non possiede le autorizzazioni necessarie per spostare %s\n\
È tuttavia possibile copiare il file \n\
(usare <CTRL> durante il trascinamento).
35 Impossibile copiare una cartella su se stessa.  
36 Impossibile creare il processo figlio.

37 Non è possibile rilasciare sullo spazio di \n\
lavoro i file provenienti dal Cestino.\n\
Per eliminare un oggetto dal Cestino,\n\
   -Selezionare l'oggetto\n\
   -Scegliere 'Riportare indietro'\n\
dal menu File o dal menu a scomparsa dell'oggetto.\n\
A questo punto sarà possibile trascinare l'oggetto dalla \n\
Gestione di file e rilasciarlo sullo spazio di lavoro.

38 L'utente non possiede le autorizzazioni necessarie per spostare %s\n\
È tuttavia possibile copiare l'oggetto.\n\
Per copiare un oggetto:\n\
  - premere il tasto <Ctrl> e, tenendolo premuto,\n\
  - trascinare l'oggetto con il mouse.\n\
Oppure\n\
  - usare l'opzione 'Copiare in' del menu a scomparsa \n\
    'Selezioni' della barra dei menu.

39 Impossibile creare il processo figlio.\n\
   È stato raggiunto il numero massimo di processi per questo sistema.\n\
   Interrompere alcuni processi o programmi in esecuzione \n\
   e riprovare.

40 Gli oggetti di origine e di destinazione sono uguali.\n%s
41 La cartella\n%s\nnon esiste.
42 Spazio esaurito sull'unità\n\
   di destinazione della copia.
43 Spazio esaurito sull'unità di\n\
   destinazione dello spostamento.
44 Non è stata effettuata alcuna operazione di spostamento sull'oggetto %s.\n\
   Non è possibile spostare un oggetto nella cartella in cui si\n\
   trova già. È necessario indicare una cartella di destinazione\n\
   differente per poter effettuare lo spostamento.
45 Non è stata effettuata alcuna operazione di copia sull'oggetto %s.\n\
   È necessario modificare l'impostazione della cartella di destinazione \n\
   o quella del 'nome per la copia' per poter effettuare l'operazione.
46 Non è stata effettuata alcuna operazione di collegamento sull'oggetto %s.\n\
   È necessario modificare l'impostazione della cartella di destinazione\n\
   o quella del 'nome per la copia' per poter effettuare l'operazione.

47 Non è possibile copiare una cartella su se stessa.  

$quote "
48 "Impossibile copiare nella cartella \"%s\"\n\
i seguenti oggetti\n\n\
%s\n\n\
E' molto probabile che non si abbia l'autorizzazione\n\
in scrittura per la cartella di destinazione."

49 "Impossibile spostare nella cartella \"%s\"\n\
i seguenti oggetti\n\n\
%s\n\n\
E' molto probabile che non si abbia l'autorizzazione\n\
in scrittura per la cartella di destinazione."

50 "Impossibile spostare nella cartella \"%s\"\n\
i seguenti oggetti\n\n\
%s\n\n\
E' molto probabile che non si abbia l'autorizzazione\n\
per spostare l'oggetto di origine."
$   
$ Messages 51 - 56 are new since CDE/SI.     
$   

51 "Impossibile copiare il file/cartella poiché\n\
verrà oltrepassata la quota del disco su cui\n\
si sta effettuando la copia."

52 "Impossibile spostare il file/cartella poiché\n\
verrà oltrepassata la quota del disco su cui\n\
si sta effettuando lo spostamento."

53 "La copia del file/cartella non è \n\
riuscita a causa di un errore interno.\n\
L'errore è:"

54 "Lo spostamento del file/cartella non è\n\
riuscito a causa di un errore interno.\n\
L'errore è:"

55 "Rivolgersi al Responsabile di sistema"

56 "(Sconosciuto):"

$quote
57 Non si possiede l'autorizzazione per spostare la cartella.\n\
%s\n\

58 Process Create Error
59 Cannot create a child process to delete the dropped files.

$set 12
$ ****** Module: FileMgr.c ****(developer comment only)********

$ Message 3 is a dialog title
$ Messages 4-6 are movement warning or error messages
$ Message 7 is the title for the folder popup menus

3 Spostamento di file 

4 L'oggetto %s si trova attualmente nella cartella %s\n\
e verrà spostato nella cartella %s.\n\
Procedere con lo spostamento?
5 L'oggetto %1$s si trova attualmente nella cartella %2$s\n\
e verrà spostato nella cartella %3$s.\n\
Procedere con lo spostamento?
6 Almeno uno dei file rilasciati proviene da una cartella differente.\n\
Tutti i file verranno spostati in %s.\n\
Procedere con l'operazione?

7 Gestione di file

18 La cartella %s è vuota.
19 La cartella %s non contiene directory secondarie.


$set 13
$ ****** Module: Filter.c ****(developer comment only)********

$ Messages 11 and 21-22 are button labels for the Modify Filter List dialog
$ Messages 19 and 24 are titles for the Link File dialog, Message 19 is used
$   when the user has specified a title for dtfile windows
$ Message 23 is a text field label for the Modify Filter List dialog
$ Message 25 is a label for the Modify Filter List dialog

11 Impostazioni predefinite

19 Impostazione delle opzioni di filtro

21 Selezionare tutto
22 Annullare tutte le selezioni

23 Stringa di filtro (opzionale):

24 Gestione di file - Impostazione delle opzioni di filtro

25 Selezionare i tipi di file da nascondere o visualizzare:

26 Nascondere anche (opzionale):

27 Visualizzati

28 Nascosti

29 Mostrare anche (opzionale):

30 Selezionare i tipi di dati da nascondere:

$quote "
31 "Selezionare il tipo di dati:"

$set 14



$set 15
$ ****** Module: Find.c ****(developer comment only)********

$ Message 1 is the title for an error dialog
$ Messages 14 and 15 are button labels for the Find Files dialog
$ Messages 23, 26, and 27 are Find Files error messages
$ Messages 29 and 41 are titles for the Find Files dialog, Message 29 is used
$   when the user has specified a title for dtfile windows
$ Messages 30-40 and 42 are labels for the Find Files dialog

$quote
1 Errore di ricerca dei file

14 Iniziare
15 Interrompere 

23 Informazioni di ricerca non valide.\n\n\
È necessario specificare la cartella in cui eseguire la ricerca.
26 Informazioni di ricerca non valide.\n\n\
Il file selezionato non esiste più.
27 Nessun file corrisponde ai criteri di ricerca specificati.

29 Ricerca di file o cartelle

30 Compilare uno o più campi per specificare i file da ricercare:
31 Nome del file o della cartella:
32 Contenuto del file:

$ Messages 33-35 are used on hpux systems
33 Sì
34 No
35 Seguire i collegamenti:

36 Aprire nuova vista
37 Collocare nello spazio di lavoro
38 File trovati (per contenuto):
39 File trovati (per nome e contenuto):
40 File trovati (per nome):

41 Gestione di file - Ricerca di file o cartelle

42 Cartella in cui iniziare la ricerca:

43 Errore di ricerca

44 Non è stato indicato l'argomento 'Cartella in cui iniziare la ricerca'.\n\
Digitare il nome della cartella in cui si desidera iniziare la ricerca.

45 Il file selezionato non esiste più.\n\n\
Il file è stato cancellato dopo la fine del processo di ricerca.

46 Ricercare 

47 Compilare uno o più campi per specificare gli elementi da ricercare:

48 Aprire cartella

49 Gestione di file - Ricerca

50 Non sono stati indicati gli argomenti 'Cartella in cui iniziare la ricerca' o 'Contenuto del file'.\n\
Digitare il nome della cartella in cui si desidera iniziare la ricerca e la stringa da ricercare.


$set 16
$set 17



$set 18
$ ****** Module: Main.c ****(developer comment only)********

$ Message 1 is a blank line
$ Messages 2, 6, 17-18 and 27-28 are error messages which will ONLY APPEAR
$   in the DT ERRORLOG FILE
$ above is no longer true for message 2 - it will be displayed.
$ Messages 16 and 32 are titles for the Save Settings dialog, Message 16 is
$   used when the user has specified a title for dtfile windows
$ Messages 19-21 are error messages for invalid directory input.
$ Message 22 is the title for an error dialog
$ Messages 25 and 26 are usage strings
$ Messages 29 and 30 are text for the Save Settings dialog
$ Messages 34 and 35 are drag/drop error messages

$ DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGE 1
1 

$ MESSAGE 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
2 Impossibile creare la cartella .dt o le cartelle secondarie.

$ MESSAGE 6 WILL ONLY APPEAR IN THE DT ERRORLOG FILE, and stderr
6 Errore del Toolkit X... Uscita.\n

16 Salvare come opzioni predefinite

$ MESSSAGES 17-18 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
17 Impossibile creare un file per memorizzare le opzioni predefinite.
18 Impossibile aprire il file della sessione.

19 La cartella specificata,\n\
%s\n\
non è una cartella.
20 La cartella specificata,\n\
%s\n\
non esiste.
21 La cartella specificata non è valida, %s:%s.

22 Errore di trascinamento

23 \nSintassi: %s...\n\n\
   -noview\n\n\
\tdtfile viene eseguito in modo server.\n\n\
   -session FileSessione\n\n\
\tdtfile viene eseguito con il file di sessione specificato nel parametro\n\
\tFileSessione.\n\n\
   -folder Cartella[,Cartella,Cartella]\n\
   -dir Cartella[,Cartella,Cartella]\n\n\
\tdtfile visualizza una finestra per ogni cartella specificata\n\
\tnel parametro Cartella. Il parametro Cartella può contenere più\n\
\tcartelle separate da virgole. Le cartelle devono essere indicate \n\
\tin forma di 'percorso'.\n\n\
   -title Titolo\n\n\
\tdtfile usa la stringa specificata nel parametro Titolo come titolo\n\
\tdelle sue finestre.\n\n\
   -help_volume Volume_di_Aiuto\n\n\
\tdtfile usa il volume di aiuto specificato nel parametro Volume_di_Aiuto.\n\n\
   -restricted\n\n\
\tdtfile non visualizza le cartelle al di sopra della cartella limitata.\n\
\tSe si utilizza l'opzione -dir, la cartella limitata è quella specificata\n\
\tin questa opzione. Se non si utilizza l'opzione -dir, la cartella\n\
\tlimitata è la cartella corrente dell'utente.\n\n\
   -grid on/off\n\n\
\ton     = I file vengono visualizzati in base a una griglia.\n\
\toff    = I file vengono visualizzati senza aggiustamenti di posizione.\n\n\
   -tree on/off\n\n\
\ton     = I file vengono visualizzati per singola cartella.\n

24 \toff    = I file vengono visualizzati in modalità ad albero.\n\n\
   -tree_files /never/always/choose\n\n\
\tnever  = La modalità albero ha due stati: parzialmente espansa o chiusa.\n\
\talways = La modalità albero ha due stati: completamente espansa o chiusa.\n\
\tchoose = La modalità albero ha tre stati: parzialmente espansa  \n\
\tespansa o chiusa.\n\n\
   -order alphabetical/file_type/date/size\n\n\
\tI file vengono visualizzati nell'ordine specificato: alfabetico, per tipo di file,\n\
\tper data o per dimensione.\n\n\
   -view no_icon/large_icon/small_icon/attributes\n\n\
\tI file vengono visualizzati nel formato specificato: solo testo, testo e\n\
\ticone grandi, testo e icone piccole, con attributi.\n\n\
   -direction ascending/descending\n\n\
\tI file vengono visualizzati nella direzione specificata: crescente o\n\
\tdecrescente.\n\n\
   -large_icon_width <dimensione>\n\n\
   -large_icon_height <dimensione>\n\n\
   -small_icon_width  <dimensione>\n\n\
   -small_icon_height <dimensione>\n\n\
       È la dimensione con cui vengono visualizzate le icone nella Gestione di file.\n\
       Le icone più grandi della dimensione specificata saranno tagliate per la parte eccedente.\n\
       La dimensione predefinita per le icone grandi è 38, quella per le icone piccole è 24.\n\n
26 \nSintassi in modalità limitata: %s...\n\n\
   -folder Cartella[,Cartella,Cartella]\n\
   -dir Cartella[,Cartella,Cartella]\n\n\
\tDove Cartella è una cartella secondaria e/o che include\n\
\tla cartella iniziale dell'utente.\n\n

$ MESSSAGES 27-28 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
27 Impossibile ripristinare il file delle impostazioni iniziali; verrà usato il file predefinito.\n
28 Impossibile ripristinare il file delle impostazioni iniziali; verrà usato il file predefinito.\n

$ Messages 29-30 are the strings displayed in a question dialog
29 Salvare le impostazioni correnti per le dimensioni, \n\
le preferenze e i filtri della Gestione di file\n\n\
Queste impostazioni verranno utilizzate per tutte le nuove \n\
finestre della Gestione di file avviate dal Pannello principale.
30 Salvare le impostazioni correnti per le dimensioni, le preferenze\n\
e i filtri della Gestione di file in modalità limitata?\n\n\
Queste impostazioni verranno utilizzate per tutte le nuove finestre della\n\
Gestione di file in modalità limitata avviate dal Pannello principale.

32 Gestione di file - Salvataggio come opzioni predefinite

34 Non è possibile copiare o collegare un file o\n\
una cartella al Cestino.

$ 35 Non è possibile copiare o collegare un file o\n\
$ una cartella fuori dal Cestino.

35 Salvare le impostazioni correnti per la dimensione, \n\
le preferenze e i filtri della Gestione di applicazioni?\n\n\
Queste impostazioni verranno utilizzate per tutte le nuove finestre\n\
della Gestione di applicazioni avviate dal Pannello principale.

36 Non è possibile copiare o collegare un file o una cartella dal Cestino.\n\
Spostare l'oggetto fuori dal Cestino e portarlo nella Gestione di file.\n\
A questo punto sarà possibile copiarlo o collegarlo.

37 Non è possibile copiare o collegare un file dal Cestino.\n\
Spostare l'oggetto fuori dal Cestino e portarlo nella Gestione di file.\n\
A questo punto sarà possibile copiarlo o collegarlo.

$quote "
38 "Specifica di cartella non valida, %s"


$quote
$ MESSSAGE 39 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
39 Cannot start a second File Manager session because a File Manager\n\
session is already open on the desktop.  If you want to start a new File\n\
Manager session, you must first close all current File Manager windows\n\
and remove all File Manager objects from the workspaces.

$ 
$ "-NEW"
40 "Could not connect to ToolTalk:\n%s\nExiting ..."


$set 19



$set 20
$ ****** Module: Menu.c ****(developer comment only)********

$ Messages 1-8, 123 and 9 are the text and mnemonics for the dtfile menu bar
$ Messages 10/11, 128/129, 124/125, 121/122, 126/127, 16/17, 18/19, 84/85,
$   90/91, 22/23, 117/118, are the text and mnemonics for the Fill 
$   pulldown menu 
$ Messages 10/11, 30/33, 92/93, 34/37, 38/41, 42/94, and 97/98 are the text
$   and mnemonics for the Folder pulldown menu 
$ Messages 46/47, 99/100, 48/51, 52/55, 101/102, 103/104, 119/120, and 
$   62/63 are the text and mnemonics for the View pulldown menu 
$ Messages 105-116 are the text and mnemonics for the Help pulldown menu

1 File
2 F
3 Cartella
4 C
5 Visualizzare
6 V
7 Selezioni
8 S

9 A

10 Nuovo...
11 N
16 Caratteristiche...
17 t
18 Ricercare...
19 R
22 Mostrare cestino...
23 M
30 Aggiornare
33 g
34 Passare a...
37 P
38 Cartella iniziale
41 i
$ NOTE: The mnemonic for Up is Message 94 in this set.
42 Cartella precedente
46 Nuovo
47 N
48 Selezionare tutto
51 z
52 Annullare tutte le selezioni
55 A
62 Salvare tutte le caratteristiche ...
63 S
84 Collocare nello spazio di lavoro
85 o
90 Spostare nel cestino
91 p
92 Passaggio veloce a
93 v
$ NOTE: Message 94 is the mnemonic for the "Up" menu item
94 s
97 Aprire terminale
98 T
99 Riordinare le icone
100 R
101 Mostrare file nascosti
102 M
103 Modificare l'elenco dei filtri...
104 f
105 Panoramica
106 P
107 Attività
108 A
109 Riferimenti
110 R
111 Sull'elemento
112 S
113 Uso dell'aiuto
114 U
115 Informazioni sulla Gestione di file
116 I
117 Chiudere
118 C
119 Impostare caratteristiche della vista...
120 I
121 Copiare...
122 C
123 Aiuto
124 Spostare...
125 S
126 Collegare...
127 l
128 Cambiare nome
129 b
130 Nuova cartella...
131 N
132 Nuovo file...
133 f
134 Andare a...
135 A
136 Aprire la finestra del cestino
137 Cambiare nome...
138 Aprire nuova vista
139 A
140 Copiare come collegamento...
141 Impostare le opzioni di visualizzazione...
142 Salvare come opzioni predefinite...
143 S
144 Impostare le opzioni di filtro...
145 f
146 Aprire terminale
147 t
148 Indice
149 I
150 Modificare autorizzazioni...
151 Spostare nel Cestino
152 Spostare in...
153 Copiare in...
154 Copiare come collegamento...
155 Livello superiore
156 Mostrare oggetti nascosti
157 Informazioni sulla Gestione di applicazioni


$set 21
$ ****** Module: ModAttr.c ****(developer comment only)********

$ Messages 1 and 21 are titles for the File Properties dialog, Message 1 is
$   used when the user has specified a title for dtfile windows
$ Messages 2-13 and 18 are labels for the File Properties dialog
$ Message 14 is the output of the file modification date; DO NOT LOCALIZE
$ Messages 15 and 16 are file properties error messages
$ Message 17 is the title for an error dialog
$ Message 19 is the default file owner/group, if real one can't be determined
$ Messages 22-28 are error messages that can be generated when dtfile
$   tries to use the dtfile.config configuration file, this file is
$   used to customize the File Properties dialog

1 Autorizzazioni del file

2 Nome del file:
3 Nome del proprietario:
4 Nome del gruppo:
5 Autorizzazioni:
6 Lettura
7 Scrittura
8 Esecuzione
9 Proprietario:
10 Gruppo:
11 Altri:
12 Dimensione (in byte):
13 Ultima modifica:

$ DO NOT LOCALIZE MESSAGE 14
14 %x %X

15 Informazioni sull'attributo del file non valide.\n\n\
L'utente specificato non esiste.
16 Informazioni sull'attributo del file non valide.\n\n\
Il gruppo specificato non esiste.

17 Errore delle caratteristiche

18 Collegato a:

19 Sconosciuto

21 Gestione di file - Autorizzazioni del file

22 Impossibile aprire il file di configurazione della Gestione di file:
24 Etichetta di campo non identificata nel file di configurazione della Gestione di file:
25 Il file di configurazione della Gestione di file non contiene informazioni per l'identificativo del file system 
26 Impossibile creare un processo per il riquadro di dialogo del file system 
27 Errore nell'esecuzione del riquadro di dialogo del file system 
28 Tentativo di eseguire:

$ do not localize "fsDialog"
29 Non è stato fornito un valore per il campo fsDialog nel file di configurazione di dtfile

30 Nome:

31 Il nome del proprietario non è valido.\n\
L'utente specificato non esiste.

32 Il nome del gruppo non è valido.\n\n\
Il sistema non comprende nessun gruppo di utenti con questo nome.\n\
Per creare un gruppo di utenti con questo nome, utilizzare il \n\
software di amministrazione del sistema.

33 Errore di autorizzazione

34 Gestione di file - Autorizzazioni

$quote "
35 "La casella Autorizzazioni per l'oggetto\n\n\
%s/%s\n\n\
è stata chiusa poiché l'oggetto è stato cancellato\n\
dal sistema."

36 "Avvertenza per Modificare autorizzazioni"

37 "Si stanno modificando le autorizzazioni\n\
della cartella corrente in modo tale da non\n\
poter più a visualizzare tale cartella.\n\
La cartekka verrà chiusa automaticamente e \n\
sarà più possibile riaprirla.\n\n\
Si desidera proseguire?"

38 File Manager Error
39 File Open Error

$set 22
$ ****** Module: ModAttrP.c ****(developer comment only)********

$ Messages 1 and 3 are file properties error messages
$ Message 2 is the title for the properties error dialog

$quote
1 Impossibile aprire il file, operazione interrotta
2 Errore delle caratteristiche
3 Impossibile modificare le caratteristiche per %s
4 Impossibile aprire l'oggetto.
5 Errore di impostazione delle autorizzazioni



$set 23
$ ****** Module: Prefs.c ****(developer comment only)********

$ Messages 1 and 22 are titles for the Set Preferences dialog, Message 1 is
$   used when the user has specified a title for dtfile windows
$ Messages 2-20 and 23-29 are labels for the Set Preferences dialog

1 Impostazione delle opzioni di visualizzazione

2 Visualizzare 
3 Per nome
4 Per nome e icona
5 Per nome e icona piccola
6 Per nome, data, dimensione...
7 Mostrare
8 Per singola cartella
9 Con struttura ad albero
10 Ordinamento
11 Per tipo di file
12 Alfabetico
13 Per data
14 Per dimensione
15 Direzione
16 Crescente
17 Decrescente
18 Disposizione
19 Griglia ordinata 
20 Punto di collocazione

22 Gestione di file - Impostazione delle opzioni di visualizzazione

23 Intestazioni
24 Percorso ad icone
25 Cartella corrente
26 Riga di stato
27 Solo cartelle
28 Cartelle e poi file
29 Cartelle e file
30 Impostazioni predefinite

31 Rappresentazione
32 Solo per nome
33 Per icone grandi
34 Per icone piccole
35 Righe e colonne
36 Percorso esplicito
37 Riga messaggi


$set 24
$set 25
$set 26



$set 27
$ ****** Module: Trash.c ****(developer comment only)********

$ Messages 3, 4, and 81 are titles for error warning dialogs
$ Message 5-8, 56, 58, 78, 82, and 83 are trash error messages
$ Message 9 is the title of the trash container
$ Messages 24/26 and 28/30 are the text and mnemonics for the File pulldown
$   menu
$ Message 73 is an error dialog title
$ Messages 74, 75, 79, and 80 are trash warning messages

3 Errore del Cestino
4 Avvertenza del Cestino

5    I file o le cartelle seguenti non possono essere depositati nel Cestino:     \n
6    Gli elementi seguenti sono cartelle non vuote o file per \n   \
i quali non si possiede l'autorizzazione in scrittura.    \n   \
Si desidera continuare?\n
7    Non si possiede l'autorizzazione in scrittura per i seguenti file.    \n   \
Si desidera continuare?\n
8    Le seguenti cartelle non sono vuote.    \n   \
Si desidera continuare?\n

9 Cestino

24 Riportare indietro 
26 R
28 Cancellare definitivamente 
30 C

56    I seguenti file non sono stati riportati indietro:\n
$ Message 58 supplements Message 56
58 (Più altri %d file)

73 Cancellare definitivamente i file

74 Il file seguente si trova anche sullo spazio di lavoro:\n\
   %s\n\
Spostando il file nel cestino, esso verrà cancellato \n\
dallo spazio di lavoro.
75 I file seguenti si trovano anche sullo spazio di lavoro:\n\
   %s\n\
Spostando i file nel cestino, essi verranno cancellati \n\
dallo spazio di lavoro.

78 La Gestione di file non supporta file e cartelle\n\
con nomi contenenti spazi vuoti, e perciò non consente\n\
di cancellarli.\n

79 Il file sullo spazio di lavoro %s si trova anche nel file system dell'utente in:\n\
   %s\n\
Spostando il file nel Cestino, esso verrà cancellato dal\n\
file system.

80 Gli oggetti sullo spazio di lavoro si trovano anche nel file \n\
system dell'utente in:\n\
   %s\n\
Spostando i file nel cestino, essi verranno cancellati dal\n\
file system.

81 Errore di cancellazione definitiva di file/cartelle

82 Impossibile eliminare il file seguente dal file system:   \n

83 Non è stato possibile aprire il file .trashinfo in $HOME/.dt/Trash.\n\
   Questo file è necessario per il funzionamento del Cestino.\n\
   Controllare le autorizzazioni del file e della relativa cartella.\n
84 Gli oggetti cancellati definitivamente dal Cestino vengono \n\
   eliminati in modo permanente dal sistema.\n\n\
   Procedere ugualmente?\n
85 L'oggetto dello Spazio di lavoro che si sta cancellando è una copia \n\
   collegata di un oggetto della Gestione di file, situato in\n\
   %s\n\
   Facendo clic su OK, sia la copia collegata che l'oggetto \n\
   originale verranno cancellati.\n\
   Se si desidera cancellare solo l'oggetto dello Spazio di lavoro:\n\
      1) Fare clic su Annullare in questa finestra di dialogo\n\
      2) Selezionare 'Eliminare dallo spazio di lavoro' \n\
         dal menu a scomparsa dell'oggetto.
86 Gli oggetti dello Spazio di lavoro da cancellare sono copie \n\
   collegate di altri oggetti della Gestione di file, situati in\n\
   %s\n\
   Facendo clic su OK, sia le copie collegate che gli \n\
   oggetti originali verranno cancellati.\n\
   Se si desidera cancellare solo gli oggetti dello Spazio di lavoro:\n\
      1) Fare clic su Annullare in questa finestra di dialogo\n\
      2) Selezionare 'Eliminare dallo spazio di lavoro' \n\
         dal menu a scomparsa degli oggetti.
87 Non è possibile aprire gli oggetti contenuti nel Cestino.\n\n\
   Per aprire uno di questi oggetti, riportarlo nella \n\
   Gestione di file usando l'opzione 'Riportare indietro'.

88 Impossibile scrivere nel file temporaneo.\n\
   È possibile che il file system sia pieno.

89 Impossibile aprire la directory del Cestino\n\
   %s\n\
   (problema di autenticazione o di autorizzazione all'accesso)\n\n\
   Non sarà possibile eseguire nessuna operazione con il Cestino.

90 Impossibile inizializzare il Cestino.\n\
   Controllare:\n\
        1)Le autorizzazioni di $HOME/.dt/Trash.\n\
        2)Lo spazio disponibile su disco.\n\
        3)L'autenticazione.

$ 91 is not used
91 Una copia collegata del seguente oggetto si trova\n\
sullo sfondo dello spazio di lavoro:\n\
Spostando l'oggetto nel cestino questo verrà cancellato\n\
sia dalla Gestione di file che dallo sfondo dello spazio di lavoro.

$ 92 is not used
92 Una copia collegata dei seguenti oggetti si trova anche\n\
sullo sfondo dello spazio di lavoro:\n\
Spostando gli oggetti nel cestino questi verranno cancellati\n\
sia dalla Gestione di file che dallo sfondo dello spazio di lavoro.

93 Impossibile accedere al file di informazioni sul Cestino:\n\
   %s\n\
   Non sarà possibile eseguire nessuna operazione con il Cestino.\n\
   Le cause più comuni di questo problema sono:\n\
     - Problemi di autenticazione di rete.\n\
     - Spazio su disco non sufficiente.\n\
     - Autorizzazioni errate per $HOME/.dt/Trash.

94 La Gestione di file non supporta oggetti il cui nome\n\
contiene spazi vuoti, e perciò non consente di cancellarli.\n\
Per cancellare questi oggetti:\n\
  - selezionare 'Aprire finestra di terminale' dalla barra \n\
    dei menu della Gestione di file\n\
  - eseguire il comando Eliminare per cancellarli.

95 Impossibile accedere alla directory del Cestino:\n\
   %s\n\
   Non sarà possibile eseguire nessuna operazione con il Cestino.\n\
   Le cause più comuni di questo problema sono:\n\
     - Problemi di autenticazione nella rete.\n\
     - Spazio su disco non sufficiente.\n\
     - Autorizzazioni errate. 

96 Non è stato possibile cancellare i seguenti oggetti dal sistema:   \n

97 Non è stato possibile spostare i seguenti oggetti nel Cestino:     \n

98 (Più %d oggetto/i aggiuntivo/i)

$ message 99 is not used
99 L'utente non possiede le autorizzazioni in scrittura per i file seguenti:\n\
Procedere ugualmente?\n

100 Le seguenti cartelle non sono vuote.\n\
Procedere ugualmente?\n

101 I seguenti oggetti non sono stati riportati indietro:\n

102 Una copia collegata del seguente oggetto si trova \n\
nello spazio di lavoro:\n\
%s\n\
Spostando l'oggetto nel Cestino, esso verrà cancellato\n\
sia dalla Gestione di file che dallo spazio di lavoro.

103 Una copia collegata dei seguenti oggetti si trova anche \n\
nello spazio di lavoro:\n\
%s\n\
Spostando gli oggetti nel cestino, essi verranno cancellati\n\
sia dalla Gestione di file che dallo spazio di lavoro.

104 L'utente non possiede l'autorizzazione in scrittura per i seguenti file:\n\
Procedere ugualmente?\n

105 Non è possibile eseguire l'azione predefinita per un oggetto del cestino.\n\n\
    Per eseguire l'azione predefinita per questo oggetto, \n\
    usare l'opzione 'Riportare indietro' per riportarlo\n\
    nella Gestione di file e quindi eseguire l'azione.

106 Informazioni sul Cestino

$quote "
107 "Almeno un oggetto nella seguente Cartella\n\
non può essere cancellato.\n\n\
%s\n\n\
E' molto probabile che non si abbiano le autorizzazioni \n\
per cancellare tale oggetto."

108 "Non è possibile cancellare il seguente file.\n\n\
%s\n\n\
E' molto probabile che non si abbiano le autorizzazioni\n\
per cancellare tale oggetto."

109 "Avvertenza per Riportare indietro"

110 "Esiste già un oggetto con il\n\
seguente nome\n\n\
    %s\n\n\
Selezionare Ok se si desidera sostituirlo\n\
con l'oggetto posto nel Cestino.\n\n\
Selezionare Annullo se si desidera conservare\n\
l'oggetto. Se si ridenomina l'oggetto, \n\
sarà possibile eseguire di nuovo Riportare \n\
indietro e conservare entrambi gli oggetti."

111 "Errore per Riportare Indietro"

112 "Non è possibile cancellare il seguente \n\
oggetto/i seguenti oggetti. E' molto \n\
probabile che sia/siano già stato/stati \n\
cancellato/i.\n\n"

113 "Almeno un oggetto nella seguente Cartella(e)\n\
non può essere cancellato oppure la \n\
cartella stessa non può essere cancellata.\n\
E' molto probabile che non si abbiano le autorizzazioni\n\
per cancellare tale oggetto.\n\n"

114 "Impossibile cancellare il/i seguente/i file.\n\
E' molto probabile che non si abbiano le autorizzazioni\n\
per cancellare il/i file.\n\n"

115 "Non è possibile cancellare il/i seguente/i oggetto/i.\n\
La scrivania non può operare correttamente senza \n\
tale oggetto/tali oggetti.\n\n"


$set 28
$ ****** Module: Desktop.c ****(developer comment only)********

$ Messages 1-3 Workspace error messages
$ Message 4 is the title for the Workspace popup menus
$ Messages 5-8 are the labels and mnemonics for the Workspace obj popup menu

$quote
1 Nello spazio di lavoro è già presente un file con questo nome.\n\
Scegliere un nome diverso.
2 Esiste già un file con questo nome nella cartella originale\n\
(la cartella da cui proviene il file).\n\
Scegliere un nome diverso.
3 Nello spazio di lavoro è già presente un file di nome %s.\n\
Non è possibile aggiungere un altro file con lo stesso nome.\n\
Per disporre entrambi i file sullo spazio di lavoro, cambiare nome a uno dei due.

4 Oggetto dello spazio di lavoro

5 Eliminare dallo spazio di lavoro
6 E
7 Aprire cartella precedente
8 A

9 Nello spazio di lavoro è già presente un oggetto con questo nome.\n\
Scegliere un nome diverso.

10 Esiste già un oggetto con questo nome nella cartella originale\n\
(la cartella da cui proviene l'oggetto).\n\
Scegliere un nome diverso.

11 Nello spazio di lavoro è già presente un oggetto con il nome %s.\n\
Non è possibile aggiungere un altro oggetto con lo stesso nome.\n\
Per disporre entrambi gli oggetti sullo spazio di lavoro, cambiare nome a uno dei due.

$quote "
12 "E' stato specificato non valido. Lo spazio \n\
di lavoro assunto è quello Uno e già esiste \n\
sul fondo di questo spazio di lavoro un \n\
oggetto denominato:\n\
   %s\n\
Non è possibile porre un altro oggetto con lo stesso nome\n\
sullo Spazio di lavoro. Per avere entrambi gli oggetti\n\
sullo stesso spazio di lavoro occorre ridenominarne uno."

13 "L'oggetto:\n\
   %s\n\
non esiste nel sistema di file."



$set 29
$ ****** Module: Help.c ****(developer comment only)********

$ Message 1 is the title for the dtfile help dialog
$ Messages 1 and 2 are titles for the Help dialog, Message 2 is
$   used when the user has specified a title for dtfile windows
$ Message 3 is the text for help on the "Open New View" action
$ Message 4 is the text for help on the "Open In Place" action
$ Messages 5-6 are help error messages

$quote
1 Aiuto della Gestione di file
2 Aiuto su %s 

3 L'azione `%s` visualizza la cartella richiesta in una nuova finestra della Gestione di file.

4 L'azione `%s` visualizza la cartella richiesta nella finestra corrente della Gestione di file.

5 Non sono disponibili informazioni di aiuto per l'azione %s 
6 Non sono disponibili informazioni di aiuto per il tipo di file %s 

7 L'azione `%s` visualizza la cartella richiesta \n\
in una nuova finestra della Gestione di file.

8 L'azione `%s` visualizza la cartella richiesta \n\
nella finestra corrente della Gestione di file.

9 Aiuto sul Cestino

$quote "
10 "L'aiuto non è disponibile se sono selezionati più oggetti."

$set 30
$ ****** Module: SharedMsgs.c ****(developer comment only)********

$ The following messages are used in more than one place within the
$ dtfile source.  Unless otherwise specified, they are error messages.

$quote
1 Impossibile leggere %s

$ Message 2 is used to format the file attributes in 'by attributes' mode
2 %d %b %Y %H:%M:%S

3 Impossibile spostare o copiare il file %s dall'host %s.\n\
È possibile che il mount dell'host %s sul file system non sia stato eseguito correttamente.
4 Impossibile spostare o copiare il file %1$s dall'host %2$s.\n\
È possibile che il mount dell'host %3$s sul file system non sia stato eseguito correttamente.
5 Impossibile leggere %s
6 Impossibile scrivere nella cartella %s
7 Impossibile creare %s
8 Impossibile cancellare %s
9 Un file o una cartella con il nome\n\
  %s\n\
  esiste già e non può essere sovrascritto/a.\n\
  Inserire un nome diverso.

$ Message 10 is the help topic string for help on a file icon or action
10 Descrizione del tipo di file o dell'azione

11 Non è possibile specificare un percorso come parte del nome del file.
12 Informazioni di ricerca errate.\n\n\
La cartella seguente non esiste, o non è accessibile:\n\n\
     %s\n

$ Messages 13-16 are dialog titles
13 Panoramica su %s 
14 Supporto didattico su %s
15 Informazioni su %s
16 Gestione di file

$ Message 18 is appended to ".." folders
18 (livello superiore)

$ Messages 19 and 20 are messages which appear when a user clicks item help
$ outside the application.  19 is the title, while 20 is the message.
19 Errore dell'aiuto sull'elemento
20 È necessario selezionare un componente\n\
nella Gestione di file.

$ Messages 21 and 22 are titles for error dialogs
21 Errore di spostamento file
22 Errore di collegamento file
23 Impossibile creare '%s'\n\n\
La causa più probabile è la mancanza delle \n\
autorizzazioni appropriate per i file\n\
o le cartelle in oggetto.\n\n\
Per visualizzare le autorizzazioni, selezionare l'oggetto \n\
e quindi scegliere 'Modificare autorizzazioni...'

24 È necessario selezionare un oggetto nella Gestione di file.\n\
La funzione di aiuto 'Sull'elemento' opera correttamente solo\n\
all'interno dell'applicazione da cui è stata lanciata.

25 Impossibile creare '%s'\n\n\
La causa più probabile è la mancanza delle \n\
autorizzazioni appropriate per i file\n\
o le cartelle in oggetto.\n\n\
Per visualizzare le autorizzazioni, selezionare l'oggetto e\n\
quindi scegliere 'Modificare autorizzazioni...' dal menu\n\
Selezioni o dal menu a scomparsa.

26 È necessario selezionare un oggetto nella Gestione di applicazioni.\n\
La funzione di aiuto 'Sull'elemento' opera correttamente solo\n\
all'interno dell'applicazione da cui è stata lanciata.


$set 31

$set 32
$ ****** Module: dterror.src ****(developer comment only)********
$ Message 1 is a title for dterror
1 Errore dt 
2 Errore di apertura della directory nella Gestione di file

$set 33
1 Oggetti multipli
2 Cartella corrente

$quote "
3 "Errore per Spostare oggetto"

4 "Errore per Copiare oggetto"

5 "Errore per Collegare oggetto"

6 "Nel sistema di file non esiste \n\
l'ubicazione in cui si sta cercando di \n\
spostare:\n\n\
   %s"

7 "Nel sistema di file non esiste \n\
l'ubicazione in cui si sta cercando di\n\
copiare:\n\n\
   %s"

8 "Nel sistema di file non esiste \n\
l'ubicazione cui si sta cercando di \n\
collegare:\n\n\
   %s"

9 "L'ubicazione in cui si sta cercando di \n\
spostare:\n\n\
   %s\n\n\
non è una cartella."

10 "L'ubicazione in cui si sta cercando \n\
di copiare:\n\n\
   %s\n\n\
non è una cartella."

11 "L'ubicazione cui si sta cercando \n\
di collegare:\n\n\
   %s\n\n\
non è una cartella."

12 "L'oggetto in si sta cercando di spostare:\n\n\
   %s\n\n\
non esiste nel sistema di file."

13 "Uno degli oggetti che si sta cercando di \n\
spostare:\n\n\
   %s\n\n\
non esiste nel sistema di file.\n\
Non effettuare alcun spostamento."

14 "L'oggetto che si sta cercando di copiare:\n\n\
   %s\n\n\
non esiste nel sistema di file."

15 "Uno degli oggetti che si sta cercando di \n\
copiare:\n\n\
   %s\n\n\
non esiste nel sistema di file.\n\
Non effettuare alcuna copia."

16 "L'oggetto cui si sta cercando di collegare:\n\n\
   %s\n\n\
non esiste nel sistema di file."

17 "Uno degli oggetti cui si sta cercando \n\
di collegare:\n\n\
   %s\n\n\
non esiste nel sistema di file.\n\
Non effettuare alcun collegamento."

$set 99
$ ****** Version Information ********
$ DO NOT TRANSLATE THESE MESSAGES
$ DO NOT CHANGE THESE MESSAGES
$ DO NOT LOCALIZE THESE MESSAGES
$ These messages are used for the version information.
$quote
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $TOG: dtfile.msg /main/10 1999/09/16 09:44:40 mgreess $

