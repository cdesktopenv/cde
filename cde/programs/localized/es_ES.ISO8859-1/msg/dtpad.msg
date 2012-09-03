$ $TOG: dtpad.msg /main/21 1999/09/15 14:20:17 mgreess $
$ **********************************<+>************************************* 
$ ************************************************************************** 
$ ** 
$ **  File:        dtpad.msg 
$ ** 
$ **  Project:     DT Text Editor client (sometimes called dtpad) 
$ ** 
$ **  Description: 
$ **  ----------- 
$ **  This file is the source for the message catalog for dtpad 
$ ** 
$ ** 
$ ************************************************************************** 
$ ** 
$ **     (c) Copyright 1992, 1993 Hewlett-Packard Company 
$ **     All Rights reserved 
$ ** 
$ ** 
$ ************************************************************************** 
$ ** 
$ ************************************************************************** 
$ **  (c) Copyright 1993, 1994 Hewlett-Packard Company 
$ **  (c) Copyright 1993, 1994 International Business Machines Corp. 
$ **  (c) Copyright 1993, 1994 Sun Microsystems, Inc. 
$ **  (c) Copyright 1993, 1994 Novell, Inc. 
$ ************************************************************************** 
$ ** 
$quote "

$set 2
$ ********************************************** 
$ **  General messages used in multiple files ** 
$ ********************************************** 
$ Label used for all "Help" buttons in all DIALOGS. 
$ It is not used for the Help menu. 

1 "Ayuda"

$quote

$ Unrecognized arguments
2 dtpad: opción no válida: %s 
$ Extra arguments
3 dtpad: argumento extra: %s


$set 4        Redefine set#

$ ***************************************************************** 
$ **                                                    fileCB.c ** 
$ ** These are the strings which change between the Open dialog  ** 
$ ** and the Include dialog.  These two operations use the same  ** 
$ ** Motif dialog, and simply change these two labels.           ** 
$ ***************************************************************** 
$ Title of the Open dialog 

$quote "
1 "Abrir un archivo"
$ label for the File text field in the file selection box 
$ ***UNUSED***\tThis label is no longer used and need not be localized. 

2 "Archivo para abrir:"
$ Title of the Include dialog 

3 "Incluir un archivo"
$ Label for the File text field in the file selection box 
$ ***UNUSED***\tThis label is no longer used and need not be localized. 

4 "Archivo para incluir:"
$ Error when printing a file and a temporary copy can not be saved. 

5 "No es posible crear un archivo temporal."

$set 5        Redefine set#

$ *************************************************************************** 
$ **                                                               dtpad.c ** 
$ **                                                             fileDlg.c ** 
$ **                                                                       ** 
$ ** Messages and strings used in dialogs for all operations involving the ** 
$ ** "File" menu items.                                                    ** 
$ *************************************************************************** 
$ These compose the dialog which appears if the user attempts to exit, open 
$ a different file, drop a file, or start a "New" session with unsaved 
$ changes to the current file. 
$ This is used to build: "Save changes to FILENAME?" 

1 "¿Guardar cambios en %s?"
$ This is used as-is, whenever the current file is unnamed. 

2 "El archivo actual no tiene nombre.\n\
¿Desea guardarlo?"
$ This is used to build a message like "Do you want to save the current 
$ copy of FILENAME before you Edit NEWFILENAME?" when a new file is droped 
$ on the edit window. 
$ ***UNUSED***\tCurrently, dropped files are INSERTED in the text rather than 
$ \topened for editing so this message is no longer used and need 
$ \tnot be localized. 

3 "¿Desea guardar la copia actual\n\
de %s\n\
antes de Editar %s?"
$ Used when about to write over an existing file. 
$ The "%s" is replaced by the file name. 

5 "El archivo %s ya existe.\n\
¿Desea escribir encima de dicho archivo\n\
el contenido de esta sesión de edición?"
$ Title of "file already exists" dialog.  Also used for other "Warning" type 
$ dialogs. 

