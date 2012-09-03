$ $TOG: dtcreate.msg /main/4 1999/09/17 17:21:36 mgreess $
$ %Z%%M%        %I%  %W% %G% %U%
$ 
$   COMPONENT_NAME: dtcreate
$ 
$   FUNCTIONS: none
$ 
$   ORIGINS: 27
$ 
$ 
$   (C) COPYRIGHT International Business Machines Corp. 1993
$   All Rights Reserved
$   Licensed Materials - Property of IBM
$   US Government Users Restricted Rights - Use, duplication or
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ **   Description:
$ **   ------------
$ **   This is the source message catalog file for dtcreate.

$set 2
$ *****************************************************************************
$ **
$ ** Message set for menu options on main window
$ **
$ *****************************************************************************
10 File
11 F
15 Open...
16 O
20 New
21 N
25 Save
26 S
30 Exit
31 x
35 Options
36 O
40 Show Advanced Functions
41 A
42 Icon View
43 I
44 Color
46 C
47 Monochrome
48 M
49 Show Monochrome Icons
50 M
53 Help
54 H
60 Overview
61 v
65 Table of Contents
66 C
70 Tasks
71 T
75 Reference
76 R
80 Using Help
81 U
85 About Create Action
86 A

$set 3
$ *****************************************************************************
$ **
$ ** Message set for  main Create Action window
$ **
$ *****************************************************************************
10 Create Action
12 Action Name (Icon Label):
13 Action Icons:
20 Command When Action Icon Is Double-clicked:
21 Command To Execute When Double-clicked:
22 Command When Action Is Opened (Double-clicked):
30 Help Text For Action Icon:
32 Window Type:
34 Graphical (X-Window)
36 Terminal (Auto-Close)
38 Terminal (Manual Close)
40 No Output
50 Advanced
52 Basic

$set 4
$ *****************************************************************************
$ **
$ ** Message set for  main Create Action window advanced function area
$ **
$ *****************************************************************************
10 Optional
11 When Action Opens, Ask Users for:
12 Datatypes That Use This Action:
13 Add...
14 Delete
15 Edit...
16 Dropable Datatypes:
17 All Datatypes
18 Only Above List

$set 5
$ *****************************************************************************
$ **
$ ** Error and Information message set for  main Create Action window
$ **
$ *****************************************************************************
10 The Action Name is missing.\n\
Please enter a name in the 'Action Name' field.
$ 
15 The execute command is missing.\n\
Please enter a command in the\n\
'Command When Action Is Opened (Double-clicked)' field.
$ 
20 The 'Datatypes That Use This Action' list is empty.\n\
You have selected 'Only Above List' in the\n\
'Dropable Datatypes' field.\n\
Change the Dropable Datatype choice to 'All Datatypes'\n\
or add at least one datatype to the list.
$ 
25 An action and datatype definition file already exists\n\
with this Action Name.\n\
You can cancel the Save and change your Action Name\n\
or you can overwrite the existing definition file.\n\
Do you wish to overwrite the existing definition file?
$ 
30 A variable name is missing in the\n\
'Command When Action Is Opened (Double-clicked)' field.\n\
Add a variable name for the label you have entered\n\
in the 'When Action Opens, Ask Users for' field.\n\
Valid variable names are $*, $1, $2, ..., $9.
$ 
35 These changes have not been saved.\n\
Do you wish to save these changes before you Exit?
$ 
40 The current changes have not been saved.\n\
Do you want to save this action before clearing\n\
the fields for a New action?
$ 
45 The current changes have not been saved.\n\
Do you want to save these changes before clearing\n\
the fields to load another action definition file?
$ 
50 The Action Name contains one or more illegal\n\
characters.\n\
The following characters should not be used in a name:\n\
  / \ " ' ;  (  ) ~ ! $ { } < > # @ | &
$ 
55 Could not create the following file:
$ 
60 Could not open the following file:
$ 
65 Could not load the following file because it was\n\
not originally created by the Create Action tool:
$ 
66 Could not load the file because it has been altered by some\n\
other means other than Create Action.  This could have been caused\n\
by modifying the file with a text editor or some other tool.
$ 
67 Create Action can only edit files that were created\n\
and edited by Create Action.
$ 
68 Could not load the following file:
$ 
$  The following messages are all part of the same message dialog
70 The new action
71 has been placed in your home folder.
73 The action definition file that was created is called:
$ 
80 Could not find all of the sizes of the icon\n\
you have selected.
$ 
85 Could not start the Icon Editor.
$ 
90 Could not parse the following file:
$ 
95 Could not find the following icon file within the icon\n\
search path.
$ 
98 The icon you have selected is not in the icon search path.\n\
You must move the icon to a directory in the icon search\n\
path so that the icon will be displayed properly within desktop\n\
environment.\n\
Select 'Ok' if you want to use this icon.\n\
Select 'Cancel' if you want to select a different icon.
$ 
$ ONLY TRANSLATE  "Usage"  and  "filename"
110 Usage: dtcreate [<filename>] [-?]
$ 
120 You have not selected a Datatype from the\n\
'Datatypes That Use This Action' list.
125 Please select the Datatype you would like to Delete.
130 Please select the Datatype you would like to Edit.
$ 
150 Your current work has not been saved.
152 Don't Save
154 Cancel Close
$ 
160 Could not save session to file:
162 Could not restore session from file:
$ 
$ TRANSLATORS: DO NOT TRANSLATE the word "HOME" in the following message
170 The HOME environment variable is not set.\n\
The HOME environment variable must be set in order\n\
for this action to be saved.

