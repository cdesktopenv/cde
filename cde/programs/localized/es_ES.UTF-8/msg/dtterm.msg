$ $TOG: dtterm.msg /main/6 1999/09/17 13:28:05 mgreess $
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
$quote "

$set 1
$ ****** Module: DtTermMain.c     
$ Message 1 is the window menu addition - do not translate f.separator or    
$ f.send_msg.  It will be used as the format string for a sprintf() command,    
$ so it is necessary to double up on any percent signs ('%') that you    
$ want to keep in the string.    
$ Message 2 is an error message    

1 "Sep             f.separator\n\
\tConmutar\\ Barra\\ Menú  f.send_msg %d\t"

2 "%s: la opción -S no se puede usar con las opciones -server o -serverid\n"

3 "Terminal"

$set 2        Redefine set#

$ ****** Module: DtTermSyntax.c     
$ These are the usage messages.     
$ DO NOT TRANSLATE Messages 37 or 41 ("End-of-list")    

1 "-/+132\t\t\t\thabilitar/inhabilitar 80<->132 sec escape columna"

2 "-/+aw\t\t\t\thabilitar/inhabilitar autoacomodación"

3 "-bg color\t\t\tcolor de fondo"

4 "-/+bs\t\t\t\tapagar sí/no fondo Term si color seleccionado"

5 "-display nombrepantalla\t\tServidor X a contactar"

6 "-e mandatos args\t\tmandato a ejecutar"

7 "-fb juegofonts\t\t\tfont texto negrita"

8 "-fg color\t\t\tcolor del primer plano"

9 "-fn juegofonts\t\t\tfont texto normal"

10 "-geometry geom\t\t\ttamaño (en caracteres) y posición de la ventana"

11 "-help\t\t\t\timprimir este mensaje"

12 "-/+iconic\t\t\tiniciar/no iniciar ruta iconos"

13 "-/+j\t\t\t\thabilitar/inhabilitar desplazar grupo de líneas"

14 "-/+kshMode\t\t\thabilitar/inhabilitar modo ksh"

15 "-/+l\t\t\t\thabilitar/inhabilitar registro cronológico"

16 "-lf nombrearchivo\t\t\tnombre de archivo registro cronológico"

17 "-/+ls\t\t\t\thabilitar/inhabilitar shell inicio de sesión"

18 "-/+map\t\t\t\thabilitar/inhabilitar correspondencia ventana en salida pty"

19 "-/+mb\t\t\t\thabilitar/inhabilitar avisador de margen"

20 "-ms color\t\t\tcolor del puntero"

21 "-n cadena\t\t\tespecificar nombre del icono"

22 "-name cadena\t\t\tcadenas caso cliente, icono y título"

23 "-nb distancia\t\t\tespecificar distancia para avisador margen derecho"

24 "-/+rw\t\t\t\thabilitar/inhabilitar acomodación línea anterior"

25 "-/+sb\t\t\t\thabilitar/inhabilitar barra de desplazamiento"

26 "-/+sf\t\t\t\thabilitar/inhabilitar teclas de función SUN"

27 "-sl número[s]\t\t\tnúmero de líneas desplazadas [pantallas] a guardar"

28 "-ti nombre\t\t\tcadena usada para identif. programática"

29 "-title cadena\t\t\tcadena de título para ventana"

30 "-tm cadena\t\t\tcaracteres y palabras claves modo de terminal"

31 "-tn nombre\t\t\tnombre de variable de entorno TERM"

32 "-/+vb\t\t\t\thabilitar/inhabilitar avisador visual"

33 "-xrm cadenarecursos\t\tespecificaciones recursos adicionales"

34 "-C\t\t\t\tmodo consola"

35 "-Sxxd\t\t\t\tmodo esclavo en \"ttyxx\" descriptor de archivo \"d\""

36 "-Sxxx.d\t\t\tmodo esclavo en \"ttyxxx\" descriptor archivo \"d\""

37 "Fin-de-Lista"

38 "La opción -e, si se proporciona debe aparecer al final de la línea de mandatos,"

39 "de lo contrario el shell predeterminado del usuario se iniciará. Las opciones que"

40 "empiezan con un signo más (+) restauran el predeterminado."

41 "Fin-de-Lista"

47 "%s:  opción línea de mandatos errónea \"%s\"\r\n\n"

48 "uso:  %s"

49 "\r\n\
   "

50 "[%s]"

51 "\r\n\n\
Escriba \"%s -help\" para una descripción completa.\r\n\n"

52 "uso:\n"

53 "\t%s [-options ...] [-e mandato args]\n\n"

54 "donde opciones incluye:\n"

$set 3        Redefine set#

$ ****** Module: TermFunction.c    
$ This is a warning message dialog box    

1 "MEMORIA LLENA\n\
Pulse OK para borrar"

$set 4        Redefine set#

$ ****** Module: TermPrim.c    
$ This is the title of a dialog box    

$ 1 OBS- "Dtterm - Aviso"

$ This is the title of a dialog box
2 "Terminal - Advertencia"

$set 5        Redefine set#

$ ****** Module: TermFunction.c    
$ This is a warning message dialog box    

1 "MEMORIA LLENA\n\
Pulse OK para borrar"

$set 6        Redefine set#

