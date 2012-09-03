
$ "$TOG: dtfile.msg /main/10 1999/09/16 09:42:03 mgreess $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtfile.msg
$ **
$ **  Project:     Common Desktop Environment dtfile
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtfile
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **  ORIGINS: 27,118,119,120,121
$ **
$ **  This module contains IBM CONFIDENTIAL code. -- (IBM
$ **  Confidential Restricted when combined with the aggregated
$ **  modules for this product)
$ **  OBJECT CODE ONLY SOURCE MATERIALS
$ **
$ **  (C) COPYRIGHT International Business Machines Corp. 1995
$ **  All Rights Reserved
$ **  US Government Users Restricted Rights - Use, duplication or
$ **  disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ **
$ *****************************************************************************
$ **
$ **  (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994, 1995 Novell, Inc.
$ **
$ *****************************************************************************
$ *****************************************************************************
$ *************************************<+>*************************************

$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There may be three types of messages in this file:
$ 
$ 1. Messages that appear in dialogs or are displayed to the user.
$ 
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$ 
$ 2. Messages that only appear in the DT error log file ($HOME?.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$     MESSAGES xx-yy WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$     DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ********** PROLOGUE ****************
$  Date    Name               Note
$ ======== =================  ===============================================
$ 03/25/93 H. Amro            removed the guilty $ from message 30
$ 
$ 
$ 
$ *****************************************************************************

$set 2
$ ****** Module: ChangeDir.c ****(developer comment only)********

$ Messages 3 and 10 are titles for the Change Folder dialog, Message 3 is
$   used when the user has specified a title for dtfile windows
$ Messages 2 and 4 are labels for the Change Folder dialog
$ Message 1 is the title for Change Folder errors
$ Messages 7, 8, 9, 11, and 12 are Change Folder error messages

1 Change Folder Error
2 New Folder:
3 Change Folder
4 Host:
7 The following folder name is invalid.\n\n%s:%s
8 You cannot switch to this folder.  You are\n\
allowed to view only folders beneath your\n\
current folder.  You cannot specify an absolute\n\
path to the new folder.
9 This view is in restricted mode.  You cannot go\n\
to the specified folder because it is not in the\n\
relative path of the restricted folder.\n
10 File Manager - Change Folder
11 Cannot get the new folder name.
12 Please enter folder name.

13 Go To Error
14 Destination Folder:
15 Go To
16 System Name:
17 File Manager - Go To
18 Destination Folder name is missing.\n\
Type in a folder name or select a folder from the list.
19 The following folder name is invalid.\n\n%s
20 You cannot switch to folder:\n\n%s\n\n\
You are not allowed to view  this folder\n\
because it is a restricted folder.

$set 3
$ ****** Module: ChangeDirP.c ****(developer comment only)********

$ Messages 2-6 are labels for the folder Status Line
$ Message 3 is required for SUN systems
$ Message 2-9 are obsolete

2 %d Files %d Hidden
3 %1$d Files %2$d Hidden
4 %3d Files ...
5 Reading ...
6 %d Hidden

7 %d Items %d Hidden
8 %1$d Items %2$d Hidden
9 %3d Items ...



10 %d Item(s)
11 %d Item(s)...
12 %d Item(s) %d Hidden



$set 4
$set 5
$set 6
$set 7
$set 8



$set 9
$ ****** Module: File.c ****(developer comment only)********

$ Messages 6 and 7 are the title and text for an action error message
$ Messages 8 and 9 are rename error messages
$ Messages 11-19 are the title and text for warning messages displayed
$   when an object/set of objects is moved/copied/linked to a folder
$   containing object(s) with the same name

6 Action Error
7 There are no actions defined for %s\n

8 This file is represented on the Workspace and a file\n\
already exists on the Workspace with this new name.\n\
Please choose a different name.
9 A file with this name already exists.\n\
Please choose a different name.

11 File Manager - Move/Copy/Link Warning
$ Messages 12-15 deal with multiple files/folders
12 The following files (or folders)\n\
already exist in\n\
   %s:
13 \nThe Copy operation will overwrite these files.\n\
          Do you wish to proceed?\n
14 \nThe Move operation will overwrite these files.\n\
          Do you wish to proceed?\n
15 \nThe Link operation will overwrite these files.\n\
          Do you wish to proceed?\n

$ Messages 16&17 deal with a single file/folder
16 An object named\n%s\nalready exists inside the folder\n%s
17 A folder named\n%s\nalready exists inside the folder\n%s

$ Labels in rename/replace and merge/replace dialogs
20 What do you want to do?
21 Replace existing object
22 Rename existing object to:
24 Merge contents of the two folders
25 Replace existing folder

27 Ok
28 Cancel
29 Help
30 Cancel Move
31 Cancel Copy
32 Cancel Link

$ Window title for overwrite dialogs
33 File Manager - Move/Copy/Link Warning
34 File Manager - Move Warning
35 File Manager - Copy Warning
36 File Manager - Link Warning

37 Object Rename - Error
38 %s\nalready exists: choose another name.
39 %s\ncannot be renamed to\n%s\n%s
40 Object Replace - Error
$ 
$ 41 Got obsoleted - Don't use.
$ 
51 copying
52 moving
53 linking
54 changing
56 Replace existing objects
57 Rename existing objects by appending a number
58 copied
59 moved
60 linked
61 changed
68 1 object %s.
69 %d objects %s.
70 %s\n%d objects have the same name as objects in the folder\n%s
71 %s\n1 object has the same name as an object in the folder\n%s
72 Copy
73 Move
74 Link
75 Operation
76 copy
77 move
78 link
79 operation
80 %s Canceled - the folder was not %s.\n\nThe folder you want to replace could not be deleted\nbecause an object inside that folder could not be deleted.\nSome other objects inside that folder may have already been\ndeleted before the %s process was halted.
81 %s\n\nThe most probable cause of this problem is that you do not\nhave Write permission for the object that could not be removed.\nChange Write permission or manually rename the folder you\nare %s so it no longer has the same name.
82 %s Canceled\n\n%s\ncannot be removed.\n\nThe most probable cause of this problem is that\nyou do not have Write permission for the file.\nChange Write permission or manually rename the\nfile you are %s so it no longer has the same name.
83 %s Canceled - None of the objects were %s.\n\n The following object could not be automatically renamed\n%s\n\n%d objects were renamed before the %s process was halted.
84 %s\n\nThe most probable cause of this problem is that you do not have Write\npermission for the above object. Change Write permission or manually\nrename the object you are %s so it no longer has the same name.
85 %s Canceled - None of the objects were %s.\n\n The following object could not be automatically renamed\n%s\n\n1 object was renamed before the %s process was halted.
86 %s Canceled - none of the objects were %s.\n\nA folder that you want to replace could not be deleted.\n   Folder: %s\n\nSome objects inside this folder may have been deleted\nbefore the %s process was halted.\n\n%d other objects have been deleted.\n\nThe most probable cause of this problem is that you do not have Write\npermission for the above folder. Change Write permission or manually\nrename the folder you are %s so it no longer has the same name.
87 %s Canceled - none of the objects were %s.\n\nA folder that you want to replace could not be deleted.\n   Folder: %s\n\nSome objects inside this folder may have been deleted\nbefore the %s process was halted.\n\n1 other object has been deleted.\n\nThe most probable cause of this problem is that you do not have Write\npermission for the above folder. Change Write permission or manually\nrename the folder you are %s so it no longer has the same name.
88 %s Canceled - none of the objects were %s.\n\nA file that you want to replace could not be deleted.\n    File: %s\n\n%d other objects have been deleted.\n\nThe most probable cause of this problem is that you do not have Write\npermission for the above file. Change Write permission or manually\nrename the file you are %s so it no longer has the same name.
89 %s Canceled - none of the objects were %s.\n\nA file that you want to replace could not be deleted.\n    File: %s\n\n1 other object has been deleted.\n\nThe most probable cause of this problem is that you do not have Write\npermission for the above file. Change Write permission or manually\nrename the file you are %s so it no longer has the same name.
90 Name Conflict.\n\
  This object is out on the Workspace back drop.\n\
  Another object on the back drop already has the name you\n\
  are trying to enter.\n\
  Please choose a different name.

91 Stop Copy
92 Stop Link
93 Stop Move
94 Error

95 Move Canceled - the folder was not moved.\n\n\
The folder you want to replace could not be deleted\n\
because an object inside that folder could not be deleted.\n\
Some other objects inside that folder may have already been\n\
deleted before the move process was halted.\n\n\
The most probable cause of this problem is that you do not\n\
have Write permission for the object that could not be removed.\n\
Change Write permission or manually rename the folder you\n\
are moving so it no longer has the same name.

96 Copy Canceled - the folder was not copied.\n\n\
The folder you want to replace could not be deleted\n\
because an object inside that folder could not be deleted.\n\
Some other objects inside that folder may have already been\n\
deleted before the copy process was halted.\n\n\
The most probable cause of this problem is that you do not\n\
have Write permission for the object that could not be removed.\n\
Change Write permission or manually rename the folder you\n\
are copying so it no longer has the same name.

97 Link Canceled - the folder was not linked.\n\n\
The folder you want to replace could not be deleted\n\
because an object inside that folder could not be deleted.\n\
Some other objects inside that folder may have already been\n\
deleted before the link process was halted.\n\n\
The most probable cause of this problem is that you do not\n\
have Write permission for the object that could not be removed.\n\
Change Write permission or manually rename the folder you\n\
are linking so it no longer has the same name.

98 Operation Canceled - the folder was not changed.\n\n\
The folder you want to replace could not be deleted\n\
because an object inside that folder could not be deleted.\n\
Some other objects inside that folder may have already been\n\
deleted before the process was halted.\n\n\
The most probable cause of this problem is that you do not\n\
have Write permission for the object that could not be removed.\n\
Change Write permission or manually rename the folder you\n\
are changing so it no longer has the same name.

99 Move Canceled\n\n%s\ncannot be removed.\n\n\
The most probable cause of this problem is that\n\
you do not have Write permission for the file.\n\
Change Write permission or manually rename the\n\
file you are moving so it no longer has the same name.

100 Copy Canceled\n\n%s\ncannot be removed.\n\n\
The most probable cause of this problem is that\n\
you do not have Write permission for the file.\n\
Change Write permission or manually rename the\n\
file you are copying so it no longer has the same name.

101 Link Canceled\n\n%s\ncannot be removed.\n\n\
The most probable cause of this problem is that\n\
you do not have Write permission for the file.\n\
Change Write permission or manually rename the\n\
file you are linking so it no longer has the same name.

102 Operation Canceled\n\n%s\ncannot be removed.\n\n\
The most probable cause of this problem is that\n\
you do not have Write permission for the file.\n\
Change Write permission or manually rename the\n\
file you are changing so it no longer has the same name.

103 Move Canceled - None of the objects were moved.\n\n\
The following object could not be automatically renamed\n%s\n\n\
%d objects were renamed before the move process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are moving so it no longer has the same name.

104 Copy Canceled - None of the objects were copied.\n\n\
The following object could not be automatically renamed\n%s\n\n\
%d objects were renamed before the copy process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are copying so it no longer has the same name.

105 Link Canceled - None of the objects were linked.\n\n\
The following object could not be automatically renamed\n%s\n\n\
%d objects were renamed before the link process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are linking so it no longer has the same name.

106 Operation Canceled - None of the objects were changed.\n\n\
The following object could not be automatically renamed\n%s\n\n\
%d objects were renamed before the process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are changing so it no longer has the same name.

107 Move Canceled - None of the objects were moved.\n\n\
The following object could not be automatically renamed\n%s\n\n\
1 object was renamed before the move process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are moving so it no longer has the same name.

108 Copy Canceled - None of the objects were copied.\n\n\
The following object could not be automatically renamed\n%s\n\n\
1 object was renamed before the copy process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are copying so it no longer has the same name.

109 Link Canceled - None of the objects were linked.\n\n\
The following object could not be automatically renamed\n%s\n\n\
1 object was renamed before the link process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are linking so it no longer has the same name.

110 Operation Canceled - None of the objects were changed.\n\n\
The following object could not be automatically renamed\n%s\n\n\
1 object was renamed before the process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are changing so it no longer has the same name.

111 Move Canceled - none of the objects were moved.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\nSome objects inside this folder may have been deleted\n\
before the move process was halted.\n\n%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are moving so it no longer has the same name.

112 Copy Canceled - none of the objects were copied.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\nSome objects inside this folder may have been deleted\n\
before the copy process was halted.\n\n\
%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are copying so it no longer has the same name.

113 Link Canceled - none of the objects were linked.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\nSome objects inside this folder may have been deleted\n\
before the link process was halted.\n\n%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are linking so it no longer has the same name.

114 Operation Canceled - none of the objects were changed.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\nSome objects inside this folder may have been deleted\n\
before the process was halted.\n\n%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are changing so it no longer has the same name.

115 Move Canceled - none of the objects were moved.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\nSome objects inside this folder may have been deleted\n\
before the move process was halted.\n\n1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are moving so it no longer has the same name.

116 Copy Canceled - none of the objects were copied.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\nSome objects inside this folder may have been deleted\n\
before the copy process was halted.\n\n1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are copying so it no longer has the same name.

117 Link Canceled - none of the objects were linked.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\nSome objects inside this folder may have been deleted\n\
before the link process was halted.\n\n1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are linking so it no longer has the same name.

118 Operation Canceled - none of the objects were changed.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\nSome objects inside this folder may have been deleted\n\
before the process was halted.\n\n1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are changing so it no longer has the same name.

119 Move Canceled - none of the objects were moved.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are moving so it no longer has the same name.

120 Copy Canceled - none of the objects were copied.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are copying so it no longer has the same name.

121 Link Canceled - none of the objects were linked.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are linking so it no longer has the same name.

122 Operation Canceled - none of the objects were changed.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are changing so it no longer has the same name.

123 Move Canceled - none of the objects were moved.\n\n\
A file that you want to replace could not be deleted.\n\ File: %s\n\n1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are moving so it no longer has the same name.

124 Copy Canceled - none of the objects were copied.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are copying so it no longer has the same name.

125 Link Canceled - none of the objects were linked.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are linking so it no longer has the same name.

126 Operation Canceled - none of the objects were changed.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are changing so it no longer has the same name.

127 1 object moved.
128 %d objects moved.
129 1 object copied.
130 %d objects copied.
131 1 object linked.
132 %d objects linked.
133 1 object changed.
134 %d objects changed.

135 Cannot move object %s onto itself.
136 Cannot copy object %s onto itself.
137 Cannot link object %s onto itself.
138 Failed to rename the following object(s)\n\n%s\n\n\
The most likely cause is that you do not have\n\
permission to rename these object(s).
139 Failed to replace the following object(s)\n\n%s\n\n\
The most likely cause is that you do not have\n\
permission to replace these object(s).
140 Rename new object to:
141 Rename new objects by appending a number

$set 10
$ ****** Module: FileDialog.c ****(developer comment only)********

$ Messages 1, 2, 20, and 21 are titles for error dialogs
$ Message 3 is an error message for dialogs requiring new file names
$ Messages 5 and 27 are titles for the Copy File dialog, Message 5 is used
$   when the user has specified a title for dtfile windows
$ Messages 6-9 are labels for the File Copy, New Dir, and New File dialogs
$ Messages 10-13 and 25 are error messages for the file copy operation
$ Messages 14 and 28 are titles for the New Folder dialog, Message 14 is
$   used when the user has specified a title for dtfile windows
$ Message 16 is a label for the New Folder dialog
$ Messages 22-24 are error messages for the New File/Folder operations
$ Messages 26 and 29 are titles for the New File dialog, Message 26 is used
$   when the user has specified a title for dtfile windows
$ Messages 30 and 31 are titles for the Move File dialog, Message 30 is used
$   when the user has specified a title for dtfile windows
$ Messages 32 and 33 are titles for the Link File dialog, Message 32 is used
$   when the user has specified a title for dtfile windows

1 File Rename Error
2 File Copy Error

3 Spaces are not allowed in a file or folder name.

5 Copy File

6 Current File Name:
7 New File Name:
8 File Type
9 Show File Type

10 The originally selected file no longer exists.
11 A file or folder cannot have a blank name.
13 Invalid file or folder name.

14 New Folder

16 New Folder Name:

20 Create Folder Error
21 Create File Error

22 Cannot create a file or folder with a blank name.
23 Invalid folder name.
24 Invalid file name.

25 You cannot use the copy command to copy a\n\
file to the Workspace.  You must use the\n\
`Put In Workspace` command in the File menu, or\n\
drop the file on the Workspace.

26 New File

27 File Manager - Copy File
28 File Manager - New Folder
29 File Manager - New File

30 Move File
31 File Manager - Move File

32 Link File
33 File Manager - Link File
34 Destination Folder:
35 Object Name:
36 Show Icon

37 Selected object:
38 Name for copy:

39 Rename error
40 This object is an Action.\n\
Action icon labels cannot be directly renamed.
41 The correct icon for this datatype is already shown.

42 Move Object
43 File Manager - Move Object

44 Copy Object
45 File Manager - Copy Object

46 Link Object
47 File Manager - Link Object

48 New Object Name:


$set 11
$ ****** Module: FileManip.c ****(developer comment only)********

$ Message 6 is the title for an error dialog
$ Messages 9, 14, 16, 28, and 30-36 are file manipulation error messages

6 File Manipulation Error

9 You can't drop files from\n\
the Trash Can on to the Workspace.

14 Cannot find the folders location.
16 A folder cannot be moved into itself.\n%s
28 %s cannot be found.
$ 
$ 29 Got obsoleted - Don't use.
$ 
30 Cannot move or rename the folder %s.\n\
All File Manager views displayed for a folder or its sub-folders\n\
must be closed before a folder can be moved or renamed.
31 Error while reading %s
32 Cannot rename %s
33 Cannot open %s
34 You do not have permission to move %s\n\
However, you can copy the file (use <CTRL>\n\
while you drag the file).
35 Cannot copy a folder into itself.
36 Cannot create child process.

37 You can't drop files from\n\
the Trash Can on to the Workspace.\n\
To remove an object from Trash,\n\
   -Select the object, and\n\
   -Choose 'Put Back'\n\
from the File menu or the object's popup menu.\n\
You can then drag the object out of File Manager and drop it on\n\
the Workspace backdrop.

38 You do not have permission to move %s\n\
However, you can copy the object.\n\
To copy an object:\n\
  - press and hold the <Ctrl> key, and\n\
  - drag the object with your mouse.\n\
Or\n\
  - use 'Copy To' in the 'Selected' menu popup of the menu bar.

39 Cannot create child process.\n\
The maximum number of processes for this system has been reached.\n\
Stop some of the processes or programs that are currently\n\
running and then retry this function.

40 Source and destination objects are the same.\n%s
41 The folder\n%s\ndoes not exist.
42 No space available on the\n\
device you are copying to.
43 No space available on the\n\
device you are moving to.
44 No Move Operation performed on object %s.\n\
You cannot move an object to the folder it is\n\
already in. You must enter a different Destination\n\
Folder name before the object can be moved.
45 No Copy Operation performed on object %s.\n\
You must change either the Destination Folder\n\
or the Name for Copy before a copy can be created.
46 No Link Operation performed on object %s.\n\
You must change either the Destination Folder\n\
or the Name for Copy before a link can be created.
47 Cannot move a folder into itself.
48 Cannot copy the following objects to folder \"%s\"\n\n%s\n\n\
The most likely cause is that you do not have\n\
write permission for the target folder.
49 Cannot move the following objects to folder \"%s\"\n\n%s\n\n\
The most likely cause is that you do not have\n\
write permission for the target folder.
50 Cannot move the following objects to folder \"%s\"\n\n%s\n\n\
The most likely cause is that you do not have\n\
permission to move source object.

$ 
$ Messages 51 - 56 are new since CDE/SI.
$ 
51 Unable to copy the file/folder because\n\
the disk quota will be exceeded on the disk\n\
you are copying it to.
52 Unable to move the file/folder because\n\
the disk quota will be exceeded on the disk\n\
you are moving it to.
53 The copy of the file/folder failed\n\
due to some internal error. The internal\n\
error given is:
54 The move of the file/folder failed\n\
due to some internal error. The internal\n\
error given is:
55 Please see your System Adminstrator
56 (Unknown):
57 You do not have permission to move the folder\n\
%s\n\
Write permission is required.
58 Process Create Error
59 Cannot create a child process to delete the dropped files.

$ 
$set 12
$ ****** Module: FileMgr.c ****(developer comment only)********

$ Message 3 is a dialog title
$ Messages 4-6 are movement warning or error messages
$ Message 7 is the title for the folder popup menus

3 Move File(s)

4 The object %s is currently in folder %s.\n\
You are moving the object to folder %s.\n\
Is this what you want to do?
5 The object %1$s is currently in folder %2$s.\n\
You are moving the object to folder %3$s.\n\
Is this what you want to do?
6 At least one of the files you dropped is from a different\n\
folder.  You are moving all these files to %s.\n\
Is this what you want to do?

7 File Manager

18 The folder %s is empty.
19 The folder %s contains no subdirectories.


$set 13
$ ****** Module: Filter.c ****(developer comment only)********

$ Messages 11 and 21-22 are button labels for the Modify Filter List dialog
$ Messages 19 and 24 are titles for the Link File dialog, Message 19 is used
$   when the user has specified a title for dtfile windows
$ Message 23 is a text field label for the Modify Filter List dialog
$ Message 25 is a label for the Modify Filter List dialog

11 Defaults

19 Set Filter Options

21 Select All
22 Deselect All

23 Filter String (Optional):

24 File Manager - Set Filter Options

25 Select Filetypes to be Hidden:

26 Also hide (Optional):

27 Shown

28 Hidden

29 Also show (Optional):

30 Select Datatypes to be Hidden:

31 Select Datatypes to be:




$set 14



$set 15
$ ****** Module: Find.c ****(developer comment only)********

$ Message 1 is the title for an error dialog
$ Messages 14 and 15 are button labels for the Find Files dialog
$ Messages 23, 26, and 27 are Find Files error messages
$ Messages 29 and 41 are titles for the Find Files dialog, Message 29 is used
$   when the user has specified a title for dtfile windows
$ Messages 30-40 and 42 are labels for the Find Files dialog

1 Find File Error

14 Start
15 Stop
$ 
$ 18 Got obsoleted - Don't use.
$ 
$ 
$ 19 Got obsoleted - Don't use.
$ 
23 Invalid Find Information.\n\n\
A search folder must be specified.
26 Invalid Find Information.\n\n\
The selected file no longer exists.
27 No files were found that match the search data.

29 Find Files or Folders

30 Fill in one or more fields to specify which files to find:
31 File or Folder Name:
32 File Contents:

$ Messages 33-35 are used on hpux systems
33 On
34 Off
35 Follow Links:

36 Open New View
37 Put In Workspace
38 Files Found (by Contents):
39 Files Found (by Name and Contents):
40 Files Found (by Name):

41 File Manager - Find Files or Folders

42 Search Folder:

43 Find Error

44 Search Folder name argument is missing.\n\
Type in the name of the folder where you want the search to begin.

45 The selected file no longer exists.\n\n\
Someone deleted the file after the search process completed.

46 Find

47 Fill in one or more fields to specify which items to find:

48 Open Folder

49 File Manager - Find

50 Search Folder name or File Content argument is missing\n\
Type in the name of the folder where you want the search to begin.\n\
Or type in the string that you want to search.


$set 16
$set 17



$set 18
$ ****** Module: Main.c ****(developer comment only)********

$ Message 1 is a blank line
$ Messages 2, 6, 17-18 and 27-28 are error messages which will ONLY APPEAR
$   in the DT ERRORLOG FILE
$ Messages 16 and 32 are titles for the Save Settings dialog, Message 16 is
$   used when the user has specified a title for dtfile windows
$ Messages 19-21 are error messages for invalid directory input.
$ Message 22 is the title for an error dialog
$ Messages 25 and 26 are usage strings
$ Messages 29 and 30 are text for the Save Settings dialog
$ Messages 34 and 35 are drag/drop error messages

$ DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGE 1
1 

$ MESSAGE 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ above no longer true - it will be displayed - see CDExc21626
2 Could not create the ~/.dt folder or sub folders.

$ MESSAGE 6 WILL ONLY APPEAR IN THE DT ERRORLOG FILE, and stderr
6 An X Toolkit error occurred... Exiting.\n

16 Save As Default Options

$ MESSSAGES 17-18 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
17 Unable to create a file to store the default options.
18 Could not open the session file.

19 The folder specification,\n\
%s\n\
is not a folder.
20 The folder specification,\n\
%s\n\
does not exist.
21 Invalid folder specification, %s:%s.

22 Drag Error

23 \nUsage: %s...\n\n\
   -noview\n\n\
\tDtfile runs in server mode.\n\n\
   -session SessionFile\n\n\
\tDtfile runs with the session file specified in the SessionFile\n\
\tparameter.\n\n\
   -folder Folder[,Folder,Folder]\n\
   -dir Folder[,Folder,Folder]\n\n\
\tDtfile displays a window for each folder specified in the\n\
\tFolder parameter.  The Folder parameter may contain many\n\
\tfolders separated by commas.  Folders may be in the form\n\
\t'path'.\n\n\
   -title Title\n\n\
\tDtfile uses the string specified in the Title parameter as the\n\
\ttitle for its windows.\n\n\
   -help_volume HelpVolume\n\n\
\tDtfile uses the help volume specified in the HelpVolume parameter.\n\n\
   -restricted\n\n\
\tDtfile will not display folders above the restricted folder.\n\
\tIf the -dir option is used, the folder specified in that option\n\
\tis the restricted folder.  If the -dir option is not used, the\n\
\tuser's current folder is the restricted folder.\n\n\
   -grid on/off\n\n\
\ton     = Files are displayed in a grid pattern.\n\
\toff    = Files are displayed as placed.\n\n\
   -tree on/off\n\n\
\ton     = Files are displayed in single folder mode.\n

24 \toff    = Files are displayed in folder tree mode.\n\n\
   -tree_files /never/always/choose\n\n\
\tnever  = Tree mode has two states: partially expanded or collapsed.\n\
\talways = Tree mode has two states: fully expanded or collapsed.\n\
\tchoose = Tree mode has three states: partially expanded, fully\n\
\texpanded, or collapsed.\n\n\
   -order alphabetical/file_type/date/size\n\n\
\tFiles are displayed in the specified order: alphabetical, by file\n\
\ttype, by date, or by size.\n\n\
   -view no_icon/large_icon/small_icon/attributes\n\n\
\tFiles are displayed in the specified format: text only, text and\n\
\tlarge icons, text and small icons, with attributes.\n\n\
   -direction ascending/descending\n\n\
\tFiles are displayed in the specified direction: ascending or\n\
\tdescending.\n\n\
   -large_icon_width <size>\n\n\
   -large_icon_height <size>\n\n\
   -small_icon_width  <size>\n\n\
   -small_icon_height <size>\n\n\
       The display area size for the icon images in File Manager\n\
       Icon images larger than this size will be clipped to this size\n\
       The default display area size for large is 38 and small is 24\n\n
26 \nRestricted Mode Usage: %s...\n\n\
   -folder Folder[,Folder,Folder]\n\
   -dir Folder[,Folder,Folder]\n\n\
\tWhere Folder is a folder below and/or including\n\
\tthe user's Home folder.\n\n

$ MESSSAGES 27-28 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
27 Unable to recover the saved default home settings file, will use default.\n
28 Unable to recover the saved default tool settings file, will use default.\n

$ Messages 29-30 are the strings displayed in a question dialog
29 Save the current File Manager View window size,\n\
preferences, and filter settings?\n\n\
They will be used for any new File Manager View windows\n\
started from the Front Panel.
30 Save the current Restricted File Manager View\n\
window size, preferences, and filter settings?\n\n\
They will be used for any new Restricted File Manager\n\
View windows started from the Front Panel.

32 File Manager - Save As Default Options

34 You can't copy or link a file or\n\
folder to the Trash Can.

$ 35 You can't copy or link a file or\n\
$ folder out of the Trash Can.

35 Save the current Application Manager View\n\
window size, preferences, and filter settings?\n\n\
They will be used for any new Application Manager\n\
windows started from the Front Panel.

36 You can't copy or link a file or folder out of the Trash Can.\n\
Move the object out of the Trash and put it into the File Manager.\n\
You can then copy or link it from there.

37 You can't copy or link a file or folder out of the Trash Can.\n\
Move the object out of the Trash and put it into the File Manager.\n\
You can then copy or link it from there.
38 Invalid folder specification, %s

$ MESSSAGE 39 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
39 Cannot start a second File Manager session because a File Manager\n\
session is already open on the desktop.  If you want to start a new File\n\
Manager session, you must first close all current File Manager windows\n\
and remove all File Manager objects from the workspaces.

$ 
$ "-NEW"
40 "Could not connect to ToolTalk:\n%s\nExiting ..."


$set 19



$set 20
$ ****** Module: Menu.c ****(developer comment only)********
$ 
$ IMPORTANT
$ The mnemonic has to stay the same for Japanese and Chinese locale as it's in English
$ In Japanese and Chinese locale, the text for the menu bar and menu item
$   must have mnemonic character in it.
$ 

$ Messages 1-2 are the text and mnemonic for the menu bar File option
1 File
2 F
$ Messages 3-4 are obsolete
3 Folder
4 D
$ Messages 5-6 are the text and mnemonic for the menu bar View option
$ Message 6 is also the mnemonic for the View.SetViewOptions menu option
5 View
6 V
$ Messages 7-8 are the text and mnemonic for the menu bar Selected option
7 Selected
8 S
$ Message 9 is the mnemonic for the menu bar Help option
9 H
$ Messages 10-16 are obsolete
10 New...
11 N
16 Properties...
$ Message 17 is the mnemonic for the Selected.ChangePermmisions menu option
$ Message 17 is also the mnemonic for the popup ChangePermmisions menu option
17 P
$ Messages 18-19 are the text and mnemonic for the File.Find menu option
$ Messages 18-19 are also the text and mnemonic for the popup Find menu option
18 Find...
19 F
$ Messages 22-23 are obsolete
22 Show Trash...
23 S
$ Messages 30-33 are the text and mnemonic for the View.Update menu option
30 Update
33 U
$ Messages 34-37 are obsolete
34 Change To...
37 C
$ Messages 38-41 are the text and mnemonic for the File.GoHome menu option
38 Go Home
41 H
$ Messages 42-47 are obsolete
42 Up
46 New
47 N
$ Messages 48-51 are the text and mnemonic for the Selected.SelectAll menu option
$ Messages 48-51 are also the text and mnemonic for the popup SelectAll menu
$ option
48 Select All
51 S
$ Messages 52-55 are the text and mnemonic for the Selected.DeselectAll menu
$ option
52 Deselect All
55 D
$ Messages 62-63 are obsolete
62 Save All Properties ...
63 t
$ Messages 84-85 are the text and mnemonic for the Selected.PutinWorkspace menu
$ option
$ Messages 84-85 are also the text and mnemonic for the popup PutinWorkspace menu
$ option
84 Put in Workspace
85 W
$ Message 90 is obsolete
90 Delete To Trash
$ Message 91 is the mnemonic for the Selected.PutinTrash menu option
$ Message 91 is also the mnemonic for the object PutinTrash menu option
91 T
$ Messages 92-93 are obsolete
92 Fast Change To
93 F
$ Message 94 is the mnemonic for the File.GoUp menu option
94 U
$ Messages 97-98 are obsolete
97 Terminal
98 T
$ Messages 99-100 are the text and mnemonic for the View.CleanUp menu option
$ Messages 99-100 are also the text and mnemonic for the popup CleanUp menu option
99 Clean Up
100 C
$ Message 101 is obsolete
101 Show Hidden Files
$ Message 102 is the mnemonic for the View.ShowHiddenObjects menu option
$ Message 102 is also the mnemonic for the popup ShowHiddenObjects menu option
$ Message 102 is also the mnemonic for the popup Help menu option
102 H
$ Message 103 is obsolete
103 Modify Filter List...
$ Message 104 is the mnemonic for the View.SetFilterOptions menu option
104 F
$ Messages 105-106 are the text and mnemonic for the Help.Overview menu option
105 Overview
106 v
$ Messages 107-108 are the text and mnemonic for the Help.Tasks menu option
107 Tasks
108 T
$ Messages 109-110 are the text and mnemonic for the Help.Reference menu option
$ Message 110 is also the mnemonic for the Selected.Rename menu option
109 Reference
110 R
$ Messages 111-112 are the text and mnemonic for the Help.OnItem menu option
$ Message 112 is also the mnemonic for the File.OpenTrash menu option
111 On Item
112 O
$ Messages 113-114 are the text and mnemonic for the Help.UsingHelp menu option
113 Using Help
114 U
$ Messages 115-116, and 157 are the texts and mnemonic for the Help.About menu
$ option; Message 115 is used for File Manager help, Message 157 is used for
$ App Manager help
115 About File Manager
116 A
$ Messages 117-118 are the text and mnemonic for the File.Close menu option
117 Close
118 C
$ Messages 119-122 are obsolete
119 Set View Properties...
120 P
121 Copy...
122 y
$ Message 123 is the text for the menu bar Help option
$ Message 123 is also the text for the popup Help option
123 Help
$ Message 124 is obsolete
124 Move...
$ Message 125 is the mnemonic for the Selected.Moveto menu option
125 M
$ Message 126 is obsolete
126 Link...
$ Message 127 is the mnemonic for the Selected.CopyAsLink menu option
127 L
$ Messages 128-129 are the text and mnemonic for the workspace popup Rename menu
$ option
128 Rename
129 n
$ Messages 130-131 are the text and mnemonic for the File.NewFolder menu option
130 New Folder...
131 N
$ Messages 132-133 are the text and mnemonic for the File.NewFile menu option
132 New File...
133 w
$ Messages 134-135 are the text and mnemonic for the File.GoTo menu option
134 Go To...
135 G
$ Message 136 is the text for the File.OpenTrash menu option
136 Open Trash
$ Message 137 is the text for the Selected.Rename menu option
137 Rename...
$ Messages 138-139 are the text and mnemonic for the View.OpenNewView menu option
138 Open New View
139 O
$ Message 140 is obsolete
140 Copy As Link...
$ Message 141 is the text for the View.SetViewOptions menu option
141 Set View Options...
$ Messages 142-143 are the text and mnemonic for the View.SaveAsDefaultOptions
$ menu option
142 Save As Default Options...
143 S
$ Message 144 is the mnemonic for the View.SetFilterOptions menu option
144 Set Filter Options...
$ Message 145 is the mnemonic for the Selected.Copyto menu option
145 C
$ Messages 146-147 are the text and mnemonic for the File.OpenTerminal menu option
146 Open Terminal
147 O
$ Messages 148-149 are the text and mnemonic for the Help.TableOfContents menu option
148 Table Of Contents
149 C
$ Message 150 is the text for the Selected.ChangePermissions menu option
$ Message 150 is also the text for the popup ChangePermissions menu option
150 Change Permissions...
$ Message 151 is the text for the Selected.PutinTrash menu option
$ Message 151 is the also text for the popup PutinTrash menu option
151 Put in Trash
$ Message 152 is the text for the Selected.Moveto menu option
152 Move to...
$ Message 153 is the text for the Selected.Copyto menu option
153 Copy to...
$ Message 154 is the text for the Selected.CopyAsLink menu option
154 Copy As Link...
$ Message 155 is the text for the File.GoUp menu option
155 Go Up
$ Message 156 is the text for the View.ShowHiddenObjects menu option
$ Message 156 is also the text for the popup ShowHiddenObjects menu option
156 Show Hidden Objects
157 About Application Manager



$set 21
$ ****** Module: ModAttr.c ****(developer comment only)********

$ Messages 1 and 21 are titles for the File Properties dialog, Message 1 is
$   used when the user has specified a title for dtfile windows
$ Messages 2-13 and 18 are labels for the File Properties dialog
$ Message 14 is the output of the file modification date; DO NOT LOCALIZE
$ Messages 15 and 16 are file properties error messages
$ Message 17 is the title for an error dialog
$ Message 19 is the default file owner/group, if real one can't be determined
$ Messages 22-28 are error messages that can be generated when dtfile
$   tries to use the dtfile.config configuration file, this file is
$   used to customize the File Properties dialog

1 File Permissions

2 File Name:
3 Owner Name:
4 Group Name:
5 Permissions:
6 Read
7 Write
8 Execute
9 Owner:
10 Group:
11 Other:
12 Size (in bytes):
13 Modified:

$ DO NOT LOCALIZE MESSAGE 14
14 %x %X

15 Invalid File Attribute Information.\n\n\
The specified user does not exist.
16 Invalid File Attribute Information.\n\n\
The specified group does not exist.

17 Properties Error

18 Is Linked To:

19 Unknown

21 File Manager - File Permissions

22 Cannot open file manager configuration file:
24 Unknown field label in file manager configuration file:
25 No information found in file manager configuration file for file-system identifier
26 Unable to create a process for file system dialog
27 Error executing file system dialog
28 Attempting to execute:

$ do not localize "fsDialog"
29 No value was provided for the fsDialog field in dtfile's configuration file

30 Name:

31 Owner Name is invalid.\n\
The specified user does not exist.

32 Group Name is invalid.\n\n\
No group of users with this name has been created on your system.\n\
Use your system administration software if you want to create\n\
a user group with this name.

33 Permissions Error

34 File Manager - Permissions

35 The Permissions dialog for the object\n\n%s/%s\n\n\
has been closed because this object has been deleted\n\
from your system.

36 Change Permissions Warning
37 You are changing the permissions on the Current\n\
Folder in a way that won't allow you to view\n\
this folder any longer.  It will be automatically\n\
closed and you will no longer be able to reopen\n\
the folder.\n\n\
Is this what you want to do?
38 File Manager Error
39 File Open Error

$set 22
$ ****** Module: ModAttrP.c ****(developer comment only)********

$ Messages 1 and 3 are file properties error messages
$ Message 2 is the title for the properties error dialog

1 Cannot open file, operation aborted
2 Properties Error
3 Cannot change properties for %s
4 Cannot open object.
5 Set Permissions Error



$set 23
$ ****** Module: Prefs.c ****(developer comment only)********

$ Messages 1 and 22 are titles for the Set Preferences dialog, Message 1 is
$   used when the user has specified a title for dtfile windows
$ Messages 2-20 and 23-29 are labels for the Set Preferences dialog

1 Set View Options

2 View
3 By Name
4 By Name and Icon
5 By Name and Small Icon
6 By Name, date, size ...
7 Show
8 By Single Folder
9 By Tree
10 Order
11 By File Type
12 Alphabetically
13 By Date
14 By Size
15 Direction
16 Ascending
17 Descending
18 Placement
19 Sorted Grid
20 As Placed

22 File Manager - Set View Options

23 Headers
24 Iconic Path
25 Current Folder
26 Status Line
27 Folders only
28 Folders, then Files
29 Folders and Files
30 Defaults

31 Representation
32 By Name Only
33 By Large Icons
34 By Small Icons
35 Rows and Columns
36 Text Path
37 Message Line


$set 24
$set 25
$set 26



$set 27
$ ****** Module: Trash.c ****(developer comment only)********

$ Messages 3, 4, and 81 are titles for error warning dialogs
$ Message 5-8, 56, 58, 78, 82, and 83 are trash error messages
$ Message 9 is the title of the trash container
$ Messages 24/26 and 28/30 are the text and mnemonics for the File pulldown
$   menu
$ Message 73 is an error dialog title
$ Messages 74, 75, 79, and 80 are trash warning messages

3 Trash Can Error
4 Trash Can Warning

5    The following files or folders could not be placed in the trash can:     \n
6    Each of the following is either a non-empty folder,    \n   \
or a file for which you do not have write permission.    \n   \
Do you want to proceed?\n
7    You do not have write permission for the following files.    \n   \
Do you want to proceed?\n
8    The following folders are not empty.    \n   \
Do you want to proceed?\n

9 Trash Can

$ Messages 24-26 are the text and mnemonic for the trash popup Putback option
24 Put back
26 P
$ Messages 28-30 are the text and mnemonic for the trash popup Shred option
28 Shred
30 h

56    The following files could not be put back:\n
$ Message 58 supplements Message 56
58 (Plus %d additional files)

73 Shred File(s)

74 The following file is also on the Workspace:\n\
   %s\n\
Moving the file to the trash will remove it from\n\
the Workspace.
75 The following files are also on the Workspace:\n\
   %s\n\
Moving the files to the trash will remove them from\n\
the Workspace.

78 The File Manager does not support Files or\n\
Folders with spaces in their names,\n\
so we cannot delete them.\n

79 The Workspace file %s is also in your file system at:\n\
   %s\n\
Moving the file to the trash will remove it from\n\
the file system.

80 The Workspace objects are also in your file system at:\n\
   %s\n\
Moving the files to the trash will remove them from\n\
the file system.

81 Shred File/Folder Error

82 The following file could not be removed from the file system:   \n

83 The .trashinfo file in $HOME/.dt/Trash can not be opened.\n\
You need access to this for Trash to work.\n\
Check the permissions on that file and its folder.\n
84 When trash objects are shredded, they are permanently\n\
removed from your system.\n\n\
Are you sure you want to shred?
85 The Workspace object you are deleting is a linked copy of \n\
an object inside the File Manager at\n\
   %s\n\
If you click OK, both the linked copy and the original object\n\
will be deleted.\n\
If you just want to delete the Workspace object,\n\
      1) click Cancel on this dialog\n\
      2) then select 'Remove From Workspace'\n\
         from the Workspace object's popup menu.
