$ $XConsortium: dtsearch.msg /main/7 1996/10/30 17:35:31 cde-ibm $
$ 
$ (c) Copyright 1996 Digital Equipment Corporation.
$ (c) Copyright 1996 Hewlett-Packard Company.
$ (c) Copyright 1996 International Business Machines Corp.
$ (c) Copyright 1996 Sun Microsystems, Inc.
$ (c) Copyright 1996 Novell, Inc. 
$ (c) Copyright 1996 FUJITSU LIMITED.
$ (c) Copyright 1996 Hitachi.

$   COMPONENT_NAME: austext
$ 
$   FUNCTIONS: database
$ 
$   ORIGINS: 27
$ 
$   IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$   combined with the aggregated modules for this product)
$   OBJECT CODE ONLY SOURCE MATERIALS
$ 
$   (C) COPYRIGHT International Business Machines Corp. 1994,1995
$   All Rights Reserved
$   US Government Users Restricted Rights - Use, duplication or
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.

$ ********************** DTSEARCH.MSG ************************
$ $Id: dtsearch.msg /main/7 1996/10/30 17:35:31 cde-ibm $
$ December 1994.
$ Messages specific to DtSearch.
$ Symbolic msg sets replaced with numbers for portability.
$ Basically there is a msgset for each module.

$ $Log$
$ Revision 2.15  1996/04/10  22:54:07  miker
$ Revision 2.14  1996/04/10  19:47:18  miker
$ Revision 2.13  1996/03/20  19:32:13  miker
$ Revision 2.12  1996/03/13  22:53:07  miker
$ Revision 2.11  1996/03/05  16:04:30  miker
$ Revision 2.10  1996/02/01  17:16:48  miker
$ Revision 2.9  1995/12/27  21:17:19  miker
$ Revision 2.8  1995/12/27  16:04:46  miker
$ Revision 2.7  1995/12/07  23:26:31  miker
$ Revision 2.6  1995/12/01  16:10:25  miker
$ Revision 2.5  1995/10/26  16:21:21  miker
$ Revision 2.4  1995/10/20  21:30:42  miker
$ Revision 2.3  1995/10/19  20:52:30  miker
$ Revision 2.2  1995/10/03  21:37:17  miker
$ Revision 2.1  1995/09/22  17:54:34  miker

$quote "

$ General comments about all messages in this file:
$ - Any message that begins with a %1$s or %s is using that variable
$   to represent the name of the executable which is causing the
$   message to be output.
$ - Any variables specified in Usage messages should be translated
$   consistently.

$ ****************************************************
$ set 1 formerly set MS_misc, used many places
$set 1

1 "<null>"
$ message 2 can be translated to match local conventions.  It is used
$ in output files for information.
2 "%Y/%m/%d,%H:%M:%S"
3 "*** %1$sNo queda memoria.  %2$s solicita %3$lu bytes. ***\n"
4 "%1$s Versión %2$s.\n"
$ message 6 refers to any value which is null when printing information.
6 "<desconocido>"
8 "%1$s No quedan bases de datos válidas."
9 "%1$s Argumento de línea de comando '%2$s' no válido.\n"
10 "%1$s Se ha ignorado el argumento de línea de comando desconocido '%2$s'.\n"
11 "%1$s Nombre de base de datos no válido '%2$s'.\n"
$ In message 13, do not translate "dtsrcreate." It is an executable name.
13 "%1$s No hay ninguna anotación DB en la base de datos '%2$s'.\n\
  La causa frecuente es que no se inicializa la base de datos (ejecutar dtsrcreate)."
14 "%1$s La versión de programa '%2$s' no es compatible con\n\
 la versión de la base de datos '%3$s' '%4$s'.\n"
15 "%1$s Se recibió la señal de cancelación %2$d.\n"
16 "<no hay variable de entorno>"
17 "<no hay daemon de servidor>"
18 "%1$s No se ha especificado el nombre de la base de datos.\n"
19 " ,\t\n"
21 "%1$s Código de salida = %2$d.\n"
$ Message 22 can be translated.  It represents a human-readable date like
$ "Thursday, Jan 5, 1996, 6:30 pm".  The "%5$I" is 12 hour clock time, but
$ it could be changed to "%5$H" for 24 hour clock time.
22 "%A, %b %d %Y, %I:%M %p"
23 "%1$s: Versión %2$s.  Ejecución %3$s.\n"
$ Message 24 should not translate ".fzk".
24 "%1$s: directorio de trabajo actual = '%2$s', .fzk archivo = '%3$s'\n"
25 "%1$s La cancelación se debe a un error del programa.\n"
27 "%1$s: %2$s: Formato de archivo no válido."
96 "%s La lista de elementos encontrados está vacía."
301 "%1$s texto de entrada es %2$s."
$ Msg 362 is a generic system message.  The format is
$ "<program name> <filename>: <system error>" where <system error>
$ is a system errno which will already be translated.
362 "%1$s %2$s: %3$s"
$ Msg 400 is like "<Program name>: <file name>, line <line number>:..."
400 "%1$s: %2$s, línea %3$ld: La palabra '%4$s' contiene caracteres no válidos."
$ Msg 423 is like "<Program name> Word <word> in <file name> is a duplicate."
423 "%1$s la palabra '%2$s' de %3$s es un duplicado."
1596 "%1$s %2$s No se puede abrir el archivo de auditoría %3$s: %4$s"

$ ****************************************************
$ set 2 formerly set MS_ausapi (ausapi.*)
$set 2

1 "%1$s El nombre de la base de datos '%2$s' no es válido o no está disponible."
$ Msg 7 should not translate "dbnames" or "dbcount."
7 "Se han especificado %s dbnames pero ningún dbcount."
20 "%1$s search_type '%2$c' no válido."
30 "%s Consulta nula.  No se efectuó ninguna búsqueda."
$ Msg 37 should not translate "DtSearchInit."  It is a function name.
37 "%s La primera llamada de función API debe ser DtSearchInit().\n"
40 "%1$s Eliminación de texto nula.  No se realzó nada."
$ Msg 115 must have the date formatted in the way specified.
115 "%1$s '%2$s' es una cadena de fecha no válida o incompleta.\n\
Formato correcto '[aa]aa [mm [dd]]'."
$ Msg 187 must not translate "LOGNAME."  It is an environment variable.
187 "%1$s Faltan la identificación del usuario y la variable de entorno LOGNAME."
198 "%s La fecha 'Antes' es igual o posterior a la 'Después'.\n\
  No se devolvería ninguna anotación."
216 "\n%1$s %2$s Detectó una señal %3$d.\n"
294 "%1$s Error del programa en DtSearchResultsToText."
437 "%s La consulta contiene una cadena de navegación no válida."
492 "%1$s No hay ninguna lista actual de elementos encontrados."
498 "%1$s Los argumentos de DtSearchResultsToText no son válidos."
621 "%1$s %2$s ya se ha inicializado."
806 "%1$s La consulta no es suficiente o las opciones de búsqueda son\n\
incompatibles con la base de datos '%2$s' para iniciar la búsqueda."
$ Msg 1072 "stem" is the root of a word.
1072 "%1$s Error del programa: El total del tronco (%2$d) es mayor que el máximo (%3$d)."
1342 "%1$s Error del motor de búsqueda %2$d para la solicitud de realce de\n\
la base de datos '%3$s', total de palabras encontradas =%4$ld, tipo de búsqueda ='%5$c', texto ='%6$.30s'"


$ ****************************************************
$ set 5 formerly set MS_chandel, used in dtsrhan.c
$set 5

3 "El archivo de salida '%1$s' ya existe.\n"
$ Msg 4 needs to use the same 2 characters from messages 4 and 5.
$ The "q" is not needed because it is assumed that any character
$ other than "a" or "o" means to quit the process.
4 "Anexar, sobreescribir o cancelar? [a,s,c] "
$ Msg 5 is the shortcut key for "a" or Append from message 4.
5 "a"
$ Msg 6 is the shortcut key for "o" or overwrite from message 4.
6 "s"
7 "No se pudo abrir el archivo de salida '%1$s'.\n"
11 "\nError - no se puede abrir el archivo de perfil '%1$s'.\n"
12 "Línea de error %1$d: identificador no válido '%2$s'.\n"
13 "Línea de error %1$d - faltan valores del identificador '%2$s'.\n"
14 "Línea de error %1$d - cero o valor incorrecto de '%2$s'.\ntestigo incorrecto: %3$s.\n"
$ Msg 15 should not translate "ANY."
15 "Línea de error %1$d - del identificador '%2$s', la columna se ha configurado con ANY\n\
  pero no hay ninguna cadena de identificación de firma.\n"
16 "Línea de error %1$d - cero o valor incorrecto para el identificador '%2$s'\n\
  testigo incorrecto: %3$s.\n"
18 "Línea de error %1$d - falta el valor del identificador '%2$s'\n"
23 "Línea de advertencia %1$d: Se ha vuelto a definir el delimitador.\n"
25 "Línea de error %1$d: No se ha especificado el delimitador como 'superior' o 'inferior'.\n"
26 "Línea de error %1$d: testigo no válido '%1$s'.\n"
27 "Línea de error %1$d: testigo no válido '%2$s'.\n"
$ Msg 34 needs to match the 'all' from msgs 36 and 37.
34 "todo"
$ Msg 35 needs to match the 'none' from msgs 36 and 37.
35 "ninguno"
$ Msg 36 needs 'all' to match msg 34 and 'none' to match msg 35.
36 "Línea de error %1$d: el modo de imagen debe ser 'todo' o 'ninguno' -'%2$s' no se reconoce.\n"
$ Msg 37 needs "all" to match msg 34 and "none" to match msg 35.
37 "Línea de error %1$d: el modo de texto debe ser 'todo' o 'ninguno' - '%2$s' no se reconoce.\n"
38 "Línea de advertencia %1$d: Se ha vuelto a definir el carácter clave.\n"
39 "Línea de error %1$d: Carácter clave no válido:'%2$c'.\n"
40 "Línea de advertencia %1$d - Se ha vuelto a definir el campo clave.\n"
43 "Línea de error %1$d - Identificador incorrecto '%2$s' para la clave.\n"
$ Msg 45 should not translate "discard."
45 "Línea de error %1$d: Opción desconocida para 'discard': '%2$s'.\n"
$ Msg 46 should not translate "delblanklines."
46 "Línea de error %1$d: Opción desconocida para 'delblanklines': '%2$s'.\n"
47 "Línea de error %1$d: Opción desconocida para resumen:'%2$s'\n"
48 "Línea de error %1$d -Tipo de identificador desconocido '%2$s'.\n"
49 "Error - Delimitador no definido.\n"
50 "Error - No se ha definido el carácter de tipo de clave.\n"
51 "Error - No se ha definido la clave.\n"
52 "Error - No se ha encontrado el delimitador definido como '%1$s'.\n"
53 "Error - El delimitador definido como '%1$s' hace referencia a una línea física de la anotación.\n\
   Debido a que el delimitador define líneas físicas\n\
  no puede hacerse referencia al mismo como línea física.\n"
