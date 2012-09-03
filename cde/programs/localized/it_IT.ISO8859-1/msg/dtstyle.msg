 
$ "$XConsortium: dtstyle.msg /main/10 1996/11/19 19:29:20 rws $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtstyle.msg
$ **
$ **  Project:     DT Style Manager
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtstyle
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
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
$ 2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.  
$ 
$     These messages are identified by the following:
$ 
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ******************************************************************************
$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a 
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ * 
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$set 2
$ Message 1 from set 2 was deleted from the message catalog

$ _DtMessage 2 is the title of all Information dialog boxes
2 Avvertenza
$ _DtMessage 3 is the title of all Error dialog boxes
3 Errore
$ _DtMessage 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
4 Impossibile comunicare con la \n\
Gestione delle sessioni... Uscita.
 
$  MESSAGES 5-6 IN SET 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
5 La Gestione degli stili è già in esecuzione,\n\
il secondo tentativo è stato abbandonato.
6 Si è verificato un errore del toolkit X...\n\
Uscita.

$ _DtMessage 7 is the title of all Help dialog boxes
7 Gestione degli stili - Aiuto

$ _DtMessage 8 is an error message that appears when you are using Itemh Help
$ and select on something outside of the Style Manager
8 È necessario selezionare un elemento\n\
all'interno della Gestione degli stili.

$ _DtMessage 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
9 Impossibile ottenere informazioni sul risparmio schermo\n\
dalla Gestione delle sessioni. Le impostazioni di avvio\n\
potrebbero essere errate.


$set 4
$ _DtMessage set 4 is for the Style Manager main window
$ Messages 6,8-26,34,39 from set 4 were deleted from the message catalog

$ _DtMessage 1 is the title of the main dtstyle window
1 Gestione degli stili

$ mnemonic and label for the File menubar item.
2 F
3 File
$ mnemonic and label for the Help menubar item.
4 A
5 Aiuto

$ _DtMessage 7 is the label for the Exit button in the File pulldown
$ the mnenomic for the Exit button is set 4 _DtMessage 40
7 Chiudere

$ Messages 27-33,35  are the labels on the buttons in the dtstyle main window.
27 Colori
28 Font
29 Sfondo schermo
30 Tastiera
31 Mouse
32 Segnale acustico
33 Schermo
35 Avvio

$  MESSAGE 36 IN SET 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
36 Nessuna informazione sul ripristino di Dtstyle disponibile per la sessione corrente

$ _DtMessage 37 is the label on the dtstyle icon
37 Stili 

$ Message 38 is the label on the Window button in the dtstyle main window.
38 Finestra

$ _DtMessage 40 is replaced with message 53
$ 40 C

$ *
$ * mnemonic and label for the Help menu items.
$ *

$ Messages 41-42 have been replaced with messages 54-55
$ Overview... menu item
$ 41 I
$ 42 Introduzione...

$ Tasks... menu item 
43 A
$ Message 44 has been replaced with message 56
$ 44 Attività...

$ Reference... menu item
$ 45 R
$ Message 46 has been replaced with message 57
$ 46 Riferimenti...

$ On Item menu item
$ Keybaord menu item
47 S
48 Sull'elemento 

$ Using Help... menu item
49 U
$ Message 50 has been replaced with message 58
$ 50 Uso dell'aiuto... 

$ Messages 51-52 have been replaced with messages 59-60
$ Overview menu item
$ 51 V
$ 52 Versione...

$ Message 53 is replaced with message 40
53 C

$ Messages 54-55 are replacements for messages 41-42
54 P
55 Panoramica

$ Message 56 is a replacement for messages 44
56 Attività

$ Message 57 is a replacement for messages 46
57 Riferimenti

$ Message 58 is a replacement for messages 50
58 Uso dell'aiuto

$ Messages 59-60 are replacements for messages 51-52
59 I
60 Informazioni sulla Gestione degli stili


$set 5
$ _DtMessage set 5 is for the Font dialog
$ Messages 1-6, 7-16 from set 5 were deleted from the message catalog

$ _DtMessage 17 is the label of the titlebox of the preview font area
17 Anteprima

$ _DtMessage 18 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is True
18 Per alcune applicazioni, il nuovo font apparirà \n\
dalla prossima apertura di una finestra. \n\
Per altre applicazioni, come la Gestione di file e \n\
la Gestione di applicazioni, occorrerà uscire dal \n\
desktop ed effettuare nuovamente il login.

