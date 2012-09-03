$ "$TOG: dtfile.msg /main/10 1999/09/16 09:43:40 mgreess $"   
$ *************************************<+>*************************************   
$ *****************************************************************************   
$ **   
$ **  File:        dtfile.msg   
$ **   
$ **  Project:     Common Desktop Environment dtfile   
$ **   
$ **  Description:   
$ **  -----------   
$ **  This file is the source for the message catalog for dtfile   
$ **  Any additional messages are to be added to this file by hand,   
$ **  and the associated index number placed in the code.   
$ **   
$ *****************************************************************************   
$ **   
$ **     (c) Copyright 1990, 1991, 1992 Hewlett-Packard Company   
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
$ 03/25/93 H. Amro            removed the guilty $ from message 30   
$    
$    
$    
$ *****************************************************************************   
$quote "

$set 2
$ ****** Module: ChangeDir.c ****(developer comment only)********   
$ Messages 3 and 10 are titles for the Change Folder dialog, Message 3 is   
$   used when the user has specified a title for dtfile windows   
$ Messages 2 and 4 are labels for the Change Folder dialog   
$ Message 1 is the title for Change Folder errors   
$ Messages 7, 8, 9, 11, and 12 are Change Folder error messages   

1 "Error de cambiar carpeta"

2 "Carpeta nueva:"

3 "Cambiar carpeta"

4 "Host:"

7 "El siguiente nombre de carpeta no es válido.\n\n\
%s:%s"

8 "No puede conmutar a esta carpeta. Sólo tiene\n\
permiso para examinar carpetas que estén por debajo\n\
de la carpeta actual. No puede especificar una \n\
ruta absoluta para la nueva carpeta."

9 "Esta vista está en modalidad restringida. No puede\n\
ir a la carpeta especificada porque ésta no se encuentra \n\
en la ruta relativa de la carpeta restringida.\n"

10 "Gestor de archivos - Cambiar carpeta"

11 "No se puede obtener el nombre de la nueva carpeta."

12 "Entre el nombre de la carpeta."

13 "Ir a Error"

14 "Carpeta de destino:"

15 "Ir a"

16 "Nombre del sistema:"

17 "Gestor de archivos - Ir a"

18 "Falta la Carpeta de destino.\n\
   Escriba un nombre de carpeta o seleccione una carpeta de la lista."

19 "El siguiente nombre de carpeta no es válido. \n\n\
%s"

20 "No se puede conmutar a la carpeta\n\n\
%s\n\n\
No está permitido visualizar esta carpeta\n\
porque es una carpeta restringida."

$set 3        Redefine set#

$ ****** Module: ChangeDirP.c ****(developer comment only)********   
$ Messages 2-6 are labels for the folder Status Line   
$ Message 3 is required for SUN systems   
$ Message 2-9 are obsolete   

2 "%d Archivos %d Ocultos"

3 "%1$d Archivos %2$d Ocultos"

4 "%3d Archivos ..."

5 "Leyendo ..."

6 "%d Ocultos"

7 "%d Elementos %d Ocultos"

8 "%1$d Elementos %2$d Ocultos"

9 "%3d Elementos ..."

10 "%d Elemento(s)"

11 "%d Elemento(s)..."

12 "%d Elemento(s) %d Oculto"

$set 4        Redefine set#


$set 5        Redefine set#


$set 6        Redefine set#


$set 7        Redefine set#


$set 8        Redefine set#


$set 9        Redefine set#

$ ****** Module: File.c ****(developer comment only)********   
$ Messages 6 and 7 are the title and text for an action error message   
$ Messages 8 and 9 are rename error messages   
$ Messages 11-19 are the title and text for warning messages displayed   
$   when an object/set of objects is moved/copied/linked to a folder   
$   containing object(s) with the same name   

6 "Error de acción"

7 "No hay ninguna acción definida para %s\n"

8 "Este archivo está representado en el Espacio de trabajo  \n\
y ya existe en él un archivo con este nuevo nombre.\n\
Elija un nombre distinto."

9 "Ya existe un archivo con este nombre.\n\
Elija un nombre distinto."

11 "Gestor de archivos - Aviso de Mover/Copiar/Enlazar"
$ Messages 12-15 deal with multiple files/folders   

12 "Los archivos (o carpetas) siguientes\n\
ya existen en\n\
%s:"

13 "\n\
La operación Copiar escribirá encima de estos archivos.\n\
          ¿Desea proseguir?\n"

14 "\n\
La operación Mover escribirá encima de estos archivos.\n\
          ¿Desea proseguir?\n"

15 "\n\
La operación Enlazar escribirá encima de estos archivos.\n\
          ¿Desea proseguir?\n"
$ Messages 16&17 deal with a single file/folder   

16 "Un objeto con nombre\n\
 %s\n\
ya existe en la carpeta\n\
%s"

17 "Una carpeta con nombre\n\
%s\n\
ya existe en la carpeta\n\
%s"
$ Labels in rename/replace and merge/replace dialogs   

20 "¿Qué desea hacer?"

21 "Reemplazar objeto existente"

22 "Renombrar objeto existente como:"

24 "Fusionar contenido de las dos carpetas"

25 "Reemplazar la carpeta existente"

27 "Ok"

28 "Cancelar"

29 "Ayuda"

30 "Cancelar Mover"

31 "Cancelar Copiar"

32 "Cancelar Enlazar"
$ Window title for overwrite dialogs   

33 "Gestor de archivos - Aviso Mover/Copiar/Enlazar"

34 "Gestor de archivos - Aviso Mover"

35 "Gestor de archivos - Aviso Copiar"

36 "Gestor de archivos - Aviso Enlazar"

37 "Renombrar objeto - Error"

38 "%s\n\
ya existe: elegir otro nombre."

39 "%s\n\
no se puede renombrar como\n\
%s\n\
%s"

40 "Reemplazar objeto - Error"

51 "copiando"

52 "moviendo"

53 "enlazando"

54 "cambiando"

56 "Reemplazar objetos existentes"

57 "Renombrar objetos existentes añadiendo un número"

58 "copiado"

59 "movido"

60 "enlazado"

61 "cambiado"

68 "1 objeto %s."

69 "%d objetos %s."

70 "%s\n\
%d objetos tienen el mismo nombre que los de la carpeta\n\
%s"

71 "%s\n\
1 objeto tiene el mismo nombre que uno de la carpeta\n\
%s"

72 "Copiar"

73 "Mover"

74 "Enlazar"

75 "Operación"

76 "copiar"

77 "mover"

78 "enlazar"

79 "operación"

80 "%s Cancelado - la carpeta no era %s.\n\n\
La carpeta que desea reemplazar no se pudo suprimir\n\
porque un objeto interno de dicha carpeta no se pudo suprimir.\n\
Otros objetos de la carpeta pueden haber sido ya suprimidos\n\
antes de que el proceso %s se parara."

81 "%s\n\n\
La causa más probable de este problema puede ser que no tenga\n\
permisos de Escritura para el objeto que no se pudo suprimir.\n\
Cambie los permisos de Escritura o renombre manualmente la carpeta\n\
%s de forma que ya no tenga el mismo nonmbre."

82 "%s Cancelado\n\n\
%s\n\
no se puede suprimir.\n\n\
La causa más probable de este problema es que\n\
no tenga permiso de Escritura para el archivo.\n\
Cambie el permiso de Escritura o renombre manualmente el\n\
archivo %s para que ya no tenga el mismo nombre."

83 "%s Cancelado - Ninguno de los objetos era %s.\n\n\
 El siguiente objeto no se pudo renombrar automáticamente\n\
%s\n\n\
%d objetos se renombraron antes de que se parara el proceso %s."

84 "%s\n\n\
La causa más probable de este problema es que no tenga permiso \n\
de Escritura para el objeto mencionado arriba. Cambie el permiso\n\
de Escritura o renombre manualmente el objeto %s para que ya\n\
no tenga el mismo nombre."

85 "%s Cancelado - Ninguno de los objetos era %s.\n\n\
 No se pudo renombrar automáticamente el siguiente objeto\n\
%s\n\n\
1 objeto se renombró antes de que se parara el proceso %s."

86 "%s Cancelado - ninguno de los objetos era %s.\n\n\
No se pudo suprimir una carpeta que deseaba reemplazar.\n\
   Carpeta: %s\n\n\
Se puede haber suprimido algún objeto de esta carpeta\n\
antes de que se parara el proceso %s.\n\n\
%d objetos se han suprimido.\n\n\
La causa más probable de este problema es que no tenga permiso de\n\
Escritura (w) para la carpeta arriba mencionada. Cambie el permiso\n\
de Escritura o renombre manualmente la carpeta %s para que ya\n\
no tenga el mismo nombre."

