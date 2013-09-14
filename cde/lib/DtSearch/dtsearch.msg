$ $XConsortium: dtsearch.msg /main/15 1996/10/30 17:29:27 cde-ibm $
$ 
$ (c) Copyright 1996 Digital Equipment Corporation.
$ (c) Copyright 1996 Hewlett-Packard Company.
$ (c) Copyright 1996 International Business Machines Corp.
$ (c) Copyright 1996 Sun Microsystems, Inc.
$ (c) Copyright 1996 Novell, Inc. 
$ (c) Copyright 1996 FUJITSU LIMITED.
$ (c) Copyright 1996 Hitachi.

$   COMPONENT_NAME: austext
$ 
$   FUNCTIONS: database
$ 
$   ORIGINS: 27
$ 
$   IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$   combined with the aggregated modules for this product)
$   OBJECT CODE ONLY SOURCE MATERIALS
$ 
$   (C) COPYRIGHT International Business Machines Corp. 1994,1995
$   All Rights Reserved
$   US Government Users Restricted Rights - Use, duplication or
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.

$ ********************** DTSEARCH.MSG ************************
$ $Id: dtsearch.msg /main/15 1996/10/30 17:29:27 cde-ibm $
$ December 1994.
$ Messages specific to DtSearch.
$ Symbolic msg sets replaced with numbers for portability.
$ Basically there is a msgset for each module.

$ $Log$
$ Revision 2.15  1996/04/10  22:54:07  miker
$ Revision 2.14  1996/04/10  19:47:18  miker
$ Revision 2.13  1996/03/20  19:32:13  miker
$ Revision 2.12  1996/03/13  22:53:07  miker
$ Revision 2.11  1996/03/05  16:04:30  miker
$ Revision 2.10  1996/02/01  17:16:48  miker
$ Revision 2.9  1995/12/27  21:17:19  miker
$ Revision 2.8  1995/12/27  16:04:46  miker
$ Revision 2.7  1995/12/07  23:26:31  miker
$ Revision 2.6  1995/12/01  16:10:25  miker
$ Revision 2.5  1995/10/26  16:21:21  miker
$ Revision 2.4  1995/10/20  21:30:42  miker
$ Revision 2.3  1995/10/19  20:52:30  miker
$ Revision 2.2  1995/10/03  21:37:17  miker
$ Revision 2.1  1995/09/22  17:54:34  miker

$quote "

$ General comments about all messages in this file:
$ - Any message that begins with a %1$s or %s is using that variable
$   to represent the name of the executable which is causing the
$   message to be output.
$ - Any variables specified in Usage messages should be translated
$   consistently.

$ ****************************************************
$ set 1 formerly set MS_misc, used many places
$set 1

1 "<null>"
$ message 2 can be translated to match local conventions.  It is used
$ in output files for information.
2 "%Y/%m/%d,%H:%M:%S"
3 "*** %1$sOut of Memory at %2$s asking for %3$lu bytes! ***\n"
4 "%1$s Version %2$s.\n"
$ message 6 refers to any value which is null when printing information.
6 "<unknown>"
8 "%1$s No valid databases remain."
9 "%1$s Invalid command line argument '%2$s'.\n"
10 "%1$s Ignored unknown command line argument '%2$s'.\n"
11 "%1$s Invalid database name '%2$s'.\n"
$ In message 13, do not translate "dtsrcreate." It is an executable name.
13 "%1$s No DB record in '%2$s' database.\n\
  The usual cause is failure to initialize database (run dtsrcreate)."
14 "%1$s Program schema version '%2$s' incompatible with\n\
 database '%3$s' version '%4$s'.\n"
15 "%1$s Received abort signal %2$d.\n"
16 "<no environment variable>"
17 "<no server daemon>"
18 "%1$s Database name not specified.\n"
19 " ,\t\n"
21 "%1$s Exit code = %2$d.\n"
$ Message 22 can be translated.  It represents a human-readable date like
$ "Thursday, Jan 5, 1996, 6:30 pm".  The "%I" is 12 hour clock time, but
$ it could be changed to "%H" for 24 hour clock time.
22 "%A, %b %d %Y, %I:%M %p"
23 "%1$s: Version %2$s.  Run %3$s.\n"
$ Message 24 should not translate ".fzk".
24 "%1$s: current working directory = '%2$s', .fzk file = '%3$s'\n"
25 "%1$s Abort due to program error.\n"
27 "%1$s: %2$s: Invalid file format."
96 "%s Hitlist is empty."
301 "%1$s input text is %2$s."
$ Msg 362 is a generic system message.  The format is
$ "<program name> <filename>: <system error>" where <system error>
$ is a system errno which will already be translated.
362 "%1$s %2$s: %3$s"
$ Msg 400 is like "<Program name>: <file name>, line <line number>:..."
400 "%1$s: %2$s, line %3$ld: Invalid characters in word '%4$s'."
$ Msg 423 is like "<Program name> Word <word> in <file name> is a duplicate."
423 "%1$s Word '%2$s' in %3$s is a duplicate."
1596 "%1$s %2$s Cannot open audit file %3$s: %4$s"

$ ****************************************************
$ set 2 formerly set MS_ausapi (ausapi.*)
$set 2

1 "%1$s Invalid or unavailable database name '%2$s'."
$ Msg 7 should not translate "dbnames" or "dbcount."
7 "%s dbnames specified but not dbcount."
20 "%1$s Invalid search_type '%2$c'."
30 "%s Null query.  No search performed."
$ Msg 37 should not translate "DtSearchInit."  It is a function name.
37 "%s First API function call must be DtSearchInit().\n"
40 "%1$s Null cleartext.  No highlighting performed."
$ Msg 115 must have the date formatted in the way specified.
115 "%1$s '%2$s' is invalid or incomplete date string.\n\
The correct format is '[yy]yy [mm [dd]]'."
$ Msg 187 must not translate "LOGNAME."  It is an environment variable.
187 "%1$s Missing both userid and LOGNAME environment variable."
198 "%s 'Before' date is equal to or after 'After' date.\n\
  No records would be returned."
216 "\n%1$s %2$s Caught signal %3$d.\n"
294 "%1$s Program Error in DtSearchResultsToText."
437 "%s Query contains invalid navigator string."
492 "%1$s There is no current hitlist."
498 "%1$s Invalid args for DtSearchResultsToText."
621 "%1$s %2$s has already been initialized."
806 "%1$s Query insufficient or search options\n\
incompatible with database '%2$s' to commence search."
$ Msg 1072 "stem" is the root of a word.
1072 "%1$s Program Error: Stem count (%2$d) greater than maximum (%3$d)."
1342 "%1$s Search Engine Error %2$d for highlight request for\n\
database '%3$s', hit word count=%4$ld, search type='%5$c', text='%6$.30s'"


$ ****************************************************
$ set 5 formerly set MS_chandel, used in dtsrhan.c
$set 5

3 "Output file '%1$s' already exists.\n"
$ Msg 4 needs to use the same 2 characters from messages 4 and 5.
$ The "q" is not needed because it is assumed that any character
$ other than "a" or "o" means to quit the process.
4 "Append, overwrite, or quit? [a,o,q] "
$ Msg 5 is the shortcut key for "a" or Append from message 4.
5 "a"
$ Msg 6 is the shortcut key for "o" or overwrite from message 4.
6 "o"
7 "Unable to open output file '%1$s'.\n"
11 "\nError - unable to open profile file '%1$s'.\n"
12 "Error line %1$d: invalid identifier '%2$s'.\n"
13 "Error line %1$d - identifier '%2$s' missing value(s).\n"
14 "Error line %1$d - zero or bad value for '%2$s'.\noffensive token: %3$s.\n"
$ Msg 15 should not translate "ANY."
15 "Error line %1$d - for identifier '%2$s', column has been set to ANY\n\
  but there is no identifying signature string.\n"
16 "Error line %1$d - zero or bad value for identifier '%2$s'\n\
  offensive token: %3$s.\n"