$ _DtMessage 19 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is False
19 Il font selezionato verrà usato\n\
al riavvio della sessione.

$ _DtMessage 20 and 21 is the text that initially appears in the Font Preview 
$ area displaying sample user and system fonts.
20 AaBbCcDdEeFfGg0123456789
21 AaBbCcDdEeFfGg0123456789

$ _DtMessage 22 is the label of the titlebox containing the list of font sizes
22 Dimensione

$ _DtMessage 23 is the title of the Fonts dialog
23 Gestione degli stili - Font
$ _DtMessage 24 is an informative message used by the Fonts dialog  

24 Il font usato al momento per la scrivania non M-h \n\
disponibile nell'elenco Dimensione. Se viene selezionato\n\
ed applicato un nuovo font, non sarM-` possibile ritornare al \n\
font corrente usando la finesta Gestore degli stili - Font.


$set 6
$ _DtMessage set 6 is for the Beep dialog
$ _DtMessage 1,6 from set 6 have been deleted from the message catalog

$ Messages 2 through 5 are the default buttons and labels of the scales.  
2 Impostazioni predefinite
3 Volume
4 Tono 
5 Durata 

$ _DtMessage 7 is the title of the Beep dialog box
7 Gestione degli stili - Segnale acustico


$set 7
$ _DtMessage set 7 is for the Startup dialog
$ Messages 1-2 from set 7 have been deleted from the message catalog

$ Messages 3 through 9 are labels of the buttons and text visible in the 
$ Session Startup dialog box. 
3 Al login:
4 Riprendere la sessione corrente
5 Ritornare alla sessione iniziale 

$ Message 6 is obsolete - DO NOT localize message #6 from this set
$ 6 Richiedere al logout

7 Richiesta di conferma del logout:
8 Sì
9 No
10 Impostare sessione iniziale...

$ _DtMessage 11 is a warning message when the user pushes Set Home Session...
$ Notice the spaces before Continue .. the spaces make it centered.
11 Questa operazione sostituirà la sessione \n\
iniziale esistente con la sessione corrente.\n\n \
                 Continuare?

$ _DtMessage 12 is the title of the warning dialog box when Set Home Session is 
$ pushed. 
12 Avvertenza

$ _DtMessage 13 is an error message
13 Questa sessione è stata avviata da uno script del sistema\n\
a finestre. Poiché le impostazioni di avvio non sono valide,\n\
la sessione non potrà essere ripristinata.

$ _DtMessage 14 is the title of the Session Startup dialog box.
14 Gestione degli stili - Avvio

$ _DtMessage 15 is an error message
15 La Gestione delle sessioni non è in esecuzione.\n\
Le impostazioni di avvio non sono valide.

$ Message 16 is used in the Sessin Startup dialog.  It is the
$   string that appears to right of the Startup dialog icon IF
$   the user selects a display-specfic session at the login screen.
$quote "
16 "Sessione dedicata al display"


$set 8
$ All of the messages from set 8 have been deleted from the message catalog


$set 9
$ _DtMessage set 9 is for the Mouse dialog
$ Message 7 from set 9 have been deleted from the message catalog

$ Messages 1-4 are labels of toggle buttons in the Mouse Dialog. 
$quote
1 Sinistra
2 Destra
3 Mano:
4 Pulsante 2:
$ _DtMessage 5 is the label of default button.
5 Impostazioni predefinite

$ Messages 6,8,9 are labels of the scales.
6 Doppio clic
8 Accelerazione
9 Soglia

$  MESSAGE 10 IN SET 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
10 ERRORE: Impostazione della mappatura del puntatore\n

11 Trasferimento
12 Regolazione

$ _DtMessage 13 appears in a Notice dialog after the user selects the 
$ left-handed or right-handed toggle
13 Le funzioni dei pulsanti destro e sinistro del \n\
mouse verranno invertite immediatamente facendo \n\
clic su OK. L'immagine `Prova del doppio clic' \n\
nel riquadro mostra l'impostazione corrente \n\
dei pulsanti 1 e 3.