87 "%s Cancelado - ninguno de los objetos era %s.\n\n\
No se ha podido suprimir la carpeta que deseaba reemplazar.\n\
   Carpeta: %s\n\n\
Se puede haber suprimido algún objeto de la carpeta\n\
antes de que se parara el proceso %s.\n\n\
1 objeto se ha suprimido.\n\n\
La causa más probable de este problema es que no tenga permiso de\n\
Escritura (w). Cambie el permiso de Escritura o renombre manualmente\n\
la carpeta %s para que ya no tenga el mismo nombre."

88 "%s Cancelado - ninguno de los objetos era %s.\n\n\
No se pudo suprimir un archivo que deseaba reemplazar.\n\
    Archivo: %s\n\n\
%d objetos se han suprimido.\n\n\
La causa más probable de este problema es que no tenga permiso\n\
de Escritura para el archivo mencionado arriba. Cambie el permiso\n\
de escritura o renombre manualmente el archivo %s para que ya no \n\
tenga el mismo nombre."

89 "%s Cancelado - ninguno de los objetos era %s.\n\n\
No se pudo suprimir el archivo que deseaba reemplazar.\n\
    Archivo: %s\n\n\
1 objeto (otro) se ha suprimido.\n\n\
La causa más probable de este problema es que no tenga permiso \n\
de Escritura para el archivo mencionado arriba. Cambie el permiso de\n\
Escritura o renombre nanualmente el archivo %s para que ya no tenga\n\
el mismo nombre."

90 "Conflicto de nombre.\n\
  Este objeto está fuera del fondo del Espacio de Trabajo.\n\
  Otro objeto en el fondo ya tiene el nombre que está \n\
  intentando entrar.\n\
  Por favor, elija un nombre diferente."

91 "Detener Copiar"

92 "Detener Enlazar"

93 "Detener Mover"

94 "Error"

95 "Se ha cancelado Mover - la carpeta no se ha movido. \n\n\
La carpeta que desea sustituir no se ha podido suprimir \n\
al no haberse podido eliminar un objeto de dicha carpeta. \n\
Puede que ya se hayan suprimido otros objetos de la carpeta \n\
antes de la detención del proceso de mover. \n\n\
La causa más probable de este problema es que no tenga permisos\n\
de Escritura para el objeto que no se ha podido eliminar. \n\
Cambie el permiso de escritura o renombre manualmente la carpeta\n\
que está moviendo, para que ya no tenga el mismo nombre. "

96 "Se ha cancelado Copiar - la carpeta no se ha copiado. \n\n\
La carpeta que desea sustituir no se ha podido suprimir\n\
al no haberse podido eliminar un objeto de dicha carpeta.\n\
Puede que ya se hayan suprimido otros objetos de la carpeta\n\
antes de la detención del proceso de copia.\n\n\
La causa más probable de este problema es que no tenga\n\
permiso de Escritura para el objeto que no se ha podido suprimir.\n\
Cambie el permiso de escritura o renombre manualmente la carpeta\n\
que esté copiando, para que ya no tenga el mismo nombre. "

97 "Se ha cancelado Enlazar - la carpeta no se ha enlazado.\n\n\
La carpeta que desea sustituir no se ha podido suprimir \n\
al no haberse podido eliminar un objeto de dicha carpeta.\n\
Puede que haya otros objetos de la carpeta que ya se hayan\n\
suprimido antes de la detención del proceso de enlazar. \n\n\
La causa más probable de este problema es que no tenga \n\
permiso de Escritura para el objeto que no se ha podido suprimir.\n\
Cambie el permiso de escritura o renombre manualmente la carpeta \n\
que está enlazando, para que ya no tenga el mismo nombre. "

98 "Operación cancelada - la carpeta no se ha cambiado.\n\n\
La carpeta que desea sustituir no se ha podido suprimir\n\
al no haberse podido eliminar un objeto de dicha carpeta.\n\
Puede que ya se hayan suprimido algunos objetos de la carpeta\n\
antes de la detención del proceso. \n\n\
La causa más probable de este problema es que no tenga permiso\n\
de Escritura para el objeto que no se ha podido eliminar. \n\
Cambie el permiso de escritura o renombre manualmente la carpeta\n\
que esté cambiando, para que ya no tenga el mismo nombre. "

99 "Se ha cancelado Mover \n\n\
%s\n\
no se puede eliminar.\n\n\
La causa más probable de este problema es que no\n\
tenga permiso de Escritura para el archivo. \n\
Cambie el permiso de Escritura o renombre manualmente\n\
el archivo que esté moviendo, para que ya no tenga el \n\
mismo nombre. "

100 "Se ha cancelado Copiar\n\n\
%s\n\
no se puede eliminar. \n\n\
La causa más probable de este problema es que no\n\
tenga permiso de Escritura para el archivo.\n\
Cambie el permiso de escritura o renombre manualmente\n\
el archivo que esté copiando, para que ya no tenga el\n\
mismo nombre. "

101 "Se ha cancelado Enlazar \n\n\
%s\n\
no se puede eliminar.\n\n\
La causa más probable de este problema es que no \n\
tenga permiso de Escritura para el archivo. \n\
Cambie el permiso de escritura o renombre manualmente\n\
el archivo que esté enlazando, para que ya no tenga el\n\
mismo nombre. "

102 "Operación cancelada\n\n\
%s\n\
no se puede eliminar.\n\n\
La causa más probable de este problema es que no\n\
tenga permiso de Escritura para el archivo. \n\
Cambie el permiso de escritura o renombre manualmente\n\
el archivo que esté cambiando, para que ya no tenga el\n\
mismo nombre. "

103 "Se ha cancelado Mover - No se ha movido ningún objeto. \n\n\
El siguiente objeto no se ha podido renombrar automáticamente\n\
%s\n\n\
Se han renombrado %d objetos antes de detener el proceso de mover. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el objeto mencionado. Cambie el permiso de escritura o\n\
renombre manualmente el objeto que está moviendo, para que ya no tenga\n\
el mismo nombre. "

104 "Se ha cancelado Copiar - No se ha copiado ningún objeto. \n\n\
El siguiente objeto no se ha podido renombrar automáticamente\n\
%s\n\n\
Se han renombrado %d objetos antes de detener el proceso de copia.\n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el objeto mencionado. Cambie el permiso de escritura o\n\
renombre manualmente el objeto que esté copiando, para que ya no tenga\n\
el mismo nombre. "

105 "Se ha cancelado Enlazar - No se ha enlazado ningún objeto. \n\n\
El siguiente objeto no se ha podido renombrar automáticamente \n\
%s\n\n\
Se han renombrado %d objetos antes de detener el proceso de enlazar. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el objeto mencionado. Cambie el permiso de escritura \n\
o renombre manualmente el objeto que está enlazando, para que ya no\n\
tenga el mismo nombre. "

106 "Se ha cancelado la operación - No se ha cambiado ningún objeto. \n\n\
El siguiente objeto no se ha podido renombrar automáticamente \n\
%s\n\n\
Se han renombrado %d objetos antes de detener el proceso. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el objeto mencionado. Cambie el permiso de escritura \n\
o renombre manualmente el objeto que esté cambiando, para que ya no \n\
tenga el mismo nombre. "

107 "Se ha cancelado Mover - No se ha movido ningún objeto. \n\n\
El siguiente objeto no se ha podido renombrar automáticamente \n\
%s\n\n\
Se ha renombrado 1 objeto antes de detener el proceso de mover.\n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el objeto mencionado. Cambie el permiso de escritura \n\
o renombre manualmente el objeto que está moviendo, para que ya no \n\
tenga el mismo nombre. "

108 "Se ha cancelado Copiar - No se ha copiado ningún objeto. \n\n\
El siguiente objeto no se ha podido renombrar automáticamente \n\
%s\n\n\
Se ha renombrado 1 objeto antes de detener el proceso de copia. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el objeto mencionado. Cambie el permiso de escritura\n\
o renombre manualmente el objeto que esté copiando, para que ya no \n\
tenga el mismo nombre. "

109 "Se ha cancelado Enlazar - No se ha enlazado ningún objeto.\n\n\
El siguiente objeto no se ha podido renombrar automáticamente \n\
%s\n\n\
Se ha renombrado 1 objeto antes de detener el proceso de enlazar. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el objeto mencionado. Cambie el permiso de escritura\n\
o renombre manualmente el objeto que está enlazando, para que ya no \n\
tenga el mismo nombre."

110 "La operación se ha cancelado - No se ha cambiado ningún objeto. \n\n\
El siguiente objeto no se ha podido renombrar automáticamente\n\
%s\n\n\
Se ha renombrado 1 objeto antes de detener el proceso. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el objeto mencionado. Cambie el permiso de escritura \n\
o renombre manualmente el objeto que está cambiando, para que ya no\n\
tenga el mismo nombre. "

111 "Se ha cancelado Mover - no se ha movido ningún objeto. \n\n\
No se ha podido suprimir una carpeta que desea sustituir. \n\
   Carpeta: %s\n\n\
Puede que se hayan suprimido algunos objetos de la carpeta \n\
antes de detener el proceso de mover. \n\n\
Se han suprimido %d objetos más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para la carpeta mencionada. Cambie el permiso de escritura\n\
o renombre la carpeta manualmente, para que ya no tenga el mismo nombre. "