18 "Error line %1$d - missing value for identifier '%2$s'\n"
23 "Warning line %1$d: Delimiter redefined.\n"
25 "Error line %1$d: delimiter not specified as 'top' or 'bottom'.\n"
26 "Error line %1$d: invalid token '%1$s'.\n"
27 "Error line %1$d: invalid token '%2$s'.\n"
$ Msg 34 needs to match the 'all' from msgs 36 and 37.
34 "all"
$ Msg 35 needs to match the 'none' from msgs 36 and 37.
35 "none"
$ Msg 36 needs 'all' to match msg 34 and 'none' to match msg 35.
36 "Error line %1$d: image mode must be 'all' or 'none' -'%2$s' not recognized.\n"
$ Msg 37 needs "all" to match msg 34 and "none" to match msg 35.
37 "Error line %1$d: text mode must be 'all' or 'none' - '%2$s' not recognized.\n"
38 "Warning line %1$d: Key character redefined.\n"
39 "Error line %1$d: invalid Key Character:'%2$c'.\n"
40 "Warning line %1$d - key field redefined.\n"
43 "Error line %1$d - bad identifier '%2$s' for key.\n"
$ Msg 45 should not translate "discard."
45 "Error line %1$d: unknown option for 'discard': '%2$s'.\n"
$ Msg 46 should not translate "delblanklines."
46 "Error line %1$d: unknown option for 'delblanklines': '%2$s'.\n"
47 "Error line %1$d: Unknown option for abstract :'%2$s'\n"
48 "Error line %1$d -unknown identifier type '%2$s'.\n"
49 "Error - delimiter not defined.\n"
50 "Error - key-type character never defined.\n"
51 "Error - key never defined.\n"
52 "Error - delimiter defined as '%1$s' was never found.\n"
53 "Error - delimiter defined as '%1$s' references a physical line in the record.\n\
   Since the delimiter defines the physical lines\n\
  it cannot be referenced as a physical line.\n"
54 "Error - for field '%1$s', no line identifer matches '%2$s'.\n"
55 "Error - field include/exclude list included\n\
  the field '%1$s', which was never defined.\n"
56 "Error - image include/exclude list included\n\
  the line '%1$s', which was never defined.\n"
57 "Error - text include/exclude list included\n\
  the line '%1$s', which was never defined.\n"
58 "Error - key definition references field '%1$s'\n\
  which was never defined.\n"
59 "Error - abstract definition references field '%1$s'\n\
  which was never defined.\n"
68 "Warning - fields necessary for key not found.\n\
  discarding record %1$ld that began:\n\
  %2$s\n"
69 "Warning - record %1$ld deemed meaningless, discarding...\n\
  record began: %2$.60s\n"
70 " Unable to open input file '%1$s'.\n"
$ Msg 71 should not translate ".fzk."
71 "\n\
USAGE: %1$s [options] <profile> <infile> [<outfile>]\n\
  -m         Turn off all but error messages.\n\
  -w<N>      Change target screen width to <N>.\n\
  -oo        Preapprove overwrite of outfile.\n\
  -oa        Preapprove append to outfile.\n\
  <profile>  Input file containing profile of records\n\
             to be processed.\n\
  <infile>   Input file containing actual records.\n\
  <outfile>  Output file name, .fzk format (%2$s).\n\n"
72 "Invalid screen width specified.\n"
75 "'%1$s' is invalid output mode.\n"
76 "Unknown command line argument '%1$s'.\n"
77 "Missing required profile-file name.\n"
78 "Missing required input-file name.\n"
79 "Error - using stdin as input, output filename is required!\n"
80 "Profile file and input file have same name:'%1$s'.\n"
81 "Input file and output file have same name:'%1$s'.\n"
82 "Profile file and output file have same name:'%1$s'.\n"
83 "        Profile file: %1$s\n"
$ Msg 84 should not translate "stdin."
84 "          Input file: stdin\n"
85 "          Input file: %1$s\n"
$ Msg 86 should not translate "stdout."
86 "         Output file: stdout\n"
87 "         Output file: %1$s\n"
88 "%1$s. %2$s %3$s Text Filter.\n"
89 "\n  Each dot is %ld records...\n"
90 "\nError closing output file - disk full?\n"
91 "Quitting due to errors in profile file.\n"
92 "\n%1$s: Normal completion.  %2$ld records processed.  Exit code = 0.\n"
93 "Error line %1$d - '%2$s' string not enclosed in double quotes.\n"
110 "Warning line %d - date field redefined.\n"
111 "Error line %1$d - bad identifier '%2$s' for date.\n"
115 "%1$s Default object dates will be '%2$s'.\n"
116 "Error - date references undefined field '%s'.\n"
124 "%1$s Unable to write to output file '%2$s':\n  %3$s\n"
$ Msg 125 should not be translated.
125 "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC"
130 "%1$s Unable to load language processing files:\n%2$s\n"
133 "\n\
Warning - '%1$s' is invalid date specification.\n\
  Using '%2$s' date for record number %3$ld that began: %4$.30s\n"


$ ****************************************************
$ set 6 formerly set MS_ve
$set 6

26 "Called dummy_workproc().\n"
142 "%1$s Client Error: Requested record with invalid\n\
  database addr %2$ld (%3$d:%4$ld, x'%5$08.8lx') for database '%6$s'."
156 "%1$s Incompatible data compression table used for database '%2$s'.\n\
  Database compressed with %3$ld, engine decompressor is %4$ld.\n"
157 "DTSRVE157 Client Program Error: Null database address in usrblk.dba."
167 "%1$s No data in database '%2$s'."
178 "%1$s Database '%2$s' version '%3$s' incompatible\n\
  with Engine version '%4$s'."
183 "%1$s Could not open user notes semaphore file '%2$s': %3$s.\n"
185 "%1$s Database '%2$s' maximum word size %3$d is too short."
199 "%1$s Could not acquire user notes semaphore '%2$s' within %3$d tries.\n"
230 "%1$s Could not open user notes backup file '%2$s': %3$s."
231 "%1$sDatabase '%2$s' has invalid abstract size %3$d."
251 "%1$s Database '%2$s' corrupted:\n\
Incompatible record counts and database addresses.\n"
$ Msgs 310 and 311 can be appended to msg 309 to produce output.
$ For example, one message might be
$ "<Programname> User notes disabled for entire Engine."
309 "%1$s User notes disabled "
$ Msg 310 can be appended to msg 309 to produce output.
310 "for entire Engine."
$ Msg 311 can be appended to msg 309 to produce output.
311 "for database '%1$s'."
$ Msg 332 can be translated to local timestamp.  It currently outputs
$ something like "1996/03/04 at 16:40 CDT"
332 "%Y/%m/%d at %H:%M %Z"
333 "\n <User Note Appended by '%1$s' on %2$s>\n"
398 "DTSRVE398 NULL query string."

$ ****************************************************
$ set 7 formerly set MS_uiutil, no longer used in uiutil/dtsrutil.c.

$ ****************************************************
$ set 8 formerly set MS_loadocf, for module ocf.c.

$ All words in set 8 which are capitalized should not be translated.
$ They represent keywords in a configuration file.  Words like PATH,
$ SEARCH, MAXHITS, FILEIO, KEYTYPE, FILEPATH, KEYTYPES, KEYTYPE,
$ UPPERKEYS, FALSE, and HOME.

$set 8

