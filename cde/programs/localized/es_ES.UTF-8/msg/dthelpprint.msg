$ $XConsortium: dthelpprint.msg /main/3 1995/11/08 12:11:41 rswiston $
$ *************************************<+>*************************************   
$ *****************************************************************************   
$ **   
$ **  File:        dthelpprint.msg   
$ **   
$ **  Project:     Cde1 Help   
$ **   
$ **  Description:   
$ **  -----------   
$ **  This file is the source for the message catalog for dthelpprint   
$ **   
$ **   
$ *****************************************************************************   
$ **   
$ ** (c) Copyright 1993, 1994 Hewlett-Packard Company   
$ ** (c) Copyright 1993, 1994 International Business Machines Corp.   
$ ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.   
$ ** (c) Copyright 1993, 1994 Unix System Labs, Inc.,a subsidiary of Novell,Inc.   
$ **         All Rights reserved   
$ **   
$ **   
$ **     There are two types of messages in this file:   
$ **   
$ **     1) Messages that appear in printed output   
$ **        These messages are the default and they should all be localized.   
$ **        These messages are marked with LOCALIZE THESE MESSAGES.   
$ **   
$ **     2) Messages that should not be localized.   
$ **        These messages are marked with DO NOT LOCALIZE THESE MESSAGES.   
$ **   
$ *****************************************************************************   
$ **   
$ **   
$ **  ------------------------- MODIFICATION RECORD --------------------------   
$ *   Major Mods, 7/11/94 for dthelpprint   
$ *   
$ **  ----------------------- MODIFICATION RECORD END ------------------------   
$ *****************************************************************************   
$ *************************************<+>*************************************   
$ Use the double quote char around all messages   
$quote "


$set 2        Redefine set#

$ *****  Module: PrintUtil.c *****   
$    
$set 3
$ *****  Module: PrintTopics.c *****   
$    
$ ** LOCALIZE THESE MESSAGES **   

1 "%s Error: helpType es un tema, pero no se ha especificado helpVolume.\n"

2 "%s Error: no es posible ubicar el volumen de ayudas %s\n"

3 "%s Error: problema al procesar el volumen de ayudas %s\n"

4 "%s Error: ha fallado la asignación de memoria\n"

5 "%s Error: no es posible obtener la información del tema:\n\
volumen %s, Id ubicación %s\n"

6 "%s Error: no es posible abrir el archivo temporal %s\n"
$ String used to replace $SECTNUM when printing the index section   

10 "Indice"
$ String used to replace $SECTNUM when printing the table of contents section   

11 "Tabla de contenido"
$ ** DO NOT LOCALIZE THESE MESSAGES **   
$ index entry, page number   

20 "%s, %d\n"
$ index subentry, page number   

21 "     %s, %d\n"
$ location ID associated with the index in the TOC maintained by dthelpprint   

30 "__INDICE-GENERADO"
$ string used by can't get a topic title associated with an index subentry   

31 ""

$set 4        Redefine set#

$ *****  Module: PrintManStrFile.c *****   
$    
$ ** LOCALIZE THESE MESSAGES **   

1 "%s Error: helpType es una cadena, pero no se ha especificado stringData.\n"

2 "%s Error: helpType es una cadena dinámica, pero no se ha especificado stringData.\n"

3 "%s Error: helpType es una página manual, pero no se ha especificado manPage.\n"

4 "%s Error: helpType es un archivo, pero no se ha especificado helpFile.\n"

5 "%s Error: ha fallado la asignación de memoria\n"

$set 5        Redefine set#

$ *****  Module: Initialize.c *****   
$    
$ ** LOCALIZE THESE MESSAGES **   
$ Messages 1 to 29: dthelpprint usage message   
$ **DO NOT LOCALIZE** the command line option names (e.g. -copies).   
$ **DO LOCALIZE** the option argument (e.g. number) and description.   

1 "dthelpprint - Programa de impresión para la Ayuda\n\n"

2 "Uso: dthelpprint [opciones]\n"

3 "Opciones que controlan la impresión:\n"

4 "\t-printer nombreimpresora  impresora a utilizar\n"

5 "\t-copies número        número de copias a imprimir\n"

6 "\t-outputFile nombrearchivo  grabar la salida a este archivo\n"

7 "\t-paperSize tamaño       formatear el contenido a este tamaño de papel\n"
$ DO NOT LOCALIZE THE PAPER SIZE NAMES help_papersize_xxx   

8 "\t\tsize = { help_papersize_letter|help_papersize_legal|\n"

9 "\t\t         help_papersize_executive|help_papersize_a4|help_papersize_b5}\n"

10 "\t-display nombrepantalla  pantalla desde la que se obtienen recursos\n"

11 "\t-name                 nombre del programa usado cuando se obtienen recursos\n"

12 "\t-class                nombre de la clase usada cuando se obtienen recursos\n"

13 "\t-xrm cadenarecursos   recursos adicionales\n"

14 "Opciones que controlan lo que se imprime:\n"

15 "\t-helpType tipo        tipo de datos de la Ayuda\n"

16 "\t\ttype = 0 (volumen de ayudas), 1 (cadena), 2 (página manual), \n\
\t\t       3 (archivo de ayudas)\n"

17 "\t-helpVolume volumen    ruta completa del archivo del volumen de ayudas\n"

18 "\t-locationId ubicación  nombre del tema de Ayuda del volumen\n"

19 "\t-all                  imprimir todos los temas, toc e índice del \n\
\t                      volumen de ayudas\n"

20 "\t-sub                  imprimir el id de ubicación y todos los subtemas\n"

21 "\t-one                  imprimir el id ubicación del tema\n"

22 "\t-toc                  imprimir la tabla de contenido del volumen de ayudas\n"

23 "\t-index                imprimir el índice del volumen de ayudas\n"

24 "\t-frontMatter          imprimir la cubierta del volumen de ayudas\n"

25 "\t-manPage nombrepáginaman  nombre de la página manual\n"

26 "\t-stringData cadena    Texto de ayuda a imprimir\n"

27 "\t-helpFile nombrearchivo    archivo que contiene texto de Ayuda\n"

28 "\t-jobTitle título       título para el trabajo de impresión\n"

29 "\t-topicTitle título     título para el texto de Ayuda\n"
$ Warning and error messages   
$ Do NOT localize the default size name help_papersize_letter   

40 "%s Aviso: El tamaño de papel '%s' no es válido. Utilizado help_papersize_letter.\n"

41 "%s Aviso: Falta valor de tamaño, altura o anchura del papel. Utilizado help_papersize_letter.\n"

42 "%s Aviso: No se puede abrir la pantalla %s\n"

45 "Error: no se puede asignar memoria para el archivo temporal\n"
$ ** DO NOT LOCALIZE THESE MESSAGES **   
$ arg order: directory prefix processid filecnt suffix   

50 "%1$s/%2$s%3$d_%4$d%5$s"

$set 6        Redefine set#

$ *****  Module: Main.c *****   
$    
$ ** LOCALIZE THESE MESSAGES **   

1 "%s Error: helpType %d no permitido.\n"
