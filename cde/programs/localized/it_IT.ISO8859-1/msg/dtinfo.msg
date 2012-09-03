
$quote "
$ $TOG: dtinfo.msg /main/25 1999/09/20 13:34:13 mgreess $
$ 
$ (c) Copyright 1996 Digital Equipment Corporation.
$ (c) Copyright 1996 Hewlett-Packard Company.
$ (c) Copyright 1996 International Business Machines Corp.
$ (c) Copyright 1996 Sun Microsystems, Inc.
$ (c) Copyright 1996 Novell, Inc.
$ (c) Copyright 1996 FUJITSU LIMITED.
$ (c) Copyright 1996 Hitachi.
$ 
$ ******************************************************************************
$ 

$set	1
$ 
$ messages specific to message-catalog manager
$ 
1 "Il messaggio predefinito non si trova nel catalogo."


$set	2
$ 
$ messages specific to UAS/DtSearch modules
$ 
1 "La funzione di ricerca ``%s'' non è supportata."
$ "

2 "frase citata"
3 "peso"
4 "prossimità"
$       5       "xor" == Exclusive OR (Boolean)
5 "xor"


$set	4
$ 
$ messages specific to booklist window
$ 
3 "Dtinfo: Elenco libri BiblioInfo"
63 "Benvenuti in Dtinfo!"
65 "Sezione corrente"


$set	5
$ 
$ messages specific to browser window
$ 
1 "Dtinfo: Browser"
2 "Per una descrizione della destinazione del collegamento, fare clic e mantenere sul collegamento."
3 "Biblioteca delle informazioni"

$       4       "The locator for this section is %s"
$       This message is the result of the "Show Locator" operation.
4 "Il localizzatore di questa sezione è %s"

$ line-break rules (reserved)
5 ""
6 ""

7 "Il testo selezionato è inserito in modo dinamico\n\
e non è sufficiente per creare un segnalibro.\n\
Selezionare alto testo vicino il punto che si desidera\n\
segnalare oppure includere più del testo circondante."

$set	6
$ 
$ messages specific to add information library dialog
$ Note: "Infolib" is a CDE glossary term.
$ 

1 "Dtinfo: Aggiunta Biblioteca delle informazioni"

$ 2     "Please enter an absolute path to the Infolib."
$       (Path must exist and include "/".)
2 "Inserire un percorso assoluto alla BiblioInfo."

$ 3     "Infolib open failed. Check the path specified."
$	(Path passed basic format check, but could not open target found.)
3 "L'apertura di BiblioInfo è fallita. Controllare che il percorso specificato sia corretto."

$ 4     "Infolib open failed. Check entry syntax or Infolib integrity."
$       (An unidentified error occurred on attempt to open.)
4 "L'apertura di BiblioInfo è fallita. Controllare la sintassi del comando e l'integrità della BiblioInfo."

$ 5     "Infolib specification format error."
5 "Errore nella specificazione del formato BiblioInfo."

$ 6     "No Infolib specified."
6 "Nessuna BiblioInfo specificata."

$ 7     "No Infolib found or specified."
7 "BiblioInfo non specificata o introvabile."

$ 10    "Dtinfo: Infolib Selection"
$       (For title to the File Selection Dialog)
10 "Dtinfo: Selezione BiblioInfo"

$ 11    "No Infolibs present"
$       (This string will be shown in a directory listing where there
$        are no Infolibs found in the directory.)
11 "BiblioInfo assente"


$set	7
$ 
$ dtinfo GUI label strings
$ 
1 "File"
2 "F"
4 "Visualizzare"
5 "V"

$	6	"Clone"
$ "Clone" is applied to the selected entry in the booklist, and
$ has the effect of opening another (secondary) window with a
$ copy of the booklist hierarchy below the selection, and rooted
$ (based) at the selected item.
$ [Currently, this functionality is deferred beyond CDE 2.0.]
6 "Duplicare"
$	7	"N" for "N" in "Clone"
7 "D"

