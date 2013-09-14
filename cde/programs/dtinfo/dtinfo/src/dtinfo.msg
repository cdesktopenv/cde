
$quote "
$ $TOG: dtinfo.msg /main/16 1999/09/20 13:31:43 mgreess $
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
1 "Default message not found in catalog."


$set	2
$ 
$ messages specific to UAS/DtSearch modules
$ 
1 "The ``%s'' Search feature is not supported."
2 "quoted phrase"
3 "weight"
4 "proximity"
$       5       "xor" == Exclusive OR (Boolean)
5 "xor"


$set	4
$ 
$ messages specific to booklist window
$ 
3 "Dtinfo: Infolib Book List"
63 "Welcome to Dtinfo!"
65 "Current Section"


$set	5
$ 
$ messages specific to browser window
$ 
1 "Dtinfo: Browser"
2 "To preview a link destination, click and hold on the link."
3 "Information Library"

$       4       "The locator for this section is %s"
$       This message is the result of the "Show Locator" operation.
4 "The locator for this section is %s"


$set	6
$ 
$ messages specific to add information library dialog
$ Note: "Infolib" is a CDE glossary term.
$ 

1 "Dtinfo: Add Document Library"

$ 2     "Please enter an absolute path to the Infolib."
$       (Path must exist and include "/".)
2 "Please enter an absolute path to the Infolib."

$ 3     "Infolib open failed. Check the path specified."
$	(Path passed basic format check, but could not open target found.)
3 "Infolib open failed. Check the path specified."

$ 4     "Infolib open failed. Check entry syntax or Infolib integrity."
$       (An unidentified error occurred on attempt to open.)
4 "Infolib open failed. Check entry syntax or Infolib integrity."

$ 5     "Infolib specification format error."
5 "Infolib specification format error."

$ 6     "No Infolib specified."
6 "No Infolib specified."

$ 7     "No Infolib found or specified."
7 "No Infolib found or specified."

$ 10    "Dtinfo: Infolib Selection"
$       (For title to the File Selection Dialog)
10 "Dtinfo: Infolib Selection"

$ 11    "No Infolibs Present"
$       (This string will be shown in a directory listing where there
$        are no Infolibs found in the directory.)
11 "No Infolibs Present"


$set	7
$ 
$ dtinfo GUI label strings
$ 
1 "File"
2 "F"
4 "View"
5 "V"

$	6	"Clone"
$ "Clone" is applied to the selected entry in the booklist, and
$ has the effect of opening another (secondary) window with a
$ copy of the booklist hierarchy below the selection, and rooted
$ (based) at the selected item.
$ [Currently, this functionality is deferred beyond CDE 2.0.]
6 "Clone"
$	7	"N" for "N" in "Clone"
7 "N"

8 "Print"
9 "P"
10 "Print..."
11 "r"
12 "Close"
13 "C"
14 "Exit"
15 "x"
16 "Edit"
17 "E"
18 "Copy"
19 "C"
20 "Add..."
21 "A"
22 "Remove"
23 "R"
24 "Options"
25 "O"
26 "Preferences..."
27 "P"
28 "Auto Track"
29 "A"
30 "Show ToolBar"
31 "T"
32 "Show Search Area"
33 "S"
34 "Windows"
35 "W"
36 "Graphical Map"
37 "G"
38 "Section History"
39 "H"
40 "Marks List"
41 "M"

$	42	"Search History " presents a dialog listing
$ a history of prior search results, as provided in the Search
$ Results Dialog.
42 "Search History"
43 "S"
44 "Query Editor"
45 "Q"
46 "Scope Editor"
47 "c"
48 "Help"
49 "H"
50 "On Item..."
51 "O"
59 "Search:"
61 "Scope Editor"

$  Messages 62-77 for "Browser" pulldown menu entries
$ 
62 "Browser"
63 "B"
64 "New Window"
65 "N"
66 "Open Locator..."
67 "O"
68 "Show Locator"
69 "S"
70 "Print"
71 "P"
72 "Print..."
73 "r"
74 "Close"
75 "C"
76 "Exit"
77 "x"

