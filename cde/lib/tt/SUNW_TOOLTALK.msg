$ $TOG: SUNW_TOOLTALK.msg /main/5 1999/09/16 13:45:26 mgreess $
$quote "
$set 1
3	"clnt_create for rpc.ttdbserverd on %s failed%s"
4	"Error: rpc.ttdbserverd on %s is not running"
6	"server version (%s) does notmatch the version of the database tables \
(%s). Please run a version %s database server"
7	"Attempt to open database table %s:%s failed"
14	"could not delete the X root window property %s advertising this sess\
ion"
15	"address version is %d, but I only understand %d! (address: <%s>)"
17	"could not parse X display name: \"%s\""
18	"could not queue a message on file \"%s\" because of internal error %\
d"
19	"could not delete stale session <%s> from interest list of file \"%s\"\
 because of internal error %d"
20	"could not delete uninterested session <%s> from interest list of fil\
e \"%s\" because of internal error %d"
21	"tttracefile syntax error in line:\n%s"
22	"Could not process tttracefile in $%s"
23	"No tracing allowed if real UID does not equal effective UID"
24	"Accepting request"
25	"empty document and no file"
26	"cannot get messageID; operation may not be cancel-able, because tt_m\
essage_arg_val()"
27	"cannot get title; document will be untitled because tt_message_arg_v\
al()"
28	"unknown opnum offset"
$set 2
2	"indistinguishable signaturesfound for ptype %s -"
3	"No connection-oriented transport"
4	"A ToolTalk client died before it could be signalled to retrieve a mes\
sage it was due"
5	"Connection to ToolTalk client lost while signalling it to retrieve a \
message"
6	"Found another session running (host=%s, pid=%d)"
7	"Can't contact alleged session (host=%s, pid=%d); assuming it crashed.\
.."
8	"Can't parse advertised session id; overwriting it with my own..."
9	"%s is a version %d types database, and this version can only read ver\
sions %d and earlier"
10	"could not decode types from types database: %s. It may be damaged."
11	"Overwrote %s"
12	"Ignoring unknown attribute <%s> of ToolTalk signature..."
13	"ttce2xdr failed (status=%d); types in Classing Engine \"user\" datab\
ase not converted..."
14	"duplicate ISAM record"
15	"ISAM file not open"
16	"illegal ISAM argument"
17	"illegal ISAM key descriptor"
18	"too many ISAM files open"
19	"bad ISAM file format"
20	"ISAM non-exclusive access"
21	"ISAM record locked"
22	"ISAM key already exists"
23	"ISAM key is primary key"
24	"end or beginning of ISAM file"
25	"no ISAM record found"
26	"no current ISAM record"
27	"ISAM file locked"
28	"ISAM file name too long"
29	"ISAM cannot allocate memory"
30	"ISAM RPC timeout"
31	"Broken ISAM TCP/IP connection"
32	"Cannot connect to ISAM server"
33	"Cannot import ISAM data"
34	"no local SAM daemon"
35	"ISAM internal fatal error"
36	"ISAM Locale/LANG mismatch"
$set 3
2	"child (%d) exited due to signal %d%s"
3	" (core dumped)"
4	"child (%d) neither exited nor was signaled!"
5	"cannot maximize clients because %m"
6	"No scope to manage.  Use -c, -p, -d, or set $DISPLAY."
7	"Cannot use -S option with -c"
8	"child ttsession exited with status %d"
9	"child ttsession exited due to signal %d%s"
10	" (core dumped)"
11	"child ttsession neither exited nor was signaled!"
12	"starting"
13	"have re-read types"
14	"error in types; keeping old types"
15	"$OPENWINHOME not set"
16	"\nUsage: ttsession [-a unix|des][-d display][-spStvhNX"
17	"][-c command]\n -c [command]	start a process tree session, and run c\
ommand in it.\n		Subsequent options are passed to command.  Default: $SH\
ELL\n -p		start a process tree session, and print its id\n -d display	st\
art an X session on display\n\n -a unix|des	set server authentication le\
vel\n -s		silent. Don't print out any warnings\n -S		don't fork into the\
 background\n -N		maximize the number of clients allowed\n -t		turn on m\
essage tracing\n -X		use XDR databases for static types (default)\n"
18	" -E		use Classing Engine for static types\n"
19	"\n -v		print out version number\n -h		print out this message\n\nSign\
al interface:\n kill -USR1 ttsession_pid	toggle message tracing\n kill -\
USR2 ttsession_pid	re-read static types"
20	"exiting"
$set 4
2	"Usage:\ntt_type_comp [-s] [-d db] [-mM] source_file\ntt_type_comp [-s\
] [-d db] -r type ...\n-M	merge source types into specified database, no\
t updating existing types\n-m	merge, but update existing types.  Default\
.\n-r	remove source types from the specified database\n-d db	database to\
 operate on. One of: user, system, or network. Default: user\n-G	perform\
 garbage collection on the ToolTalk database server.\n\ntt_type_comp [-s\
E] -p|O|P [-d db]\ntt_type_comp [-s]  -p|O|P compiled_file\n-O	enumerate\
 on stdout the names of all otypes read\n-P	enumerate on stdout the name\
s of all ptypes read\n-p	pretty-print on stdout all the ToolTalk types r\
ead\n-E	use the Classing Engine database(s) instead of the XDR database(\
s)\n-d db	database to read from. One of: user, system, or network. Defau\
lt: all\n\ntt_type_comp [-s] -x [-o compiled_file] source_file\n-x	compi\
le types from source_file (or stdin, if file is \"-\")\n-o	write compile\
d types to compiled_file (or stdout, if file is \"-\")\n	Default: source\
_file.xdr, or \"types.xdr\" if source is stdin\n\ntt_type_comp [-hv]\n-v\
	print out version number\n-h	print out this message\n-s	do not print ou\
t any status messages.\n\nThese cpp options will be passed through:\n   \
     -undef -Dname -Idirectory -Uname -Ydirectory"
