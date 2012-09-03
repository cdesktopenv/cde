$ "$XConsortium: dtlp.msg /main/5 1996/10/19 14:46:21 rws $";   
$ *************************************<+>*************************************   
$ *****************************************************************************   
$ **   
$ **  File:        dtlp.msg   
$ **   
$ **  Project:     DT printer utility (dtlp)   
$ **   
$ **  Description:   
$ **  -----------   
$ **  This file is the source for the message catalog for dtlp   
$ **   
$ **   
$ *****************************************************************************   
$ **   
$ ** (c) Copyright 1990, 1992, 1993, 1994 Hewlett-Packard Company   
$ ** (c) Copyright 1994 International Business Machines Corp.   
$ ** (c) Copyright 1994 Sun Microsystems, Inc.   
$ ** (c) Copyright 1994 Novell, Inc.   
$ **   
$ **     All Rights reserved   
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
$ * You don't need to localize the following sets:   
$ *   
$ * Please localize the following sets:   
$ *  sets 1   
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------   
$ *************************************<L>*************************************   
$quote "

$set 1
$    
$  -- The first group of messages all appear on the Print dialog --   
$    
$  This is the message that appears in the "Printer:" field when no   
$  printer name has been specified.   

10 "Predeterminado"
$  This is the label that appears in the title bar of the print dialog.   

20 "Imprimir"
$  This is the label for the field indicating the file that will be printed.   

30 "Archivo:"
$  This is the label for the field indicating the name of the printer.   

40 "Impresora:"
$  This is the message that will appear in the "Printer:" field if no printer   
$  has been specified.   

50 "Predeterminada"
$  This is the label for the field indicating the number of copies to print.   

60 "Copias:"
$  This is the label for the field indicating the title for the banner page.   

70 "Título de página del mensaje de cabecera:"
$  This is the label for the field indicating if page numbers are to be added.   

80 "Imprimir números de página"
$  This is thef label for the field indicating additional options for the   
$  "lp" command.   

90 "Imprimir opciones del mandato:"
$  This is the label on the left button. Pushing this button causes the   
$  dialog to disappear and the print operation to start.   

100 "Imprimir"
$  This is the label on the center button. Pushing this button causes the   
$  dialog to disappear and cancels the print operation.   

110 "Cancelar"
$  This is the label on the right button. Pushing this button causes a   
$  help dialog to be posted.   

120 "Ayuda"
$    
$  -- The following are error messages. --   
$    
$  This message is posted when there is a problem with the file specified.    
$  The file name will be appended to the message. One additional message   
$  from the list below will also be included in the dialog to provide   
$  more details.   

130 "Lo siento--No se puede imprimir:"
$  This message is added to the error dialog when the file is not printable    
$  or does not exist.   

132 "No es un archivo imprimible o no existe."
$  This message is added to the error dialog when the file is not readable    
$  by the user invoking dtlp.   

134 "No tiene permiso para leer el archivo."
$  This message is added to the error dialog when there is a general failure    
$  in setting up to print.   

136 "Por favor, compruebe si el archivo existe."
$  This is the label that appears in the title bar of the error dialog.   

160 "Información de impresión"

$ 
$  -- Strings used in the Print dialog and Select Printer dialog in CDEnext --
$ 
$  This is the label for the field indicating the file(s) that will be printed.
170 "Nombres de archivo: "

$  This is used as the window title of the Select Printer dialog and a button
$  in that dialog.
180 "Seleccionar impresora"

$  This is the label for the printer list in the Select Printer dialog.
190 "Impresoras disponibles:"

$  This is the label for the text box in the Select Printer dialog.
200 "Información acerca de la impresora:"

$ 
$  -- The following are error messages added for CDEnext --
$ 
$  File is not printable or does not exist.
210 "Este archivo no se puede imprimir o no existe."

$  File is not printable or does not exist.
220 "No hay ninguna impresora definida para este sistema o no hay ninguna activada."

$  Printer does not exist.
230 "La impresora no existe."

$  Printer is not enabled.
240 "La impresora no está activada."

$  No default printer.
250 "No hay ningún destino de impresora predeterminada."

$  Printer not accepting requests.
260 "La impresora no acepta solicitudes actualmente."

$  Number of copies is not a positive integer.
270 "La cantidad de copias debe ser un número positivo."


$set 99        Redefine set#

$    DO NOT TRANSLATE THESE MESSAGES   
$    DO NOT CHANGE THESE MESSAGES   
$    DO NOT LOCALIZE THESE MESSAGES   
$    These messages are used for the version information.   

1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $XConsortium: dtlp.msg /main/5 1996/10/19 14:46:21 rws $"