8 "Stampare"
9 "S"
10 "Stampare..."
11 "t"
12 "Chiudere"
13 "C"
14 "Uscire"
15 "U"
16 "Editare"
17 "E"
18 "Copiare"
19 "p"
20 "Aggiungere..."
21 "A"
22 "Eliminare"
23 "E"
24 "Opzioni"
25 "O"
26 "Preferenze..."
27 "P"
28 "Auto Track"
29 "A"
30 "Mostrare barra degli strumenti"
31 "b"
32 "Mostrare l'area di ricerca"
33 "A"
34 "Finestre"
35 "i"
36 "Mappa grafica"
37 "M"
38 "Cronologia delle sezioni"
39 "s"
40 "Elenco dei segnalibri"
41 "E"

$	42	"Search History " presents a dialog listing
$ a history of prior search results, as provided in the Search
$ Results Dialog.
42 "Cronologia delle ricerche"
43 "C"
44 "Editor delle ricerche"
45 "r"
46 "Editor dell'ambito"
47 "a"
48 "Aiuto"
49 "u"
50 "Sull'elemento..."
51 "e"
59 "Ricerca in:"
61 "Editor dell'ambito"

$  Messages 62-77 for "Browser" pulldown menu entries
$ 
62 "Browser"
63 "B"
64 "Nuova finestra"
65 "N"
66 "Aprire localizzatore..."
67 "A"
68 "Mostrare localizzatore"
69 "M"
70 "Stampare"
71 "S"
72 "Stampare..."
73 "t"
74 "Chiudere"
75 "C"
76 "Uscire"
77 "U"

$	78	"On Selection", where selection is a text
$ selection that will be used directly as a search query string
78 "Testo selezionato"
79 "T"

80 "Editor delle ricerche"
81 "E"
82 "Editor dell'ambito"
83 "a"
84 "Cronologia delle ricerche"
85 "r"
86 "Azzerare risultati della ricerca"
87 "z"
88 "Indietro (Cronologia)"
89 "d"
90 "In avanti (Cronologia)"
91 "v"
92 "Cronologia..."
93 "C"
94 "Sezione precedente"
95 "p"
96 "Sezione successiva"
97 "s"

$   Messages 98-101 are for traversal of the document view in order
$ to present and highlight the Previous or Next "search hit" (if any)
$ in the section. Presumably, all search hits have already been
$ highlighted to some degree, and this will add the equivalent of a
$ keyboard traversal highlight to the appropriate one when brought
$ into view.
98 "Ricorrenza precedente"
99 "p"
100 "Ricorrenza successiva"
101 "s"

102 "Ricerca"
103 "R"

$	104	"Go" is a pulldown menu label, where menu items are
$ various methods by which the user may "go" to a specific document.
104 "Andare"
105 "n"

$   Message 106-119 are for the "Marks" pulldown menu
106 "Segnalibri"
107 "S"
108 "Creare segnalibro"
109 "s"
110 "Creare nota..."
111 "n"

$	112	"List Marks" means to show all saved Marks as a list
$ in a separate dialog window.
112 "Elencare segnalibri"
113 "l"

114 "Editare selezione..."
115 "E"
116 "Spostare selezione"
117 "S"
118 "Cancellare selezione"
119 "C"

$   Messages 120-129 are for "Options" pulldown menu entries
120 "Opzioni"
121 "O"
122 "Preferenze..."
123 "P"

$	124	"Detach Graphic" removes a selected graphic from the
$ document flow and places it in its own window. A "placeholder"
$ remains in the document where the graphic originally appeared.
124 "Staccare grafico"
125 "S"

126 "Mostrare barra degli strumenti"
127 "b"
128 "Mostrare area della ricerca"
129 "a"

$   Messages 130-135 are for "Windows" pulldown menu entries
130 "Finestre"
131 "F"
132 "Elenco libri"
133 "l"
134 "Mappa grafica"
135 "M"

$   Messages 136-142	are believed to be *unused*/obsolete
136 "Autore"
137 "A"
138 "Get Locator"
139 "Debug"
140 "D"
141 "Generate Fake Infobase List"
142 "Command Processor"

