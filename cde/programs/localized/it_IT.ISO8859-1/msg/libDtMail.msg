$ +SNOTICE
$ 
$ 	$TOG: libDtMail.msg /main/20 1999/03/26 16:47:37 mgreess $
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
1 "Nessun errore."

$ // DTME_AlreadyLocked
2 "La cartella è stata bloccata da un'altra sessione."

$ // DTME_BadArg
3 "Un argomento incorretto è stato riferito come parametro all'operazione."

$ // DTME_BadMailAddress
4 "Non è stato possibile elaborare l'indirizzo postale specificato."

$ // DTME_BadRunGroup
5 "Il programma dtmail non è in esecuzione come gruppo \"mail\"."

$ // DTME_FileCreateFailed
6 "Impossibile creare il file specificato."

$ // DTME_FolderNotSet
7 "La casella postale da incorporare non è stata impostata."

$ // DTME_GetLockRefused
8 "L'utente non ha sbloccato la casella postale."

$ // DTME_ImplFailure
9 "L'implementazione specificata non è in grado di eseguire l'operazione specificata."

$ // DTME_InitializationFailed
10 "Impossibile inizializzare l'istanza."

$ // DTME_InternalFailure
11 "Errore interno durante l'operazione."

$ // DTME_InvalidError
12 "La struttura errori non è valida."

$ // DTME_InvalidOperationSequence
13 "Occorre inizializzare l'istanza prima dell'operazione"

$ // DTME_MD5ChecksumFailed
14 "La specifica di identificazione MD5 non corrisponde ai contenuti del messaggio."

$ // DTME_MailTransportFailed
15 "Impossibile communicare con il trasporto posta"

$ // DTME_NoDataType
16 "Non c'è un tipo di dati che corrisponde al tipo di trasporto"

$ // DTME_NoImplementations
17 "Per la libreria della posta non è stata trovata alcuna implementazione."

$ // DTME_NoMailBox
18 "Casella postale non inizializzata al tempo di carico"

$ // DTME_NoMemory
19 "Memoria insufficiente per l'operazione."

$ // DTME_NoMsgCat
20 "Non esiste un catalogo di messaggi per DtMail."

$ // DTME_NoNewMail
21 "Non ci sono messaggi nuovi da incorporare."

$ // DTME_NoObjectValue
22 "Impossibile trovare un valore per l'oggetto."

$ // DTME_NoSuchFile
23 "La casella postale non esiste e la sua creazione non è stata richiesta."

$ // DTME_NoSuchImplementation
24 "L'implementazione specificata non esiste."

$ // DTME_NoSuchType
25 "La libreria non riconosce questo tipo di dati."

$ // DTME_NoUser
26 "Impossibile identificare l'utente per questa sessione."

$ // DTME_NotInbox
27 "Il file specificato non è una casella postale."

$ // DTME_NotLocked
28 "l'accesso alla casella non è bloccato"

$ // DTME_NotMailBox
29 "Il file richiesto non è una casella postale riconoscibile"

$ // DTME_NotSupported
30 "L'implementazione corrente non supporta l'operazione richiesta."

$ // DTME_ObjectAccessFailed
31 "Impossibile accedere ad un oggetto obbligatorio."

$ // DTME_ObjectCreationFailed
32 "Impossibile creare un oggetto obbligatorio."

$ // DTME_ObjectInUse
33 "Non è possibile inizializzare un oggetto già inizializzato."

$ // DTME_ObjectInvalid
34 "È stato specificato un oggetto non valido"

$ // DTME_OpenContainerInterrupted
35 "Apertura di casella postale interrotta dall'utente."

$ // DTME_OperationInvalid
36 "Errore interno durante l'operazione"

$ // DTME_OtherOwnsWrite
37 "L'autorizzazione di scrittura appartiene a un altro programma di gestione posta."

$ // DTME_RequestDenied
38 "La richiesta di blocco o di copia è stata negata dall'altra sessione."

$ // DTME_TTFailure
39 "Impossibile elaborare un messaggio ToolTalk."

$ // DTME_TransportFailed
40 "Fallimento del trasporto messaggi."

$ // DTME_UnknownFormat
41 "Il messaggio non è di un formato supportato."

$ // DTME_UnknownOpenError
42 "Errore sconosciuto durante l'apertura della casella postale."

$ // DTME_UserAbort
43 "Operazione terminata dall'utente."

$ // DTME_UserInterrupted
44 "Operazione interrotta dall'utente."

$ // DTME_ObjectReadOnly
45 "Questa casella postale è di sola lettura."

$ // DTME_NoPermission
46 "L'utente non ha l'accesso alla casella postale."

$ // DTME_IsDirectory
47 "Il percorso specificato è una directory."

$ // DTME_CannotRemoveStaleMailboxLockFile
48 "Impossibile bloccare la casella postale (eliminare un vecchio file di blocchi).\nVecchio file: %s\nCausa: %s"