54 "Error - Para el campo '%1$s', no hay ningún identificador de línea correspondiente a '%2$s'.\n"
55 "Error - La lista incluir/excluir campos incluye\n\
  el campo '%1$s' que no se ha definido.\n"
56 "Error - La lista incluir/excluir imágenes incluye\n\
  la línea '%1$s' que no se ha definido.\n"
57 "Error - La lista incluir/excluir texto incluye\n\
  la línea '%1$s' que no se ha definido.\n"
58 "Error - La definición de la clave hace referencia a '%1$s'\n\
  que no se ha definido.\n"
59 "Error - La definición abstracta hace referencia al campo '%1$s'\n\
  que no se ha definido.\n"
68 "Advertencia - No se han encontrado los campos necesarios para la clave.\n\
  Se desechará la anotación %1$ld que comienza con:\n\
  %2$s\n"
69 "Advertencia - La anotación %1$ld no tiene sentido. Se descartará...\n\
  La anotación comienza con: %2$.60s\n"
70 " No se puede abrir el archivo de entrada '%1$s'.\n"
$ Msg 71 should not translate ".fzk."
71 "\n\
USO: %1$s [opciones] <perfil> <archivo de entrada> [<archivo de salida>]\n\
  -m         Desactiva todos los mensajes de error.\n\
  -w<N>      Cambia el ancho de la pantalla de destino a <N>.\n\
  -oo        Aprobación previa de sobreescritura de un archivo de salida.\n\
  -oa        Aprobación previa de anexión a un archivo de salida.\n\
  <perfil>  Archivo de entrada que contiene el perfil de las anotaciones\n\
             a procesar.\n\
  <archivo de entrada>   Archivo de entrada que contiene las anotaciones.\n\
  <archivo de salida>  Nombre de archivo de salida, formato .fzk (%2$s).\n\n"
72 "El ancho de pantalla especificado no es válido.\n"
75 "'%1$s' es un modo de salida no válido.\n"
76 "Argumento de línea de comando desconocido '%1$s'.\n"
77 "Falta el nombre de archivo de perfil requerido.\n"
78 "Falta el nombre del archivo de entrada requerido.\n"
79 "Error - Se utiliza stdin como entrada. Se requiere el nombre del archivo de salida.\n"
80 "Los archivos de perfil y entrada tienen el mismo nombre:'%1$s'.\n"
81 "Los archivos de entrada y salida tienen el mismo nombre:'%1$s'.\n"
82 "Los archivos de perfil y salida tienen el mismo nombre:'%1$s'.\n"
83 "        Archivo de perfil: %1$s\n"
$ Msg 84 should not translate "stdin."
84 "          Archivo de entrada: stdin\n"
85 "          Archivo de entrada: %1$s\n"
$ Msg 86 should not translate "stdout."
86 "         Archivo de salida: stdout\n"
87 "         Archivo de salida: %1$s\n"
88 "%1$s. Filtro de texto %2$s %3$s.\n"
89 "\n  Cada punto representa %ld anotaciones...\n"
90 "\nError al cerrar el archivo de salida. ¿Está lleno el disco?\n"
91 "Se abandona debido que el archivo de perfil contiene errores.\n"
92 "\n%1$s: Se ha completado normalmente.  Se han procesado %2$ld anotaciones.  Código de salida = 0.\n"
93 "Línea de error %1$d - La cadena '%2$s' no se encuentra entre comillas dobles.\n"
110 "Línea de advertencia %d - Se ha vuelto a definir el campo de fecha.\n"
111 "Línea de error %1$d - El identificador de fecha '%2$s' no es correcto.\n"
115 "%1$s Las fechas predeterminadas del objeto serán '%2$s'.\n"
116 "Error - Campo de referencias de fecha '%s' no definido.\n"
124 "%1$s No se puede escribir en el archivo de salida '%2$s':\n  %3$s\n"
$ Msg 125 should not be translated.
125 "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC"
130 "%1$s No se puede cargar los archivos de procesamiento de idioma:\n%2$s\n"
133 "\n\
Advertencia - '%1$s' es una especificación de fecha no válida.\n\
  Se utiliza la fecha '%2$s' para la anotación número %3$ld que comienza con: %4$.30s\n"


$ ****************************************************
$ set 6 formerly set MS_ve
$set 6

26 "Se llamó dummy_workproc().\n"
142 "%1$s Error del cliente: Anotación solicitada con n\
  dirección de base de datos no válida %2$ld (%3$d:%4$ld, x'%5$08.8lx') para la base de datos '%6$s'."
156 "%1$s Se utilizó una tabla de compresión de datos incompatible para la base de datos '%2$s'.\n\
  Base de datos comprimida con %3$ld. Descompresor de motor: %4$ld.\n"
157 "DTSRVE157 Error del programa cliente: usrblk.dba contiene una dirección de base de datos nula."
167 "%1$s No hay datos en la base de datos '%2$s'."
178 "%1$s Base de datos '%2$s' Versión '%3$s' no es compatible\n\
  con la versión del motor '%4$s'."
183 "%1$s No se puede abrir el archivo del semáforo de notas del usuario '%2$s': %3$s.\n"
185 "%1$s Base datos '%2$s': el tamaño máximo de palabra %3$d es demasiado pequeño."
199 "%1$s No se pudo adquirir el semáforo de notas del usuario '%2$s' con %3$d intentos.\n"
230 "%1$s No se pudo abrir el archivo de respaldo de notas de usuario '%2$s': %3$s."
231 "%1$sLa base de datos '%2$s' cuenta con el tamaño de resumen %3$d no válido."
251 "%1$s La base de datos '%2$s' está dañada:\n\
Totales de anotaciones y direcciones de base de datos incompatibles.\n"
$ Msgs 310 and 311 can be appended to msg 309 to produce output.
$ For example, one message might be
$ "<Programname> User notes disabled for entire Engine."
309 "%1$s Se han desactivado las notas de usuario "
$ Msg 310 can be appended to msg 309 to produce output.
310 "de todo el motor."
$ Msg 311 can be appended to msg 309 to produce output.
311 "de la base de datos '%1$s'."
$ Msg 332 can be translated to local timestamp.  It currently outputs
$ something like "1996/03/04 at 16:40 CDT"
332 "%Y/%m/%d a las %H:%M %Z"
333 "\n <La nota del usuario no fue anexada por '%1$s' en %2$s>\n"
398 "Cadena de consulta DTSRVE398 NULL."

$ ****************************************************
$ set 7 formerly set MS_uiutil, no longer used in uiutil/dtsrutil.c.

$ ****************************************************
$ set 8 formerly set MS_loadocf, for module ocf.c.

$ All words in set 8 which are capitalized should not be translated.
$ They represent keywords in a configuration file.  Words like PATH,
$ SEARCH, MAXHITS, FILEIO, KEYTYPE, FILEPATH, KEYTYPES, KEYTYPE,
$ UPPERKEYS, FALSE, and HOME.

$set 8

$ Msg 1 is "<Programname><filename>: <keyword> keyword is obsolete."
1 "%1$s%2$s: La palabra clave %3$s es obsoleta."
$ Msg 140 "(missing)" refers to a token which is expected to be found but
$ is missing.  The token is from an input string of words being indexed.
140 "(falta)"
$ Msgs 142-168 are combined into "<token>: <error>" where msg 142 or a
$ default file name can be <token> and msgs 143, 165, 166, and 168
$ can be <error>.
142 "resto de la línea"
143 "está vacía"
165 "identificación desconocida"
166 "se ha especificado anteriormente"
168 "le falta la especificación de la ruta"
184 "%1$s %2$s: El nombre de la base de datos '%3$.16s' contiene más de 8 caracteres.\n"
$ Msg 221 should not translate "PATH."
221 "%1$s %2$s: Base de datos '%3$s', no se ha especificado ningún PATH, se ignorará la instrucción.\n"
$ Msg 275 should not translate "SEARCH."
275 "%1$s %2$s: El valor SEARCH no válido se ha ignorado."
$ Msg 304 should not translate "MAXHITS."
304 "%1$s %2$s: Base de datos '%3$s', valor MAXHITS no válido.\n"
352 "%1$s %2$s: Valor %3$s no válido, se ignorará la línea.\n"
365 "%1$s %2$s: Falta el valor %3$s."
414 "%1$s No se puede abrir el archivo de configuración '%2$s':\n  %3$s."
457 "'%1$c' Anotaciones"
477 "%1$s %2$s: Falta el nombre de la base de datos después de la palabra clave '%3$s'.\n"
495 "%1$s %2$s: '%3$.16s' es una palabra clave no válida.  Se ignorará la línea %4$d.\n"
$ Msg 607 should not translate "FILEIO."
607 "%1$s %2$s: Valor FILEIO no válido.  Las operaciones de lectura y escritura del archivo del usuario están prohibidas."
610 "%1$s %2$s: El valor %3$s es '%4$s'. Debe ser un número entero positivo. Se ignorará la línea."
$ Msg 684 should not translate "KEYTYPE."
684 "%1$s %2$s: Base de datos '%3$s', carácter KEYTYPE '%4$c' no válido."
732 "%1$s %2$s: Base de datos '%3$s', no se permiten más de %4$d tipos de clave.\n"
$ Msg 903 should not translate "FILEPATH."
903 "%1$s %2$s: FILEPATH '%3$s' %4$s. Se ignorará la línea."
929 "%1$s Se ha abierto el archivo de estilo de configuración viejo '%2$s'."
$ Msg 986 should not translate "KEYTYPES."
986 "%1$s %2$s: Falta KEYTYPES para la base de datos '%3$s'."
$ Msg 1002 should not translate "KEYTYPE."
1002 "%1$s %2$s: Base de datos '%3$s', carácter KEYTYPE '%4$c' duplicado.\n"
1007 "%1$s%2$s: Se ignorará la cadena vacía %3$s."
$ Msg 1011 should not translate "KEYTYPE," "UPPERKEYS," or "FALSE."
1011 "%1$s %2$s: Base datos '%3$s': No se podrá obtener acceso a anotaciones que contienen \n\
  el carácter KEYTYPE '%4$c' en minúscula.\n\
  Configure UPPERKEYS = FALSE si se desean utilizar claves en minúsculas."