3	"Semantic error in types file"
4	"Not a valid ToolTalk types file"
5	"Cannot read any ToolTalk types from Classing Engine database"
6	"Merging Classing Engine tables is no longer supported"
7	"Cannot read types in %s database - version mismatch"
8	"Cannot read types in %s database"
9	"$OPENWINHOME not set"
10	"Cannot initialize %s database for writing"
11	"Could not remove old definition for %s"
12	"Overwriting"
13	"Writing"
14	"Could not add new definition for %s"
15	"Could not remove old definition for %s"
16	"Overwriting"
17	"Writing"
18	"Could not add new definition for %s"
19	"Version mismatch in compiled types"
20	"Cannot read types in database"
21	"$OPENWINHOME not set"
22	"Cannot read types in %s database - version mismatch"
23	"Cannot read types in %s database"
24	"$OPENWINHOME not set"
25	"Removing otype %s\n"
26	"Removing ptype %s\n"
27	"output written to %s\n"
28	"Invalid database: %s"
29	"Specify only one of the options -O -P -m -M -p -r -x"
30	"no preprocessing done because: %s: %s"
31	"otype inheritance cycle involving %s"
32	"ancestor %s of otype %s does not exist"
33	"ptype %s does not exist, but was named as implementorby otype %s"
34	"multiple handlers defined in otype %s"
35	"inherited signature in otype %s does not exist in parent"
36	"two ptypes, %s and %s, with the same handler"
37	"ancestor %s of otype %s does not exist"
38	"cyclic otype inheritance hierarchy -\n  {"
39	"duplicated definitions of otype %s"
40	"duplicated definitions of ptype %s"
$set 5
3	"Security violation: RPC call wanted me to open a file that is not a T\
oolTalk database"
4	"Any data written using an old (<= 1.0.2) rpc.ttdbserverd after using \
a new (>= 1.1) rpc.ttdbserverd will be ignored"
5	"Any data written using an old (<= 1.0.2) rpc.ttdbserverd after using \
a new (>= 1.1) rpc.ttdbserverd will be ignored"
6	"rpc.ttdbserverd version (%s) does not match the version (%s) of the d\
atabase tables. Please install an rpc.ttdbserverd version %s (or greater\
)"
7	"Usage:\nrpc.ttdbserverd [-S] [-n] [-m DTMOUNTPOINT]\nrpc.ttdbserverd \
[-S] [-v]"
8	"Unable to start garbage collection thread. thr_create()\n"
9	"Unable to fork() for garbage collection.\n"
$set 6
2	"Usage:\nttdbck [-f file] [-k objkey] [-t type] [-bx] \n[-impa] [-IZ] \
[-F newfilename] [-T newtype] [mountpoints]\n"
3	"ttdbck: you must specify a selection [-fkt] option or a diagnosis [-b\
x] option\nif a repair [-FTZ] option is specified\n"
4	"Version mismatch in compiled types"
5	"Cannot read types in database"
6	"ttdbck: try 'ttdbck -I'.\n"
7	"Select by filename: %s\n"
8	"Select by objid key:"
9	"Select by type: %s\n"
10	"Diagnose badly formed entities\n"
11	"Diagnose references to non-existent entities\n"
12	"Display ids\n"
13	"Display mandatory data\n"
14	"Display properties and values data\n"
15	"Invoke NetISAM isrepair() function before inspecting\n"
16	"Repair by setting to type: %s\n"
17	"Repair by deleting\n"
18	"Debugging printout level %d\n"
19	"Repair by setting to file: %s\n"
20	"Error: no file for spec.\n"
21	"Error: no type for spec.\n"
22	"Error: \"%s\" is not an installed otype.\n"
23	"Error: spec has multiple values for type property.\n"
24	"Error: "
25	"ttdbck: no errors found.\n"
26	"Error: "
$set 7
2	"Usage: %s {ctx}[fhpPv[v]] [tarfile] pathname ...\n       %s {ctx}fL[h\
pPRv[v]] tttarfile pathname ...\n       %s -v\n       %s -h\n"
3	"\tc       create an archive\n\tt       list an archive's contents\n\t\
x       extract from an archive\n\tf       use next arg <tarfile> as arc\
hive\n\th       follow symbolic links\n\tL       do not invoke tar(1)\n\t\
p       preserve file modes\n\tP       (root) do not preserve objects' o\
wner, mod time, etc.\n\tR       do not recurse into directories\n\tv    \
   be verbose\n\tvv      be very verbose\n\t-v      print the version nu\
mber and quit\n\t-h[elp] print this message\n"
4	"%s: Could not read object kind from archive stream.\n"
5	"%s: Could not read archive version from archive stream.\n"
6	"%s: Found archive version %d, but expected version %d.\n"
7	"%s: found object of unknown kind %d in archive.\n"
8	"%s: Could not read object kind from archive stream.\n"
9	"%s: Could not read archive version from archive stream.\n"
10	"%s: Found archive version %d, but expected version %d.\n"
11	"%s: found object of unknown kind %d in archive.\n"
$set 8
2	"%s: Could not remove ToolTalk objects of %s because %s\n"
3	"%s: Could not move ToolTalk objects of \"%s\" to \"%s\" because %s\n"
4	"%s: Will not attempt to move the ToolTalk objects of:\n"
5	"Usage: %s [-] [-fL] path1 path2\n       %s [-] [-fL] path1 [path2 ...\
] dir\n       %s -v\n       %s -h\n"
6	"\t-L      do not perform a mv(1)\n\t-v      print the version number \
and quit\n\t-h      print this message\n"
7	"%s: Could not remove ToolTalk objects of %s because %s\n"
8	"Usage: %s [-] [-%s] %s ...\n       %s -v\n       %s -h\n"
9	"file"
10	"dir"
11	"\t-L      do not perform a %s(1)\n\t-v      print the version number\
 and quit\n\t-h[elp] print this message\n"
12	"%s: Could not remove ToolTalk objects of %s because %s\n"
13	"Usage: %s [-pL] file1 file2\n       %s [-prRL] path1 [path2 ...] dir\
\n       %s -v\n       %s -h\n"
14	"\t-L      do not perform a cp(1)\n\t-v      print the version number\
 and quit\n\t-h      print this message\n"