6 "Aviso"
$ Title of "Save As" dialog. 

7 "Guardar como"
$ Labels for add/don't-add newlines radio buttons in the Save As dialog 
$ ***UNUSED***\tThese labels are no longer used and need not be localized. 
$ \tThey've been replaced by messages 44 and 45 

8 "Añada caracteres de nueva línea al final de las líneas acomodadas."

9 "No añada nuevas líneas.  Sólo las rupturas de línea creadas por\n\
[Intro] se conservarán."
$ Labels for the Save Warning dialog. Also used in the confirmation, or 
$ Editor Return dialog. 

14 "No"

15 "Sí"
$ Title of the dialog - used when user selects "New", "Open", or "Exit" with 
$ unsaved changes in the text. 

16 "¿Guardar?"
$ Title of general error dialog 

17 "Error"
$ Title of the resource-specified "confirmation" dialog 
$ This dialog is generally specified by programs such as mailers, which 
$ want to know if the user wants to send the message. 
$ There is no default message for the dialog, since it is _only_ posted if 
$ the message string is specified via a resource (dtpad*confirmationString:) 
$ This message is only used if the "dtpad*mainTitle:" resource is not set. 
$ If this resource _is_ set, the the mainTitle is also used as the title 
$ for the exit confirmation dialog. 
$ DO NOT LOCALIZE - the user should never see this message 

18 "Confirmación salir"
$ Title for general information dialog. e.g. Find dialog can't find word in 
$ file. 

20 "Información"
$ Default file name in Save As dialog box. 

21 "SIN TÍTULO"
$ Error message when a file can not be saved.  The "%1$s" gets replaced by 
$ the file name and the "%2$s" gets replaced by the reason why the file could 
$ not be saved (messages 31-39). 

30 "No se puede guardar %1$s.\n\
%2$s"
$ Messages 31-39 are reasons why a file could not be saved. 
$ Messages 34-39 are substitued in message 30 for "%2$s". 

31 "No es posible guardar el archivo.\n\
No se ha especificado el nombre del archivo."

34 "El archivo no permite grabar a nadie."

35 "El archivo es un directorio."

36 "El archivo es un dispositivo especial de caracteres."

37 "El archivo es un dispositivo de modo por bloques."
$ ***UNUSED***\tThis message should never appear since currently there is 
$ \tonly one supported file format - text.  And this type is 
$ \thard coded into dtpad. 

38 "Formato de archivo no válido."
$ This is a "catch all" when the file can not be saved and none of the above 
$ errors apply. 

39 "Razón desconocida - posiblemente no hay suficiente espacio de disco."
$ This is displayed when a ToolTalk edit request on a buffer is received, 
$ a Save operation is done and there are unsaved changes. 

40 "¿Guardar los cambios?"
$ This is displayed when a ToolTalk edit request on a file or buffer is 
$ received with a document name specified in the request, a Save operation 
$ is done and there are unsaved changes.  The "%s" in the message is 
$ replaced by the document name (instead of the filename). 

41 "¿Guardar los cambios en \"%s\"?"
$ Title of "Copy As" dialog which is substitued for the "Save As" dialog 
$ when file name changes are disallowd.  This should match the menu button 
$ label specified in message 108 of set 11. 
$ ***UNUSED***\tThis title is no longer used and need not be localized. 

42 "Copiar como"
$ Title of "Copy To File" dialog which is substitued for the "Save As" dialog 
$ when file name changes are disallowd.  This should match the menu button 
$ label specified in message 110 of set 11. 

43 "Copiar en archivo"
$ Labels for add/don't-add newlines radio buttons in the SaveAs and Save dialogs 

44 "Añada los caracteres de línea nueva al final de las líneas que se reinician."