$ _DtMessage 14 appears in a Notice dialog after the user selects the 
$ transfer toggle to change the function of the middle mouse button.
14 Facendo clic su OK, il pulsante 2 del mouse assumerà la\n\
funzione di `trasferimento' (trascinamento) dalla prossima sessione.

$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is True)
15 La nuova impostazione del doppio clic avrà \n\
effetto dal prossimo avvio delle applicazioni.

$ _DtMessage 16 appears in a Notice dialog after the user selects a new 
$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is False)
16 La nuova impostazione del doppio clic\n\
avrà effetto dalla prossima sessione.

$ _DtMessage 17 is the text below the mouse icon. Notice the two lines.
17 Prova del\ndoppio clic

$ _DtMessage 18 is the Mouse Dialog title
18 Gestione degli stili - Mouse

$ _DtMessage 19 appears in a Notice dialog after the user selects the 
$ adjust toggle to change the function of the middle mouse button.
19 Facendo clic su OK, il pulsante 2 assumerà la funzione di\n\
`regolazione' dalla prossima sessione. In questa modalità,\n\
il pulsante 2 potrà essere utilizzato per estendere o\n\
ridurre una selezione, mentre le azioni di trasferimento\n\
(trascinamento) verranno effettuate con il pulsante 1.\n\

$set 10
$ _DtMessage set 10 is for the SaveRestore functionality
$ Messages 2,4 from set 10 have been deleted from the message catalog

$  MESSAGE 1,3 IN SET 10 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ Messages 1,3 make up one error message.  Put together, they say 
$ "<filename> <_DtMessage 2> <_DtMessage 1>
1 Controllare le autorizzazioni del file.
3 %s è il file che si è cercato di utilizzare per salvare la sessione\n


$set 11
$ _DtMessage set 11 is for the Backdrop dialog

$  MESSAGE 1 IN SET 11 WILL ONLY APPEAR IN THE DT ERRORLOG FILE

$ _DtMessage 1 - could not open a backdrop directory
1 Impossibile aprire la directory %s. 
$ _DtMessage 2 - error opening the backdrop files
2 L'impostazione dello sfondo schermo con la Gestione degli\n\
stili non funziona correttamente perché non è disponibile nessun\n\
file per lo sfondo dello schermo. Controllare il file $HOME/.dt/errorlog.


$ _DtMessage 12 is the title of the Backdrop dialog box
12 Gestione degli stili - Sfondo schermo


$set 12
$ _DtMessage set 12 is for the Screen dialog
$ Message 2 from set 12 has been deleted from the message catalog

$ _DtMessage 1 is the title of the Screen dialog box. 
1 Gestione degli stili - Schermo

$ Messages 3-14 are the labels and buttons for the Screen dialog
3 Impostazioni predefinite
4 Risparmio schermo
5 Avviare il risparmio schermo dopo
6 Durata di ogni risparmio schermo 
7 minuti
8 Blocco dello schermo
$ Message 9 has been removed 
10 Sì
11 No
12 Risparmio schermo:
13 Blocco dello schermo:
14 Avviare il blocco dello schermo dopo
15 Oscuramento automatico dello schermo
16 Oscuramento dello schermo:
17 Avviare l'oscuramento dello schermo dopo
18 Blocco del pannello principale
$ Message 19 has been replaced by message 21
19 Con sfondi
20 Durata di ciascuno sfondo 
$ Message 21 is a replacement for message 19
21 Blocco con sfondi
22 Blocco senza sfondi

$set 13
$ _DtMessage set 13 is for the Keyboard dialog
$ _DtMessage 1,5 from set 13 have been deleted from the message catalog

$ _DtMessage 2 through 4 are labels of scales, buttons, and text visible in the
$ Keyboard dialog box.
2 Impostazioni predefinite
3 Ripetizione automatica
4 Volume del clic

$ _DtMessage 6 is the title of the Keyboard dialog box
6 Gestione degli stili - Tastiera


$set 14
$ _DtMessage set 14 is for the Color dialog
$ _DtMessage 1-4,9,14-15,17,19,20,22,24 from set 14 have been deleted from the message catalog

$ Messages 5 through 8 are labels and buttons visible in the Color dialog
5 Tavolozze
6 Aggiungere...
7 Cancellare...
8 Modificare...

$ _DtMessage 10 is the title above the text area in the Add Palette dialog box
10 Nome della nuova tavolozza:

$ _DtMessage 11 is the title of the Add Palette dialog box.
11 Nuova tavolozza

$ Messages 12 through 14 are error messages.
12 Il nome della tavolozza non può contenere\n\
questi caratteri:\n\n\
<spazio> * : ( ) [ ] { } < > ! | \" / \\
$ LOCALIZERS: FOR 8 AND 16 BIT LANGUAGES YOU MAY WANT TO CHANGE THE MESSAGE 
$ TO SAY THE PALETTE NAME MUST BE 10 BYTES OR LESS
13 Il nome della tavolozza non può avere\nuna dimensione maggiore di 10 byte.\n