$	78	"On Selection", where selection is a text
$ selection that will be used directly as a search query string
78 "On Selection"
79 "O"

80 "Query Editor"
81 "Q"
82 "Scope Editor"
83 "S"
84 "Search History"
85 "H"
86 "Clear Search Hits"
87 "C"
88 "Back (History)"
89 "B"
90 "Forward (History)"
91 "F"
92 "History..."
93 "H"
94 "Previous Section"
95 "P"
96 "Next Section"
97 "N"

$   Messages 98-101 are for traversal of the document view in order
$ to present and highlight the Previous or Next "search hit" (if any)
$ in the section. Presumably, all search hits have already been
$ highlighted to some degree, and this will add the equivalent of a
$ keyboard traversal highlight to the appropriate one when brought
$ into view.
98 "Previous Search"
99 "r"
100 "Next Search"
101 "x"

102 "Search"
103 "S"

$	104	"Go" is a pulldown menu label, where menu items are
$ various methods by which the user may "go" to a specific document.
104 "Go"
105 "G"

$   Message 106-119 are for the "Marks" pulldown menu
106 "Marks"
107 "M"
108 "Create Bookmark"
109 "B"
110 "Create Note..."
111 "N"

$	112	"List Marks" means to show all saved Marks as a list
$ in a separate dialog window.
112 "List Marks"
113 "L"

114 "Edit Selection..."
115 "E"
116 "Move Selection"
117 "M"
118 "Delete Selection"
119 "D"

$   Messages 120-129 are for "Options" pulldown menu entries
120 "Options"
121 "O"
122 "Preferences..."
123 "P"

$	124	"Detach Graphic" removes a selected graphic from the
$ document flow and places it in its own window. A "placeholder"
$ remains in the document where the graphic originally appeared.
124 "Detach Graphic"
125 "D"

126 "Show Tool Bar"
127 "T"
128 "Show Search Area"
129 "S"

$   Messages 130-135 are for "Windows" pulldown menu entries
130 "Windows"
131 "W"
132 "Book List"
133 "B"
134 "Graphical Map"
135 "G"

$   Messages 136-142	are believed to be *unused*/obsolete
136 "Author"
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
143 "Help"
144 "H"
145 "On Item..."
146 "C"
147 "On Window"
148 "W"
149 "On Application"
150 "A"
151 "Index"
152 "I"
153 "On Help"
154 "H"
155 "On Keys"
156 "K"
157 "On Version"
158 "V"

$	159	"Retain" is believed to be unused
159 "Retain"

160 "Infolib Path:"
161 "Ok"
162 "Cancel"
163 "Name"
164 "Notes"
165 "Display"

$	166	"Panner" is a graphical user interface control for
$ selecting that part of a larger area which the associated view
$ window is to display.
166 "Panner"

167 "Attach"
168 "Display Section"

$   Messages 169-182 are for the window which is used to present a
$ detached graphic.
$	169	Fit >graphic< to window
169 "Fit to Window"
$	170	Fit >window< to graphic
170 "Fit to Graphic"
171 "25 %"
172 "50 %"
173 "100 %"
174 "125 %"
175 "150 %"
176 "200 %"
$	177	"Custom" value entry for re-scaling of graphic
177 "Custom..."

$   Messages 178-182 refer to where the panner is to be placed within
$ the detached graphic dialog window. Is a "panner" masculine or
$ feminine ? "Beats" me ;-).
178 "None"
179 "Upper Left"
180 "Upper Right"
181 "Lower Left"
182 "Lower Right"

183 "Delete"
184 "Book"
185 "Section"
186 "Preferences for"
187 "Browsing"
188 "Map"
189 "Searching"
190 "Browser Geometry"
191 ". . ."
192 "Font Scale"

