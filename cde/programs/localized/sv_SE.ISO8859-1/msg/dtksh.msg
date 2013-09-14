$ $XConsortium: dtksh.msg /main/3 1996/07/24 18:59:36 drk $
$set 1
1 DtkshCvtStringToPixel: Färgen '%s' har inte definierats

$set 5
1 Du har angivit för många resursargument. '%s' hoppas över.
2 Felaktig resursspecifikation. Den bör vara på formatet 'namn:värde' : %s
3 Resursavskiljaren är ':' men du har använt '=' : %s
4 Verktygslådan har redan initierats
5 Syntax: XtInitialize variable applicationName applicationClass [arg ...]
6 Syntax: %s variable name class parent [arg:val ...]
7 Syntax: XtCreateApplicationShell variable name class [arg:val ...]
8 Syntax: XtPopup widget GrabNone|GrabNonexclusive|GrabExclusive
9 Kopieringstypen '%s' är okänd. 'GrabNone' används.
10 Syntax: XtIsShell widget
11 Syntax: XtMapWidget widget
12 Syntax: XtUnmapWidget widget
13 Syntax: %s variable widget
14 Syntax: XtScreen variable widget
15 Syntax: XtWindow variable widget
16 Syntax: XtCallCallbacks widget callbackName
17 Syntax: XtHasCallbacks variable widget callbackName
18 Syntax: XtAddCallback widget callbackName ksh-command
19 Syntax: XtRemoveCallback widget callbackName ksh-command
20 Följande är ett ogiltigt anropshandtag: %s
21 Angivet anrop har inte registrerats
22 Syntax: XtAddEventHandler widget mask nonMaskable ksh-command
23 Syntax: XtRemoveEventHandler widget mask nonMaskable ksh-command
24 Angiven händelsehanterare har inte registrerats
25 Syntax: XtGetValues widget resource:variable ...
26 Följande resursargument har felaktigt format: %s
27 Syntax: XtSetValues widget arg:val ...
28 Syntax: XtAddWorkProc variable command
29 Syntax: XtRemoveWorkProc workProcId
30 Argumentet workProcId måste vara ett hexadecimalt tal: %s
31 Syntax: XtAddTimeOut variable milliseconds command
32 Argumentet milliseconds måste vara större än noll
33 Syntax: XtRemoveTimeOut intervalId
34 Argumentet intervalId måste vara ett hexadecimalt tal: %s
35 Syntax: %s widget ...
36 Syntax: XBell display volume
37 Syntax: %s variable screen
38 Argumentet screen är ogiltigt: %s
39 Syntax: XDefineCursor display window cursorId
40 Argumentet cursorId är ogiltigt: %s
41 Syntax: XUndefineCursor display window
42 Syntax: XtRemoveAllCallbacks widget callbackName
43 Syntax: XTextWidth variable fontName string
44 Syntax: %s display drawable [args ...]
45 Argumentet drawable är ogiltigt: %s
46 Argumentet destination är ogiltigt: %s
47 Okänt namn på grafikfunktionen: %s
48 Okänd linjestil: %s
49 Okänt ritalternativ: %s
50 Det fanns kvarlämnade bildpunkter som ignorerades
51 Syntax: XCopyArea display source dest sourceX sourceY width height destX destY [args ...]
52 Det går inte att konvertera resurstypen '%s' till 'String'
53 Det går inte att konvertera resurstypen 'String' till typen '%s'
54 Syntax: XtAddInput variable [-r] fileDescriptor kshCommand
56 Syntax: XtRemoveInput inputId
57 Argumentet inputId måste vara ett hexadecimalt tal: %s
59 Syntax: XFlush display
60 Syntax: XSync display discard
61 Syntax: XRaiseWindow display window
62 Syntax: %s widget [True|False]
63 Syntax: %s widget translations
64 Syntax: XtUninstallTranslations widget
65 Syntax: XtParent variable widget
66 Syntax: XtLastTimestampProcessed variable display
67 Syntax: DtSessionRestorePath widget pathVariable saveFile
68 Syntax: DtSessionSavePath widget pathVariable fileVariable
69 Syntax: DtShellIsIconified widget
70 Syntax: DtSetStartupCommand widget command
71 Syntax: DtSetIconifyHint widget boolean
73 Argumentet rootWindow är ogiltigt: %s
74 Syntax: DtWsmSetCurrentWorkspace widget atom
75 Arbetsytans atom är ogiltig: %s
78 Syntax: DtWsmSetWorkspacesOccupied display window workspaceList
79 Syntax: DtGetHourGlassCursor variable display
80 Syntax: DtWsmAddCurrentWorkspaceCallback variable widget ksh-command
81 Syntax: DtWsmRemoveWorkspaceCallback handle
82 Syntax: XtNameToWidget variable referenceWidget names
83 Syntax: DtDbLoad
84 Syntax: DtDbReloadNotify ksh-command
85 Syntax: DtActionExists actionName
86 Syntax: DtActionLabel variable actionName
87 Syntax: DtActionDescription variable actionName
88 Syntax: DtActionInvoke widget actionName termParms execHost contextDir useIndicator ksh-command ["FILE" fileName] ...
89 Syntax: DtDtsLoadDataTypes
90 Syntax: DtDtsFileToDataType variable fileName
91 Syntax: DtDtsFileToAttributeValue variable fileName attrName
92 Syntax: DtDtsFileToAttributeList variable fileName
93 Syntax: DtDtsDataTypeToAttributeValue variable dataType attrName optName
94 Syntax: DtDtsDataTypeToAttributeList variable dataType optName
95 Syntax: DtDtsFindAttribute variable name value
96 Syntax: DtDtsDataTypeNames variable
97 Syntax: DtDtsSetDataType variable fileName dataType override
98 Syntax: DtDtsDataTypeIsAction dataType
99 Syntax: ttdt_open variable status variable2 toolname vendor version sendStarted
100 Syntax: tttk_Xt_input_handler procId source id
101 Argumentet source måste vara ett heltal: %s
102 Argumentet id måste vara ett hexadecimalt tal: %s
103 Syntax: ttdt_close status procId newProcId sendStopped
104 Syntax: ttdt_session_join variable status sessId shellWidgetHandle join
105 Syntax: ttdt_session_quit status sessId sessPatterns quit
106 Argumentet sessPatterns är ogiltigt: %s
107 Syntax: ttdt_file_event status op patterns send
108 Syntax: ttdt_file_quit status patterns quit
109 Syntax: ttdt_Get_Modified pathName scope timeout
110 Syntax: %s status pathName scope timeout
112 Syntax: tt_error_pointer variable ttStatus
113 Syntax: %s status msg
114 Syntax: %s status msg msgStatus msgStatusString destroy
115 Syntax: ttdt_file_join variable status pathName scope join ksh-command
116 Syntax: tt_file_netfile variable status filename
117 Syntax: tt_netfile_file variable status netfilename
118 Syntax: tt_host_file_netfile variable status host filename
119 Syntax: tt_host_netfile_file variable status host netfilename
120 Syntax: XtIsSubclass widget class
121 %s är ett ogiltigt widget class name
122 Syntax: XtClass variable widget

$set 6
1 DtkshCvtWindowToString: Värdet 'from' har ogiltig storlek
2 DtkshCvtHexIntToString: Värdet 'from' har ogiltig storlek
3 DtkshCvtIntToString: Värdet 'from' har ogiltig storlek
4 DtkshCvtBooleanToString: Värdet 'from' har ogiltig storlek
5 DtkshCvtStringToWidget: Värdet 'from' har ogiltig storlek
6 DtkshCvtStringToWidget: Det går inte att hitta en byggkloss med namnet '%s'
7 DtkshCvtStringToCallback: Värdet 'from' har ogiltig storlek
8 DtkshCvtCallbackToString: Värdet 'from' har ogiltig storlek
9 DtkshCvtWidgetToString: Värdet 'from' har ogiltig storlek
10 DtkshCvtWidgetToString: Det går inte att hitta byggklossens namn
11 DtkshCvtWidgetClassToString: Värdet 'from' har ogiltig storlek
12 DtkshCvtWidgetClassToString: Okänd byggklossklass
13 DtkshCvtStringToWidgetClass: Okänt namn på byggklossklassen
14 DtkshCvtScreenToString: Värdet 'from' har ogiltig storlek

$set 11
1 Det går inte att hitta fältet '%s' i strukturen '%s'
2 DtkshCvtCallbackToString: En intern konvertingsbuffert blev överfull
3 Hashfel för resursen '%s' i byggklossklassen '%s'
4 Det fanns inget funktionsnamn
5 Kommandot kan inte exekveras. Verktygslådan har inte initierats.
6 Det gick inte att skapa byggklossen '%s'
7 Argumentet display är ogiltigt: %s
8 Följande egenskapsatom är ogiltig: %s
9 Följande resurs har inte definierats för byggklossen '%s': %s
10 Den angivna positionen är ogiltig: %s
11 Den angivna fonten är ogiltig: %s
12 Handtaget '%s' refererar inte till en befintlig byggkloss
13 Handtaget måste vara en byggkloss av typen 'command'
14 Handtaget måste vara en byggkloss av typen 'mainWindow'
15 Handtaget måste vara en byggkloss av typen 'scale'
16 Handtaget måste vara en byggkloss av typen 'scrollBar'
17 Handtaget måste vara en byggkloss eller gadget av typen 'toggleButton'
18 Argumentet window är ogiltigt: %s
19 Det går inte att tilldela allt minne som krävs. Programmet avslutas.
20 Det går inte att hitta den överordnade byggklossen.
21 Det går inte att hitta symbolen '%s'
22 Det går inte att hitta typen '%s'
23 Det går inte att analysera deklarationen '%s'. 'unsigned long' används.
24 Okänd typ av underordnad process: %s
25 Okänd alternativflagga: %s
26 Syntax: %s widget position
27 Syntax: %s widget time
28 Syntax: %s widget
29 Fel i dtksh
30 dtksh - varning
31 Syntax: %s display window
32 Syntax: %s display window variable
33 Syntax: %s display rootWindow variable
34 Argumentet timeout är ogiltigt: %s
35 Argumentet msg är ogiltigt: %s
36 Argumentet patterns är ogiltigt: %s

$set 14
1 Fel i intern hash-tabell medan byggklossklassen initierades. Programmet avslutas.
2 Det gick inte att hitta en byggklossklass med namnet '%s'
3 Identifieraren '%s' är inte ett giltigt byggklosshandtag.
4 Det går inte att hitta byggklossklassen.
5 Syntax: DtLoadWidget widgetClassName widgetClassRecordName
6 Det går inte att hitta en post för en byggklossklass med namnet '%s'
7 Fel i intern hash-tabell medan byggklossklassen '%s' initierades.  
8 \n%sRESURSER FÖR%s%s%s:\n
9 VILLKOR 
10 R
11 M
12 S
13 %-15s %-6s %-6s %-18s %-6s %s\n
14 MILJÖVARIABEL    HANDTAG ÖVERORDN. KLASS              STATUS NAMN\n
15 \nSyntax:\tDtWidgetInfo [widgetHandle]\n\tDtWidgetInfo -r <widgetHandle|className>\n\tDtWidgetInfo -R <widgetHandle|className>\n\tDtWidgetInfo -c [className]\n\tDtWidgetInfo -h [widgetHandle]

$set 15
1 Det går inte att initiera verktygslådan.
2 Följande resurs kan inte anges när\nbyggklossen skapas. Använd XtSetValues efteråt i stället: %s
3 Syntax: %s variable parent name [argument:value ...]
4 Syntax: DtHelpReturnSelectedWidgetId variable widget variable
5 Syntax: DtHelpSetCatalogName catalogName
6 Syntax: DtHelpQuickDialogGetChild variable quickHelpWidget child
7 Byggklossen måste vara av typen 'quickHelp'
8 Byggklossen måste vara av typen 'list'
9 Syntax: %s widget position item
10 Syntax: %s widget position itemList
11 Syntax: %s widget item
12 Syntax: XmListDeleteItemsPos widget count position
13 Syntax: XmListDeleteItems widget itemList
14 Syntax: XmListDeletePositions widget positionList
15 Syntax: XmListGetKbdItemPos variable widget
16 Syntax: XmListItemExists widget item
17 Syntax: XmListItemPos variable widget item
18 Syntax: XmListPosSelected widget position
19 Syntax: XmListPosToBounds widget position variable variable variable variable
20 Syntax: %s widget position notifyFlag
21 Syntax: %s widget item notifyFlag
22 Syntax: XmListSetAddMode widget boolean
23 Syntax: XmListSetKbdItemPos widget position
24 Syntax: XmMainWindowSetAreas mainwindow menu command hscroll vscroll work
25 Handtaget 'mainWindow' är NULL
26 Syntax: %s variable mainwindow
27 Syntax: XmProcessTraversal widget direction
28 Okänd förflyttningsriktning: %s
29 Syntax: XmInternAtom variable display name onlyIfExists
30 Syntax: XmGetAtomName variable display atom
31 Den angivna atomen är ogiltig: %s
32 Syntax: XmGetColors widget background foreground topshadow bottomshadow select
33 Bakgrundsbildpunkten är ogiltig: %s
34 Syntax: XmUpdateDisplay widget
35 Syntax: %s widget protocol [protocol ...]
36 Den angivna atomen är ogiltig: %s
37 Syntax: XmAddWMProtocolCallback widget protocol ksh-command
38 Syntax: XmRemoveWMProtocolCallback widget protocol ksh-command
39 Syntax: XmMenuPosition menu event
40 Den angivna händelsen är ogiltig: %s
41 Syntax: XmCommandAppendValue commandWidget string
42 Syntax: XmCommandError commandWidget errorMessage
43 Syntax: XmCommandSetValue commandWidget command
44 Syntax: XmCommandGetChild variable commandWidget child
45 Syntax: XmMessageBoxGetChild variable commandWidget child
46 Byggklossen måste vara av typen 'messageBox'
47 Syntax: XmFileSelectionBoxGetChild variable widget child
48 Byggklossen måste vara av typen 'file selection box'
49 Syntax: XmSelectionBoxGetChild variable widget child
50 Byggklossen måste vara av typen 'selection box'
51 Syntax: XmScaleGetValue scaleWidget variable
52 Syntax: XmScaleSetValue scaleWidget value
53 Det angivna skalningsvärdet är ogiltigt: %s
54 Syntax: XmScrollBarGetValues scrollbar variable variable variable variable
55 Syntax: XmScrollBarSetValues scrollbar value sliderSize increment pageIncrement notify
56 Det angivna värdet är ogiltigt: %s
57 Den angivna reglagestorleken är ogiltig: %s
58 Det angivna ökningsvärdet är ogiltigt: %s
59 Den angivna sidökningen är ogiltig: %s
60 Syntax: XmScrollVisible scrolledWin widget leftRightMargin topBottomMargin
61 Byggklossen måste vara av typen 'scrolledWindow'
62 Byggklossen som ska göras synlig finns inte.
63 Den angivna vänster-/högermarginalen är ogiltig: %s
64 Den angivna över-/undermarginalen är ogiltig: %s
65 Syntax: %s widget state notify
66 Syntax: catopen variable catName
67 Syntax: catclose catId
68 Syntax: catgets variable catId setNum msgNum dftMsg
69 Byggklossen måste vara av typen 'text' eller 'textField'
70 Syntax: %s variable widget
71 Syntax: %s widget boolean
72 Det angivna argumentet är ogiltigt: %s
73 Syntax: %s widget lines
74 Syntax: %s widget maxLength
75 Syntax: %s widget string
76 Syntax: %s widget variable variable
77 Syntax: %s widget position string
78 Syntax: %s widget position variable variable
79 Syntax: %s widget fromPosition toPosition string
80 Det angivna värdet på 'from' är ogiltigt: %s
81 Det angivna värdet på 'to' är ogiltigt: %s
82 Syntax: %s widget firstPosition lastPosition time
83 Den första angivna positionen är ogiltig: %s
84 Den sista angivna positionen är ogiltig: %s
85 Den angivna tiden är ogiltig: %s
86 Syntax: %s variable widget x y
87 Den angivna x-positionen är ogiltig: %s
88 Den angivna y-positionen är ogiltig: %s
89 Syntax: %s widget left right mode
90 Den vänstra angivna positionen är ogiltig: %s
91 Den högra angivna positionen är ogiltig: %s
92 Syntax: %s widget start string direction variable
93 Den angivna startpositionen är ogiltig: %s
94 Syntax: XmListGetSelectedPos variable widget
95 Syntax: XmListGetMatchPos variable widget item
96 Syntax: XmOptionLabelGadget variable widget
97 Syntax: XmOptionButtonGadget variable widget
98 Syntax: XmGetVisibility variable widget
99 Syntax: XmGetTearOffControl variable widget
100 Syntax: XmGetTabGroup variable widget
101 Syntax: XmGetPostedFromWidget variable widget
102 Syntax: XmGetFocusWidget variable widget
103 Syntax: XmFileSelectionDoSearch widget directoryMask