45 "No añada líneas nuevas. Sólo se conservarán las divisiones de línea\n\
creadas por [Intro]."
$ New message as of 5/31/95 (i.e. new since Sample Implementation). This 
$ new message is in response to bug DTS CISlx19530.
$ 
46 "O bien no se puede escribir en el archivo o bien\nla ruta al archivo no existe. "

$ 
$ "-NEW"
$ 
47 "Operation failed due to insufficient memory.\nTry increasing swap space."

$ 
$ "-NEW"
$ 
48 "Format operation failed."

$set 6        Redefine set#

$ ************************************************************************ 
$ **                                                           fileIo.c ** 
$ **                                                                    ** 
$ ** Error messages generated when there are problems reading a file.   ** 
$ ************************************************************************ 

1 "Este archivo se ha abierto como de sólo lectura."

2 "No existe este archivo."

3 "No se puede editar un directorio."

4 "No se puede editar un archivo de dispositivo especial de caracteres."

5 "No se puede editar un archivo de dispositivo de modo por bloques."

$quote

6 Se han eliminado los Nulls incrustados del archivo.

7 No se puede cargar el archivo (no hay suficiente memoria).

8 No se puede obtener acceso al archivo (no se puede determinar su estado).

$quote "
9 "No se puede leer del archivo."

$ Message placed in the status line message area based on the -viewOnly 
$ resource and whether or not the file being edited is read only 

10 "Sólo ver"

11 "Sólo lectura"

$quote

12 No se puede leer el archivo (razón desconocida).


$set 7        Redefine set#

$ ************************************ 
$ **                         main.c ** 
$ **                        dtpad.c ** 
$ ** Prefix string for all dialogs. ** 
$ ************************************ 
$ bogus?? blank string gotten but otherwise unused in main() 
$ DO NOT LOCALIZE string 1 

$quote "
1 ""
$ The name used in the title bar of every window. 
$ Be sure to end this message with a space character. 

2 "Editor de textos -"
$ Default file name used in the main window title. 
$ ***UNUSED***\tThis mesage has been replaced by message 6 of this set and 
$ \tneed not be localized. 

3 "(sin nombre)"
$ Terminal error message displayed to stdout when the desk top services 
$ can not be initialized (via DtInitialize()) 

4 "No es posible inicializar los servicios de Escritorio ... se sale del Editor de textos."
$ Terminal error message displayed to stdout when a connection to the 
$ display can not be opend (via XtDisplay()) 

5 "No se puede abrir la pantalla ... se sale del Editor de textos."
$ Default file name used in the main window title. 
$ This should correspond to the default file name used in the "Save As" 
$ dialog box - specified in message 21 of set 5. 

6 "(SIN TÍTULO)"

$quote

7 ERROR: Un servidor dtpad en %s\n\
ya está serviendo esta pantalla.

8 ERROR: Un servidor dtpad ya está\n\
sirviendo esta pantalla.
$ 
$ Message 9 is also new since CDE/SI - new as of 5/22/95.  It is a usage message
$ for dtpad. Don't translate the '-OPTION' just the text explaining the
$ option.
$ 

9 '%s' es una opción incorrecta.\n\
Uso: %s...\n\
   -saveOnClose\n\
\tDtpad guarda automática y silenciosamente el texto actual cuando\n\
\thay modificaciones no guardadas y se cierra el Editor de textos.\n\n\
   -missingFileWarning\n\
\tMuestra un cuadro de diálogo de advertencia cuando se especifica el nombre de archivo y\n\
\téste no existe o no se puede obtener acceso al mismo.\n\n\
   -noReadOnlyWarning\n\
\tDesactiva el cuadro de diálogo de advertencia que aparece cuando\n\
\tse especifica un archivo para el cual el usuario no cuenta con permiso de acceso.\n\n\
   -noNameChange\n\
\tIndica que el nombre de archivo predeterminado correspondiente al\n\
\ttexto actual no se debe modificar cuando se guarda el texto bajo un\n\
\tnombre distinto del que se ha utilizado para leerlo.\n\n\
   -viewOnly\n\