112 "Se ha cancelado Copiar - no se ha copiado ningún objeto. \n\n\
No se ha podido suprimir una carpeta que desea sustituir. \n\
   Carpeta: %s\n\n\
Puede que se hayan suprimido algunos objetos de la \n\
carpeta antes de detener el proceso de copiar. \n\n\
Se han suprimido %d objetos más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para la carpeta mencionada. Cambie el permiso de escritura\n\
o renombre manualmente la carpeta que está copiando, para que ya no \n\
tenga el mismo nombre. "

113 "Se ha cancelado Enlazar - no se ha enlazado ningún objeto. \n\n\
No se ha podido suprimir una carpeta que desea sustituir. \n\
   Carpeta: %s\n\n\
Puede que se hayan suprimido algunos objetos de esta \n\
carpeta antes de detener el proceso de enlazar. \n\n\
Se ha suprimido %d objetos más.\n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para la carpeta mencionada. Cambie el permiso de escritura o\n\
reombre manualmente la carpeta que está enlazando, para que ya no \n\
tenga el mismo nombre. "

114 "Se ha cancelado la operación - no se ha cambiado ningún objeto. \n\n\
No se ha podido suprimir una carpeta que desea sustituir. \n\
   Carpeta: %s\n\n\
Puede que se hayan suprimido algunos objetos de esta \n\
carpeta antes de detener el proceso. \n\n\
Se han suprimido %d objetos más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para la carpeta mencionada. Cambie el permiso de escritura\n\
o renombre manualmente la carpeta que está cambiando, para que ya \n\
no tenga el mismo nombre. "

115 "Se ha cancelado Mover - no se ha movido ningún objeto. \n\n\
No se ha podido suprimir una carpeta que desea sustituir. \n\
   Carpeta: %s\n\n\
Puede que se hayan suprimido algunos objetos de esta carpeta \n\
antes de detener el proceso de mover. \n\n\
Se ha suprimido 1 objeto más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para la carpeta mencionada. Cambie el permiso de escritura\n\
o renombre manualmente la carpeta que esté moviendo, para que ya no\n\
tenga el mismo nombre. "

116 "Se ha cancelado Copiar - no se ha copiado ningún objeto. \n\n\
No se ha podido suprimir una carpeta que desea sustituir. \n\
   Carpeta: %s\n\n\
Puede que se hayan suprimido algunos objetos de esta \n\
carpeta antes de detener el proceso de copiar.\n\n\
Se ha suprimido 1 objeto más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para la carpeta mencionada. Cambie el permiso de escritura\n\
o renombre manualmente la carpeta que esté copiando, para que ya no\n\
tenga el mismo nombre. "

117 "Se ha cancelado Enlazar - no se ha enlazado ningún objeto. \n\n\
No se ha podido suprimir una carpeta que desea sustituir. \n\
   Carpeta: %s\n\n\
Puede que se hayan suprimido algunos objetos de esta\n\
carpeta antes de detener el proceso de enlazar. \n\n\
Se ha suprimido 1 objeto más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para la carpeta mencionada. Cambie el permiso de escritura\n\
o renombre manualmente la carpeta que está enlazando, para que ya no \n\
tenga el mismo nombre. "

118 "La operación se ha cancelado - no se ha cambiado ningún objeto. \n\n\
No se ha podido suprimir una carpeta que desea sustituir. \n\
   Carpeta: %s\n\n\
Puede que se hayan suprimido algunos objetos de esta carpeta \n\
antes de detener el proceso. \n\n\
Se ha suprimido 1 objeto más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para la carpeta mencionada. Cambie el permiso de escritura\n\
o renombre manualmente la carpeta que está cambiando, para que ya \n\
no tenga el mismo nombre. "

119 "Se ha cancelado Mover - no se ha movido ningún objeto. \n\n\
No se ha podido suprimir un archivo que desea sustituir. \n\
    Archivo: %s\n\n\
Se han suprimido %d objetos más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el archivo mencionado. Cambie el permiso de escritura o\n\
renombre manualmente el archivo que está moviendo, para que ya no \n\
tenga el mismo nombre. "

120 "Se ha cancelado Copiar - no se ha copiado ningún objeto. \n\n\
No se ha podido suprimir un archivo que desea sustituir. \n\
    Archivo: %s\n\n\
Se han suprimido %d objetos más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el archivo mencionado. Cambie el permiso de escritura\n\
o renombre manualmente el archivo que esté copiando, para que ya no\n\
tenga el mismo nombre. "

121 "Se ha cancelado Enlazar - no se ha enlazado ningún objeto. \n\n\
No se ha podido suprimir un archivo que desea sustituir. \n\
    Archivo: %s\n\n\
Se han suprimido %d objetos más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el archivo mencionado. Cambie el permiso de escritura o \n\
renombre manualmente el archivo que está enlazando, para que ya no \n\
tenga el mismo nombre. "

122 "Se ha cancelado la operación - no se ha cambiado ningún objeto. \n\n\
No se ha podido suprimir un archivo que desea sustituir. \n\
    Archivo: %s\n\n\
Se han suprimido %d objetos más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el archivo mencionado. Cambie el permiso de escritura o\n\
renombre manualmente el archivo que está cambiando, para que ya no \n\
tenga el mismo nombre. "

123 "Se ha cancelado Mover - no se ha movido ningún objeto. \n\n\
No se ha podido suprimir un archivo que desea sustituir.  \n\
\ Archivo: %s\n\n\
Se ha suprimido 1 objeto más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el archivo mencionado. Cambie el permiso de escritura\n\
o renombre manualmente el archivo que está moviendo, para que ya no \n\
tenga el mismo nombre. "

124 "Se ha cancelado Copiar - no se ha copiado ningún objeto. \n\n\
No se ha podido suprimir un archivo que desea sustituir. \n\
    Archivo: %s\n\n\
Se ha suprimido 1 objeto más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el archivo mencionado. Cambie el permiso de escritura\n\
o renombre manualmente el archivo que está copiando, para que ya no \n\
tenga el mismo nombre. "

125 "Se ha cancelado Enlazar - no se ha enlazado ningún objeto.\n\n\
No se ha podido suprimir un archivo que desea sustituir. \n\
    Archivo: %s\n\n\
Se ha suprimido 1 objeto más. \n\n\
La causa más probable de este problema es que no tenga permiso de\n\
Escritura para el archivo mencionado. Cambie el permiso de escritura\n\
o renombre manualmente el archivo que está enlazando, para que ya \n\
no tenga el mismo nombre. "

126 "Se ha cancelado la operación - no se ha cambiado ningún objeto. \n\n\
No se ha podido suprimir un archivo que desea sustituir. \n\
    Archivo: %s\n\n\
Se ha suprimido 1 objeto más. \n\n\
La causa más probable de este problema es que no tenga permiso de \n\
Escritura para el archivo mencionado. Cambie el permiso de escritura\n\
o renombre manualmente el archivo que está cambiando, para que ya\n\
no tenga el mismo nombre. "

127 "Se ha movido 1 objeto."

128 "Se han movido %d objetos."

129 "Se ha copiado 1 objeto."

130 "Se han copiado %d objetos."

131 "Se ha enlazado 1 objeto."

132 "Se han enlazado %d objetos."

133 "Se ha cambiado 1 objeto."

134 "Se han cambiado %d objetos. "

135 "No se puede mover el objeto %s en sí mismo."

136 "No se puede copiar el objeto %s en sí mismo."

137 "No se puede enlazar el objeto %s a sí mismo."

138 "No se han podido renombrar los siguientes objetos \n\n\
%s\n\n\
La causa más probable es que no tenga permiso \n\
para renombrar estos objetos. "

139 "No se han podido sustituir los siguientes objetos \n\n\
%s\n\n\
La causa más probable es que no tenga permiso\n\
para sustituir estos objetos. "

140 Cambiar el nombre del nuevo objeto a:

141 Cambiar el nombre de los nuevos objetos anexando un número

$set 10        Redefine set#

$ ****** Module: FileDialog.c ****(developer comment only)********   
$ Messages 1, 2, 20, and 21 are titles for error dialogs   
$ Message 3 is an error message for dialogs requiring new file names   
$ Messages 5 and 27 are titles for the Copy File dialog, Message 5 is used   
$   when the user has specified a title for dtfile windows   
$ Messages 6-9 are labels for the File Copy, New Dir, and New File dialogs   
$ Messages 10-13 and 25 are error messages for the file copy operation   
$ Messages 14 and 28 are titles for the New Folder dialog, Message 14 is   
$   used when the user has specified a title for dtfile windows   
$ Message 16 is a label for the New Folder dialog   
$ Messages 22-24 are error messages for the New File/Folder operations   
$ Messages 26 and 29 are titles for the New File dialog, Message 26 is used   
$   when the user has specified a title for dtfile windows   
$ Messages 30 and 31 are titles for the Move File dialog, Message 30 is used   
$   when the user has specified a title for dtfile windows   
$ Messages 32 and 33 are titles for the Link File dialog, Message 32 is used   
$   when the user has specified a title for dtfile windows   

