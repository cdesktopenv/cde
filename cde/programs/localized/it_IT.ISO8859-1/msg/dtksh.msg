$ $XConsortium: dtksh.msg /main/3 1996/07/24 18:52:57 drk $
$set 1
1 DtkshCvtStringToPixel: Il colore '%s' non è definito

$set 5
1 Sono stati specificati troppo parametri per le risorse; '%s' verrà ignorato
2 La risorsa è stata specificata in modo errato; usare la forma 'nome:valore' : %s
3 Il separatore delle risorse è ':', mentre è stato usato '=' : %s
4 Il toolkit è già stato inizializzato
5 Sintassi: XtInitialize variabile nome_applicazione classe_applicazione [argomenti ...]
6 Sintassi: %s variabile nome classe padre [arg:valore ...]
7 Sintassi: XtCreateApplicationShell variabile nome classe [arg:valore ...]
8 Sintassi: XtPopup widget GrabNone|GrabNonexclusive|GrabExclusive
9 Il tipo di trascinamento '%s' non è stato riconosciuto; verrà utilizzato 'GrabNone'
10 Sintassi: XtIsShell widget
11 Sintassi: XtMapWidget widget
12 Sintassi: XtUnmapWidget widget
13 Sintassi: %s variabile widget
14 Sintassi: XtScreen variabile widget
15 Sintassi: XtWindow variabile widget
16 Sintassi: XtCallCallbacks widget nome_callback
17 Sintassi: XtHasCallbacks variabile widget nome_callback
18 Sintassi: XtAddCallback widget nome_callback comando-ksh
19 Sintassi: XtRemoveCallback widget nome_callback comando-ksh
20 Il seguente è uno handle di callback non valido: %s
21 La callback specificata non è registrata
22 Sintassi: XtAddEventHandler widget maschera non_mascherabile comando-ksh
23 Sintassi: XtRemoveEventHandler widget maschera non_mascherabile comando-ksh
24 La gestione dell'evento specificato non è registrata
25 Sintassi: XtGetValues widget risorsa:variabile ...
26 Il seguente parametro della risorsa ha un formato non corretto: %s
27 Sintassi: XtSetValues widget arg:valore ...
28 Sintassi: XtAddWorkProc variabile comando
29 Sintassi: XtRemoveWorkProc Id_proc_lavoro
30 Il parametro Id_proc_lavoro deve essere un numero esadecimale: %s
31 Sintassi: XtAddTimeOut variabile millisecondi comando
32 Il parametro millisecondi deve essere maggiore di zero
33 Sintassi: XtRemoveTimeOut Id_intervallo
34 Il parametro Id_intervallo deve essere un numero esadecimale: %s
35 Sintassi: %s widget ...
36 Sintassi: XBell display volume
37 Sintassi: %s variabile display 
38 Il parametro display non è valido: %s
39 Sintassi: XDefineCursor display finestra Id_cursore
40 Il parametro Id_cursore non è valido: %s
41 Sintassi: XUndefineCursor display finestra
42 Sintassi: XtRemoveAllCallbacks widget nome_callback
43 Sintassi: XTextWidth variabile nome_font stringa
44 Sintassi: %s display tracciabile [argomenti ...]
45 Il parametro tracciabile non è valido: %s
46 Il parametro destinazione non è valido: %s
47 Nome della funzione grafica non riconosciuto: %s
48 Stile della linea non riconosciuto: %s
49 Opzione di disegno non riconosciuta: %s
50 Alcuni punti sono stati ignorati
51 Sintassi: XCopyArea display origine dest origineX origineY larghezza altezza destX destY [argomenti ...]
52 Impossibile convertire il tipo di risorsa '%s' in 'String'
53 Impossibile convertire il tipo di risorsa 'String' in '%s'
54 Sintassi: XtAddInput variabile [-r] descrittore_file comando_ksh
56 Sintassi: XtRemoveInput Id_input
57 Il parametro Id_input deve essere un numero esadecimale: %s
59 Sintassi: XFlush display
60 Sintassi: XSync display discard
61 Sintassi: XRaiseWindow display finestra
62 Sintassi: %s widget [True|False]
63 Sintassi: %s widget traduzioni
64 Sintassi: XtUninstallTranslations widget
65 Sintassi: XtParent variabile widget
66 Sintassi: XtLastTimestampProcessed variabile display
67 Sintassi: DtSessionRestorePath widget variabile_path file_di_salvataggio
68 Sintassi: DtSessionSavePath widget variabile_path variabile_file
69 Sintassi: DtShellIsIconified widget
70 Sintassi: DtSetStartupCommand widget comando
71 Sintassi: DtSetIconifyHint widget valore_booleano
73 Il parametro finestra_radice non è valido: %s
74 Sintassi: DtWsmSetCurrentWorkspace widget atomo
75 L'atomo dello spazio di lavoro non è valido: %s
78 Sintassi: DtWsmSetWorkspacesOccupied display finestra spazi_di_lavoro
79 Sintassi: DtGetHourGlassCursor variabile display
80 Sintassi: DtWsmAddCurrentWorkspaceCallback variabile widget comando-ksh
81 Sintassi: DtWsmRemoveWorkspaceCallback handle
82 Sintassi: XtNameToWidget variabile widget_di_riferimento nomi
83 Sintassi: DtDbLoad
84 Sintassi: DtDbReloadNotify comando-ksh
85 Sintassi: DtActionExists nome_azione
86 Sintassi: DtActionLabel variabile nome_azione
87 Sintassi: DtActionDescription variabile nome_azione
88 Sintassi: DtActionInvoke widget nome_azione parametri_term host_esec dir_contesto indicatore_uso comando-ksh ["FILE" nome_file] ...
89 Sintassi: DtDtsLoadDataTypes
90 Sintassi: DtDtsFileToDataType variabile nome_file
91 Sintassi: DtDtsFileToAttributeValue variabile nome_file nome_attr
92 Sintassi: DtDtsFileToAttributeList variabile nome_file
93 Sintassi: DtDtsDataTypeToAttributeValue variabile tipo_dati nome_attr nome_opz
94 Sintassi: DtDtsDataTypeToAttributeList variabile tipo_dati nome_opz
95 Sintassi: DtDtsFindAttribute variabile nome valore
96 Sintassi: DtDtsDataTypeNames variabile
97 Sintassi: DtDtsSetDataType variabile nome_file tipo_dati override
98 Sintassi: DtDtsDataTypeIsAction tipo_dati
99 Sintassi: ttdt_open variabile stato variabile2 nome_tool produttore versione sendStarted
100 Sintassi: tttk_Xt_input_handler Id_proc origine id
101 Il parametro origine deve essere un numero intero: %s
102 Il parametro Id deve essere un numero esadecimale: %s
103 Sintassi: ttdt_close stato Id_proc nuovo_id_proc sendStopped
104 Sintassi: ttdt_session_join variabile stato Id_sess handle_widget_shell join
105 Sintassi: ttdt_session_quit stato Id_sess pattern_sess quit
106 Il parametro pattern_sess non è valido: %s
107 Sintassi: ttdt_file_event stato op pattern send
108 Sintassi: ttdt_file_quit stato pattern quit
109 Sintassi: ttdt_Get_Modified percorso scope timeout
110 Sintassi: %s stato pathName scope timeout
112 Sintassi: tt_error_pointer variabile ttStatus
113 Sintassi: %s stato msg
114 Sintassi: %s stato msg Status_msg stringa_Status_msg destroy
115 Sintassi: ttdt_file_join variabile stato percorso scope join comando-ksh
116 Sintassi: tt_file_netfile variabile stato nome_file
117 Sintassi: tt_netfile_file variabile stato nome_file_rete
118 Sintassi: tt_host_file_netfile variabile stato host nome_file
119 Sintassi: tt_host_netfile_file variabile stato host nome_file_rete
120 Sintassi: XtIsSubclass widget classe
121 %s non è un nome di classe valido
122 Sintassi: XtClass variabile widget

