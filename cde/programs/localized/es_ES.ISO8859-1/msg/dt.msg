$ "$TOG: dt.msg /main/7 1999/09/16 14:53:28 mgreess $";  
$ *************************************<+>*************************************  
$ *****************************************************************************  
$ **  
$ **  File:        dt.msg  
$ **  
$ **  Project:     DT  
$ **  
$ **  Description:  
$ **  -----------  
$ **  This file is the source for the message catalog for DT library.  
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
$ **  $TOG: dt.msg /main/7 1999/09/16 14:53:28 mgreess $  
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
$  2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).  
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
$quote "

$set 2
$ *****  Module: Action.c  *****  
$ DO NOT TRANSLATE or CHANGE or LOCALIZE  messages 1 and 2 from set 2  
$ These messages are the result from incorrect actions.  
$ Do not remove the two spaces after the ":" in message 3.  
$ The name of an action will follow the spaces at the end of message 3.    
$ _DtMessage 5 is a prompt stating that the user must supply information  
$   for the action.  Execute the "TERM_PROMPT" action for an example.  

1 "%1$s%2$s%3$s"

2 "%1$s%2$s"

3 "Acción:  "

4 "[Error]"

5 "Por favor, entre la siguiente información:"

6 "Ha suministrado más parámetros que los que necesita la acción seleccionada.\n\n\
Seleccione 'Bien' para ignorar los parámetros adicionales.\n\n\
Seleccionar 'Cancelar' para terminar la acción."
$ NOTE: trailing spaces ARE important in message 7!  

7 "El siguiente host no era accesible:\n\n\
        "

8 "\n\n\
Verificar que se ha efectuado la conexión remota de acceso \n\
de datos adecuada.\n\n"
$ ** The arguments for message #9 are: <action name>, <file name>, <file type>.  

9 "La acción \"%1$s\" no se ha encontrado o\n\
no se aplica a este archivo:\n\
    \"%2$s\"\n\
con atributos de datos::   \"%3$s\"\n\n"
$ ** The argument for message #10 is: <action name>  

10 "La acción \"%s\" no se ha encontrado.\n"
$ ** Message number 11 is difficult to generate without an internal error.  

11 "No se ha encontrado el siguiente archivo:\n\n\
       "
$ Do NOT localize the word "MAP".  

12 "No se puede ejecutar esta acción porque contiene demasiados\n\
niveles de MAP o la correspondencia es \"circular\"."

13 "No es posible invocar la acción solicitada.\n\n\
¿Están accesibles los siguientes hosts?\n\
\t(%s)\n\
¿Existe el programa correspondiente?\n\
(Ejecutar /usr/dt/bin/dttypes para hacer coincidir acciones y programas.)\n\n\
¿Tiene su sistema espacio de ejecución suficiente para ejecutar \n\
 nuevos procesos?"

14 "La petición de servicio a esta acción ha fallado."

15 "La petición de servicio a esta acción ha fallado.\n\
No se ha podido establecer una conexión ToolTalk:\n\n%s"

16 "La petición de servicio a esta acción ha fallado."

17 "La petición de servicio a esta acción ha fallado, debido a la razón siguiente:\n\n\
    %s."

18 "Se ha producido un error al intentar hacer corresponder\n\
uno de los argumentos de archivo."
$ ** The arguments for message #19 are: <action name>, <buffer type>.  

19 "La acción \"%1$s\" no se ha encontrado, o bien esta acción\n\
no se aplica a almacenamientos intermedios de tipo: \n\n\
\"%2$s\""
$ **  MESSAGE NUMBER 20 IS OBSOLETE.  

20 "No es posible crear el archivo temporal: \"%1$s\" \n\
para la acción: \"%2$s\""

21 "No se da soporte a la clase de objeto de entrada: \"%d\"\n\
para la acción: \"%s\"."

22 "No es posible crear un archivo temporal en el directorio: \"%s\"\n\
para la acción denominada: \"%s\"."

23 "No es posible abrir el archivo temporal: \"%s\"\n\
para la acción denominada: \"%s\"."

24 "No es posible grabar un archivo temporal: \"%s\"\n\
para la acción denominada: \"%s\"."

$set 3

$ ****  Module: CmdMain.c ****  
$ These messages are generated by the command invocation library.   
$ Note that set 3, messages 1, 3, 4, 12, 13, 16 have been obsoleted.  

2 "Ha fallado un intento de cambiar al siguiente directorio:\n\n\
   %1$s\n\n\
en el host \"%2$s\".\n\n\
Verificar la ortografía y permisos y cerciorarse de que el directorio existe."
$ Do NOT remove the leading spaces from any of the lines of the following message.  