$ _DtMessage 16 is the title of the Delete Palette dialog box.  See screen 4a (2).
16 Cancellazione della tavolozza

$ _DtMessage 18 appears in an information dialog.
18 Impossibile cancellare l'ultima tavolozza.\n

$ _DtMessage 21 is an error message
21 Avvertenza

$ _DtMessage 23 appears in an information dialog
23 Cancellare la tavolozza '%s'?\n

$ _DtMessage 25 is an error message that appears when you press the color
$ button and the color server is not running for some reason.
25 L'impostazione dei colori con la Gestione degli stili\n\
non funziona correttamente perché il server dei colori\n\
non è in esecuzione. Controllare il file $HOME/.dt/errorlog.

$ _DtMessage 26 can be caused by setting the resource *useColorObj to False
$ and then running dtstyle and trying to post the Color Dialog.
26 L'impostazione dei colori con la Gestione degli \n\
stili non funziona correttamente perché la risorsa\n\
'useColorObj' è impostata come False.

$ _DtMessage 27 is for the new ColorUse dialog when you press ok.
27 La nuova impostazione per il numero dei colori\n\
avrà effetto dalla prossima sessione.

$ _DtMessage 28 is a warning message that appears on Black and White systems
28 La tavolozza selezionata verrà\n\
applicata dalla prossima sessione.

$ _DtMessage 29 appears in a warning dialog
29 Esiste già una tavolozza denominata '%s'.\n\
La nuova tavolozza sostituirà la precedente.\n\
Confermare l'operazione?

$ _DtMessage 30 is the title of the Color dialog box.
30 Gestione degli stili - Colori

$ _DtMessage 31-34 are the ColorUse options
$ 31 Molto colorato
$ 32 Colore normale
$ 33 Poco colorato
34 Bianco e nero
35 Impostazioni predefinite

$ Dt Message 36 has been replaced by message 39
$ _DtMessage 36 is the title of the Color Use dialog
$ 36 DT Uso dei colori 

$ Dt Message 37 has been replaced by message 40
$ _DtMessage 37 is the Color Use button in the main Color Dialog 
$ 37 DT Uso dei colori...

$ _DtMessage 38 is an error message that appears when you press the color
$ button and there are no palette files to be found
38 L'impostazione dei colori con la Gestione degli stili\n\
non funziona correttamente perché non sono disponibili\n\
file di tavolozze. Controllare il file $HOME/.dt/errorlog.

$ DtMessage 39 is a replacement for DtMessage 36
$ DtMessage 39 is the title of the Color Use dialog
39 Numero di colori da utilizzare:

$ DtMessage 40 is a replacement for DtMessage 37
$ DtMessage 40 is the title of the Color Use dialog
40 Numero di colori...

$ _DtMessages 41-43 are replacements for Dtmessages 31-33
$ _DtMessage 41-43 are the ColorUse options
41 Più colori per il desktop
42 Più colori per le applicazioni
43 Massimo di colori per le applicazioni


$set 15
$ All of the messages from set 15 have been deleted from the message catalog


$set 16
$ _DtMessage set 16 is for the Color dialog
$ _DtMessage 1-10,13-17 from set 16 have been deleted from the message catalog

$ _DtMessage 11-12 form one error message that appears in an error dialog
11 Impossibile cancellare '%s'.\n
12 Per maggiori informazioni, aprire il file $HOME/.dt/errorlog.

$  MESSAGE 18-21 IN SET 16 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
18 Attenzione, la risorsa paletteDirectories contiene troppe directory,\n\
Il numero massimo è %d. 
$ _DtMessage 19 - could not open a palette directory
19 Impossibile aprire la directory %s. 
$ _DtMessage 20 - error opening the palette file
20 Impossibile aprire %s. 
$ _DtMessage 21 - error reading a palette file
21 %s è un file di tavolozza non ammesso.


$set 17
$ _DtMessage set 17 is for the Modify Color dialog
$ Messages 1-3,13,16 from set 17 have been deleted from the message catalog

$ Messages 4 through 12 are labels, buttons, and text visible in the
$ Modify Color dialog box
4 Anteprima del colore
5 Vecchio
6 Nuovo
7 Catturare dallo schermo
8 Editor dei colori 
9 Sfumatura
$ We recommend that you don't translate R,G,B which stands for Red, Green,
$ Blue unless there are standard single letter acronyms in your language.
10 R
11 G
12 B