86 The Workspace objects you are deleting are linked copies of\n\
objects inside the File Manager at\n\
   %s\n\
If you click OK, both the linked copies and the original objects\n\
will be deleted.\n\
If you just want to delete the Workspace objects,\n\
      1) click Cancel on this dialog\n\
      2) then select 'Remove From Workspace'\n\
         from the Workspace objects' popup menu.
87 Object in the Trash cannot be opened.\n\n\
To open an object use 'Put Back' to return it to the\n\
File Manager then open it there.

88 Cannot write to a temporary file.\n\
Perhaps your file system is full.

89 Unable to open the Trash directory\n\
   %s\n\
(Authentication, access permission problem)\n\n\
All trash operations will not be performed.

90 The 'Trash Can' cannot be initialized.\n\
Please check:\n\
        1)$HOME/.dt/Trash for permissions.\n\
        2)Available disk space.\n\
        3)Authentication.

$ 91 is not used
91 A linked copy of the following object is on\n\
the Workspace backdrop:\n\
Moving the object to the trash will remove it from\n\
both the File Manager and Workspace backdrop.

$ 92 is not used
92 A linked copy of the following objects are also on\n\
the Workspace backdrop:\n\
Moving the objects to the trash will remove them from\n\
both the File Manager and Workspace backdrop.

