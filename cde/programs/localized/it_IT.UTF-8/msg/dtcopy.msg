$ $XConsortium: dtcopy.msg /main/8 1996/11/19 19:28:11 rws $
$ *****************************************************************************
$ **
$ **  File:        dtcopy.msg
$ **
$ **  Project:     CDE File Manager
$ **
$ *****************************************************************************

$set 2
$ ****** Module: main_dtcopy.c ******
3 file
4 file
5 cartella secondaria
6 cartelle secondarie
7 Errori: nessuno
8 Errori: %d
10 Sintassi: %s [opzioni ...] origine destinazione\n
11 Gestione di file - Spostare cartella
12 Gestione di file - Copiare cartella
13 %s non eliminata a causa di errori.
14 Completato.
15 Annullato in
$ ** Message 20 - 32
$ ** LongFileOpNames ... message number = op + 20.
20 elaborazione in corso
21 apertura in corso
22 apertura in corso
23 apertura in corso
24 lettura del collegamento in corso
25 cancellazione in corso
26 copia in corso
27 creazione della directory in corso
28 copia del collegamento in corso
29 creazione del collegamento in corso
30 lavoro in corso
31 è una copia
32 è un collegamento
33 cambiamento del nome in corso

$set 3
$ ****** Module: copydialog.c ******
1 Riprendere
2 Pausa
3 Spostamento di:
4 Copia di:
5 Nella cartella:
7 Questa operazione interromperà il processo di copia.\n\
I file già copiati resteranno nella cartella di destinazione.
10 Questa operazione interromperà il processo di spostamento.\n\
I file già spostati resteranno nella cartella di destinazione.
11 Copiare cartella - Avvertenza
12 Spostare cartella - Avvertenza
13 Annullare la copia
14 Continuare la copia
15 Annullare lo spostamento
16 Continuare lo spostamento
17 Spostata
18 Copiata

19 "Impossibile spostare cartella su se stessa\n\
%s\n"

20 "Impossibile copiare cartella su se stessa\n\
%s\n"


$set 4
$ ****** Module: errordialog.c ******
1 Errore %s\n%s
2 Pausa per errori
3 Copiare cartella - Errore
4 Spostare cartella - Errore

$quote "
5 "Spostare oggetto - Errore"

6 "Copiare oggetto - Errore"

7 "Cestinare oggetto - Errore"

8 "Non si dispone dell'autorizzazione per porre l'oggetto \n\n\
    %s\n\n\
nel cestino.\n\n\
Usare la scelta Modificare autorizzazioni dal menu \n\
a scomparsa dell'oggetto oppure dal menu Selezionato\n\
per attivare l'autorizzazione Lettura per l'oggetto.\n\n\n\
Nota: Se l'oggetto è una cartella, occorre avere\n\
anche l'autorizzazione Lettura per tutti gli \n\
oggetti contenuti nella cartella prima di poterla\n\
porre nel cestino."

9 Impossibile aprire %s


$set 5
$ ****** Module: overwrtdialog.c ******
$quote
1 Un file di nome\n%s\nesiste già nella cartella\n%s
2 Spostare cartella - Sovrascrivere file
3 Sostituire il file esistente
4 Rinominare il file esistente in:
5 Rinominare il nuovo oggetto in:
6 Tralasciare questo file
7 Applicare questa azione ai successivi conflitti di nome
8 Azione da eseguire:
9 Copiare cartella - Sovrascrivere file
10 %s\nesiste già: scegliere un altro nome.
11 Sovrascrivere file - Errore
12 %s\nnon può essere rinominato in\n%s\n%s

$set 6
$ ****** Module: utils.c ******
$ ** Button labels.
1 OK
2 Pausa
3 Annullare
4 Aiuto
5 Aiuto per la copia di directory
6 Continuare
7 Annullare la copia
8 Annullare lo spostamento