$	193	"Pin []" is a verb
193 "Pin Window"
194 "File a Bug"
195 "Map Geometry"
196 "Auto Update"
197 "Section History Size"
198 "Search History Size"
199 "Maximum Search Hits"
200 "Auto Display First Hit"
201 "Apply"
202 "Reset"
203 "On"
204 "Off"
205 "Number of Sections"
206 "Selected:"
207 "To Be Printed:"
208 "Error."
209 "What to Print"
210 "Sections"
211 "Hierarchy"
212 "Scope Name"
213 "Unnamed"
214 "Books"
215 "Components"
216 "New"
217 "Save"
218 "Rename"
219 "Retrieved:"
220 "Scope:"
221 "Query:"
222 "Enter a Unique Locator:"
223 "Clear"
224 "Cut"
225 "Paste"
226 "Group"
227 "Ungroup"
228 "Undo"
229 "Redo"
230 "Query"
231 "Clear All"

$   Messages 232-242 relate to operators available in a query
$ expression, and appear in the Query Editor on option menu buttons.
$	232 "Matches" means the entry in the adjacent entry field will
$	    be compared exactly to find a search index database entry.
232 "Matches"
233 "Doesn\'t Match"
234 "Starts With"
235 "Doesn\'t Start With"
$	236	Boolean operator OR
236 "Or"
$	237	"Xor" is for Boolean logic "Exclusive OR"
237 "Xor"
$	238	Boolean operator AND
238 "And"
239 "Near"
240 "Before"
241 "Weight"
242 "Proximity"
243 "Scope"
244 "Yes"
245 "No"
246 "Matches"
247 "Introduction"
248 "I"
249 "Tasks"
250 "T"
251 "Reference"
252 "R"
253 "Using Help"
254 "U"
255 "About Information Manager"
256 "A"
257 "Overview"
258 "v"
259 "o"
260 "History"
261 "History List"
262 "The section is no longer available."
263 "Click on a graphic to detach it."
264 "Raise Graphic"
265 "Attach Graphic"


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
1 "Go to the previous section in history"
2 "Go to the next section in history"
3 "Go to the previous section in document"
4 "Go to the next section in document"

$ A search "match" is the same as a search "hit". "Match" is more
$ descriptive.
5 "Go to the previous search match"
6 "Go to the next search match"

7 "Print current section"
8 "Display Book List"
9 "Display Graphical Map"
10 "Pin (unpin) this window, to prevent (allow) use for new documents."
11 "Specify search scope using the Scope Editor"
52 "Display selected section"
53 "Display selected section in a separate Book List"
54 "Print selected section"
55 "Open an additional information library"
56 "Close selected information library"
57 "Perform a search using the Query Editor"
58 "Display the bookmarks and notes list"
60 "Specify search scope"
62 "Specify search scope using the Scope Editor"
64 "Search %s"


$set	9
$ 
$ messages specific to bookmark edit dialog
$ 
1 "Dtinfo: Bookmark Editor"
2 "Bookmark update has failed."


$set	10
$ 
$ messages specific to detached graphic window
$ 
1 "Dtinfo: Detached Graphic"
2 "Detached Graphic"
3 "Raster illustration (non-scalable)"
4 "Vector illustration (scalable)"
5 "Scaling illustration, please wait..."
$ 
$ Dtinfo*SetZoom
$ 
6 "Enter the new scaling percentage:"
$ 
$ Dtinfo*SetZoomTitle
$ 
7 "Dtinfo: Set Scale"
$ 
8 "Dtinfo: "
9 "Raster illustration (scalable)"


$set	11
$ 
$ messages specific to mark list window
$ 

$	1 is the title of a window containing a list of the
$ user's bookmarks
1 "Dtinfo: User Marks List"


$set	12
$ 
$ messages specific to mark selection dialog
$ 
$   Messages 1 & 2: "Select" and "Edit" are verbs
1 "Dtinfo: Select Mark"
2 "Dtinfo: Edit Mark"


$set	13
$ 
$ messages specific to section history window
$ 
1 "Dtinfo: Section History"


$set	14
$ 
$ messages specific to preferences window
$ 
1 "Dtinfo: Preferences"
2 "Dtinfo: Get Geometry"


$set	15
$ 
$ messages specific to printing
$ 
1 "Dtinfo: Print"
2 No Print Dialog Manager can be found in the environment
3 Cannot start the Print Dialog Manager
4 Print Dialog Manager is not authorized to connect to Video display
5 Print Dialog Manager is not authorized to connect to Print display
6 Unable to display the Print Dialog Manager
7 Unable to copy default print data
8 Error printing to file
9 Print job already running
10 Dtinfo: Print Status