93 Unable to access this trash information file:\n\
   %s\n\
All trash operations will not be performed.\n\
The most common causes are:\n\
     - Network authentication.\n\
     - Insufficient disk space.\n\
     - Wrong permissions $HOME/.dt/Trash.

94 The File Manager does not support objects\n\
with spaces in their names, so we cannot delete them.\n\
To delete these objects:\n\
  - select 'Open Terminal' from the File Manager menu bar\n\
  - then issue the remove command to delete them.

95 Unable to access the Trash directory:\n\
   %s\n\
All trash operations will not be performed.\n\
The most common causes are:\n\
     - Network authentication.\n\
     - Insufficient disk space.\n\
     - Wrong permissions.

96 The following objects could not be removed from the file system:   \n

97 The following objects could not be placed in the trash can:     \n

98 (Plus %d additional object(s))

$ message 99 is not used
99 You do not have write permission for the following file(s):\n\
Do you want to proceed?\n

100 The following folder(s) are not empty.\n\
Do you want to proceed?\n

101 The following object(s) could not be put back:\n

102 A linked copy of the following object is on\n\
the Workspace backdrop:\n\
%s\n\
Moving the object to the trash will remove it from\n\
both the File Manager and Workspace backdrop.

103 A linked copy of the following objects are also on\n\
the Workspace backdrop:\n\
%s\n\
Moving the objects to the trash will remove them from\n\
both the File Manager and Workspace backdrop.