$set 9
2	"Usage: %s [-0FCa][-o outfile] [-S session | command [options]]\n     \
  %s [-e script | -f scriptfile][-S session | command [options]]\n -0		T\
urn off message tracing in session, or run command\n		without message tr\
acing (i.e. only API tracing)\n -F		Follow all children forked by comman\
d or subsequently\n		started in session by ttsession(1)\n -C		Do not tra\
ce ToolTalk API calls\n -a		Print all attributes, arguments, and context\
 slots of\n		traced messages.  Default is single-line summary.\n -e scri\
pt	Read tttracefile(4) settings from script\n -f scriptfile	Read tttrace\
file(4) settings from scriptfile. \"-\": stdin.\n -o outfile	Output. \"-\
\": stdout. default: stdout for session tracing,\n		stderr (of tttrace) \
for command tracing\n -S session	Session to trace.  default: see tt_defa\
ult_session()\n command	ToolTalk client command to invoke and trace\n"
3	"%s: session <%s> does not support Session_Trace.  Use kill -USR1 inst\
ead. See ttsession(1).\n"
$set 10
2	"Ignoring unknown Tt_address passed to Tts_address::add()\n"
3	"Ignoring Unknown Tt_address passed to Tts_address::remove()\n"
4	"Ignoring Unknown address passed to tts_address::tts_set()\n"
5	"Ignoring Unknown address passed to tts_address::tts_unset()\n"
6	"Ignoring Unknown address read by tts_address::load()\n"
7	"Tts_arglist::Tts_arglist - error reading argument - \n\t"
8	"Tts_arglist::load - Unknown arg type encountered - using BARG\n"
9	"Abstract class Tts_arg::assign (Tt_pattern) called - ignoring\n"
10	"Abstract class Tts_arg::assign(Tt_message) called - ignoring\n"
11	"Abstract class Tts_arg::generate called - ignoring\n"
12	"Abstract class Tts_arg::save called - ignoring\n"
13	"Abstract class Tts_arg::load called - ignoring\n"
14	"Tts_arg::mode_to_chars invalid mode - using TT_INOUT\n"
15	"Tts_arg::chars_to_mode invalid mode - using TT_INOUT\n"
16	"Ignoring unknown Tt_category passed to Tts_category::set()\n"
17	"Tts_category::generate() called for a message - ignoring\n"
18	"Ignoring Unknown category read by tts_category::load()\n"
19	"Ignoring unknown Tt_class passed to Tts_class::add()\n"
20	"Ignoring Unknown Tt_class passed to Tts_class::remove()\n"
21	"Ignoring Unknown class passed to tts_class::tts_set()\n"
22	"Ignoring Unknown class passed to tts_class::tts_unset()\n"
23	"Ignoring Unknown class read by tts_class::load()\n"
24	"Tts_connection::universal_callback - no pattern molding for pattern!\
\n"
25	"Tts_cntxtlist::Tts_cntxtlist - error reading context - \n\t"
26	"Tts_contextlist::load - Unknown cntxt type encountered - using BCNTX\
T\n"
27	"Abstract class Tts_context::assign (Tt_pattern) called - ignoring\n"
28	"Abstract class Tts_context::assign (Tt_pattern) called - ignoring\n"
29	"Abstract class Tts_context::generate called - ignoring\n"
30	"Abstract class Tts_context::save called - ignoring\n"
31	"Abstract class Tts_context::load called - ignoring\n"
32	"Ignoring unknown Tt_disposition passed to Tts_disposition::add()\n"
33	"Ignoring Unknown Tt_disposition passed to Tts_disposition::remove()\n\
"
34	"Ignoring Unknown disposition passed to tts_disposition::tts_set()\n"
35	"Ignoring Unknown disposition passed to tts_disposition::tts_unset()\n\
"
36	"Ignoring Unknown disposition read by tts_disposition::load()\n"
37	"Called %s\n"
38	"ToolTalk Error: %s\n        Origin: Source File %s Line %d\n"
39	"Warning: %s\n        Origin: Source File %s Line %d\n"
40	" Tts_message_molding::send - Attempt to send uninitialized message m\
olding\n"
41	"Ignoring unknown Tt_scope passed to Tts_scope::add()\n"
42	"Ignoring Unknown Tt_scope passed to Tts_scope::remove()\n"
43	"Ignoring Unknown scope passed to tts_scope::tts_set()\n"
44	"Ignoring Unknown scope passed to tts_scope::tts_unset()\n"
45	"Ignoring Unknown scope read by tts_scope::load()\n"
46	"Ignoring unknown Tt_state passed to Tts_state::add()\n"
47	"Ignoring Unknown Tt_state passed to Tts_state::remove()\n"
48	"Ignoring Unknown state passed to tts_state::tts_set()\n"
49	"Ignoring Unknown state passed to tts_state::tts_unset()\n"
50	"Tts_state::generate() called for a message - ignoring\n"
51	"Ignoring Unknown state read by tts_state::load()\n"
82	"Obtaining TTSnoop Help"
83	"----------------------"
84	"Help for individual buttons and settings can be obtained by"
85	"pressing the help key while the mouse is over the button or"
86	"setting of interest. To see what api calls are being used"
87	"by ttsnoop, use the -t switch when invoking ttsnoop."
88	""
89	"Using TTSnoop"
90	"-------------"
91	"To observe messages select the Start setting choice."
92	""
93	"To turn off message observation select the Stop setting choice."
94	""
95	"To limit the types of messages:"
96	"      1) Select Patterns button"
97	"      2) Enter the type of pattern you want to observe"
98	"      3) Select Apply button"
99	""
100	"To highlight information that is displayed:"
101	"      1) Select Display button"
102	"      2) Mark those items that you want to highlight"
103	"      3) Select Apply button"
104	""
105	"To send a message:"
106	"      1) Select the Messages button"
107	"      2) Compose the message"
108	"      3) Select the Send Message button"
109	""
110	"To store a message:"
111	"      1) Select the Message button"
112	"      2) Compose the message"
113	"      3) Select the Add Message button"
114	""
115	"To send a stored message:"
116	"      Select the message from the Send Message menu"
117	""
118	"To clear the message output window select the Clear button."
119	""
120	"General ToolTalk Information"
121	"----------------------------"
122	"Before sending or receiving object oriented"
123	"messages through ToolTalk, a process must"
124	"register with the message passer. By registering,"
125	"the process tells ToolTalk several things:"
126	""
127	"      The process type (ptype) of the process."
128	"      This allows the message passer to direct"
129	"      messages implemented by this ptype to"
130	"      this process."
131	""
132	"      The sessions the process participates in."
133	""
134	"      The documents that the process is"
135	"      observing. Messages to objects in these"
136	"      documents can then be routed to the"
137	"      process."
138	""
139	"Note that these registrations generally modify"
140	"generated patterns. It would be possible to use"
141	"the pattern manipulation API calls to observe or"
142	"handle object-oriented messages, but the methods"
143	"described here are much shorter since they take"
144	"advantage of the information declared in the type"
145	"definitions."
146	""
147	"Use tt_session_join to become part of a session."
148	"When joining, you must provide the session id of"
149	"the session to join."
150	""
151	"When your tool no longer needs ToolTalk services,"
152	"quit the session with tt_session_quit."
153	""
154	"When your tool loads a file, it should join the"
155	"collection of processes interested in that file"
156	"by calling tt_file_join(); when through with the"
157	"file, quit the collection by calling"
158	"tt_file_quit(). Some tools may have several files"
159	"open at once; other tools may only open one at a time."
160	"Using TTSnoop"
161	"-------------"
162	"To observe messages select the Start setting choice."
163	"To turn off message observation select the Stop setting choice."
164	""
165	"To limit the types of messages:"
166	"        1) Select Patterns button"
167	"        2) Enter the type of pattern you want to observe"
168	"        3) Select Apply button"
169	""
170	"To highlite information that is displayed:"
171	"        1) Select Display button"
172	"        2) Mark those items that you want to highlite"
173	"        3) Select Apply button"
174	""
175	"To send a message:"
176	"        1) Select the Messages button"
177	"        2) Compose the message"
178	"        3) Select the Send Message button"
179	""
180	"To store a message:"
181	"        1) Select the Message button"
182	"        2) Compose the message"
183	"        3) Select the Add Message button"
184	""
185	"To send a stored message:"
186	"        Select the message from the Send Message menu"
187	""
188	"To clear the message output window select the Clear button."
189	""
190	"General ToolTalk Information"
191	"----------------------------"
192	"Before sending or receiving object oriented messages through"
193	"ToolTalk, a process must register with the message passer. By"
194	"registering, the process tells ToolTalk several things:"
195	""
196	"        The process type (ptype) of the process. This allows"
197	"        the message passer to direct messages implemented by"
198	"        this ptype to this process."
199	""
200	"        The sessions the process participates in."
201	""
202	"        The documents that the process is observing. Messages"
203	"        to objects in these documents can then be routed to the"
204	"        process."
205	""
206	"Note that these registrations generally modify generated patterns."
207	"It would be possible to use the pattern manipulation API calls to"
208	"observe or handle object-oriented messages, but the methods"
209	"described here are much shorter since they take advantage of the"
210	"information declared in the type definitions."
211	""
212	"Use tt_session_join to become part of a session. When joining, you"
213	"must provide the session id of the session to join."
214	""
215	"When your tool no longer needs ToolTalk services, quit the session"
216	"with tt_session_quit."
217	""
218	"When your tool loads a file, it should join the collection of"
219	"processes interested in that file by calling tt_file_join(); when"
220	"through with the file, quit the collection by calling tt_file_quit(\
)."
221	"Some tools may have several files open at once; other tools may"
222	"only open one at a time."
223	"Start/Stop receiving and displaying messages."
224	"Clear the message window."
225	"Display"
226	"-------"
227	"When a message arrives for your process, a file descriptor becomes"
228	"active. Depending on the particular structure of your tool, you can\
"
229	"either arrange for the window system toolkit to call a callback whe\
n"
230	"the file descriptor becomes active, or include the file descriptor"
231	"in a select(3) call that blocks on a number of file descriptors."
232	"In either case, after the file descriptor becomes active, you call"
233	"tt_message_receive() to get a handle for a message containing the"
234	"incoming message. You can use the tt_message_*() calls to examine"
235	"the attributes of the message to determine the action you should ta\
ke."
236	"You can recognize replies to messages you sent by comparing the"
237	"handles, by placing information meaningful to your application in"
238	"the message with the tt_message_user_set() call, or by placing"
239	"specific callbacks on messages and patterns with the"
240	"tt_message_callback_add and tt_pattern_callback_add calls."
241	"Messages"
242	"--------"
243	"To send a message, first allocate a new message, fill in the proper\
"
244	"information, then send it. Any reply will show up in the same messa\
ge."
245	"When finished with the message free the storage for the message. (f\
or"
246	"value returning requests, this is after any needed return values ar\
e"
247	"copied out; for notifications, this is right after its been sent)."
248	""
249	"To allocate the new message, use tt_message_create(). It returns a"
250	"\"handle\" or \"opaque pointer\" to the message; use this handle on\
"
251	"succeeding calls to reference this message."
252	""
253	"To fill in message information, use the tt_message_*_set() calls;"
254	"there is one of these for each attribute of the message."
255	""
256	"You should set Class, Address, Op and Args."
257	""
258	"Send the message with tt_message_send()."
259	"Patterns"
260	"--------"
261	"Since messages are not explicitly directed to a particular receiver\
 by"
