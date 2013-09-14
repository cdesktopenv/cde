$ "$XConsortium: dtsession.msg /main/4 1995/12/08 09:50:48 rswiston $";

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtsession.msg
$ **
$ **  Project:     HP DT  Session Manager (dtsession)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtsession
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *
$ * Revision 1.1  93/12/15  09:18:54  09:18:54  bertanij_hp_cv
$ * Initial revision
$ *
$ * Revision 1.7  93/08/12  20:18:36  20:18:36  xbuild (See Marc Ayotte)
$ * CoeToDt check-in
$ * 
$ * Revision 1.6  93/03/30  08:39:54  08:39:54  cose ( Marc Ayotte)
$ * revision 1.6
$ * date: 1993/03/30 16:12:17;  author: daveb;  state: Exp;  lines: +17 -10
$ * added message (28,12), removed instances of HP in message text
$ * daveb@HP-CORVALLIS
$ * 
$ * Revision 1.6  1993/03/30  16:12:17  daveb
$ * added message (28,12), removed instances of HP in message text
$ *
$ * Revision 1.5  1993/03/12  03:35:48  build
$ * revision 1.5
$ * date: 93/03/10 14:41:28;  author: bill;  state: Exp;  lines added/del: 39/16
$ * insured that comment lines start with "$-blank"
$ * bill@ibm
$ *
$ * Revision 1.5  93/03/10  14:41:28  14:41:28  bill (Bill Frolik)
$ * insured that comment lines start with "$-blank"
$ * 
$ * Revision 1.4  93/02/26  05:03:38  05:03:38  ronv (Ronald Voll)
$ * Names Changed: 
$ * # ======= MapCB ====> _DtMapCB =====
$ * # ======= Message ====> _DtMessage =====
$ * # ======= MessageClose ====> _DtMessageClose =====
$ * # ======= MessageDialog ====> _DtMessageDialog =====
$ * # ======= MessageOK ====> _DtMessageOK =====
$ * # ======= PName ====> _DtPName =====
$ * # ======= SaveSubdialogArray ====> _DtSaveSubdialogArray =====
$ * # ======= StringsAreEquivalent ====> _DtStringsAreEquivalent =====
$ * # ======= StripSpaces ====> _DtStripSpaces =====
$ * # ======= TextChangeSpace ====> _DtTextChangeSpace =====
$ * # ======= AdFtEntryConvert20 ====> _DtAdFtEntryConvert20 =====
$ * # ======= AdFtEntryConvert30 ====> _DtAdFtEntryConvert30 =====
$ * # ======= FtEntryConvert20 ====> _DtFtEntryConvert20 =====
$ * # ======= FtEntryConvert30 ====> _DtFtEntryConvert30 =====
$ * # ======= NextToken ====> _DtNextToken =====
$ * # ======= AddEntry ====> _DtAddEntry =====
$ * # ======= AppendEntryToBuffer ====> _DtAppendEntryToBuffer =====
$ * # ======= AppendToBuffer ====> _DtAppendToBuffer =====
$ * # ======= CompareEntries ====> _DtCompareEntries =====
$ * 
$ * Revision 1.3  93/01/28  15:48:32  15:48:32  xbuild (See Marc Ayotte)
$ * XmpToDt check-in
$ * 
$ * Revision 1.2  93/01/27  19:51:40  19:51:40  xbuild (See Marc Ayotte)
$ * XueToDt check-in
$ * 
$ * Revision 1.1  93/01/12  10:43:06  10:43:06  xbuild (See Marc Ayotte)
$ * Initial revision
$ * 
$ * Revision 1.1  93/01/07  16:40:28  16:40:28  xbuild (See Marc Ayotte)
$ * Initial revision
$ * 
$ * Revision 1.17  92/07/13  16:41:57  16:41:57  fredh (Fred Handloser)
$ * added set 16 message 9 for the panacomm work
$ * 
$ * Revision 1.16  92/07/10  16:25:38  16:25:38  fredh (Fred Handloser)
$ * revised error message for failed remote execution
$ * 
$ * Revision 1.15  92/07/08  10:49:23  10:49:23  fredh (Fred Handloser)
$ * added set 16 message 8 for failed remote execution.
$ * 
$ * Revision 1.14  92/06/09  16:23:32  16:23:32  fredh (Fred Handloser)
$ * added message that will go to the error log if the 
$ * wmStartupCommand is not executable.
$ * 
$ * Revision 1.13  92/05/18  10:27:34  10:27:34  jenny (Jennefer Wood)
$ * set 28, msg 10 uncomment second line - bug
$ * 
$ * Revision 1.12  92/05/11  11:02:48  11:02:48  fredh (Fred Handloser)
$ * changed comment in general section and set 29
$ * 
$ * Revision 1.11  92/05/04  14:36:20  14:36:20  jenny (Jennefer Wood)
$ * removed unused Color Srv messages
$ * 
$ * Revision 1.10  92/04/29  16:36:31  16:36:31  fredh (Fred Handloser)
$ * changed item 2 of the bms died warning from 
$ * "Press the logout button ..." to "Logout"
$ * 
$ * Revision 1.9  92/04/23  13:15:41  13:15:41  kimd (Kim Drongesen)
$ * changed /usr/admin to /usr/adm
$ * 
$ * Revision 1.8  92/04/09  17:13:46  17:13:46  fredh (Fred Handloser)
$ * obsoleted set 18 message 11 and replaced it with set 18
$ * message 38 
$ * 
$ * Revision 1.7  92/03/27  15:23:15  15:23:15  kimd (Kim Drongesen)
$ * fixed no bms startup dialog
$ * 
$ * Revision 1.6  92/03/26  15:54:32  15:54:32  kimd (Kim Drongesen)
$ * made change to compatibility mode logout dialog
$ * 
$ * Revision 1.5  92/01/29  15:59:46  15:59:46  kimd (Kim Drongesen)
$ * added the SmHelp files
$ * 
$ * Revision 1.4  92/01/23  15:59:00  15:59:00  kimd (Kim Drongesen)
$ * added in MOST of the display/lang independent stuff
$ * 
$ * Revision 1.3  92/01/16  15:29:47  15:29:47  kimd (Kim Drongesen)
$ * added error message when lock on timeout isn't available
$ * 
$ * Revision 1.2  92/01/16  14:44:49  14:44:49  kimd (Kim Drongesen)
$ * fixed remote execution (added error message on failure)
$ * 
$ * Revision 1.1  91/10/31  16:26:48  16:26:48  keith (Keith Taylor)
$ * Initial revision
$ * 
$ * Revision 1.7  91/08/13  12:08:27  12:08:27  tom (Tom Houser)
$ * commented out entire message where only first line was commented before
$ * for OSF/1
$ * 
$ * Revision 1.6  90/11/26  08:40:32  08:40:32  kimd
$ * added Barbara Wingert's changes
$ * 
$ * Revision 1.5  90/11/14  13:31:08  13:31:08  kimd (Kim Drongesen)
$ * Fixed problem with lock on trusted systems
$ * 
$ * Revision 1.4  90/11/13  08:03:21  08:03:21  kimd (Kim Drongesen)
$ * made changes to long message
$ * 
$ * Revision 1.3  90/10/25  14:34:05  14:34:05  fredh (Fred Handloser)
$ * added Log and Revision string
$ * 
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a 
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ * Do not localize sets 2, 4, 6, 8, 10, 12, 14, 16.
$ * Do not localize sets 20, 22, 24, 26, 28, 99.  
$ * They only appear in the errorlog.
$ *
$ * DO localize sets 18 and 29. 
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$set 2
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmMain.c --
$ These are errorlog messages
$ DO NOT TRANSLATE THIS SET.