104 You do not have write permission for the following file(s):\n\
Do you want to proceed?\n

105 Default action of a trash object will not be executed.\n\n\
To execute the default action of this object \n\
use 'Put Back' to return it to the File Manager\n\
then execute it there.

106 About Trash Can

107 At least one object in the following Folder cannot be deleted.\n\n%s\n\n\
The most likely cause is that you do not have permissions to \n\
delete that object.

108 The following file cannot be deleted. \n\n%s\n\n\
The most likely cause is that you do not have permissions to\n\
delete that object.

109 Put Back Warning

110 There is already an object named\n\n\
    %s\n\n\
Select Ok if you want to replace the object\n\
with the object from the Trash.\n\n\
Select Cancel if you want to keep the object.\n\
If you then rename the object, you will be able\n\
to repeat the Put Back and keep both objects.

111 Put Back Error

112 The following object(s) cannot be deleted.\n\
The most likely cause is that these object(s)\n\
have already been deleted.\n\n

113 At least one object in the following Folder(s) or \n\
the folder itself cannot be deleted.\n\
The most likely cause is that you do not have permissions\n\
to delete that object.\n\n

114 The following file(s) cannot be deleted.  The most likely cause \n\
is that you do not have permissions to delete the file(s).\n\n

115 The following object(s) cannot be deleted.\n\
The desktop cannot function properly without this object(s).\n\n