262	"the sending process, a \"pattern matching\" method is used to deter\
mine"
263	"the receiver. Tools register descriptions of the messages in which \
they"
264	"are interested and ToolTalk uses these descriptions to infer the ro\
uting"
265	"of the message. Separate sets of patterns are kept to describe the"
266	"messages the tool wants to handle and the messages the tool wants t\
o"
267	"observe. Tools wishing to receive messages declare their interest e\
ither"
268	"dynamically at run time or statically at installation time. A dynam\
ic"
269	"registration consists of a set of patterns against which all messag\
es"
270	"are compared. If a message matches the pattern, the tool which"
271	"registered the pattern is eligible to receive the message."
272	""
273	"Before receiving procedural messages through ToolTalk, a process mu\
st"
274	"register with the message passer. By registering, the process gives\
"
275	"patterns to ToolTalk; which then delivers messages that match those\
"
276	"patterns to the process.  The patterns can be created in two ways:"
277	""
278	"Statically. Through a ptype. A process can declare its ptype to the\
"
279	"message passer; ToolTalk then generates patterns from each signatur\
e"
280	"in the ptype. These generated patterns can be modified by joining a\
nd"
281	"quitting sessions and files, just as in object oriented message pas\
sing."
282	""
283	"Dynamically. A process can create patterns \"on the fly\" and regis\
ter"
284	"them with ToolTalk. A typical use of this facility would be a messa\
ge"
285	"logging utility that simply observes all messages that go by, displ\
aying"
286	"them in a window; such a utility would be useful in debugging messa\
ge"
287	"protocols."
288	""
289	"This application uses only the Dynamic patterns."
290	""
291	"To register a pattern, first you must allocate a new pattern, fill \
in"
292	"the proper information, then register it. When you are through with\
 the"
293	"pattern (which is only after you no longer are interested in messag\
es"
294	"that match it), free the storage for the pattern."
295	""
296	"To allocate the new pattern, use tt_pattern_create(). It returns a"
297	"\"handle\" or \"opaque pointer\" to the pattern; use this handle on\
"
298	"succeeding calls to reference this pattern."
299	""
300	"To fill in pattern information, use the tt_pattern_*_add() calls; t\
here"
301	"is one of these for each attribute of the pattern. It is possible t\
o"
302	"supply multiple values for each attribute in the pattern; the patte\
rn"
303	"attribute matches a message attribute if any of the values in the"
304	"pattern match the value in the message."
305	""
306	"This application uses non-exclusive settings and comma separated va\
lues"
307	"for the multiple attributes."
308	""
309	"To match messages without regard to the value in a particular attri\
bute,"
310	"simply omit the attribute from the pattern."
311	""
312	"If no pattern attribute is specified, the ToolTalk service counts t\
he"
313	"message attribute as matched. The fewer pattern attributes you spec\
ify,"
314	"the more messages you become eligible to receive."
315	""
316	"If there are multiple values specified for a pattern attribute, one\
 of"
317	"the values must match the message attribute value. If no value matc\
hes,"
318	"the ToolTalk service will not consider your application as a receiv\
er."
319	""
320	"The attributes Category and Scope must always be supplied."
321	""
322	"When the pattern is complete, register the pattern with"
323	"tt_pattern_register(), and then join sessions or files as required \
by"
324	"SELECT'ing the Apply button."
325	""
326	"Since the primary effect of joining files and sessions is to update\
"
327	"currently registered patterns, patterns that are registered after t\
he"
328	"joins will not be updated. You should either register all patterns"
329	"before joining, or re-do the joins after registering new patterns. \
This"
330	"applies to patterns registered implicitly by joining ptypes, as wel\
l."
331	"As you add messages via the Messages window, the menu attached to t\
his"
332	"button grows. You may send a message by selecting it."
333	"Address:"
334	"Because there are many types of tools and different users will use"
335	"different sets of tools at different times, it's often impossible f\
or"
336	"the sender of a message to identify the precise recipient of a mess\
age."
337	"Instead, the sender gives an operation name which specifies the mea\
ning"
338	"of the message and may give an object to which the message is direc\
ted,"
339	"or the object type (otype)."
340	""
341	"Use object or otype for object oriented messages. If the address is\
"
342	"handler, specify the handler."
343	""
344	"        TT_PROCEDURE: Op and Args supplied."
345	""
346	"        TT_OBJECT:    Object, Op, and Args supplied."
347	""
348	"        TT_HANDLER:   Handler, Op, and Args supplied."
349	""
350	"        TT_OTYPE:     Otype, Op, and Args supplied."
351	"Handler:  "
352	"If you know the exact procid of the handler, you can address messag\
es"
353	"to it directly. The usual way this would happen would be for one pr\
ocess"
354	"to make a general request and then pick the Handler attribute out o\
f the"
355	"reply, directing further messages to the same handler; this allows \
two"
356	"processes to rendezvous through broadcast message passing and then \
go"
357	"into a dialogue."
358	"Handler_Ptype:"
359	"If you know the ptype of the process that will handle the message, \
fill"
360	"it in. You would know the ptype if you consulted the ptype definiti\
on"
361	"to find the details of the protocol."
362	"Object:"
363	"Fill in the objid of the object to which the message is to be sent.\
"
364	"OType:"
365	"Type of the object."
366	"Op:"
367	"Fill in the operation that describes the notification or request"
368	"being made. Consult the otype definition for the target object to"
369	"determine the operation name."
370	"Opnum:"
371	"Operation number. This is used to distinguish between overloaded"
372	"operations (operations with the same name but different argument"
373	"types), and to simplify a client's \"internal\" dispatch (invoking"
374	"the proper internal procedure for a given message.)"
375	"Scope:"
376	"Use scope session if messages from other processes in the same sess\
ion"
377	"as your process are desired; use scope file if messages about a fil\
e is"
378	"desired."
379	""
380	"      TT_SESSION:          Receive messages from other processes in\
"
381	"                           your session."
382	""
383	"      TT_FILE:             Receive messages about the file joined."
384	""
385	"      TT_BOTH:             Receive messages about a file and the"
386	"                           session."
387	""
388	"      TT_FILE_IN_SESSION:  Receive messages for the file joined whi\
le"
389	"                           in this session."
390	"Session:"
391	"For scope session, specify the session (tt_default_session() by"
392	"default)."
393	"File:"
394	"For file scope, specify the name of the file."
395	"Category:"
396	"Use category observe if you just want to look at the messages; use"
397	"category handle to volunteer to be the unique handler of the messag\
e."
398	""
399	"    TT_OBSERVE: Observing processes just get copies of the message \
for"
400	"                their own information. Any number of processes can"
401	"                observe a message. Even if the message is a request\
,"
402	"                observers cannot return values to the sender. Often\
,"
403	"                the action taken by observers just affects the"
404	"                interactive display of the underlying tool data."
405	""
406	"    TT_HANDLE:  Handling processes actually perform an action based\
 on"