\tDesactiva la edición de texto en la ventana de edición, es decir,\n\
\tconvierte el Editor de texto en un visor de texto.\n\n\
   -statusLine\n\
\tMuestra la línea de estado en la parte inferior de la ventana de edición.  La\n\
\tlínea de estado muestra el número de la línea donde se encuentra\n\
\tel cursor.\n\n\
   -wrapToFit\n\
\tInicialmente activa el modo de ajuste automático de texto.\n\n\
   -workspaceList WorkspaceList\n\
\tMuestra la ventana de edición del\n\
\tEditor de texto en las áreas de trabajo especificadas.\n\n\
   -session SessionFile\n

10 \tRestaura el Editor de texto en todas las ventanas de edición de texto y\n\
\tlas opciones que estaban en efecto antes de detener CDE anteriormente.  Se\n\
\tignoran todas las demás opciones de línea de comando cuando se\n\
\tespecifica esta opción.\n\n\
   -standAlone\n\
\tFuerza a la invocación del Editor de texto activa a\n\
\tefectuar su propio procesamiento de texto en su propia ventana, independientemente del\n\
\tservidor del Editor de texto.\n\n\
   -noBlocking\n\
\tDetiene la ejecución del solicitante del Editor de texto cuando\n\
\tel servidor del Editor de texto determina que puede procesar\n\
\tla solicitud de edición del solicitante.\n\n\
   -server\n\
\tFuerza a un servidor del Editor de texto a activarse (si no hay ninguno\n\
\ten ejecución) para procesar todas las solicitudes de edición subsiguientes de\n\
\tla pantalla.\n\n\
   -exitOnLastClose\n\
\tEspecifica que se debe detener el proceso del servidor del Editor de texto\n\
\tcuando se cierra la última ventana de edición de la pantalla.  Sólo\n\
\tse debe utilizar con la opción -server ya que\n\
\tsolamente corresponde al proceso del servidor.\n\n\
Vea la página manual dtpad para obtener información detallada adicional acerca de las funciones mencionadas anteriormente.\n\n
$ 

$ "NEW-"
11 "Exiting text editor ..."
$ "NEW-"
12 "Running in standalone mode ..."
$ "NEW-"
13 "Could not connect to ToolTalk service:\n%s\n%s\n"

$set 10        Redefine set#

$ ************************************************************************ 
$ **                                                          session.c ** 
$ ** THESE MESSAGES ARE _ONLY_ WRITTEN TO THE ERRORLOG FILE. 
$ ** 
$ ************************************************************************ 
$ Strings used to build error messages issued when the session save 
$ file cannot be read or written. 

$quote "
1 "Comprobar los permisos de archivo."

2 "%s es el archivo que se tendría que haber usado para guardar la sesión."

$set 11        Redefine set#

$ *********************************************************** 
$ **                                              dtpad.c ** 
$ **                                                       ** 
$ ** These are all of the menu button labels and mnemonics ** 
$ *********************************************************** 
$ ***UNUSED*** \tMessages 1-14 are no longer used and need not be localized. 
$ \tAll of the mnemonics and labels relating to the "Help" menu 
$ \thave been replaced by messages 81-96. 
$ * 
$ * Mnemonics and labels for the "Help" pulldown menu and items. 
$ * 
$ Mnemonic and label for the "Help" pulldown menu 

1 "A"

2 "Ayuda"
$ mnemonic and label for the "Overview" item 

3 "I"

4 "Información general..."
$ mnemonic and label for the "Tasks" item 

5 "T"

6 "Tareas..."
$ nmemonic and label for the "Table of Contents" item are message nos. 71 & 72 
$ mnemonic and label for the "Reference" item 

7 "R"

8 "Referencia..."
$ mnemonic and label for the "On Item" item 

9 "S"

10 "Sobre el tema"
$ mnemonic and label for "Using Help" item 

11 "U"