$set 28
$ ****** Module: Desktop.c ****(developer comment only)********

$ Messages 1-3 Workspace error messages
$ Message 4 is the title for the Workspace popup menus
$ Messages 5-8 are the labels and mnemonics for the Workspace obj popup menu

1 A file with this name already exists on the Workspace.\n\
Please choose a different name.
2 A file with this name already exists in the original folder\n\
(The folder where the file came from).\n\
Please choose a different name.
3 A file named %s already exists on the Workspace in this workspace.\n\
You cannot put another file with the same name on the Workspace.\n\
To place both files on the Workspace, rename one of them.

4 Workspace Object

$ Messages 5-6 are the text and mnemonic for the workspace popup
$ RemoveFromWorkspace menu option
5 Remove From Workspace
6 R
$ Messages 7-8 are the text and mnemonic for the workspace popup i
$ OpenParentFolder menu option
7 Open Parent Folder
8 O

9 An object with this name already exists on the Workspace.\n\
Please choose a different name.

10 An object with this name already exists in the original folder\n\
(The folder this object came from).\n\
Please choose a different name.

11 An object named:\n\n   %s\n\n\
already exists on the backdrop in this Workspace.\n\
You cannot put another object with the same name on the Workspace.\n\
To have both objects on the Workspace, rename one of them.

