$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        DtPrint.msg
$ **
$ **  Project:     DT
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for the Dt Print
$ **  library.
$ **
$ **
$ *****************************************************************************
$ **
$ **  (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994 Novell, Inc.
$ **
$ *****************************************************************************
$ **
$ **
$ **  $XConsortium: DtPrint.msg /main/10 1996/11/19 19:27:43 rws $
$ *****************************************************************************
$ *****************************************************************************

$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There MAY be three types of messages in this file:
$ 
$  1. Messages that appear in dialogs or are displayed to the user.
$ 
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$ 
$  2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$  3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ******************************************************************************

$quote "


$set 1
$ *************************************************************
$ ** 							     ** 
$ ** PrintSetupBox messages and labels 			     **
$ ** 							     ** 
$ *************************************************************
$ Warning messages
$ Note: do not localize the chars %s

$quote

1 Impostazione della risorsa workAreaLocation non è valida.

$ Label values
$ 
2 Altre stampanti...
3 Stampare su file:
4 Selezionare file...
5 Annullare
6 Copie:
7 Aiuto
8 Stampare
9 Nome della stampante:
10 Impostare...
11 Descrizione della stampante:

$ Warning messages
$ Note: do not localize the chars %s
12 Il valore Copie non è dell'intervallo consentito.
13 Modalità per l'Impostazione della stampa.
14 Impossibile cambiare la modalità dell'Impostazione della stampa.

$ Label values
$ 
15 Informazioni...

$ Warning messages
$ Note: do not localize the chars %s
16 Stampante non valida
17 Impossibile trovare la stampante %s sul server di stampa %s
18 Impossibile connettersi al server di stampa
19 Server X %s non supporta la stampa
20 Impossibile determinare una stampante predefinita
21 Impossibile trovare un server X per %s 
22 La specificazione non indica il nome della stampante:
23 Destinazione di stampa non valida
24 Impossibile convertire dal testo composto
25 Gli argomenti non sono necessari nella conversione di risorse.

$ Label values
$ 
26 Errore nella selezione della stampante

$ Warning messages
$ Note: do not localize the chars %s
27 Impossibile trovare alcuna stampante.

$ Label values
$ 
28 descrizione non disponibile
29 Selezionare file
30 Informazioni sulla stampante
31 Formato:
32 Modello:
33 Stampanti:
34 Altre stampanti
35 Stampare su stampante
36 Nome file:
37 Impostazione della stampa - Aiuto

$set 2
$ *************************************************************
$ ** 							     ** 
$ ** _DtPrint Frame GUI messages			     **
$ ** 							     ** 
$ *************************************************************
$ 
$quote "

$ Label values for Margin Frame
$ 
1 "Margini"
2 "Superiore:"
3 "Destro:"
4 "Inferiore:"
5 "Sinistro:"
$ 
$ Label values for Header/Footer Frame
$ 
6 "Intestazioni e piè di pagina"
7 "In alto a sinistra:"
8 "In alto a destra:"
9 "In basso a sinistra:"
10 "In basso a destra:"

$set 3
$ *************************************************************
$ ** 							     ** 
$ ** Dt Print Dialog Manager Job Options GUI messages        **
$ ** 							     ** 
$ *************************************************************
$ 
$quote

$ Label values for Margin Frame
1 Spedizione posta al termine
2 Titolo pagina di testa:
3 Opzioni del comando di stampa:
