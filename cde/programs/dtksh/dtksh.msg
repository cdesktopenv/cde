$ $XConsortium: dtksh.msg /main/3 1995/11/01 15:53:40 rswiston $
$set 1
1 DtkshCvtStringToPixel: The color '%s' is not defined

$set 5
1 Too many resource parameters have been specified; skipping '%s'
2 Bad resource specification; should be of the form 'name:value' : %s
3 The resource separator is ':' but you used '=' : %s
4 The toolkit has already been initialized
5 Usage: XtInitialize variable applicationName applicationClass [args ...]
6 Usage: %s variable name class parent [arg:val ...]
7 Usage: XtCreateApplicationShell variable name class [arg:val ...]
8 Usage: XtPopup widget GrabNone|GrabNonexclusive|GrabExclusive
9 The grab type '%s' is not recognized; using 'GrabNone'
10 Usage: XtIsShell widget
11 Usage: XtMapWidget widget
12 Usage: XtUnmapWidget widget
13 Usage: %s variable widget
14 Usage: XtScreen variable widget
15 Usage: XtWindow variable widget
16 Usage: XtCallCallbacks widget callbackName
17 Usage: XtHasCallbacks variable widget callbackName
18 Usage: XtAddCallback widget callbackName ksh-command
19 Usage: XtRemoveCallback widget callbackName ksh-command
20 The following is an invalid callback handle: %s
21 The specified callback is not registered
22 Usage: XtAddEventHandler widget mask nonMaskable ksh-command
23 Usage: XtRemoveEventHandler widget mask nonMaskable ksh-command
24 The specified event handler is not registered
25 Usage: XtGetValues widget resource:variable ...
26 The following resource parameter is incorrectly formed: %s
27 Usage: XtSetValues widget arg:val ...
28 Usage: XtAddWorkProc variable command
29 Usage: XtRemoveWorkProc workProcId
30 The workProcId parameter must be a hex number: %s
31 Usage: XtAddTimeOut variable milliseconds command
32 The milliseconds parameter must be greater than zero
33 Usage: XtRemoveTimeOut intervalId
34 The intervalId parameter must be a hex number: %s
35 Usage: %s widget ...
36 Usage: XBell display volume
37 Usage: %s variable screen
38 The screen parameter is invalid: %s
39 Usage: XDefineCursor display window cursorId
40 The cursorId parameter is invalid: %s
41 Usage: XUndefineCursor display window
42 Usage: XtRemoveAllCallbacks widget callbackName
43 Usage: XTextWidth variable fontName string
44 Usage: %s display drawable [args ...]
45 The drawable parameter is invalid: %s
46 The destination parameter is invalid: %s
47 Unrecognized graphics function name: %s
48 Unrecognized line style: %s
49 Unrecognized drawing option: %s
50 There were left over points which were ignored
51 Usage: XCopyArea display source dest sourceX sourceY width height destX destY [args ...]
52 Unable to convert resource type '%s' to 'String'
53 Unable to convert resource type 'String' to type '%s'
54 Usage: XtAddInput variable [-r] fileDescriptor kshCommand
56 Usage: XtRemoveInput inputId
57 The inputId parameter must be a hex number: %s
59 Usage: XFlush display
60 Usage: XSync display discard
61 Usage: XRaiseWindow display window
62 Usage: %s widget [True|False]
63 Usage: %s widget translations
64 Usage: XtUninstallTranslations widget
65 Usage: XtParent variable widget
66 Usage: XtLastTimestampProcessed variable display
67 Usage: DtSessionRestorePath widget pathVariable saveFile
68 Usage: DtSessionSavePath widget pathVariable fileVariable
69 Usage: DtShellIsIconified widget
70 Usage: DtSetStartupCommand widget command
71 Usage: DtSetIconifyHint widget boolean
73 The rootWindow parameter is invalid: %s
74 Usage: DtWsmSetCurrentWorkspace widget atom
75 The workspace atom is invalid: %s
78 Usage: DtWsmSetWorkspacesOccupied display window workspaceList
79 Usage: DtGetHourGlassCursor variable display
80 Usage: DtWsmAddCurrentWorkspaceCallback variable widget ksh-command
81 Usage: DtWsmRemoveWorkspaceCallback handle
82 Usage: XtNameToWidget variable referenceWidget names
83 Usage: DtDbLoad
84 Usage: DtDbReloadNotify ksh-command
85 Usage: DtActionExists actionName
86 Usage: DtActionLabel variable actionName
87 Usage: DtActionDescription variable actionName
88 Usage: DtActionInvoke widget actionName termParms execHost contextDir useIndicator ksh-command ["FILE" fileName] ...
89 Usage: DtDtsLoadDataTypes
90 Usage: DtDtsFileToDataType variable fileName
91 Usage: DtDtsFileToAttributeValue variable fileName attrName
92 Usage: DtDtsFileToAttributeList variable fileName
93 Usage: DtDtsDataTypeToAttributeValue variable dataType attrName optName
94 Usage: DtDtsDataTypeToAttributeList variable dataType optName
95 Usage: DtDtsFindAttribute variable name value
96 Usage: DtDtsDataTypeNames variable
97 Usage: DtDtsSetDataType variable fileName dataType override
98 Usage: DtDtsDataTypeIsAction dataType
99 Usage: ttdt_open variable status variable2 toolname vendor version sendStarted
100 Usage: tttk_Xt_input_handler procId source id
101 The source parameter must be an integer: %s
102 The id parameter must be a hex number: %s
103 Usage: ttdt_close status procId newProcId sendStopped
104 Usage: ttdt_session_join variable status sessId shellWidgetHandle join
105 Usage: ttdt_session_quit status sessId sessPatterns quit
106 The sessPatterns parameter is invalid: %s
107 Usage: ttdt_file_event status op patterns send
108 Usage: ttdt_file_quit status patterns quit
109 Usage: ttdt_Get_Modified pathName scope timeout
110 Usage: %s status pathName scope timeout
112 Usage: tt_error_pointer variable ttStatus
113 Usage: %s status msg
114 Usage: %s status msg msgStatus msgStatusString destroy
115 Usage: ttdt_file_join variable status pathName scope join ksh-command
116 Usage: tt_file_netfile variable status filename
117 Usage: tt_netfile_file variable status netfilename
118 Usage: tt_host_file_netfile variable status host filename
119 Usage: tt_host_netfile_file variable status host netfilename
120 Usage: XtIsSubclass widget class
121 %s is not a valid widget class name
122 Usage: XtClass variable widget

$set 6
1 DtkshCvtWindowToString: The 'from' value is an invalid size
2 DtkshCvtHexIntToString: The 'from' value is an invalid size
3 DtkshCvtIntToString: The 'from' value is an invalid size
4 DtkshCvtBooleanToString: The 'from' value is an invalid size
5 DtkshCvtStringToWidget: The 'from' value is an invalid size
6 DtkshCvtStringToWidget: Unable to find a widget named '%s'
7 DtkshCvtStringToCallback: The 'from' value is an invalid size
8 DtkshCvtCallbackToString: The 'from' value is an invalid size
9 DtkshCvtWidgetToString: The 'from' value is an invalid size
10 DtkshCvtWidgetToString: Unable to find a name for the widget
11 DtkshCvtWidgetClassToString: The 'from' value is an invalid size
12 DtkshCvtWidgetClassToString: Unknown widget class
13 DtkshCvtStringToWidgetClass: Unknown widget class name
14 DtkshCvtScreenToString: The 'from' value is an invalid size

$set 11
1 Cannot find a field named '%s' in the structure '%s'
2 DtkshCvtCallbackToString: An internal conversion buffer overflowed
3 Hashing failure for resource '%s' in widget class '%s'
4 No function name was supplied
5 The command cannot be executed; the toolkit has not been initialized
6 The creation of widget '%s' failed
7 The display parameter is invalid: %s
8 The following property atom is invalid: %s
9 The following resource is not defined for widget '%s': %s
10 The position specified is invalid: %s
11 The specified font is invalid: %s
12 The widget handle '%s' does not refer to an existing widget
13 The widget must be a 'command' widget
14 The widget must be a 'mainWindow' widget
15 The widget must be a 'scale' widget
16 The widget must be a 'scrollBar' widget
17 The widget must be a 'toggleButton' widget or gadget
18 The window parameter is invalid: %s
19 Unable to allocate required memory; exiting
20 Unable to find the parent widget
21 Unable to locate the symbol '%s'
22 Unable to locate the type '%s'
23 Unable to parse the declaration '%s'; using 'unsigned long'
24 Unknown child type: %s
25 Unrecognized option flag: %s
26 Usage: %s widget position
27 Usage: %s widget time
28 Usage: %s widget
29 dtksh error
30 dtksh warning
31 Usage: %s display window
32 Usage: %s display window variable
33 Usage: %s display rootWindow variable
34 The timeout parameter is invalid: %s
35 The msg parameter is invalid: %s
36 The patterns parameter is invalid: %s