$ Msg 1 is "<Programname><filename>: <keyword> keyword is obsolete."
1 "%1$s%2$s: %3$s keyword is obsolete."
$ Msg 140 "(missing)" refers to a token which is expected to be found but
$ is missing.  The token is from an input string of words being indexed.
140 "(missing)"
$ Msgs 142-168 are combined into "<token>: <error>" where msg 142 or a
$ default file name can be <token> and msgs 143, 165, 166, and 168
$ can be <error>.
142 "rest of line"
143 "is empty"
165 "unknown id"
166 "previously specified"
168 "missing path specification"
184 "%1$s %2$s: More than 8 characters in Database name '%3$.16s'.\n"
$ Msg 221 should not translate "PATH."
221 "%1$s %2$s: Database '%3$s', No PATH specified, statement ignored.\n"
$ Msg 275 should not translate "SEARCH."
275 "%1$s %2$s: Invalid SEARCH value ignored."
$ Msg 304 should not translate "MAXHITS."
304 "%1$s %2$s: Database '%3$s', invalid MAXHITS value.\n"
352 "%1$s %2$s: Invalid %3$s value, line ignored.\n"
365 "%1$s %2$s: %3$s value is missing."
414 "%1$s Unable to open configuration file '%2$s':\n  %3$s."
457 "'%1$c' Records"
477 "%1$s %2$s: Missing database name after '%3$s' keyword.\n"
495 "%1$s %2$s: '%3$.16s' is invalid keyword.  Line %4$d ignored.\n"
$ Msg 607 should not translate "FILEIO."
607 "%1$s %2$s: Invalid FILEIO value.  User file reads and writes prohibited."
610 "%1$s %2$s: %3$s value is '%4$s'. Should be positive integer. Line ignored."
$ Msg 684 should not translate "KEYTYPE."
684 "%1$s %2$s: Database '%3$s', Invalid KEYTYPE character '%4$c'."
732 "%1$s %2$s: Database '%3$s', No more than %4$d keytypes allowed.\n"
$ Msg 903 should not translate "FILEPATH."
903 "%1$s %2$s: FILEPATH '%3$s' %4$s. Line ignored."
929 "%1$s Opened old style configuration file '%2$s'."
$ Msg 986 should not translate "KEYTYPES."
986 "%1$s %2$s: KEYTYPES missing for database '%3$s'."
$ Msg 1002 should not translate "KEYTYPE."
1002 "%1$s %2$s: Database '%3$s', Duplicate KEYTYPE character '%4$c'.\n"
1007 "%1$s%2$s: Empty %3$s string ignored."
$ Msg 1011 should not translate "KEYTYPE," "UPPERKEYS," or "FALSE."
1011 "%1$s %2$s: Database '%3$s': Records with lowercase\n\
  KEYTYPE character '%4$c' will be inaccessible.\n\
  Set UPPERKEYS = FALSE if lowercase keys are intended."
1404 "%1$s Status unavailable for configuration file '%2$s': %3$s"
$ Msg 1643 should not translate "DTSROCFPATH," "HOME," "/etc/inittab,"
$ or "/etc/inetd.conf."
1643 "%1$sCannot find or open '%2$s'\n\
configuration file in any of the following locations:\n\
  DTSROCFPATH directory =     %3$s,\n\
  current working directory = %4$s,\n\
  HOME directory =            %5$s,\n\
  /etc/inittab directory =    %6$s,\n\
  /etc/inetd.conf directory = %7$s."
1869 "%1$s %2$s: Configuration file must specify at least one database."

$ ****************************************************
$ set 9 formerly set MS_oeinit, used in oeinit.c and opendblk.c.
$set 9

$ Msg 99 should not translate "open_dblk".  It is a function name.
99 "%s Programming Error: Invalid input to open_dblk()."
137 "%1$s User Interface version %2$s and Engine version %3$s are incompatible."
202 "%s Initialization failed due to errors in configuration file."
265 "%s No valid databases remain."
266 "%1$s Initialization failed due to errors in database,\n\
  language, or related files."
317 "%1$s Cannot open database file '%2$s'\n\
  Errno %3$d = %4$s\n\
  %5$s is removing '%6$s' from list of available databases."
318 "\n  This can usually be corrected by specifying a valid\n\
  database PATH in the site configuration file."
1404 "%1$s Removing database '%2$s' from list of available databases because status is unavailable for file %3$s: %4$s"

$ ****************************************************
$ set 10 formerly set MS_oe, used in oe.c and dbchange.c.
$set 10
48 "OE48 Request Aborted: '%1$s' database not available at this site."
71 "OE71 %1$s has expired."
$ Msg 76 should not translate "dblk".  It is a structure name.
76 "OE76 Program Error: Cannot find dblk for database '%1$s'."
217 "OE217 Program Error: record text too long."
367 "OE367: User Interface Error.  %1$d is invalid request code.\n"
429 "OE429 Unable to open deletion file '%1$s': %2$s. Request ignored."
440 "OE440 No record keytypes were selected in database '%1$s'."
490 "OE490 %1$s Engine permanently disabled."
516 "OE516 No more than %1$d characters are allowed in the search string."
523 "OE523 Request Denied: First request must be Engine Initialization."
753 "%1$s Semantic categories are not defined for database '%2$s'."
1300 "%1$s Cannot access status of database file\n  '%2$s': %3$s"
1313 "%1$s *** REQUEST CANCELED *** %2$s Engine reinitialized\n\
  due to modification of file %3$s, probably caused by\n\
  update to one or more databases."


$ ****************************************************
$ set 11 formerly set MS_cravel, used in dtsrload.c.
$set 11
1 "\n\
USAGE: %1$s -d<dbname> [options] <infile>\n\
       Listed default file name extensions can be overridden.\n\
  -d<dbname>  1 - 8 character database name, include optional path prefix.\n\
              File name extensions automatically appended.\n\
  -t<etxstr>  End of text document delimiter string.  Default '\\f\\n'.\n\
  -c          Initialize database record count by counting records.\n\
  -p<N>       Print a progress dot every <N> records (default %2$d).\n\
  -h<N>       Change duplicate record id hash table size from %3$ld to <N>.\n\
              -h0 means there are no duplicates, do not check for them.\n\
  -e<path>    Path-filename of huffman encode table (default %4$s).\n\
  <infile>    Input [path]file name.  Default extension %5%s.\n"
2 "%1$s: Exit code = %2$d\n"
13 "%1$s Duplicate record id checking disabled.\n"
14 "\n%1$s Unknown command line argument '%2$s'.\n"
15 "\nMissing required input file name."
16 "\nNo database name specified (-d argument).\07"
17 "%1$s Initializing total record count in database by actually counting...\n"
18 "%1$s: '%2$s' schema version = %3$s, record count = %4$ld, last slot = %5$ld.\n"
19 "%1$s: Final database record count = %2$ld, last slot = %3$ld.\n"
20 "%1$s %2$ld input records processed in %3$ldm %4$lds, (%5$ld%%).\n\
  %6$ld duplicates, %7$ld new, %8$ld updates.\n"
29 "%1$s Unable to get status for %2$s: %3$s\n"
30 "%1$s File %2$s is empty.\n"
31 "%1$s: Each dot = %2$d records processed.\n"
33 "%1$s Discarded record number %2$ld:  Key too long:\n  '%3$s'.\n"
34 "%1$s Out of Memory!  Set -h arg to a smaller number,\n\
  or reduce the number of input records."
35 "%1$s: Discarded duplicate record number %2$ld '%3$s'.\n"
246 "\n%1$s '%2$s' is invalid path/dictionary name.\n"
522 "%s This version of %s does not support semantic databases.\n"
$ Msg 579 should not translate ".fzk."
579 "%1$s Discarded record number %2$ld:  Invalid .fzk file format.\n"
582 "%1$sIgnored invalid progress dot argument '%2$s'.\n"
666 "%1$sProgramming Error: Unknown state = %2$d.\n"
717 "%1$s Aborting: Uncompressed text not yet supported.\n"
844 "%1$sAborting at %2$ld warnings.\n\
  Warning count is specified on command line.\n"
926 "%1$s Arg -m ignored: %2$s: %3$s.\n"
927 "%1$sDiscarded record number %2$ld:  First character (keytype) of key\n\
  '%3$s' is not alphanumeric.\n"
1048 "%1$sDatabase objects not changed because input file effectively empty.\n"
1086 "%1$s Record '%2$s' has invalid date format--using run date.\n"
1153 "%1$s Current data compression table id in '%2$s' is %3$ld.\n\
  Database '%4$s' previously compressed with table %5$ld.\n"
1215 "%1$s Warning.  Record '%2$s' has no text.\n"


$ ****************************************************
$ set 12 formerly set MS_initausd, used in dtsrcreate.c.
$set 12

