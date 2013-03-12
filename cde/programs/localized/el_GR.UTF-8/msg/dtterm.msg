$ $TOG: dtterm.msg /main/4 1999/09/17 13:27:14 mgreess $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtterm.msg
$ **
$ **  Project:     Common Desktop Environment dtterm
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtterm
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
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
$ 03/15/94 B. May             Initial rev
$
$ *****************************************************************************

$ ************************************************************
$ ******* Translated to Greek by Antonis Tsolomitis **********
$ ********************* 6 March 2013 *************************
$ ************************************************************

$set 1
$ ****** Module: DtTermMain.c 
$ Message 1 is the window menu addition - do not translate f.separator or
$ f.send_msg.  It will be used as the format string for a sprintf() command,
$ so it is necessary to double up on any percent signs ('%') that you
$ want to keep in the string.
$ Message 2 is an error message

1 \
	Sep             f.separator\n\
	Toggle\\ Menu\\ Bar  f.send_msg %d\
	
2 %s: Η επιλογή -S δεν μπορεί να χρησιμοποιηθεί με την -server ή την -serverid\n
3 Τερματικό

$set 2
$ ****** Module: DtTermSyntax.c 
$ These are the usage messages. 
$ DO NOT TRANSLATE Messages 37 or 41 ("End-of-list")

1 -/+132			ενεργοποίηση/απενεργοποίηση 80<->132 στήλη escape seq
2 -/+aw				ενεργοποίηση/απενεργοποίηση αυτόματης αναδίπλωσης
3 -bg color			χρώμα παρασκηνίου
4 -/+bs				turn off/on Term background is select color
5 -display displayname		ο εξυπηρετητής X στον οποίο θα γίνει σύνδεση
6 -e command args		εκτέλεση εντολής
7 -fb fontset			έντονη γραμματοσειρά κειμένου
8 -fg color			χρώμα κειμένου
9 -fn fontset			γραμματοσειρά κειμένου
10 -geometry geom			μέγεθος (σε χαρακτρες) και θέση του παραθύρου
11 -help				αυτό το μήνυμα
12 -/+iconic			εκκίνηση ή όχι σε εικονίδιο
13 -/+j				ενεργοποίηση/απενεργοποίηση κύλισης (jump scroll)
14 -/+kshMode			ενεργοποίηση/απενεργοποίηση ksh
15 -/+l				ενεργοποίηση/απενεργοποίηση αρχείου καταγραφής
16 -lf filename			όνομα αρχείου καταγραφής
17 -/+ls				ενεργοποίηση/απενεργοποίηση κελύφους εισόδου (login shell)
18 -/+map				ενεργοποίηση/απενεργοποίηση map window στην έξοδο pty
19 -/+mb				ενεργοποίηση/απενεργοποίηση κουδουνίσματος στο περιθώριο
20 -ms color			χρώμα δείκτη
21 -n string			καθορισμός ονόματος εικονιδίου
22 -name string			εμφάνιση πελάτη, εικονίδιο, και συμβολοσειρά τίτλου
23 -nb distance			καθορισμός απόστασης για το κουδούνισμα από το δεξί περιθώριο
24 -/+rw				ενεργοποίηση/απενεργοποίηση αντίστροφης αναδίπλωσης
25 -/+sb				ενεργοποίηση/απενεργοποίηση κύλισης
26 -/+sf				ενεργοποίηση/απενεργοποίηση ειδικά πλήκτρα της SUN
27 -sl number[s]			αριθμός γραμμών για αποθήκευση
28 -ti name			συμβολοσειρά για προγραμματική ταυτοποίηση
29 -title string			συμβολοσειρά τίτλου παραθύρου
30 -tm string			λέξεις κλειδιά και χαρακτήρες κατάσταση τερματικού
31 -tn name			όνομα μεταβλητής περιβάλλοντος TERM
32 -/+vb				ενεργοποίηση/απενεργοποίηση οπτικό κουδούνισμα
33 -xrm resourcestring		additional resource specifications
34 -C				κατάσταση κονσόλας
35 -Sxxd				slave mode on \"ttyxx\" file descriptor \"d\"
36 -Sxxx.d			slave mode on \"ttyxxx\" file descriptor \"d\"
37 End-Of-List
38 The -e option, if given must appear at the end of the command line,
39 otherwise the user's default shell will be started.  Options that start
40 with a plus sign (+) restore the default.
41 End-Of-List
47 %s:  κακή επιλογή στη γραμμή εντολών \"%s\"\r\n\n
48 χρήση:  %s
49 \r\n   
50  [%s]
51 \r\n\nΓράψτε \"%s -help\" για εκτενή βοήθεια.\r\n\n
52 χρήση:\n
53 \t%s [-options ...] [-e command args]\n\n
54 όπου οι επιλογές περιλαμβάνουν:\n

$set 3
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 Η ΜΝΗΜΗ ΓΕΜΙΣΕ\nΠατήστε OK για το καθάρισμά της

$set 4
$ ****** Module: TermPrim.c
$ This is the title of a dialog box
2 Τερματικό - Προειδοποίηση

$set 5
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 Η ΜΝΗΜΗ ΓΕΜΙΣΕ\nΠατήστε OK για το καθάρισμά της