$set 14
1 Internal hash table failure during widget class initialization; exiting
2 Could not find a widget class named '%s'
3 The identifier '%s' is not a valid widget handle
4 Unable to find the widget class
5 Usage: DtLoadWidget widgetClassName widgetClassRecordName
6 Unable to locate a widget class record named '%s'
7 Internal hash table failure during initialization of widget class '%s'
8 \n%sRESOURCES FOR %s%s%s:\n
9 CONSTRAINT 
10 R
11 M
12 S
13 %-15s %-6s %-6s %-18s %-6s %s\n
14 ENV VARIABLE    HANDLE PARENT CLASS              STATUS NAME\n
15 \nUsage:\tDtWidgetInfo [widgetHandle]\n\tDtWidgetInfo -r <widgetHandle|className>\n\tDtWidgetInfo -R <widgetHandle|className>\n\tDtWidgetInfo -c [className]\n\tDtWidgetInfo -h [widgetHandle]

$set 15
1 Unable to initialize the Toolkit
2 The following resource cannot be set at widget\ncreation time.  Use XtSetValues after creation instead: %s
3 Usage: %s variable parent name [argument:value ...]
4 Usage: DtHelpReturnSelectedWidgetId variable widget variable
5 Usage: DtHelpSetCatalogName catalogName
6 Usage: DtHelpQuickDialogGetChild variable quickHelpWidget child
7 The widget must be a 'quickHelp' widget
8 The widget must be a 'list' widget
9 Usage: %s widget position item
10 Usage: %s widget position itemList
11 Usage: %s widget item
12 Usage: XmListDeleteItemsPos widget count position
13 Usage: XmListDeleteItems widget itemList
14 Usage: XmListDeletePositions widget positionList
15 Usage: XmListGetKbdItemPos variable widget
16 Usage: XmListItemExists widget item
17 Usage: XmListItemPos variable widget item
18 Usage: XmListPosSelected widget position
19 Usage: XmListPosToBounds widget position variable variable variable variable
20 Usage: %s widget position notifyFlag
21 Usage: %s widget item notifyFlag
22 Usage: XmListSetAddMode widget boolean
23 Usage: XmListSetKbdItemPos widget position
24 Usage: XmMainWindowSetAreas mainwindow menu command hscroll vscroll work
25 The 'mainWindow' handle is NULL
26 Usage: %s variable mainwindow
27 Usage: XmProcessTraversal widget direction
28 Unknown traversal direction: %s
29 Usage: XmInternAtom variable display name onlyIfExists
30 Usage: XmGetAtomName variable display atom
31 The specified atom is invalid: %s
32 Usage: XmGetColors widget background foreground topshadow bottomshadow select
33 The background pixel is invalid: %s
34 Usage: XmUpdateDisplay widget
35 Usage: %s widget protocol [protocol ...]
36 The atom specified is invalid: %s
37 Usage: XmAddWMProtocolCallback widget protocol ksh-command
38 Usage: XmRemoveWMProtocolCallback widget protocol ksh-command
39 Usage: XmMenuPosition menu event
40 The specified event is invalid: %s
41 Usage: XmCommandAppendValue commandWidget string
42 Usage: XmCommandError commandWidget errorMessage
43 Usage: XmCommandSetValue commandWidget command
44 Usage: XmCommandGetChild variable commandWidget child
45 Usage: XmMessageBoxGetChild variable commandWidget child
46 The widget must be a 'messageBox' widget
47 Usage: XmFileSelectionBoxGetChild variable widget child
48 The widget must be a 'file selection box' widget
49 Usage: XmSelectionBoxGetChild variable widget child
50 The widget must be a 'selection box' widget
51 Usage: XmScaleGetValue scaleWidget variable
52 Usage: XmScaleSetValue scaleWidget value
53 The scale value specified is invalid: %s
54 Usage: XmScrollBarGetValues scrollbar variable variable variable variable
55 Usage: XmScrollBarSetValues scrollbar value sliderSize increment pageIncrement notify
56 The value specified is invalid: %s
57 The slider size specified is invalid: %s
58 The increment specified is invalid: %s
59 The page increment specified is invalid: %s
60 Usage: XmScrollVisible scrolledWin widget leftRightMargin topBottomMargin
61 The widget must be a 'scrolledWindow' widget
62 The widget to be made visible does not exist.
63 The left/right margin specified is invalid: %s
64 The top/bottom margin specified is invalid: %s
65 Usage: %s widget state notify
66 Usage: catopen variable catName
67 Usage: catclose catId
68 Usage: catgets variable catId setNum msgNum dftMsg
69 The widget must be a 'text' or 'textField' widget
70 Usage: %s variable widget
71 Usage: %s widget boolean
72 The parameter specified is invalid: %s
73 Usage: %s widget lines
74 Usage: %s widget maxLength
75 Usage: %s widget string
76 Usage: %s widget variable variable
77 Usage: %s widget position string
78 Usage: %s widget position variable variable
79 Usage: %s widget fromPosition toPosition string
80 The 'from' position specified is invalid: %s
81 The 'to' position specified is invalid: %s
82 Usage: %s widget firstPosition lastPosition time
83 The first position specified is invalid: %s
84 The last position specified is invalid: %s
85 The time specified is invalid: %s
86 Usage: %s variable widget x y
87 The x position specified is invalid: %s
88 The y position specified is invalid: %s
89 Usage: %s widget left right mode
90 The left position specified is invalid: %s
91 The right position specified is invalid: %s
92 Usage: %s widget start string direction variable
93 The start position specified is invalid: %s
94 Usage: XmListGetSelectedPos variable widget
95 Usage: XmListGetMatchPos variable widget item
96 Usage: XmOptionLabelGadget variable widget
97 Usage: XmOptionButtonGadget variable widget
98 Usage: XmGetVisibility variable widget
99 Usage: XmGetTearOffControl variable widget
100 Usage: XmGetTabGroup variable widget
101 Usage: XmGetPostedFromWidget variable widget
102 Usage: XmGetFocusWidget variable widget
103 Usage: XmFileSelectionDoSearch widget directoryMask