$ Msg 3 should not translate "dtsearch.dbd."
3 "\n\
USAGE: %1$s [options] <dbname>\n\
       Creates and initializes DtSearch/AusText database files.\n\
  -q       Do not print information messages.\n\
  -o       Ok to overwrite preexisting database.\n\
  -a<N>    Set maximum abstract size to <N> (default per flavor).\n\
  -d<dir>  Where to find dtsearch.dbd file if not in <dbname> directory.\n\
  -wn<N>   Change minimum word size to <N>.  Default %2$d.\n\
  -wx<N>   Change maximum word size to <N>.  Default per language.\n\
  ---------- Database Flavor ----------\n\
  -fd      DtSearch flavor.  No documents, only document references\n\
           in abstracts (default).\n\
  -fa      AusText flavor.  Documents stored in central server repository.\n\
  ------------ Supported Languages ------------\n\
  -l<N>    Set language number to <N>.  Default is 0.  Supported values:\n\
           0 English-ASCII\n\
           1 English-Latin1\n\
           2 Spanish\n\
           3 French\n\
           4 Italian\n\
           5 German\n\
           6 Japanese-autoknj\n\
           7 Japanese-knjlist\n\
  <dbname> Optional path prefix, then 1 - 8 character\n\
           database name.  Do not specify 'austext' or 'dtsearch'.\n"

5 "DTSRCREATE Minimum word size %1$d greater than maximum word size %2$d.\n"
6 "DTSRCREATE Adjusted minimum word size to %1$d.\n"
8 "DTSRCREATE Adjusted maximum word size to %1$d.\n"
9 "DTSRCREATE Specifying small minimum word sizes may require extensive\n\
  editing of stopword file to prevent significantly\n\
  increased index storage requirements.\n"
10 "DTSRCREATE Specifying large maximum word sizes may significantly\n\
  increase storage requirements.\n"
$ Msg 12 needs option "y" for "yes" to be translated the same as in msg 15.
$ Option "n" does not need a translation because the code assumes anything
$ other than the translated "y" option means "no."
12 "\nFile '%s' already exists.\n\
Is it ok to overwrite it and other database files? [y,n] "
$ Msg 13 should not translate "load_custom_language."
13 "%s Warning! you have specified an unsupported, custom language.\n\
  You will have to provide your own language loaders at run time\n\
  in user function 'load_custom_language' to access this database.\07\n"

14 "\n\
%s semantic processing is only available for English language databases.\n\07"
$ Msg 15 needs "y" to match the "yes" option in msg 12.
15 "y"
24 "DTSRCREATE Successfully initialized database '%1$s'.\n"
98 "DTSRCREATE %1$s Invalid Huffman compression table id."
186 "DTSRCREATE186 Unable to rename '%1$s' to '%2$s':\n  %3$s\n"
205 "DTSRCREATE205 Unable to change permissions for '%1$s': %2$s\n"
213 "%1$s Unable to open '%2$s':\n  %3$s.\07\n"
214 "%1$s Unable to copy '%2$s' to '%3$s':\n  %4$s\n"
230 "DTSRCREATE230 Could not open database '%1$s'.\n"
239 "DTSRCREATE239 Could not initialize database '%1$s'.\n"
244 "DTSRCREATE244 Unable to remove '%1$s':\n  %2$s\n"
326 "DTSRCREATE326 Unable to change permissions of '%1$s':\n  %2$s\n\
  All database files must be readable and writeable.\n\
  May not be able to open or initialize database files.\n"
327 "  However it may not be usable unless all database files\n\
  have both read and write permissions."
433 "%1$sAdjusted maximum abstract size upward to %2$hd.\n"
509 "DTSRCREATE509 Could not initialize database header record.\n"

$ ****************************************************
$ set 13 formerly set MS_vista.
$ Msg numbers 1-100 and 900-1000 match
$ user_error[] and system_error[] respectively in dberr.h.
$ Used in dberr.c, ausdopen.c, and opendblk.c.

$ Msgs 1-100 and 900-1000 in set 13 are generated during database
$ input/output.  They are all fatal errors and should be treated as
$ standalone messages.  They are not combined with other messages to
$ produce errors.  The messages come from a third party vendor, so we
$ do not intend to modify the vendor code and make complete sentences
$ out of them.

$set 13

$ For msgs 1-100, see comments at top of set 13.
1 "database not opened"
2 "invalid set"
3 "invalid record"
4 "invalid database"
5 "invalid field name"
6 "invalid db_address"
7 "no current record"
8 "set has no current owner"
9 "set has no current member"
10 "key value required"
11 "invalid lock value"
12 "record is owner of non-empty set(s)"
13 "record is member of set(s)"
14 "member already owned"
15 "field is a compound key"
16 "record not connected to set"
17 "field is not a valid key"
18 "record not legal owner of set"
19 "record not legal member of set"
20 "error in d_setpages (database open or bad param)"
21 "incompatible dictionary file"
22 "illegal attempt to delete system record"
23 "illegal attempt to lock locked set/record"
24 "attempt to access unlocked set/record"
25 "transaction id not supplied"
26 "transaction already active"
27 "transaction not active"
28 "transaction cannot begin due to locked files"
29 "cannot free locks within a transaction"
30 "too many changed database pages"
31 "cannot update database outside a transaction"
32 "exclusive access required"
33 "write locks not allowed on static files"
34 "unspecified or duplicate user id"
35 "database, path or file name too long"
36 "invalid file number was passed to d_renfile"
37 "field is not an optional key field"
38 "field is not defined in current record type"
39 "record/field has/in a compound key"
40 "invalid record or set number"
41 "record/set not timestamped"
42 "bad DBUSERID (contains non-alphanumeric)"
43 "NetBIOS has not been installed on machine"
44 "wrong number of elements in DBDPATH"
45 "wrong number of elements in DBFPATH"
46 "no current record type"
47 "invalid country table sort string"
48 "database not closed"
$ Msg 301 should not translate "errno."
301 "\n*** System I/O errno %1$d = %2$s\n"
302 "\n\
*** The usual cause for this kind of error is a missing\n\
*** or read-only database file, or some system limit\n\
*** on the number of open files has been exceeded."
303 "(unspecified location)"
$ Msg 304 should not translate "db_VISTA."
304 "\n*** db_VISTA auto recovery in process...\n"
$ Msg 311 should not translate "db_status."
311 "*** Database error at %1$s, db_status = %2$d: %3$n"
312 "Programming Error."
313 "Unknown Error."
315 "Database not available because it is in use by other users."
348 "%1$s Invalid database name '%2$s'."
378 "%1$s Could not open following database name string:\n  '%2$s'"
379 "%1$s Could not open database '%2$s':\n  %3$s."

$ For msgs 900-1000, see comments at top of set 13.
900 "no more space on file"
901 "system error"
902 "page fault"
$ Msg 903 should not translate "dio."
903 "no working file in dio"
904 "unable to allocate sufficient memory"
905 "error opening file"
906 "unable to get access to database taf file"
907 "database taf/log file error"
908 "inconsistent database locks"
909 "file record limit exceeded"
910 "key file inconsistency"
911 "maximum concurrent user limit exceeded"
912 "error seeking in database file"
913 "invalid file specified"
914 "error reading from a database/overflow file"
915 "lock manager synchronization error"
916 "debug check interrupt"
917 "network communications error"
918 "auto-recovery in process"
919 "error writing to a database/overflow file"
920 "no lock manager is installed"
921 "DBUSERID is already being used"
922 "the lock manager is busy"


$ ****************************************************
$ set 14 formerly set MS_cborodin, used in dtsrindex.c.
$set 14

1 "%1$s.  Run %2$s.\n"
2 "%1$s: Warnings were detected."
$ Msg 6 should not be translated.
6 "put_addrs_2_dtbs_addr_file() nitems=%1$d, batchsz=%2$ld\n"
$ Msg 9 should not translate "d99."
9 "\n\
\n%1$s Corrupted d99 file for word '%2$s',\n\
  database address %3$ld @ file position %4$ld => bitvector[%5$ld],\n\
  but maximum bitvector allocation = %6$ld.\n"