1 "Error al renombrar archivo"

2 "Error al copiar archivo"

3 "En un nombre de archivo o carpeta no se admiten espacios."

5 "Copiar archivo"

6 "Nombre del archivo actual:"

7 "Nombre del nuevo archivo:"

8 "Tipo de archivo"

9 "Mostrar tipo de archivo"

10 "El archivo seleccionado originalmente ya no existe."

11 "Un archivo o carpeta no puede tener un nombre en blanco."

13 "Nombre de archivo o de carpeta no válido."

14 "Carpeta nueva"

16 "Nombre de la carpeta nueva:"

20 "Error al crear carpeta"

21 "Error al crear archivo"

22 "No se puede crear un archivo o carpeta con un nombre en blanco."

23 "Nombre de carpeta no válido."

24 "Nombre de archivo no válido."

25 "No puede utilizar el mandato copiar para copiar un\n\
archivo al Espacio de trabajo. Debe utilizar el mandato\n\
`Poner En Espacio de trabajo` del menú Archivo o soltar el\n\
archivo en el Espacio de trabajo."

26 "Archivo nuevo"

27 "Gestor de archivos- Copiar archivo"

28 "Gestor de archivos - Nueva carpeta"

29 "Gestor de archivos - Archivo nuevo"

30 "Mover archivo"

31 "Gestor de archivos - Mover archivo"

32 "Enlazar archivo"

33 "Gestor de archivos - Enlazar archivo"

34 "Carpeta de Destino:"

35 "Nombre del Objeto:"

36 "Mostrar Icono"

37 "Objeto seleccionado:"

38 "Nombre para copiar:"

39 "Renombrar error"

40 "Este objeto es una Acción.\n\
   Las etiquetas del icono de acción no se pueden renombrar directamente."

41 "Ya se muestra el icono correcto para este tipo de datos."

42 "Mover objeto"

43 "Gestor de archivos - Mover objeto"

44 "Copiar objeto"

45 "Gestor de Archivos - Copiar objeto"

46 "Enlazar objeto"

47 "Gestor de archivos - Enlazar objeto"

48 "Nombre del objeto nuevo:"


$set 11        Redefine set#

$ ****** Module: FileManip.c ****(developer comment only)********   
$ Message 6 is the title for an error dialog   
$ Messages 9, 14, 16, 28, and 30-36 are file manipulation error messages   

6 "Error en manipulación de archivo"

9 "No puede soltar archivos desde la\n\
Papelera en el Espacio de trabajo."

14 "No se puede encontrar la ubicación de las carpetas."

16 "No se puede mover una carpeta a sí misma.\n\
%s"

28 "%s no se puede encontrar."

30 "No se puede mover o renombrar la carpeta %s.\n\
Todas las vistas del Gestor de archivos visualizadas para una \n\
carpeta o sus subcarpetas deben estar cerradas para\n\
poder mover o renombrar una carpeta."

31 "Error al leer %s"

32 "No se puede renombrar %s"

33 "No se puede abrir %s"

34 "No tiene permiso para mover %s.\n\
Sin embargo, puede copiar el archivo (use <CONTROL>\n\
mientras arrastra el archivo)."

35 "No se puede copiar una carpeta en sí misma.  "

36 "No se puede crear un proceso hijo."

37 "No puede soltar archivos de la\n\
Papelera en el Espacio de trabajo.\n\
Para eliminar un objeto de la Papelera,\n\
   -Seleccione el objeto, y\n\
   -Elija 'Reponer'\n\
en el menú Archivo o en el menú emergente del objeto.\n\
Entonces, podrá arrastrar el objeto fuera del Gestor de archivos y \n\
soltarlo en el fondo del Espacio de trabajo."

38 "No tiene permiso para mover %s\n\
Sin embargo, puede copiar el objeto.\n\
Para copiar un objeto:\n\
  - pulse y mantenga retenida la tecla <Control>, y\n\
  - arrastre el objeto con el ratón.\n\
O\n\
  - use 'Copiar en' en el menú emergente 'Seleccionado' de la barra de menús."

39 "No se pudo crear proceso hijo.\n\
   Se ha alcanzado el número máximo de procesos para este sistema.\n\
   Detenga alguno de los procesos que se están ejecutando\n\
   actualmente y, a continuación, reintente esta función."

40 "Los objetos de destino y de origen son los mismos.\n\
%s"

41 "La carpeta\n\
%s\n\
no existe."

42 "No hay espacio disponible en el\n\
   dispositivo al que está copiando."

43 "No hay espacio disponible en el\n\
   dispositivo al que está moviendo."

44 "No se ha realizado ninguna Operación Mover en el objeto %s.\n\
   No puede mover un objeto a la misma carpeta en la que\n\
   se encuentra. Debe entrar una nombre de Carpeta Destino\n\
   distinto para poder mover el objeto."

45 "No se ha realizado ninguna Operación Copiar en el objeto %s.\n\
   Debe cambiar la Carpeta Destino o el Nombre para Copiar\n\
   para poder crear una copia."

46 "No se ha realizado ninguna Operación Enlazar en el objeto %s.\n\
   Debe cambiar la Carpeta Destino o el Nombre para Copiar\n\
   para poder crear un enlace."

47 "No se puede mover una carpeta en sí misma.  "

48 "No se pueden copiar los siguientes objetos a la carpeta \"%s\"\n\n\
%s\n\n\
La causa más probable es que no tenga permiso\n\
de escritura para la carpeta de destino."

49 "No se pueden mover los siguientes objetos a la carpeta \"%s\"\n\n\
%s\n\n\
La causa más probable es que no tenga permiso\n\
de escritura para la carpeta de destino. "

50 "No se pueden mover los siguientes objetos a la carpeta \"%s\"\n\n\
%s\n\n\
La causa más probable es que no tenga permiso\n\
para mover el objeto origen. "
$        
$ Messages 51 - 56 are new since CDE/SI.          
$        

51 "No es posible copiar el archivo/carpeta porque\n\
se excederá la cuota de disco en el disco adonde\n\
está realizando la operación de copiar."

52 "No es posible mover el archivo/carpeta porque\n\
se excederá la cuota de disco en el disco adonde\n\
está realizando la operación de mover."

53 "La operación de copia del archivo/carpeta no ha sido\n\
satisfactoria debido a algún error interno. El error\n\
interno proporcionado es:"

54 "La operación de mover el archivo/carpeta no ha sido\n\
satisfactoria debido a algún error interno. El error\n\
interno proporcionado es:"

55 "Por favor, póngase en contacto con el Administrador del sistema"

56 "(Desconocido):"

$quote

57 No cuenta con autorización para mover la carpeta\n\
%s\n\
Es necesario contar con autorización.

58 Process Create Error
59 Cannot create a child process to delete the dropped files.

$set 12        Redefine set#

$ ****** Module: FileMgr.c ****(developer comment only)********   
$ Message 3 is a dialog title   
$ Messages 4-6 are movement warning or error messages   
$ Message 7 is the title for the folder popup menus   

$quote "
3 "Mover archivo(s)"

4 "El objeto %s está actualmente en la carpeta %s.\n\
Está moviendo el objeto a la carpeta %s.\n\
¿Es esta acción la que desea realizar?"

5 "El objeto %1$s está actualmente en la carpeta %2$s.\n\
Está moviendo el objeto a la carpeta %3$s.\n\
¿Es esta acción la que desea realizar?"

6 "Por lo menos uno de los archivos que ha soltado es de una\n\
carpeta distinta. Está moviendo todos estos archivos a %s.\n\
¿Es esto lo que desea hacer?"

7 "Gestor de archivos"

18 "La carpeta %s está vacía."

19 "La carpeta %s no contiene subdirectorios."

$set 13        Redefine set#

$ ****** Module: Filter.c ****(developer comment only)********   
$ Messages 11 and 21-22 are button labels for the Modify Filter List dialog   
$ Messages 19 and 24 are titles for the Link File dialog, Message 19 is used   
$   when the user has specified a title for dtfile windows   
$ Message 23 is a text field label for the Modify Filter List dialog   
$ Message 25 is a label for the Modify Filter List dialog   

11 "Predeterminados"

19 "Establecer Opciones de Filtro"

21 "Seleccionar todo"

22 "Deseleccionar todo"

23 "Cadena de filtro (Opcional):"

24 "Gestor de archivos - Establecer opciones de filtro"

25 "Seleccionar tipos de archivo a ocultar:"

26 "También oculto (Opcional):"

27 "Mostrado"

28 "Oculto"

29 "También mostrar (Opcional):"

30 "Seleccionar Tipos de datos a Ocultar:"

31 "Seleccionar tipo de datos como:"

$set 14        Redefine set#


$set 15        Redefine set#