12 "Uso de la Ayuda..."
$ mnemonic and label for "About Text Editor" item 

13 "A"

14 "Acerca del Editor de textos..."
$ * 
$ * Mnemonics and labels for the "File" pulldown menu and items. 
$ * 
$ mnemonic and label for the "File" pulldown menu 

15 "r"

16 "Archivo"
$ mnemonic and label for the "New" item 

17 "N"

18 "Nuevo"
$ mnemonic and label for the "Open" item 

19 "b"

20 "Abrir..."
$ mnemonic and label for the "Include" item 

21 "I"

22 "Incluir..."
$ mnemonic and label for the "Save" item 
$ NOTE: The "Save" label can be either Message 24 or message 73 

23 "G"

24 "Guardar"
$ mnemonic and label for the "Save As" item 

25 "c"

26 "Guardar como..."
$ mnemonic and label for the "Print" item 

27 "p"

28 "Imprimir..."
$ mnemonic and label for the "Close" item 

29 "C"

30 "Cerrar"
$ * 
$ * Mnemonics and labels for the "Edit" pulldown menu and items. 
$ * 
$ mnemonic and label for the "Edit" pulldown menu 

31 "E"

32 "Editar"
$ mnemonic and label for the "Undo" menu item 

33 "D"

34 "Deshacer"
$ mnemonic and label for the "Cut" menu item 

35 "o"

36 "Cortar"
$ mnemonic and label for the "Copy" menu item 

37 "C"

38 "Copiar"
$ mnemonic and label for the "Paste" menu item 

39 "P"

40 "Pegar"
$ mnemonic and label for the "Clear" menu item 

41 "B"

42 "Borrar"
$ mnemonic and label for the "Delete" menu item 

43 "S"

44 "Suprimir"
$ ***UNUSED***\tMessages 45-50 are no longer used and need not be localized. 
$ \tThey have been replaced by messages 101-106 of this set. 
$ mnemonic and label for the "Select All" menu item 
$ 45 A 
$ 46 Select All 
$ mnemonic and label for the "Find/Change" menu item 
$ 47 F 
$ 48 Find/Change... 
$ mnemonic and label for the Check Spelling menu item 
$ 49 S 
$ 50 Check Spelling... 
$ * 
$ * Mnemonics and labels for the "Format" pulldown menu and items. 
$ * 
$ mnemonic and label for the "Format" pulldown menu 

51 "F"

52 "Formato"
$ mnemonic and label for the "Settings..." menu item 

53 "C"

54 "Configuración..."
$ mnemonic and label for the "Paragraph" menu item 

55 "P"

56 "Párrafo"
$ mnemonic and label for the "All" menu item 

57 "T"

58 "Todo"
$ * 
$ * Mnemonics and labels for the "Options" pulldown menu and items. 
$ * 
$ mnemonic and label for the "Format" pulldown menu 

59 "O"

60 "Opciones"
$ mnemonic and label for the "Overstrike" toggle button 

61 "P"

62 "Pulsación"
$ mnemonic and label for the "Word Wrap" toggle button 
$ ***UNUSED***\tThis label and mnemonic are no longer used and need not be 
$ \tlocalized.  They've been replaced by messages 111 and 112. 

63 "d"

64 "Acomodación automática texto"
$ mnemonic and label for the "Status Line" toggle button 

65 "L"

66 "Línea de estado"
$ * 
$ * Late additions. 
$ * 
$ nmemonic and label for the "Table of Contents" item of the "Help" menu 
$ ***UNUSED*** \tThis nmemonic and label are no longer used and need not be 
$ \tlocalized.  They've been replaced by messages 87 and 88. 

71 "c"

72 "Tabla de contenido"
$ label for the "Save" item in the "File" pulldown menu 
$ NOTE: This label is used in place of message 24 when text has been 
$       changed but has not been saved 