407	"                the message.  Only one process will handle any give\
n"
408	"                message. If the message is a request, the handling"
409	"                process is the process that returns any values."
410	"                Generally, the action taken by a handler affects th\
e"
411	"                persistently-stored representation of the tool's da\
ta."
412	"Class:"
413	"Use class request for messages that return values, or for which you\
"
414	"want feedback telling you when the message is handled or queued, or\
"
415	"when a process is started to handle the request. Use class notifica\
tion"
416	"for messages that just notify other processes of events."
417	""
418	"    TT_NOTICE:   Notice are messages that inform other tools that s\
ome"
419	"                 event has happened. Tools sending notices don't ex\
pect"
420	"                 replies back; the sender is just letting the other\
"
421	"                 tools know what's happening."
422	""
423	"     TT_REQUEST: Requests are messages that ask another tool to"
424	"                 perform an action. Often, but not always, the"
425	"                 requesting tool expects some value returned. This"
426	"                 value is returned as a reply to the request. Even"
427	"                 if no value is returned, the tool processing the"
428	"                 request sends a reply indicating success or failur\
e."
429	"Disposition:"
430	"Specifies the action to take if the message cannot be handled by an\
y"
431	"running process. Queue if the message should be queued until a proc\
ess"
432	"of the Handler_Ptype registers. Start if a process of the Handler_P\
type"
433	"should be started."
434	""
435	"    TT_QUEUE: Queue the message until a process of the proper ptype\
"
436	"              receives the message."
437	""
438	"    TT_START: Attempt to start a process of the proper ptype if non\
e"
439	"              is running."
440	""
441	"Note that Tt_disposition values can be added together, so that"
442	"TT_QUEUE+TT_START means both to queue the message and to try to sta\
rt"
443	"a process. This can be useful if the start can fail (or be vetoed b\
y"
444	"the user), to ensure the message is processed as soon as an eligibl\
e"
445	"process does start."
446	"State:"
447	"Values for the State attribute of a message. Possible values and th\
eir"
448	"meanings are:"
449	""
450	"    TT_CREATED:  Message has been created but not yet sent. Only th\
e"
451	"                 sender of a message will see a message in this sta\
te."
452	""
453	"    TT_SENT:     Message has been sent but not yet handled."
454	""
455	"    TT_HANDLED:  Message has been handled, return values are valid.\
"
456	""
457	"    TT_FAILED:   Message could not be delivered to a handler."
458	""
459	"    TT_QUEUED:   Message has been queued for later delivery."
460	""
461	"    TT_STARTED:  Attempting to start a process to handle the messag\
e."
462	""
463	"    TT_REJECTED: Message has been rejected by a possible handler."
464	"                 This state is seen only by the rejecting process;"
465	"                 ToolTalk changes the state back to TT_SENT before"
466	"                 delivering the message to another possible handler\
."
467	"Status:"
468	"A unique number indicating the results of the message returned by t\
he"
469	"replier. This number must be larger than TT_ERR_LAST (2047)."
470	"Status string:"
471	"A text description of the Status of the message."
472	"Sender:"
473	"Identifies the process that sent the message."
474	"Sender ptype:"
475	"The ptype of the process that sent the message."
476	"Uid:"
477	"The effective user identifier of the process that sent the message.\
"
478	"Gid:"
479	"The effective group identifier of the process that sent the message\
."
480	"Mode:"
481	"Specifies the mode of a message argument. Possible values and"
482	"meanings are:"
483	""
484	"    TT_IN:    The argument is written by the sender and read by the\
"
485	"              handler and any observers."
486	""
487	"    TT_OUT:   The argument is written by the handler and read by th\
e"
488	"              sender and any reply observers."
489	""
490	"    TT_INOUT: The argument is written and read by the sender and th\
e"
491	"              handler and any observers."
492	"Type:"
493	"String indicating the type. Common values are \"string\" and \"int\"\
"
494	"although developers can create there own."
495	"Value:"
496	"Actual data of the value dependant on the type."
497	"When the apply button is pressed, the values of the specified"
498	"settings will be displayed for each message received."
499	"When pressed, the display values are reset to the state of the last\
"
500	"apply or the initial value, whichever is last."
501	"Edit Receive Contexts:"
502	"Add, delete, or change contexts you want to receive."
503	"Pattern matcher"
504	"---------------"
505	"Since messages are not explicitly directed to a particular receiver\
"
506	"by the sending process, a \"pattern matching\" method is used to"
507	"determine the receiver. Tools register descriptions of the messages\
"
508	"in which they are interested and ToolTalk uses these descriptions t\
o"
509	"infer the routing of the message. Separate sets of patterns are kep\
t"
510	"to describe the messages the tool wants to handle and the messages"
511	"the tool wants to observe. Tools wishing to receive messages declar\
e"
512	"their interest either dynamically at run time or statically at"
513	"installation time. A dynamic registration consists of a set of"
514	"patterns against which all messages are compared. If a message matc\
hes"
515	"the pattern, the tool which registered the pattern is eligible to"
516	"receive the message."
517	""
518	"Before receiving procedural messages through ToolTalk, a process mu\
st"
519	"register with the message passer. By registering, the process gives\
"
520	"patterns to ToolTalk; which then delivers messages that match those\
"
521	"patterns to the process. The patterns can be created in two ways:"
522	""
523	"Statically. Through a ptype. A process can declare its ptype to the\
"
524	"message passer; ToolTalk then generates patterns from each signatur\
e"
525	"in the ptype. These generated patterns can be modified by joining a\
nd"
526	"quitting sessions and files, just as in object oriented message pas\
sing."
527	""
528	"Dynamically. A process can create patterns \"on the fly\" and regis\
ter"
529	"them with ToolTalk. A typical use of this facility would be a messa\
ge"
530	"logging utility that simply observes all messages that go by, displ\
aying"
531	"them in a window; such a utility would be useful in debugging messa\
ge"
532	"protocols."
533	""
534	"This application uses only the Dynamic patterns."
535	""
536	"To register a pattern, first you must allocate a new pattern, fill \
in"
537	"the proper information, then register it. When you are through with\
 the"
538	"pattern (which is only after you no longer are interested in messag\
es"
539	"that match it), free the storage for the pattern."
540	""
541	"To allocate the new pattern, use tt_pattern_create(). It returns a"
542	"\"handle\" or \"opaque pointer\" to the pattern; use this handle on\
"
543	"succeeding calls to reference this pattern."
544	""
545	"To fill in pattern information, use the tt_pattern_*_add() calls;"
546	"there is one of these for each attribute of the pattern. It is poss\
ible"
547	"to supply multiple values for each attribute in the pattern; the"
548	"pattern attribute matches a message attribute if any of the values"
549	"in the pattern match the value in the message."
550	""
551	"This application uses non-exclusive settings and comma separated va\
lues"
552	"for the multiple attributes."
553	""
554	"To match messages without regard to the value in a particular attri\
bute,"
555	"simply omit the attribute from the pattern."
556	""
557	"If no pattern attribute is specified, the ToolTalk service counts t\
he"
558	"message attribute as matched. The fewer pattern attributes you spec\
ify,"
559	"the more messages you become eligible to receive."
560	""
561	"If there are multiple values specified for a pattern attribute, one\
 of"