$ 
$ Messages 12 and 13 are new messages since the CDE/Sample.
$ 
12 An invalid workspace was passed. The default workspace will be\n\
workspace one and an object named:\n\n\
   %s\n\n\
already exists on the backdrop in this Workspace.\n\
You cannot put another object with the same name on the Workspace.\n\
To have both objects on the Workspace, rename one of them.

13 Object:\n\n\
   %s\n\n\
does not exist in the file system.



$set 29
$ ****** Module: Help.c ****(developer comment only)********

$ Message 1 is the title for the dtfile help dialog
$ Messages 1 and 2 are titles for the Help dialog, Message 2 is
$   used when the user has specified a title for dtfile windows
$ Message 3 is the text for help on the "Open New View" action
$ Message 4 is the text for help on the "Open In Place" action
$ Messages 5-6 are help error messages

1 File Manager Help
2 %s Help

3 The `%s` action opens a new File Manager view of the requested folder.

4 The `%s` action displays the requested folder in the current File Manager view.

5 There is no help available for the %s action
6 There is no help available for the %s filetype

7 The `%s` action opens a new File Manager view window\n\
of the requested folder.

8 The `%s` action displays the requested folder in\n\
the current File Manager view window.

9 Trash Can Help

10 Help is not available for multiple selected objects.


