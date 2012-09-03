$ +SNOTICE
$ 
$ 	$TOG: libDtMail.msg /main/18 1999/03/26 16:46:04 mgreess $
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
1 "No se produjo ningún error."

$ // DTME_AlreadyLocked
2 "La carpeta está bloqueada por otra sesión."

$ // DTME_BadArg
3 "Se envió un argumento incorrecto como parámetro de la operación."

$ // DTME_BadMailAddress
4 "No se pudo procesar la dirección de correo especificada."

$ // DTME_BadRunGroup
5 "El programa dtmail no se está ejecutando como grupo \"mail\"."

$ // DTME_FileCreateFailed
6 "No se pudo crear el archivo solicitado."

$ // DTME_FolderNotSet
7 "No se configuró el buzón para incorporar."

$ // DTME_GetLockRefused
8 "El usuario se ha negado a tomar el bloqueo de un buzón bloqueado."

$ // DTME_ImplFailure
9 "La implementación especificada no pudo efectuar la operación solicitada."

$ // DTME_InitializationFailed
10 "No se pudo inicializar el caso."

$ // DTME_InternalFailure
11 "Se produjo un fallo interno durante la realización de la operación."

$ // DTME_InvalidError
12 "La estructura del error no es válida."

$ // DTME_InvalidOperationSequence
13 "Se ha intentado realizar una operación antes de inicializar el caso."

$ // DTME_MD5ChecksumFailed
14 "La identificación de MD5 no corresponde al contenido del mensaje."

$ // DTME_MailTransportFailed
15 "No se puede comunicar con el transporte de correo."

$ // DTME_NoDataType
16 "No hay ningún tipo de datos que corresponda al tipo de transporte."

$ // DTME_NoImplementations
17 "No se encontró ninguna implementación para la biblioteca de correo."

$ // DTME_NoMailBox
18 "El buzón no estaba inicializado al cargar."

$ // DTME_NoMemory
19 "No hay memoria disponible para la operación."

$ // DTME_NoMsgCat
20 "No existe ningún catálogo de mensajes para DtMail."

$ // DTME_NoNewMail
21 "No hay nuevo correo para incorporar."

$ // DTME_NoObjectValue
22 "No se pudo encontrar ningún valor para el objeto."

$ // DTME_NoSuchFile
23 "El buzón no existe y no se solicitó la creación."

$ // DTME_NoSuchImplementation
24 "La implementación especificada no existe."

$ // DTME_NoSuchType
25 "La biblioteca desconoce el tipo de datos."

$ // DTME_NoUser
26 "No se pudo identificar el usuario de esta sesión."

$ // DTME_NotInbox
27 "El archivo especificado no es un buzón."

$ // DTME_NotLocked
28 "El acceso al buzón no está bloqueado."

$ // DTME_NotMailBox
29 "El archivo solicitado no es un buzón de ningún formato reconocido por esta implementación."

$ // DTME_NotSupported
30 "La operación no está soportada por la implementación actual."

$ // DTME_ObjectAccessFailed
31 "No se pudo obtener acceso a un objeto necesario para completar la operación."

$ // DTME_ObjectCreationFailed
32 "No se pudo crear un objeto necesario para completar la operación."

$ // DTME_ObjectInUse
33 "Se ha intentado inicializar un objeto que ya estaba inicializado."

$ // DTME_ObjectInvalid
34 "Se ha hecho referencia a un objeto no válido."

$ // DTME_OpenContainerInterrupted
35 "El usuario ha interrumpido el proceso de apertura de un buzón."

$ // DTME_OperationInvalid
36 "Se produjo un error interno durante la realización de la operación."

$ // DTME_OtherOwnsWrite
37 "Otro programa de correo es propietario del acceso de escritura del buzón."

$ // DTME_RequestDenied
38 "La otra sesión denegó la solicitud de bloqueo o copia."

$ // DTME_TTFailure
39 "No se pudo procesar un mensaje ToolTalk."

$ // DTME_TransportFailed
40 "Ha fallado el transporte de entrega de correo."

$ // DTME_UnknownFormat
41 "El mensaje no se encuentra en un formato soportado."

$ // DTME_UnknownOpenError
42 "Se produjo un error desconocido al abrir el buzón."

$ // DTME_UserAbort
43 "El usuario ha cancelado la operación."

$ // DTME_UserInterrupted
44 "El usuario ha interrumpido la operación."

$ // DTME_ObjectReadOnly
45 "Los permisos de buzón solamente autorizan acceso para leer."

$ // DTME_NoPermission
46 "El usuario no cuenta con acceso al buzón."

$ // DTME_IsDirectory
47 "La ruta especificada es un directorio."

$ // DTME_CannotRemoveStaleMailboxLockFile
48 "No se puede bloquear el buzón (no se pudo quitar el archivo de bloqueo dañado).\nArchivo de bloqueo dañado: %s\nCausa: %s"

$ // DTME_CannotCreateMailboxLockFile
49 "No se puede bloquear el buzón (no se pudo crear el archivo de bloqueo).\nArchivo de bloqueo del buzón: %s\nCausa: %s"

