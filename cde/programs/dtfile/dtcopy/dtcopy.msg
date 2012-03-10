$ $XConsortium: dtcopy.msg /main/4 1996/10/29 17:06:57 mustafa $
$ ************************************<+>*************************************
$ ****************************************************************************
$ *
$ *   FILE:           dtcopy.msg
$ *
$ *
$ *   DESCRIPTION:    Message catalog file
$ *
$ *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
$ *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
$ *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
$ *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
$ *
$ ****************************************************************************
$ ************************************<+>************************************* 

$set 2
$ ****** Module: main_dtcopy.c ******
3 file
4 files
5 subfolder
6 subfolders
7 Errors: none
8 Errors: %d
10 Usage: %s [options ...] source target\n
11 File Manager - Folder Move
12 File Manager - Folder Copy
13 %s not removed because of errors.
14 Completed.
15 Cancelled in
$ ** Message 20 - 32
$ ** LongFileOpNames ... message number = op + 20.
20 processing
21 opening
22 opening
23 opening
24 reading link
25 deleting
26 copying
27 creating directory
28 copying link
29 creating link
30 working
31 is a copy
32 is a link
33 renaming

$set 3
$ ****** Module: copydialog.c ******
1 Resume
2 Pause
3 Moving:
4 Copying:
5 Into folder:
7 This will stop the copy process. Any files that have\n\
already been copied will remain in the destination folder.
10 This will stop the move process. Any files that have\n\
already been moved will remain in the destination folder.
11 Folder Copy - Warning
12 Folder Move - Warning
13 Cancel Copy
14 Continue Copy
15 Cancel Move
16 Continue Move
17 Moved
18 Copied
19 "Cannot move folder into itself\n\
%s\n"
20 "Cannot copy folder into itself\n\
%s\n"

$set 4
$ ****** Module: errordialog.c ******
1 Error %s\n%s
2 Pause for errors
3 Folder Copy - Error
4 Folder Move - Error
5 Object Move - Error
6 Object Copy - Error
7 Object Trash - Error
8 You do not have permission to put the object \n\n\
    %s\n\n\
into trash.\n\n\
Use the Change Permissions choice from the object's\n\
popup menu or from the Selected menu to turn on your\n\
Read permission on the object.\n\n\
\n\
Note: If this object is a folder, you must also have\n\
Read permission for each of the objects inside the\n\
folder before you can put the folder in the trash.

9 Cannot open %s

$set 5
$ ****** Module: overwrtdialog.c ******
1 A file named\n%s\nalready exists inside the folder\n%s
2 Folder Move - File Overwrite
3 Replace existing file
4 Rename existing file to:
5 Rename new object to:
6 Skip this file
7 Apply this action to subsequent name conflicts
8 Action to take:
9 Folder Copy - File Overwrite
10 %s\nalready exists: choose another name.
11 File Overwrite - Error
12 %s\ncannot be renamed to\n%s\n%s

$set 6
$ ****** Module: utils.c ******
$ ** Button labels.
1 OK
2 Pause
3 Cancel
4 Help
5 Directory Copy Help
6 Continue
7 Cancel Copy
8 Cancel Move
