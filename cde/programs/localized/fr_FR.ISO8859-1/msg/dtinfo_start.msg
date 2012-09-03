$ $TOG: dtinfo_start.msg /main/3 1999/09/20 13:27:59 mgreess $
$ 
$ (c) Copyright 1996 Digital Equipment Corporation.
$ (c) Copyright 1996 Hewlett-Packard Company.
$ (c) Copyright 1996 International Business Machines Corp.
$ (c) Copyright 1996 Sun Microsystems, Inc.
$ (c) Copyright 1996 Novell, Inc. 
$ (c) Copyright 1996 FUJITSU LIMITED.
$ (c) Copyright 1996 Hitachi.
$ 
$ ******************************************************************************
$ 
$set 1
$ The messages in this set will only appear in the users
$   "errorlog" file.
$ 
$ ******************************************************************************
$ 
$ Messages 1-5: These messages result from an error in a tt_* function
$    call.
$ 
1 Le processus de la session tt n'est pas lancé et le service ToolTalk\n\
ne peut pas le relancer.
2 L'identificateur courant du processus par défaut est périmé ou incorrect.
3 La session ToolTalk sécifiée est périmée ou incorrecte.

$ 
$ Message 4: Do not localize the word DtInfo_Start.
$ 
4 Le type de processus 'DtInfo_Start' n'est pas un \n\
de ceux qui sont installés.

5 Le numéro de l'erreur n'est pas identifié.

6 Impossible d'appeler une action parce que malloc(1) a échoué.

$ 
$ Message 7: Do not localize the word DISPLAY.  Do not localize the
$   word XtOpenDisplay().
$ 
7 XtOpenDisplay() a échoué. La variable d'environnement DISPLAY\n\
est incorrecte ou elle n'a pas été définie.

$ 
$ Message 8: Do not localize the word DtAppInitialize().
$ 
8 DtAppInitialize() a échoué. Vérifiez que l'environnement\n\
de bureau est installé correctement.

$ 
$ Message 9: Do NOT localize the word DTINFOLIBDEFAULT
$ 
9 Le message'%s' message ne contient pas de Biblinfo.\n\
On utilisera la valeur de la variable d'environnement DTINFOLIBDEFAULT\n\
(%s) pour remplacer Biblinfo.

$ 
$ Message 10: Do NOT localize the word DTINFOLIBDEFAULT
$ 
10 Le message  '%s' ne contient pas de Biblinfo \n\
et la variable d'environnement DTINFOLIBDEFAULT n'a pas été définie.

$ 
$ Messages 11-16: These message are the result of the dtinfo_start
$   getting an error while trying to parse a ToolTalk message.
$ 
$   Do NOT localize the word NULL
$   
11 L'opération de message '%d' n'est pas prise en charge.

12 Le message  '%s' ne contient pas le nombre d'arguments\n\
nécessaire '%d'.

13 Le message '%s' a spécifié le nom d'une action de valeur NULL\n\
L'action '%s' sera appelée.

14 Le message '%s' a spécifié un hôte d'exécution de valeur NULL.

15 Le message '%s' a spécifié un environnement du pays de valeur NULL.

16 Le message '%s' a spécifié un sujet de valeur NULL.

$set 2
$ *****************************************************************************
$ **
$ ** Message set for  DieFromToolTalkError Dialog
$ **
$ *****************************************************************************
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Dtinfo: Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
$ "-NEW"
3 OK