$ // DTME_CannotCreateMailboxLockFile_NoPermission
50 "No se puede crear el archivo de bloqueo ni bloquear el buzón debido a que el usuario no cuenta con\nacceso al buzón ni al directorio que lo contiene."

$ // DTME_CannotCreateMailboxLockFile_IsDirectory
51 "No se puede crear el archivo de bloqueo ni bloquear el buzón debido a que el nombre del\narchivo de bloqueo del buzón ya existe y es un directorio."

$ // DTME_CannotCreateMailboxLockFile_NoSuchFile
52 "No se puede crear el archivo de bloqueo ni bloquear el buzón debido a que un componente del\nnombre de ruta del archivo de bloqueo no es un directorio."

$ // DTME_CannotCreateMailboxLockFile_RemoteAccessLost
53 "No se puede crear el archivo de bloqueo ni bloquear el buzón debido a que no se puede obtener acceso\nal sistema remoto en el que se debería crear el archivo de bloqueo."

$ // DTME_CannotObtainInformationOnOpenMailboxFile
54 "No se puede obtener información acerca del archivo de buzón actual.\nArchivo de buzón: %s\nCausa: %s"

$ // DTME_CannotCreateTemporaryMailboxFile
55 "No se puede crear un archivo de buzón temporal.\nArchivo de buzón temporal: %s\nCausa: %s"

$ // DTME_CannotCreateTemporaryMailboxFile_NoPermission
56 "No se puede crear un archivo de buzón temporal debido a que el usuario\nno cuenta con acceso al buzón o al directorio que lo contiene."

$ // DTME_CannotCreateTemporaryMailboxFile_IsDirectory
57 "No se puede crear el archivo de buzón temporal debido a que el nombre\ndel buzón temporal ya existe y es un directorio."

$ // DTME_CannotCreateTemporaryMailboxFile_NoSuchFile
58 "No se puede crear el archivo de buzón temporal debido a que un componente del\nnombre de ruta del archivo temporal no es un directorio."

$ // DTME_CannotCreateTemporaryMailboxFile_RemoteAccessLost
59 "No se puede crear el archivo de buzón temporal debido a que ya no se puede obtener acceso\nal sistema remoto en el que se deseaba crear el archivo."

$ // DTME_CannotSetPermissionsOfTemporaryMailboxFile
60 "No se pueden configurar permisos en el archivo de buzón temporal.\nArchivo de buzón temporal: %s\nPermisos solicitados: %o\nCausa: %s"

$ // DTME_CannotSetOwnerOfTemporaryMailboxFile
61 "No se puede configurar el propietario del archivo de buzón temporal.\nArchivo de buzón temporal: %s\nIdentificación de usuario del propietario solicitada: %d\nCausa: %s"

$ // DTME_CannotSetGroupOfTemporaryMailboxFile
62 "No se puede configurar el grupo del archivo de buzón temporal..\nArchivo de buzón temporal: %s\nIdentificación de grupo solicitada: %d\nCausa: %s"

$ // DTME_CannotWriteToTemporaryMailboxFile
63 "No se puede escribir en el archivo de buzón temporal.\nArchivo de buzón temporal: %s\nCausa: %s"

$ // DTME_CannotWriteToTemporaryMailboxFile_ProcessLimitsExceeded
64 "No se puede escribir en un archivo de buzón temporal debido a que se ha llegado\nal límite de tamaño del archivo del proceso o al tamaño máximo de archivo."

$ // DTME_CannotWriteToTemporaryMailboxFile_RemoteAccessLost
65 "No se puede escribir el archivo de buzón temporal debido a que ya no se puede\nobtener acceso al sistema en el que se ha creado el archivo."

$ // DTME_CannotWriteToTemporaryMailboxFile_NoFreeSpaceLeft
66 "No se puede escribir en el archivo de buzón temporal debido a que no queda espacio libre\nen el dispositivo en el que se ha creado el archivo."

$ // DTME_CannotReadNewMailboxFile
67 "No se puede leer el nuevo archivo de buzón\nCausa: %s"

$ // DTME_CannotReadNewMailboxFile_OutOfMemory
68 "No se puede leer el nuevo archivo de buzón debido a que no queda memoria disponible para la operación."

$ // DTME_CannotRemoveMailboxLockFile
69 "No se puede desbloquear el buzón (no se puede quitar el archivo de bloqueo).\nArchivo de bloqueo de buzón: %s\nCausa: %s"

$ // DTME_CannotRenameNewMailboxFileOverOld
70 "No se puede cambiar el nombre del viejo archivo de buzón por el nuevo.\nEl archivo de buzón viejo todavía existe pero el contenido correcto y completo\ndel buzón se ha guardado en el nuevo archivo de buzón.\nEste problema debe corregirse manualmente lo antes posible.\nArchivo de buzón viejo: %s\nArchivo de buzón nuevo: %s\nCausa: %s"

$ // DTME_InternalAssertionFailure
71 "Se ha producido un error interno en la aplicación.\nNo es posible recuperarse de este error y continuar.\nSituación de error: %s\n"

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