$ ****** Module: TermView.c    
$ DO NOT TRANSLATE Message 1    
$ Messages 2 and 3 are titles for dialog boxes.    

1 "80x24 132x24"

$ 2 OBS- "Dtterm - Página Man"

$ 3 OBS- "Dtterm - Ayuda"

$ Messages 4 and 5 are titles for dialog boxes.
4 "Terminal - Página de manual"

5 "Terminal - Ayuda"

$set 7        Redefine set#

$ ****** Module: TermViewGlobalDialog.c    
$ These are the user-visible labels in the Global Options dialog box.    
$ #1 is the title.     
$ The rest are resource types and values - look at the dialog box and     
$ documentation for full specs.     
$     
$ DO NOT TRANSLATE MESSAGE 15    
$     

1 "Dtterm - Opciones globales"

2 "OK"

3 "Cancelar"

4 "Ayuda"

5 "global"

6 "Control del cursor"

7 "Recuadro"

8 "Subrayado"

9 "Estilo del cursor"

10 "Habilitado"

11 "Inhabilitado"

12 "Parpadeo de cursor"

13 "Frecuencia de parpadeo (milisegundos)"

14 "Invisible"

15 "blinkRate"

16 "Control de colores"

17 "Inverso"

18 "Normal"

19 "Fondo de ventana"

20 "Comportamiento desplazamiento"

21 "Inhabilitado"

22 "Habilitado"

23 "Desplazamiento uniforme"

24 "Control del avisador"

25 "Audible"

26 "Visible"

27 "Tipo de avisador"

28 "Habilitado"

29 "Inhabilitado"

30 "Aviso de margen"

31 "Distancia al margen"

32 "Aplicar"

33 "Restablecer"

34 "Terminal - Opciones globales"

$set 8        Redefine set#

$ ****** Module: TermViewMenu.c    
$ These are the menu bar entries and menu pane choices.    
$ NOTE TO TRANSLATORS: The Single letter entries are the menu mnemonic    
$ for the entry above them (i.e. message 11 "F" is the mnemonic for    
$ message 10 "Font Size").     
$ The items with a "+" in them are the accellerators for an item.    
$ The single-character items are the mnemonics for a menu item, where the    
$ text of the menu item is specified in the previous message    
$ Do not translate messages 24 and 28.    

1 "Predeterminado"

2 "Tamaño de ventana"

3 "T"

4 "-iso8859-1"

5 "%d punto"

6 "%.1f punto"

7 "%.1f punto"

8 "%.2f punto"

9 "Predeterminado"

10 "Tamaño de font"

11 "T"
$   Window menu    

12 "Nuevo"

13 "N"

14 "Imprimir"

15 "I"

16 "Imprimir..."

17 "r"

18 "Cerrar"

19 "C"

20 "Ventana"

21 "V"
$   Edit menu    

22 "Copiar"

23 "C"

24 "Ctrl osfInsert"

25 "Control+Insert"

26 "Pegar"

27 "P"

28 "Despl osfInsert"

29 "Despl+Insert"

30 "Editar"

31 "E"
$   Option menu    

32 "Barra de menús"

33 "B"

34 "Barra de desplazamiento"

35 "d"

36 "Global..."

37 "G"

38 "Terminal..."

39 "T"

40 "Reinicio por software"

41 "R"

42 "Reinicio por hardware"

43 "h"

44 "Restablecer"

45 "R"

46 "Opciones"

47 "O"
$ Old Help menu (no longer used for CDE)    

48 "Información general"

49 "I"

50 "Tareas"

51 "T"

52 "Referencia"

53 "R"

54 "Sobre el tema"

55 "S"

56 "Uso de la ayuda"

57 "U"

58 "Acerca de dtterm"

59 "A"

60 "Ayuda"

61 "A"

62 "Alt+F4"

63 "dtterm"
$ Current CDE Help menu    

64 "Información general"

65 "g"

66 "Indice"

67 "I"

68 "Tabla de contenido"

69 "c"

70 "Tareas"

71 "T"

72 "Referencia"

73 "R"

74 "Teclado"

75 "e"

76 "Uso de la ayuda"

77 "U"

78 "Acerca de dtterm"

79 "A"

80 "Ayuda"

81 "A"

82 "Acerca de Terminal"

83 "Acerca de Terminal"

$set 9        Redefine set#

$ ****** Module: TermTerminalDialog.c    
$ These are the messages for the terminal options dialog    

1 "dtterm - Opciones de terminal"

2 "OK"

3 "Cancelar"

4 "Ayuda"

5 "Control del teclado"

6 "Normal"

7 "Aplicación"

8 "Modo de teclas de cursor"

9 "Numérico"

10 "Aplicación"

11 "Modo de teclado numérico"

12 "Sólo retorno"

13 "Retorno/Avance de línea"

14 "Secuencia nueva línea"

15 "Bloqueado"

16 "Desbloqueado"

17 "Teclas de función de usuario"

18 "Control de pantalla"

19 "Habilitado"

20 "Inhabilitado"

21 "Conmutación a 132 columnas"

22 "Acomodación por fin de línea"

23 "Acomodación por fin de línea en la línea anterior"

24 "Aplicar"

25 "Restablecer"

26 "Terminal - Opciones de Terminal"

$set 10
$ ****** Module: sunDtTermServer.c
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Terminal - Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
$ "-NEW"
3 OK
