$ $XConsortium: dthelpgen.msg /main/3 1995/11/08 12:11:30 rswiston $
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
$  
$quote "

$set 1
$  
$ _DtMessage 1 is the usage message 
$  
$ Localize only the words 'directory', 'name', and 'language'. 
$ 'directory' means the directory name in which the resulting files 
$ will be placed. 'name' mean what base name to give the files placed 
$ in 'directory'. 'language' means which localized versions of help files 
$ to look for. 
$  

1 "%s -dir <directorio> [-generate] [-file <nombre>] [-lang <lenguaje>]\n"
$  
$ Mesages 2-18 are error messages. 
$  

2 "%s: Un elemento de %s no es un directorio\n"

3 "%s: Acceso denegado para el directorio %s\n\
¿Se intenta ejecutar como superusuario?\n"

4 "%s: Un elemento de %s no existe\n"

5 "%s: El sistema de archivos que contiene a %s está lleno\n"

6 "%s: No se puede acceder a %s - número de estado de error %d\n"

7 "%s: El sistema de archivos que contiene %s es de sólo lectura\n"

8 "%s: Se requiere permiso de root para escribir en %s\n"

9 "%s: Escritura en %s no válida\n"

10 "%s: Ruta de Búsqueda vacía\n"

11 "%s: Falta recurso 'título'\n"

12 "%s: Falta recurso 'abstracto'\n"

13 "%s: Falta recurso 'volúmenes'\n"

14 "%s: Falta recurso de juego de 'caracteres'\n"

15 "%s: Falta directorio de destino\n"

16 "%s: No hallado ningún archivo de la Familia\n"

17 "%s: No hallado ningún archivo de Volumen\n"

18 "%s: No se puede acceder al directorio de trabajo actual - número de estado de error %d\n"

19 "%s: No es posible asignar memoria\n"

20 "%s: Especificado lenguaje del sistema no válido %s\n"
$  

$set 2        Redefine set#

$  
$ Specifies the character set used to create this file 
$  
$ This must be localized for the various languages. That is 
$ for Japanese shift JIS, it would be 'ja_JP.SJIS'; for Japanese  
$ EUC it would be 'ja_JP.eucJP'. For files written using 
$ HP Roman8 character set it would be '<lang+terr>.HP-ROMAN8', etc. 
$ The set of allowable locale strings can be found in 
$ /usr/dt/config/svc/CDE.lcx 
$  

1 "es_ES.ISO-8859-1"
$  
$ Specifies the title for the browser. 
$ It is used in the body of text displayed to the user. 
$  

2 "Bienvenido al Gestor de Ayuda"
$  
$ Specifies the body of text displayed in the browser. 
$  
$ When localizing, DO NOT alter any text between the "<" and ">" pairs. 
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>. 
$           DO     localize the text between the tags. 
$  

3 "<ABBREV>Welcome to the Help Manager</ABBREV>                          <PARAGRAPH>Each of the titles listed below represents a\n\
               <ANGLE italic> product family</>\n\
                                      that has installed and registered its online help.  Each title\n\
        (and icon) is a hyperlink that lists the help within the family.</>     <PARAGRAPH after 0 first 1 left 3 label \"<CHAR C.DT-SYMBOL-1><0xB7></>\"> To display a list of the help available for a product family, choose\n\
  its title (underlined text) or icon.</PARAGRAPH>                        <PARAGRAPH after 0 first 1 left 3 label \"<CHAR C.DT-SYMBOL-1><0xB7></>\"> Within a product family, find the help you want to view, then\n\
         choose its title.</PARAGRAPH>                                           <PARAGRAPH first 1 left 3 label \"<CHAR C.DT-SYMBOL-1><0xB7></>\">        If you need help while using help windows, press F1.</PARAGRAPH>"
$  
$ Specifies the Volume Title 
$  

4 "Ayuda - Nivel superior"
$  
$ Specifies the preamble to the help file if no volumes or family files 
$ are found. 
$  
$ When localizing, DO NOT alter any text between the "<" and ">" pairs. 
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>. 
$           DO     localize the text between the tags. 
$  

5 "<ABBREV>Bienvenido al Gestor de Ayuda</ABBREV>                         <LINK 0 \"Help4Help Cómo-Registrar-Ayuda\">                                <TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>                         Nota:   No Se Ha Registrado Ayuda                                            </SIZE></WEIGHT></TYPE></></LINK>                                        <PARAGRAPH leftindent 3 firstindent 3>                                   <WEIGHT bold>Ninguna familia de productos ha registrado su ayuda en línea       archivos para examinar.</>  Se puede disponer de ayuda para algunas aplicaciones   eligiendo mandatos de Ayuda directamente dentro de las aplicaciones.</>"
$  
$ Specifies the title to use in the dthelpgen dtksh dialog. 
$  

6 "dthelpgen"
$  
$ Specifies the message to display in the dthelpgen dtksh dialog. 
$ This message indicates that dthelpgen is building (or rebuilding) 
$ the browser information. 
$  

7 "Generando información de examinador. Por favor, espere."
$  

$set 3        Redefine set#

$  
$ ******* Text Formatting Templates ******** 
$ DO NOT TRANSLATE THESE MESSAGES 
$ DO NOT CHANGE THESE MESSAGES 
$ DO NOT LOCALIZE THESE MESSAGES 
$  

1 "<TOPIC charset %s>"

2 "<TITLE><TYPE serif><WEIGHT bold><SIZE 14>%s</SIZE></WEIGHT></TYPE></TITLE>"

3 "<PARAGRAPH before 1 first 1 left 1>"

4 "<CHARACTERSET %s>"

5 "<TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>%s</></></></>"

6 "<PARAGRAPH before 1 first 1 left 1 graphic %s glink %s gtypelink 0>"

7 "<PARAGRAPH before 1 first 1 left 1 graphic %s glink \"%s %s\" gtypelink 1>"