$ ****** Module: Find.c ****(developer comment only)********   
$ Message 1 is the title for an error dialog   
$ Messages 14 and 15 are button labels for the Find Files dialog   
$ Messages 23, 26, and 27 are Find Files error messages   
$ Messages 29 and 41 are titles for the Find Files dialog, Message 29 is used   
$   when the user has specified a title for dtfile windows   
$ Messages 30-40 and 42 are labels for the Find Files dialog   

1 "Buscar error archivo"

14 "Iniciar"

15 "Detener"

23 "Información de buscar no válida.\n\n\
Se debe especificar una carpeta de búsqueda."

26 "Información de buscar no válida.\n\n\
El archivo seleccionado ya no existe."

27 "No se han encontrado archivos que coincidan con los datos de búsqueda."

29 "Buscar archivos o carpetas"

30 "Cumplimente uno o más campos para especificar los archivos a buscar:"

31 "Nombre de archivo o carpeta:"

32 "Contenido del archivo:"
$ Messages 33-35 are used on hpux systems   

33 "Activada"

34 "Desactivada"

35 "Siguen los enlaces:"

36 "Abrir nueva vista"

37 "Poner en el Espacio de trabajo"

38 "Archivos encontrados (por Contenido):"

39 "Archivos encontrados (por Nombre y Contenido):"

40 "Archivos encontrados (por Nombre):"

41 "Gestor de archivos - Buscar archivos o carpetas"

42 "Buscar carpeta:"

43 "Buscar error"

44 "Falta argumento de nombre Buscar Carpeta.\n\
Escriba el nombre de la carpeta en donde desea comenzar la búsqueda."

45 "El archivo seleccionado ya no existe.\n\n\
Alguién suprimió el archivo después de que se completara el proceso de búsqueda."

46 "Buscar"

47 "Cumplimente uno o más campos para especificar los elementos a buscar:"

48 "Abrir carpeta"

49 "Gestor de archivos - Buscar"

50 "Falta nombre de Buscar Carpeta o argumento de Contenido de Archivo.\n\
Escriba el nombre de la carpeta donde desea comenzar la búsqueda.\n\
O escriba la cadena que desea buscar."

$set 16        Redefine set#


$set 17        Redefine set#


$set 18        Redefine set#

$ ****** Module: Main.c ****(developer comment only)********   
$ Message 1 is a blank line   
$ Messages 2, 6, 17-18 and 27-28 are error messages which will ONLY APPEAR   
$   in the DT ERRORLOG FILE   
$ above no longer true - message 2 will be displayed.
$ Messages 16 and 32 are titles for the Save Settings dialog, Message 16 is   
$   used when the user has specified a title for dtfile windows   
$ Messages 19-21 are error messages for invalid directory input.   
$ Message 22 is the title for an error dialog   
$ Messages 25 and 26 are usage strings   
$ Messages 29 and 30 are text for the Save Settings dialog   
$ Messages 34 and 35 are drag/drop error messages   
$ DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGE 1   

1 ""
$ MESSAGE 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE   

2 "No se pudo crear la carpeta .dt o subcarpetas."
$ MESSAGE 6 WILL ONLY APPEAR IN THE DT ERRORLOG FILE, and stderr   

6 "Se ha producido un error X Toolkit... Se sale.\n"

16 "Guardar Como Opciones Predeterminadas"
$ MESSSAGES 17-18 WILL ONLY APPEAR IN THE DT ERRORLOG FILE   

17 "No es posible crear un archivo para almacenar las opciones por omisión."

18 "No se pudo abrir el archivo de sesión."

19 "La especificación de carpeta,\n\
%s\n\
no es una carpeta."

20 "La especificación de carpeta,\n\
%s\n\
no existe."

21 "Especificación de carpeta no válida, %s:%s."

22 "Error arrastrar"

23 "\n\
Uso:   %s...\n\n\
   -noview\n\n\
\tDtfile se ejecuta en modo de servidor.\n\n\
   -session ArchivoSesión\n\n\
\tDtfile se ejecuta con el archivo de sesión especificado\n\
\ten el parámetro ArchivoSesión\n\n\
   -folder Carpeta[,Carpeta,Carpeta]\n\
   -dir Carpeta[,Carpeta,Carpeta]\n\n\
\tDtfile muestra una ventana para cada carpeta especificada en\n\
\tel parámetro Carpeta. El parámetro Carpeta puede contener\n\
\tvarias carpetas separadas por comas. Las carpetas pueden estar\n\
\ten el formato 'ruta'.\n\n\
   -title Título\n\n\
\tDtfile usa la cadena especificada en el parámetro Título\n\
\tcomo el título para sus ventanas.\n\n\
   -help_volume VolumenAyudas\n\n\
\tDtfile utiliza el volumen de ayudas especificado en el parámetro\n\
\tVolumenAyudas.\n\n\
   -restricted\n\n\
\tDtfile no mostrará las carpetas por encima de la carpeta restringida.\n\
\tSi se utiliza la opción -dir, la carpeta especificada en dicha opción\n\
\tes la carpeta restringida. Si no se utiliza la opción -dir, la \n\
\tcarpeta actual del usuario es la carpeta restringida.\n\n\
   -grid on/off\n\n\
\ton     = Los archivos se muestran en un patrón de cuadrícula.\n\
\toff    = Los archivos se muestran tal como están situados.\n\n\
   -tree on/off\n\n\
\ton     = Los archivos se muestran en modo de carpetas individuales.\n"

24 "\toff    = Los archivos se muestran en modo de árbol de carpetas.\n\n\
   -tree_files /never/always/choose\n\n\
\tnever  = El modo de árbol tiene dos estados: parcialmente ampliado \n\
\t         o colapsado.\n\
\talways = El modo de árbol tiene dos estados: completamente ampliado \n\
\t         o colapsado.\n\
\tchoose = El modo de árbol tiene tres estados: parcialmente ampliado, \n\
\t         completamente ampliado o colapsado.\n\n\
   -order alphabetical/file_type/date/size\n\n\
\tLos archivos se muestran en el orden especificado: alfabéticamente, por\n\
\ttipo de archivo, por fecha o por tamaño.\n\n\
   -view no_icon/large_icon/small_icon/attributes\n\n\
\tLos archivos se muestran en el formato especificado: sólo texto, texto e \n\
\ticonos grandes, texto e iconos pequeños, con atributos.\n\n\
   -direction ascending/descending\n\n\
\tLos archivos se visualizan en la dirección especificada: ascendente o\n\
\tdescendente.\n\n\
   -large_icon_width <tamaño>\n\n\
   -large_icon_height <tamaño>\n\n\
   -small_icon_width  <tamaño>\n\n\
   -small_icon_height <tamaño>\n\n\
       El tamaño del área de mostrar para las imágenes de iconos \n\
       en el Gestor de archivos.\n\
       Las imágenes de iconos mayores que este tamaño \n\
       se recortarán a este tamaño.\n\
       El tamaño del área de mostrar predeterminada es como \n\
       máximo 38 y como mínimo 24\n\n"

26 "\n\
Uso en Modo Restringido:   %s...\n\n\
   -folder Carpeta[,Carpeta,Carpeta]\n\
   -dir Carpeta[,Carpeta,Carpeta]\n\n\
\tDonde Carpeta es una carpeta que está por debajo y/o incluye\n\
\ta la carpeta de Inicio del usuario.\n\n"
$ MESSSAGES 27-28 WILL ONLY APPEAR IN THE DT ERRORLOG FILE   

27 "No es posible recuperar el archivo de configuración de inicio guardado, se usará el predeterminado.\n"

28 "No es posible recuperar el archivo de configuración de herramienta predeterminado guardado, se usará el predeterminado.\n"
$ Messages 29-30 are the strings displayed in a question dialog   

29 "¿Guardar el tamaño de ventana, las preferencias y los valores de\n\
filtro de la Vista del Gestor de archivos actual?\n\n\
Se utilizarán para cualquier nueva ventana de la Vista del\n\
Gestor de archivos iniciada desde el Panel frontal."

30 "¿Guardar el tamaño de ventana, las preferencias y los valores\n\
de filtro de la Vista del Gestor de archivos restringida actual?\n\n\
Se utilizarán para cualquier nueva ventana de Vista del Gestor de\n\
archivos restringida iniciada desde el Panel frontal."

32 "Gestor de archivos - Guardar como opciones predeterminadas"

34 "No puede copiar ni enlazar un archivo o\n\
carpeta a la Papelera."
$ 35 You can't copy or link a file or  
$ \

$ folder out of the Trash Can.   

35 "¿Guardar la configuración de filtro, preferencias \n\
y tamaño de ventanas actuales de la Vista del\n\
Gestor de aplicaciones?\n\n\
Se utilizarán por cualquiera otra nueva ventana\n\
del Gestor de aplicaciones iniciada en el Panel Frontal."

