$ $XConsortium: Agent.msg /main/14 1996/10/26 13:24:40 cde-hal $

$set	Set_AgentLabel
$ 
$ dtinfo GUI label strings
$ 
1	"File"
2	"F"
4	"View"
5	"V"

$	6	"Clone"
$ "Clone" is applied to the selected entry in the booklist, and
$ has the effect of opening another (secondary) window with a
$ copy of the booklist hierarchy below the selection, and rooted
$ (based) at the selected item.
$ [Currently, this functionality is deferred beyond CDE 2.0.]
6	"Clone"
$	7	"N" for "N" in "Clone"
7	"N"

8	"Print"
9	"P"
10	"Print..."
11	"r"
12	"Close"
13	"C"
14	"Exit"
15	"x"
16	"Edit"
17	"E"
18	"Copy"
19	"C"
20	"Add..."
21	"A"
22	"Remove"
23	"R"
24	"Options"
25	"O"
26	"Preferences..."
27	"P"
28	"Auto Track"
29	"A"
30	"Show ToolBar"
31	"T"
32	"Show Search Area"
33	"S"
34	"Windows"
35	"W"
36	"Graphical Map"
37	"G"
38	"Section History"
39	"H"
40	"Marks List"
41	"M"

$	42	"Search History " presents a dialog listing
$ a history of prior search results, as provided in the Search
$ Results Dialog.
42	"Search History"
43	"S"
44	"Query Editor"
45	"Q"
46	"Scope Editor"
47	"c"
48	"Help"
49	"H"
50	"On Item..."
51	"O"
59	"Search:"
61	"Scope Editor"

$  Messages 62-77 for "Browser" pulldown menu entries
$ 
62	"Browser"
63	"B"
64	"New Window"
65	"N"
66	"Open Locator..."
67	"O"
68	"Show Locator"
69	"S"
70	"Print"
71	"P"
72	"Print..."
73	"r"
74	"Close"
75	"C"
76	"Exit"
77	"x"

$	78	"On Selection", where selection is a text
$ selection that will be used directly as a search query string
78	"On Selection"
79	"O"

80	"Query Editor"
81	"Q"
82	"Scope Editor"
83	"S"
84	"Search History"
85	"H"
86	"Clear Search Hits"
87	"C"
88	"Back (History)"
89	"B"
90	"Forward (History)"
91	"F"
92	"History..."
93	"H"
94	"Previous Section"
95	"P"
96	"Next Section"
97	"N"

$   Messages 98-101 are for traversal of the document view in order
$ to present and highlight the Previous or Next "search hit" (if any)
$ in the section. Presumably, all search hits have already been
$ highlighted to some degree, and this will add the equivalent of a
$ keyboard traversal highlight to the appropriate one when brought
$ into view.
98	"Previous Search"
99	"r"
100	"Next Search"
101	"x"

102	"Search"
103	"S"

$	104	"Go" is a pulldown menu label, where menu items are
$ various methods by which the user may "go" to a specific document.
104	"Go"
105	"G"

$   Message 106-119 are for the "Marks" pulldown menu
106	"Marks"
107	"M"
108	"Create Bookmark"
109	"B"
110	"Create Note..."
111	"N"

$	112	"List Marks" means to show all saved Marks as a list
$ in a separate dialog window.
112	"List Marks"
113	"L"

114	"Edit Selection..."
115	"E"
116	"Move Selection"
117	"M"
118	"Delete Selection"
119	"D"

$   Messages 120-129 are for "Options" pulldown menu entries
120	"Options"
121	"O"
122	"Preferences..."
123	"P"

$	124	"Detach Graphic" removes a selected graphic from the
$ document flow and places it in its own window. A "placeholder"
$ remains in the document where the graphic originally appeared.
124	"Detach Graphic"
125	"D"

126	"Show Tool Bar"
127	"T"
128	"Show Search Area"
129	"S"

$   Messages 130-135 are for "Windows" pulldown menu entries
130	"Windows"
131	"W"
132	"Book List"
133	"B"
134	"Graphical Map"
135	"G"

$   Messages 136-142	are believed to be *unused*/obsolete
136	"Author"
137	"A"
138	"Get Locator"
139	"Debug"
140	"D"
141	"Generate Fake Infobase List"
142	"Command Processor"

$   Messages 143-158 are Help pulldown menu entries, and
$ should of course read the same across all CDE applications,
$ within a localization. They always mean the same thing:
$ provide "help On []"
143	"Help"
144	"H"
145	"On Item..."
146	"C"
147	"On Window"
148	"W"
149	"On Application"
150	"A"
151	"Index"
152	"I"
153	"On Help"
154	"H"
155	"On Keys"
156	"K"
157	"On Version"
158	"V"