562	"the values must match the message attribute value. If no value matc\
hes,"
563	"the ToolTalk service will not consider your application as a receiv\
er."
564	""
565	"The attributes Category and Scope must always be supplied."
566	""
567	"When the pattern is complete, register the pattern with"
568	"tt_pattern_register(), and then join sessions or files as required \
by"
569	"SELECT'ing the Apply button."
570	""
571	"Since the primary effect of joining files and sessions is to update\
"
572	"currently registered patterns, patterns that are registered after t\
he"
573	"joins will not be updated. You should either register all patterns"
574	"before joining, or re-do the joins after registering new patterns."
575	"This applies to patterns registered implicitly by joining ptypes, a\
s"
576	"well."
577	"Address:"
578	"Because there are many types of tools and different users will use"
579	"different sets of tools at different times, it's often impossible f\
or"
580	"the sender of a message to identify the precise recipient of a mess\
age."
581	"Instead, the sender gives an operation name which specifies the mea\
ning"
582	"of the message and may give an object to which the message is direc\
ted,"
583	"or the object type (otype)."
584	""
585	"Use object or otype for object oriented messages. If the address is\
"
586	"handler, specify the handler."
587	""
588	"    TT_PROCEDURE: Op and Args supplied."
589	""
590	"    TT_OBJECT:    Object, Op, and Args supplied."
591	""
592	"    TT_HANDLER:   Handler, Op, and Args supplied."
593	""
594	"    TT_OTYPE:     Otype, Op, and Args supplied."
595	"Object:"
596	"Fill in the objid of the object to which the message is to be sent.\
"
597	"OType:"
598	"Type of the object."
599	"Op:"
600	"Fill in the operation that describes the notification or request"
601	"being made. Consult the otype definition for the target object to"
602	"determine the operation name."
603	"Scope:"
604	"Use scope session if messages from other processes in the same sess\
ion"
605	"as your process are desired; use scope file if messages about a fil\
e"
606	"is desired."
607	""
608	"    TT_SESSION:          Receive messages from other processes in"
609	"                         your session."
610	""
611	"    TT_FILE:             Receive messages about the file joined."
612	""
613	"    TT_BOTH:             Receive messages about a file and the sess\
ion."
614	""
615	"    TT_FILE_IN_SESSION:  Receive messages for the file joined while\
 in"
616	"                         this session."
617	"File:"
618	"For file scope, specify the name of the file."
619	"Session:"
620	"For scope session, specify the session (tt_default_session() by"
621	"default)."
622	"Category:"
623	"Use category observe if you just want to look at the messages; use"
624	"category handle to volunteer to be the unique handler of the messag\
e."
625	""
626	"    TT_OBSERVE: Observing processes just get copies of the message \
for"
627	"                their own information. Any number of processes can"
628	"                observe a message. Even if the message is a request\
,"
629	"                observers cannot return values to the sender. Often\
,"
630	"                the action taken by observers just affects the"
631	"                interactive display of the underlying tool data."
632	""
633	"    TT_HANDLE:  Handling processes actually perform an action based\
 on"
634	"                the message. Only one process will handle any given\
"
635	"                message. If the message is a request, the handling"
636	"                process is the process that returns any values."
637	"                Generally, the action taken by a handler affects th\
e"
638	"                persistently-stored representation of the tool's da\
ta."
639	"Class:"
640	"Use class request for messages that return values, or for which you\
"
641	"want feedback telling you when the message is handled or queued, or\
"
642	"when a process is started to handle the request. Use class notifica\
tion"
643	"for messages that just notify other processes of events."
644	""
645	"    TT_NOTICE:  Notice are messages that inform other tools that so\
me"
646	"                event has happened. Tools sending notices don't exp\
ect"
647	"                replies back; the sender is just letting the other \
tools"
648	"                know what's happening."
649	""
650	"    TT_REQUEST: Requests are messages that ask another tool to perf\
orm"
651	"                an action.  Often, but not always, the requesting t\
ool"
652	"                expects some value returned. This value is returned\
 as"
653	"                a reply to the request. Even if no value is returne\
d,"
654	"                the tool processing the request sends a reply indic\
ating"
655	"                success or failure."
656	"State:"
657	"Values for the State attribute of a message. Possible values and"
658	"their meanings are:"
659	""
660	"    TT_CREATED:  Message has been created but not yet sent. Only th\
e"
661	"                 sender of a message will see a message in this sta\
te."
662	""
663	"    TT_SENT:     Message has been sent but not yet handled."
664	""
665	"    TT_HANDLED:  Message has been handled, return values are valid.\
"
666	""
667	"    TT_FAILED:   Message could not be delivered to a handler."
668	""
669	"    TT_QUEUED:   Message has been queued for later delivery."
670	""
671	"    TT_STARTED:  Attempting to start a process to handle the messag\
e."
672	""
673	"    TT_REJECTED: Message has been rejected by a possible handler."
674	"                 This state is seen only by the rejecting process;"
675	"                 ToolTalk changes the state back to TT_SENT before"
676	"                 delivering the message to another possible handler\
."
677	"Disposition:"
678	"Specifies the action to take if the message cannot be handled by an\
y"
679	"running process. Queue if the message should be queued until a proc\
ess"
680	"of the Handler_Ptype registers. Start if a process of the Handler_P\
type"
681	"should be started."
682	""
683	"    TT_QUEUE: Queue the message until a process of the proper ptype\
"
684	"              receives the message."
685	""
686	"    TT_START: Attempt to start a process of the proper ptype if non\
e"
687	"              is running."
688	""
689	"Note that Tt_disposition values can be added together, so that"
690	"TT_QUEUE+TT_START means both to queue the message and to try to sta\
rt"
691	"a process. This can be useful if the start can fail (or be vetoed b\
y"
692	"the user), to ensure the message is processed as soon as an eligibl\
e"
693	"process does start."
694	"Sender:"
695	"Identifies the process that sent the message."
696	"Sender ptype:"
697	"The ptype of the process that sent the message."
698	"Args:"
699	"Fill in any arguments specific to the operation. Use tt_message_arg\
_add"
700	"to add each argument in turn. For each argument, you must specify: \
mode"
701	"(in, out, or inout), type, and if the mode is in or inout, the valu\
e."
702	"Mode:"
703	"Specifies the mode of a message argument.  Possible values and"
704	"meanings are:"
705	""
706	"    TT_IN:    The argument is written by the sender and read by the\
"
707	"              handler and any observers."
708	""
709	"    TT_OUT:   The argument is written by the handler and read by th\
e"
710	"              sender and any reply observers."
711	""
712	"    TT_INOUT: The argument is written and read by the sender and th\
e"
713	"              handler and any observers."
714	"Type:"
715	"String indicating the type. Common values are \"string\" and \"int\"\
"
716	"although developers can create there own."
717	"Value:"
718	"Actual data of the value dependant on the type."
719	"Add Button:"
720	"Add an argument with the Mode, Type and Value."
721	"Delete Button:"
722	"Delete the selected argument."
723	"Change Button:"
724	"Change the selected argument to the current values of Mode, Type"
725	"and Value."
726	"Apply button:"
727	"Register to receive messages that match this pattern."
728	"Name:"
729	"Name of this context"
730	"Value:"
731	"A value appropriate to the type,"
732	""
733	"i.e. 55 for a type of int, \"fred\" for a type of char, or ^V^L^X f\
or"
734	"a type of bytes."
735	"Add Context:"
736	"Add the current values of the Name and Value fields to the list of"
737	"contexts."
738	"Delete Context:"
739	"Delete the selected entry from the list of contexts."
740	"Change Context:"
741	"Change the selected entry to what's in the Name and Value fields."
742	"Edit Send Contexts:"
743	"Popup window to edit the list of contexts you wish to send."
744	"Message"
745	"-------"
746	"To send a message, first allocate a new message, fill in the proper\
"
747	"information, then send it. Any reply will show up in the same messa\
ge."
748	"When finished with the message free the storage for the message. (f\
or"
749	"value returning requests, this is after any needed return values ar\
e"
750	"copied out; for notifications, this is right after its been sent),"
751	""
752	"To allocate the new message, use tt_message_create(). It returns a"
753	"\"handle\" or \"opaque pointer\" to the message; use this handle"
754	"on succeeding calls to reference this message."
755	""
756	"To fill in message information, use the tt_message_*_set() calls;"
757	"there is one of these for each attribute of the message."
758	""
759	"You should set Class, Address, Op and Args."
760	""
761	"Send the message with tt_message_send()."
762	"Address:"
763	"Because there are many types of tools and different users will use"
764	"different sets of tools at different times, it's often impossible f\
or"
765	"the sender of a message to identify the precise recipient of a mess\
age."
766	"Instead, the sender gives an operation name which specifies the mea\
ning"
767	"of the message and may give an object to which the message is direc\
ted,"
768	"or the object type (otype)."
769	""
770	"Use object or otype for object oriented messages. If the address is\
"
771	"handler, specify the handler."
772	""
773	"    TT_PROCEDURE: Op and Args supplied."
774	""
775	"    TT_OBJECT:    Object, Op, and Args supplied."
776	""
777	"    TT_HANDLER:   Handler, Op, and Args supplied."
778	""
779	"    TT_OTYPE:     Otype, Op, and Args supplied."
780	"Handler:"
781	"If you know the exact procid of the handler, you can address messag\
es"
782	"to it directly. The usual way this would happen would be for one"
783	"process to make a general request and then pick the Handler attribu\
te"
784	"out of the reply, directing further messages to the same handler; t\
his"
785	"allows two processes to rendezvous through broadcast message passin\
g"
786	"and then go into a dialogue."
787	"Handler_Ptype:"
788	"If you know the ptype of the process that will handle the message,"
789	"fill it in. You would know the ptype if you consulted the ptype"
790	"definition to find the details of the protocol."
791	"Object:"
792	"Fill in the objid of the object to which the message is to be sent.\
"
793	"OType:"
794	"Type of the object."
795	"Op:"
796	"Fill in the operation that describes the notification or request be\
ing"
797	"made. Consult the otype definition for the target object to determi\
ne"
798	"the operation name."
799	"Scope:"
800	"Use scope session if messages from other processes in the same sess\
ion"
801	"as your process are desired; use scope file if messages about a fil\
e is"
802	"desired."
803	""
804	"    TT_SESSION:          Receive messages from other processes in y\
our"
805	"                         session."
806	""
807	"    TT_FILE:             Receive messages about the file joined."
808	""
809	"    TT_BOTH:             Receive messages about a file and the sess\
ion."
810	""
811	"    TT_FILE_IN_SESSION:  Receive messages for the file joined while\
 in"