36 "No puede copiar ni enlazar un archivo o carpeta fuera de la Papelera.\n\
Mueva el objeto fuera de la Papelera y póngalo en el Gestor de archivos.\n\
A continuación, puede copiarlo o enlazarlo desde allí."

37 "No puede copiar ni enlazar un archivo o carpeta fuera de la Papelera.\n\
Mueva el objeto fuera de la Papelera y póngálo en el Gestor de archivos.\n\
A continuación, puede copiarlo o enlazarlo desde allí."

38 "Especificación de carpeta no válida, %s"

$ MESSSAGE 39 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$quote
39 No se puede iniciar una segunda sesión del Administrador de archivos debido a que ya hay una sesión\n\
del Administrador de archivos abierta en el escritorio.  Si desea iniciar una nueva sesión\n\
del Administrador de archivos, deberá cerrar todas la ventanas del Administrador de archivos abiertas actualmente\n\
y quitar todos los objetos del Administrador de archivos de las áreas de trabajo.

$ 
$ "-NEW"
40 "Could not connect to ToolTalk:\n%s\nExiting ..."


$set 19        Redefine set#


$set 20        Redefine set#

$ ****** Module: Menu.c ****(developer comment only)********   
$ Messages 1-8, 123 and 9 are the text and mnemonics for the dtfile menu bar   
$ Messages 10/11, 128/129, 124/125, 121/122, 126/127, 16/17, 18/19, 84/85,   
$   90/91, 22/23, 117/118, are the text and mnemonics for the Fill    
$   pulldown menu    
$ Messages 10/11, 30/33, 92/93, 34/37, 38/41, 42/94, and 97/98 are the text   
$   and mnemonics for the Folder pulldown menu   
$ Messages 46/47, 99/100, 48/51, 52/55, 101/102, 103/104, 119/120, and    
$   62/63 are the text and mnemonics for the View pulldown menu   
$ Messages 105-116 are the text and mnemonics for the Help pulldown menu   

$quote "
1 "Archivo"

2 "A"

3 "Carpeta"

4 "C"

5 "Vista"

6 "V"

7 "Seleccionada"

8 "S"

9 "y"

10 "Nuevo..."

11 "N"

16 "Propiedades..."

17 "P"

18 "Buscar..."

19 "B"

22 "Mostrar papelera..."

23 "t"

30 "Actualizar"

33 "A"

34 "Cambiar A..."

37 "C"

38 "Ir a Inicio"

41 "I"
$ NOTE: The mnemonic for Up is Message 94 in this set.   

42 "Arriba"

46 "Nuevo"

47 "N"

48 "Seleccionar todo"

51 "S"

52 "Deseleccionar todo"

55 "D"

62 "Guardar todas propiedades ..."

63 "G"

84 "Poner en el Espacio de trabajo"

85 "E"

90 "Suprimir en papelera"

91 "S"

92 "Cambio rápido A"

93 "b"
$ NOTE: Message 94 is the mnemonic for the "Up" menu item   

94 "A"

97 "Terminal"

98 "T"

99 "Ordenar objetos"

100 "O"

101 "Mostrar archivos ocultos"

102 "a"

103 "Modificar lista de filtros..."

104 "l"

105 "Información general"

106 "I"

107 "Tareas"

108 "T"

109 "Referencia"

110 "R"

111 "Sobre el tema"

112 "e"

113 "Uso de la ayuda"

114 "U"

115 "Acerca del Gestor de archivos"

116 "A"

117 "Cerrar"

118 "C"

119 "Establecer propiedades de vista..."

120 "v"

121 "Copiar..."

122 "o"

123 "Ayuda"

124 "Mover..."

125 "M"

126 "Enlazar..."

127 "E"

128 "Renombrar"

129 "n"

130 "Carpeta nueva"

131 "n"

132 "Archivo nuevo..."

133 "v"

134 "Ir a..."

135 "I"

136 "Abrir papelera"

137 "Renombrar..."

138 "Abrir nueva vista"

139 "A"

140 "Copiar como enlace..."

141 "Establecer opciones vista..."

142 "Guardar como opciones predeterminadas..."

143 "G"

144 "Establecer opciones de filtro..."

145 "C"

146 "Abrir terminal"

147 "A"

148 "Tabla de contenido"

149 "C"

150 "Cambiar permisos..."

151 "Echar en la papelera"

152 "Mover a..."

153 "Copiar en..."

154 "Copiar como enlace..."

155 "Ir arriba"

156 "Mostrar objetos ocultos"

157 "Acerca de Gestor de aplicaciones"

$set 21        Redefine set#

$ ****** Module: ModAttr.c ****(developer comment only)********   
$ Messages 1 and 21 are titles for the File Properties dialog, Message 1 is   
$   used when the user has specified a title for dtfile windows   
$ Messages 2-13 and 18 are labels for the File Properties dialog   
$ Message 14 is the output of the file modification date; DO NOT LOCALIZE   
$ Messages 15 and 16 are file properties error messages   
$ Message 17 is the title for an error dialog   
$ Message 19 is the default file owner/group, if real one can't be determined   
$ Messages 22-28 are error messages that can be generated when dtfile   
$   tries to use the dtfile.config configuration file, this file is   
$   used to customize the File Properties dialog   

1 "Permisos de archivo"

2 "Nombre del archivo:"

3 "Nombre del propietario:"

4 "Nombre del grupo:"

5 "Permisos:"

6 "Lectura"

7 "Escritura"

8 "Ejecución"

9 "Propietario:"

10 "Grupo:"

11 "Otros:"

12 "Tamaño (en bytes):"

13 "Modificado:"
$ DO NOT LOCALIZE MESSAGE 14   

14 "%x %X"

15 "Información de atributos de archivo no válida.\n\n\
El usuario especificado no existe."

16 "Información de atributos de archivo no válida.\n\n\
El grupo especificado no existe."

17 "Error propiedades"

18 "Se enlaza a:"

19 "Desconocido"

21 "Gestor de archivos - Permisos de archivo"

22 "No se puede abrir el archivo de configuración del gestor de archivos:"

24 "Etiqueta de campo desconocida en el archivo de configuración del gestor de archivos:"

25 "No encontrada información para el identificador del sistema-archivos en el archivo de configuración del gestor de archivos"

26 "No se puede crear un proceso para el diálogo del sistema de archivos"

27 "Error al ejecutar el diálogo de sistema de archivos"

28 "Intentando ejecutar:"
$ do not localize "fsDialog"   

29 "No se ha proporcionado valor para el campo fsDialog en el archivo de configuración del dtfile"

30 "Nombre:"

31 "Nombre de propietario no válido.\n\
El usuario especificado no existe."

32 "El Nombre de grupo no es válido.\n\n\
No se ha creado el grupo de usuarios con este nombre en el sistema.\n\
Utilice el software de administración del sistema si desea crear\n\
un grupo de usuarios con este nombre."

33 "Error de permisos"

34 "Gestor de archivos - Permisos"

35 "El diálogo Permisos para el objeto\n\n\
%s/%s\n\n\
se ha cerrado porque este objeto ha sido suprimido\n\
del sistema."

36 "Aviso de Cambiar permisos"

37 "Está cambiando los permisos de la Carpeta Actual\n\
de forma que en lo sucesivo no se le permitirá\n\
visualizarla. La carpeta se cerrará automáticamente\n\
y ya no le será posible abrirla.\n\n\
¿Desea que se lleve a cabo esta acción?"
38 File Manager Error
39 File Open Error

$set 22        Redefine set#

$ ****** Module: ModAttrP.c ****(developer comment only)********   
$ Messages 1 and 3 are file properties error messages   
$ Message 2 is the title for the properties error dialog   

1 "No se puede abrir el archivo, operación cancelada anormalmente"

2 "Error propiedades"

3 "No se pueden cambiar las propiedades de %s"

4 "No se puede abrir el objeto."

5 "Establecer error de permisos"

$set 23        Redefine set#

$ ****** Module: Prefs.c ****(developer comment only)********   
$ Messages 1 and 22 are titles for the Set Preferences dialog, Message 1 is   
$   used when the user has specified a title for dtfile windows   
$ Messages 2-20 and 23-29 are labels for the Set Preferences dialog   

1 "Establecer opciones de vista"

2 "Vista"

3 "Por nombre"

4 "Por nombre e icono"

5 "Por nombre e icono pequeño"

6 "Por nombre, fecha, tamaño..."

7 "Mostrar"

8 "Por carpeta sencilla"

9 "Por árbol "

10 "Ordenar"

11 "Por tipo de archivo"

12 "Alfabéticamente"

13 "Por fecha"

14 "Por tamaño"

15 "Dirección"

16 "Ascendente"

17 "Descendente"

18 "Colocación"

19 "Ordenados en cuadrícula"

20 "Como está colocado"

22 "Gestor de archivos - Establecer opciones de vista"

23 "Cabeceras"

24 "Mostrar ruta con iconos"

25 "Carpeta actual"

26 "Línea de estado"

27 "Sólo carpetas"

28 "Carpetas, después archivos"