$set 6
$ ****** Module: TermView.c
$ DO NOT TRANSLATE Message 1
$ Messages 2 and 3 are titles for dialog boxes.
1 80x24 132x24
4 Τερματικό - Σελίδες βοήθειας
5 Τερματικό - Βοήθεια

$set 7
$ ****** Module: TermViewGlobalDialog.c
$ These are the user-visible labels in the Global Options dialog box.
$ #1 is the title. 
$ The rest are resource types and values - look at the dialog box and 
$ documentation for full specs. 
$
$ DO NOT TRANSLATE MESSAGE 15
$
2 OK
3 Ακύρωση
4 Βοήθεια
5 Γενικά
6 Έλεγχος δείκτη
7 Κουτί
8 Υπογράμμιση
9 Είδος δείκτη
10 Ενεργοποιημένο
11 Απενεργοποιημένο
12 Δείκτης που αναβοσβήνει
13 Ρυθμός που αναβοσβήνει (σε χιλιοστά του δευτερολέπτου)
14 Αόρατος
15 blinkRate
16 Έλεγχος χρώματος
17 Αντίστροφο
18 Κανονικό
19 Παρασκήνιο παραθύρου
20 Κύλιση
21 Απενεργοποιημένη
22 Ενεργοποιημένη
23 Ομαλή κύλιση
24 Κουδούνισμα
25 Ακουστικό
26 Ορατό
27 Είδος κουδουνίσματος 
28 Ενεργοποιημένο
29 Απενεργοποιημένο
30 Προειδοποίηση περιθωρίου
31 Απόσταση περιθωρίου
32 Εφαρμογή
33 Επαναφορά
34 Τερματικό - Γενικές επιλογές
$set 8
$ ****** Module: TermViewMenu.c
$ These are the menu bar entries and menu pane choices.
$ NOTE TO TRANSLATORS: The Single letter entries are the menu mnemonic
$ for the entry above them (i.e. message 11 "F" is the mnemonic for
$ message 10 "Font Size"). 
$ The items with a "+" in them are the accellerators for an item.
$ The single-character items are the mnemonics for a menu item, where the
$ text of the menu item is specified in the previous message
$ Do not translate messages 24 and 28.
1 Default
2 Μέγεθος Παραθύρου
3 Π
4 -iso8859-1
5 %d point
6 %.1f point
7 %.1f point
8 %.2f point
9 Default
10 Μέγεθος Γραμματοσειράς
11 Γ

$   Window menu
12 Νέο
13 Ν
14 Εκτύπωση
15 Ε
16 Εκτύπωση...
17 τ
18 Κλείσιμο
19 Κ
20 Παράθυρο
21 Π

$   Edit menu
22 Αντιγραφή
23 Α
$ IBM fixed msg 24 for 41J
$ 24 Ctrl osfInsert
24 Ctrl<Key>osfInsert
25 Ctrl+Insert
26 Επικόληση
27 Ε
$ IBM fixed msg 28 for 41J
$ 28 Shift osfInsert
28 Shift<Key>osfInsert
29 Shift+Insert
30 Επεξεργασία
31 π

$   Option menu

32 Γραμμή Μενού
33 Μ
34 Γραμμή Κύλισης
35 Κ
36 Γενικά...
37 Γ
38 Τερματικό...
39 Τ
40 Soft Επαναφορά
41 S
42 Hard Επαναφορά
43 H
44 Επαναφορά
45 Ε
46 Επιλογές
47 π

$ Old Help menu (no longer used for CDE)

48 Overview
49 O
50 Tasks
51 T
52 Reference
53 R
54 On Item
55 O
56 Using Help
57 U
59 A
60 Help
61 H

62 Alt+F4

63 dtterm

$ Current CDE Help menu

64 Επισκόπηση
65 Ε
66 Ευρετήριο
67 ρ
68 Περιεχόμενα
69 Π
70 Ρόλοι
71 Ρ
72 Αναφορά
73 Α
74 Πληκτρολόγιο
75 λ
76 Χρήση Βοήθειας
77 Β
79 Α
80 Βοήθεια
81 θ
82 Περί του Τερματικού
83 Περί του Τερματικού

$set 9
$ ****** Module: TermTerminalDialog.c
$ These are the messages for the terminal options dialog
2 OK
3 Ακύρωση
4 Βοήθεια
5 Έλεγχος πληκτρολογίου
6 Κανονικό
7 Εφαρμογή
8 Κατάσταση δείκτη
9 Αριθμητικό
10 Εφαρμογή
11 Κατάσταση αριθμητικού πληκτρολογίου
12 Μόνο Return
13 Return/Line Feed
14 Ακολουθία νέας γραμμής
15 Κλειδωμένο
16 Ξεκλείδωτο
17 Συντομεύσεις πληκτρολογίου χρήστη
18 Έλεγχος οθόνης
19 Ενεργοποιημένος
20 Απενεργοποιημένος
21 132 Αλλαγή στήλης
22 Αναδίπλωση τέλους γραμμής
23 Αντίστροφη αναδίπλωση τέλους γραμμής
24 Εφαρμογή
25 Επαναφορά
26 Τερματικό - Επιλογές τερματικού

$set 10
$ ****** Module: sunDtTermServer.c
$ These are the messages for the ToolTalk error dialog
1 Τερματικό - Σφάλμα
2 "Δεν ήταν δυνατή η σύνδεση με τον εξυπηρετητή ToolTalk:\n%s\nΈξοδος ..."
3 OK