15 "%1$s Fewer than 2 bytes in input.\n"
17 "\n\
USAGE: %1$s -d<dbname> [options] <infile>\n\
       Listed default file name extensions can be overridden.\n\
  -d<dbname>  1 - 8 character database name, include optional path prefix.\n\
  -t<etxstr>  End of text document delimiter string.  Default '\\f\\n'.\n\
  -r<N>       Change Pass 1 records-per-dot from %2$d to <N>.\n\
  -b<N>       Change max batch size from %3$ld to <N>.\n\
  -c<N>       Change database paging cache from %4$ld 1K pages to <N> 1K pages.\n\
              <N> >= 16 by powers of 2.  Initially try only small changes.\n\
  -i<N>       Change (i)nput buffer size from default %5$d to <N>.\n\
  -h<N>       Change duplicate record id hash table size from %6$ld to <N>.\n\
              -h0 means there are no duplicates, do not check for them.\n\
  <infile>    Input [path]file name.  Default extension %7$s.\n"
21 "\n\
\n%s '%1$s' record overflows word counter array.\n\
  Record number %2$ld > maxdba %3$ld, dba=%4$ld, sld00=%5$d, offs=%6$d.\n"
22 "\n\
\n%1$s '%2$s' record in database (dba=%3$ld)\n\
  overflows bitvector allocation (%4$ld >= %5$ld).\n"
33 "\n%1$s Number of incoming records exceeded %2$d.\n\
  This will usually result in 'Out of Paging Space' error in Pass 2\n\
  and corruption of database.  Either split the incoming file to reduce\n\
  record count or use the -b option, and rerun.\n"
40 "Ignored %1$ld duplicate records"
41 "No duplicate records found"
164 "\n%1$s: %2$s Abort due to signal %3$d.  Database %4$s\n\
  possibly corrupted.  Restore backup database.\n"
206 "%1$s: Exit code = %2$d, Total elapsed time %3$ldm %4$lds.\n"
$ Msg 280 should not be translated.
280 "\n%1$s num_addrs_for_word (%2$ld) >= batchsz (%3$ld).\n"
288 "%1$s Abort. There are no words in the input file %2$s.\n"
374 "\n%1$s Out of Memory!\n\
  Split the incoming file into several smaller files and try again.\n"
539 "%1$s Duplicate record id checking disabled.\n"
550 "%1$s '%2$s' is invalid path/database name.\n"
558 "%1$s Invalid input buffer size '%2$s'.\n"
567 "%1$s Unknown command line argument '%2$s'.\n"
577 "%1$s Invalid arg '%2$s'.  Using default -r%3$d.\n"
580 "%1$s Missing required input file name.\n"
589 "%1$s No database name specified (-d argument).\07\n"
595 "%1$s Invalid batch size argument '%2$s'.\n"
600 "%1$sCache size readjusted to %2$d.\n"
659 "\n%1$s: %2$s Discarding record, key too long:\n  '%3$s'.\n"
713 "\n%1$s Word '%2$s' occurs in %3$ld records,\n\
  but there are only %4$ld records in database!\n\
  (This may be a good candidate for the stoplist).\n"
727 "\n%1$s Number of dbaddr records (%2$ld)\n\
exceeds bitvector allocation (%3$ld).\n"
$ Msg 776 should not translate "d99."
776 "%1$s Write Failure d99 file: %2$s\n"
$ Msg 848 should not translate "d99."
848 "\n%1$s Could not fread %2$ld bytes (%3$ld dba's) of d99 file\n\
  at offset %4$ld.  Number of dba's read (return code) = %5$ld.\n"
849 "\n%1$s: Word number %2$ld, %3$.0f%% done.  Est %4$lum %5$02lus to completion.\n"
$ Msg 875 should not translate "fseek" or "d99."
875 "\n%1$s Could not fseek d99 file to offset %2$ld.\n"
1068 "%1$s Cannot open new inverted index file '%2$s': %3$s\n"
1083 "%1$s Cannot read input file '%2$s': %3$s\n"
1097 "%1$s Aborting due to errors in loading language files.\n"
1108 "%1$s: Beginning Pass 1, reading records from '%2$s'.\n\
   Each dot = %3$d records.\n"
$ Msg 1129 should not translate ".fzk."
1129 "%1$s: %2$s Invalid .fzk file format.\n"
1168 "%1$s: %2$s Discarded '%3$s', key not in database.\n"
1190 "\n%1$s: Record number%2$lu, %3$.0f%% done.  Est %4$lum %5$02lus to end Pass 1.\n"
1225 "%1$s: Pass 1 completed in %2$lum %3$lus, read %4$lu records.\n\
  %5$s, parsed %6$lu words.\n"
1233 "%1$s: Beginning Pass 2: batch index traversal and database update.\n\
  Each dot = %2$d words.\n"
1246 "%1$s: Pass 2 completed in %2$lum %3$lus, updated %4$lu words.\n"
1402 "%1$s: Discarded duplicate record number%2$lu '%3$s'.\n"


$ ****************************************************
$ set 15 formerly set MS_delsfx, now MS_lang, used in lang.c and jpn.c
$set 15

$ Msg 4 should not translate "dblk."  It is a structure name.
4 "%s dblk not initialized.\n"
$ Msg 5 should not translate "parg."
5 "%s Program Error: parg contains neither file nor string.\n"

8 "\n\
%1$s '%2$.*s...' is larger\n\
than the maximum word size of database '%3$s'."

10 "%s Program Error: Unknown parser state.\n"
12 "%1$s '%2$.120s' is not a valid word in database '%3$s'."

17 "\n\
%1$s '%2$s' is less than the\n\
minimum word size of database '%3$s'."

19 "%1$s The word '%2$s' is not indexed in database '%3$s'."
20 "%1$s Program Error: Unknown jstate %2$d.\n"
21 "%1$s '%2$.*s' is not a valid Japanese word."
23 "%1$s Word '%2$s' on line %3$ld is too short."
24 "%1$s No Japanese words in word file '%2$s'."
25 "%1$s Database '%2$s' path too long: '%3$s'."
31 "%s Stemmer suffixes file never loaded.\n"
$ Msg 41 should not translate "Paice."
41 "%1$s %2$s: Invalid Paice Rule starting at '%3$s'."
$ Msg 42 should not translate "Paice."
42 "%1$s %2$s: Paice Rules out of order starting at '%3$s'."
45 "%s Out of Memory.\n"
50 "English-ASCII"
$ Msg 51 refers to the Latin 1 codeset.
51 "English-Latin1"
52 "Spanish"
53 "French"
54 "Italian"
55 "German"
56 "Japanese-comp"
57 "Japanese-.knj"


$ ****************************************************
$ set 16 formerly set MS_vewords, now MS_boolsrch, used in boolsrch.c.
$set 16

$ Msg 2 is like "<program name> Part of ... term <word> which is
$ not in database <database name>."
2 "\n\
%1$s Part of a collocation operation includes\n\
term '%2$s' which is not in database '%3$s'."
$ Msg 6, variable 3 is the database name.
6 "\n\
%1$s Database Error.  Word '%2$s' is\n\
listed in database '%3$s' but has no index record."
$ Msg , variable 3 is the database name.
11 "%1$s Word '%2$s' was not found in database '%3$s'."
$ Msg 14 should not translate "WHITLIM."  The message is like
$ "<Program name> <word in query> has more than <some number> hits."
14 "\n\
%1$s '%2$s' has more than %3$ld hits.\n\
Please remove it from the query or raise the WHITLIM\n\
value in the search engine configuration file."
15 "\n\
%s This query requires retrieving every\n\
document in the database that does not have any of\n\
the query words.  This type of search may take an\n\
unusually long time."

$ Msg 28 should not translate "d99."  Variable 2 is the database name.
28 "%1$s Database Read Error in %2$s.d99."
$ Msg 32 should not translate "d99."  The message is formed using either
$ msg 33 or 34 as variable 2, and it is like "<program name> Database
$ Error: Word <word> in database <database name> has invalid d99 record
$ number <number>."
32 "\n\
%1$s Database Error: %2$s '%3$s'\n\
in database '%4$s' has invalid d99 record number %5$ld."
$ Msg 33 is used for variable 2 of message 32.
33 "Word"
$ Msg 34 is used for variable 2 of message 32.
34 "Stem of"
$ Msg 35 should not translate "stemct," and "pmsz."
35 "%1$s Program Error: stemct=%2$d pmsz=%3$d\n"