$set 6
1 DtkshCvtWindowToString: Il valore 'from' ha una dimensione errata
2 DtkshCvtHexIntToString: Il valore 'from' ha una dimensione errata
3 DtkshCvtIntToString: Il valore 'from' ha una dimensione errata
4 DtkshCvtBooleanToString: Il valore 'from' ha una dimensione errata
5 DtkshCvtStringToWidget: Il valore 'from' ha una dimensione errata
6 DtkshCvtStringToWidget: Impossibile trovare un widget di nome '%s'
7 DtkshCvtStringToCallback: Il valore 'from' ha una dimensione errata
8 DtkshCvtCallbackToString: Il valore 'from' ha una dimensione errata
9 DtkshCvtWidgetToString: Il valore 'from' ha una dimensione errata
10 DtkshCvtWidgetToString: Impossibile trovare un nome per il widget
11 DtkshCvtWidgetClassToString: Il valore 'from' ha una dimensione errata
12 DtkshCvtWidgetClassToString: Classe del widget sconosciuta
13 DtkshCvtStringToWidgetClass: Nome della classe del widget sconosciuto
14 DtkshCvtScreenToString: Il valore 'from' ha una dimensione errata

$set 11
1 Impossibile trovare un campo denominato '%s' nella struttura '%s'
2 DtkshCvtCallbackToString: Errore di overflow del buffer interno di conversione
3 Errore di hashing per la risorsa '%s' nella classe di widget '%s'
4 Non è stato specificato il nome della funzione
5 Il comando non può essere eseguito; il toolkit non è stato inizializzato
6 La creazione del widget '%s' non è riuscita
7 Il parametro display non è valido: %s
8 Il seguente atomo di caratteristica non è valido: %s
9 La seguente risorsa non è definita per il widget '%s': %s
10 La posizione specificata non è ammessa: %s
11 Il font specificato non è valido: %s
12 Lo handle del widget '%s' non si riferisce a nessun widget esistente
13 Il widget deve essere di tipo 'command' 
14 Il widget deve essere di tipo 'mainWindow' 
15 Il widget deve essere di tipo 'scale' 
16 Il widget deve essere di tipo 'scrollBar' 
17 Il widget deve essere un widget o un gadget di tipo 'toggleButton' 
18 Il parametro finestra non è valido: %s
19 Impossibile allocare la memoria richiesta; uscita
20 Impossibile trovare il widget padre
21 Impossibile individuare il simbolo '%s'
22 Impossibile individuare il tipo '%s'
23 Impossibile analizzare la dichiarazione '%s'; utilizzando 'unsigned long'
24 Tipo di processo figlio sconosciuto: %s
25 Flag dell'opzione non riconosciuto: %s
26 Sintassi: %s widget posizione
27 Sintassi: %s widget tempo
28 Sintassi: %s widget
29 errore dtksh 
30 avvertenza dtksh 
31 Sintassi: %s display finestra
32 Sintassi: %s display finestra variabile
33 Sintassi: %s display rootWindow variabile
34 Il parametro timeout non è valido: %s
35 Il parametro msg non è valido: %s
36 Il parametro pattern non è valido: %s