$   Messages 143-158 are Help pulldown menu entries, and
$ should of course read the same across all CDE applications,
$ within a localization. They always mean the same thing:
$ provide "help On []"
143 "Aiuto"
144 "A"
145 "Sull'elemento..."
146 "e"
147 "Sulla finestra"
148 "f"
149 "Sull'applicazione"
150 "p"
151 "Indice analitico"
152 "I"
153 "Aiuto sull'aiuto"
154 "u"
155 "Sui tasti"
156 "t"
157 "Sulla versione"
158 "v"

$	159	"Retain" is believed to be unused
159 "Mantenere"

160 "Percorso BiblioInfo:"
161 "Ok"
162 "Annullare"
163 "Nome"
164 "Note"
165 "Visualizzare"

$	166	"Panner" is a graphical user interface control for
$ selecting that part of a larger area which the associated view
$ window is to display.
166 "Panner"

167 "Ricollegare"
168 "Visualizzare sezione"

$   Messages 169-182 are for the window which is used to present a
$ detached graphic.
$	169	Fit >graphic< to window
169 "Grafico alla finestra"
$	170	Fit >window< to graphic
170 "Finestra al grafico"
171 "25 %"
172 "50 %"
173 "100 %"
174 "125 %"
175 "150 %"
176 "200 %"
$	177	"Custom" value entry for re-scaling of graphic
177 "Personalizzata..."

$   Messages 178-182 refer to where the panner is to be placed within
$ the detached graphic dialog window. Is a "panner" masculine or
$ feminine ? "Beats" me ;-).
178 "Nessuno"
179 "In alto a sinistra"
180 "In alto a destra"
181 "In basso a sinistra"
182 "In basso a destra"

183 "Cancellare"
184 "Libro"
185 "Sezione"
186 "Preferenze per"
187 "Browser"
188 "Mappa grafica"
189 "Ricerca"
190 "Dimensioni del Browser"
191 ". . ."
192 "Dimensione del font"

$	193	"Pin []" is a verb
193 "Fissare la finestra"
194 "Registrare un bug"
195 "Dimensioni della mappa"
196 "Aggiornamento automatico"
197 "Voci nella Cronologia delle sezioni"
198 "Voci nella Cronologia delle ricerche"
199 "Numero massimo di ricorrenze"
200 "Visualizzazione autom. della prima ricorrenza"
201 "Applicare"
202 "Ripristinare"
203 "Sì"
204 "No"
205 "Numero di sezioni"
206 "Selezionate:"
207 "Da stampare:"
208 "Errore."
209 "Elementi da stampare"
210 "Sezioni"
211 "Gerarchia"
212 "Nome dell'ambito"
213 "Senza nome"
214 "Libri"
215 "Componenti"
216 "Nuovo"
217 "Salvare"
218 "Rinominare"
219 "Trovati:"
220 "Ambito:"
221 "Oggetto della ricerca:"
222 "Specificare un localizzatore unico:"
223 "Azzerare"
224 "Tagliare"
225 "Incollare"
226 "Raggruppare"
227 "Separare"
228 "Annullare"
229 "Ripetere"
230 "Ricercare"
231 "Azzerare tutto"

$   Messages 232-242 relate to operators available in a query
$ expression, and appear in the Query Editor on option menu buttons.
$       232 "Matches" means the entry in the adjacent entry field will
$           be compared exactly to find a search index database entry.
232 "Corrisponde a"
233 "Non corrisponde a"
234 "Inizia con"
235 "Non inizia con"
$	236	Boolean operator OR
236 "O"
$	237	"Xor" is for Boolean logic "Exclusive OR"
237 "XO"
$	238	Boolean operator AND
238 "E"
239 "Vicino"
240 "Prima di"
241 "Peso"
242 "Prossimità"
243 "Ambito"
244 "Sì"
245 "No"
246 "Ricorrenze"
247 "Introduzione"
248 "I"
249 "Attività"
250 "A"
251 "Riferimenti"
252 "R"
253 "Uso dell'aiuto"
254 "U"
255 "Informazioni sulla Gestione informazioni"
256 "I"
257 "Panoramica"
258 "P"
259 "o"
260 "Cronologia"
261 "Elenco cronologico"
262 "Questa sezione non è più  disponibile."
263 "Fare clic su un grafico per staccarlo."
264 "Alzare grafico"
265 "Ricollegare grafico"