$ ****************************************************
$ set 17 formerly set MS_vestatis
$set 17

104 "Cannot detach shared memory."
107 "Qsort stack overflow.\n"
110 "Cannot remove shared memory."
115 "No shared memory available"
310 "%1$s Out of Memory.  Need %2$ld bytes.\n"
314 "Out of Memory.\n"
421 "Total Number Hits = %1$ld. Discarded hits beyond maximum number specified."
$ Msg 437 should not be translated.
437 "%1$s db_status = %2$d, dba = %3$d:%4$ld (x'%5$08.8lx'), vistano = %6$d.\n"
504 "Cannot allocate cur_ditto.\n"
677 "Query field is empty."
990 "The system is now sorting. Please wait."


$ ****************************************************
$ set 23 formerly set MS_dbrec

$ Set 23 has several words in parenthesis which should not be translated.
$ For example, msg 3: "objkey,"  msg 5: "fzkeysz," msg 12: "abstrsz," 
$ msg 24: "minwordsz," msg 26: "maxwordsz," ...  The single words in
$ parenthesis should not be translated.
$ Msgs like 10, where the entire text is in parentheses should be translated.

$set 23

1 "---------- System Values for Database '%1$s' ----------\n"
2 "Schema version number (version) is '%1$s'.\n"
$ Msg 3 should not translate "sizeof" or "objkey."
3 "Maximum object key size (sizeof(objkey)) is %1$ld bytes.\n"
$ Msg 4 "Huge" means very, very large.
4 "Optional 'Huge' keys enabled.\n"
5 "Maximum number of semantic categories (fzkeysz) is %1$d.\n"
10 "  (Semantic access to objects is disabled)."
12 "Maximum length of an abstract string (abstrsz) is %1$d.\n"
14 "  (Abstracts are not used in this database)."
20 "Abstracts are %1$scompressed.\n"
22 "Parsing language is number %1$d, %2$s.\n"
24 "Minimum word length (minwordsz) is %1$d.\n"
26 "Maximum word length (maxwordsz) is %1$d.\n"
30 "Number of .d00 slots per object (recslots) is %1$d.\n"
36 "  (Maximum number of database objects is %1$ld).\n"
40 "Huffman compression table id (hufid) is %1$ld.\n"
42 "  (Compression is disabled in this database)."
44 "  (Specific compression table is not yet determined)."
50 "Engine accessibility to data may vary from object to object."
54 "Data objects are not directly accessible from the engine."
56 "Data objects are stored internally as compressed text."
60 "Only server file references to objects are stored in the compressed text."
64 "Only client file references to objects are stored in the compressed text."
68 "Object keys are server file references to the objects."
72 "Object keys are client file references to the objects."
74 "Server file references to objects are stored in the 'huge' keys."
80 "Server file references to objects are stored in the abstracts."
86 "Client file references to objects are stored in the abstracts."
90 "Error: meaning of or_dbaccess value (%1$hd) is unknown.\n"
100 "Repository compressed text is %1$scompressed.\n"
110 "Repository compressed text is not used in this database."
120 "Database switches (dbflags) are 0x%1$lx:\n"
$ Msgs 124 and 125 are used in the variable in messages 130 and 140.
$ For example: "Inverted index INCLUDES words exactly as parsed."
124 "INCLUDES"
125 "EXCLUDES"
$ Msgs 126 and 127 are used in the variable in messages 160, 162, 164.
$ For example, "Use of optional 'huge' keys is ENABLED."
126 "ENABLED"
127 "DISABLED"
$ Msg 130 uses msgs 124 and 125 in the variable.
130 "  Inverted index %1$s words exactly as parsed.\n"
$ Msg 140 uses msgs 124 and 125 in the variable.
140 "  Inverted index %1$s word stems.\n"
$ Msg 160 uses msgs 126 and 127 in the variable.
160 "  Use of optional 'huge' keys is %1$s.\n"
$ Msg 162 uses msgs 126 and 127 in the variable.
162 "  Mark-for-deletion is %1$s.\n"
$ Msg 164 uses msgs 126 and 127 in the variable.
164 "  Appendable user notes are %1$s.\n"
$ Msg 170 is followed by either msg 172 or msg 174.
$ For example, "Text characters are a SINGLE byte wide."
170 "  Text characters are %1$s wide.\n"
172 "TWO bytes"
174 "a SINGLE byte"
200 "Current number of database objects (reccount) is %1$ld.\n"
210 "Last currently used slot number (maxdba) is %1$ld.\n"
310 "\n\
USAGE: %1$s <dbname>\n"
330 "Could not open '%1$s' database.\n%2$s\n"
340 "No dbrec record in database '%1$s'.\n"
350 "Cannot read dbrec record in database '%1$s'.\n%2$s\n"

$ ****************************************************
$ set MS_dtsrkdump is set 25.  5/20/96.  dtsrkdump.c
$set 25

1 "%s Program Error Abort.\07\n"
$ Msg 2 should not be translated
2 "%1$c dba=%2$d:%3$-7ld ofs=%4$-9ld adr=%5$-6ld fre=%6$ld\n"
$ Msg 3 is like "<program name> <RCS revision number>, engine
$ <DtSearch version>.  <time stamp>."
3 "%1$s %2$s, engine %3$s.  %4$s.\n"
$ Msg 4, option "-p" can include decimal points -- this is handled by
$ using sscanf, so if sscanf can read numbers with commas in them, then
$ the "-p" option will process that correctly.  atof is used on what
$ sscanf reads in.
4 "\n\
USAGE: %1$s -o|w|ow [-v] [-t<N> | -p<N>] <dbname>\n\
       Reads DtSearch key files and prints summary report.\n\
  -o        Keys examined are OBJECT record keys.\n\
  -w        Keys examined are inverted index WORDS.\n\
  -v        VERBOSE mode, lists every key.\n\
  -t<N>     Threshold.  Sets w and v options, and lists only words\n\
            with >= <N> addresses.  All words will be listed if <N> = 1.\n\
  -p<N>     Another threshold.  Same as -t except <N> is percent\n\
            of the entire database (<N> may include a decimal point).\n\
            For example -p99.9 prints out every word that occurs\n\
            in 99.9%% or more of the records--an excellent way to find\n\
            candidates for the stop list.\n\
            If w and v are set without threshold, default is -t%2$d.\n\
  <dbname>  1 - 8 character database name with optional path prefix.\n"
5 "%1$s Invalid percent value %2$lf.\07\n"
53 "%1$s Invalid threshold value.\07\n"
55 "%1$s Unknown command line argument '%2$c'.\07\n"
56 "%1$s Missing required database name.\07\n"
57 "%1$s Either -o or -w must be specified.\07\n"
58 "%1$s Invalid database name '%2$s'.\07\n"
$ Msg 60 should not be translated.
60 "%1$s: '%2$s' reccount=%3$ld maxdba=%4$ld recslots=%5$hd minw=%6$hd maxw=%7$hd\n"
70 "%1$s Will only list words occurring in %2$ld or more records.\n"
80 "%1$s: Listing all words in database.\n"
$ Msg 100 should not be translated.
100 "dba x%1$08lx, %2$6ld\n"
$ Msg 110 is like "in <directory name>"
110 " in %1$s"
120 "Object Summary for '%1$s'%2$s:\n"
130 "Object Count by Keytypes:"
160 "TOTAL Object Count = %1$ld\n"
$ Msg 170 should not translate "DBA."
170 "Largest Object DBA  = %1$ld\n"
180 "%1$s: * Words marked with asterisk occur in every record.\n"
200 "Words Summary for '%1$s'%2$s:\n"
210 "TOTAL Word Count = %1$ld\n"

$ ****************************************************
$ set MS_dtsrclean is set 26.  5/21/96.  dtsrclean.c
$set 26