29 "Carpetas y archivos"

30 "Predeterminados"

31 "Representación"

32 "Sólo por nombre"

33 "Por iconos grandes"

34 "Por iconos pequeños"

35 "Filas y columnas"

36 "Ruta de texto"

37 "Línea de mensaje"

$set 24        Redefine set#


$set 25        Redefine set#


$set 26        Redefine set#


$set 27        Redefine set#

$ ****** Module: Trash.c ****(developer comment only)********   
$ Messages 3, 4, and 81 are titles for error warning dialogs   
$ Message 5-8, 56, 58, 78, 82, and 83 are trash error messages   
$ Message 9 is the title of the trash container   
$ Messages 24/26 and 28/30 are the text and mnemonics for the File pulldown   
$   menu   
$ Message 73 is an error dialog title   
$ Messages 74, 75, 79, and 80 are trash warning messages   

3 "Error en papelera"

4 "Aviso de papelera"

5 "Los archivos o carpetas siguientes no se han podido colocar en la papelera:\n"

6 "Cada una de las siguientes es una carpeta no vacía o un\n\
   archivo para el que no tiene permiso de escritura.\n\
   ¿Desea proseguir?\n"

7 "No tiene permiso de escritura para los archivos siguientes.\n\
   ¿Desea proseguir?\n"

8 "Las carpetas siguientes no están vacías.\n\
   ¿Desea proseguir?\n"

9 "Papelera"

24 "Reponer"

26 "R"

28 "Eliminar definitivamente"

30 "l"

56 "Los archivos siguientes no se han podido reponer:\n"
$ Message 58 supplements Message 56   

58 "(Más de %d archivos adicionales)"

73 "Eliminar definitivamente archivo(s)"

74 "El archivo siguiente también está en el Espacio de trabajo:\n\
   %s\n\
Si se mueve el archivo a la papelera se suprimirá\n\
del Espacio de trabajo."

75 "Los archivos siguientes también están en el Espacio de trabajo:\n\
   %s\n\
Si se mueven los archivos a la papelera se suprimirán\n\
del Espacio de trabajo."

78 "El Gestor de archivos no da soporte a archivos o\n\
carpetas cuyos nombres contengan espacios, \n\
por lo tanto no se pueden suprimir.\n"

79 "El archivo del Espacio de trabajo %s también está en el sistema de archivos, en:\n\
   %s\n\
Si se mueve el archivo a la papelera se suprimirán del sistema\n\
de archivos."

80 "Los archivos del Espacio de trabajo también están en el sistema de archivos, en:\n\
   %s\n\
Si se mueven los archivos a la papelera se eliminarán del sistema\n\
de archivos."

81 "Error en eliminación definitiva de archivo/carpeta"

82 "El archivo siguiente no se ha podido suprimir del sistema de archivos:\n"

83 "El archivo .trashinfo de $HOME/.dt/Trash no se puede abrir.\n\
   Necesita acceder a dicho archivo para que funcione la Papelera.\n\
   Compruebe los permisos sobre dicho archivo y su carpeta.\n"

84 "Cuando se eliminan definitivamente los objetos de la papelera, se eliminan \n\
   permanentemente del sistema.\n\n\
   ¿Está seguro qué desea eliminarlos definitivamente?\n"

85 "El objeto del Espacio de trabajo que está suprimiendo es una\n\
   copia enlazada de un objeto dentro del Gestor de archivos en\n\
   %s\n\
   Si pulsa OK, se suprimirá tanto la copia enlazada como el\n\
   objeto original.\n\
   Si desea suprimir el objeto del Espacio de trabajo,\n\
      1) pulse Cancelar en este diálogo\n\
      2) luego, seleccione 'Eliminar del Espacio de trabajo'\n\
         en el menú emergente de objetos del Espacio de trabajo."

86 "Los objetos del Espacio de trabajo que está suprimiendo son copias\n\
   enlazadas de objetos dentro del Gestor de archivos en\n\
   %s\n\
   Si pulsa OK, se suprimirán tanto las copias enlazadas como\n\
   los objetos originales.\n\
   Si desea suprimir los objetos del Espacio de trabajo,\n\
      1) pulse Cancelar en este diálogo\n\
      2) luego, seleccione 'Eliminar del Espacio de trabajo'\n\
         en el menú emergente de objetos del Espacio de trabajo."

87 "El objeto de la papelera no puede abrir.\n\n\
   Para abrir un objeto utilice 'Reponer' para volverlo al Gestor de archivos\n\
   y abrirlo allí."

88 "No se puede grabar en un archivo temporal.\n\
   Quizás el sistema de archivos está lleno."

89 "No se puede abrir el directorio Trash\n\
   %s\n\
   (Autentificación, problema de permiso de acceso)\n\n\
   No todas las operaciones de papelera no se realizarán."

90 "No se puede inicializar la 'Papelera'.\n\
   Por favor, compruebe:\n\
        1)Los permisos de $HOME/.dt/Trash.\n\
        2)El espacio de disco disponible.\n\
        3)La Autentificación."
$ 91 is not used   

91 "Una copia enlazada del siguiente objeto está en\n\
el fondo del Espacio de trabajo:\n\
Al mover el objeto a la papelera se eliminará del\n\
Gestor de archivos y del fondo del Espacio de trabajo."
$ 92 is not used   

92 "Una copia enlazada de los siguientes objetos está también en\n\
el fondo del Espacio de trabajo:\n\
Al mover los objetos a la papelera se suprimirán del Gestor de\n\
archivos y del fondo del Espacio de trabajo."

93 "No es posible acceder a este archivo de información de la papelera:\n\
   %s\n\
   No se realizará ninguna de las operaciones de papelera.\n\
   Las causa más comunes son:\n\
     - La Autentificación de la red.\n\
     - El espacio de disco insuficiente.\n\
     - Los permisos erróneos para $HOME/.dt/Trash."

94 "El Gestor de archivos no da soporte a objetos\n\
con espacios en blanco en sus nombres, por ello\n\
no se pueden suprimir.\n\
Para suprimir estos objetos:\n\
  - seleccione 'Abrir terminal' en la barra de menús del Gestor de archivos\n\
  - luego, emita el mandato de supresión para eliminarlos."

95 "No es posible acceder al directorio Trash:\n\
   %s\n\
   No se realizará ninguna de las operaciones de papelera.\n\
   Las causas más comunes son:\n\
     - La Autentificación de la red.\n\
     - El espacio de disco insuficiente.\n\
     - Los permisos erróneos."

96 "Los siguientes objetos no se pudieron suprimir del sistema de archivos:\n"

97 "Los objetos siguientes no se pudieron colocar en la papelera:\n"

98 "(Más de %d objeto(s) adicional(es))"
$ message 99 is not used   

99 "No tiene permiso de escritura para el(los) siguiente(s) archivo(s):\n\
¿Desea continuar?\n"

100 "La(s) siguiente(s) carpeta(s) no están vacías.\n\
¿Desea seguir?\n"

101 "El (los) objeto(s) siguiente(s) no se pudieron reponer:\n"

102 "Una copia enlazada del siguiente objeto está en el\n\
fondo del Espacio de trabajo:\n\
%s\n\
Al mover el objeto a la papelera se suprimirá del\n\
Gestor de archivos y del fondo del Espacio de trabajo."

103 "Una copia enlazada de los siguientes objetos también está en el fondo\n\
del Espacio de trabajo:\n\
%s\n\
Al mover los objetos a la papelera se suprimirán del\n\
Gestor de archivos y del fondo del Espacio de trabajo."

104 "No tiene permiso de escritura para los siguientes archivos:\n\
¿Desea seguir?\n"

105 "No se ejecutará la acción predeterminada de un objeto de papelera.\n\n\
    Para ejecutar la acción predeterminada de este objeto\n\
    utilice 'Reponer' para devolverlo al Gestor de archivos\n\
    y ejecutarlo allí."

106 "Acerca de la papelera"

107 "Hay al menos un objeto de la siguiente Carpeta que no se puede suprimir. \n\n\
%s\n\n\
La causa más probable es que no tenga los permisos necesarios\n\
para suprimir dicho objeto. "

108 "El siguiente archivo no se puede suprimir.  \n\n\
%s\n\n\
La causa más probable es que no tenga los permisos necesarios \n\
para suprimir dicho objeto. "

109 "Aviso de Reponer "

110 "Ya hay un objeto denominado \n\n\
    %s\n\n\
Seleccione Ok si desea sustituir el objeto\n\
por el objeto de la Papelera. \n\n\
Seleccione Cancelar si desea mantener el objeto. \n\
Si a continuación renombra el objeto, podrá repetir\n\
la operación de Reponer y mantener ambos objetos. "

111 "Error de Reponer "

112 "El objeto u objetos siguientes no se pueden suprimir.\n\
La causa más probable es que este objeto u\n\
objetos ya se hayan suprimido.\n\n"