$       270  "..." is a button label indicating a list from which an
$            entry may be selected to fill a preceeding text field.
$            In this case, the user may enter a path into a text field
$            or activate the button with "..." to be presented with
$            a File Selection Box. The object is to select an Infolib
$            path.
270 "..."



$set	8
$ 
$ dtinfo quick help messages (to be exhibited on status display area)
$ 
1 "Andare alla sezione precedente nella cronologia"
2 "Andare alla sezione successiva nella cronologia"
3 "Andare alla sezione precedente nel documento"
4 "Andare alla sezione successiva nel documento"	

$ A search "match" is the same as a search "hit". "Match" is more
$ descriptive.
5 "Andare alla ricorrenza precedente"
6 "Andare alla ricorrenza successiva"

7 "Stampare la sezione corrente"
8 "Visualizzare l'elenco libri"
9 "Visualizzare la mappa grafica"
10 "Fissare (liberare) questa finestra, per impedire (permettere) l'uso da nuovi documenti"
11 "Limitare la ricerca con l'Editor dell'ambito"
52 "Visualizzare la sezione selezionata"
53 "Visualizzare la sezione selezionata in un Elenco libri separato"
54 "Stampare la sezione selezionata"
55 "Aprire ulteriori biblioteche"
56 "Chiudere la biblioteca selezionata"
57 "Eseguire una ricerca con l'Editor delle ricerche"
58 "Visualizzare un elenco di segnalibri e di note"
60 "Specificare l'ambito della ricerca"
62 "Limitare la ricerca con l'Editor dell'ambito"
64 "Ricercare %s"


$set	9
$ 
$ messages specific to bookmark edit dialog
$ 
1 "Dtinfo: Editor dei segnalibri"
2 "Aggiornamento dei segnalibri è fallito."


$set	10
$ 
$ messages specific to detached graphic window
$ 
1 "Dtinfo: Grafico staccato"
2 "Grafico staccato"
3 "Illustrazione raster (non ridimensionabile)"
4 "Illustrazione vettoriale (ridimensionabile)"
5 "Ridimensionando l'illustrazione..."
$ 
$ Dtinfo*SetZoom
$ 
6 "Specificare una percentuale di scala:"
$ 
$ Dtinfo*SetZoomTitle
$ 
7 "Dtinfo: Impostazione scala"
$ 
8 "Dtinfo: "
9 "Illustrazione raster (ridimensionabile)"


$set	11
$ 
$ messages specific to mark list window
$ 

$	1 is the title of a window containing a list of the
$ user's bookmarks
1 "Dtinfo: Elenco segnalibri d'utente"


$set	12
$ 
$ messages specific to mark selection dialog
$ 
$   Messages 1 & 2: "Select" and "Edit" are verbs
1 "Dtinfo: Selezionare segnalibro"
2 "Dtinfo: Editare segnalibro"


$set	13
$ 
$ messages specific to section history window
$ 
1 "Dtinfo: Cronologia delle sezioni"


$set	14
$ 
$ messages specific to preferences window
$ 
1 "Dtinfo: Preferenze"
2 "Dtinfo: Geometria"


$set	15
$ 
$ messages specific to printing
$ 
1 "Dtinfo: Stampa"
$          Messages 2 - 5 are obsolete, and have been replaced by 12
2 Impossibile trovare la Gestione del dialogo di stampa PDM nell'ambiente corrente
3 Impossibile avviare la Gestione del dialogo di stampa PDM
4 Gestione del dialogo di stampa non è autorizzata a collegarsi al display Video
5 Gestione del dialogo di stampa non è autorizzata a collegarsi al display Stampa
6 Impossibile visualizzare la Gestione del dialogo di stampa
7 Impossibile copiare i dati della stampa predefinita
8 Errore durante la stampa su file
9 Lavoro di stampa già  in esecuzione
10 Dtinfo: Stato della stampa
11 Lavoro di stampa fallito.\n\nIl server di stampa X è temporaneamente senza risorse.
$       Message 12 is used to handle errors from the PDM
12	Errore Gestione del dialogo di stampa (PDM) - Impostazione non riuscita.