1 "\n\
USAGE: %1$s [options] <dbname> <newpath>\n\
       Compresses unused d99 space and validates d00-d99 links.\n\
  -p<N>     Progress dots printed every <N> records (default %lu).\n\
            Complete progress message printed every %d dots.\n\
  -oy       Authorizes overlaying preexisting d99/d2<N> files in newpath.\n\
  -on       Forces exit if preexisting d99/d2<N> files in newpath.\n\
  -v        Validates d99 and d00 links, uncorrupts d99 file, and ensures\n\
            accurate record count.  Also use -c0 to uncorrupt entire database.\n\
  -v<fname> Same as -v but also writes all d00 recs unreferenced by d99\n\
            to fname in format suitable to extract into .fzk file format.\n\
  -c<N>     Exits if more than <N> corrupted/incomplete links (default %d).\n\
            Corruption limit turned off by -c0.\n\
  <dbname>  1 - 8 char database name = the old d99/d2<N> files to be updated.\n\
            Files found in local directory.\n\
  <newpath> Specifies where the new d99/d2<N> files will be placed.\n\
            If first char is not slash, path is relative to local directory.\n\
EXIT CODES:\n\
  0: Complete success.  1: Warning.  2: Job never started.\n\
  3-49: Job ended prematurely, old files ok, new files unusable.\n\
  50-99: Fatal Error, even old database may be corrupted.\n\
  100+: Ctrl-C, kill, and all other signal interrupts cause premature\n\
     end, new files may be unusable.  Signal = exit code - 100.\n"
2 "%1$s: %2$s Compression %3$d%% (about %4$lu KB) in %5$ld:%6$02ld min:sec.\n"
$ Msg 3 should not translate "d99."  "WORDS" should be translated.
3 "%1$s: Counted %2$ld WORDS in %3$s.d99.\n"
4 "%1$s: No corrupted links detected.\n"
5 "%1$s: Exit code = %2$d.\n"
$ Msg 6 should not be translated.
6 "  DBA = %1$d:%2$ld (x%3$02x:%4$06lx),  orig addr val = x%5$08lx\n\
  Word='%6$c%7$s' offset=%8$ld addrs=%9$ld free=%10$d\n"
7 "%1$s: Copying from old d2<N> files to %2$s...\n"
$ Msg 11 is like "<program name> Version <version number>. Run <date program
$ was run>."
11 "%1$s Version %2$s.  Run %3$s.\n"
$ Msg 14 should not translate "d99."
14 "%1%s: Old files: '%2$s.d2x, .d99'.\n"
$ Msg 15 should not translate "d99."
15 "%1$s: New files: '%2$s.d2x, .d99'.\n"
24 "%1$s: One or more new files already exist.\n"
$ Msg 25 should not be translated.
25 "%1$s: curr reccnt=%2$ld, mxdba=%3$ld, sl/rec=%4$ld, tot#adr=%5$ld.\n"
26 "%1$s: Reading d00 file.  Each dot approximates %2$ld database documents...\n"
$ Msg 27 should translate DOCUMENTS.
27 "%1$s: Confirmed %2$ld DOCUMENTS in %3$s.d00.\n"
$ Msg 28 should translate DOCUMENTS.
28 "%1$s: %2$ld DOCUMENTS actually in %3$s.d00 not = %4$ld count stored there.\n\
  Count will be corrected in new d00 file.\n"
29 "%1$s: Compressing into %2$s.  Each dot approximates %3$lu words...\n"
$ Msg 45 needs the "y" for "yes" to match msg 46.
45 "    Overlay files in new directory? [y/n] "
$ Msg 46 must match the "yes" option from msg 45.
46 "y"
66 "%1$s Aborting after interrupt signal %2$d.\n"
$ Msg 111 should not translate "d99."
111 "*** %1$s DBA in d99 = -1.  Probable overrun into expansion\n\
  area due to incorrect count values in d2x file.\n"
159 "%1$s Unknown argument: '%2$s'.\n"
177 "%1$s Invalid %2$.2s argument.\n"
193 "%1$s Aborting at %2$ld corrupted links.\n"
194 "%1$s Detected%2$s %3$ld corrupted/incomplete link(s).\n"
210 "%1$s Missing required dbname argument.\n"
211 "%1$s Missing required newpath argument.\n"
212 "%1$s Too many arguments.\n"
$ Msg 222 should not translate "d99" or "DBA."
222 "*** %1$s DBA in d99 not in d00, slot > maximum number of documents.\n"
229 "%1$s Invalid database name '%2$s'.\n"
301 "%1$s The -m argument is no longer necessary.\n"
$ Msg 333 should not translate "d99" or "d00."
333 "*** %1$s DBA in d99 does not exist in d00.\n"
393 "%1$s Old and new directories are identical: '%2$s'.\n"
439 "%1$s Cannot open %2$s: %3$s\n"
440 "%1$s Cannot access status of %2$s: %3$s\n"
$ Msg 444 should not translate "d99."  Do not translate starting with "DBA = ..."
444 "*** %1$s d00 record '%2$s' is not referenced in d99.\n\
  DBA = %3$d:%4$ld (x%5$02x:%6$06lx).\n"
463 "%1$s Command line argument disallows file overlay.\n"
$ Msg 465 "Validate" should match the description for the "-v" option in msg 1.
465 "%1$s WARNING: Cannot allocate memory for bit vector.\n\
  'Validate' mode switched off.\n"
489 "%1$s Write error on %2$s: %3$s.\n"
499 "%1$s %2$s is empty.\n"
517 "%1$s Read error on %2$s: %3$s.\n"
$ Msg 561 should not translate "DBA," "maxdba," or "dbrec."
561 "%1$s DBA '%2$d:%3$ld' (record number %4$ld) in d00 exceeds total number of  addresses %5$ld;\n\
  Bit vector overflow because maxdba %6$ld in dbrec is incorrect.\n"
$ Msg 657 is like "<program name> Read error on <filename>: <system errno>."
657 "%1$s Read error on %2$s: %3$s.\n"
$ Msg 665 is like "<program name> Write error on <filename>: <system errno>."
665 "%1$s Write error on %2$s: %3$s.\n"
$ Msg 802 is like "<program name> Unable to open <filename> to output
$ unreferenced d00 records: <system errno>."  Do not translate "d00."
802 "%1$s Unable to open '%2$s' to output unreferenced d00 records:\n  %3$s\n"

$ ****************************************************
$ set MS_dui is set 27.  5/21/96.  dui.c
$set 27
1 "%1$s DtSearch Version %2$s.\n\
USAGE: %3$s [-options] <query>...\n\
       Options beginning with the same character are mutually exclusive.\n\
  -ss         Search type S (word stems) instead of default statistical search.\n\
  -sw         Search type W (exact words) instead of default statistical search.\n\
  -pa         Print abstracts (default).\n\
  -pk         Print record keys.\n\
  -pb         Print both abstracts and record keys.\n\
  -p<N>       Retrieve and print the <N>th record from the hitlist.\n\
  -k<C>       Restrict searches to keys beginning with character <C>.\n\
  -d<dbname>  1 - 8 character database name.  Defaults to\n\
              first database in site config file.\n\
  -m<N>       Maximum number of hits returned.  Defaults to %4$d.\n\
  <query>     Set of search words on remainder of command line.\n\
              Query may be enclosed in quotes to include boolean operators.\n"
4 "Searching '%1$s' Database...\n"
9 "<no abstract available>"
117 "%1$s Invalid command line argument '%2$s'.\07\n"
125 "%1$s No query specified.\07\n"
165 "%1$s DtSearch failed to initialize:\n  %2$s\n"
190 "%1$s No hits found.\n"
195 "%1$s Search failed.\07\n"
200 "%1$s Text not available for document '%2$s'.\n"
203 "%1$s Could not determine available keytypes:\n  %2$s\n"
224 "%1$s No record keys begin with '%2$c' in database '%3$s'.\n"
247 "%1$s ----- Engine Messages After Initialization -----\n%2$s\n"
248 "%1$s ----- End of Engine Initialization Messages -----\n"

$ ****************************************************
$ set MS_boolpars is set 28.   5/22/96.
$set 28

1 "\n\
%1$s This search string is an invalid\n\
boolean query.  Please reformulate and try again."

2 "%1$s Query field is empty."

$ Msg 3 should not translate "@".
3 "\n\
%1$s Collocation operator (@) may\n\
only be positioned between two words."