$set	16
$ 
$ messages specofic to search scope editor window
$ 
$	1	Edit the Scope for for use in a Search
1 "Dtinfo: Search Scope Editor"
$ 
$ Dtinfo*SaveScope
$ 
2 "Enter a name for the new scope:"
$ 
$ Dtinfo*SaveScopeTitle
$ 
3 "Dtinfo: Save Scope"
$ 
4 "All Infolibs"


$set	17
$ 
$ messages specific to search result window
$ 
$	1	"Search Results" is for the results of a search
1 "Dtinfo: Search Results"


$set	18
$ 
$ messages specific to open locator dialog, and locator
$ input formats in general.
$ 
$ 1	"Dtinfo: Open Locator"
1 "Dtinfo: Open Locator"

$ 2     "Locator specification format error."
2 "Locator specification format error."

$ 3     "Locator resolution failed."
3 "Locator resolution failed."

$ 4     "Locator resolution failed within the loaded infolibs."
4 "Locator resolution failed within the loaded infolibs."

$ 5     "Document not found."
5 "Document not found."

$ 6     "No locator value provided."
6 "No locator value provided."


$set	19
$ 
$ messages specific to query editor window
$ 
1 "Dtinfo: Query Editor"


$set	20
$ 
$ dtinfo general messages
$ 
$ 
$ Dtinfo*PixmapAllocationError
$ 
1 "There is not enough Server memory\n\
available to render the graphic."
$ 
$ Dtinfo*AbandonChanges
$ 
2 "Abandon changes?"
$ 
$ Dtinfo*NoHelp
$ 
3 "The help information cannot not be found."
$ 
$ Dtinfo*NoSelection
$ 
4 "There is no text selected.\n\
Select some text, then try again."
$ 
$ Dtinfo*AbandonBookmarks
$ 
5 "There are unsaved bookmarks open.\n\
Quit without saving them?"
$ 
$ Dtinfo*ConfirmQuit
$ 
6 "Quit Dtinfo?"
$ 
$ Dtinfo*BadTocHierarchy
$ 
7 "File a Bug"
$ 
$ Dtinfo*LinkToBookTitle
$ 
8 "Link to %s: %s"
$ 
$ Dtinfo*LinkToTitle
$ 
9 "Link to %s"
$ 
$ Dtinfo*MarkCreateFailed
$ 
10 "Dtinfo is unable to create this bookmark."
$ 
$ Dtinfo*MarkDeleteFailed
$ 
11 "Dtinfo is unable to delete this bookmark."
$ 
$ Dtinfo*MarkMoveFailed
$ 
12 "Dtinfo is unable to remove this bookmark."
$ 
$ Dtinfo*MaxDepthExceeded
$ 
13 "File a Bug"
$ 
$ Dtinfo*MarkIconInvisible
$ 
14 "The bookmark selected cannot be displayed.\n\
It is outside the viewable region of this section."
$ 
$ Dtinfo*MViewNoBase
$ 
15 "The mark selected is not\n\
in an accessible document.\n\
Load the appropriate Infolib."
$ 
$ Dtinfo*GetGeometry
$ 
16 "Click inside a Dtinfo window that has\n\
the size to be saved as the default."
$ 
$ Dtinfo*SaveModifiedScope
$ 
17 "Save changes to the current scope?"
$ 
$ Dtinfo*BadScopeName
$ 
18 "The characters colon (:), comma (,),\n\
and semicolon (\;) are not allowed\n\
in a search scope name."
$ 
$ Dtinfo*EmptyScopeName
$ 
19 "Please enter a valid scope name."
$ 
$ Dtinfo*ScopeNameTooLong
$ 
20 "A search scope name may not exceed 30 characters.\n\
Please enter a shorter scope name."
$ 
$ Dtinfo*DuplicateScopeName
$ 
21 "The name you entered is already in use.\n\
Please enter a unique scope name."
$ 
$ Dtinfo*Everything
$	22	"Everything" refers to section components like
$		titles, body, and related.
$ 
22 "Everything"
$ 
$ Dtinfo*Titles
$ 
23 "Titles"
$ 
$ Dtinfo*Body
$	24	"Body" as in the body of text.
$ 
24 "Body"
$ 
$ Dtinfo*Examples
$ 
25 "Examples"
$ 
$ Dtinfo*Index
$ 
26 "Index"
$ 
$ Dtinfo*Tables
$ 
27 "Tables"
$ 
$ Dtinfo*Graphics
$ 
28 "Graphics"
$ 
$ Dtinfo*ScopeDeleteConfirm
$ 
29 "Delete the displayed scope?"
$ 
$ Dtinfo*CurrentSection
$ 
30 "Current Section"
$ 
$ Dtinfo*IgnoredScopes
$ 
31 "Ignoring saved scopes that aren't\n\
valid for this Infolib(s)."
$ 
$ Dtinfo*MarkBaseOpenFailed
$ 
32 "Cannot open the user marks database."
$ 
$ Dtinfo*NoOpenMarkBases
$ 
33 "Dtinfo marks database was not found.\n\
Dtinfo cannot create a bookmark."
$ 
$ Dtinfo*MarkBaseReadOnly
$ 
34 "The current marks database has read-only permissions."
$ 
35 "Unknown error in document: Please notify product support."