2 Another dtsession is currently running - exiting.
3 Screen lock when the screen saver is turned on is not available on this server.  Your screen will not be locked when the screen saver is turned on. 

$set 4
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmGlobals.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 Invalid display name - exiting.
2 No defaults files exist.  No applications will be restarted.
3 No defaults files exist.  No applications will be restarted.
4 Unable to remove session directory.  Make sure write permissions exist \
on $HOME/.dt directory.  Invalid session files will not be removed.
5 Unable to malloc memory for operation.
6 Unable to lock display.  Another application may have the pointer or \
keyboard grabbed.
7 Unable to open session file.  No clients will be restarted.
8 Unable to fork client process.
9 Unable to create DT directories.  Check permissions on home directory.
10 Unable to lock display due to security restrictions.
$ The next message is added to the error log if a required temporary     
$ file can not be created by the session manager.     
$ The insert is the name of the temporary file.     
11 "àÒÎÎÐúÇ¡í²Çôóò: %s. îùíýÌùè×ÈºóòÕùÈ´ìÑÄ«ûâ¡¤ÈÜâêÑôÉ¢äÆê§Ü¡Ï¯á¸ÊÖ¡¤"

$set 6
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmMsgSrv.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 Unable to start message server - exiting.
2 The DT message server has exited.