1404 "%1$s El estado del archivo de configuración '%2$s': %3$s no está disponible."
$ Msg 1643 should not translate "DTSROCFPATH," "HOME," "/etc/inittab,"
$ or "/etc/inetd.conf."
1643 "%1$sNo se puede encontrar o abrir el archivo de configuración '%2$s'\n\
de ninguna de las siguientes ubicaciones:\n\
  Directorio DTSROCFPATH =     %3$s,\n\
  Directorio de trabajo actual = %4$s,\n\
  Directorio HOME =            %5$s,\n\
  Directorio /etc/inittab =    %6$s,\n\
  Directorio /etc/inetd.conf = %7$s."
1869 "%1$s %2$s: El archivo de configuración debe especificar una base de datos como mínimo."

$ ****************************************************
$ set 9 formerly set MS_oeinit, used in oeinit.c and opendblk.c.
$set 9

$ Msg 99 should not translate "open_dblk".  It is a function name.
99 "%s Error de programación: Entrada de datos a open_dblk() no válida."
137 "%1$s La versión de interfaz del usuario %2$s y la versión del motor %3$s son incompatibles."
202 "%s La inicialización ha fallado debido a que el archivo de configuración contiene errores."
265 "%s No quedan bases de datos válidas."
266 "%1$s La inicialización ha fallado debido a errores de la base de datos,\n\
  idioma o archivos relacionados."
317 "%1$s No se puede abrir el archivo de la base de datos '%2$s'\n\
  Error número %3$d = %4$s\n\
  %5$s está quitando '%6$s' de la lista de bases de datos disponibles."
318 "\n  Generalmente esto se puede corregir especificando un\n\
  PATH de base de datos válido en la ubicación del archivo de configuración."
1404 "%1$s Se quita la base de datos '%2$s' de la lista de bases de datos disponibles debido a que el estado del archivo %3$s no está disponible: %4$s"

$ ****************************************************
$ set 10 formerly set MS_oe, used in oe.c and dbchange.c.
$set 10
48 "OE48 Se canceló la solicitud: La base de datos '%1$s' no está disponible en esta ubicación."
71 "OE71 %1$s ha expirado."
$ Msg 76 should not translate "dblk".  It is a structure name.
76 "OE76 Error del programa: No se puede encontrar dblk para la base de datos '%1$s'."
217 "OE217 Error del programa: El texto de la anotación es demasiado largo."
367 "OE367: Error de la interfaz del usuario.  %1$d es un código de solicitud no válido.\n"
429 "OE429 No se puede abrir el archivo de eliminación '%1$s': %2$s. Se ignorará la solicitud."
440 "OE440 No se seleccionaron tipos de clave en la base de datos '%1$s'."
490 "OE490 El motor %1$s está desactivado de forma permanente."
516 "OE516 Las cadenas de búsqueda no pueden contener más de %1$d caracteres."
523 "OE523 Solicitud denegada: La primera solicitud debe ser la inicialización del motor."
753 "%1$s Las categorías semánticas no están definidas para la base de datos '%2$s'."
1300 "%1$s No se puede obtener acceso al estado del archivo de la base de datos\n  '%2$s': %3$s"
1313 "%1$s *** SOLICITUD CANCELADA *** %2$s Se ha reinicializado el motor\n\
  ya que se ha modificado el archivo %3$s, probablemente debido\n\
  a la actualización de bases de datos."


$ ****************************************************
$ set 11 formerly set MS_cravel, used in dtsrload.c.
$set 11
1 "\n\
USO: %1$s -d<nombre de la base de datos> [opciones] <archivo de entrada>\n\
       Se pueden reemplazar las extensiones de nombre de archivo predeterminadas.\n\
  -d<nombre de la base de datos>  Nombre de la base de datos de 1 a 8 caracteres. Incluye el prefijo de ruta opcional.\n\
              Las extensiones de nombre de archivo se anexan automáticamente.\n\
  -t<etxstr>  Fin de la cadena del delimitador del documento de texto.  Valor predeterminado '\\f\\n'.\n\
  -c          Inicializar la cuenta de anotaciones de la base de datos contando anotaciones.\n\
  -p<N>       Imprimir un punto de progreso cada <N> anotaciones (valor predeterminado %2$d).\n\
  -h<N>       Cambiar tamaño de tabla de cálculo de cable de anotación de identificación de %3$ld a <N>.\n\
              -h0 significa que no hay duplicados. No los busque.\n\
  -e<ruta>    Ruta o nombre de archivo de la tabla de codificación de huffman (valor predeterminado %4$s).\n\
  <archivo de entrada>    Nombre de archivo [ruta] de entrada. Extensión predeterminada: %5%s.\n"
2 "%1$s: Código de salida = %2$d\n"
13 "%1$s Verificación de identificación de anotación desactivada.\n"
14 "\n%1$s Argumento de línea de comando '%2$s' desconocido.\n"
15 "\nFalta el nombre de archivo de entrada requerido.\a"
16 "\nNo se ha especificado el nombre de la base de datos (argumento -d).\a"
17 "%1$s Se inicializa la cuenta total de anotaciones de la base de datos contando...\n"
18 "%1$s: Versión de esquema '%2$s' versión = %3$s, total de anotaciones = %4$ld, última posición = %5$ld.\n"
19 "%1$s: Total final de anotaciones de la base de datos = %2$ld, última posición = %3$ld.\n"
20 "%1$s %2$ld anotaciones de entrada procesadas en %3$ldm %4$lds, (%5$ld%%).\n\
  %6$ld duplicadas, %7$ld nuevas, %8$ld actualizaciones.\n"
29 "%1$s No se puede obtener el estado de %2$s: %3$s\n"
30 "%1$s El archivo %2$s está vacío.\n"
31 "%1$s: Cada punto = %2$d anotaciones procesadas.\n"
33 "%1$s Se ha desechado la anotación número %2$ld:  La clave es demasiado larga:\n  '%3$s'.\n"
34 "%1$s Se ha agotado la memoria.  Configure el argumento -h con un número más pequeño\n\
  o disminuya la cantidad de anotaciones de entrada."
35 "%1$s: Se ha desechado la anotación número %2$ld '%3$s' duplicada.\n"
246 "\n%1$s '%2$s' es un nombre de ruta o diccionario no válido.\n"
522 "%s Esta versión de %s no soporta bases de datos semánticas.\n"
$ Msg 579 should not translate ".fzk."
579 "%1$s Se ha desechado la anotación número %2$ld:  Formato del archivo .fzk no válido.\n"
582 "%1$sSe ha ignorado el argumento de punto de progreso '%2$s' no válido.\n"
666 "%1$sError de programación: Estado desconocido = %2$d.\n"
717 "%1$s Se cancelará: El texto descomprimido no está soportado todavía.\n"
844 "%1$sSe cancelará cuando haya %2$ld advertencias.\n\
  El total de advertencias está especificado en la línea de comando.\n"
926 "%1$s Se ignorará el argumento -m: %2$s: %3$s.\n"
927 "%1$sSe ha desechado la anotación número %2$ld:  El primer carácter (tipo de clave) de la clave\n\
  '%3$s' no es alfanumérico.\n"
1048 "%1$sLos objetos de la base de datos no se han modificado porque el archivo e entrada está vacío.\n"
1086 "%1$s El formato de la anotación '%2$s' no es válido.  Se utilizará la fecha de ejecución.\n"
1153 "%1$s La identificación de la tabla de compresión de datos actual '%2$s' es %3$ld.\n\
  La base de datos '%4$s' ha sido comprimida anteriormente mediante la tabla %5$ld.\n"
1215 "%1$s Advertencia.  La anotación '%2$s' no contiene texto.\n"


$ ****************************************************
$ set 12 formerly set MS_initausd, used in dtsrcreate.c.
$set 12