73 "Guardar (necesario)"
$ * 
$ * Mnemonics and labels for the "Help" pulldown menu and items. 
$ * 
$ Mnemonic and label for the "Help" pulldown menu 

81 "A"

82 "Ayuda"
$ mnemonic and label for the "Overview" item 

83 "g"

84 "Información general"
$ mnemonic and label for the "Tasks" item 

85 "T"

86 "Tareas"
$ nmemonic and label for the "Table of Contents" item 

87 "c"

88 "Tabla de contenido"
$ mnemonic and label for the "Reference" item 

89 "R"

90 "Referencia"
$ mnemonic and label for the "On Item" item 

91 "S"

92 "Sobre el tema"
$ mnemonic and label for "Using Help" item 

93 "U"

94 "Uso de la ayuda"
$ mnemonic and label for "About Text Editor" item 

95 "A"

96 "Acerca del Editor de textos"
$ * 
$ * Changes for "Edit" pulldown menu messages 45-50. 
$ * 
$ mnemonic and label for the "Select All" menu item 

101 "l"

102 "Seleccionar todo"
$ mnemonic and label for the "Find/Change" menu item 

103 "u"

104 "Buscar/Cambiar"
$ mnemonic and label for the Check Spelling menu item 

105 "e"

106 "Verificar ortografía..."
$ mnemonic and label for the "Copy As" File menu item 
$ (should match the dialog title specified in message 42 of set 5) 
$ ***UNUSED***\tThis label is no longer used and need not be localized. 

107 "c"

108 "Copiar como..."
$ mnemonic and label for the "Copy To File" File menu item 
$ (should match the dialog title specified in message 42 of set 5) 

109 "a"

110 "Copiar en archivo..."
$ mnemonic and label for the "Wrap To Fit" toggle button 

111 "R"

112 "Reiniciar"

$ Label for the "Print" item
113 Imprimir

$ *********************************************************** 
$ **                                              helpCB.c ** 
$ **                                                       ** 
$ ** Messages and strings associated with online help.     ** 
$ **                                                       ** 
$ *********************************************************** 

$set 12        Redefine set#

$ Message used for "Help - On Item" if the user clicks outside application 

1 "Debe seleccionar un elemento en el Editor de textos."
$ *********************************************************** 
$ **                                        ttMsgSupport.c ** 
$ **                                                       ** 
$ ** Messages and strings associated with ToolTalk.        ** 
$ **                                                       ** 
$ *********************************************************** 

$set 13        Redefine set#


1 "Ha fallado guardar punto de comprobación."

2 "Ha fallado responder punto de comprobación."


$ more errors/warnings can
$ be detected while reading in a file via ToolTalk.
3 "No se puede determinar el contenido del Editor de texto (causa desconocida)."


$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SETS 14 - 15 WERE ADDED TO SUPPORT CDEnext PRINTING
$ **
$ **	The following set was added to support CDEnext printing based on
$ **	the Xp Print Server.
$ **
$ ****************************************************************************
$ ****************************************************************************

$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SET 14
$ **
$ ****************************************************************************
$ ****************************************************************************
$set 14
$ 
$	Messages 1 - 11 are used in printJob.
$ 
$	This is warning message is displayed when the user has provided
$	a margin specifier in the PrintSetup dialog which cannot be parsed.
$	
1 "La sintaxis de uno de los siguientes especificadores de margen \nes incorrecta: \n %s \n %s \n %s \n %s \n¿Desea utilizar márgenes predeterminados?"
$ 
$	Messages 2 - 7 are messages for errors which can occur in the
$	process of loading a dtpad document file for printing.	
$ 
2 "El archivo no existe."
3 "El archivo especificado es un directorio."
4 "Error de tipo de archivo."
5 "El archivo contiene caracteres NULL."
6 "Incapaz de cargar el archivo (no hay suficiente memoria)."
7 "El archivo no cuenta con autorizaciones de lectura"

