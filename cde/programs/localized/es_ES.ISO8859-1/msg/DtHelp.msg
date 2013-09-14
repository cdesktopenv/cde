$ *************************************<+>*************************************  
$ *****************************************************************************  
$ **  
$ **  File:        dthelp.msg  
$ **  
$ **  Project:     CDE 1.0 Help  
$ **  
$ **  Description:  
$ **  -----------  
$ **  This file is the source for the message catalog for DtHelp  
$ **  
$ **  
$ *****************************************************************************  
$ **  
$ **     (c) Copyright 1990 Hewlett-Packard Company  
$ **         All Rights reserved  
$ **  
$ **  
$ **     There are two types of messages in this file:  
$ **  
$ **     1) Messages that appear in dialogs or are displayed to the user.  
$ **        These messages are the default and they should ALL BE LOCALIZED.  
$ **        Note that these messages do NOT have any identification.  
$ **  
$ **     2) Messages that should not be localized.  
$ **        These messages are marked with DO NOT TRANSLATE THESE MESSAGES.  
$ **  
$ *****************************************************************************  
$ **  
$ **  
$ **  ------------------------- MODIFICATION RECORD --------------------------  
$ *   Major Mods, 3/26/92 in preperation for the NLS package release on the  
$ *               27th.  (Mike Wilson).  
$ *  
$ *   Major Mods, 6/10/94 in prep for Cde Message Catalog Freeze  
$ *  
$ **  ----------------------- MODIFICATION RECORD END ------------------------  
$ *****************************************************************************  
$ *************************************<+>*************************************  
$quote "

$set 2
$ *****  Module: HelpDialog.c  and a few from HelpQuickD.c *****  
$   
$ These Messages appear in the pulldown menus for Cde Help General Help  
$ dialog widget.  
$   
$ They come in pairs where the first is the menu name (e.g. 1 Print...) and the  
$ second is the mnemonic (e.g. 2 P).  
$   
$ Messages 1 through 6 are the pulldown menus for the File menu.  

1 "Imprimir..."

2 "I"

3 "Cerrar"

4 "C"

5 "Ventana Nueva..."

6 "V"
$ Messages 7 through 8 are the pulldown menus for the Edit menu.  

7 "Copiar"

8 "C "
$ Messages 9 through 10 are the pulldown menus for the Search menu.  

9 "Indice..."

10 "I"
$ Messages 11 through 16 are the pulldown menus for the Navigate menu.  

11 "Tema de Inicio"

12 "T"

13 "Retroceder"

14 "R"

15 "Histórico..."

16 "H"
$ Messages 17 through 28 are the pulldown menus for the Help menu.  

17 "Información General - Uso de la Ayuda"

18 "f"

19 "Tabla de Contenido"

20 "C"

21 "Tareas"

22 "T"

23 "Referencia"

24 "R"

25 "Ratón y Teclado"

26 "a"

27 "Acerca de la Ayuda"

28 "y"
$   
$ Messages 29 through 36 are the menu labels and mnemonics for for the  
$ Help Dialog.  

29 "Archivo"

30 "r"

31 "Buscar"

32 "B"

33 "Desplazarse"

34 "D"

35 "Ayuda "

36 "A"

37 "Editar"

38 "E"
$   
$   
$ >>>>> Skip #39 through #49  
$   
$ Messages 50 through 55 are used to display formatting errors within the  
$ help dialog and quick help dialog widgets.  
$   

50 "No se pudieron formatear los datos de la serie."

51 "No se pudieron formatear los datos de la serie dinámica."

52 "No se pudo formatear Página Man. La Página Man requerida no está presente o está dañada."

53 "No se pudieron formatear los datos del archivo de texto. El archivo de texto requerido no existe o está dañado."

54 "No se pudo formatear Tema de inicio."

55 "ID de ubicación no existente:"
$   
$ Messages 56 and 67 are displayed when a hypertext link type is selected but  
$ not supported within the current application.  
$   

56 "El enlace de Hipertexto no tiene soporte en esta aplicación."

57 "Esta aplicación no proporciona soporte a los enlaces a Páginas Man."
$   
$ Messages 58 through 62  are used when bad or null help volumes or  
$ locationId's are used.  
$   

58 "La ayuda en línea solicitada no está instalada o no está en la vía adecuada de búsqueda de ayuda. Para información sobre la instalación de la ayuda en línea consulte la documentación del producto.\n"

59 "No especificado ID de ubicación."