$set 14
1 Errore interno della tabella hash durante l'inizializzazione della classe del widget; uscita
2 Impossibile trovare una classe di widget denominata '%s'
3 L'identificatore '%s' non è un handle di widget valido
4 Impossibile trovare la classe del widget 
5 Sintassi: DtLoadWidget widgetClassName widgetClassRecordName
6 Impossibile trovare un record della classe del widget denominato '%s'
7 Errore interno della tabella hash durante l'inizializzazione della classe del widget '%s'
8 \n%sRISORSE PER %s%s%s:\n
9 CONSTRAINT 
10 R
11 M
12 S
13 %-15s %-6s %-6s %-18s %-6s %s\n
14 VARIABILE AMB    CLASSE PADRE HANDLE             NOME STATO\n
15 \nSintassi:\tDtWidgetInfo [handle_widget]\n\tDtWidgetInfo -r <handle_widget|classe>\n\tDtWidgetInfo -R <handle_widget|classe>\n\tDtWidgetInfo -c [classe]\n\tDtWidgetInfo -h [handle_widget]

$set 15
1 Impossibile inizializzare il Toolkit
2 La seguente risorsa non è stata impostata alla creazione\ndel widget. Usare XtSetValues dopo la creazione: %s
3 Sintassi: %s variabile padre nome [argomento:valore  ...]
4 Sintassi: DtHelpReturnSelectedWidgetId variabile widget variabile
5 Sintassi: DtHelpSetCatalogName catalogo
6 Sintassi: DtHelpQuickDialogGetChild variabile widget_quickHelp figlio
7 Il widget deve essere di tipo 'quickHelp' 
8 Il widget deve essere di tipo 'list' 
9 Sintassi: %s widget posizione item
10 Sintassi: %s widget posizione itemList
11 Sintassi: %s widget item
12 Sintassi: XmListDeleteItemsPos widget count posizione
13 Sintassi: XmListDeleteItems widget itemList
14 Sintassi: XmListDeletePositions widget posizioni
15 Sintassi: XmListGetKbdItemPos variabile widget
16 Sintassi: XmListItemExists widget item
17 Sintassi: XmListItemPos variabile widget item
18 Sintassi: XmListPosSelected widget posizione
19 Sintassi: XmListPosToBounds widget posizione variabile variabile variabile variabile
20 Sintassi: %s widget posizione flag_notify
21 Sintassi: %s widget item flag_notify
22 Sintassi: XmListSetAddMode widget valore_booleano
23 Sintassi: XmListSetKbdItemPos widget posizione
24 Sintassi: XmMainWindowSetAreas finestra_principale menu comando scroll_orizz scroll_vert lavoro
25 Lo handle 'mainWindow' è NULLO
26 Sintassi: %s variabile finestra_principale
27 Sintassi: XmProcessTraversal widget direzione
28 Direzione dello spostamento sconosciuta: %s
29 Sintassi: XmInternAtom variabile display nome onlyIfExists
30 Sintassi: XmGetAtomName variabile display atomo
31 L'atomo specificato non è valido: %s
32 Sintassi: XmGetColors widget sfondo primo_piano ombr_sup ombr_inf select
33 Il pixel di sfondo non è valido: %s
34 Sintassi: XmUpdateDisplay widget
35 Sintassi: %s widget protocollo [protocollo ...]
36 L'atomo specificato non è valido: %s
37 Sintassi: XmAddWMProtocolCallback widget protocollo comando-ksh
38 Sintassi: XmRemoveWMProtocolCallback widget protocollo comando-ksh
39 Sintassi: XmMenuPosition menu evento
40 L'evento specificato non è valido: %s
41 Sintassi: XmCommandAppendValue widget_comando stringa
42 Sintassi: XmCommandError widget_comando messaggio_errore
43 Sintassi: XmCommandSetValue widget_comando comando
44 Sintassi: XmCommandGetChild variabile widget_comando figlio
45 Sintassi: XmMessageBoxGetChild variabile widget_comando figlio
46 Il widget deve essere di tipo 'messageBox' 
47 Sintassi: XmFileSelectionBoxGetChild variabile widget figlio
48 Il widget deve essere di tipo 'file selection box' 
49 Sintassi: XmSelectionBoxGetChild variabile widget figlio
50 Il widget deve essere di tipo 'selection box' 
51 Sintassi: XmScaleGetValue widget_scala variabile
52 Sintassi: XmScaleSetValue widget_scala valore
53 Il valore di scala specificato non è valido: %s
54 Sintassi: XmScrollBarGetValues barra_scorrimento variabile variabile variabile variabile
55 Sintassi: XmScrollBarSetValues barra_scorrimento valore dimensione_guida incremento incremento_pagina notifica
56 Il valore specificato non è valido: %s
57 La dimensione della guida di scorrimento specificata non è ammessa: %s
58 L'incremento specificato non è valido: %s
59 L'incremento di pagina specificato non è valido: %s
60 Sintassi: XmScrollVisible finestra_scorrim widget margine_sinistro_destro margine_alto_basso
61 Il widget deve essere di tipo 'scrolledWindow' 
62 Il widget che deve essere reso visibile non esiste.
63 Il margine sinistro/destro specificato non è valido: %s
64 Il margine superiore/inferiore specificato non è valido: %s
65 Sintassi: %s widget stato notifica
66 Sintassi: catopen variabile nome_cat
67 Sintassi: catclose Id_cat
68 Sintassi: catgets variabile Id_cat num_set num_msg msg_dft
69 Il widget deve essere di tipo 'text' o 'textField' 
70 Sintassi: %s variabile widget
71 Sintassi: %s widget valore_booleano
72 Il parametro specificato non è valido: %s
73 Sintassi: %s widget righe
74 Sintassi: %s widget lungh_max
75 Sintassi: %s widget stringa
76 Sintassi: %s widget variabile variabile
77 Sintassi: %s widget posizione stringa
78 Sintassi: %s widget posizione variabile variabile
79 Sintassi: %s widget fromPosition toPosition stringa
80 La posizione 'from' specificata non è ammessa: %s
81 La posizione 'to' specificata non è ammessa: %s
82 Sintassi: %s widget prima_posizione ultima_posizione tempo
83 La prima posizione specificata non è ammessa: %s
84 L'ultima posizione specificata non è ammessa: %s
85 Il tempo specificato non è valido: %s
86 Sintassi: %s variabile widget x y
87 La posizione x specificata non è ammessa: %s
88 La posizione y specificata non è ammessa: %s
89 Sintassi: %s widget pos_sinistra pos_destra modo
90 La posizione sinistra specificata non è ammessa: %s
91 La posizione destra specificata non è ammessa: %s
92 Sintassi: %s widget inizio stringa direzione variabile
93 La posizione di inizio specificata non è ammessa: %s
94 Sintassi: XmListGetSelectedPos variabile widget
95 Sintassi: XmListGetMatchPos variabile widget item
96 Sintassi: XmOptionLabelGadget variabile widget
97 Sintassi: XmOptionButtonGadget variabile widget
98 Sintassi: XmGetVisibility variabile widget
99 Sintassi: XmGetTearOffControl variabile widget
100 Sintassi: XmGetTabGroup variabile widget
101 Sintassi: XmGetPostedFromWidget variabile widget
102 Sintassi: XmGetFocusWidget variabile widget
103 Sintassi: XmFileSelectionDoSearch widget maschera_directory