$	159	"Retain" is believed to be unused
159	"Retain"

160	"Infolib Path:"
161	"Ok"
162	"Cancel"
163	"Name"
164	"Notes"
165	"Display"

$	166	"Panner" is a graphical user interface control for
$ selecting that part of a larger area which the associated view
$ window is to display.
166	"Panner"

167	"Attach"
168	"Display Section"

$   Messages 169-182 are for the window which is used to present a
$ detached graphic.
$	169	Fit >graphic< to window
169	"Fit to Window"
$	170	Fit >window< to graphic
170	"Fit to Graphic"
171	"25 %"
172	"50 %"
173	"100 %"
174	"125 %"
175	"150 %"
176	"200 %"
$	177	"Custom" value entry for re-scaling of graphic
177	"Custom..."

$   Messages 178-182 refer to where the panner is to be placed within
$ the detached graphic dialog window. Is a "panner" masculine or
$ feminine ? "Beats" me ;-).
178	"None"
179	"Upper Left"
180	"Upper Right"
181	"Lower Left"
182	"Lower Right"

183	"Delete"
184	"Book"
185	"Section"
186	"Preferences for"
187	"Browsing"
188	"Map"
189	"Searching"
190	"Browser Geometry"
191	". . ."
192	"Font Scale"

$	193	"Pin []" is a verb
193	"Pin Window"
194	"File a Bug"
195	"Map Geometry"
196	"Auto Update"
197	"Section History Size"
198	"Search History Size"
199	"Maximum Search Hits"
200	"Auto Display First Hit"
201	"Apply"
202	"Reset"
203	"On"
204	"Off"
205	"Number of Sections"
206	"Selected:"
207	"To Be Printed:"
208	"Error."
209	"What to Print"
210	"Sections"
211	"Hierarchy"
212	"Scope Name"
213	"Unnamed"
214	"Books"
215	"Components"
216	"New"
217	"Save"
218	"Rename"
219	"Retrieved:"
220	"Scope:"
221	"Query:"
222	"Enter a Unique Locator:"
223	"Clear"
224	"Cut"
225	"Paste"
226	"Group"
227	"Ungroup"
228	"Undo"
229	"Redo"
230	"Query"
231	"Clear All"

$   Messages 232-242 relate to operators available in a query
$ expression, and appear in the Query Editor.
232	"Matches"
233	"Doesn't Match"
234	"Starts With"
235	"Doesn't Start With"
$	236	Boolean operator OR
236	"Or"
$	237	"Xor" is for Boolean logic "Exclusive OR"
237	"Xor"
$	238	Boolean operator AND
238	"And"
239	"Near"
240	"Before"
241	"Weight"
242	"Proximity"
243	"Scope"
244	"Yes"
245	"No"
246	"Matches"
247	"Introduction"
248	"I"
249	"Tasks"
250	"T"
251	"Reference"
252	"R"
253	"Using Help"
254	"U"
255	"About Information Manager"
256	"A"
257	"Overview"
258	"v"
259	"o"
260	"History"
261	"History List"
262	"The section is no longer available."
263	"Click on a graphic to detach it."
264	"Raise Graphic"
265	"Attach Graphic"

$	270  "..." is a button label indicating a list from which an
$            entry may be selected to fill a preceeding text field.
$            In this case, the user may enter a path into a text field
$            or activate the button with "..." to be presented with
$            a File Selection Box. The object is to select an Infolib
$            path.
270	"..."


$set	Set_AgentQHelp
$ 
$ dtinfo quick help messages (to be exhibited on status display area)
$ 
1	"Go to the previous section in history"
2	"Go to the next section in history"
3	"Go to the previous section in document"
4	"Go to the next section in document"

$ A search "match" is the same as a search "hit". "Match" is more
$ descriptive.
5	"Go to the previous search match"
6	"Go to the next search match"

7	"Print current section"
8	"Display Book List"
9	"Display Graphical Map"
10	"Pin (unpin) this window, to prevent (allow) use for new documents."
11	"Specify search scope using the Scope Editor"
52	"Display selected section"
53	"Display selected section in a separate Book List"
54	"Print selected section"
55	"Open an additional information library"
56	"Close selected information library"
57	"Perform a search using the Query Editor"
58	"Display the bookmarks and notes list"
60	"Specify search scope"
62	"Specify search scope using the Scope Editor"
64	"Search %s"