$set	16
$ 
$ messages specofic to search scope editor window
$ 
$	1	Edit the Scope for for use in a Search
1 "Dtinfo: Editor dell'ambito della ricerca"
$ 
$ Dtinfo*SaveScope
$ 
2 "Inserire un nome per il nuovo ambito:"
$ 
$ Dtinfo*SaveScopeTitle
$ 
3 "Dtinfo: Salvataggio dell'ambito"
$ 
4 "Tutte le BiblioInfo"


$set	17
$ 
$ messages specific to search result window
$ 
$	1	"Search Results" is for the results of a search
1 "Dtinfo: Risultati della ricerca"


$set	18
$ 
$ messages specific to open locator dialog, and locator
$ input formats in general.
$ 
$ 1	"Dtinfo: Open locator
1 "Dtinfo: Apertura del localizzatore"

$ 2     "Locator specification format error."
2 "Errore nella specificazione del formatto del localizzatore"

$ 3     "Locator resolution failed."
3 "Impossibile risolvere il collegamento al localizzatore."

$ 4     "Locator resolution failed within the loaded infolibs."
4 "Impossibile risolvere il collegamento al localizzatore nelle biblioteche caricate."

$ 5     "Document not found."
5 "Documento introvabile"

$ 6     "No locator value provided."
6 "Non è stato fornito un valore per il localizzatore."


$set	19
$ 
$ messages specific to query editor window
$ 
1 "Dtinfo: Editor delle ricerche"


$set	20
$ 
$ dtinfo general messages
$ 
$ 
$ Dtinfo*PixmapAllocationError
$ 
1 "Memoria insufficiente nel server per\n\
comporre il grafico."
$ 
$ Dtinfo*AbandonChanges
$ 
2 "Perdere le modifiche?"
$ 
$ Dtinfo*NoHelp
$ 
3 "Informazioni dell'aiuto introvabili."
$ 
$ Dtinfo*NoSelection
$ 
4 "Non c'è nessun testo selezionato.\n\
Selezionare del testo e riprovare."
$ 
$ Dtinfo*AbandonBookmarks
$ 
5 "I segnalibri aperti contengono modifiche che non sono state\n\
salvate. Uscire senza salvarle?"
$ 
$ Dtinfo*ConfirmQuit
$ 
6 "Si desidera uscire da Dtinfo?"
$ 
$ Dtinfo*BadTocHierarchy
$ 
7 "Registrare un bug"
$ 
$ Dtinfo*LinkToBookTitle
$ 
8 "Collegamento a %s: %s"
$ 
$ Dtinfo*LinkToTitle
$ 
9 "Collegamento a %s"
$ 
$ Dtinfo*MarkCreateFailed
$ 
10 "Dtinfo non è in grado di creare il segnalibro."
$ 
$ Dtinfo*MarkDeleteFailed
$ 
11 "Dtinfo non è in grado di cancellare il segnalibro."
$ 
$ Dtinfo*MarkMoveFailed
$ 
12 "Dtinfo non è in grado di rimuovere il segnalibro."
$ 
$ Dtinfo*MaxDepthExceeded
$ 
13 "Registrare un bug"
$ 
$ Dtinfo*MarkIconInvisible
$ 
14 "Visualizzazione impossibile: il segnalibro selezionato si\n\
trova all'esterno dell'area visualizzabile della sezione."
$ 
$ Dtinfo*MViewNoBase
$ 
15 "Il segnalibro selezionato non si trova\n\
in un documento accessibile.\n\
Caricare la BiblioInfo appropriata."
$ 
$ Dtinfo*GetGeometry
$ 
16 "Fare clic nella finestra Dtinfo la cui\n\
grandezza va salvata come default."
$ 
$ Dtinfo*SaveModifiedScope
$ 
17 "Salvare le modifiche all'ambito corrente?"
$ 
$ Dtinfo*BadScopeName
$ 
18 "Il nome non può contenere i seguenti caratteri:\n\
due punti (:), punto e virgola (\;) e virgola (,)"
$ 
$ Dtinfo*EmptyScopeName
$ 
19 "Inserire un nome valido."
$ 
$ Dtinfo*ScopeNameTooLong
$ 
20 "Il nome non può superare i 30 caratteri.\n\
Specificare un nome più corto."
$ 
$ Dtinfo*DuplicateScopeName
$ 
21 "Il nome specificato è già in uso.\n\
Inserire un nome unico."
$ 
$ Dtinfo*Everything
$	22	"Everything" refers to section components like
$		titles, body, and related.
$ 
22 "Tutto"
$ 
$ Dtinfo*Titles
$ 
23 "Titoli"
$ 
$ Dtinfo*Body
$	24	"Body" as in the body of text.
$ 
24 "Testo principale"
$ 
$ Dtinfo*Examples
$ 
25 "Esempi"
$ 
$ Dtinfo*Index
$ 
26 "Indice analitico"
$ 
$ Dtinfo*Tables
$ 
27 "Tabelle"
$ 
$ Dtinfo*Graphics
$ 
28 "Grafici"
$ 
$ Dtinfo*ScopeDeleteConfirm
$ 
29 "Cancellare l'ambito visualizzato?"
$ 
$ Dtinfo*CurrentSection
$ 
30 "Sezione corrente"
$ 
$ Dtinfo*IgnoredScopes
$ 
31 "Gli ambiti salvati che non sono validi per queste BiblioInfo\n\
verrano ignorati."
$ 
$ Dtinfo*MarkBaseOpenFailed
$ 
32 "Impossibile aprire il database dei segnalibri."
$ 
$ Dtinfo*NoOpenMarkBases
$ 
33 "Il database dei segnalibri è introvabile.\n\
Dtinfo non può creare un segnalibro."
$ 
$ Dtinfo*MarkBaseReadOnly
$ 
34 "Il database corrente è di sola lettura."
$ 
35 "Errore sconosciuto nel documento: Rivolgersi al Supporto prodotti."