$set 16
1 DtkshCvtNamedValueToString: No match found
2 DtkshCvtStringToNamedValue: Unable to convert the string '%s'


$ The following messages are for ksh93 itself.  The message numbers must
$ match those specified in the MsgStr array allmsgs in 
$ ksh93/src/cmd/ksh93/sh/init.c, as must the set number.
$set 25

1  Done
3  Running
4  [-n] [arg...]
5  [arg...]
6  [dir] [list]
7  [job...]
8  [n]
9  [name [pathname] ]
10  [name]
11  [top] [base]
12  expr...
13  format [arg...]
14  is a function
15  is a keyword
16  is a shell builtin
17  is an exported function
18  is an undefined function
19  name [arg...]
20 :a:[name] optstring name [args...]
21  seconds
23 ${HOME:-.}/.profile
24 %c: invalid character in expression - %s
25 %c: unknown format specifier
26 %d-%d: invalid range
27 %d: invalid binary script version
28 %s is an alias for 
29 %s is an exported alias for 
30 %s missing
31 %s unknown base
32 %s: ':' expected for '?' operator
33 %s: Ambiguous
34 %s: Arguments must be %job or process ids
35 %s: alias not found\n
36 %s: arithmetic syntax error
37 %s: assignment requires lvalue
38 %s: bad file unit number
39 %s: bad format
40 %s: bad number
41 %s: bad option(s)
42 %s: bad substitution
43 %s: bad trap
44 %s: cannot create
45 %s: cannot execute
46 %s: cannot open
47 %s: divide by zero
48 %s: domain exception
49 %s: fails %s
50 %s: file already exists
51 %s: illegal function name
52 %s: invalid alias name
53 %s: invalid discipline function
54 %s: invalid export name
55 %s: invalid function name
56 %s: invalid name
57 %s: invalid regular expression
58 %s: invalid self reference
59 %s: invalid use of :
60 %s: invalid variable name
61 %s: is not an identifier
62 %s: is read only
63 %s: label not implemented
64 %s: limit exceeded
65 %s: more tokens expected
66 %s: no parent
67 %s: no reference name
68 %s: not found
69 %s: not implemented
70 %s: operands have incompatible types
71 %s: overflow exception
72 %s: parameter not set
73 %s: parameter null or not set
74 %s: recursion too deep
75 %s: reference variable cannot be an array
76 %s: requires pathname argument
77 %s: restricted
78 %s: singularity exception
79 %s: subscript out of range
80 %s: unbalanced parenthesis
81 %s: unknown function
82 %s: unknown locale
83 %s: unknown operator
84 %s: unknown signal name
85 %s: would cause loop
86 (coredump)
87 -c requires argument
88 -e - requires single argument
89 /vpix
90 <command unknown>
91 AC:E#?F#?H:[name]L#?R#?Z#?fi#?[base]lnprtux [name=[value]...]
92 AE#?F#?HL#?R#?Z#?fi#?[base]lnprtux [name=[value]...]
93 Abort
94 Ad:[delim]prst#[timeout]u#[filenum] [name...]
95 Alarm call
96 Bad root node specification
97 Bad system call
98 Broken Pipe
99 Bus error
100 Cannot start job control
101 Current option settings
102 DIL signal
103 Death of Child
104 DircabefhkmnpstuvxCR:[file]o:?[option] [arg...]
105 DircabefhkmnpstuvxCo:?[option] [arg...]
106 EMT trap
107 Exceeded CPU time limit
108 Exceeded file size limit
109 Floating exception
110 HSacdfmnstv [limit]
111 Hangup
112 IO signal
113 Illegal instruction
114 Interrupt
115 Killed
116 LP [dir] [change]
117 Memory fault
118 Migrate process
119 No job control
120 Phone interrupt
121 Polling alarm
122 Power fail
123 Profiling time alarm
124 Quit
125 Resources lost
126 Reverting to old tty driver...
127 S [mask]
128 SIGAPOLLO
129 Security label changed
130 Socket interrupt
131 Sound completed
132 Stopped (signal)
133 Stopped (tty input)
134 Stopped process continued
135 Stopped
136 Stopped(tty output)
137 Switching to new tty driver...
138 System crash soon
139 Terminated
140 Trace/BPT trap
141 Unrecognized version
142 Use 'exit' to terminate this shell
143 User signal 1
144 User signal 2
145 Version not defined
146 Virtual time alarm
147 Window size change
148 You have running jobs
149 You have stopped jobs
150 [_[:alpha:]]*([_[:alnum:]])
151 \n@(#)Version 12/28/93\0\n
152 \n@(#)Version M-12/28/93\0\n
153 \nreal
154 \r\n\007shell will timeout in 60 seconds due to inactivity
155 a name...
156 a:[name]c [command [args...] ]
157 afpv name...
158 alarm %s %.3f\n
159 alarm -r %s +%.3g\n
160 argument expected
161 bad directory
162 bad file unit number
163 bad substitution
164 cannot access parent directories
165 cannot create pipe
166 cannot create tempory file
167 cannot fork
168 cannot get %s
169 cannot set %s
170 cannot set alarm
171 condition(s) required
172 dsf:[library] [name...]
173 e:[editor]lnrsN# [first] [last]
174 end of file
175 f:[format]enprsu:[filenum] [arg...]
176 fnv name...
177 hist -e "${VISUAL:-${EDITOR:-vi}}" 
178 history file cannot open
179 incorrect syntax
180 invalid argument of type %c
181 is a shell builtin version of
182 is a tracked alias for
183 kill
184 line %d: $ not preceeded by \\
185 line %d: %c within ${} should be quoted
186 line %d: %s unknown label
187 line %d: %s within [[...]] obsolete, use ((...))
188 line %d: '=' obsolete, use '=='
189 line %d: -a obsolete, use -e
190 line %d: \\ in front of %c reserved for future use
191 line %d: `...` obsolete, use $(...)
192 line %d: escape %c to avoid ambiguities
193 line %d: label %s ignored
194 line %d: quote %c to avoid ambiguities
195 line %d: set %s obsolete
196 line %d: spaces required for nested subshell
197 line %d: use braces to avoid ambiguities with $id[...]
198 line %d: use space or tab to separate operators %c and %c
199 ln#[signum]s:[signame] sig...
200 login setuid/setgid shells prohibited
201 mapping
202 newline
203 nlp [job...]
204 no history file
205 no query process
206 no such job
207 no such process
208 not supported
209 off
210 on
211 open file limit exceeded
212 out of memory
213 p [action condition...]
214 p [name[=value]...]
215 parameter not set
216 permission denied
217 process already exists
218 ptx [name=[value]...]
219 pvV name [arg]...
220 r [varname seconds]
221 syntax error at line %d: `%s' %s
222 syntax error at line %d: duplicate label %s
223 syntax error: `%s' %s
224 sys
225 timed out waiting for input
226 unexpected
227 universe not accessible
228 unlimited
229 unmatched
230 user
231 versions
232 write to %d failed
233 you have mail in $_
234 zero byte