$ Msg 3 should not translate "dtsearch.dbd."
3 "\n\
USO: %1$s [opciones] <nombre de la base de datos>\n\
       Crea e inicializa los archivos de base de datos DtSearch/AusText.\n\
  -q       No imprimir mensajes de información.\n\
  -o       Permiso para sobreescribir la base de datos preexistente.\n\
  -a<N>    Fijar tamaño de resumen máximo en <N> (predeterminado por variedad).\n\
  -d<dir>  Dónde encontrar el archivo dtsearch.dbd si no se encuentra en el directorio <nombre de base de datos>.\n\
  -wn<N>   Cambiar tamaño mínimo de palabra a <N>.  Valor predeterminado: %2$d.\n\
  -wx<N>   Cambiar tamaño máximo de palabra a <N>.  Valor predeterminado por idioma.\n\
  ---------- Variedad de base de datos ----------\n\
  -fd      Variedad DtSearch.  Sin documentos, solamente referencias a documentos\n\
           de resumen (predeterminado).\n\
  -fa      Variedad AusText.  Documentos almacenados en el depósito central del servidor.\n\
  ------------ Idiomas soportados ------------\n\
  -l<N>    Fijar el número de idioma en <N>.  Valor predeterminado: 0.  Valores soportados:\n\
           0 Inglés-ASCII\n\
           1 Inglés-Latin1\n\
           2 Español\n\
           3 Francés\n\
           4 Italiano\n\
           5 Alemán\n\
           6 Japonés-autoknj\n\
           7 Japonés-knjlist\n\
  <nombre de la base de datos> Prefijo de ruta opcional, después el nombres\n\
           de la base de datos de 1 a 8 caracteres.  No especificar 'austext' ni 'dtsearch'.\n"

5 "DTSRCREATE El tamaño mínimo de palabra %1$d es mayor que el máximo %2$d.\n"
6 "DTSRCREATE Se ajustó el tamaño mínimo de palabra a %1$d.\n"
8 "DTSRCREATE Se ajustó el tamaño máximo de palabra en %1$d.\n"
9 "DTSRCREATE La especificación de pequeños tamaños mínimos de palabra puede requerir\n\
  numerosas modificaciones del archivo de palabras de parada para\n\
  evitar que el almacenamiento de índices ocupe más espacio.\n"
10 "DTSRCREATE La especificación de grandes tamaños máximos de palabras puede\n\
  aumentar mucho el espacio de almacenamiento necesario.\n"
$ Msg 12 needs option "y" for "yes" to be translated the same as in msg 15.
$ Option "n" does not need a translation because the code assumes anything
$ other than the translated "y" option means "no."
12 "\nEl archivo '%s' ya existe.\n\
¿Desea sobreescribir dicho archivo y otros de la base de datos? [s,n] "
$ Msg 13 should not translate "load_custom_language."
13 "%s ¡Advertencia! Ha especificado un idioma personalizado no soportado.\n\
  Deberá suministrar sus propios cargadores de idiomas al ejecutar\n\
  en la función de usuario 'load_custom_language' para obtener acceso a esta base de datos.\a\n"

14 "\n\
El procesamiento semántico %s sólo está disponible para bases de datos en inglés.\n\a"
$ Msg 15 needs "y" to match the "yes" option in msg 12.
15 "s"
24 "DTSRCREATE Ha inicializado la base de datos '%1$s' correctamente.\n"
98 "DTSRCREATE %1$s Identificación de tabla de compresión Huffman no válida."
186 "DTSRCREATE186 No se puede cambiar el nombre de '%1$s' a '%2$s':\n  %3$s\n"
205 "DTSRCREATE205 No se pueden cambiar los permisos de '%1$s': %2$s\n"
213 "%1$s Incapaz de abrir '%2$s':\n  %3$s.\7\n"
214 "%1$s Incapaz de copiar '%2$s' en '%3$s':\n  %4$s\n"
230 "DTSRCREATE230 No se pudo abrir la base de datos '%1$s'.\n"
239 "DTSRCREATE239 No se pudo inicializar la base de datos '%1$s'.\n"
244 "DTSRCREATE244 No se pudo quitar '%1$s':\n  %2$s\n"
326 "DTSRCREATE326 No se pueden cambiar los permisos de '%1$s':\n  %2$s\n\
  Se deben poder leer y escribir todos los archivos de base de datos.\n\
  Es posible que no se puedan abrir ni inicializar archivos de la base de datos.\n"
327 "  Sin embargo, es posible que no pueda utilizarse a menos que todos los archivos\
  de la base de datos cuenten con permisos para leer y escribir."
433 "%1$sSe ajustó el tamaño máximo de resumen para aumentarlo a %2$hd.\n"
509 "DTSRCREATE509 No se pudo inicializar la anotación del registro del encabezamiento.\n"

$ ****************************************************
$ set 13 formerly set MS_vista.
$ Msg numbers 1-100 and 900-1000 match
$ user_error[] and system_error[] respectively in dberr.h.
$ Used in dberr.c, ausdopen.c, and opendblk.c.

$ Msgs 1-100 and 900-1000 in set 13 are generated during database
$ input/output.  They are all fatal errors and should be treated as
$ standalone messages.  They are not combined with other messages to
$ produce errors.  The messages come from a third party vendor, so we
$ do not intend to modify the vendor code and make complete sentences
$ out of them.

$set 13

$ For msgs 1-100, see comments at top of set 13.
1 "no se ha abierto la base de datos"
2 "conjunto no válido"
3 "anotación no válida"
4 "base de datos no válida"
5 "nombre de campo no válido"
6 "db_address no válido"
7 "no hay ninguna anotación actual"
8 "el conjunto no tiene propietario"
9 "el conjunto no tiene miembros"
10 "el valor clave es necesario"
11 "valor de bloqueo no válido"
12 "la anotación es propietaria de conjuntos no vacíos"
13 "la anotación es miembro de conjuntos"
14 "el miembro ya tiene propietario"
15 "el campo es una clave compuesta"
16 "la anotación no está conectada con el conjunto"
17 "el campo no es una clave válida"
18 "la anotación no es propietaria válida del conjunto"
19 "la anotación no es miembro válido del conjunto"
20 "error en d_setpages (la base de datos está abierta o el parámetro es incorrecto)"
21 "archivo de diccionario incompatible"
22 "intento no válido de eliminar la anotación del sistema"
23 "intento no válido de bloquear un conjunto o anotación bloqueados"
24 "intento de obtener acceso a un conjunto o anotación desbloqueados"
25 "no se suministró la identificación de la transacción"
26 "la transacción ya está activa"
27 "la transacción no está activa"
28 "la transacción no puede comenzar debido a archivos bloqueados"
29 "no se pueden desbloquear bloqueos dentro de una transacción"
30 "demasiadas páginas de base de datos cambiadas"
31 "no se puede actualizar una base de datos afuera de una transacción"
32 "se requiere acceso exclusivo"
33 "los archivos estáticos no pueden contener bloqueos de escritura"
34 "identificación de usuario no especificada o duplicada"
35 "nombre de base de datos, ruta o archivo demasiado largo"
36 "se envió un número de archivo a d_renfile"
37 "el campo no es de clave opcional"
38 "el campo no está definido en el tipo de anotación actual"
39 "la anotación o campo contiene una clave compuesta"
40 "número de anotación o conjunto no válido"
41 "la anotación o conjunto no tiene sello de hora"
42 "DBUSERID incorrecto (contiene un carácter no alfanumérico)"
43 "NetBIOS no se ha instalado en la computadora"
44 "la cantidad de elementos de DBDPATH es incorrecta"
45 "la cantidad de elementos de DBFPATH es incorrecta"
46 "no es del tipo de anotación actual"
47 "cadena de tabla de clasificación de país no válida"
48 "la base de datos no está cerrada"
$ Msg 301 should not translate "errno."
301 "\n*** Núm. de error de E/S de sistema %1$d = %2$s\n"
302 "\n\
*** Este error se debe frecuentemente a que un\n\
*** archivo de la base de datos falta o es para leer solamente o se ha\n\
*** excedido un límite de la cantidad de archivos abiertos."
303 "(ubicación no especificada)"
$ Msg 304 should not translate "db_VISTA."
304 "\n*** Recuperación automática db_VISTA en curso...\n"
$ Msg 311 should not translate "db_status."
311 "*** Error de base de datos en %1$s, db_status = %2$d: %3$n"
312 "Error de programación."
313 "Error desconocido."
315 "La base de datos no está disponible debido a que otros usuarios la están utilzando."
348 "%1$s Nombre de base de datos '%2$s' no válido."
378 "%1$s No se pudo abrir la siguiente cadena de nombre de base de datos:\n  '%2$s'"
379 "%1$s No se pudo abrir la base de datos '%2$s':\n  %3$s."

$ For msgs 900-1000, see comments at top of set 13.
900 "no hay más espacio en el archivo"
901 "error del sistema"
902 "falla de página"
$ Msg 903 should not translate "dio."
903 "no hay archivos de trabajo en dio"
904 "no se puede asignar suficiente memoria"
905 "error al abrir el archivo"
906 "no se pudo obtener acceso al archivo taf de la base de datos"
907 "error del archivo taf o registro de la base de datos"
908 "bloqueos de base de datos inconsistentes"
909 "se excedió el límite de anotaciones del archivo"
910 "inconsistencia del archivo de claves"
911 "se excedió el límite actual máximo de usuarios"
912 "error al buscar en el archivo de la base de datos"
913 "se ha especificado un archivo no válido"
914 "error al leer de un archivo de base de datos o desbordamiento"
915 "error de sincronización del gestor de bloqueos"
916 "interrupción de la verificación de depuración"
917 "error de comunicación de red"
918 "recuperación automática en curso"
919 "error al escribir en el archivo de base de datos o desbordamiento"
920 "no se ha instalado ningún gestor de bloqueos"
921 "DBUSERID ya está siendo usado"
922 "el gestor de bloqueos está ocupado"


$ ****************************************************
$ set 14 formerly set MS_cborodin, used in dtsrindex.c.
$set 14

1 "%1$s.  Ejecución %2$s.\n"
2 "%1$s: Se han detectado advertencias."
$ Msg 6 should not be translated.
6 "put_addrs_2_dtbs_addr_file() nitems=%1$d, batchsz=%2$ld\n"
$ Msg 9 should not translate "d99."
9 "\n\
\n%1$s Archivo d99 dañado para palabra '%2$s',\n\
  dirección de base de datos %3$ld @ posición de archivo %4$ld => bitvector[%5$ld],\n\
  pero la asignación máxima de bitvector = %6$ld.\n"