36 "An unexpected message was received from the\n\
search engine (in UAS_SearchEngine):\n\
Please notify product support."
$ 
$ Dtinfo*TooManyIbases
$ 
37 "Due to a search engine limitation only the first %d\n\
bookcases in the current scope may be searched.\n\
Continue?"
$ 
$ Dtinfo*DemoFailure
$ 
38 "This is an Evaluation Version of the Browser.\n\
There is no permission to access this Infolib."
$ 
$ Dtinfo*StyleSheetSyntaxError
$ 
39 "There is an error in the Style Sheet for this document.\n\
Default features have been applied."
$ 
$ Dtinfo*NoHits
$ 
40 "No matches were found for this query."
$ 
$ Dtinfo*BadQuery
$ 
41 "Dtinfo does not understand the query. \n\
Please enter a valid query."
$ 
$ Dtinfo*SearchError
$ 
42 "An internal search engine error has been detected. All\n\
subsequent searching will fail for this Browser session.\n\n\
This problem has been logged and will be fixed in a later\n\
release of the Browser. As a temporary workaround, try to\n\
avoid entering queries that cause this condition to occur.\n\
To re-enable searching, quit and restart the Browser."
$ 
$ Dtinfo*KernelError
$ 
43 "An internal search engine error has been detected.\n\
It\'s likely that this machine\'s /usr/tmp directory\n\
is on (or links to) a tmpfs filesystem. All subsequent\n\
searching will fail for this Browser session.\n\n\
This problem is known and has been logged. A workaround\n\
is to reconfigure the /usr/tmp directory on this machine\n\
so that it is not on (or linked to) a tmpfs filesystem.\n\
After do so and restarting the Browser, searching should\n\
be re-enabled and work properly. For more information see\n\
a system administrator."
$ 
$ Dtinfo*FulcrumNoMemory
$ 
44 "The search engine was unable to complete your search request\n\
because it could not allocate memory. Before attempting to\n\
search again try quitting other applications, or increase\n\
the system's physical or virtual memory."
$ 
$ Dtinfo*FulcrumError
$ 
45 "A Search Engine error has occurred. \n\
Please report Search Error %d to Product Support."
$ 
$ Dtinfo*BadHelp
$ 
46 "This button links to an Dtinfo document which is\n\
not present in the currently loaded Infolib(s)."
$ 
$ Dtinfo*query_is_read_only
$ 
47 "This is a display-only field.  To edit the query\n\
use the Query Editor or the quick search field in\n\
the Reading window or Booklist window."
$ 
$ Dtinfo*ExceedRdWindows
$ 
48 "The maximum number of active reading windows (%d)\n\
as set under User Preferences is already reached."
$ 
$ Dtinfo*NoPrefBase
$ 
49 "There is no User Preference Database available."
$ 
$ Dtinfo*PrefBaseReadOnly
$ 
50 "The User Preference Database has been opened in\n\
read-only mode. No new preferences can be saved to disk."
$ 
$ Dtinfo*DeadEndLink
$ 
51 "The document or section requested is not available."
$ 
$ Dtinfo*MarkPositionFailed
$ 
52 "Dtinfo cannot display the mark named\n\
\42%s\42\n\
because the paragraph it was associated with\n\
no longer exists."
$ 
$ Dtinfo*ScopeChangeAbandon
$ 
53 "Abandon changes to the current scope?"
$ 
$ Dtinfo*InformationLibrary
$ 
63 "Information Library"
$ 
$ Dtinfo*On
$ 
64 "On"
$ 
$ Dtinfo*Off
$ 
65 "Off"
$ 
67 "Could not create the Infolib."