$set 6
$ *****************************************************************************
$ **
$ ** General message set for dialogs
$ **
$ *****************************************************************************
10 Ok
11 Apply
12 Cancel
13 Help
14 Clear
18 Find Set
19 Find Set...
20 Edit Icon...
31 Create Action - Error
41 Create Action - Confirmation
46 Create Action - Information
51 Create Action - Question

$set 7
$ *****************************************************************************
$ **
$ ** Message set for Add Datatype and Edit Datatype dialogs
$ **
$ *****************************************************************************
10 Add Datatype
11 Edit Datatype
14 Name of Datatype Family:
15 Identifying Characteristics:
16 Edit...
17 Help Text for this Datatype Icon:
18 Datatype Icons:
21 Commands
22 Command to Open this Datatype:
23 Command to Print this Datatype:
30 Open
31 Print

$set 8
$ *****************************************************************************
$ **
$ ** Error and Information message set for
$ **    Add Datatype and Edit Datatype dialogs
$ **
$ *****************************************************************************
10 The Datatype Family Name is missing.\n\
Enter a name in the 'Name of Datatype Family' field.
$ 
15 The Identifying Characteristics are missing.\n\
You must specify the characteristics before the datatype\n\
can be created. Select the Edit button next to the\n\
Identifying Characteristics list to specify the characteristics.
$ 
20 The 'Name of Datatype Family' field contains one or more\n\
illegal characters.\n\
The following characters should not be used in a name:\n\
  / \ " ' ;  (  ) ~ ! $ { } < > # @ | &
$ 
25 A Datatype Family with this name already exists.\n\
Please change the name in the 'Name of Datatype Family'\n\
field.

$set 9
$ *****************************************************************************
$ **
$ ** Message set for  Identifying Characteristics dialog
$ **
$ *****************************************************************************
10 Identifying Characteristics
11 Include All
12 Files
13 Folders
14 That Match The Following:
20 Name Pattern:
21 Permission Pattern:
22 Readable
24 Writeable
25 Executable
27 AND
28 Contents:
29 Pattern:
30 Type:
31 String
32 Byte (Hex)
33 Short (Decimal)
34 Long (Decimal)
35 Start Byte:
40 Read:
41 Write:
42 Execute:
43 On
44 Off
45 Either

$set 10
$ *****************************************************************************
$ **
$ ** Error and Information message set for Identifying Characteristics dialog
$ **
$ *****************************************************************************
10 Name Pattern is missing.\n\
If Name Pattern is selected, a name pattern\n\
must be entered.
$ 
15 Permissions choice is missing.\n\
If Permission Pattern is selected, one or\n\
more of the choices under Permission\n\
Pattern must be selected.
$ 
20 Contents Pattern is missing.\n\
If Contents is selected, then the Pattern field\n\
under Contents must be filled in.
$ 
25 Start byte value is missing.\n\
If Contents is selected, then a number must be\n\
entered in the 'Start Byte' field.
$ 
30 An identifying characteristic has not been selected.\n\
Select one or more of the characteristics\n\
(Name Pattern, Permissions Pattern, or Contents).
$ 
45 The Name Pattern contains one or more illegal\n\
characters.\n\
The following characters should not be used in\n\
a Name Pattern:\n\
  / \ " ' ;  (  ) ~ ! $ { } < > # @ | &
$ 
46 The Contents Pattern does not match the format for\n\
the contents Type you have selected.\n\
Retype the Contents Pattern in the correct format or\n\
change the Type so that it matches the Contents Pattern.
$ 
47 The Start Byte field contains illegal characters.\n\
This field can only accept numbers in a decimal format.
$ 
48 Using the Contents Pattern may significantly slow\n\
down your File Manager performance.\n\
When Contents Pattern is used, the File Manager must\n\
search the contents of all files in a folder before\n\
it can open that folder. This means that folders\n\
will open more slowly.\n\
Therefore, it is recommended that you only use name and\n\
permissions patterns.

$set 11
$ *****************************************************************************
$ **
$ ** Message set for  Icon Selection Dialog
$ **
$ *****************************************************************************
10 Directories
11 Icon Files
12 Enter Icon Filename
20 Loading icons.  Please wait.
30 Icon Folders
50 The Icon Filename entered is invalid.\n\
Please enter a valid icon file name in the\n\
'Enter Icon Filename' field.
$ ONLY TRANSLATE  "Empty".   Do not translate the brackets "[" and "]".
60 [Empty]

$set 12
$ *****************************************************************************
$ **
$ ** Message set for  Open File Dialog
$ **
$ *****************************************************************************
10 Filter
11 Directories
12 Files
13 Selection
20 Open File
25 Create Action - Open

$set 13
$ *****************************************************************************
$ **
$ ** Message set for  DieFromToolTalkError Dialog
$ **
$ *****************************************************************************
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Create Action - Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
$ "-NEW"
3 OK