$set 16
1 DtkshCvtNamedValueToString: Nessun elemento corrispondente
2 DtkshCvtStringToNamedValue: Impossibile convertire la stringa '%s'


$ The following messages are for ksh93 itself.  The message numbers must
$ match those specified in the MsgStr array allmsgs in 
$ ksh93/src/cmd/ksh93/sh/init.c, as must the set number.
$set 25

1  Fatto 
3  In esecuzione
4  [-n] [arg...]
5  [arg...]
6  [dir] [lista]
7  [lavoro...]
8  [n]
9  [nome [percorso] ]
10  [nome]
11  [livello superiore] [base]
12  espr...
13  formato [arg...]
14  è una funzione 
15  è una parola chiave
16  è una shell integrata 
17  è una funzione esportata 
18  è una funzione indefinita
19  nome [arg...]
20  stringa_opz nome [argomenti...]
21  secondi
23 ${HOME:-.}/.profile
24 %c: carattere non valido nell'espressione - %s
25 %c: specifica di formato non valida
26 %d-%d: fuori dominio
27 %d: versione dello script binario errata 
28 %s è un alias per 
29 %s è un alias esportato per 
30 %s mancante
31 %s base sconosciuta 
32 %s: ':' richiesto per l'operatore '?' 
33 %s: Ambiguo
34 %s: Gli argomenti devono essere %job o id_di_processo
35 %s: alias non trovato\n
36 %s: errore di sintassi aritmetico
37 %s: l'assegnazione richiede lvalue
38 %s: numero di file errato
39 %s: formato errato
40 %s: numero errato 
41 %s: opzione(i) errate
42 %s: sostituzione errata
43 %s: trap errata 
44 %s: creazione non riuscita
45 %s: esecuzione non riuscita
46 %s: apertura non riuscita
47 %s: divisione per zero
48 %s: eccezione di dominio
49 %s: errore di %s
50 %s: il file esiste già
51 %s: nome di funzione errato
52 %s: nome alias non valido
53 %s: funzione di disciplina non ammessa
54 %s: nome di esportazione non valido
55 %s: nome della funzione non valido
56 %s: nome non valido
57 %s: espressione regolare non ammessa
58 %s: auto-riferimento non valido
59 %s: uso non valido di:
60 %s: nome della variabile non valido
61 %s: non è un identificatore
62 %s: sola lettura
63 %s: etichetta non implementata
64 %s: limite superato
65 %s: sono richieste più istruzioni
66 %s: nessun processo padre
67 %s: nessun nome di riferimento
68 %s: non trovato
69 %s: non implementato
70 %s: gli operandi sono incompatibili
71 %s: eccezione di overflow 
72 %s: parametro non impostato
73 %s: parametro nullo o non impostato
74 %s: ricursione troppo profonda
75 %s: la variabile di riferimento non può essere un array
76 %s: richiede un argomento percorso
77 %s: vietato
78 %s: eccezione di singolarità
79 %s: subscript fuori dominio
80 %s: parentesi mancante
81 %s: funzione sconosciuta 
82 %s: versione locale sconosciuta 
83 %s: operatore sconosciuto 
84 %s: nome di segnale sconosciuto
85 %s: provoca un loop
86 (salvataggio in un core)
87 -c richiede un argomento 
88 -e - richiede un singolo argomento
89 /vpix
90 <comando sconosciuto>
91 ACE#?F#?H:[nome]L#?R#?Z#?fi#?[base]lnprtux [nome=[valore]...]
92 AE#?F#?HL#?R#?Z#?fi#?[base]lnprtux [nome=[valore]...]
93 Interruzione
94 Ad:[delim]prst#[timeout]u#[filenum] [nome...]
95 Chiamata di allarme
96 Specifica errata del nodo radice 
97 Chiamata al sistema errata
98 Pipe interrotta
99 Errore di bus 
100 Impossibile avviare il controllo del lavoro
101 Impostazioni correnti dell'opzione
102 Segnale DIL 
103 Morte del processo figlio
104 DircabefhkmnpstuvxCI:[file]o:?[opzione]A:[nome] [arg...]
105 DircabefhkmnpstuvxCo:?[opzione]A:[nome] [arg...]
106 Trap EMT 
107 Limite di tempo della CPU superato
108 Dimensione del file superata
109 Eccezione in virgola mobile 
110 HSacdfmnstv [limite]
111 Interruzione del collegamento
112 Segnale I/O 
113 Istruzione errata
114 Interruzione
115 Interrotto da kill
116 LP [dir] [modifica]
117 Errore di memoria
118 Processo di migrazione
119 Nessun controllo sul job 
120 Interruzione telefonica
121 Allarme di polling 
122 Interruzione di corrente
123 Allarme di tempo nella creazione del profilo
124 Uscita
125 Risorse perdute
126 Ritorno al vecchio driver tty...
127 S [maschera]
128 SIGAPOLLO
129 Etichetta di sicurezza modificata
130 Interrupt del socket 
131 Suono completato
132 Interruzione (segnale)
133 Interruzione (input tty)
134 Processo interrotto continuato
135 Interruzione
136 Interruzione (output tty)
137 Passaggio al nuovo driver tty...
138 Il sistema andrà in crash 
139 Processo abbandonato
140 Trap Trace/BPT 
141 Versione non riconosciuta
142 Usare 'exit' per chiudere questa shell
143 Segnale utente 1
144 Segnale utente 2
145 Versione non definita
146 Allarme tempo virtuale
147 Modifica della dimensione della finestra
148 Sono presenti job in esecuzione
149 Sono presenti job interrotti
150 [_[:alpha:]]*([_[:alnum:]])
151 \n@(#)Versione 12/28/93\0\n
152 \n@(#)Versione M-12/28/93\0\n
153 \nreale
154 \r\n\007shell avrà un timeout tra 60 secondi per inattività
155 a nome...
156 a:c [comando[argomenti...] ]
157 afpv nome...
158 allarme %s %.3f\n
159 allarme -r %s +%.3g\n
160 è richiesto un argomento 
161 directory errata
162 numero di file errato
163 sostituzione errata 
164 impossibile accedere alle directory precedenti
165 impossibile creare la pipe
166 impossibile creare il file temporaneo
167 impossibile eseguire un fork
168 impossibile individuare %s
169 impossibile impostare %s
170 impossibile impostare l'allarme
171 sono richieste le condizioni 
172 dsf:[libreria] [nome...]
173 e:[editor]lnrsN# [primo] [ultimo]
174 fine del file
175 f:[formato]nprsu:[num_file] [arg...]
176 fnv nome...
177 hist -e "${VISUAL:-${EDITOR:-vi}}" 
178 impossibile aprire il file history 
179 errore di sintassi 
180 argomento non valido di tipo %c
181 è una versione shell integrata di 
182 è un tracked alias per 
183 kill
184 riga %d: $ non è preceduto da \\
185 riga %d: %c compresi tra ${} devono essere tra virgolette
186 riga %d: %s etichetta sconosciuta
187 riga %d: %s comprese tra [[...]] obsolete, usare ((...))
188 riga %d: '=' obsoleto, usare '=='
189 riga %d: -a è obsoleto, usare -e
190 riga %d: \\ prima di %c riservati per un uso futuro
191 riga %d: `...` è obsoleto, usare $(...)
192 riga %d: usare un carattere di escape prima di %c per evitare ambiguità
193 riga %d: etichetta %s ignorata
194 riga %d: racchiudere %c tra virgolette per evitare ambiguità
195 riga %d: insieme %s obsoleto
196 riga %d: sono richiesti gli spazi per le sottoshell annidate
197 riga %d: usare le parentesi graffe per evitare ambiguità con $id[...]
198 riga %d: usare spazi o tabulazioni per separare gli operatori %c e %c
199 ln#[num_segn]s:[nome_segn] segn...
200 shell di setuid/setgid non autorizzate al login
201 mappatura
202 newline
203 nlp [job...]
204 history del file mancante
205 nessun processo di interrogazione 
206 lavoro non trovato
207 processo non trovato
208 non supportato
209 off
210 on
211 superato il limite di file aperti 
212 memoria esaurita
213 p [azione condizione...]
214 p [nome[=valore]...]
215 parametro non impostato
216 autorizzazione negata
217 il processo esiste già
218 ptx [nome=[valore]...]
219 pvV nome [arg]...
220 r [nome_var secondi]
221 errore di sintassi alla riga %d: `%s' %s
222 errore di sintassi alla riga %d: etichetta doppia %s
223 errore di sintassi: `%s' %s
224 sys
225 tempo scaduto in attesa dell'input
226 inatteso
227 universo non accessibile
228 illimitato 
229 mancante 
230 utente
231 versioni
232 scrittura su %d non riuscita
233 sono presenti messaggi in $_
234 zero byte