$	Messages 8 - 11 are format strings used to construct the header
$	and footer strings which appear in the printing output.
$ 
$	Message 8 is the format string used to make a Date header/footer.
$	Message 9 is the format string used to make a DocName header/footer.
$	Message 10 is the format string used to make a Page# header/footer.
$	Message 11 is the format string used to make a UserName header/footer.
8 "Fecha:  %s"
9 "Documento:  %s"
10 "Página %d de %d"
11 "Documento para:  %s"

$ 
$	Messages 12 - 13 are used in printSetup to construct the
$	app-specific printing options for DtMail which will be included
$	in the DtPrintSetupBox.
$ 
$	Message 12 is the string labeling the field in the GUI which
$	contains the name of the document.
$	Message 13 is the label used on the Toggle Button to indicate that
$	lines which would extend beyond the edge of the printed page be 
$	"wrapped" to the next line.
12 "Nombre de documento: "
13 "Auste automático de texto"


$	Messages 14 - 15 are status messages for the PrintToFile function.
$ 
$	Message 14 indicates that the PrintToFile function was unsuccessful.
$	Message 15 indicates that the PrintToFile function was successful.
$       NOTE:  'Print to File' should be translated as in DtPrint.msg
$               Set 1, Message 3.
14 "No se pudo efectuar la operación 'Imprimir en archivo'."
15 "'Imprimir en archivo' se ha realizado correctamente."

$          Messages 16 - 19 are obsolete, and have been replaced by 25
$ 
$	Messages 16 - 19 are status messages for the popup PDM function.
$ 
$	Message 16 indicates that there was no pdm deamon running.
$	Message 17 indicates that the pdm deamon was unable to start a pdm.
$	Message 18 indicates that the pdm couldnt connect to the video display.
$	Message 19 indicates that the pdm couldnt connect to the print display.
16 "No se encontró ningún PDM en el entorno."
17 "No se puede activar el PDM."
18 "PDM no está autorizado para conectarse con la pantalla de video."
19 "PDM no está autorizado para conectarse con la pantalla de impresión."

$	Messages 20 - 22 are status messages displayed while printing
$ 
$	Message 20 indicates that the user has asked to close the editor
$		   window.  Dtpad is prevented from doing so until the
$		   active print jobs have completed.
$	Message 21 status message displayed when the print job is started.
$	Message 22 status message displayed as the print job progresses.
20 "La operación de cierre está pendiente. Esperando que la tarea termine..."
21 "Imprimiendo %s ..."
22 "Imprimiendo %s: página %d de %d ..."

$       Message 23 is used in the construction of a title string for the
$                  Print Setup dialog.
23 "Configurar impresión - "

$	Message 24 is used to alert the user to the fact that the X Print
$		   Server cannot complete the print request due to a lack
$		   of resources.
24 "El servidor de impresión X está temporalmente sin recursos"

$       Messages 25 is used to handle errors from the PDM
25	"Error del Administrador de diálogo de impresión (PDM) - Falló la configuración."

$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SET 15
$ **
$ ****************************************************************************
$ ****************************************************************************
$ 
$set 15
$ 
$	Messages 1-5 are used in printSetup.c in the pulldown menu for the
$	header/footer options in the print setup dialog.
$ 
$	Message 1 specifies that the page header/footer field be empty.
$	Message 2 put the Date in the page header/footer field.
$	Message 3 put the Document Name in the page header/footer field.
$	Message 4 put the Page Nmbr msg header in the page header/footer field.
$	Message 5 put the User Name msg header in the page header/footer field.
1 "Vacío"
2 "Fecha"
3 "Nombre de documento"
4 "Número de página"
5 "Nombre de usuario"

$	Messages 6-9 are used to construct error messages informing users
$	that they have input an invalid print page margin specification for 
$ 
6 "El especificador del margen superior no es válido:  "
7 "El especificador del margen derecho no es válido:  "
8 "El especificador del margen inferior no es válido:  "
9 "El especificador del margen izquierdo no es válido:  "
