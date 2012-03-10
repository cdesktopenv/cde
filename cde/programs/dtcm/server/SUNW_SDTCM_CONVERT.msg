$ $XConsortium: SUNW_SDTCM_CONVERT.msg /main/2 1995/09/06 08:25:07 lehors $
$
$quote "


$set	1
1	"usage: %s [-v output-version] [-s <mm/dd/yy>] [-c character-set] [-d backup-directory] calendar\n"
$ 	/*
$ 	 * NL_COMMENT
$ 	 * Attention Translator:
$ 	 * messages number 1 to 5 are to be print out all at the same time
$ 	 */
2	"\t-v specifies the output version, 3 or 4\n"
3	"\t-s specifies the start date to do the conversion\n"
4	"\t-c specifies the locale name to be used in the character set attribute\n"
5	"\t-d specifies the backup-directory\n"
6	"Invalid version number specified.\n"
7	"Invalid start date specified.\n"
8	"%s: insufficient memory\n"
9	"unknown failure"
10	"insufficient memory"
11	"invalid attribute"
12	"invalid attribute value"
13	"invalid date and time"
14	"invalid enumeration value"
15	"invalid flag"
16	"invalid recurrence rule"
17	"unsupported attribute"
18	"unsupported enumeration value"
19	"unsupported flag"
20	"Failed to convert appointment(s):\n"
21	"%d) id %d, what/summary = %s\n"
22	"error code = %d (%s)\n"
23	"Failed to lock lockfile, %s.\n"
24	"Failed to lock lockfile, %s.\n"
25	"Failed to kill rpc.cmsd, please kill it before\nrunning this program.\n"
26	"Failed to open %s\n"
27	"Failed to open %s\n"
28	"Failed to set %s to the correct mode.\n"
29	"Failed to backup calendar file to %s\n"
30	"There is not enough space on %s\n"
$ 		/*
$ 		 * NL_COMMENT
$ 		 * Attention Translator:
$ 		 * messages number (30, 31 and 32) or number (30, 31 and 34) or
$ 		 * number (30, 31 and 35) are to be printed out together as a
$ 		 * group depending on the error situation
$ 		 */
31	"Please free up some space before running this program.\n"
32	"The new calendar file may need as much as %d bytes of disk space.\n"
33	"Problem accessing %s: "
34	"The calendar file needs %d bytes of disk space to backup.\n"
35	"The new calendar file may need as much as %d bytes of disk space.\n"
36	"The calendar name (%s) is inconsistent with that in the calendar name\nattribute (%s)."
$ 	/*
$ 	 * NL_COMMENT
$ 	 * Attention Translator:
$ 	 * messages number (36, 37 and 38) or number (36 and 39) are
$ 	 * to be printed out together as a group depending on the situation
$ 	 */
37	"  There is already a calendar with the name %s\n"
38	"Do you want to correct the value of the calendar name attribute? (Y/N) [Y] "
39	"\nDo you want to correct it? (Y/N) [Y] "
40	"Please select a calendar name, 1) %s or 2) %s\n"
$ 		/*
$ 		 * NL_COMMENT
$ 		 * Attention Translator:
$ 		 * messages number 40 and 41 are to be printed out together
$ 		 */
41	"Name to use? (1, 2, or q to quit) "
42	"The ownership of the calendar file and/or the calendar owner attribute\nis wrong, the calendar should be owned by %s\n"
43	"Do you want to correct it? (Y/N) [Y] "
44	"Owner (%s) of the calendar file is inconsistent with that in the calendar\nowner attribute (%s).\n"
$ 		/*
$ 		 * NL_COMMENT
$ 		 * Attention Translator:
$ 		 * messages number (44 and 45) or (44 and 46) are to be
$ 		 * printed out together as a group depending on the
$ 		 * situation
$ 		 */
45	"Do you want to correct the value of the calendar owner attribute? (Y/N) [Y] "
46	"Do you want to correct it? (Y/N) [Y] "
47	"Please select an owner, 1) %s or 2) %s\n"
$ 			/*
$ 			 * NL_COMMENT
$ 			 * Attention Translator:
$ 			 * messages number 47 and 48 are to be printed out
$ 			 * together
$ 			 */
48	"Name to use? (1, 2, or q to quit) "
49	"Failed to get uid for %s\n"
50	"Problem getting the owner name for %s: \n"
51	"Problem getting the owner name for %s: \n"
52	"The ownership and/or the permission mode of the calendar file is wrong.\n"
$ 			/*
$ 			 * NL_COMMENT
$ 			 * Attention Translator:
$ 			 * messages number 52 and 53 are to be printed out
$ 			 * together
$ 			 */
53	"Do you want to correct it ? (Y/N) [Y] "
54	"Failed to do the correction.\n"
55	"Total number of appointments\t\t= %d\n"
56	"Number of one-time appointments\t\t= %d\n"
57	"Number of repeating appointments\t= %d\n"
58	"Please specify a calendar.\n"
$ 		/*
$ 		 * NL_COMMENT
$ 		 * Attention Translator:
$ 		 * If message number 58 is printed, it will be
$ 		 * followed by messages number 1 to 5.
$ 		 */
59	"You have specified a calendar in a remote host, %s\n"
$ 			/*
$ 			 * NL_COMMENT
$ 			 * Attention Translator:
$ 			 * Messages number 59 and 60 will be printed out
$ 			 * together.  If they are printed, they will be
$ 			 * followed by messages number 1 to 5.
$ 			 */
60	"The calendar must be located in the local host, %s\n"
61	"Problem accessing calendar file %s:\n"
62	"%s: Must be run by super-user or the owner of the calendar.\n"
63	"Loading the calendar ...\n"
64	"Failed to parse the calendar file %s\n"
65	"The calendar file is ok.\n"
66	"Correction done.\n"
67	"Root does not have permission to write to %s\n"
68	"The calendar server, rpc.cmsd, running on your machine does not support\nthe version 4 data format.  If you convert your calendar to the version 4\ndata format, you have to upgrade your calendar server to one that\nunderstands the version 4 data format.\n\n"
$ 			/*
$ 			 * NL_COMMENT
$ 			 * Attention Translator:
$ 			 * Messages 68 and 69 will be printed out together
$ 			 */
69	"Do you want to continue? (Y/N) [Y] "
70	"Could not get the platform independent locale name for %s\n"
$ 				/*
$ 				 * NL_COMMENT
$ 				 * Attention Translator:
$ 				 * Messages 70 and 71 will be printed out
$ 				 * together
$ 				 */
71	"Please specify a valid locale name.\n"
72	"\nWARNING!! Data will be lost when converting version 4 data format\nback to version 3 data format.\n\n"
$ 		/*
$ 		 * NL_COMMENT
$ 		 * Attention Translator:
$ 		 * Messages 72 and 73 will be printed out together
$ 		 */
73	"Do you want to continue? (Y/N) [Y] "
74	"Doing conversion ...\n"
75	"The original calendar file %s is not changed.\n"
76	"Failed to move the calendar file to %s\n"
77	"Writing out new file ...\n"
78	"Failed to write the new file.\n"
$ 		/*
$ 		 * NL_COMMENT
$ 		 * Attention Translator:
$ 		 * Messages number (78 and 79) or (78 and 80) or (78 and 81)
$ 		 * or (78 and 82) will be printed out together depending
$ 		 * on the error condition
$ 		 */
79	"You need to recover %s from %s\n"
80	"The original calendar file %s is not changed.\n"
81	"You need to recover %s from %s\n"
82	"The original calendar file %s is not changed.\n"
83	"Correction done.\n"
$ 		/*
$ 		 * NL_COMMENT
$ 		 * Attention Translator:
$ 		 * Messages number 83, may be 84, and 85 will be printed out
$ 		 * together
$ 		 */
84	"The new file is saved in %s\n"
85	"The original file is saved in %s\n"
86	"Conversion done successfully with exceptions noted above.\n"
$ 		/*
$ 		 * NL_COMMENT
$ 		 * Attention Translator:
$ 		 * Either message 86 or 87 will be printed out
$ 		 * together with messages number 88 to 94.
$ 		 */
87	"Conversion done successfully.\n"
88	"Total number of appointments\t\t\t= %d\n"
89	"Number of one-time appointments converted\t= %d\n"
90	"Number of repeating appointments converted\t= %d\n"
91	"Number of one-time appointments pruned\t\t= %d\n"
92	"Number of repeating appointments pruned\t\t= %d\n"
93	"The new file is saved in %s\n"
94	"The original file is saved in %s\n"