60 "No especificado volumen de ayuda."

61 "Volumen de Ayuda:"

62 "ID Ubicación:"
$   
$ >>>>> Skip #63 through #69  
$   
$   
$ Message 70 is used when a null value for DtNtopicTitle is used.  
$   

70 "Título Tema No Existente."
$   
$ New messages added after early msg cat freeze; not in order  

80 "El tipo de ayuda especificado no es válido."

$set 3        Redefine set#

$ ****  Module: History.c ****  
$ These messages are used as labels for the History Dialog.  

1 "Ayuda - Examinador del histórico"

2 "Volúmenes de ayuda observados:"

3 "Cerrar"

4 "Ayuda"

5 "Archivos de ayuda"

6 "Páginas Man"

7 "Mensajes de ayuda"

8 "Temas de ayuda observados:"

9 "Archivos de ayuda observados:"

10 "Páginas Man observadas:"

11 "Mensajes de ayuda observados:"

$set 4        Redefine set#

$ *****  Module: Print.c  *****  
$ These messages are used as labels for the Print Dialog.  
$ 1: dialog title  

1 "Ayuda - Imprimir"
$ 10-12: standard printer dialog labels  

10 "Impresora:"

11 "Copias:"

12 "Título Página Separadora:"
$ 20-25: paper size label and sizes  

20 "Tamaño Papel:"

21 "Carta 8,5 x 11 pulg"

22 "Legal 8,5 x 14 pulg"

23 "Ejecutivo 7,25 x 10,5 pulg"

24 "A4 210 x 297 mm"

25 "B5 182 x 257 mm"
$ 30-33: help volume-specific printing options  

30 "Temas a Imprimir"

31 "Tema Actual"

32 "Actual y Subtemas"

33 "Tabla de Contenido e Indice"

34 "Todos los Temas en el Volumen"
$ 40-42: buttons at bottom of dialog  

40 "Imprimir"

41 "Cancelar"

42 "Ayuda"
$ These messages are used to describe the kind of content to be printed  
$ 50 "Help Volume: "  

50 "Volumen Ayuda:"

51 "Mensaje Ayuda"

52 "Archivo Ayuda"
$ 53 "Manual Page: "  

53 "Página Manual: "

54 "Mensaje de error"
$ Don't translate message 60 unless it is not an empty string  
$ This is the default value that goes in the Printer field  
$ 60 " "  

60 ""
$ Don't translate message 61  
$ This is the default value that goes in the Copies field  

61 "1"

$set 5        Redefine set#

$ *****  Module: GlobSearch.c  *****  
$ Message 1 is the label used to label the dialog  

1 "Ayuda - Búsqueda de índice"

2 "Buscar"

3 "Actual "
$ DO NOT TRANSLATE MESSAGE 4  
$ " (%s)"  

4 "(%s)"

5 "Todos los volúmenes"

6 "Seleccionado"

7 "Seleccionar volúmenes..."

8 "Mostrar"

9 "Completar índice"

10 "Entradas con:"

11 "Ir a"

12 "Iniciar búsqueda"

13 "Continuar búsqueda"

14 "Detener búsqueda"

15 "Cerrar"

16 "Ayuda"

17 "Examinando volúmenes... Encontrado(s) %d"

18 "Buscando... Volúmenes restantes: %d"

19 "No se han hallado entradas de índice"

20 "Completar índice"

21 "Entradas con \"%s\""

22 "No se han seleccionado volúmenes"

23 "Seleccionar opciones de búsqueda de arriba"
$ Do NOT translate messages 25-31  
$ " "  

25 " "

26 "+"

27 "-"
$ "  +"  

28 "  +"
$ "  -"  

29 "  -"
$ 30 "       "  

30 "       "
$ 31 "         "  

31 "         "
$ DO translate messages 32 and above  
$ kind of topic messages  

32 "Mensaje de ayuda"

33 "Archivo de Ayuda"

34 "Página manual"

35 "Formato desconocido"
$ specialized title of Search Volume Selection dialog  

40 "Ayuda - Buscar Selección de Volumen"
$ additional messages (late addition caused this ordering)  

50 "No encontradas entradas de índices.  Intentar en Todos los Volúmenes."

51 "No encontradas entradas de índices.  Intentar otra palabra."
$ additional messages (later addition caused this ordering)  
$ These messages are the braces that go around the current volume name  
$ in the index search dialog.  They are kept in the message catalog  
$ in case they need to be localized.  