5 "Ha fallado un intento de ejecutar el siguiente mandato en el \n\
host \"%1$s\":\n\n\
   %2$s\n\n\
Verificar que exista el programa, que tenga los permisos adecuados\n\
y que sea ejecutable."
$ Do NOT remove the leading spaces from any of the lines of the following message.  

6 "Ha fallado un intento de ejecutar el mandato siguiente:\n\n\
   %s\n\n\
Verificar que exista el programa, que tenga los permisos adecuados\n\
y que sea ejecutable."
$ Do NOT remove the leading spaces from any of the lines of the following message.  
$ Do NOT localize the word "PATH".  

7 "No puede iniciarse esta acción porque el siguiente\n\
emulador de terminal no se puede ejecutar:\n\n\
   %s\n\n\
Verificar que exista el programa, que tenga los permisos adecuados\n\
y que sea ejecutable.  Este problema puede haber ocurrido\n\
porque el programa no está en el \"PATH\" del usurio."
$ Message number 8 is OBSOLETE.  
$ 8 This action is missing one or more of the following: 
$  
$ \

$   execution parameters, execution string or execution host. 
$  
$ \

$ Check that the action has an appropriate number of parameters.  
$ Do NOT remove the leading spaces from any of the lines of the following message.  

9 "El número total de caracteres en esta acción sobrepasa el límite de \"%d\".\n\n\
Puede ser necesario fragmentar la acción en varias acciones."
$ Message number 10 is OBSOLETE.  
$ 10 The shell program \"%s\" cannot be started. 
$  
$ \

$ Check that the program has the correct permissions and 
$ \

$ is executable.  

11 "Ha fallado un intento de iniciar un nuevo proceso en el host \"%s\".\n\n\
Para continuar, puede ser necesario detener un proceso no necesario en este host."
$ Message number 14 is OBSOLETE.  
$ 14 An attempt to log the output from a remote host failed. 
$  
$ \

$ To continue, you may need to stop an existing process.  

15 "Ha fallado un intento de registrar el registro cronológico de salida\n\
de un host remoto.\n\n\
Para continuar, puede ser necesario detener un proceso existente."
$ Do NOT remove the leading spaces from any of the lines of the following message.  
$ Do NOT localize the words "WINDOW_TYPE".  

17 "Esta acción \"%1$s\" de WINDOW_TYPE no se reconoce.\n\
WINDOW-TYPE debe ser uno de los siguientes:\n\n\
  %2$s, %3$s, %4$s,\n\
  %5$s, %6$s o %7$s"
$ Do NOT remove the leading spaces from any of the lines of the following message.  

18 "Esta acción no puede iniciarse porque el programa de subproceso de DT\n\
  %s\n\n\
no puede ser ejecutado. Comprobar que el programa tiene los\n\
permisos adecuados y que es ejecutable."
$ MESSAGE 19 IN SET 3 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  
$ MESSAGE 19 IN SET 3 IS OBSOLETE  
$ Do NOT remove the leading spaces from any of the lines of the following message.  

19 "Ha fallado un intento de ejecutar 'xhost' en el host de Xserver \"%s\".\n\n\
Para suprimir este mensaje e inhabilitar el xhost automático, \n\
ejecutar la acción \"EditarRecursos\" y añadir el siguiente recurso:\n\n\
   *autoXhosting:  False\n"
$ Do NOT remove the leading spaces from any lines in the following message.  

20 "Esta acción no se puede ejecutar en el host \"%1$s\" \n\
porque el siguiente programa requerido no existe o\n\
no es ejecutable:\n\n\
  %2$s\n"
$ MESSAGE 21 IN SET 3 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

21 "La acción \"%1$s\" se ha ejecutado satisfactoriamente en el host \"%2$s\"."
$ MESSAGE 22 IN SET 3 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

22 "El directorio \"%1$s\" en el host \"%2$s\" no se \n\
ha podido convertir a una vía de acceso de red.\n\
(%3$s)"
$ MESSAGE 23 IN SET 3 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

23 "El archivo de registro cronológico de errores \"%1$s\" en el host \"%2$s\"\n\
no se ha podido convertir a una vía de acceso de red.\n\
(%3$s)"

24 "Esta acción no se puede ejecutar porque\n\
no se ha podido alcanzar el host \"%s\"."

25 "Esta acción no se ha podido ejecutar en el host \"%1$s\" porque\n\
el servicio \"%2$s\" no está correctamente configurado en este host."

26 "Esta acción no se ha podido ejecutar porque el servicio \"%1$s\"\n\
no está configurado en el host \"%2$s\"."