$set 30
$ ****** Module: SharedMsgs.c ****(developer comment only)********

$ The following messages are used in more than one place within the
$ dtfile source.  Unless otherwise specified, they are error messages.

1 Cannot read from %s

$ Message 2 is used to format the file attributes in 'by attributes' mode
2 %b %d %H:%M:%S %Y

3 Cannot move or copy file %s from host %s.\n\
Host %s may not be properly mounted to your file system.
4 Cannot move or copy file %1$s from host %2$s.\n\
Host %3$s may not be properly mounted to your file system.
5 Cannot read %s
6 Cannot write in folder %s
7 Cannot create %s
8 Cannot delete %s
9 A folder or file with the name\n\
  %s\n\
already exists and cannot be overwritten.\n\
Enter a different name.

$ Message 10 is the help topic string for help on a file icon or action
10 Description Of Filetype/Action

11 You cannot specify a path as part of the filename.
12 Invalid Find Information.\n\n\
The following folder does not exist, or is not accessible:\n\n\
     %s\n

$ Messages 13-16 are dialog titles
13 %s Overview
14 %s Tutorial
15 About %s
16 File Manager

$ Message 18 is appended to ".." folders
18 (go up)

$ Messages 19 and 20 are messages which appear when a user clicks item help
$ outside the application.  19 is the title, while 20 is the message.
19 Help On Item Error
20 You must select a component\n\
within the File Manager.