36 "Messaggio inatteso dal motore di ricerca\n\
(in UAS_SearchEngine):\n\
Rivolgersi al Supporto prodotti."
$ 
$ Dtinfo*TooManyIbases
$ 
37 "Il motore di ricerca è in grado di eseguire la ricerca\n\
solamente nelle prime %d librerie specificate.\n\
Continuare?"
$ 
$ Dtinfo*DemoFailure
$ 
38 "Questa è una versione demo del browser.\n\
Impossibile accedere alla BiblioInfo."
$ 
$ Dtinfo*StyleSheetSyntaxError
$ 
39 "Errore nella specificazione degli stili per il documento.\n\
Verrano usati i valori predefiniti."
$ 
$ Dtinfo*NoHits
$ 
40 "Non è stata trovata nessuna ricorrenza del testo specificato."
$ 
$ Dtinfo*BadQuery
$ 
41 "Dtinfo non è riuscito ad analizzare la stringa richiesta.\n\
Riformulare e riprovare."
$ 
$ Dtinfo*SearchError
$ 
42 "Errore interno del motore di ricerca. Tutte le ricerche\n\
seguenti in questa sessione risulteranno con lo stesso errore.\n\n\
Questo problema è conosciuto e verrà risolto nel prossimo release\n\
del Browser. Per il momento, vi consigliamo di evitare simili stringhe.\n\
Per continuare la ricerca, uscire e riavviare il Browser."
$ 
$ Dtinfo*KernelError
$ 
43 "Errore interno del motore di ricerca.\n\
È probabile che la directory /usr/tmp di questo\n\
computer si trovi in (o si colleghi a) un filesytem tmpfs.\n\
Tutte le ricerche seguenti di questa sessione risulteranno\n\
con lo stesso errore.\n\n\
Questo problema è conosciuto e verrà risolto nel prossimo release.\n\
Per il momento, vi consigliamo di riconfigurare la directory /usr/tmp \n\
in modo che non si trovi (non si colleghi a) un filesystem tmpfs.\n\
Dopo aver eseguito le modifiche e riavviato il Browser, la ricerca\n\
dovrebbe funzionare correttamente. Per ulteriori informazioni, rivolgersi\n\
all'amministratore di sistema."
$ 
$ Dtinfo*FulcrumNoMemory
$ 
44 "Il motore di ricerca non è riuscito ad eseguire la ricerca\n\
dovuto a memoria insufficiente.\n\
Uscire da altre applicazioni o aumentare la memoria fisica o virtuale."
$ 
$ Dtinfo*FulcrumError
$ 
45 "Errore del motore di ricerca. Notificare il \n\
Supporto prodotti dell'errore di ricerca %d."
$ 
$ Dtinfo*BadHelp
$ 
46 "Questo pulsante è collegato ad un documento che non\n\
si trova nella BiblioInfo corrente."
$ 
$ Dtinfo*query_is_read_only
$ 
47 "Questo campo è di sola visualizzazione.  Per editare\n\
l'oggetto della ricerca, usare l'Editor delle ricerche oppure il campo delle in\n\
ricerche rapide nelle finestre Lettura o Elenco libri."
$ 
$ Dtinfo*ExceedRdWindows
$ 
48 "Il numero massimo di finestre di lettura (%d) è stato\n\
raggiunto. Questo limite viene specificato nelle Preferenze."
$ 
$ Dtinfo*NoPrefBase
$ 
49 "Il database Preferenze non è disponibile."
$ 
$ Dtinfo*PrefBaseReadOnly
$ 
50 "Il database Preferenze è stato aperto con l'autorizzazione di\n\
sola lettura. Le nuove preferenze non possono essere salvate."
$ 
$ Dtinfo*DeadEndLink
$ 
51 "Il documento o la sezione non è disponibile."
$ 
$ Dtinfo*MarkPositionFailed
$ 
52 "Dtinfo non può visualizzare il segnalibro specificato.\n\
\42%s\42\n\
Il paragrafo associato al segnalibro non esiste più."
$ 
$ Dtinfo*ScopeChangeAbandon
$ 
53 "Perdere le modifiche?"
$ 
$ Dtinfo*InformationLibrary
$ 
63 "Biblioteca delle informazioni"
$ 
$ Dtinfo*On
$ 
64 "Sì"
$ 
$ Dtinfo*Off
$ 
65 "No"
$ 
67 "Impossibile creare la BiblioInfo."