27 "Esta acción no se puede ejecutar en el host \"%1$s\" porque el\n\
usuario \"%2$s\" tiene un id de usuario de \"%3$d' en el host \"%4$s\" \n\
y no coincide el nombre de usuario y el id de usuario en la\n\
acción de llamada al host \"%5$s\"."

28 "Esta acción no se puede ejecutar en el host \"%1$s\" porque\n\
el usuario \"%2$s\" no tiene un perfil en este host."

29 "Esta acción no se puede ejecutar en el host \"%s\" porque\n\
no se puede crear un nombre de vía al archivo de autentificación."

30 "Esta acción no se puede ejecutar en el host \"%s\" porque\n\
no se puede abrir el archivo de autentificación de este host.\n\n\
Ello puede ser debido a que el directorio de inicio \n\
de red no se ha configurado adecuadamente."

31 "Esta acción no se puede ejecutar en el host \"%1$s\" porque \n\
el entorno excede de \"%2$d\" bytes."

32 "Esta acción no se puede ejecutar en el host \"%1$s\" porque\n\
el host \"%2$s\" no está autorizado a utilizar el servicio \"%3$s\".\n\n\
Para corregirlo, añada el host \"%4$s\" a la entrada de \n\
servicio \"%5$s\" en el archivo \"%6$s\" en el host \"%7$s\"."

$set 4

$ *****  Module: DbReader.c  *****  
$   
$  MESSAGES 1-11 IN SET 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  
$   
$ Do NOT remove the leading spaces from any of the lines of the following message.  

1 "A una definición \"%s\" en el archivo:\n\
  \"%s\"\n\
le falta un nombre.  Añadir el nombre\n\
  a la definición.\n"
$ Do NOT remove the leading spaces from any of the lines of the following message.  
$ Do NOT localize the word "END".  
$ THIS MESSAGE IS OBSOLETE  

2 "La definición \"%s\" en el archivo\n\
  \"%s\"\n\
no está completa. En la definición puede faltar\n\
el campo \"END\".\n"
$ Do NOT remove the leading spaces from any of the lines of the following message.  

3 "Un campo multilínea en la definición \"%s\"\n\
en el archivo\n\
  \"%s\"\n\
está incompleto.  Puede faltar un carácter \"\\\" \n\
en el campo multilínea.\n"
$ Do NOT remove the leading spaces from any of the lines of the following message.  

4 "La definición \"%s\" en el archivo\n\
  \"%s\"\n\
tiene demasiados campos.\n"
$ Do NOT localize the word "Dt.TypesDirs".  

5 "Ha fallado un intento de leer las bases de datos de acciones y tipos de archivo.\n\
Esto puede ser producido porque se ha establecido incorrectamente el recurso\n\
\"Dt.TypesDirs\".\n"
$ Do NOT localize the strings "'{'" or "'}'".  

6 "La definición \"%s\" en el archivo\n\
  \"%s\"\n\
  tiene una llave final '}' sin llave inicial '{'.\n"
$ Do NOT localize the strings "'{'" or "'}'".  

7 "La definición \"%s\" en el archivo\n\
  \"%s\"\n\
  tiene una llave inicial no esperada '{'.\n"
$ Do NOT localize the strings "'{'" or "'}'".  

8 "La definición \"%s\" en el archivo\n\
  \"%s\"\n\
  está incompleta. A la definición le puede\n\
  faltar el campo \"}\".\n"
$ Do NOT localize the strings "'{'" or "'}'".  

9 "A la definición \"%s\" en el archivo\n\
  \"%s\"\n\
  le falta una llave inicial '{'.\n"

10 "La variable DtDbVersion sólo se puede establecer\n\
al principio del archivo.  Se ignora el resto del\n\
archivo '%s'.\n"

11 "El archivo '%s'\n\
contiene un identificador de DtDbVersion no válido.\n"
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  
$ $  
$ set 5 (Module: Filetype.c) is OBSOLETE.  Do NOT reuse this set number.  
$ $  
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  

$set 6

$ *****  Module: ActionDb.c  *****  
$   
$  MESSAGES 1-11 IN SET 6 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  
$   
$ Do NOT remove the leading spaces from any of the lines of the following message.  

1 "La definición de acción \"%s\" en el archivo:\n\
  \"%s\"\n\
no tiene campos.\n"
$ Do NOT remove the leading spaces from any of the lines of the following message.  

2 "La definición de acción \"%s\" en el archivo\n\
  \"%s\"\n\
contiene el siguiente nombre y valor de campo no reconocido:\n\
\"%s\"\n"
$ Message number 3 is OBSOLETE.  
$ 3 The action definition \"%s\" in the file 
$ \

$   \"%s\" 
$ \

$ does not have the required field \"%s\". 
$   