15 "%1$s Menos de 2 bytes en la entrada.\n"
17 "\n\
USO: %1$s -d<nombre de la base de datos> [opciones] <archivo de entrada>\n\
       Las extensiones de nombre de archivo predeterminadas pueden reemplazarse.\n\
  -d<nombre de base de datos>  Nombre de base de datos de 1 a 8 caracteres, incluye el prefijo de ruta opcional.\n\
  -t<cadena de fin de texto>  Cadena de delimitación de fin de texto del documento.  Valor predeterminado '\\f\\n'.\n\
  -r<N>       Cambiar pasar 1 anotaciones por punto de %2$d a <N>.\n\
  -b<N>       Cambiar tamaño máximo de lote de %3$ld a <N>.\n\
  -c<N>       Cambiar caché de paginación de base de datos de páginas de %4$ld 1K a <N> 1K.\n\
              <N> >= 16 en potencias de 2.  Inicialmente intente efectuar cambios pequeños.\n\
  -i<N>       Cambiar tamaño de memoria intermedia (i)nput del valor predeterminado %5$d a <N>.\n\
  -h<N>       Cambiar tamaño de tabla de cálculo de clave de identificación de anotación duplicada de %6$ld a <N>.\n\
              -h0 significa que no hay duplicados. No los busque.\n\
  <archivo de entrada>    Nombre de archivo [ruta] de entrada.  Extensión predeterminada: %7$s.\n"
21 "\n\
\nLa anotación %s '%1$s' desborda la matriz del contador de palabras.\n\
  Anotación número %2$ld > maxdba %3$ld, dba=%4$ld, sld00=%5$d, offs=%6$d.\n"
22 "\n\
\nLa anotación %1$s '%2$s' en la base de datos (dba=%3$ld)\n\
  desborda la asignación de bitvector (%4$ld >= %5$ld).\n"
33 "\n%1$s La cantidad de anotaciones recibidas excede %2$d.\n\
  Generalmente se producirá un error  'se ha agotado el espacio de paginación' en el pase 2\n\
  y se dañará la base de datos.  Divida el archivo recibido para disminuir el\n\
  total de anotaciones o utilice la opción -b y vuelva a ejecutar.\n"
40 "Se han ignorado %1$ld anotaciones duplicadas"
41 "No se han encontrado anotaciones duplicadas"
164 "\n%1$s: %2$s Se ha cancelado debido a la señal %3$d.  Es posible que la base de datos %4$s\n\
  esté dañada.  Restaura la base de datos de respaldo.\n"
206 "%1$s: Código de salida = %2$d, tiempo transcurrido %3$ldm %4$lds.\n"
$ Msg 280 should not be translated.
280 "\n%1$s num_addrs_for_word (%2$ld) >= batchsz (%3$ld).\n"
288 "%1$s Cancelar. No hay palabras en el archivo de entrada %2$s.\n"
374 "\n%1$s Se ha agotado la memoria.\n\
  Divida el archivo recibido en varios archivos más pequeños e intente de nuevo.\n"
539 "%1$s La verificación de identificaciones de anotaciones duplicadas está desactivada.\n"
550 "%1$s '%2$s' es un nombre de ruta o base de datos no válido.\n"
558 "%1$s Tamaño de memoria intermedia de entrada '%2$s' no válido.\n"
567 "%1$s Argumento de línea de comando '%2$s' desconocido.\n"
577 "%1$s Argumento '%2$s' no válido.  Se utiliza el argumento predeterminado -r%3$d.\n"
580 "%1$s Falta el nombre de archivo de entrada requerido.\n"
589 "%1$s No se ha especificado ningún nombre de base de datos (argumento -d).\a\n"
595 "%1$s Argumento de tamaño de lote '%2$s' no válido.\n"
600 "%1$sEl tamaño del caché se ha vuelto a ajustar en %2$d.\n"
659 "\n%1$s: %2$s Se desechará la anotación. La clave es demasiado larga:\n  '%3$s'.\n"
713 "\n%1$s La palabra '%2$s' aparece en %3$ld anotaciones,\n\
  pero sólo hay %4$ld anotaciones en la base de datos.\n\
  (Este puede ser un buen candidato para la lista de parada).\n"
727 "\n%1$s La cantidad de anotaciones dbaddr (%2$ld)\n\
excede la asignación de bitvector (%3$ld).\n"
$ Msg 776 should not translate "d99."
776 "%1$s Falló la escritura en el archivo d99: %2$s\n"
$ Msg 848 should not translate "d99."
848 "\n%1$s No se pudieron leer %2$ld bytes (dbas %3$ld) del archivo d99\n\
  en el desplazamiento %4$ld.  Cantidad de dbas leídos (código devuelto) = %5$ld.\n"
849 "\n%1$s: Cantidad de palabras %2$ld, %3$.0f%% terminado.  Estimación %4$lum %5$02lus hasta finalizar.\n"
$ Msg 875 should not translate "fseek" or "d99."
875 "\n%1$s No se pudo fseek archivo d99 hasta el desplazamiento %2$ld.\n"
1068 "%1$s No se puede abrir el nuevo archivo de índice invertido '%2$s': %3$s\n"
1083 "%1$s No se puede leer el archivo de entrada '%2$s': %3$s\n"
1097 "%1$s Se cancela debido a errores al cargar archivos de idioma.\n"
1108 "%1$s: Iniciando el pase 1, leyendo anotaciones de '%2$s'.\n\
   Cada punto representa = %3$d anotaciones.\n"
$ Msg 1129 should not translate ".fzk."
1129 "%1$s: %2$s El formato del archivo .fzk no es válido.\n"
1168 "%1$s: %2$s Se ha desechado '%3$s', la clave no se encuentra en la base de datos.\n"
1190 "\n%1$s: Anotación número %2$lu, %3$.0f%% terminada.  Estimado %4$lum %5$02lus hasta finalizar el pase 1.\n"
1225 "%1$s: Pase 1 completado en %2$lum %3$lus, se leyeron %4$lu anotaciones.\n\
  %5$s, se ha efectuado el análisis sintáctico de %6$lu palabras.\n"
1233 "%1$s: Iniciando el pase 2: desplazamiento a través del índice de lote y actualización de base de datos.\n\
  Cada punto representa = %2$d palabras.\n"
1246 "%1$s: El pase 2 se ha completado en %2$lum %3$lus, se han actualizado %4$lu palabras.\n"
1402 "%1$s: Se ha desechado e número de anotación duplicada %2$lu '%3$s'.\n"


$ ****************************************************
$ set 15 formerly set MS_delsfx, now MS_lang, used in lang.c and jpn.c
$set 15

$ Msg 4 should not translate "dblk."  It is a structure name.
4 "%s dblk no se ha inicializado.\n"
$ Msg 5 should not translate "parg."
5 "%s Error del programa: parg no contiene archivos ni cadenas.\n"

8 "\n\
%1$s '%2$.*s...' es mayor\n\
que el tamaño máximo de palabra de la base de datos '%3$s'."

10 "%s Error del programa: Estado del analizador de sintaxis desconocido.\n"
12 "%1$s '%2$.120s' no es una palabra válida en la base de datos '%3$s'."

17 "\n\
%1$s '%2$s' menor que\n\
el tamaño mínimo de palabra de la base de datos '%3$s'."

19 "%1$s La palabra '%2$s' no está indexada en la base de datos '%3$s'."
20 "%1$s Error del programa: Estado j %2$d desconocido.\n"
21 "%1$s '%2$.*s' no es una palabra japonesa válida."
23 "%1$s La palabra '%2$s' de la línea %3$ld es demasiado corta."
24 "%1$s No hay palabras japonesas en el archivo de palabras '%2$s'."
25 "%1$s La ruta de la base de datos '%2$s' es demasiado larga: '%3$s'."
31 "%s Nunca se cargaron sufijos Stemmer de archivos.\n"
$ Msg 41 should not translate "Paice."
41 "%1$s %2$s: Regla Paice no válida comenzando en '%3$s'."
$ Msg 42 should not translate "Paice."
42 "%1$s %2$s: Reglas Paice fuera de orden comenzando en '%3$s'."
45 "%s Se ha agotado la memoria.\n"
50 "Inglés-ASCII"
$ Msg 51 refers to the Latin 1 codeset.
51 "Inglés-Latin1"
52 "Español"
53 "Francés"
54 "Italiano"
55 "Alemán"
56 "Japonés-comp"
57 "Japonés-.knj"


$ ****************************************************
$ set 16 formerly set MS_vewords, now MS_boolsrch, used in boolsrch.c.
$set 16

$ Msg 2 is like "<program name> Part of ... term <word> which is
$ not in database <database name>."
2 "\n\
%1$s Parte de la operación de colocación incluye\n\
el término '%2$s' que no se encuentra en la base de datos '%3$s'."
$ Msg 6, variable 3 is the database name.
6 "\n\
%1$s Error de la base de datos.  La palabra '%2$s' está\n\
listada en la base de datos '%3$s' pero no cuenta con anotación de índice."
$ Msg , variable 3 is the database name.
11 "%1$s La palabra '%2$s' no se encontró en la base de datos '%3$s'."
$ Msg 14 should not translate "WHITLIM."  The message is like
$ "<Nombre del programa> <palabra de la consulta> ha sido encontrada más de <número> de veces."
14 "\n\
%1$s '%2$s' ha sido encontrada más de %3$ld veces.\n\
Por favor elimínela de la consulta o aumenta el valor de WHITLIM\n\
del archivo de configuración del motor de búsqueda."
15 "\n\
%s Para esta consulta se deben obtener todos los\n\
documentos de la base de datos que no contienen ninguna\n\
palabra de consulta.  Este tipo de búsqueda puede tomar\n\
mucho tiempo."