$set 8
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmError.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 Connection to server lost - exiting.


$set 10
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmProperty.c --


$set 12
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmProtocol.c --


$set 14
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmSave.c --


$set 16
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmRestore.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 Unable to exec process /usr/bin/X11/xrdb.  No session resources will be \
restored.
2 Invalid client settings file.  No settings restored.
3 Unable to exec %s.
4 Unable to exec process /usr/dt/bin/dtwm.  No window manager will be started.
5 You have reached the maximum allowed number of remote clients.  No further remote clients will be restored.
6 Invalid display/language independent resource file.  No display/language independent resources will be restored.
7 The wmStartupCommand resource is set to:\n\n\
      %s\n\n\
This file does not exist or is not executable.\n\
/usr/dt/bin/dtwm will be started instead.\n
8 An attempt to restore the following\n\
command (using the DT remote execution process)\n\
on host "%s" failed:\n\n\
      %s\n\n\
The following execution string will be tried:\n\n\
   %s\n\n
9 The following window manager did not start:\n\n\
      %s\n\n\
This message indicates you tried to start a\n\
window manager that is built into an X terminal.\n\
This will only work with X terminals that support this protocol.\n\
/usr/dt/bin/dtwm will be started instead.\n


$set 18
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmUI.c -- 
$ LOCALIZE Set 18
$ Deleted Messages:  13, 14, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
$						29, 30, 31, 32, 33

$ _DtMessage 1 tells the user who currently has the display locked.
1 üÏÆüðÂÝ·ËðÆîÏ¯ %s ÷ÕÌù¡¤

$ _DtMessage 2 tells the user to enter the password for the above user to unlock
$ the display.
2 îùòÓÄ«Ùïî£ÅèæØØæ÷ÕÌù¡¤

$ _DtMessage 3 is a prompt for the user to enter a password.
3 Ùïî£¡¨

$ _DtMessage 4 is the logout message that appears when dtsession is going
$ to save the current session information. 
$ Access this message by pressing the logout button.
4 âäÌîàôÅøÝ¨Ü¡...\n\n\
ÍÔÈ´ÆÜóÃÇôÎûä»æñÕèÆ«×äÝçäÄòÜÆÂ¡¤\n\n\
ÚÀÆøÐ©ÎûâêÑôÉ¢äÆäÄÝ·óÃÇô¡§àÓÈÔÈ´ËèóÜÆîá£È¢\n\n\
Æ«×äëæÓÑÇãÄ¶È¹àôÄ«ÕëÇÂÓìä»ÚöÙ¯¡¤

$ _DtMessage 5,6 is the logout message that appears when dtsession is not going
$ to save the current session information - but return to the home session info
5 âäÌîàôÅøÝ¨Ü¡...\n\n\
ÍÔÈ´ÆÜóÃÇôÎûä»æñÕèÆ«×äÝçäÄòÜÆÂ¡¤\n\n\
ÝÕÔ¶âêÑôÉ¢äÆÄâäÄÝ·óÃÇô¡¤ÈþÚÀÎûØÄÌîâêÑôÉ¢äÆ\n\n\
ÇãÚÀÄ¶È¹àôÄ«ÕëäÄÇßß¦¡¤
$ _DtMessage 6 is the title of the logout dialog box.
6 àôÅøíýë¥