$set 16
1 DtkshCvtNamedValueToString: Det fanns inget matchande värde. 
2 DtkshCvtStringToNamedValue: Det går inte att konvertera strängen '%s'.


$ The following messages are for ksh93 itself.  The message numbers must
$ match those specified in the MsgStr array allmsgs in 
$ ksh93/src/cmd/ksh93/sh/init.c, as must the set number.
$set 25
1  Klart
3  Kör
4  [-n] [arg...]
5  [arg...]
6  [kat] [lista]
7  [jobb...]
8  [n]
9  [namn [sökväg] ]
10  [namn]
11  [topp] [bas]
12  uttr...
13  format [arg...]
14  är en funktion
15  är ett nyckelord
16  är inbyggt i skalet
17  är en exporterad funktion
18  är en odefinierad funktion
19  namn [arg...]
20  optsträng namn [arg...]
21  sekunder
23 ${HOME:-.}/.profile
24 %c: ogiltigt tecken i uttryck - %s
25 %c: ogiltig formatangivelse
26 %d-%d: ogiltigt intervall
27 %d: ogiltig binärskriptversion
28 %s är ett alias för 
29 %s är ett exporterat alias för 
30 %s saknas
31 %s okänd bas
32 %s: ':' väntades som operatorn '?'
33 %s: Mångtydig
34 %s: Argumentet måste vara %jobb- eller process-ID
35 %s: alias kan inte hittas\n
36 %s: aritmetiskt syntaxfel
37 %s: uppdraget kräver värde
38 %s: felaktigt filenhetsnummer
39 %s: felaktigt format
40 %s: felaktigt nummer
41 %s: felaktigt tillägg
42 %s: felaktig ersättning
43 %s: felaktig fälla
44 %s: det går inte att skapa
45 %s: det går inte att exekvera
46 %s: det går inte att öppna
47 %s: division med noll
48 %s: domänundantag
49 %s: misslyckas %s
50 %s: filen finns redan
51 %s: ogiltigt funktionsnamn
52 %s: ogiltigt aliasnamn
53 %s: ogiltig disciplinfunktion
54 %s: ogiltigt exportnamn
55 %s: ogiltigt funktionsnamn
56 %s: ogiltigt namn
57 %s: ogiltigt reguljärt uttryck
58 %s: ogiltig självreferens
59 %s: ogiltig användning av:
60 %s: ogiltigt variabelnamn
61 %s: är inte en identifierare
62 %s: är skrivskyddad
63 %s: namnet har inte implementerats
64 %s: gränsen har överskridits
65 %s: flera ord krävs
66 %s: inte överordnad
67 %s: inget referensnamn
68 %s: finns inte
69 %s: har inte implementerats
70 %s: operander har inkompatibla typer
71 %s: spillundantag
72 %s: argumentet har inte angivits
73 %s: argumentet har värdet null eller har inte angivits
74 %s: för djup rekursion
75 %s: referensvariabeln kan inte vara en vektor
76 %s: kräver sökvägsargument
77 %s: begränsad
78 %s: singularitetsundantag
79 %s: Underordnat skript är inte inom intervallet
80 %s: parenteserna stämmer inte överens
81 %s: okänd funktion
82 %s: okänd lokal miljö
83 %s: okänd operator
84 %s: okänt signalnamn
85 %s: kommer att loopa
86 (kärnan dumpas)
87 -c kräver argument
88 -e - kräver ett argument
89 /vpix
90 <okänt kommando>
91 ACE#?F#?H:[namn]L#?R#?Z#?fi#?[bas]lnprtux [namn=[värde]...]
92 AE#?F#?HL#?R#?Z#?fi#?[bas]lnprtux [namn=[värde]...]
93 Avbryt
94 Ad:[avgränsare]prst#[tidsgräns]u#[filnr] [namn...]
95 Alarmanrop
96 Felaktig rotnodsspecifikation
97 Felaktigt systemanrop
98 Avbruten programkoppling
99 Bussfel
100 Det går inte att starta jobbinställning
101 Aktuella alternativinställningar
102 DIL-signal
103 Det underordnade objektet har upphört att gälla
104 DircabefhkmnpstuvxCI:[fil]o:?[alternativ]A:[namn] [arg...]
105 DircabefhkmnpstuvxCo:?[alternativ]A:[namn] [arg...]
106 EMT-fälla
107 CPU-tidsgränsen har överskridits
108 Filstorleken har överskridits
109 Flytande undantag
110 HSacdfmnstv [gräns]
111 avslutning
112 in-utsignal
113 Ogiltig instruktion
114 Avbrottssekvens
115 Upphört att gälla
116 LP [kat] [byt]
117 Minnesfel
118 Förflyttningsprocess
119 Ingen jobbinställning
120 Avbrottssekvens för uppringning
121 Sökalarm
122 Strömavbrott
123 Tidsalarm för profil
124 Avsluta
125 Resurser saknas
126 Återgår till gammal tty-drivrutin...
127 S [mask]
128 SIGAPOLLO
129 Säkerhetsnamnet har ändrats
130 Avbrottssekvens för sockel
131 Ljudet är färdigt
132 Stoppade (signal)
133 Stoppade (tty-indata)
134 Stoppad process fortsätter
135 Stoppade
136 Stoppade (tty-utdata)
137 Växlar till en ny tty-drivrutin...
138 Systemet kraschar snart
139 Avslutade
140 Trace/BPT-fälla
141 Okänd version
142 Använd 'exit' för att avsluta skalet
143 Användarsignal 1
144 Användarsignal 2
145 Versionen har inte definierats
146 Virtuellt tidsalarm
147 Fönsterstorleken ändras
148 Du har jobb som körs
149 Du har stoppade jobb
150 [_[:alfa:]]*([_[:alnum:]])
151 \n@(#)Version 12/28/93\0\n
152 \n@(#)Version M-12/28/93\0\n
153 \nreal
154 \r\n\007skal kommer att avslutas om 60 sekunder eftersom det inte används
155 ett namn...
156 a:c [kommando [arg...] ]
157 afpv namn...
158 alarm %s %.3f\n
159 alarm -r %s +%.3g\n
160 argument krävs
161 felaktig katalog
162 felaktigt filenhetsnummer
163 felaktig ersättning
164 det går inte att få tillgång till överordnade kataloger
165 det går inte att skapa programkoppling
166 det går inte att skapa temporär fil
167 det går inte att klyva
168 det går inte att hämta %s
169 det går inte att ange %s
170 det går inte att ställa in alarm
171 villkor krävs
172 dsf:[bibliotek] [namn...]
173 e:[redigerare]lnrsN# [först] [sist]
174 filslut
175 f:[format]nprsu:[filnr] [arg...]
176 fnv namn...
177 hist -e "${VISUAL:-${EDITOR:-vi}}" 
178 det går inte att öppna historikfil
179 felaktig syntax
180 felaktigt argument av typen %c
181 är en skalinbyggd version av
182 är ett spårat alias för
183 stäng
184 rad %d: $ inleds inte med \\
185 rad %d: %c i ${} bör omges av citattecken
186 rad %d: %s okänt namn
187 rad %d: %s i [[...]] används inte, använd ((...))
188 rad %d: '=' används inte, använd '=='
189 rad %d: -a används inte, använd -e
190 rad %d: \\ framför %c har reserverats för framtida bruk
191 rad %d: `...` används inte, använd $(...)
192 rad %d: Välj escape %c för att undvika mångtydigheter
193 rad %d: namnet %s ignoreras
194 rad %d: omge %c med citattecken för att undvika mångtydighet
195 rad %d: ange %s som föråldrad
196 rad %d: mellanslag krävs i nästlat underordnat skal
197 rad %d: använd hakparenteser för att undvika sammanblandning med $id[...]
198 rad %d: använd mellanslag eller tabbar för att avskilja operatorerna %c och %c
199 ln#[signum]s:[signamn] sig...
200 inloggningsskal av typen setuid/setgid är inte tillåtna
201 mappar
202 radmatning
203 nlp [jobb...]
204 ingen historikfil
205 ingen frågeprocess
206 finns inget sådant jobb
207 finns ingen sådan process
208 stöds inte
209 av
210 på
211 antalet tillåtna öppna filer har överskridits
212 finns inget minne
213 p [funktionsvillkor...]
214 p [namn[=värde]...]
215 argumentet har inte angivits
216 du saknar behörighet
217 processen finns redan
218 ptx [namn=[värde]...]
219 pvV namn [arg]...
220 r [varnamn sekunder]
221 syntaxfel på rad %d: `%s' %s
222 syntaxfel på rad %d: dublett av namnet %s
223 syntaxfel: `%s' %s
224 sys
225 tidsgränsen har nåtts utan indata
226 oväntat
227 universum finns inte tillgängligt
228 obegränsat
229 omatchat
230 användare
231 versioner
232 det gick inte att skriva till %d
233 du har post i $_
234 noll byte