68 "Infolib creation failed."
$ 
69 "Search result weighting is not available."

$	70	The "qualified locator" is a "fully qualified
$		locator" as opposed to the simple format.
70 "Invalid access method in qualified locator (URL)."

71 "Document creation failed."

72      "Dtinfo: Detach Graphic"
73      "Click on a graphic to detach it."



$set	21
$ 
$ messages specific to message dialogs
$ 

$ Dtinfo*ErrorDialogTitle
$ 
1 "Dtinfo: Error"

$ Dtinfo*WarningDialogTitle
$ 
2 "Dtinfo: Warning"

$ Dtinfo*InfoDialogTitle
$ 
3 "Dtinfo: Information"

$ Dtinfo*ProgressDialogTitle
$ 
$	4	"Progress" is a level of completion in regards
$		 to a task.
4 "Dtinfo: Progress"

$ Dtinfo*QuitDialogTitle
$ 
5 "Dtinfo: Confirm Quit"

$ Dtinfo*QuestionDialogTitle
$ 
6 "Dtinfo: Question"
$ 
7 "Dtinfo: File a Bug"


$set	22
$ 
$ dtinfo version information
$ 
$ 
$	Dtinfo*Version
$ 
1 "CDE Dtinfo Browser\n\
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
1 "No access part provided in locator (URL)."
2 "Don't know how to create %s: objects"

$ In the following, do not translate any part of "UAS_ObjList<T>::operator[]"
3 "Range Error: UAS_ObjList<T>::operator[]"
4 "Range Error: UAS_PtrList<T>::operator[]"


$set	27
$ 
$ messages specific to UAS/MMDB modules
$ 
1 "The following bookcases are not valid:"
2 "Invalid Infolib path: %s"
3 "Ignoring invalid Infolib path: %s"
4 "MMDB Library %d"
5 "MMDB Infolib: "
6 "Infolib %s"


$set	28
$ 
$ messages specific to graphical map window
$ 
1 "Dtinfo: Local Map"
2 "Dtinfo: "


$set	29
$ 
$ messages specific to search history window
$ 
$	1	"Search Results History" is a history of the
$ search results already obtained under the dtinfo session, for
$ prior queries.
1 "Dtinfo: Search Results History"


$set	30
$ 
$ messages specific to window system module
$ 
1 "Dtinfo: Fatal Error"
2 "Terminating..."
3 Unable to open display.\n

$set	31
$ 
$ messages specific to printing
$ 
1 "Some page header"
2 "Some page footer - page %d"


$set	40
$ 
$ messages specific to environmental interface or program context
$ 

$ 1     ""
1 "Not implemented."


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
1 Can't get tt session for X display %s

$ 2     "Could not set default tooltalk session."
2 Could not set default tooltalk session.

$ 3 Failed to open ToolTalk connection.
3 Failed to open ToolTalk connection.

$ 4     "Could not declare ptype DtInfo."
4 Could not declare ptype DtInfo.

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
1 "DtSearch does not support the query."


$set 43
$ *****************************************************************************
$ **
$ ** Message set for  ToolTalkError Dialog
$ **
$ *****************************************************************************
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 "Could not connect to ToolTalk:\n%s\n"