$ _DtMessage 7,8,9,10 are all part of the logout dialog that appears when
$ the user has requested to have the choice
$ to either save the current session or return to the home session.
7 âäÌîàôÅøÝ¨Ü¡...\n\n\
Ä¶È¹ÚÀàôÄ«Õë¡¢È´Ì¥Ô¶òÙðåÆ«ÅèÚêÇÄÖ¤Ä¸ê§Ü¡Ë·ÜÓ¡¨\n\n\
ÚÀÆ«ÅèóÃÇôÜ¢ÈçâêÑôÉ¢äÆ¡¢Ä¶È¹É»Æ«ÅèÔÏíµÓìä»ÚöÙ¯¡¤\n\n\
ÍÐÏ¯or\n\n\
ÚÀÆ«ÅèÍóÛ¹Ü¢ÈçâêÑôÉ¢äÆ¡¢ÇñÈºÚÀÎûØÄÌîâêÑôÉ¢äÆÞóÆ«ÅèÇßß¦¡¤\n\n\
ÈþÄâê§ËðÆîÈôê¡ÅÉÈ¢¡¢ÆÜóÃÇôÎûä»æñÕèÝçäÄòÜÆÂ¡¤
8 àôÅø¡¨\nóÃÇôÜ¢ÈçâêÑôÉ¢äÆ
9 àôÅø¡¨\nÇßß¦ØÄÌîâêÑôÉ¢äÆ
$ _DtMessage 10 is the title of the logout dialog box.
10 àôÅøíýë¥

$ _DtMessage 11,12 are part of the error dialog that appears when the DT
$ messaging system dies.
$ These messages are generated when you kill the BMS (broadcast _DtMessage
$ Server), which is /usr/softbench/bin/softmsgsrv.
$ _DtMessage 11 is obsolete.  _DtMessage 38 replaces message 11

$ 11 DT Messaging System Inoperative\n\n\
$ To restart:\n\n\
$ 1) Save all open data files.\n\
$ 2) Press the reset key sequence to return to the login screen.\n\
$ 3) Log in again.\n\n\
$ Note: The current session will not be saved.\n\n\
$ When you are ready to begin the restart process, click [OK] and\n\
$ proceed to save your files.

12 _DtMessage ÆÂÚõ

$ Messages 15, 16, 17 are used for identical purpose as messages 1, 2,3
$ in this set.
15 üÏÆüðÂÝ·ËðÆîÏ¯ %s ÷ÕÌù¡¤
16 îùòÓÄ«Ùïî£ÅèæØØæ÷ÕÌù¡¤
17 Ùïî£¡¨

$ _DtMessage 34, 35 are messages that appear when the user started up in 
$ compatibility mode, and tries to log out.  You can only exit out of 
$ compatibility mode by using the reset key sequence.
$ To generate this message, start in x11 (not DT) and then run dtsession.
$ Refer to the dtsession manpage.  Then try to log out.
34 ÝÕÔ¶âêÑôÉ¢äÆÑÒÅèÄ¡Ô¶ X áþá§Ë·ÜÓÚöÙ¯ÌÔÅïâõÍÔÚöÙ¯¡¤\n\n\
ÄâäÄóÃÇôÇ¶ÈôâêÑôÉ¢äÆæñØ¹¡¤\n\nîùËðÆîÓìÝÃÑºõïÊ©ÇÄàôÅø¡¤
35 àôÅø _DtMessage