812	"                         this session."
813	"Session:"
814	"For scope session, specify the session (tt_default_session() by"
815	"default)."
816	"File:"
817	"For file scope, specify the name of the file."
818	"Class:"
819	"Use class request for messages that return values, or for which you\
"
820	"want feedback telling you when the message is handled or queued, or\
"
821	"when a process is started to handle the request. Use class"
822	"notification for messages that just notify other processes of event\
s."
823	""
824	"    TT_NOTICE:  Notice are messages that inform other tools that so\
me"
825	"                event has happened. Tools sending notices don't exp\
ect"
826	"                replies back; the sender is just letting the other"
827	"                tools know what's happening."
828	""
829	"    TT_REQUEST: Requests are messages that ask another tool to perf\
orm"
830	"                an action.  Often, but not always, the requesting t\
ool"
831	"                expects some value returned. This value is returned\
 as"
832	"                a reply to the request. Even if no value is returne\
d,"
833	"                the tool processing the request sends a reply"
834	"                indicating success or failure."
835	"Disposition:"
836	"Specifies the action to take if the message cannot be handled by an\
y"
837	"running process. Queue if the message should be queued until a proc\
ess"
838	"of the Handler_Ptype registers. Start if a process of the Handler_P\
type"
839	"should be started."
840	""
841	"    TT_QUEUE: Queue the message until a process of the proper ptype\
"
842	"              receives the message."
843	""
844	"    TT_START: Attempt to start a process of the proper ptype if non\
e"
845	"              is running."
846	""
847	"Note that Tt_disposition values can be added together, so that"
848	"TT_QUEUE+TT_START means both to queue the message and to try to sta\
rt"
849	"a process. This can be useful if the start can fail (or be vetoed b\
y"
850	"the user), to ensure the message is processed as soon as an eligibl\
e"
851	"process does start."
852	"Sender_Ptype:"
853	"The ptype of the process that sent the message."
854	"Status:"
855	"A unique number indicating the results of the message returned by"
856	"the replier. This number must be larger than TT_ERR_LAST (2047)."
857	"Status string:"
858	"A text description of the Status of the message."
859	"Args:"
860	"Fill in any arguments specific to the operation. Use"
861	"tt_message_arg_add to add each argument in turn. For each argument,\
"
862	"you must specify: mode (in, out, or inout), type, and if the mode i\
s"
863	"in or inout, the value."
864	"Mode:"
865	"Specifies the mode of a message argument. Possible values and"
866	"meanings are:"
867	""
868	"    TT_IN:    The argument is written by the sender and read by the\
"
869	"              handler and any observers."
870	""
871	"    TT_OUT:   The argument is written by the handler and read by th\
e"
872	"              sender and any reply observers."
873	""
874	"    TT_INOUT: The argument is written and read by the sender and th\
e"
875	"              handler and any observers."
876	"Type:"
877	"String indicating the type. Common values are \"string\" and \"int\"\
"
878	"although developers can create there own."
879	"Value:"
880	"Actual data of the value dependant on the type."
881	"Add Button:"
882	"Add an argument with the Mode, Type and Value."
883	"Delete Button:"
884	"Delete the selected argument."
885	"Change Button:"
886	"Change the selected argument to the current values of Mode, Type"
887	"and Value."
888	"Send button:"
889	"Send this message. Also print out to standard output the Tooltalk"
890	"calls that will be made to create and send this message."
891	"Name:"
892	"Name of this context."
893	"Value:"
894	"Value of this context, interpreted as Type."
895	"Add:"
896	"Add the values in the text fields as a new context."
897	"Delete:"
898	"Delete the currently selected context."
899	"Change:"
900	"Change the selected context to reflect what is currently in the"
901	"text fields."
902	"Unable to open pipe for sieve output.\n"
903	"Tts_string_list:generate called for message - ignoring \n"
$set 11
2	"TT_OK\tRequest successful."
3	"TT_WRN_NOTFOUND\tThe object was not removed because it was not found.\
"
4	"TT_WRN_STALE_OBJID\tThe object attribute in the message has been repl\
aced with a newer one. Update the place from which the object id was obt\
ained."
5	"TT_WRN_STOPPED\tThe query was halted by the filter procedure."
6	"TT_WRN_SAME_OBJID\tThe moved object retains the same objid."
7	"TT_WRN_START_MESSAGE\tThis message caused this process to be started.\
  This message should be replied to even if it is a notice."
