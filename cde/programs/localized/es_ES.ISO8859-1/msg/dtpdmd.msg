$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtpdmd.msg
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for the Dt Print
$ **  Dialog Manager Daemon.
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1996 Digital Equipment Corporation.
$ ** (c) Copyright 1996 Hewlett-Packard Company.
$ ** (c) Copyright 1996 International Business Machines Corp.
$ ** (c) Copyright 1996 Sun Microsystems, Inc.
$ ** (c) Copyright 1996 Novell, Inc. 
$ ** (c) Copyright 1996 FUJITSU LIMITED.
$ ** (c) Copyright 1996 Hitachi.
$ **
$ *****************************************************************************
$ **
$ **
$ **  $XConsortium: dtpdmd.msg /main/2 1996/11/19 20:16:56 rws $
$ *****************************************************************************
$ *****************************************************************************

$set 1

$    /* NL_COMMENT
$ 
$	General Translator Comments
$ 
$       - The acronyms "pdm" and "PDM" are used in several
$         messages, and were derived from the english phrase
$         "Print Dialog Manager". The acronyms should not be
$         localized.
$ 
$       - The acronym "pdmd" is used in several messages,
$         and was derived from the english phrase "Print
$         Dialog Manager Daemon". The acronym should not be
$         localized.
$ 
$     */
1 uso: %1$s [opciones]\n\
\n\
donde [opciones] incluye:\n\
   -d <display>        anfitrión:pantalla[.pantalla] de X Print Server\n\
   -a <selección alternativa>  nombre de selección X alternativo (\"%2$s\" predeterminado)\n\
   -p <pdm predeterminado>    pdm predeterminado a activar (\"%3$s\" predeterminado)\n\
   -P <pdm>        pdm a activar. Reemplaza todos los demás\n\
   -s                  activa el protocolo de seguridad\n\
   -f <archivo de autorización>      conjunto inicial de cookies de autorización\n\
   -l <archivo de registro>       archivo para el registro de errores, etc

2 %1$s: error, no se puede abrir la pantalla %2$s\n

3 %1$s: error, ya se está ejecutando otro pdm (%2$s)\n

4 %s: error, no se puede crear el tubo de señal Xt\n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "SelectionRequest" in
$         messages 5, 6 and 7.
$ 
$       - Do not translate the word "XmbTextPropertyToTestList"
$         in message 7.
$ 
$     */
5 %s: error, no se pudo obtener la propiedad SelectionRequest\n

6 %s: error, el formato de la propiedad SelectionRequest no es válido\n

7 %s: error, XmbTextPropertyToTestList ha fallado en la propiedad SelectionRequest\n

8 %s: error, no se pudo crear el tubo de mensaje\n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "fork" in message 9.
$ 
$     */
9 %s: error, no se pudo fork\n

10 %1$s: error, no se pudo ejecutar \"%2$s\" \n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "exec" in message 11.
$ 
$       - Do not translate the words "-display" or
$         "-vdisplay" in message 11.
$ 
$     */
11 %1$s: mensaje de exec registrado a las %2$s\n\
   mensaje de error de exec = \"%3$s\"\n\
   código de error de exec    = %4$s\n\
   pdm                = \"%5$s\"\n\
   -display           = \"%6$s\"\n\
   -vdisplay          = \"%7$s\" \n

12 %1$s: error, no se pudo enviar el mensaje final %2$s\n\
al cliente que lo ha solicitado en nombre del\n\
debido a un error generado X\n

13 %1$s: error, no se pudo enviar el mensaje final %2$s\n\
al cliente que lo ha solicitado en nombre del\n\
PDM debido a un error XIO\n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "stderr" in message 14.
$ 
$       - Do not translate the words "-display" or
$         "-vdisplay" in message 14.
$ 
$     */
14 %1$s: mensaje de pdm registrado a las %2$s\n\
   pdm       = \"%3$s\"\n\
   -display  = \"%4$s\"\n\
   -vdisplay = \"%5$s\"\n\
   exit code = \"%6$d\"\n\
   stderr    = \"%7$s\" \n

15 error de pdmd anexado= \n\"%s\" \n

16 %1$s: error, no se puede abrir el archivo de registro \"%2$s\". Se desactivará el registro de mensajes stderr\n