$ Messages 21 and 22 are titles for error dialogs
21 File Move Error
22 File Link Error
23 Cannot create '%s'\n\n\
The most common cause is that you do not have\n\
the correct permissions for the involved\n\
files or folders.\n\n\
To view permissions, select the object and\n\
then select 'Change Permissions...'

24 You must select an object within the File Manager.\n\
'On Item' help only works within the application where\n\
you launched it.

25 Cannot create '%s'\n\n\
The most common cause is that you do not have\n\
the correct permissions for the involved\n\
files or folders.\n\n\
To view permissions, select the object and\n\
then select 'Change Permissions...' from the\n\
Selected or popup menu.

26 You must select an object within the Application Manager.\n\
'On Item' help only works within the application where\n\
you launched it.


$set 31

$set 32
$ ****** Module: dterror.src ****(developer comment only)********
$ Message 1 is a title for dterror
1 dt Error
2 File Manager Open Directory Error

$set 33
1 Multiple Objects
2 Current Folder
3 Move Object Error
4 Copy Object Error
5 Link Object Error
6 The location you are trying to Move to:\n\n   %s\n\n\
does not exist in the file system.
7 The location you are trying to Copy to:\n\n   %s\n\n\
does not exist in the file system.
8 The location you are trying to Link to:\n\n   %s\n\n\
does not exist in the file system.
9 The location you are trying to Move to:\n\n   %s\n\n\
is not a folder.
10 The location you are trying to Copy to:\n\n   %s\n\n\
is not a folder.
11 The location you are trying to Link to:\n\n   %s\n\n\
is not a folder.
12 The object you are to trying Move:\n\n   %s\n\n\
does not exist in the file system.
13 One of the objects you are trying to Move:\n\n   %s\n\n\
does not exist in the file system.\nNot Moving any of them.
14 The object you are to trying Copy:\n\n   %s\n\n\
does not exist in the file system.
15 One of the objects you are trying to Copy:\n\n   %s\n\n\
does not exist in the file system.\nNot Copying any of them.
16 The object you are trying to Link:\n\n   %s\n\n\
does not exist in the file system.
17 One of the objects you are trying to Link:\n\n   %s\n\n\
does not exist in the file system.\nNot Linking any of them.

$set 99
$ ****** Version Information ********
$ DO NOT TRANSLATE THESE MESSAGES
$ DO NOT CHANGE THESE MESSAGES
$ DO NOT LOCALIZE THESE MESSAGES
$ These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $TOG: dtfile.msg /main/10 1999/09/16 09:42:03 mgreess $