8	"TT_WRN_APPFIRST\tThis code should be unused."
9	"TT_WRN_LAST\tThis code should be unused."
10	"TT_ERR_CLASS\tThe Tt_class value passed is invalid."
11	"TT_ERR_DBAVAIL\tA required database is not available. The condition \
may be temporary, trying again later may work."
12	"TT_ERR_DBEXIST\tA required database does not exist. The database mus\
t be created before this action will work."
13	"TT_ERR_FILE\tFile object could not be found."
14	"TT_ERR_MODE\tThe Tt_mode value is not valid."
15	"TT_ERR_ACCESS\tAn attempt was made to access a ToolTalk object in a \
way forbidden by the protection system."
16	"TT_ERR_NOMP\tNo ttsession process is running, probably because tt_op\
en() has not been called yet. If this code is returned from tt_open() it\
 means ttsession could not be started, which generally means ToolTalk is\
 not installed on this system."
17	"TT_ERR_NOTHANDLER\tOnly the handler of the message can do this."
18	"TT_ERR_NUM\tThe integer value passed is not valid."
19	"TT_ERR_OBJID\tThe object id passed does not refer to any existing ob\
ject spec."
20	"TT_ERR_OP\tThe operation name passed is not syntactically valid."
21	"TT_ERR_OTYPE\tThe object type passed is not the name of an installed\
 object type."
22	"TT_ERR_ADDRESS\tThe Tt_address value passed is not valid."
23	"TT_ERR_PATH\tOne of the directories in the file path passed does not\
 exist or cannot be read."
24	"TT_ERR_POINTER\tThe opaque pointer (handle) passed does not indicate\
 an object of the proper type."
25	"TT_ERR_PROCID\tThe process id passed is not valid."
26	"TT_ERR_PROPLEN\tThe property value passed is too long."
27	"TT_ERR_PROPNAME\tThe property name passed is syntactically invalid."
28	"TT_ERR_PTYPE\tThe process type passed is not the name of an installe\
d process type."
29	"TT_ERR_DISPOSITION\tThe Tt_disposition value passed is not valid."
30	"TT_ERR_SCOPE\tThe Tt_scope value passed is not valid."
31	"TT_ERR_SESSION\tThe session id passed is not the name of an active s\
ession."
32	"TT_ERR_VTYPE\tThe value type name passed is not valid."
33	"TT_ERR_NO_VALUE\tNo property value with the given name and number ex\
ists."
34	"TT_ERR_INTERNAL\tInternal error (bug)"
35	"TT_ERR_READONLY\tThe attribute cannot be changed."
36	"TT_ERR_NO_MATCH\tNo handler could be found for this message, and the\
 disposition was not queue or start."
37	"TT_ERR_UNIMP\tFunction not implemented."
38	"TT_ERR_OVERFLOW\tToo many active messages (try again later)."
39	"TT_ERR_PTYPE_START\tAttempt to launch instance of ptype failed."
40	"TT_ERR_CATEGORY\tPattern object has no category set."
41	"TT_ERR_DBUPDATE\tThe database is inconsistent: another tt_spec_write\
 updated object first."
42	"TT_ERR_DBFULL\tTooltalk database is full."
43	"TT_ERR_DBCONSIST\tDatabase is corrupt or access information is incom\
plete (run ttdbck)."
44	"TT_ERR_STATE\tThe Tt_message is in a Tt_state that is invalid for th\
e attempted operation."
45	"TT_ERR_NOMEM\tNo more memory."
46	"TT_ERR_SLOTNAME\tThe slot name is syntactically invalid."
47	"TT_ERR_XDR\tThe XDR proc. passed evaluated to zero len, or failed du\
ring invocation."
48	"TT_ERR_NETFILE\tThe netfilename passed is not syntactically valid."
49	"TT_DESKTOP_UNMODIFIED\tOperation does not apply to unmodified entiti\
es"
50	"TT_MEDIA_ERR_SIZE\tThe specified size was too big or too small"
51	"TT_MEDIA_ERR_FORMAT\tThe data do not conform to their alleged format\
"
52	"TT_ERR_APPFIRST\tThis code should be unused."
53	"TT_ERR_LAST\tThis code should be unused."
54	"TT_STATUS_LAST\tThis code should be unused."
55	"Unknown Tt_status value."
56	"TT_DESKTOP_EPERM\tNot super-user"
57	"TT_DESKTOP_ENOENT\tNo such file or directory"
58	"TT_DESKTOP_EINTR\tInterrupted system call"
59	"TT_DESKTOP_EIO\tI/O error"
60	"TT_DESKTOP_EAGAIN\tNo more processes"
61	"TT_DESKTOP_ENOMEM\tNot enough space"
62	"TT_DESKTOP_EACCES\tPermission denied"
63	"TT_DESKTOP_EFAULT\tBad address"
64	"TT_DESKTOP_EEXIST\tFile exists"
65	"TT_DESKTOP_ENODEV\tNo such device"
66	"TT_DESKTOP_ENOTDIR\tNot a directory"
67	"TT_DESKTOP_EISDIR\tIs a directory"
68	"TT_DESKTOP_EINVAL\tInvalid argument"
69	"TT_DESKTOP_ENFILE\tFile table overflow"
70	"TT_DESKTOP_EMFILE\tToo many open files"
71	"TT_DESKTOP_ETXTBSY\tText file busy"
72	"TT_DESKTOP_EFBIG\tFile too large"
73	"TT_DESKTOP_ENOSPC\tNo space left on device"
74	"TT_DESKTOP_EROFS\tRead-only file system"
75	"TT_DESKTOP_EMLINK\tToo many links"
76	"TT_DESKTOP_EPIPE\tBroken pipe"
77	"TT_DESKTOP_ENOMSG\tNo message of desired type"
78	"TT_DESKTOP_EDEADLK\tDeadlock condition"
79	"TT_DESKTOP_ENODATA\tNo data available"
80	"TT_DESKTOP_EPROTO\tProtocol error"
81	"TT_DESKTOP_ENOTEMPTY\tDirectory not empty"
82	"TT_DESKTOP_ETIMEDOUT\tConnection timed out"
83	"TT_DESKTOP_EALREADY\tOperation already in progress"
84	"TT_DESKTOP_ECANCELED\tOperation canceled"
85	"TT_DESKTOP_ENOTSUP\tNot supported"
86	"TT_ERR_TOOLATE\tThis must be the first call made into the ToolTalk API and can therefore no longer be performed."
87	"TT_ERR_AUTHORIZATION\tThe user is not authorized to connect to this ToolTalk session."
88	"TT_AUTHFILE_ACCESS\tThe user is not authorized to access the ToolTalk authority file."
89	"TT_AUTHFILE_LOCK\tCannot lock the ToolTalk authority file."
90	"TT_AUTHFILE_LOCK_TIMEOUT\tCannot lock the ToolTalk authority file."
91	"TT_AUTHFILE_UNLOCK\tCannot unlock the ToolTalk authority file."
92	"TT_AUTHFILE_MISSING\tThe ToolTalk authority file is missing."
93	"TT_AUTHFILE_ENTRY_MISSING\tThe ToolTalk authority file is missing an entry for this session."
94	"TT_AUTHFILE_WRITE\tCannot write the ToolTalk authority file."
95	"TT_ERR_VERSION_MISMATCH\tThis client is using a different RPC or program version from the server of this ToolTalk session."