68 "Creazione della BiblioInfo fallita."
$ 
69 "La funzione di ricerca 'peso' non è disponibile."

$	70	The "qualified locator" is a "fully qualified
$		locator" as opposed to the simple format.
70 "Metodo di accesso non valido nel localizzatore (URL)."

71 "Creazione del documento fallita."

72 "Dtinfo: Staccare grafico"
73 "Fare clic su un grafico per staccarlo."


$set	21
$ 
$ messages specific to message dialogs
$ 

$ Dtinfo*ErrorDialogTitle
$ 
1 "Dtinfo: Errore"

$ Dtinfo*WarningDialogTitle
$ 
2 "Dtinfo: Avviso"

$ Dtinfo*InfoDialogTitle
$ 
3 "Dtinfo: Informazioni"

$ Dtinfo*ProgressDialogTitle
$ 
$	4	"Progress" is a level of completion in regards
$		 to a task.
4 "Dtinfo: Progresso"

$ Dtinfo*QuitDialogTitle
$ 
5 "Dtinfo: Conferma dell'uscita"

$ Dtinfo*QuestionDialogTitle
$ 
6 "Dtinfo: Domanda"
$ 
7 "Dtinfo: Registrare un bug"


$set	22
$ 
$ dtinfo version information
$ 
$ 
$	Dtinfo*Version
$ 
1 "Browser del CDE Dtinfo\n\
\n\
Copyright (c) 1996 Chisholm Technologies, Inc.,\n\
Austin, TX 78758 USA\n\
\n\
Copyright (c) 1994, 1995 HAL Computer Systems, Inc.\n\
\n\
All Rights Reserved.\n\
\n\
U. S. Government Restricted Rights\n\
Use, duplication or disclosure by the Government is subject\n\
to restrictions set forth in subparagraphs (a) through (d)\n\
of the Commercial Computer - Restricted Rights clause at FAR\n\
52.227-19 when applicable, or in subparagraph (c)(1)(ii) of\n\
the Rights in Technical Data and Computer Software clause at\n\
DFARS 252.227-7013, and in similar clauses in the NASA FAR\n\
Supplement. Contractor/manufacturer is HAL Computer\n\
Systems, 1315 Dell Avenue, Campbell, CA 95008 USA."