4 "El campo \"%s\" en la definición de la acción \"%s\"\n\
en el archivo \"%s\"\n\
tiene un tipo de acción no reconocido.\n"
$ Do NOT remove the leading spaces from any of the lines of the following message.  
$ Do NOT localize the words "TYPE MAP".  

5 "La definición de acción \"%s\" en el archivo\n\
  \"%s\"\n\
no esta correlacionada adecuadamente.  El campo \"%s\"\n\
debe ser: TYPE MAP acción_correlacionada_a.\n"
$ Message number 6 is OBSOLETE.  
$ 6 The action definition \"%s\" in the file 
$ \

$   \"%s\" 
$ \

$ contains a MAP to itself. 
$   
$ Message number 7 is OBSOLETE.  
$ 7 The \"%s\" field in the action definition \"%s\" 
$ \

$ in the file \"%s\" 
$ \

$ has the unrecognized value \"%s\". 
$   
$ Do NOT remove the leading spaces from any of the lines of the following message.  
$ Do NOT localize the word "COMMAND".  

8 "La definición de acción \"%s\" en el archivo\n\
  \"%s\"\n\
es un tipo de acción \"COMMAND\" pero falta el \n\
campo necesario \"%s\".\n"
$ Message number 9 is OBSOLETE.  
$ 9 The action definition \"%s\" in the file 
$ \

$   \"%s\" 
$ \

$ is a \"MESSAGE\" action type but lacks the 
$ \

$ required field \"%s\". 
$   
$ Message number 10 is OBSOLETE.  
$ 10 The action definition \"%s\" in the file 
$ \

$   \"%s\" 
$ \

$ is a \"MESSAGE\" action type but lacks the 
$ \

$ required field \"%s\". 
$   
$ Do NOT remove the leading spaces from any of the lines of the following message.  

11 "La definición de acción \"%s\" en el archivo\n\
  \"%s\"\n\
tiene el valor no permitido \"%s\" en el campo \"%s\".\n\
\""

12 "La definición de acción \"%s\" del archivo\n\
  \"%s\"\n\
contiene campos duplicados nombrados:\n\
 \"%s\"."

13 "La definición de acción \"%s\" del archivo\n\
  \"%s\"\n\
contiene campos no válidos para acciones de tipo \"%s\".\n"

14 "A la definición de acción \"%s\" del archivo\n\
  \"%s\"\n\
le falta el campo obligatorio \"%s\"\n\
 para acciones de tipo \"%s\".\n"
$ Message number 15 is OBSOLETE.  
$ 15 The action definition \"%s\" in the file 
$ \

$   \"%s\" 
$ \

$ contains a NULL field value, for the \"%s\" field. 
$   

16 "La definición de la acción \"%s\" en el archivo\n\
  \"%s\"\n\
tiene un valor \"%s\" no permitido en el cammpo \"%s%s%s\".\n\
\""
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  
$ $  
$ set 22 (Module: HelpUtil.c) is OBSOLETE.  Do NOT reuse this set number.  
$ $  
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  

$set 28

$ *****  Module:  DtUtil.c  *****  
$ The following are button labels.  

1 "Bien"

2 "Cancelar"

3 "Ayuda"

4 "Aplicar"

5 "Cerrar"
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  
$ $  
$ set 44 (Module: start_bms.c) is OBSOLETE.  Do NOT reuse this set number.  
$ $  
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  
$ $  
$ set 46 (Module: Utility.c) is OBSOLETE.  Do NOT reuse this set number.  
$ $  
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  

$set 48

$ *****  Module:  UserMsg.c  *****  
$ _DtMessage 1 uses the format of the "strftime" system call.  See the  
$ manual page for more information on "strftime".  WARNING: follow  
$ the man page closely because incorrect format strings could cause  
$ an application to abort.  

1 "%a %b %d %H:%M:%S %Y\n"
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  
$ $  
$ set 49 (Module: Annotate.c) is OBSOLETE.  Do NOT reuse this set number.  
$ $  
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  

$ Set 50 - message logging service 
$ 
$ The messages in this set will only appear in the DT log file.
$ The messages are used to identify messages written to this log file.
$ 
$ File: MsgLog.c
$ 
$set 50
1 INFORMACION
2 STDERR
3 DEPURAR
4 ADVERTENCIA
5 ERROR
6 DESCONOCIDO


$set 99

$    DO NOT TRANSLATE THESE MESSAGES  
$    DO NOT CHANGE THESE MESSAGES  
$    DO NOT LOCALIZE THESE MESSAGES  
$    These messages are used for the version information.  

1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $TOG: dt.msg /main/7 1999/09/16 14:53:28 mgreess $"