60 "("

61 ")"

$set 6        Redefine set#

$ *****  Module: HelpAccess.c  *****  
$ These messages are used for the Help on Help dialog in Cde Help widgets.  

1 "Ayuda sobre la ayuda"

2 "Imprimir ..."

3 "Cerrar"

4 "¡No se proporciona Ayuda sobre la ayuda en esta aplicación!"

5 "Retroceder"

6 "Ayuda ..."

$set 7        Redefine set#

$ *****  Module: Main.c & Util.c & (dthelpview CDE 1.0  *****  
$ These messages are used by the dthelpview program.  They need to be  
$ localized for dthelpview, however, they are not directly used by libDtHelp  
$ Messages 1 through 7 are command line usage response messages.  
$ Do NOT translate the option name (e.g. -helpVolume).  DO translate  
$ the argument descriptor name (e.g. <Help Volume File>)  

1 "Uso:   %s...\n"

2 "\t-helpVolume <Archivo Volumen Ayuda>\n"

3 "\t-locationId  <ID>\n"

5 "\t-file  <Archivo Texto ASCII>\n"

6 "\t-man\n"

7 "\t-manPage <Unix Man Page>\n\n\n"
$ man page dialog  

11 "Pág Man"

12 "Pág Man:"

13 "Mostrar Pág Man"
$ button labels  

16 "Salir"

17 "Cerrar"

18 "Retroceder"

19 "Imprimir ..."
$ title of helpview window  

20 "Visualizador de Ayuda"

$set 8        Redefine set#

$ *****  Module: HelpUtil.c  *****  
$ Message 1 is the label used in the definition link dialog box  
$ (e.g. Quick help close button)  

2 "Cerrar"

3 "Error interno: No se pudo tomar control del ratón\n\
DtHelpReturnSelectedWidget cancelado anormalmente.\n"

4 "Error interno: No se pudo tomar control del teclado\n\
DtHelpReturnSelectedWidget() cancelado anormalmente.\n"

5 "Error de ayuda"
$ Messages 10 and up were added after message catalogs were taken  
$ for translation.  They are not in any order or chosen location.  

10 "Ejecutar Siempre"

11 "No Ejecutar"

12 "Se necesita el volumen de ayuda para ejecutar un alias de mandato.\n\
El alias '%s' no está definido."

13 "Se necesita el volumen de ayuda para ejecutar un alias de mandato.\n\
Por razones de seguridad, se desactiva la ejecución automática del mandato.\n\
El mandato es:  %s"

14 "El volumen de ayuda intentó ejecutar un comando como usuario raíz pero\n\
el directorio de inicio de  \"%s\"  ($HOME) no corresponde al directorio\n\
raíz del usuario.  Esto puede ejecutar comandos\n\
inesperados.\n\
\n\
Comando:  \"%s\"\n\
\n\
Nota:  Para evitar esto en el futuro:\n\
 ejecute \"su - root\"  en vez de \"su root\".\n"
$ End Message set 8  

$set 9        Redefine set#

$ *****  Module: PathArea.c  *****  
$ These messages are used in the general Help Dialog Widget.  
$ Message 1 is the label used to label Topic Tree Area  

1 "Volumen: "
$ Messages 2 though 5 are used in the Button Box area of our general  
$ help dialog.  

2 "Retroceder"

3 "Histórico..."

4 "Índice..."

5 "Nivel Superior"

$set 10        Redefine set#

$ *****  Module: VolSelect.c  *****  
$ Message 1 is the default label used to label the volume selection dialog.  

1 "Ayuda - Selección Volumen"

2 "Volúmenes de ayuda"

3 "Seleccionar uno o más volúmenes de ayuda"

4 "Cerrar"

5 "Ayuda"

$set 11        Redefine set#

$ *****  Module: HelpQuickD.c  *****  
$ These messages are used as labels for the quick help dialog.  

1 "Cerrar"

2 "Más ..."

3 "Retroceder"

4 "Imprimir ..."

5 "Ayuda ..."

$set 99        Redefine set#

$    DO NOT TRANSLATE THESE MESSAGES  
$    DO NOT CHANGE THESE MESSAGES  
$    DO NOT LOCALIZE THESE MESSAGES  
$    These messages are used for the version information.  

1 "@(#)version_goes_here"

2 "\n\
@(#)Message catalog source $XConsortium: DtHelp.msg /main/5 1996/10/19 14:45:16 rws $"
