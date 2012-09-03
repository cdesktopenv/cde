$ $TOG: dtcm.msg /main/9 1999/09/20 10:31:13 mgreess $

$quote "


$set	1
1	"dtcm: empty recipient in mail reminder\n"
2	"Reminder- %s"
3	"pm"
4	"am"
7	"\n\n\t** Calendar Appointment **\n\n\tDate:\t%s\n\tStart:\t%s\n\tEnd:\t%s\n\tWhat:\t%s"
16	"You may not remove the default calendar"
17	"Select a name to remove"
20	"Invalid Date In Go To Field."
21	"Edit List..."
22	"Browse Menu Items"
23	"Prev Week"
24	"This Week"
25	"Next Week"
26	"Prev Month"
27	"Next Month"
28	"Go To:"
29	"Schedule..."
30	"Mail..."
31	"Calendar Displayed"
32	"Calendars Displayed"
33	"Day"
34	"Week"
35	"Month"
36	"Year"
37	"Time Zone..."
38	"Appointment List..."
40	"View"
41	"Find..."
42	"Go to Date..."
65	"File"
68	"Print Current View"
69	"Print..."
72	"Options..."
73	"Exit"
74	"Appointment..."
76	"Edit"
77	"Help"
79	"Tasks..."
80	"Reference..."
82	"Using Help..."
84	"Show Other Calendar..."
85	"Compare Calendars..."
86	"Menu Editor..."
87	"Browse"
88	"Today"
89	"in quit_handler\n"
90	"Error opening Calendar file"
91	"rpc.cmsd is not responding for your user name.\nMake sure the inetd process is running and the entry\nin inetd.conf for rpc.cmsd is correct for your host."
92	"User name"
93	"Host"
94	"Calendar : Error"
95	"Continue"
108	"No Calendar Location specified for"
109	"No location specified; add a hostname to the\nInitial Calendar View in Properties/Display Settings.  If\nyou ran Calendar (dtcm) with the -c option, verify you specified a hostname."
110	"Calendar : Error - Help"
113	"Calendar"
$ 		/* NL_COMMENT
$ 
$ 		   Message 113 : ``Calendar'' is used in the main titlebar 
$ 		   of the tool: Calendar : user@host.
$ 		 */
114	"January"
115	"February"
116	"March"
117	"April"
118	"May"
119	"June"
120	"July"
121	"August"
122	"September"
123	"October"
124	"November"
125	"December"
126	"Jan"
127	"Feb"
128	"Mar"
129	"Apr"
130	"May"
131	"Jun"
132	"Jul"
133	"Aug"
134	"Sep"
135	"Oct"
136	"Nov"
137	"Dec"
138	"Sun"
$ 	/* NL_COMMENT
$ 	   Attention Translator:
$ 
$ 	   The strings (message number 138-144) are abbreviations to
$ 	   the days of the week:
$ 
$ 		Sun --> Sunday
$ 		Mon --> Monday
$ 		Tue --> Tuesday
$ 		Wed --> Wednesday
$ 		Thu --> Thursday
$ 		Fri --> Friday
$ 		Sat --> Saturday
$ 		Sun --> Sunday
$ 
$ 	*/
139	"Mon"
140	"Tue"
141	"Wed"
142	"Thu"
143	"Fri"
144	"Sat"
146	"Sunday"
147	"Monday"
148	"Tuesday"
149	"Wednesday"
150	"Thursday"
151	"Friday"
152	"Saturday"
154	"S"
$ 	/* NL_COMMENT
$ 	   Attention Translator:
$ 
$ 	   The strings (message number 154-160, 168) are one letter 
$ 	   abbreviations to the days of the week:
$ 
$ 		S --> Sunday (message 154)
$ 		M --> Monday
$ 		T --> Tuesday
$ 		W --> Wednesday
$ 		T --> Thursday
$ 		F --> Friday
$ 		S --> Saturday (message 160)
$ 
$ 	*/
155	"M"
156	"T"
157	"W"
158	"T"
159	"F"
160	"S"
170	"SUN"
$ 	/* NL_COMMENT
$ 	   Attention Translator:
$ 
$ 	   The strings (message number 170-176) are abbreviations to
$ 	   the days of the week:
$ 
$ 		SUN --> Sunday
$ 		MON --> Monday
$ 		TUE --> Tuesday
$ 		WED --> Wednesday
$ 		THU --> Thursday
$ 		FRI --> Friday
$ 		SAT --> Saturday
$ 
$ 	*/
171	"MON"
172	"TUE"
173	"WED"
174	"THU"
175	"FRI"
176	"SAT"
178	"You Are Already Browsing %s"
186	"Calendar (dtcm) and rpc.cmsd versions are different."
187	"Usage:\n\tdtcm_delete [-c calendar] [-d <mm/dd/yy>] [-v view]\n"
188	"\nCould not open calendar %s\n"
189	"UNKNOWN"
190	"Item to delete (number)? "
191	"Usage:\n\tdtcm_insert [ -c calendar ] [-d <mm/dd/yy>] [ -v view ]\n"
192	"                   [-w what string] [-s <HH:MMam/pm>] [-e <HH:MMam/pm>]\n"
193	"Please enter the information for the appointment you wish to add.\nDefaults will be shown in parentheses.\n"
194	"Calendar (%s): "
195	"Date (%s): "
196	"Start (%s): "
197	"You have entered an invalid time.  Please try again:\n"
198	"End (%s): "
199	"End (%s): "
200	"One Time"
201	"Repeat (%s): "
203	"no default"
204	"For (%s): "
205	"What (you may enter up to 5 lines, use ^D to finish):\n"
206	"\nCould not open calendar %s\n"
207	"Usage:\n\tdtcm_lookup [ -c calendar ][ -d <mm/dd/yy> ] [ -v view ]\n"
208	"\nCould not open calendar \"%s\"\n"
209	"UNKNOWN"
210	"The appointment will be scheduled in the calendar\nyou are currently browsing.  Do you still want to schedule it?"
211	"Schedule in"
212	"Calendar : Schedule Appointment"
214	"Appointment scheduled: %s"
218	"Invalid DATE specified"
219	"Invalid START time specified"
220	"Invalid END time specified"
221	"Empty or missing DATE field"
222	"Empty or missing START field"
223	"Empty or missing WHAT field"
224	"REPEAT and FOR field mismatch"
225	"Schedule appointment was cancelled."
226	"Internal error scheduling appointment."
230	"Calendar : Error - Drag Appointment"
231	"Select an appointment and DRAG again."
236	"CalendarAppointment"
237	"XCreateBitmapFromData() failed for bitmap.\n"
238	"XCreateBitmapFromData() failed for mask.\n"
239	"DragStart returned NULL.\n"
247	"This appointment has an end time earlier than\nits begin time.  Do you want to\nschedule it into the next day?"
248	"Calendar : Schedule Appointment"
250	"Next Day"
251	"Author"
252	"Calendar : Appointment Editor - Delete"
253	"Select an appointment and DELETE again."
256	"Internal error selecting appointment.\nAppointment was not deleted."
258	"Calendar : Appointment Editor - Change"
259	"Select an appointment and CHANGE again."
268	"This appointment is part of a repeating series.\nDo you want to change ...?"
270	"This One Only"
271	"Forward"
272	"All"
274	"This appointment is part of a repeating series.\nDo you want to delete ...?"
279	"Calendar : Appointment Editor"
280	"Calendar : Error - Appointment List"
281	"Sorry, no appointments to list.     "
283	"Calendar : Find"
284	"Find:"
285	"Find"
290	"Specify Appt String to Match."
291	"Appointment Not Found."
292	"%d matches found"
293	"Date:"
294	"Go To"
297	"Please type in a date"
298	"You are already viewing %s"
299	"Invalid Date"
300	"Author"
304	"Mail..."
305	"Calendar : Error - Group Appointment Editor"
306	"Select a calendar and INSERT again."
314	"Calendar : Group Appointment Editor - Delete"
315	"Select a calendar and DELETE again."
329	"Calendar : Group Appointment Editor - Change"
330	"Select a calendar and CHANGE again."
340	"Calendar"
341	"Access"
342	"Insert"
343	"Change"
348	"Insert Permission"
349	"Calendar : Group Appointment Editor"
350	"Selection Error, cannot continue\n"
352	"You must select a component within your application."
353	"No Time"
354	"All Day"
355	"Unable to open callog file.\nOperation was cancelled.  Calendar"
356	"Per user request, Operation was cancelled.\nCalendar"
357	"Invalid value in DATE field.\nOperation was cancelled.  Calendar"
358	"Invalid value in START field.\nOperation was cancelled.  Calendar"
359	"Invalid value in END field.\nOperation was cancelled.  Calendar"
360	"You must provide a DATE value.\nOperation was cancelled.  Calendar"
361	"You must provide a START value.\nOperation was cancelled.  Calendar"
362	"You must provide a WHAT value.\nOperation was cancelled.  Calendar"
363	"Invalid or mismatched REPEAT and FOR values.\nOperation was cancelled.  Calendar"
364	"Calendar : Error - Editor"
367	"Calendar already exists.\nOperation was cancelled.  Calendar"
368	"Calendar does not exist.\nOperation was cancelled.  Calendar"
370	"Authorization error.  Permission denied.\nOperation was cancelled.  Calendar"
378	"Memory allocation error - not enough memory.\nOperation was cancelled.  Calendar"
383	"Unknown error.\nOperation was cancelled.  Calendar"
386	"That appointment has an end time earlier than its begin time.\nDo you want to schedule it into the next day?"
389	"Next Day"
390	"Privacy:"
391	"User Calendar Location:"
392	"Day Boundaries:"
393	"Begin:"
394	"End:"
395	"Hour Display:"
396	"12 Hour"
397	"24 Hour"
398	"Default View:"
399	"Year"
400	"Month"
401	"Week"
402	"Day"
403	"Initial Calendar View:"
405	"Please select at least one permission.        "
408	"Please select a name first.                   "
410	"User Name:"
411	"Add Name"
412	"Permissions"
413	"Permissions"
414	"Remove Name"
415	"B = Browse I = Insert D = Delete"
416	"Permissions:"
417	"Browse"
419	"Delete"
420	"Destination:"
421	"Printer"
422	"File"
423	"Printer:"
424	"Directory:"
425	"Options:"
426	"File:"
427	"Width:"
428	"Height:"
429	"Position:"
430	"Inches from left"
431	"Inches from bottom"
432	"Units:"
433	"Copies:"
434	"Include:"
435	"Show Time and Text Appts"
436	"Show Time Only Appts"
437	"Show Nothing Appts"
438	"Date Ordering:"
439	"MM | DD | YY"
440	"DD | MM | YY"
441	"YY | MM | DD"
442	"Date Separator:"
443	"Blank"
445	"An error occurred writing to ~/.desksetdefaults.\nMake sure you have permission to write to this file."
448	"An error occurred reading from ~/.desksetdefaults.\nMake sure you have permission to read this file."
450	"Calendar : Options - Help"
451	"You have made unsaved changes.\nYou may save your changes, discard your changes, \nor return to your previous place in the dialog."
452	"Save"
454	"Calendar : Error - Options"
458	"Calendar : Options"
459	"Category:"
460	"Apply"
464	"ps_open_file(): need to specify dir or file\n"
465	"ps_open_file(): Invalid Directory %s\n"
466	"w+"
467	"ps_open_file(): unable to open file %s\n"
468	"Page"
469	"Day view by Calendar Manager"
470	"Morning"
471	"Afternoon"
473	"To Do List by Calendar Manager"
474	"Appt List by Calendar Manager"
475	"Page"
476	"of"
477	"Month view by Calendar Manager"
479	"Page"
480	"Sun"
481	"Mon"
482	"Tue"
483	"Wed"
484	"Thu"
485	"Fri"
486	"Sat"
487	"ps_init_printer(): cannot open postscript prolog file: %s\n"
489	"   Page"
490	"Week view by Calendar Manager"
491	"M"
492	"T"
493	"W"
494	"T"
495	"F"
496	"S"
497	"S"
505	"Sat"
506	"Sun"
507	"Year view by Calendar Manager"
523	"To Do List: Year of %d"
524	"Appt List: Year of %d"
525	"To Do List: %s"
526	"Appt List: %s"
527	"To Do List: Week of %s"
528	"Appt List: Week of %s"
529	"To Do List: %s"
530	"Appt List: %s"
531	"Every %d %s"
532	"Calendar : Repeat Every"
533	"Repeat Every:"
534	"Occurs:"
535	"For:"
536	"Privacy:"
537	"Calendar : Editor - Schedule Appointment"
538	"Would you like to schedule this appointment as the last\nweek of the month or the 4th week of the month?"
540	"Last Week"
541	"4th Week"
542	"Every %d %s"
543	"Every %d %s"
544	"Every %d %s"
559	"Browsing %s..."
560	"Calendar : Show Other Calendar"
562	"Show"
565	"Author"
566	"Calendar : Error - To Do Editor"
567	"Select a To Do and DELETE again."
570	"Internal error selecting To Do.\nTo Do was not deleted."
573	"This To Do is part of a repeating series.\nDo you want to delete ...?"
579	"This To Do is part of a repeating series.\nDo you want to change ...?"
585	"Select a To Do and CHANGE again."
590	"Due Date:"
591	"Calendar : To Do Editor"
596	"Monday %d"
597	"Tuesday %d"
598	"Wednesday %d"
599	"Thursday %d"
600	"Friday %d"
601	"Saturday %d"
602	"Sunday %d"
603	"Type a name to add in the User Name field"
604	"is already in the list"
607	"Internal error registering calendar name."
610	"Internal error retrieving calendar name."
612	"Overview..."
613	"About Calendar..."
619	"Please enter a calendar name in the format: <user>@<hostname>"
620	"Unknown calendar. Calendar name needed: <name>%s"
622	"Unknown calendar. Hostname needed: %s@<hostname>"
623	"Calendar does not display dates prior to January 1, 1970"
624	"Calendar does not display dates after December 31, 2037"
625	"Less"
626	"More"
627	"Drag Appt"
628	"Time  What"
631	"%d match found"
632	"Please select an appointment from the list to show"
633	"Calendar Selected"
634	"Calendars Selected"
641	"Server failed to get network address\nfor the specified location:  invalid hostname."
642	"rpc.cmsd daemon is not registered.\nPlease verify rpc.cmsd is installed correctly on your system."
643	"Couldn't access calendar. Calendar"
644	"Appointment Times:"
645	"Duration:"
646	"Minutes"
647	"Looking up %s..."
648	"SunDex"
649	"Calendar : Time Zone"
650	"My Time"
652	"Custom Time"
653	"GMT"
654	"Hours"
655	"OK"
659	"Time Zone:"
671	"Version"
672	"Revision"
674	"Copyright (c) 1993, 1994, 1995:"
675	"Hewlett-Packard Company"
676	"International Business Machines Corp."
677	"Novell, Inc."
678	"Sun Microsystems, Inc."
679	"More..."
680	"Close"
681	"Calendar Contributors"
682	"This application was brought to you by:"
684	"User Names may not have embedded blanks or tabs"
686	"Add Name"
687	"Remove Name"
688	"Browse Menu Items"
691	"Reset"
696	"Usage:\n\tdtcm_admin [ -d ] [ -a action ] [ -c calendar ][ -s <mm/dd/yy> ] [ -e <mm/dd/yy> ] [-f filename]\n"
697	"You have specified an unsupported administration operation \"%s\".\n"
698	"\nCould not open output file \"%s\"\n"
699	"\nCould not open input file \"%s\"\n"
700	"Discard"
703	"context"
704	"Year of %d"
705	"%s"
706	"Week of %s"
707	"%s"
708	"This appointment repeats in an unknown fashion.  All occurrences will be changed\nDo you still wish to change it?"
711	"This appointment repeats in an unknown fashion.  All occurrences will be deleted\nDo you still wish to delete it?"
713	"You must choose a begin date before the end date."
714	"There are no appointments in the list.  You must find one before showing an appointment."
715	"Calendar : Go to Date"
721	"Others See Time And Text"
722	"Others See Time Only"
723	"Others See Nothing"
724	"No Time appointments must have a WHAT value specified.\nOperation was cancelled.  Calendar"
725	"Calendar Entry does not exist.\nOperation was cancelled.  Calendar"
727	"Your request timed out\nPlease verify rpc.cmsd is installed correctly on your system."
728	"Calendar : Print"
729	"Printer Name:"
730	"Copies:"
731	"From:"
732	"To:"
733	" Print Command Options:"
734	"Print to file:"
735	"Print"
736	"Calendar : Error - Print"
738	"Access List:"
739	"Access Rights:"
740	"Add"
743	"Public:"
744	"Semiprivate:"
745	"Private:"
749	"Name and Access Rights must be set to add an item to the Access List."
750	"Calendar : Error - Access List and Permissions"
751	"This entry already exists in the Access List.  Do you wish to change it?"
756	"Name and Access rights must be set to change an item in the Access List"
758	"midnight"
759	"value"
760	"value"
762	"Editor Defaults"
763	"Display Settings"
764	"Access List and Permissions"
765	"Printer Settings"
766	"Date Format"
772	"You may not save a blank User Calendar Location\nor one that contains blanks.\nYour properties settings have not been saved."
773	"You may not save a blank Initial Calendar View\nor one that contains blanks.\nYour properties settings have not been saved."
774	"ps_open_file(): print to file requires file name\n"
775	"You must specify a reminder advance\nOperation was cancelled.  Calendar"
776	"A reminder advance may not contain an embedded blank\nOperation was cancelled.  Calendar"
777	"Advance values must be a number with an optional sign\nOperation was cancelled.  Calendar"
778	"Calendar : To Do List - Help"
783	"Pending"
784	"Completed"
789	"To Do Type:"
790	"context"
791	"This To Do repeats in an unknown fashion.  All occurrences will be changed\nDo you still wish to delete it?"
797	"To Do"
798	"Time Due:"
802	"Delete"
803	"Clear"
806	"Year of %d"
807	"%s"
808	"Week of %s"
809	"%s"
810	"Please enter a start date after 1/1/1970"
811	"Malformed start date"
812	"Please enter an end date before 1/1/2038"
813	"Malformed end date"
814	"You must enter a date after 1969 and before 2038"
816	"Days"
817	"Weeks"
818	"Months"
819	"The Duration value must be an integer with no sign.\nYour properties settings have not been saved."
820	"The start value must be a valid time.\nYour properties settings have not been saved."
$ 821	"You have specified an illegal print width value.\nYour properties settings have not been saved."
$ "print width" has been replaced by "right margin".
821	"You have specified an illegal right margin value.\nYour properties settings have not been saved."
$ 822	"You have specified an illegal print height value.\nYour properties settings have not been saved."
$ "print height" has been replaced by "bottom margin".
822	"You have specified an illegal bottom margin value.\nYour properties settings have not been saved."
825	"Days"
826	"Weeks"
827	"Months"
828	"Date:"
829	"Start:"
830	"AM"
831	"PM"
832	"End:"
833	"AM"
834	"PM"
835	"What:"
836	"Frequency"
837	"Calendar : Error - To Do Editor"
839	"Calendar : Reminder"
841	"Click on \"Add Name\" to add a name, \"Apply\" to commit changes."
$ 		/* NL_COMMENT
$ 		   Attention Translator:
$ 
$ 		   Message 841 is used in the Menu Editor's footer.  The
$ 		   Menu Editor is accessed through the browse menu.  The 
$ 		   message is displayed when something is typed into the User
$ 		   Name field in the editor.  If the translated footer message 
$ 		   is too long it causes the menu editor to grow horizontally
$ 		   which we do not want.  If you notice that the translated
$ 		   string causes the editor to grow horizontally please
$ 		   insert a newline (\n) character somewhere in the middle
$ 		   of the string so the footer will use two lines and the
$ 		   editor will grow vertically.
$ 		*/
842	"The information transferred did not\ncontain any appointments."
843	"Drag and Drop operation failed."
844	"Unable to locate the transferred information."
845	"Drag and Drop operation Failed\nInternal consistency error."
846	"Save"
848	"Date"
849	"Time"
850	"What"
851	"Show Appointment"
852	"One Time"
853	"Daily"
854	"Weekly"
855	"Every Two Weeks"
856	"Monthly By Date"
857	"Monthly By Weekday"
858	"Yearly"
859	"Monday Thru Friday"
860	"Mon, Wed, Fri"
861	"Tuesday, Thursday"
862	"Repeat Every..."
863	"2"
864	"3"
865	"4"
866	"5"
867	"6"
868	"7"
869	"8"
870	"9"
871	"10"
872	"11"
873	"12"
874	"13"
875	"14"
876	"forever"
877	"Mins"
878	"Hrs"
879	"Days"
881	"Internal Error #1: Invalid calendar/entry handle.\nOperation was cancelled.  Calendar"
882	"Internal Error #2: Invalid session handle.\nOperation was cancelled.  Calendar"
883	"Internal Error #3: Invalid data value.\nOperation was cancelled.  Calendar"
884	"Internal Error #4: Value specified for a read-only attribute.\nOperation was cancelled.  Calendar"
885	"Internal Error #5: Incorrect data type specified for an attribute value.\nOperation was cancelled.  Calendar"
886	"Internal Error #6: Specified attribute is not supported on this calendar version.\nOperation was cancelled.  Calendar"
887	"Internal Error #7: Specified function is not supported on this calendar version.\nOperation was cancelled.  Calendar"
888	"Internal Error #8: Invalid operator specified.\nOperation was cancelled.  Calendar"
890	"Calendar : Error - Services"
892	"The dates for printing must be between 1969 and 2038."
894	"Malformed \"From\" date"
896	"Malformed \"To\" date"
898	"The \"To\" date for printing must be after the \"From\" date for printing"
899	"To Do List Beginning: %s"
900	"Appt List Beginning: %s"
901	"Repeat Every ..."
902	"Monday Thru Friday"
903	"Mon, Wed, Fri"
906	"Drag To Do"
907	"No space left for calendar file.\nOperation was cancelled.  Calendar"
908	"You may not set a blank reminder value.\nYour properties settings have not been saved."
909	"You may not set a reminder value with an embedded blank.\nYour properties settings have not been saved."
910	"Reminder values must be a number with an optional sign.\nYour properties settings have not been saved."
$ 911	"You have specified an illegal position from left offset value.\nYour properties settings have not been saved."
$ "position from left offset" has been changed to "left margin".
911	"You have specified an illegal left margin value.\nYour properties settings have not been saved."
$ 912	"You have specified an illegal position from right offset value.\nYour properties settings have not been saved."
$ "position from right offset" has been changed to "top margin".
912	"You have specified an illegal top margin value.\nYour properties settings have not been saved."
913	"A To Do item must have text in the first line of the What item."
914	"Calendar : Error - Editor"
916	"The Duration value must be less than 24 hours.\nYour properties settings have not been saved."
917	"Missing\nGraphics"
918	"Missing\nGraphics"
919	"Calendar"
920	"displayed."
921	"Couldn't access calendar: "
922	"About Calendar"
923	"Cancel"
963	"Calendar : Menu Editor"
964	"On Item"
965	"To Do List..."
966	"To Do..."
967	"Calendar Server Version"
968	"Calendar Data Version"
969	"Calendar : Error - Repeat Every"
970	"The Repeat Every value must be an integer with no sign."
971	"Time Calendar"
972	"What"
973	"Search all"
974	"Search from"
975	"to"
976	"Report Type:"
977	"Day View"
978	"Week View"
979	"Month View"
980	"Year View"
981	"Appointment List"
982	"To Do List"
983	"User Name"
984	"This appointment is part of a repeating series.\nDo you want to insert appointment ...?"
985	"Reminders"
986	"Beep"
987	"Flash"
988	"Popup"
989	"Mail"
990	"Mail To:"
992	"%A, %B %e, %Y"
$ 	/* NL_COMMENT
$ 	   Attention Translator:
$ 
$ 	   This string is used in the calendar day view.  In the C locale
$ 	   it has the form:
$ 
$ 		Monday, January 16, 1995
$ 
$ 	   strftime conversion string: "%A, %B %e, %Y" is used.  
$ 
$ 	   Use the appropriate strftime conversion for your locale.
$ 	*/
993	"Week Starting %A, %B %e, %Y"
$ 	/* NL_COMMENT
$ 	   Attention Translator:
$ 
$ 	   This string is used in the calendar week view.  In the C locale
$ 	   it has the form: 
$ 
$ 			Monday, January 16, 1995
$ 
$ 	   strftime conversion string: "%A, %B %e, %Y" is used.  The string
$ 	   will be used in a label that looks like this:
$ 
$ 			Week Starting Monday, January 16, 1995
$ 
$ 	   Use the appropriate strftime conversion for your locale.
$ 	*/
994	"days"
995	"weeks"
996	"biweeks"
997	"months"
998	"years"
1002	"Also contributing:"
1003	"This appointment is in a calendar owned by someone else.\nDo you want to change it anyway ?"
1004	"This appointment is in a calendar owned by someone else.\nDo you want to delete it anyway ?"
1005	"Delete from"
1006	"Change in"
1007	"Internal error selecting appointment.\nAppointment was not changed."
1008	"Calendar Appointment : Help"
1009	"Internal error selecting To Do.\nTo Do was not changed."
1010	"Calendar : Compare Calendars"
1011	"Invalid value in the \"Time Due\" field.\nOperation was cancelled.  Calendar"
1012	"Calendar : To Do List"
1042	"Insert Access Denied: "
1043	"Delete Access Denied: "
1044	"Lookup Access Denied: "
1045	"Cancelled for"
1046	"Appointment Cancelled\n"
1047	"This appointment: '%s' has an end\n"
$ 	/* NL_COMMENT
$ 	   
$ 	   The following four messages (1047-1050) will be printed to stdout 
$ 	   and can have the following two forms:
$ 
$ 		"This appointment: '<appt text>' has an end
$ 		 time earlier than its begin time.  Do you
$ 		 want to schedule it into the next day? [Y/N]  "
$ 
$ 	   or 
$ 
$ 		"This appointment has an end
$ 		 time earlier than its begin time.  Do you
$ 		 want to schedule it into the next day? [Y/N]  "
$ 
$ 	   The text <appt text> and [Y/N] should not be translated.
$ 
$ 	*/
1048	"This appointment has an end\n"
1049	"time earlier than its begin time.  Do you\n"
1050	"want to schedule it into the next day? [Y/N]  "
1051	"The appointment '%s' is part of a repeating series.  "
$ 		/* NL_COMMENT
$ 
$ 		   Message numbers 1051-1057 are printed to stdout and
$ 		   should appear like:
$ 
$ 		   "The appointment '<appt text>' is part of a repeating series. Do you want to:
$ 			1.  Delete all of them
$ 			2.  Delete this on only
$ 			3.  Delete forward
$ 			4.  Cancel
$ 			Option [1-4]: "
$ 
$ 		*/
1052	"Do you want to:"
1053	"\n\t1.  Delete all of them"
1054	"\n\t2.  Delete this one only"
1055	"\n\t3.  Delete forward"
1056	"\n\t4.  Cancel"
1057	"\n\tOption [1-4]: "
1058	"Invalid Date specified.\n"
1059	"Invalid Start time specified.\n"
1060	"Invalid Stop time specified.\n"
1061	"Empty or missing Date field.\n"
1062	"Empty or missing Start field.\n"
1063	"Empty or missing What field.\n"
1064	"Repeat and For field mismatch.\n"
1065	"Insert appointment was cancelled\n"
1066	"Appointment was not inserted."
1067	"Invalid date specified:"
1068	"Appointments for"
1069	"No Appointments for"
1070	"Calendar : Error - Compare Calendars"
1072	"Calendar : Error - Browse"
1073	"Calendar : Error - Drag and Drop"
1074	"Calendar Appointment"
1075	"Calendar : Error - Change Appointment"
1076	"Calendar : Help"
1078	"Calendar : Access List and Permissions - Add"
1079	"Calendar : Error - Editor Settings"
1083	"Calendar : Error - Display Settings"
1085	"Calendar : Error - Printer Settings"
1086	"Calendar : Appointment List"
1087	"From %1$2d:%2$02d%3$s to %4$2d:%5$02d%6$s"
$ 				/* NL_COMMENT
$ 
$ 				   Message 1087 : This message is used to form
$ 				   the alarm string that appears in the
$ 				   reminder popup.  In the C locale it would
$ 				   look something like this:
$ 
$ 				   ``From 11:00am to 1:00pm''
$ 
$ 				   In the printf conversion string the $n are:
$ 
$ 					$1 	-> start hour
$ 					$2	-> start minute
$ 					$3	-> am or pm
$ 					$4	-> stop hour
$ 					$5	-> stop minute
$ 					$6	-> am or pm
$ 
$ 				*/
1088	"%1$2d:%2$02d%3$s"
$ 				/* NL_COMMENT
$ 
$ 				   Message 1088 : This message is used to form
$ 				   the alarm string that appears in the
$ 				   reminder popup.  It is used when an appt
$ 				   does not have and ``end'' time.  In the 
$ 				   C locale it would look something like this:
$ 
$ 				   ``11:00am''
$ 
$ 				   In the printf conversion string the $n are:
$ 
$ 					$1 	-> start hour
$ 					$2	-> start minute
$ 					$3	-> am or pm
$ 
$ 				*/
1089	"From %1$02d%2$02d to %3$02d%4$02d"
$ 				/* NL_COMMENT
$ 
$ 				   Message 1089 : This message is used to form
$ 				   the alarm string that appears in the
$ 				   reminder popup.  This string is used when
$ 				   a user has asked that times be displayed
$ 				   in 24 hour format.  In the C locale it 
$ 				   would look something like this:
$ 
$ 				   ``From 0100 to 1600''
$ 
$ 				   In the printf conversion string the $n are:
$ 
$ 					$1 	-> start hour
$ 					$2	-> start minute
$ 					$3	-> stop hour
$ 					$4	-> stop minute
$ 
$ 				*/
1090	"%1$02d%2$02d"
$ 				/* NL_COMMENT
$ 
$ 				   Message 1090 : This message is used to form
$ 				   the alarm string that appears in the
$ 				   reminder popup.  This string is used when
$ 				   an appt does not have an end time and the
$ 				   user has asked that times be displayed
$ 				   in 24 hour format.  In the C locale it 
$ 				   would look something like this:
$ 
$ 				   ``1600''
$ 
$ 				   In the printf conversion string the $n are:
$ 
$ 					$1 	-> start hour
$ 					$2	-> start minute
$ 
$ 				*/
1091	"%B, %Y"
$ 	/* NL_COMMENT
$ 	   Attention Translator:
$ 
$ 	   This string is used in the calendar month view.  In the C locale
$ 	   it has the form:
$ 
$ 		July, 1995
$ 
$ 	   strftime conversion string: "%B, %Y" is used.
$ 
$ 	   Use the appropriate strftime conversion for your locale.
$ 	*/
1092	"%Y"
$ 	/* NL_COMMENT
$ 	   Attention Translator:
$ 
$ 	   This string is used in the calendar year view.  In the C locale
$ 	   it has the form:
$ 
$ 		1995
$ 
$ 	   strftime conversion string: "%Y" is used.
$ 
$ 	   Use the appropriate strftime conversion for your locale.
$ 	*/
$ 	/* NL_COMMENT
$ 	The following message were added because message numbers from
$    	IBM and Sunsoft conflicted - they both used the same message number
$    	for different messages.  The IBM message number was changed as
$    	follows:
$ 
$		1100 was 978 from IBM
$		1101 was 979 from IBM. 
$		1102 was 981 from IBM. 
$		1103 was 982 from IBM. 
$		1104 was 985 from IBM. 
$		1105 was 986 from IBM. 
$ 	*/
1100	"\n\n\t** Calendar To Do Item **\n\n\tDue Date:\t%s\n\tTime Due:\t%s\n\tWhat:\t\t%s"
1101	"Insert Appointment"
1102	"Insert To Do"
1103	"This To Do is part of a repeating series.\nDo you want to insert the item ...?"
1104	"Invalid value in TIME DUE field.\nOperation was cancelled.  Calendar"
1105	"You must provide a TIME DUE value.\nOperation was cancelled.  Calendar"
$ Message numbers 1106-1110 represent choices for the content of
$ left and right headers and footers on the printed page.  These
$ items are displayed in option menus in the "Printer Settings..."
$ category of the "Options..." dialog.
1106	"Date"
1107	"User Id"
1108	"Page Number"
1109	"Report Type"
1110	"None"
$ Message 1111 is placed in the title bar of an error dialog if an
$ error is encountered when the user invokes "Setup..." from the
$ "Print..." dialog box.  Message 1112 is the text displayed in
$ that error dialog.
1111	"Calendar : Error - Print Setup"
1112	"Print Dialog Manager (PDM) error - setup failed."
$ Message 1113 is the error message displayed if the user selects
$ "Print" from the "Print..." dialog box while an invalid printer
$ name is specified.
1113	"Invalid Printer or Print Server specified."
$ Message 1114 is placed in the title bar of an error dialog if an
$ error is encountered when the user invokes "Print" from the
$ "Print..." dialog box with the "Print to File" toggle selected.
$ Message 1115 is the text displayed in that error dialog.
1114	"Calendar : Error - Print To File"
1115	"Error - unable to print to file."
$ Message 1116 is placed in the title bar of an information dialog
$ if the operation completes successfully when the user invokes "Print"
$ from the "Print..." dialog box with the "Print to File" toggle
$ selected.  Message 1117 is the text displayed in that information dialog.
1116	"Calendar : Print To File"
1117	"Print to file has completed."
$ Message 1118 is placed in the title bar of a warning dialog if a
$ font cannot be found during the "Print" operation.  Message 1119
$ is the text displayed in that warning dialog; the %s will be replaced
$ by the name of the font.
1118	"Calendar : Warning - Print"
1119	"Warning - Unable to load font %s."
$ 
$ Message 1120 appears in an error dialog when an appointment cannot
$   be scheduled.
1120	"The event cannot be scheduled in the given time frame.\nThe operation was cancelled."
$ 
$ Message 1121 is placed in the title bar of a warning dialog if a
$ print job fails because the Print Server returns BadAlloc.  Message
$ 1122 is the text displayed in that warning dialog.
1121	"Calendar : Print Server Error"
1122	"Print job failed.\n\nThe X Print Server is temporarily out of resources."


$set 2
$ *****************************************************************************
$ **
$ ** Message set for  DieFromToolTalkError Dialog
$ **
$ *****************************************************************************
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Calendar - Warning
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\n"
$ "-NEW"
3 OK
$ "-NEW"
4 "Calendar : Appointment Editor - Warning"