$ Messages 36,37 are part of the error dialog that appears when dtsession is
$ unable to start the DT messaging system, and therefore not able to bring
$ up the system at all.
$ To generate this, move the message server from
$ /usr/softbench/bin/softmsgsrv to some other directory, then start Dt.
36 DT Ø¹ÕÉË·ÜÓàÒÎÎÚöÙ¯¡¤\n\n\
ÓÑæØÊîÝÕÔ¶ÙÂ÷î¡¨\n\n\
1.  îùòÙðå [íýÌù] ÏÓÇßàôÄ«ò­èÝ¡¤\n\n\
2.  Ú·àôÄ«ò­èÝÎûòÙâúÅü×äÏÐÄãòÙÌ½¡ÖâêÑôÉ¢äÆ\n\
ÑÊëâÈÜÙ¯Ïñòò¡×ÄæÑ¥àôÄ«¡¤\n\n\
3.  óôÑçÅèÄ¶ÈíåôÎûÅäñ¢ÇØê¢ÑÒÉÁÆßíý¡¨\n\
     /etc/src.sh\n\
     /etc/hosts\n\
     /usr/adm/inetd.sec\n\n\
Çñëæ÷ìÆÀÎûæñØ¹¡¢îùÙ¶ïå¡ÖDT ËðÆîÅÃÅõ¡×¡¤
37 ÆÒÓÑÎûÙ¯É¢

$ _DtMessage 38 is part of the error dialog that appears when the DT
$ messaging system dies.
$ This messages are generated when you kill the BMS (broadcast _DtMessage
$ Server), which is /usr/softbench/bin/softmsgsrv.

38 Ø¹ÕÉË·ÜÓàÒÎÎËðÆî\n\n\
Ó¼ÓÑÓìä»ÚöÙ¯¡¨\n\n\
1) óÃÇôÍÔÈ´ÄØâäÚöÎûæñÕèóò¡¤\n\
2) àôÅø¡¤\n\
3) Óìä»àôÄ«¡¤\n\n\
ÎÃãô¡¨Ü¢ÈçâêÑôÉ¢äÆÄâäÄÝ·óÃÇô¡¤\n\n\
åµÚÀäíÞ¬ÇïÓÑÓìä»ÚöÙ¯Ý¨Ü¡Õë¡¢îùÑºÄ¡Ä¶ [íýÌù] àÓÑ¥\n\
óÃÇôÚÀÎûóòÕù¡¤

$ Messages 39 and 40 are displayed on the logout confirmation dialogs

39 ú«úýàôÅø
40 Ì½Ö¼àôÅø

$set 20
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmWindow.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 Client message failed.  Client information will not be saved.
2 Invalid root window.  Can not save client information.
3 Session restoration information not updated for client .  Invalid \
information may be saved.
4 Session restoration information not updated for client %s.  Invalid \
information may be saved.


$set 22
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmLock.c --


$set 24
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- version.c --


$set 26
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SrvPalette.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 Color Server Warning: couldn't get ownership of the selection:
2 Color Server Warning, losing ownership of the selection:
3 Should never lose the selection.
4 Warning, found more pixels then are available.
5 Warning, can't allocate enough pixels.


$set 28
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SrvFile_io.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.
$ Messages 1,2,6,7,8,9 are obsolete

$ 1 Color Server Warning, the size of file '
$ 2 ' is invalid\nremoving file and trying again.
$ 6 Color Server FatalError, could not make directory
$ 7 Color Server FatalError: couldn't open or create
$ 8 Color Server FatalError, couldn't open file
$ 9 Color Server FatalError, couldn't open system file

3 Color Server Warning, the size of file is invalid:
4 Color Server Warning, invalid information in '
5 ' removing file and starting again.\n
10 Color Server FatalError, not enough memory to create the structure for \
a palette.\n
11 Warning: Color Server system default file is corrupt\n
12 Color Server Warning, could not read file %s\n

$set 29
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmHelp.c -- 
$ LOCALIZE Set 29
1 âêÑôÉ¢äÆê§Ü¡Ï¯ëºÉ·ë©Íü

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)_DtMessage catalog source $XConsortium: dtsession.msg /main/4 1995/12/08 09:50:48 rswiston $"

