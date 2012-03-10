$ $XConsortium: pam_unix.msg /main/2 1996/07/24 19:04:15 drk $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        pam_unix.msg
$ **
$ **  Project:     pam
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the pam_unix message catalog.  
$ **
$ **  It contains messages for the pam_unix service module (set 1).  
$ **
$ **  Also contains messages for pam_dial (set 2) and pam_test (set 3) modules.
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1996 Sun Microsystems, Inc.
$ **     All Rights reserved
$ **
$ *****************************************************************************
$ *************************************<+>*************************************

$ *****************************************************************************
$
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$
$ There may be three types of messages in this file:
$
$  1. Messages that appear in dialogs or are displayed to the user.
$
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$
$  2. Messages that only appear in the error log file. 
$
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$
$  3. Messages that should not be localized.
$
$     These messages are identified by the following:
$
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz
$
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$
$ ******************************************************************************

$ unix module messages 
$set 1

$ switch_utils.c
$ Msg 1 thru 8 print on consecutive lines.
$ Keep intentation for "passwd:" in messages 2 thru 7 

1 Supported configurations for passwd management are as follows:
2     passwd: files
3     passwd: files nis
4     passwd: files nisplus
5     passwd: compat
6     passwd: compat AND
7     passwd_compat: nisplus
8 Please check your /etc/nsswitch.conf file

9 Can't find name service for passwd
10 You may not use nisplus repository
11 Your specified repository is not defined in the nsswitch file!

$ unix_acct_mgmt.c
20 Your password has been expired for too long.
21 Please contact the system administrator
22 Your password will expire within 24 hours.
23 Your password will expire in %d day.
24 Your password will expire in %d days.

$ unix_authenticate.c
30 UNIX Password: 
31 Password: 

$ unix_get_authtokattr.c
40 You must specify repository when displaying passwd attributes

$ leave "%s" unchanged
41 %s: System error: repository out of range
42 Unable to retrieve username.

$ leave "%s" unchanged
43 %s: Unexpected failure. Password database unchanged.

$ unix_setcred.c
$ translate 50-51 as a single sentence 
50 Removing root credentials would break the rpc services that
51 use secure rpc on this host!

52 Root may use keylogout -f to do this (at your own risk)!
53 Could not unset your secret key.
54 Maybe the keyserver is down?
55 Warning: NFS credentials not destroyed
56 Password does not decrypt secret key for %s.
57 Could not set secret key for %s. The key server may be down.

$ unix_update_authtok.c
60 %s%s:  %s does not exist
61 %s%s: user must have NIS+ credential
62 Enter login(NIS) password: 
63 Enter login(NIS+) password: 
64 Enter login password: 
65 %s%s: Sorry.
66 %s%s: Sorry, wrong passwd
67 This password differs from your secure RPC password.
68 Please enter your old Secure RPC password: 
69 This password does not decrypt your secure RPC credentials. Try again:
70 Please enter your old Secure RPC password: 
71 %s%s: Sorry: less than %ld days since the last change.
72 %s%s: You may not change this password.
73 %s%s: can't create process
74 New password: 
75 Re-enter new password: 
76 %s%s: Too many tries; try again later
77 %s%s: They don't match; try again.
78 %s%s: Too many failures - try later.
79 %s%s: Password too short - must be at least 6 characters.
80 %s%s: Password cannot be circular shift of logonid.
81 %s%s: The first %d characters of the password
82 must contain at least two alphabetic characters and at least
83 one numeric or special character.
84 %s%s: Passwords must differ by at least 3 positions
85 nispasswd: user must have LOCAL credential
86 %s: invalid LOCAL credential

$ unix_update_authtok_file.c
90 %s%s: Password database busy. Try again later.
91 %s%s: Unexpected failure. Password database unchanged.
92 %s%s: Unexpected failure. Password database missing.
93 %s%s: passwd successfully changed for %s
94 %s: No local passwd record

$ unix_update_authtok_nis.c
100 %s%s: couldn't change passwd/attributes
101 %s%s: Couldn't change passwd/attributes for %s
102 NIS(YP) passwd/attributes changed on %s
103 System error: no NIS passwd record for %s
104 %s: System error%s: shell is set illegally
105 System error%s: homedir is set illegally.
106 System error: gecos is set illegally.
107 %s: secret key reencrypted for %s on %s

$ unix_update_authtok_nisplus.c
108 NIS+ password information changed for %s
109 NIS+ credential information changed for %s
110 Unable to reencrypt NIS+ credentials for %s;
111 %s%s: Password information update failed
112 NIS+ password information changed for %s
113 The NIS+ credential information for %s will not be changed.
114 User %s must do the following to update his/her
115 credential information:
116 Use NEW passwd for login and OLD passwd for keylogin.

$ translate 117-118 as a single sentance. 
117 Use \"chkey -p\" to reencrypt the credentials with the
118 new login passwd.

119 The user must keylogin explicitly after their next login.
120 WARNING: Could not reencrypt NIS+ credentials for %s;
121 login and keylogin passwords differ.
122 Use NEW passwd for login and OLD passwd for keylogin.
123 You must keylogin explicitly after your next login.
124 %s%s: couldn't change password for %s. 
125 Reason: failed to update the cred table with reencrypted credentials.
126 Please notify your System Administrator.
127 NIS+ Password incorrect: try again
128 NIS+ password information update failed while talking to NIS+ passwd daemon 
129 GECOS information was not updated: check NIS+ permissions.
130 SHELL information was not updated: check NIS+ permissions.
131 NIS+ Credential information was not updated.
132 No NIS+ record
133 NIS+ password information update failed (update_attr) 

$ unix_utils.c
140 %s%s: Permission denied
141 %s%s: Can't change local passwd file\n
142 %s%s: System error: repository out of range
143 Cannot change from restricted shell %s
144 Old shell: %s
145 New shell: 
146 Login shell unchanged.
147 %s is unacceptable as a new shell
148 warning: %s is unavailable on this machine
149 Default values are printed inside of '[]'.
150 To accept the default, type <return>.
151 To have a blank entry, type the word 'none'.
152 Name [%s]: 
153 Finger information unchanged.
154 Home Directory [%s]: 
155 Homedir information unchanged.
156 ':' is not allowed.
157 Maximum number of characters allowed is %d.
158 Control characters are not allowed.

$ dialup password module messages 
$set 2

1 Dialup Password: 

$ dialup password module messages 
$set 3
1 TEST Password: 
2 Password: 