$ // DTME_CannotCreateMailboxLockFile
49 "Impossibile bloccare la casella postale (creare il file di blocchi).\nFile di blocchi: %s\nCausa: %s"

$ // DTME_CannotCreateMailboxLockFile_NoPermission
50 "Impossibile creare un file per bloccare la casella. L'utente non ha\nl'accesso alla casella o alla directory."

$ // DTME_CannotCreateMailboxLockFile_IsDirectory
51 "Impossibile creare un file per bloccare la casella. Il nome del file di blocchi \nesiste già ed è una directory."

$ // DTME_CannotCreateMailboxLockFile_NoSuchFile
52 "Impossibile creare un file per bloccare la casella. Un componente del percorso\n non è una directory."

$ // DTME_CannotCreateMailboxLockFile_RemoteAccessLost
53 "Impossibile creare un file per bloccare la casella. Il sistema remoto \ndove si voleva creare il file non è accessibile."

$ // DTME_CannotObtainInformationOnOpenMailboxFile
54 "Impossibile accedere alle informazioni sul file di casella postale corrente.\nFile casella postale: %s\nCausa: %s"

$ // DTME_CannotCreateTemporaryMailboxFile
55 "Impossibile creare il file temporaneo di casella postale.\nFile temporaneo: %s\nCausa: %s"

$ // DTME_CannotCreateTemporaryMailboxFile_NoPermission
56 "Impossibile creare il file temporaneo di casella postale\nl'utente non ha accesso alla casella o alla directory che la contiene."

$ // DTME_CannotCreateTemporaryMailboxFile_IsDirectory
57 "Impossibile creare il file temporaneo di casella postale: \nil nome specificato per la casella esiste già ed è una directory."

$ // DTME_CannotCreateTemporaryMailboxFile_NoSuchFile
58 "Impossibile creare il file temporaneo di casella postale:\nun componente del percorso del file temporaneo non è una directory."

$ // DTME_CannotCreateTemporaryMailboxFile_RemoteAccessLost
59 "Impossibile creare il file temporaneo di casella postale. Il sistema remoto dove il \nfile è stato creato non è ancora disponibile."

$ // DTME_CannotSetPermissionsOfTemporaryMailboxFile
60 "Impossibile impostare le autorizzazioni del file temporaneo di casella postale.\nFile temporaneo: %s\nAutorizzazioni specificate: %o\nCausa: %s"

$ // DTME_CannotSetOwnerOfTemporaryMailboxFile
61 "Impossibile impostare il proprietario del file temporaneo di casella postale.\nFile temporaneo: %s\nUID di proprietario specificato: %d\nCausa: %s"

$ // DTME_CannotSetGroupOfTemporaryMailboxFile
62 "Impossibile impostare il gruppo del file temporaneo di casella postale.\nFile temporaneo: %s\nGID di gruppo specificato: %d\nCausa: %s"

$ // DTME_CannotWriteToTemporaryMailboxFile
63 "Impossibile scrivere nel file temporaneo di casella postale.\nFile temporaneo: %s\nCausa: %s"
$ // DTME_CannotWriteToTemporaryMailboxFile_ProcessLimitsExceeded
64 "Impossibile scrivere nel file temporaneo di casella postale. Il limite della dimensione file del processo e/o la dimensione di file massima sono stati raggiunti."

$ // DTME_CannotWriteToTemporaryMailboxFile_RemoteAccessLost
65 "Impossibile scrivere nel file temporaneo di casella postale. Non c'è spazio disponibile\nnel sistema remoto dove il file è stato creato."

$ // DTME_CannotWriteToTemporaryMailboxFile_NoFreeSpaceLeft
66 "Impossibile scrivere nel file temporaneo di casella postale. Non c'è spazio disponibile\nnell'unità dove il file è stato creato."

$ // DTME_CannotReadNewMailboxFile
67 "Impossibile leggere il nuovo file di casella postale.\nCausa: %s"

$ // DTME_CannotReadNewMailboxFile_OutOfMemory
68 "Impossibile leggere il nuovo file di casella postale.\nMemoria insufficiente per l'operazione."

$ // DTME_CannotRemoveMailboxLockFile
69 "Impossibile sbloccare la casella postale (togliere il file di blocco).\nFile di blocco: %s\nCausa: %s"

$ // DTME_CannotRenameNewMailboxFileOverOld
70 "Impossibile rinominare la nuova casella postale. Il vecchio file di casella postale esiste ancora ma\ni contenuti aggiornati e completi sono stati salvati nella casella nuova.\nCorreggere questo problema manualmente al più presto possibile.\nVecchio file: %s\nNuovo file: %s\nCausa: %s"

$ // DTME_InternalAssertionFailure
71 "Errore interno dell'applicazione. Non è possibile ripristinare e continuare.\nErrore: %s\n"

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