$ Msg 28 should not translate "d99."  Variable 2 is the database name.
28 "%1$s Error de lectura de la base de datos en %2$s.d99."
$ Msg 32 should not translate "d99."  The message is formed using either
$ msg 33 or 34 as variable 2, and it is like "<program name> Database
$ Error: Word <word> in database <database name> has invalid d99 record
$ number <number>."
32 "\n\
%1$s Error de la base de datos: %2$s '%3$s'\n\
en la base de datos '%4$s' tiene el número de anotación %5$ld no válido de d99."
$ Msg 33 is used for variable 2 of message 32.
33 "Palabra"
$ Msg 34 is used for variable 2 of message 32.
34 "Raíz de"
$ Msg 35 should not translate "stemct," and "pmsz."
35 "%1$s Error del programa: stemct=%2$d pmsz=%3$d\n"

$ ****************************************************
$ set 17 formerly set MS_vestatis
$set 17

104 "No se puede desanexar memoria compartida."
107 "Desbordamiento de la pila Qsort.\n"
110 "No se puede quitar memoria compartida."
115 "No hay memoria compartida disponible"
310 "%1$s Se agotó la memoria.  Se necesitan %2$ld bytes.\n"
314 "Se agotó la memoria.\n"
421 "Cantidad total de elementos encontrados = %1$ld. Se han desechado los elementos encontrados que excedían la cantidad especificada."
$ Msg 437 should not be translated.
437 "%1$s db_status = %2$d, dba = %3$d:%4$ld (x'%5$08.8lx'), vistano = %6$d.\n"
504 "No se puede asignar cur_ditto.\n"
677 "El campo de consulta está vacío."
990 "El sistema está clasificando. Espere por favor."


$ ****************************************************
$ set 23 formerly set MS_dbrec

$ Set 23 has several words in parenthesis which should not be translated.
$ For example, msg 3: "objkey,"  msg 5: "fzkeysz," msg 12: "abstrsz," 
$ msg 24: "minwordsz," msg 26: "maxwordsz," ...  The single words in
$ parenthesis should not be translated.
$ Msgs like 10, where the entire text is in parentheses should be translated.

$set 23

1 "---------- Valores de sistema de la base de datos '%1$s' ----------\n"
2 "Número de versión del esquema (versión): '%1$s'.\n"
$ Msg 3 should not translate "sizeof" or "objkey."
3 "Tamaño máximo de clave de objeto (sizeof(objkey)): %1$ld bytes.\n"
$ Msg 4 "Huge" means very, very large.
4 "Claves opcionales 'gigantes' activadas.\n"
5 "Cantidad máxima de categorías semánticas (fzkeysz): %1$d.\n"
10 "  (El acceso semántico a objetos está desactivado)."
12 "Longitud máxima de la cadena de resumen (abstrsz): %1$d.\n"
14 "  (No se utilizan resúmenes en esta base de datos)."
20 "Los resúmenes están comprimidos mediante %1$s.\n"
22 "Número de idioma de análisis sintáctico: %1$d, %2$s.\n"
24 "Longitud mínima de palabra (minwordsz): %1$d.\n"
26 "Longitud máxima de palabra (maxwordsz): %1$d.\n"
30 "Cantidad de posiciones .d00 por objeto (recslots): %1$d.\n"
36 "  (Cantidad máxima de objetos de base de datos: %1$ld).\n"
40 "Identificación de tabla de compresión Huffman (hufid): %1$ld.\n"
42 "  (La compresión está desactivada en esta base de datos)."
44 "  (Todavía no se ha determinado una tabla de compresión específica)."
50 "La capacidad de acceso del motor a datos puede variar dependiendo del objeto."
54 "No se puede obtener acceso a objetos de datos directamente desde el motor."
56 "Los objetos de datos se almacenan internamente como texto comprimido."
60 "Sólo referencias de archivos de servidor a objetos se almacenan en texto comprimido."
64 "Sólo referencias de archivos de cliente a objetos se almacenan en texto comprimido."
68 "Las claves de objeto son referencias de archivo de servidor correspondientes a los objetos."
72 "Las claves de objeto son referencias de archivo de cliente correspondientes a los objetos."
74 "Las referencias de archivo correspondientes a objetos se almacenan en claves 'gigantes'."
80 "Las referencias de archivo de servidor correspondientes a objetos se almacenan en los resúmenes."
86 "Las referencias de archivos de clientes correspondientes a objetos se almacenan en los resúmenes."
90 "Error: no se conoce el significado del valor (%1$hd) de or_dbaccess.\n"
100 "El texto comprimido del almacenamiento se ha comprimido mediante %1$s.\n"
110 "El texto comprimido del almacenamiento no se utiliza en esta base de datos."
120 "Los selectores de base de datos (dbflags) son 0x%1$lx:\n"
$ Msgs 124 and 125 are used in the variable in messages 130 and 140.
$ For example: "Inverted index INCLUDES words exactly as parsed."
124 "INCLUYE"
125 "EXCLUYE"
$ Msgs 126 and 127 are used in the variable in messages 160, 162, 164.
$ For example, "Use of optional 'huge' keys is ENABLED."
126 "ACTIVADO"
127 "DESACTIVADO"
$ Msg 130 uses msgs 124 and 125 in the variable.
130 "  El índice invertido %1$s palabras exactamente como se han analizado sintácticamente.\n"
$ Msg 140 uses msgs 124 and 125 in the variable.
140 "  El índice invertido %1$s raíces de palabras.\n"
$ Msg 160 uses msgs 126 and 127 in the variable.
160 "  El uso de claves opcionales 'gigante' está %1$s.\n"
$ Msg 162 uses msgs 126 and 127 in the variable.
162 "  La marca para eliminación está %1$s.\n"
$ Msg 164 uses msgs 126 and 127 in the variable.
164 "  Las notas de usuario anexables están %1$s.\n"
$ Msg 170 is followed by either msg 172 or msg 174.
$ For example, "Text characters are a SINGLE byte wide."
170 "  Los caracteres de texto tienen un ancho de %1$s.\n"
172 "DOS bytes"
174 "un SOLO byte"
200 "La cantidad actual de objetos de base de datos (reccount) es %1$ld.\n"
210 "El número de la última posición utilizada actualmente (maxdba) es %1$ld.\n"
310 "\n\
USO: %1$s <nombre de la base de datos>\n"
330 "No se pudo abrir la base de datos '%1$s'.\n%2$s\n"
340 "No hay ninguna anotación dbrec en la base de datos '%1$s'.\n"
350 "No se puede leer la anotación dbrec en la base de datos '%1$s'.\n%2$s\n"

$ ****************************************************
$ set MS_dtsrkdump is set 25.  5/20/96.  dtsrkdump.c
$set 25

1 "%s Cancelación debido a error del programa.\a\n"
$ Msg 2 should not be translated
2 "%1$c dba=%2$d:%3$-7ld ofs=%4$-9ld adr=%5$-6ld fre=%6$ld\n"
$ Msg 3 is like "<program name> <RCS revision number>, engine
$ <DtSearch version>.  <time stamp>."
3 "%1$s %2$s, motor %3$s.  %4$s.\n"
$ Msg 4, option "-p" can include decimal points -- this is handled by
$ using sscanf, so if sscanf can read numbers with commas in them, then
$ the "-p" option will process that correctly.  atof is used on what
$ sscanf reads in.
4 "\n\
USO: %1$s -o|w|ow [-v] [-t<N> | -p<N>] <nombre de la base de datos>\n\
       Lee archivos de clave de DtSearch e imprime el informe general.\n\
  -o        Las claves examinadas son claves de anotación OBJETO.\n\
  -w        Las claves examinadas son PALABRAS de índice invertido.\n\
  -v        Modo VERBOSO, lista todas las claves.\n\
  -t<N>     Umbral.  Configura las opciones w y v, y lista solamente palabras\n\
            con direcciones >= <N>.  Todas las palabras se listarán si <N> = 1.\n\
  -p<N>     Otro umbral.  Es igual a -t excepto que <N> es un porcentaje\n\
            de toda la base de datos (<N> puede incluir un punto decimal).\n\
            Por ejemplo -p99.9 imprime todas las palabras que aparecen\n\
            en 99,9%% o más de las anotaciones. Es una manera excelente de\n\
            buscar candidatos para la lista de parada.\n\
            Si w y v se configuran sin umbral, el valor predeterminado es -t%2$d.\n\
  <nombre de la base de datos>  Nombre de base de datos de 1 a 8 caracteres con un prefijo de ruta opcional.\n"
5 "%1$s Valor de porcentaje %2$lf no válido.\a\n"
53 "%1$s Valor de umbral no válido.\a\n"
55 "%1$s Argumento de línea de comando '%2$c' no válido.\a\n"
56 "%1$s Falta el nombre de base de datos necesario.\a\n"
57 "%1$s Se debe especificar -o o -w.\a\n"
58 "%1$s Nombre de base de datos '%2$s' no válido.\a\n"
$ Msg 60 should not be translated.
60 "%1$s: '%2$s' reccount=%3$ld maxdba=%4$ld recslots=%5$hd minw=%6$hd maxw=%7$hd\n"
70 "%1$s Listará solamente palabras que aparecen en %2$ld o más anotaciones.\n"
80 "%1$s: Se listarán todas las palabras de la base de datos.\n"
$ Msg 100 should not be translated.
100 "dba x%1$08lx, %2$6ld\n"
$ Msg 110 is like "in <directory name>"
110 " en %1$s"
120 "Resumen de objetos de '%1$s'%2$s:\n"
130 "Total de objetos por tipos de clave:"
160 "TOTAL de objetos = %1$ld\n"
$ Msg 170 should not translate "DBA."
170 "DBA de objetos más grande  = %1$ld\n"
180 "%1$s: * Las palabras marcadas con un asterisco aparecen en todas las anotaciones.\n"
200 "Resumen de palabras de '%1$s'%2$s:\n"
210 "TOTAL de palabras = %1$ld\n"

$ ****************************************************
$ set MS_dtsrclean is set 26.  5/21/96.  dtsrclean.c
$set 26