$ Msg 4 variable 2 is <database name>.
4 "\n\
%1$s One or more words in the\n\
query are not stored in database '%2$s'."

$ Msg 5 "AND" should be the local equivalent to the Boolean Operator
$ for "AND."  "&" should not be translated.
5 "\n\
%1$s Boolean operators must be positioned\n\
between words or expressions.  Two sequential words\n\
without an operator are interpreted as being separated\n\
by the AND operator (&)."

6 "%1$s Expression in parentheses is missing."

$ Msg 5 "NOT" should be the local equivalent to the Boolean Operator
$ for "NOT."  "~" should not be translated.
7 "\n\
%1$s NOT operator (~) must be positioned to\n\
the left of the word or expression it qualifies."

8 "%1$s Invalid boolean query.  Syntax Error number %2$d."

9 "%1$s Too many terms in boolean query."

$ Msg 10 variable 2 is <database name>.
10 "%1$s Collocation searches not available for database '%2$s'."

$ Msg 11 should not translate "@."
11 "\n\
%1$s Collocation operator '%2$.*s' is invalid.\n\
Correct format is '@<N>' where <N> is greater than zero."

$ Msg 12 should not translate "@."
12 "\n\
%1$s Collocation operator (@) is not\n\
permitted between identical words."

13 "%1$s Word '%2$.*s' is invalid."

14 "%1$s String '%2$.*s' is not a single word."

15 "\n\
%1$s This query cannot logically return\n\
any records.  Please reformulate and try again."

16 "\n\
%1$s This query will return entire database\n\
'%2$s'.  Please reformulate and try again."


$ ****************************************************
$ set 29 symbolically named MS_tomita, used in tomita.c (dtsrdelete)
$set 29

1 "\n\
\n%1$s Received interrupt %2$d.\n\
  Program will stop after current batch of deletions.\n"

3 "%1$s: Exit Code = %2$d.\n"
4 "%1$s Database '%2$s' not found.\n"
5 "Available choices are:"
8 "%1$s Invalid input format: %2$.30s...\n"
$ Msg 24 is like <Program name> Database <dbname>, <record id> not found."
24 "%1$s Database %2$s, '%3$s' not found.\n"

25 "\n\
DO NOT CONTINUE under any of the following circumstances:\n\
-> If the input file which lists record ids to be deleted is not\n\
      named '%1$s'.\n\
-> If any users are still accessing the affected database(s).\n\
-> If any database files have not been backed up.\n\n\
If you are sure you are ready to start deleting, enter 'y' now... "

$ Msg 26 is "<program name> <number of files> read, <number> deleted,
$ <time> elapsed. Database <dbname>: Current record count = <number>,
$ Batch size = <number of records>."
26 "%1$s %2$ld read, %3$ld deleted, %4$ldh %5$2ldm %6$2lds elapsed.\n\
  Database '%7$s': Current record count = %8$ld, Batch size = %9$d.\n"
$ Msg 27 is "<program name> <number of records> records read from input file.
$ <number> were deleted and <number> were not found in <time>." 
27 "%1$s %2$ld records read from input file.  %3$ld were deleted and\n\
  %4$ld were not found in %5$ld hours, %6$ld minutes, %7$ld seconds,\n"

28 "  or an average of %1$ld minutes, %2$ld seconds per record deleted.\n"
$ Msg 29 is "<program name>. Run <time stamp>."
29 "%1$s.  Run %2$s.\n"

$ Msg 30 should not translate "-y".
30 "\n\
USAGE: %1$s [options]\n\
  -i    Input file name.  If not specified, defaults to %2$s.\n\
  -d[v] Print debug statements.\n\
        -dv turns on verbose (record-by-record) debugging.\n\
  -t<N> Maximum desired number of seconds of run time.\n\
        Ctrl-C/Break will also stop deletion at next record.\n\
  -n<N> Change number of records in a batch from %5$d to <N>.\n\
  -y    Automatically answers 'yes' to Delete mode confirm prompt.\n\
  -d    trace deletion operations.\n"

31 "%1$s Unknown argument '%2$s'.\n"
32 "%1$s Unable to open input file '%2$s'.\n"
33 "\n%1$s Unable to open output file '%2$s'.\n"
34 "Initializing search engine...\n"
36 "Normal engine shutdown.\n"


$ ****************************************************
$ set 30 named MS_huff has messages for hencode.c, hdecode.c, huffcode.c
$set 30

$ Msg 1 variable 2 is <filename>, variable 3 is <system errno>.
1 "%1$s: Cannot open huffman encode file '%2$s': '%3$s' Exit Code = 2.\n"
2 "\n\
USAGE: %1$s <filename>\n"
3 "%1$s Could not open input file '%2$s.\n"
4 "%1$s Total Number of Characters = %2$ld.\n"

10 "%1$s Out of Memory.\n"
$ Msg 11 variable 2 is <tree file name>, variable 3 is <system errno>.
11 "%1$s Cannot open tree file '%2$s': %3$s.\n"
$ Msg 12 variable 2 is <tree file name>.  Do not translate "hctree."
12 "%1$s Invalid format hctree '%2$s'.\n"
$ Msg 13 should not translate "hctree_ids."
13 "%1$s Incompatible hctree_ids.\n"
14 "%1$s Cannot open cypherfile '%2$s'.\n"
$ Msg 15 should not translate "EOF."
15 "%1$s Unexpected EOF '%2$s'.\n"

30 "183 Bit strings have grown too large.  You probably have literals\n\
turned off with grossly unbalanced character counts.\n"
31 "191 Programming Error:  Still trying to build\n\
Huffman Code Tree after root created.\n"
32 "(nonascii character, high bit set)"
33 "366 Invalid file format for %1$s.\n"
34 "424 File '%1$s' failed to open for write.  Verify it is not read-only.\n"
35 "\n\
USAGE: huffcode [-l<N> | -l-] [-o] <huffname> [<infile>]\n\
  -l<N>      specifies the 'literal' threshold count.  Any character occurring\n\
             <= <N> times will be coded with the Huffman literal.  Default is -l0,\n\
             literal coding only for bytes with counts of zero.\n\
  -l-        turns off literal coding.  Turning off literal coding in unbalanced\n\
             trees leads to EXTREMELY LONG bit string codes--don't do it unless\n\
             the input is known to be a well balanced binary file.\n\
  -o         preauthorizes overwriting any currently existing decode file.\n\
  <huffname> is the filename prefix for the Huffman Code files.\n\
             If the encode file (%1$s) already exists, byte counts from infile will\n\
             be added to it, otherwise it will be newly created.\n\
             The decode file (%2$s) is always newly created each run.\n\
  <infile>   is an input file containing bytes to be counted.\n\
             It may be omitted if the encode file already exists.\n"
36 "'%1$s' is invalid argument.\n"
37 "576 Missing Huffman Code file names prefix.\n"
$ Msg 38 needs to have "y" translated to match msg 39"
38 "Decode file '%s' already exists.  Overwrite it? [y/n]\n"
$ Msg 39 needs "y" to match the "yes" option in msg 38.
39 "y"
40 "HUFFCODE Version %1$s\n"
41 "Huffman Code Tables will be newly created.\n"
42 "Table '%1$s' already contains %2$ld Kbytes from previous runs.\n"
43 "645 Input file not specified and '%s' table file\n\
does not exist -- nothing to do.\n"
44 "Could not open input file '%1$s' or access status: %2$s\n"
45 "Input file '%1$s' contains about %2$ld Kbytes.\n"
46 "\n%1$ld%% done. %2$2ld Kbytes read.  Estimate %3$3ld seconds to completion.\n"
$ Msg 47 variable 2 is a time stamp, variable 2 is from either msg 48 or 49,
$ variables3 and 4 are filenames.
47 "Identifying timestamp will be '%1$ld'.\n\
%2$s Huffman Code Tables in '%3$s' and '%4$s'...\n"
$ Msg 48 is used in msg 47 as variable 2.
48 "Creating"
$ Msg 49 is used in msg 47 as variable 2.
49 "Rebuilding"

$ ********************** DTSEARCH.MSG ************************