$set	23
$ 
$ messages specific to CanvasRenderer (dtinfo I/F to DtCanvas/DtHelp)
$ 


$set	24
$ 
$ messages specific to UAS/Base modules
$ 
1 "Nessuna parte di accesso nel localizzatore (URL)."
2 "Impossibile creare oggetti %s"

$ In the following, do not translate any part of "UAS_ObjList<T>::operator[]"
$ 
3 "Errore dell'intervallo: UAS_ObjList<T>::operatore[]"
4 "Errore dell'intervallo: UAS_PtrList<T>::operatore[]"


$set	27
$ 
$ messages specific to UAS/MMDB modules
$ 
1 "Le seguenti librerie non sono valide:"
2 "Percorso BiblioInfo MMDB non valido: %s"
3 "Il percorso BiblioInfo MMDB non è valido e verrà ignorato:"
4 "Biblioteca MMDB %d"
5 "BiblioInfo MMDB: "
6 "BiblioInfo %s"


$set	28
$ 
$ messages specific to graphical map window
$ 
1 "Dtinfo: Mappa locale"
2 "Dtinfo: "


$set	29
$ 
$ messages specific to search history window
$ 
$	1	"Search Results History" is a history of the
$ search results already obtained under the dtinfo session, for
$ prior queries.
1 "Dtinfo: Cronologia dei risultati della ricerca"


$set	30
$ 
$ messages specific to window system module
$ 
1 "Dtinfo: Errore irreversibile"
2 "Chiusura applicazione..."
3 Impossibile aprire il display.\n

$set	31
$ 
$ messages specific to printing
$ 
1 "Un'intestazione di pagina"
2 "Un piè di pagina - pagina %d"


$set	40
$ 
$ messages specific to environmental interface or program context
$ 

$ 1     ""
1 "Funzione non implementata."
2 Argomento non valido
3 ERRORE: L'opzione -sect deve essere specificato con l'opzione -print.
4 Sintassi: dtinfo...
5 Questa applicazione comprende tutte le opzioni X Toolkit\n\
per la riga di comando.

$set	41
$ The messages in this set will only appear in the users
$   "errorlog" file.
$ 
$ ******************************************************************************
$ 
$ messages specific to tooltalk interface and operations.
$ Note that many errors defined by tooltalk are passed through
$ and therefore not repeated here.
$ 

$ 1     "Can't get tt session for X display %s"
1 Can't get tt session for X display:\n%s

$ 2     "Could not set default tooltalk session."
2 Could not set default tooltalk session:\n%s

$ 3 Failed to open ToolTalk connection.
3 Failed to open ToolTalk connection:\n%s

$ 4     "Could not declare ptype DtInfo."
4 Could not declare ptype DtInfo:\n%s

$ Messages 5-10: These messages result from an error in a tt_* function
$    call.
$ 
5 The ttsession(1) process is not running and the ToolTalk\n\
service cannot restart it.
6 The current default process identifier is out of date or invalid.
7 The specified ToolTalk session is out of date or invalid.

$ 
$ Message 8: Do not localize the word DtInfo.
$ 
8 The process type 'DtInfo' is not the\n\
name of an installed process type.

$ 
$ Message 9: Do not localize the word DtInfoPrint.
$ 
9 The process type 'DtInfoPrint' is not the\n\
name of an installed process type.

10 The error number is not recognized.

$set	42
$ 
$ messages specific to UAS/DtSR_SearchEngine modules
$ 
1 "DtSearch non supporta la richiesta."