1 "\n\
USO: %1$s [opciones] <nombre de la base de datos> <nueva ruta>\n\
       Comprime espacio d99 no utilizado y valida enlaces d00-d99.\n\
  -p<N>     Se imprimen puntos de progreso cada <N> anotaciones (valor predeterminado: %lu).\n\
            Se imprimirá un mensaje de progreso completo cada %d puntos.\n\
  -oy       Autoriza la superposición de archivos d99/d2<N> preexistentes en la nueva ruta.\n\
  -on       Fuerza a salir si hay archivos d99/d2<N> preexistentes en la nueva ruta.\n\
  -v        Valida enlaces d99 y d00, repara daños en el archivo d99 y asegura\n\
            que la cuenta sea correcta.  También podrá utilizar -c0 para reparar los daños de toda la base de datos.\n\
  -v<nombre de archivo> Igual a -v pero también escribe todas las anotaciones d00 cuya referencia ha sido cancelada por d99\n\
            en el archivo especificado y en un formato adecuado para extraerlo en el formato de archivo .fzk.\n\
  -c<N>     Sale si hay más de <N> enlaces dañados o incompletos (valor predeterminado %d).\n\
            El límite de corrupción ha sido desactivado por -c0.\n\
  <nombre de la base de datos>  Nombre de la base de datos de 1 a 8 caracteres = los archivos d99/d2<N> viejos a actualizar.\n\
            Archivos que se encuentran en el directorio local.\n\
  <ruta nueva> Especifica dónde se colocarán los nuevos archivos d99/d2<N>.\n\
            Si el primer carácter no es una barra oblicua, la ruta es relativa al directorio local.\n\
CODIGOS DE SALIDA:\n\
  0: Completado correctamente.  1: Advertencia.  2: La tarea nunca se inició.\n\
  3-49: La tarea ha terminado prematuramente, los archivos viejos están bien, los nuevos no se puede utilizar.\n\
  50-99: Error fatal, incluso es posible que la base de datos vieja esté dañada.\n\
  100+: Ctrl-C, detener, y todas las otras señales de interrupción pueden producir la terminación\n\
     prematura, es posible que los nuevos archivos no se puedan utilizar.  Señal = código de salida - 100.\n"
2 "%1$s: %2$s Compresión %3$d%% (aproximadamente %4$lu KB) en %5$ld:%6$02ld min:sec.\n"
$ Msg 3 should not translate "d99."  "WORDS" should be translated.
3 "%1$s: Se han contado %2$ld PALABRAS en %3$s.d99.\n"
4 "%1$s: No se han detectado enlaces dañados.\n"
5 "%1$s: Código de salida = %2$d.\n"
$ Msg 6 should not be translated.
6 "  DBA = %1$d:%2$ld (x%3$02x:%4$06lx),  orig addr val = x%5$08lx\n\
  Word='%6$c%7$s' offset=%8$ld addrs=%9$ld free=%10$d\n"
7 "%1$s: Se copia de los archivos viejos d2<N> en %2$s...\n"
$ Msg 11 is like "<program name> Version <version number>. Run <date program
$ was run>."
11 "%1$s Versión %2$s.  Ejecución %3$s.\n"
$ Msg 14 should not translate "d99."
14 "%1%s: Archivos viejos: '%2$s.d2x, .d99'.\n"
$ Msg 15 should not translate "d99."
15 "%1$s: Archivos nuevos: '%2$s.d2x, .d99'.\n"
24 "%1$s: Ya existen archivos nuevos.\n"
$ Msg 25 should not be translated.
25 "%1$s: curr reccnt=%2$ld, mxdba=%3$ld, sl/rec=%4$ld, tot#adr=%5$ld.\n"
26 "%1$s: Leyendo el archivo d00.  Cada punto equivale aproximadamente a %2$ld documentos de base de datos...\n"
$ Msg 27 should translate DOCUMENTS.
27 "%1$s: Se han confirmado %2$ld DOCUMENTOS en %3$s.d00.\n"
$ Msg 28 should translate DOCUMENTS.
28 "%1$s: Hay %2$ld DOCUMENTOS en %3$s.d00 no es el total = %4$ld almacenado allí.\n\
  El total se corregirá en el nuevo archivo d00.\n"
29 "%1$s: Comprimiendo en %2$s.  Cada punto equivale aproximadamente a %3$lu palabras...\n"
$ Msg 45 needs the "y" for "yes" to match msg 46.
45 "    ¿Desea superponer los archivos en el nuevo directorio? [s/n] "
$ Msg 46 must match the "yes" option from msg 45.
46 "s"
66 "%1$s Se abortará después de la señal de interrupción %2$d.\n"
$ Msg 111 should not translate "d99."
111 "*** DBA %1$s en d99 = -1.  Es posible que se haya desbordado hacia el\n\
  área de expansión debido a que los valores de los totales del archivo d2x no son correctos.\n"
159 "%1$s Argumento desconocido: '%2$s'.\n"
177 "%1$s Argumento %2$.2s no válido.\n"
193 "%1$s Se cancelará cuando haya %2$ld enlaces dañados.\n"
194 "%1$s Se han detectado %2$s %3$ld enlaces dañados o incompletos.\n"
210 "%1$s Falta el argumento de nombre de base de datos requerido.\n"
211 "%1$s Falta el argumento de nueva ruta requerido.\n"
212 "%1$s Demasiados argumentos.\n"
$ Msg 222 should not translate "d99" or "DBA."
222 "*** DBA %1$s en d99 no en d00, posición > cantidad máxima de documentos.\n"
229 "%1$s Nombre de base de datos '%2$s' no válido.\n"
301 "%1$s El argumento -m ya no es necesario.\n"
$ Msg 333 should not translate "d99" or "d00."
333 "*** El DBA %1$s en d99 no existe en d00.\n"
393 "%1$s Los directorios viejo y nuevo son idénticos: '%2$s'.\n"
439 "%1$s No se puede abrir %2$s: %3$s\n"
440 "%1$s No se puede obtener el estado de %2$s: %3$s\n"
$ Msg 444 should not translate "d99."  Do not translate starting with "DBA = ..."
444 "*** No se hace referencia a la anotación d00 %1$s '%2$s' en d99.\n\
  DBA = %3$d:%4$ld (x%5$02x:%6$06lx).\n"
463 "%1$s El argumento de la línea de comando no permite superponer archivos.\n"
$ Msg 465 "Validate" should match the description for the "-v" option in msg 1.
465 "%1$s ADVERTENCIA: No se puede asignar memoria para el vector de bits.\n\
  El modo 'Validación' está desactivado.\n"
489 "%1$s Error de escritura en %2$s: %3$s.\n"
499 "%1$s %2$s está vacío.\n"
517 "%1$s Error de lectura en %2$s: %3$s.\n"
$ Msg 561 should not translate "DBA," "maxdba," or "dbrec."
561 "%1$s DBA '%2$d:%3$ld' (anotación número %4$ld) en d00 excede la cantidad total de direcciones %5$ld;\n\
  Desbordamiento de vector de bits debido a que maxdba %6$ld de dbrec no es correcto.\n"
$ Msg 657 is like "<program name> Read error on <filename>: <system errno>."
657 "%1$s Error de lectura en %2$s: %3$s.\n"
$ Msg 665 is like "<program name> Write error on <filename>: <system errno>."
665 "%1$s Error de lectura en %2$s: %3$s.\n"
$ Msg 802 is like "<program name> Unable to open <filename> to output
$ unreferenced d00 records: <system errno>."  Do not translate "d00."
802 "%1$s No se puede abrir '%2$s' para introducir anotaciones de d00 a las que no se hace referencia:\n  %3$s\n"

$ ****************************************************
$ set MS_dui is set 27.  5/21/96.  dui.c
$set 27
1 "%1$s DtSearch Versión %2$s.\n\
USO: %3$s [-opciones] <consulta>...\n\
       Las opciones que comienzan con el mismo carácter son mutuamente exclusivas.\n\
  -ss         Tipo de búsqueda S (raíces de palabras) en vez de la búsqueda estadística predeterminada.\n\
  -sw         Tipo de búsqueda W (palabras exactas) en vez de la búsqueda estadística predeterminada.\n\
  -pa         Imprimir resúmenes (predeterminado).\n\
  -pk         Imprimir claves de anotaciones.\n\
  -pb         Imprimir resúmenes y claves de anotaciones.\n\
  -p<N>       Recuperar e imprimir la <N>ésima anotación de la lista de elementos encontrados.\n\
  -k<C>       Restringir las búsquedas a claves que comienzan con el carácter <C>.\n\
  -d<nombre de base de datos>  Nombre de base de datos de 1 a 8 caracteres.  El valor predeterminado\n\
              es la primera base de datos del archivo de configuración del lugar.\n\
  -m<N>       Cantidad máxima de elementos encontrados enviados.  Valor predeterminado: %4$d.\n\
  <consulta>     Conjunto de palabras de búsqueda en el resto de la línea de comando.\n\
              La consulta puede estar entre comillas para incluir operadores boleanos.\n"
4 "Buscando en la base de datos '%1$s'...\n"
9 "<no ningún resumen disponible>"
117 "%1$s Argumento de línea de comando '%2$s' no válido.\a\n"
125 "%1$s No se ha especificado ninguna consulta.\a\n"
165 "%1$s DtSearch no se ha inicializado:\n  %2$s\n"
190 "%1$s No se han encontrado elementos.\n"
195 "%1$s La búsqueda ha fallado.\a\n"
200 "%1$s No hay texto disponible para el documento '%2$s'.\n"
203 "%1$s No se pudieron determinar los tipos de clave disponbiles:\n  %2$s\n"
224 "%1$s No hay claves de anotación que comienzan con '%2$c' en la base de datos '%3$s'.\n"
247 "%1$s ----- Mensajes del motor después de la inicialización -----\n\%2$s\n"
248 "%1$s ----- Fin de los mensajes de inicialización del motor -----\n"

$ ****************************************************
$ set MS_boolpars is set 28.   5/22/96.
$set 28

1 "\n\
%1$s Esta cadena de búsqueda contiene\n\
una consulta boleana no válida.  Por favor vuelva a formularla e intente nuevamente."

2 "%1$s El campo de consulta está vacío."

