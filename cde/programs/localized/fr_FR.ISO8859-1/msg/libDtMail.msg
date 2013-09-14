$ +SNOTICE
$ 
$ 	$TOG: libDtMail.msg /main/18 1999/03/26 16:46:50 mgreess $
$ 
$ 	RESTRICTED CONFIDENTIAL INFORMATION:
$ 	
$ 	The information in this document is subject to special
$ 	restrictions in a confidential disclosure agreement bertween
$ 	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
$ 	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
$ 	Sun's specific written approval.  This documment and all copies
$ 	and derivative works thereof must be returned or destroyed at
$ 	Sun's request.
$ 
$ 	Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
$ 
$ +ENOTICE
$ @(#)libDtMail.msg	1.9 23 May 1995

$quote "

$set 1 libDtMail API set
$ // DTME_NoError
1 "Aucune erreur n'a eu lieu."

$ // DTME_AlreadyLocked
2 "Le dossier est verrouillé par une autre session."

$ // DTME_BadArg
3 "Un argument incorrect a remplacé un paramètre de l'opération."

$ // DTME_BadMailAddress
4 "Impossible de traiter l'adresse de courrier spécifiée."

$ // DTME_BadRunGroup
5 "Le programme dtmail n'est pas exécuté en tant que groupe \"mail\"."

$ // DTME_FileCreateFailed
6 "Création du fichier demandé impossible."

$ // DTME_FolderNotSet
7 "La boîte aux lettres à incorporer n'est pas définie."

$ // DTME_GetLockRefused
8 "Refus de l'utilisateur de retirer le verrou d'une boîte aux lettres verrouillée."

$ // DTME_ImplFailure
9 "Impossible à l'implémentation spécifiée d'exécuter l'opération demandée."

$ // DTME_InitializationFailed
10 "Impossible d'initialiser l'instance."

$ // DTME_InternalFailure
11 "Une panne interne s'est produite au cours de l'opération."

$ // DTME_InvalidError
12 "La structure de l'erreur est incorrecte."

$ // DTME_InvalidOperationSequence
13 "Tentative d'opération avant l'initialisation de l'instance."

$ // DTME_MD5ChecksumFailed
14 "La signature MD5 n'est pas compatible avec le contenu du message."

$ // DTME_MailTransportFailed
15 "Communication impossible avec le transport du courrier."

$ // DTME_NoDataType
16 "Aucun type de données compatible avec le type de transport."

$ // DTME_NoImplementations
17 "Impossible de trouver des implémentations dans la bibliothèque de courrier."

$ // DTME_NoMailBox
18 "Initialisation de la boîte aux lettres au moment du chargement."

$ // DTME_NoMemory
19 "Pas de mémoire disponible pour l'opération."

$ // DTME_NoMsgCat
20 "Le catalogue de messages de DtMail n'existe pas."

$ // DTME_NoNewMail
21 "Pas de nouveau courrier à incorporer."

$ // DTME_NoObjectValue
22 "Valeur de l'objet introuvable."

$ // DTME_NoSuchFile
23 "La boîte aux lettres n'existe pas et vous n'avez pas demandé sa création."

$ // DTME_NoSuchImplementation
24 "L'implémentation spécifiée n'esiste pas."

$ // DTME_NoSuchType
25 "Le type de données n'existe pas dans la bibliothèque."

$ // DTME_NoUser
26 "Impossible d'identifier l'utilisateur de la session."

$ // DTME_NotInbox
27 "Le fichier spécifié n'est pas une boîte aux lettres."

$ // DTME_NotLocked
28 "L'accès de la boîte aux lettres n'est pas verrouillé."

$ // DTME_NotMailBox
29 "Le fichier demandé n'est pas une boîte aux lettres d'un format connu dans cette implémentation."

$ // DTME_NotSupported
30 "L'implémentation en cours ne prend pas en charge cette opération."

$ // DTME_ObjectAccessFailed
31 "Impossible d'accéder à un objet indispensable à l'opération."

$ // DTME_ObjectCreationFailed
32 " Impossible de créer un objet indispensable à l'opération."

$ // DTME_ObjectInUse
33 "Tentative d'initialisation d'un objet déjà initialisé."

$ // DTME_ObjectInvalid
34 "Vous avez fait référence à un objet incorrect."

$ // DTME_OpenContainerInterrupted
35 "L'utilisateur a interrompu l'ouverture de la boîte aux lettres."

$ // DTME_OperationInvalid
36 "Une erreur interne a eu lieu au cours de l'exécution de l'opération."

$ // DTME_OtherOwnsWrite
37 "Le droit en écriture de la boîte aux lettres appartient à un autre programme de courrier."

$ // DTME_RequestDenied
38 "L'autre session a refusé la demande de verrou ou de copie."

$ // DTME_TTFailure
39 "Impossible de traiter un messageToolTalk."

$ // DTME_TransportFailed
40 "Le transport de distribution de courrier a échoué."

$ // DTME_UnknownFormat
41 "Le message ne fait pas partie d'un format pris en charge."

$ // DTME_UnknownOpenError
42 "Une erreur inconnue a eu lieu lors de l'ouverture de la boîte aux lettres."

$ // DTME_UserAbort
43 "L'utilisateur a abandonné l'opération."

$ // DTME_UserInterrupted
44 "L'utilisateur a interrompu l'opération."

$ // DTME_ObjectReadOnly
45 "Les autorisations de la boîte aux lettres sont en lecture seule."

$ // DTME_NoPermission
46 "L'utilisateur n'a pas d'autorisation d'accès à la boîte aux lettres."

$ // DTME_IsDirectory
47 "Le chemin spécifié est un répertoire."

$ // DTME_CannotRemoveStaleMailboxLockFile
48 "Impossible de verrouiller la boîte aux lettres (impossible de supprimer le fichier ancien verrou).\nFichier Ancien verrou : %s\nRaison : %s"

$ // DTME_CannotCreateMailboxLockFile
49 "Impossible de verrouiller la boîte aux lettres (impossible de créer un fichier verrou).\nMailbox lock file : %s\nRaison : %s"

$ // DTME_CannotCreateMailboxLockFile_NoPermission
50 "Impossible de créer un fichier verrou et de verrouiller la boîte aux lettres parce que l'utilisateur \nne peut accéder ni à la boîte aux lettres ni au répertoire qui la contient "

$ // DTME_CannotCreateMailboxLockFile_IsDirectory
51 "Impossible de créer un fichier verrou et de verrouiller la boîte aux lettres parce qu'il \nexiste déjà un répertoire de même nom."

$ // DTME_CannotCreateMailboxLockFile_NoSuchFile
52 "Impossible de créer un fichier verrou et de verrouiller la boîte aux lettres parce que l'un\ndes composants de son chemin n'est pas un répertoire."

$ // DTME_CannotCreateMailboxLockFile_RemoteAccessLost
53 " Impossible de créer un fichier verrou et de verrouiller la boîte aux lettres parce que \nvous n'avez plus accès au système éloigné sur lequel le fichier a été créé."

$ // DTME_CannotObtainInformationOnOpenMailboxFile
54 "Impossible d'obtenir des informations au sujet du fichier boîte aux lettres courant.\nFichier boîte aux lettres : %s\nRaison : %s"

$ // DTME_CannotCreateTemporaryMailboxFile
55 "Impossible de créer un fichier boîte aux lettres temporaire.\nFichier boîte aux lettres temporaire : %s\nRaison : %s"

$ // DTME_CannotCreateTemporaryMailboxFile_NoPermission
56 " Impossible de créer un fichier boîte aux lettres temporaire parce que l'utilisateur ne peut accéder \nni à la boîte aux lettres ni au répertoire qui la contient."

$ // DTME_CannotCreateTemporaryMailboxFile_IsDirectory
57 " Impossible de créer un fichier boîte aux lettres temporaire parce qu'il existe déjà\nun répertoire du même nom."

$ // DTME_CannotCreateTemporaryMailboxFile_NoSuchFile
58 " Impossible de créer un fichier boîte aux lettres temporaire parce que l'un\ndes composants de son chemin n'est pas un répertoire."

$ // DTME_CannotCreateTemporaryMailboxFile_RemoteAccessLost
59 " Impossible de créer un fichier boîte aux lettres temporaire parce que \nvous n'avez plus accès au système éloigné sur lequel le fichier a été créé."

$ // DTME_CannotSetPermissionsOfTemporaryMailboxFile
60 "Impossible de définir les autorisations du fichier boîte aux lettres temporaire.\nFichier boîte aux lettres temporaire : %s\nAutorisations demandées : %o\nRaison : %s"

$ // DTME_CannotSetOwnerOfTemporaryMailboxFile
61 "Impossible d'établir l'utilisateur du fichier boîte aux lettres.\nFichier boîte aux lettres temporaire : %s\nUid du propriétaire demandée : %d\nRaison: %s"

$ // DTME_CannotSetGroupOfTemporaryMailboxFile
62 "Impossible d'établir un groupe de fichiers boîte aux lettres temporaire.\nFichier boîte aux lettres temporaire : %s\nGid du groupe demandé : %d\nRaison: %s"

$ // DTME_CannotWriteToTemporaryMailboxFile
63 "Pas de permission en écriture dans le fichier boîte aux lettres temporaire.\nFichier boîte aux lettres temporaire : %s\nRaison : %s"

$ // DTME_CannotWriteToTemporaryMailboxFile_ProcessLimitsExceeded
64 " Pas de permission en écriture dans le fichier boîte aux lettres temporaire parce que\nla limite de taille ou la taille maximale a été atteinte."

$ // DTME_CannotWriteToTemporaryMailboxFile_RemoteAccessLost
65 " Pas de permission en écriture dans le fichier boîte aux lettres temporaire parce que\nvous n'avez plus accès au système éloigné sur lequel il a été créé."

$ // DTME_CannotWriteToTemporaryMailboxFile_NoFreeSpaceLeft
66 " Pas de permission en écriture dans le fichier boîte aux lettres temporaire parce qu'il n'y a plus\nd'espace disponible sur le périphérique sur lequel il a été créé."

$ // DTME_CannotReadNewMailboxFile
67 "Impossible de lire le nouveau fichier boîte aux lettres\nRaison : %s"

$ // DTME_CannotReadNewMailboxFile_OutOfMemory
68 " Impossible de lire le nouveau fichier boîte aux lettres parce que la mémoire est insuffisante pour l'opération"

$ // DTME_CannotRemoveMailboxLockFile
69 "Impossible de déverrouiller (Impossible de supprimer le fichier verrou).\nFichier verrou boîte aux lettres : %s\nRaison : %s"

$ // DTME_CannotRenameNewMailboxFileOverOld
70 "Impossible de renommer le nouveau fichier boîte aux lettres comme l'ancien.\nCe dernier existe toujours mais le contenu complet et correct de la boîte aux lettres a été sauvegardé\ndans le nouveau fichier.\nVous devez rectifier ceci manuellement aussitôt que possible.\nFichier boîte aux lettres ancien : %s\nNouveau fichier boîte aux lettres : %s\nRaison : %s"

$ // DTME_InternalAssertionFailure
71 "Une erreur interne s'est produite au cours de l'application.\nIl est impossible de continuer.\nCondition d'erreur : %s\n"

$ "-NEW"
$ // - DTME_ResourceParsingNoEndif
$ 
72 "An error occurred while parsing the .mailrc resource file.\nThere is a conditional if statement that does not have a corresponding endif statement.\n"

$ "-NEW"
$ // - DTME_AlreadyOpened
$ 
73 "This mail folder is already opened."

$ "-NEW"
$ // - DTME_OutOfSpace
$ 
74 "No Space on Temporary Filesystem."

$ "-NEW"
$ // - DTME_CannotCreateMailboxDotDtmailLockFile
75 "Mailer has detected a mailbox lockfile:\n%s"

$ "-NEW"
$ // - DTME_MailboxInodeChanged
76 "Mailer can no longer access this mailbox.\nIt would be best to close and reopen it."

$ "-NEW"
$ // - DTME_MailServerAccess_AuthorizationFailed
77 "Cannot retrieve mail for '%s@%s' using '%s'.\nAuthorization failed.  The server returned:\n     %s\n\nPlease enter a new password"

$ "-NEW"
$ // - DTME_MailServerAccess_Error
78 "Cannot retrieve mail for '%s@%s' using '%s'.\nThe server returned:\n     %s"

$ "-NEW"
$ // - DTME_MailServerAccess_MissingPassword
79 "Please enter a password for '%s@%s' using '%s'"

$ "-NEW"
$ // - DTME_MailServerAccess_ProtocolViolation
80 "Cannot retrieve mail for '%s@%s' using '%s'.\nClient/server protocol error."

$ "-NEW"
$ // - DTME_MailServerAccess_ServerTimeoutError
81 "Cannot retrieve mail for '%s@%s' using '%s'.\nTimed out waiting for server."

$ "-NEW"
$ // - DTME_MailServerAccess_SocketIOError
82 "Cannot retrieve mail for '%s@%s' using '%s'.\n%s."

$ "-NEW"
$ // - DTME_AppendMailboxFile_Error
83 "Cannot append to mailbox"

$ "-NEW"
$ // - DTME_AppendMailboxFile_FileTooBig
84 "Cannot append to mailbox:  %s\nSYSERROR(%d):  %s."

$ "-NEW"
$ // - DTME_AppendMailboxFile_LinkLost
85 "Cannot append to mailbox:  %s\nSYSERROR(%d):  %s."

$ "-NEW"
$ // - DTME_AppendMailboxFile_NoSpaceLeft
86 "Cannot append to mailbox:  %s\nSYSERROR(%d):  %s."

$ "-NEW"
$ // - DTME_AppendMailboxFile_SystemError
87 "Cannot append to mailbox:  %s\nSYSERROR(%d):  %s."

$ "-NEW"
$ // - DTME_GetmailCommandRetrieval_SystemError
88 "Getmail command failed:  %s\nSYSERROR(%d):  %s."

$ "-NEW"
$ // - DTME_GetmailCommandRetrieval_AbnormalExit
89 "Getmail command exited abnormally:  %s."

$ "-NEW"
$ // - DTME_PathElementPermissions
90 "Search permission denied on a component of the path prefix,\n'%s'."

$ "-NEW"
$ // - DTME_PathElementNotDirectory
91 "A component of the path prefix is not a directory,\n'%s'."

$ "-NEW"
$ // - DTME_PathElementDoesNotExist
92 "A component of the path prefix does not exist,\n'%s'."

$ "-NEW"
$ // - DTME_MailServerAccessInfo_SocketOpen
93 "Opening connection for '%s@%s'."

$ "-NEW"
$ // - DTME_MailServerAccessInfo_NoMessages
94 "No messages for '%s@%s'."

$ "-NEW"
$ // - DTME_MailServerAccessInfo_RetrievingMessage
95 "Retrieving message %d of %d for '%s@%s'."

$ "-NEW"
$ // - DTME_MailServerAccessInfo_MessageTooLarge
96 "Skipping oversized message (%d bytes)."


$ 
$ Strings from libDtMail/Common/DtMailValuesBuiltin.C

$set 2
$ /* NL_COMMENT
$  * The %C is the time and date format, please refer to strftime man page for 
$  * explanation of each format.
$  */
1 "%C" 


$ 
$ SockOpen error strings from libDtMail/Common/IO.C

$ "-NEW"
2 "Unknown host:  %s"

$ "-NEW"
3 "Error creating socket: %s"

$ "-NEW"
4 "Error connecting to socket:  %s"


$ 
$ FileShare error strings from libDtMail/Common/FileShare.C

$ "-NEW"
5 "Another user would like your lock."

$ "-NEW"
6 "Another session has this mailbox locked. Request access?"

$ "-NEW"
7 "Unable to obtain lock, open this mailbox as read only?"

$ "-NEW"
8 "Unable to obtain lock because system not responding, open this mailbox as read only, read write, or cancel?"

$ "-NEW"
9 "Another user has taken your lock."