113 "Por lo menos un objeto de la carpeta o carpetas\n\
siguientes o la propia carpeta no se pueden suprimir.\n\
La causa más probable es que el usuario no tenga permiso\n\
para suprimir dicho objeto.\n\n"

114 "El archivo o archivos siguientes no se pueden suprimir. La causa más probable\n\
es que el usuario no tenga permisos para suprimir el archivo o archivos.\n\n"

115 "El objeto u objetos siguientes no se pueden suprimir.\n\
El escritorio no puede funcionar correctamente sin dicho objeto u objetos.\n\n"

$set 28        Redefine set#

$ ****** Module: Desktop.c ****(developer comment only)********   
$ Messages 1-3 Workspace error messages   
$ Message 4 is the title for the Workspace popup menus   
$ Messages 5-8 are the labels and mnemonics for the Workspace obj popup menu   

1 "En el Espacio de trabajo ya existe un archivo con este nombre.\n\
Elija un nombre distinto."

2 "En la carpeta original ya existe un archivo con este nombre\n\
(La carpeta de la que procede el archivo).\n\
Elija un nombre distinto."

3 "En el Espacio de trabajo de este espacio de trabajo ya existe \n\
un archivo llamado %s.\n\
No puede poner en el Espacio de trabajo otro archivo con el mismo nombre.\n\
Para poner ambos archivos en el Espacio de trabajo, renombre uno de ellos."

4 "Objeto del espacio de trabajo"

5 "Eliminar del espacio de trabajo"

6 "E"

7 "Abrir carpeta padre"

8 "A"

9 "Ya existe un objeto con este nombre en el Espacio de trabajo.\n\
Por favor, elija un nombre diferente."

10 "Ya existe un objeto con este nombre en la carpeta original\n\
(La carpeta de la que procede este).\n\
Por favor, elija un nombre distinto."

11 "Ya existe un objeto denominado %s en el fondo de este Espacio de trabajo.\n\
No puede poner otro objeto con el mismo nombre en el Espacio de trabajo.\n\
Para tener ambos objetos en el Espacio de trabajo, renombre uno de ellos."

12 "Se ha pasado un espacio de trabajo no válido. El espacio de trabajo\n\
predeterminado será el espacio de trabajo Uno y un objeto con nombre:\n\
   %s\n\
ya existe en el fondo de este Espacio de trabajo.\n\
No se puede poner otro objeto con el mismo nombre en el Espacio de trabajo.\n\
Para tener los dos objetos en el Espacio de trabajo, renombre uno de ellos."

13 "Objeto:\n\
   %s\n\
no existe en el sistema de archivos."

$set 29        Redefine set#

$ ****** Module: Help.c ****(developer comment only)********   
$ Message 1 is the title for the dtfile help dialog   
$ Messages 1 and 2 are titles for the Help dialog, Message 2 is   
$   used when the user has specified a title for dtfile windows   
$ Message 3 is the text for help on the "Open New View" action   
$ Message 4 is the text for help on the "Open In Place" action   
$ Messages 5-6 are help error messages   

1 "Ayuda del Gestor de archivos"

2 "Ayuda %s"

3 "La acción `%s` abre una nueva vista del Gestor de archivos de la carpeta solicitada."

4 "La acción `%s` muestra la carpeta solicitada en la vista actual de Gestor de archivos."

5 "No se dispone de ayuda para la acción %s"

6 "No se dispone de ayuda para el tipo de archivo %s"

7 "La acción `%s` abre una nueva ventana de vista del Gestor de archivos\n\
de la carpeta solicitada."

8 "La acción `%s` muestra la carpeta solicitada en la ventana\n\
actual de la vista del Gestor de archivos."

9 "Ayuda papelera"

10 "No hay ayuda disponible para varios objetos seleccionados. "

$set 30        Redefine set#

$ ****** Module: SharedMsgs.c ****(developer comment only)********   
$ The following messages are used in more than one place within the   
$ dtfile source.  Unless otherwise specified, they are error messages.   

1 "No se puede leer de %s"
$ Message 2 is used to format the file attributes in 'by attributes' mode   

2 "%b %d %H:%M:%S %Y"

3 "No se puede mover ni copiar el archivo %s desde el host %s.\n\
Es posible que el host %s no esté bien montado para el sistema de archivos."

4 "No se puede mover ni copiar el archivo %1$s desde el host %2$s.\n\
El posible que el host %3$s no esté bien montado para el sistema de archivos."

5 "No se puede leer %s"

6 "No se puede escribir en la carpeta %s"

7 "No se puede crear %s"

8 "No se puede suprimir %s"

9 "Una carpeta o un archivo con el nombre\n\
  %s\n\
  ya existe y no se puede sobreescribir.\n\
  Entre un nombre diferente."
$ Message 10 is the help topic string for help on a file icon or action   

10 "Descripción de Tipo-archivo/Acción"

11 "No puede especificar una ruta como parte del nombre de archivo."

12 "Información de Buscar no válida.\n\n\
La carpeta siguiente no existe o no es accesible.\n\n\
     %s\n"
$ Messages 13-16 are dialog titles   

13 "Información general de %s"

14 "%s Guía de aprendizaje"

15 "Acerca de %s"

16 "Gestor de archivos"
$ Message 18 is appended to ".." folders   

18 "(ir arriba)"
$ Messages 19 and 20 are messages which appear when a user clicks item help   
$ outside the application.  19 is the title, while 20 is the message.   

19 "Error de ayuda sobre el tema"

20 "Debe seleccionar un componente \n\
interno del Gestor de archivos."
$ Messages 21 and 22 are titles for error dialogs   

21 "Error mover archivo"

22 "Error de enlace de archivo"

23 "No se puede crear '%s'\n\n\
La causa más común es que no tenga los permisos \n\
correctos para las carpetas y archivos involucrados.\n\n\
Par ver los permisos, seleccione el objeto y, después,\n\
seleccione 'Cambiar permisos...'"

24 "Debe seleccionar un objeto en el Gestor de archivos.\n\
La ayuda 'Sobre el tema' sólo funciona en la aplicación desde\n\
la que se ha emitido."

25 "No se puede crear '%s'\n\n\
La causa más común es que no tenga los permisos correctos\n\
para las carpetas y archivos involucrados.\n\n\
Para permisos de vista, seleccione el objeto y, a\n\
continuación, seleccione 'Cambiar permisos...' desde  \n\
Seleccionado o desde el menú emergente."

26 "Debe seleccionar un objeto en el Gestor de aplicaciones.\n\
La ayuda 'Sobre el tema' sólo funciona en la aplicación\n\
desde la que se ha emitido."

$set 31        Redefine set#


$set 32        Redefine set#

$ ****** Module: dterror.src ****(developer comment only)********   
$ Message 1 is a title for dterror   

1 "Error dt"

2 "Error de Abrir directorio del Gestor de archivos"

$set 33        Redefine set#


1 "Varios objetos"

2 "Carpeta actual"

3 "Error de Mover objeto"

4 "Error de Copiar objeto"

5 "Error de Enlazar objeto"

6 "La ubicación a la que está intentando realizar la operación Mover:\n\n\
   %s\n\n\
no existe en el sistema de archivos."

7 "La ubicación a la que está intentando realizar la operación Copiar:\n\n\
   %s\n\n\
no existe en el sistema de archivos."

8 "La ubicación a la que está intentando realizar la operación Enlazar:\n\n\
   %s\n\n\
no existe en el sistema de archivos."

9 "La ubicación a la que está intentando realizar la operación Mover:\n\n\
   %s\n\n\
no es una carpeta."

10 "La ubicación a la que está intentando realizar la operación Copiar:\n\n\
   %s\n\n\
no es una carpeta."

11 "La ubicación a la que está intentando realizar la operación Enlazar:\n\n\
   %s\n\n\
no es una carpeta."

12 "El objeto que está intentando Mover:\n\n\
   %s\n\n\
no existe en el sistema de archivos."

13 "Uno de los objetos que está intentando Mover:\n\n\
   %s\n\n\
no existe en el sistema de archivos.\n\
No se moverá ninguno de ellos."

14 "El objeto que está intentando Copiar:\n\n\
   %s\n\n\
no existe en el sistema de archivos."

15 "Uno de los objetos que está intentando Copiar:\n\n\
   %s\n\n\
no existe en el sistema de archivos.\n\
No se copiará ninguno de ellos."

16 "El objeto que está intentando Enlazar:\n\n\
   %s\n\n\
no existe en el sistema de archivos."

17 "Uno de los objetos que está intentando Enlazar:\n\n\
   %s\n\n\
no existe en el sistema de archivos.\n\
No se enlazará ninguno de ellos."


$set 99        Redefine set#

$ ****** Version Information ********   
$ DO NOT TRANSLATE THESE MESSAGES   
$ DO NOT CHANGE THESE MESSAGES   
$ DO NOT LOCALIZE THESE MESSAGES   
$ These messages are used for the version information.   

1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $TOG: dtfile.msg /main/10 1999/09/16 09:43:40 mgreess $"