$ Msg 3 should not translate "@".
3 "\n\
%1$s El operador de colocación (@) sólo\n\
se puede ubicar entre dos palabras."

$ Msg 4 variable 2 is <database name>.
4 "\n\
%1$s Hay palabras de la\n\
consulta que no está almacenadas en la base de datos '%2$s'."

$ Msg 5 "AND" should be the local equivalent to the Boolean Operator
$ for "AND."  "&" should not be translated.
5 "\n\
%1$s Los operadores boleanos deben colocarse\n\
entre palabras o expresiones.  Se considera que dos palabras seguidas\n\
sin operador están separadas por un\n\
operador Y (&)."

6 "%1$s Falta la expresión entre paréntesis."

$ Msg 5 "NOT" should be the local equivalent to the Boolean Operator
$ for "NOT."  "~" should not be translated.
7 "\n\
%1$s El operador NO (~) debe estar ubicado a la\n\
izquierda de la palabra o expresión a la que afecta."

8 "%1$s Consulta boleana no válida.  Error de sintaxis número %2$d."

9 "%1$s La consulta boleana contiene demasiados términos."

$ Msg 10 variable 2 is <database name>.
10 "%1$s Las búsquedas de colocación no está disponibles para la base de datos '%2$s'."

$ Msg 11 should not translate "@."
11 "\n\
%1$s El operador de colocación '%2$.*s' no es válido.\n\
Formato correcto: '@<N>' donde <N> es mayor que cero."

$ Msg 12 should not translate "@."
12 "\n\
%1$s No se puede ubicar el operador de colocación (@) entre\n\
palabras idénticas."

13 "%1$s La palabra '%2$.*s' no es válida."

14 "%1$s La cadena '%2$.*s' no es una sola palabra."

15 "\n\
%1$s Esta consulta no puede enviar ninguna\n\
anotación de forma lógica.  Por favor vuelva a formularla e intente de nuevo."

16 "\n\
%1$s Esta consulta enviará toda la base de datos\n\
'%2$s'.  Por favor vuelva a formularla e intente de nuevo."


$ ****************************************************
$ set 29 symbolically named MS_tomita, used in tomita.c (dtsrdelete)
$set 29

1 "\n\
\n%1$s Se ha recibido una interrupción %2$d.\n\
  El programa se detendrá después del lote de eliminaciones en curso.\n"

3 "%1$s: Código de salida = %2$d.\n"
4 "%1$s No se ha encontrado la base de datos '%2$s'.\n"
5 "Opciones disponibles:"
8 "%1$s Formato de entrada no válido: %2$.30s...\n"
$ Msg 24 is like <Program name> Database <dbname>, <record id> not found."
24 "%1$s Base de datos %2$s, no se encontró '%3$s'.\n"

25 "\n\
NO CONTINUE en ninguna de las siguientes situaciones:\n\
-> Si el archivo de entrada que lista identificaciones de anotaciones a eliminar no\n\
      se llama '%1$s'.\n\
-> Si usuarios todavía obtienen acceso a las bases de datos afectadas.\n\
-> Si existen archivos de la base de datos de los cuales no se han creado copias de respaldo.\n\n\
Si está listo para comenzar a eliminar, introduzca una 's' ahora... "

$ Msg 26 is "<program name> <number of files> read, <number> deleted,
$ <time> elapsed. Database <dbname>: Current record count = <number>,
$ Batch size = <number of records>."
26 "%1$s %2$ld leídos, %3$ld eliminados, %4$ldh %5$2ldm %6$2lds transcurrido.\n\
  Base de datos '%7$s': Total de anotaciones actual = %8$ld, tamaño de lotes = %9$d.\n"
$ Msg 27 is "<program name> <number of records> records read from input file.
$ <number> were deleted and <number> were not found in <time>." 
27 "%1$s %2$ld anotaciones leídas del archivo de entrada.  %3$ld se eliminaron y\n\
  %4$ld no se encontraron en %5$ld horas, %6$ld minutos, %7$ld segundos,\n"

28 "  o un promedio de %1$ld minutos, %2$ld segundos por anotaciones eliminadas.\n"
$ Msg 29 is "<program name>. Run <time stamp>."
29 "%1$s.  Ejecución %2$s.\n"

$ Msg 30 should not translate "-y".
30 "\n\
USO: %1$s [opciones]\n\
  -i    Nombre del archivo de entrada.  Si no está especificado, se utiliza el predeterminado %2$s.\n\
  -d[v] Imprimir instrucciones de depuración.\n\
        -dv Activa la depuración verbosa (anotación por anotación).\n\
  -t<N> Cantidad máxima de segundos de ejecución deseada.\n\
        Ctrl-C/Interrumpir, también detendrá la eliminación en la próxima anotación.\n\
  -n<N> Cambia la cantidad anotaciones de un lote de %5$d a <N>.\n\
  -y    Contesta automáticamente 'sí' al modo de eliminación para confirmar la solicitud del sistema.\n\
  -d    Seguimiento de operaciones de eliminación.\n"

31 "%1$s Argumento desconocido '%2$s'.\n"
32 "%1$s No se pudo abrir el archivo de entrada '%2$s'.\n"
33 "\n%1$s No se pudo abrir el archivo de salida '%2$s'.\n"
34 "Inicialización del motor de búsqueda...\n"
36 "Parada del motor normal.\n"


$ ****************************************************
$ set 30 named MS_huff has messages for hencode.c, hdecode.c, huffcode.c
$set 30

$ Msg 1 variable 2 is <filename>, variable 3 is <system errno>.
1 "%1$s: No se pudo abrir el archivo de codificación huffman '%2$s': '%3$s' Código de salida = 2.\n"
2 "\n\
USO: %1$s <nombre de archivo>\n"
3 "%1$s No se pudo abrir el archivo de entrada '%2$s.\n"
4 "%1$s Total de caracteres = %2$ld.\n"

10 "%1$s Se agotó la memoria.\n"
$ Msg 11 variable 2 is <tree file name>, variable 3 is <system errno>.
11 "%1$s No se pudo abrir el archivo de árbol '%2$s': %3$s.\n"
$ Msg 12 variable 2 is <tree file name>.  Do not translate "hctree."
12 "%1$s Formato de hctree '%2$s' no válido.\n"
$ Msg 13 should not translate "hctree_ids."
13 "%1$s hctree_ids incompatible.\n"
14 "%1$s No se puede abrir cypherfile '%2$s'.\n"
$ Msg 15 should not translate "EOF."
15 "%1$s Fin de archivo inesperado '%2$s'.\n"

30 "183 Las cadenas de bits han crecido demasiado.  Es probable que\n\
estén desactivados los literales con totales de caracteres no equilibrados.\n"
31 "191 Error de programación:  Todavía se intenta construir\n\
Se ha creado el árbol de códigos Huffman.\n"
32 "(carácter no ascii, conjunto de bits alto)"
33 "366 Formato de archivo no válido para %1$s.\n"
34 "424 No se ha abierto el archivo '%1$s' para escribir.  Verifique que no sea para leer solamente.\n"
35 "\n\
USO: huffcode [-l<N> | -l-] [-o] <nombre de huff> [<archivo de entrada>]\n\
  -l<N>      Especifica el total del umbral 'literal'.  Todo carácter que aparece\n\
             <= <N> se codificará con un literal Huffman.  Valor predeterminado: -l0,\n\
             codificación literal solamente para bytes cuyos totales son cero.\n\
  -l-        desactiva codificación literal.  Al desactivar la codificación literal en árboles\n\
             no equilibrados se generan códigos de cadena de bits EXTREMADAMENTE LARGOS. No efectúe esta operación\n\
             a menos que se sepa que los datos de entrada se encuentran en un archivo binario bien equilibrado.\n\
  -o         otorga autorización previa para sobreescribir todo archivo de decodificación existente.\n\
  <nombre huff> es el prefijo de nombre de archivo de archivos de códigos Huffman.\n\
             Si el archivo de codificación (%1$s) ya existe, se le agregarán totales de bytes\n\
             del archivo de entrada, en caso contrario se cerrará un nuevo archivo.\n\
             El archivo de decodificación (%2$s) se crea nuevamente durante cada ejecución.\n\
  <archivo de entrada>   archivo de entrada que contiene los bytes a contar.\n\
             Se puede omitir si el archivo de codificación ya existe.\n"
36 "'%1$s' se un argumento no válido.\n"
37 "576 Falta el prefijo de nombres de archivo de códigos Huffman.\n"
$ Msg 38 needs to have "y" translated to match msg 39"
38 "El archivo de decodificación '%s' ya existe.  ¿Desea sobreescribirlo? [s/n]\n"
$ Msg 39 needs "y" to match the "yes" option in msg 38.
39 "s"
40 "HUFFCODE Versión %1$s\n"
41 "Las tablas de códigos Huffman se crearán nuevamente.\n"
42 "La tabla '%1$s' ya contiene %2$ld Kbytes de ejecuciones anteriores.\n"
43 "645 El archivo de entrada no se ha especificado y la tabla de archivos '%s'\n\
no existe. No hay operaciones a efectuar.\n"
44 "No se pudo abrir el archivo de entrada '%1$s' ni obtener el estado de acceso: %2$s\n"
45 "El archivo de entrada '%1$s' contiene aproximadamente %2$ld Kbytes.\n"
46 "\n%1$ld%% terminado. Se han leído %2$2ld Kbytes.  Se estima tomará %3$3ld segundos completar la operación.\n"
$ Msg 47 variable 2 is a time stamp, variable 2 is from either msg 48 or 49,
$ variables3 and 4 are filenames.
47 "Sello de hora de identificación: '%1$ld'.\n\
%2$s Las tablas de código Huffman se encuentran en '%3$s' y '%4$s'...\n"
$ Msg 48 is used in msg 47 as variable 2.
48 "Creando"
$ Msg 49 is used in msg 47 as variable 2.
49 "Reconstruyendo"

$ ********************** DTSEARCH.MSG ************************