$  MESSAGE 14-15 IN SET 17 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
14 Attenzione, impossibile localizzare il puntatore.\n
15 Attenzione, impossibile localizzare la tastiera.\n

$ _DtMessage 16 is the title of the Modify Color dialog box.
16 Gestione degli stili - Modifica dei colori


$set 18
$ _DtMessage set 18 is for the Window dialog
$ _DtMessage 1 is the title of the Window dialog box
1 Gestione degli stili - Finestra

$ Messages 2 through 5 are labels on the Window dialog.
2 Impostazioni predefinite
3 Controllo delle finestre
4 Controllo delle icone
$ Messages 5-8 have been replaced by messages 13-16 
$ 5 Attivare con spostamento del puntatore
$ 6 Attivare con clic del mouse
$ 7 Finestra attivata in primo piano
$ 8 Spostamento finestra intera
9 Usare riquadro delle icone
10 Disporre nello spazio di lavoro
11 Lasciare le finestre primarie in primo piano
12 Per applicare le modifiche è necessario \n\
riavviare la Gestione dello spazio di lavoro.\n\
Premere OK per riavviare la Gestione dello spazio \n\
di lavoro o Annullare per abbandonare le modifiche.

$ Messages 13-16 are replacements for messages 5-8
13 Attivare con spostamento del puntatore
14 Attivare con clic del mouse
15 Portare la finestra attiva in primo piano
16 Muovere tutta la finestra durante lo spostamento


$set 19
$ The set 19 contains messages for the I18N dialog

$ 
$ Messsages 1 to 11 are labels displayed in the dialog.
$ 

$ Message 1 is the title of the Dialog.
1 Gestione degli stili - Internazionalizzazione

$ Message 2 appears on the button used to reset the defailt values.
2 Valori predefiniti

$ Message 3 appears as the title of the title box containing Input
$ Methods information.
3 Metodo di input

$ Message 4 appears as the title of the title box containing Input
$ Method Start Mode choice.
4 Metodo di input iniziale

$ Message 5 appears as the title of the title box containing PreeditType
$ information.
5 Tipo di pre-editazione

$ Message 6 is the label presenting the server host names
6 Server Host ?????:

$ Message 7 is the label presenting the Input Method list
7 Metodo di input

$ Message 8 and 9 are the toggle buttons for the start mode selection:
$ "Ask at login" will cause dtimsstart to present the list of Input
$ Methods at next login; "Resume Current Input Method" will cause
$ dtimsstart to use the selected IM at next login, without asking the
$ user.
8  Scelta all'eseguire login

9 Riprendere metodo corrente

$ Message 10 is the label displayed on the button used to move up a
$ selected element in the preedit Type list (meaning at the previous
$ position).
10 Precedente

$ Message 11 is the label displayed on the button used to move down a
$ selected element in the preedit Type list (meaning at the next
$ position).
11 Successivo

$ 
$ Messages 20-21 are displayed in an warning dialog when the user
$ hits the OK button.
$ 

$ Message 20 appears when the user modifies the XmNpreeditType
$ resource value, and the resource *writeXrdbImmediate is True.
$ Do NOT localize the word XmNpreeditType.
20 Il nuovo valore XmNpreeditType avrà effetto\n\
dal prossimo avvio delle applicazioni.

$ Message 21 appears when the userselects a new Input Method Server,
$ or modifies the IMS Start Mode, or modifies the XmNpreeditType
$ resource value, and the resource *writeXrdbImmediate is False.
21 La selezione diventerà effettiva\n\
nella prossima sessione.

$ 
$ Messages 30 to 41 appear as errors in Error Dialog.
$ 

$ Do NOT localize the word HOME.
30 La variabile d'ambiente HOME non è stata definita.

$ Do NOT localize the word LANG.
31 La variabile d'ambiente LANG non è stata definita.

32 Il desktop non supporta questa versione locale.

33 Impossibile creare il file Selezione del metodo di input.

34 Impossibile trovare il file Selezione del metodo di input.

35 Impossibile salvare il file Selezione del metodo di input.

36 Host sconosciuto.

$ Do NOT translate dtimsstart.
37 dtimsstart introvabile.\n\
Controllare che il desktop sia installato\n\
correttamente.

38 Esecuzione remota è fallita sull'host\nselezionato.

39 Impossibile inizializzare l'esecuzione remota.

40 Impossibile trovare i dati sul metodo di input dall'host selezionato.

41 Errore sconosciuto



$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtstyle.msg /main/10 1996/11/19 19:29:20 rws $



