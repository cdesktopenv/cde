$ $XConsortium: dtcopy.msg /main/7 1996/11/19 20:16:37 rws $
$ ENGLISH_SOURCE_VERSION= 1.4   
$ ENGLISH_SOURCE_VERSION= 1.3   
$ %Z%%M%        %I%  %W% %G% %U%   
$ *****************************************************************************   
$ **   
$ **  File:        dtcopy.msg   
$ **   
$ **  Project:     CDE File Manager   
$ **   
$ *****************************************************************************   
$quote "

$set 2
$ ****** Module: main_dtcopy.c ******   
$ In message 1 and 2 the first string parameter will be "File" or "Files"   
$ and the second string parameter will be "Directory" or "Directories".   

1 "Se han movido %3d %s a %2d %s"

2 "Se han copiado %3d %s en %2d %s"

3 "Archivo"

4 "Archivos"

5 "Directorio"

6 "Directorios"

7 "Errores: ninguno"

8 "Errores: %d"

10 "Uso:   %s [opciones ...] origen destino\n"

11 "Mover Directorio"

12 "Copiar Directorio"
$ The string parameter in message 13 is a file name   

13 "%s no se ha eliminado debido a errores."

14 "Completado."

15 "Cancelado en"
$ ** Message 20 - 32   
$ ** LongFileOpNames ... message number = op + 20.   

20 "procesando"
$ messages 21, 22, and 23 are all used in the same context, as in   
$ "opening file 1."   

21 "abriendo"

22 "abriendo"

23 "abriendo"

24 "leyendo enlace"

25 "suprimiendo"

26 "copiando"

27 "creando directorio"

28 "copiando enlace"

29 "creando enlace"

30 "trabajando"

31 "es una copia"

32 "es un enlace"

33 "renombrando"

$set 3        Redefine set#

$ ****** Module: copydialog.c ******   

1 "Reanudar"

2 "Pausar"

3 "Mover desde:"

4 "Copiar desde:"

5 "a:"

6 "Aviso de Directorio"

7 "Una cancelación dejará un resultado \n\
parcial en el directorio de destino.\n\
¿Desea cancelar?"

8 "Sí"

9 "No"

10 "Una cancelación dejará un resultado parcial\n\
en los directorios de destino y de origen.\n\
¿Desea cancelar?"

11 "Copiar Carpeta - Aviso"

12 "Mover Carpeta - Aviso"

13 "Cancelar Copiar"

14 "Continuar Copiar"

15 "Cancelar Mover"

16 "Continuar Mover"

17 "Movido(a)"

18 "Copiado(a)"

19 "No se puede trasladar la carpeta hacia sí misma\n\
%s\n"

20 "No se puede copiar la carpeta en sí misma\n\
%s\n"



$set 4        Redefine set#

$ ****** Module: errordialog.c ******   

1 "Error %s %s:"

2 "¿Confirmar futuros errores?"

3 "Error de Directorio"

4 "Mover Carpeta - Error"

5 "Mover Objeto - Error"

6 "Copiar Objeto - Error"

7 "Objeto Papelera - Error"

8 "No tiene permisos para echar el objeto \n\n\
    %s\n\n\
en la papelera.\n\n\
Utilice la opción Cambiar permisos del menú emergente\n\
del objeto o del menú Seleccionada para activar el\n\
permiso de Lectura del objeto. \n\n\n\
Nota: Si este objeto es una carpeta, deberá tener también \n\
permiso de Lectura para cada uno de los objetos de la \n\
carpeta antes de echar la carpeta en la papelera. "

9 No se puede abrir %s

$set 5        Redefine set#

$ ****** Module: overwrtdialog.c ******   

1 "Un objeto de la carpeta destino tiene el mismo\n\
nombre, %s, que un objeto de la carpeta origen."

2 "Sobregrabar Objeto"

3 "Sobregrabar objeto existente"

4 "Cambiar el nombre de un objeto existente a:"

5 "Cambiar el nombre de un nuevo objeto a:"

6 "Saltar este objeto"

7 "¿Confirmar futuros conflictos de nombre?"

8 "Acción a emprender:"

9 "Copiar carpeta - Sobregrabar Archivo"

10 "%s\n\
ya existe: escoja otro nombre. "

11 "Sobregrabar Archivo - Error"

12 "%s\n\
no se puede renombrar como \n\
%s\n\
%s"

$set 6        Redefine set#

$ ****** Module: utils.c ******   
$ ** Button labels.   

1 "Bien"

2 "Pausar"

3 "Cancelar"

4 "Ayuda"

5 "Ayuda para Copiar Directorio"

6 "Continuar"

7 "Cancelar Copiar"

8 "Cancelar Mover"
