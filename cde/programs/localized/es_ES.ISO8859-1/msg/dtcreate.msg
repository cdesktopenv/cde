$ $TOG: dtcreate.msg /main/4 1999/09/17 17:23:07 mgreess $
$ %Z%%M%        %I%  %W% %G% %U%   
$    
$   COMPONENT_NAME: dtcreate   
$    
$   FUNCTIONS: none   
$    
$   ORIGINS: 27   
$    
$    
$   (C) COPYRIGHT International Business Machines Corp. 1993   
$   All Rights Reserved   
$   Licensed Materials - Property of IBM   
$   US Government Users Restricted Rights - Use, duplication or   
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.   
$    
$ **   Description:   
$ **   ------------   
$ **   This is the source message catalog file for dtcreate.   
$quote "

$set 2
$ *****************************************************************************   
$ **   
$ ** Message set for menu options on main window   
$ **   
$ *****************************************************************************   

10 "Archivo"

11 "A"

15 "Abrir..."

16 "b"

20 "Nuevo"

21 "N"

25 "Guardar"

26 "G"

30 "Salir"

31 "S"

35 "Opciones"

36 "O"

40 "Mostrar Funciones Avanzadas"

41 "A"

42 "Vista de Icono"

43 "I"

44 "Color"

46 "C"

47 "Monocromo"

48 "M"

49 "Mostrar Iconos Monocromos"

50 "M"

53 "Ayuda"

54 "A"

60 "Información General"

61 "G"

65 "Tabla de Contenido"

66 "C"

70 "Tareas"

71 "T"

75 "Referencia"

76 "R"

80 "Uso de la Ayuda"

81 "U"

85 "Acerca de Crear Acción"

86 "A"

$set 3        Redefine set#

$ *****************************************************************************   
$ **   
$ ** Message set for  main Create Action window   
$ **   
$ *****************************************************************************   

10 "Crear Acción"

12 "Nombre de Acción (Etiqueta del Icono):"

13 "Iconos de Acción:"

20 "Mandato Cuando Se Pulsa Dos Veces el Icono de Acción:"

21 "Mandato A Ejecutar Cuando Se Pulsa Dos veces:"

22 "Mandato Cuando Acción Está Abierta (Pulsada-Dos veces):"

30 "Texto de Ayuda para Icono de Acción:"

32 "Tipo de Ventana:"

34 "Gráfica (X-Window)"

36 "Terminal (Cierre Automático)"

38 "Terminal (Cierre Manual)"

40 "Sin Salida"

50 "Avanzada"

52 "Básica"

$set 4        Redefine set#

$ *****************************************************************************   
$ **   
$ ** Message set for  main Create Action window advanced function area   
$ **   
$ *****************************************************************************   

10 "Opcional"

11 "Cuando Se Abra la Acción, Solicitar a los Usuarios:"

12 "Tipos de Datos Que Usan Esta Acción:"

13 "Añadir..."

14 "Suprimir"

15 "Editar..."

16 "Tipos de Datos que se pueden Soltar:"

17 "Todos los Tipos de Datos"

18 "Sólo la Lista Anterior"

$set 5        Redefine set#

$ *****************************************************************************   
$ **   
$ ** Error and Information message set for  main Create Action window   
$ **   
$ *****************************************************************************   

10 "Falta el Nombre de Acción.\n\
Entre un nombre en el campo 'Nombre de Acción'."
$    

15 "Falta el mandato de ejecutar.\n\
Por favor, entre un mandato en el campo\n\
'Mandato Cuando Acción Está Abierta (Pulsada-Dos veces)'."
$    

20 "La lista 'Tipos de Datos que Usan esta Acción' está vacía.\n\
Ha seleccionado 'Sólo la Lista Anterior' en el campo 'Tipos de \n\
Datos que se pueden Soltar'.\n\
Cambie la opción Tipos de Datos que se pueden Soltar por 'Todos los Tipos de Datos'\n\
o añada como mínimo un tipo de datos a la lista."
$    

25 "Ya existe una acción y un archivo de definiciones de tipos de \n\
datos con este Nombre de Acción.\n\
Puede cancelar la operación de Guardar y cambiar el Nombre de Acción\n\
o puede sobregrabar el archivo de definiciones existente.\n\
¿Desea sobregrabar el archivo de definiciones existente?"
$    

30 "Falta un nombre de variable en el campo\n\
'Mandato Cuando Acción Está Abierta (Pulsada-Dos veces)'.\n\
Añada un nombre de variable para la etiqueta que ha entrado\n\
en el campo 'Cuando se Abre la Acción, Solicitar a los Usuarios'.\n\
Los nombres de variable válidos son $*, $1, $2, ..., $9."
$    

35 "Estos cambios no se han guardado.\n\
¿Desea guardar estos cambios antes de Salir?"
$    

40 "Los cambios actuales no se han guardado.\n\
¿Desea guardar esta acción antes de borrar\n\
los campos para una acción Nueva?"
$    

45 "Los cambios actuales no se han guardado.\n\
¿Desea guardar estos cambios antes de borrar\n\
los campos para cargar otro archivo de definiciones de acción?"
$    

50 "El Nombre de Acción contiene uno o más caracteres\n\
no permitidos.\n\
Los caracteres siguientes no se deben utilizar en un nombre:\n\
  / \\ \" ' ;  (  ) ~ ! $ { } < > # @ | &"
$    

55 "No se pudo crear el archivo siguiente:"
$    

60 "No se pudo abrir el archivo siguiente:"
$    

65 "No se pudo cargar el archivo siguiente porque no se había\n\
creado originalmente por la herramienta de Crear Acción:"
$    

66 "No se pudo cargar el archivo porque se había\n\
alterado por algún otro medio distinto de Crear Acción.\n\
Esto puede ser debido a una modificación del archivo\n\
con un editor de texto o con alguna otra herramienta."
$    

67 "Crear Acción sólo puede editar archivos que se hayan creado\n\
y editado mediante Crear Acción."
$    

68 "No se pudo cargar el archivo siguiente:"
$    
$  The following messages are all part of the same message dialog   

70 "La nueva acción"

71 "se ha puesto en la carpeta de inicio del usuario."

73 "Se llama al archivo de definiciones de acciones que se ha creado:"
$    

80 "No se pudieron encontrar todos los tamaños del icono\n\
que ha seleccionado."
$    

85 "No se pudo iniciar el Editor de iconos."
$    

90 "No se pudo analizar el archivo siguiente:"
$    

95 "No se pudo encontrar el archivo de iconos siguiente dentro de la \n\
ruta de búsqueda de iconos."
$    

98 "El icono que ha seleccionado no está en la ruta de búsqueda de iconos.\n\
Debe mover el icono a un directorio de la ruta de búsqueda de\n\
iconos de modo que el icono se visualice correctamente dentro del\n\
entorno del escritorio.\n\
Seleccione 'Ok' si desea utilizar este icono.\n\
Seleccione 'Cancelar' si desea seleccionar un icono distinto."
$    
$ ONLY TRANSLATE  "Usage"  and  "filename"   

110 "Uso:   dtcreate [<nombarchivo>] [-?]"
$    

120 "No ha seleccionado un Tipo de Datos de la lista\n\
'Tipos de Datos Que Usan Esta Acción'."

125 "Seleccione el Tipo de Datos que desea Suprimir."

130 "Seleccione el Tipo de Datos que desea Editar."
$    

150 "El trabajo actual no se ha guardado."

152 "No guardar"

154 "Cancelar Cerrar"
$    

160 "No se ha podido guardar la sesión en el archivo:"

162 "No se ha podido restaurar la sesión desde el archivo:"
$    
$ TRANSLATORS: DO NOT TRANSLATE the word "HOME" in the following message   

170 "No está establecida la variable de entorno HOME.\n\
Se debe establecer la variable de entorno HOME para\n\
que se guarde esta acción."

$set 6        Redefine set#

$ *****************************************************************************   
$ **   
$ ** General message set for dialogs   
$ **   
$ *****************************************************************************   

10 "Ok"

11 "Aplicar"

12 "Cancelar"

13 "Ayuda"

14 "Borrar"

18 "Buscar Conjunto"

19 "Buscar Conjunto..."

20 "Editar Icono..."

31 "Crear Acción - Error"

41 "Crear Acción - Confirmación"

46 "Crear Acción - Información"

51 "Crear Acción - Pregunta"

$set 7        Redefine set#

$ *****************************************************************************   
$ **   
$ ** Message set for Add Datatype and Edit Datatype dialogs   
$ **   
$ *****************************************************************************   

10 "Añadir Tipo de Datos"

11 "Editar Tipo de Datos"

14 "Nombre de la Familia de Tipos de Datos:"

15 "Características de Identificación:"

16 "Editar..."

17 "Texto de Ayuda para este Icono de Tipo de Datos:"

18 "Iconos de Tipo de Datos:"

21 "Mandatos"

22 "Mandato para Abrir este Tipo de Datos:"

23 "Mandato para Imprimir este Tipo de Datos:"

30 "Abrir"

31 "Imprimir"

$set 8        Redefine set#

$ *****************************************************************************   
$ **   
$ ** Error and Information message set for   
$ **    Add Datatype and Edit Datatype dialogs   
$ **   
$ *****************************************************************************   

10 "Falta el Nombre de la Familia de Tipos de Datos.\n\
Entre un nombre en el campo 'Nombre de la Familia de Tipos de Datos'."
$    

15 "Faltan las Características de Identificación.\n\
Debe especificar las características antes de poder crear\n\
el tipo de datos. Seleccione el botón Editar junto a la lista de\n\
Características de Identificación para especificar las características."
$    

20 "El campo 'Nombre de la Familia de Tipos de Datos' contiene uno o más\n\
caracteres no permitidos.\n\
Los caracteres siguientes no se deben utilizar en un nombre:\n\
  / \\ \" ' ;  (  ) ~ ! $ { } < > # @ | &"
$    

25 "Ya existe una Familia de Tipos de Datos con este nombre.\n\
Cambie el nombre en el campo 'Nombre de la Familia de Tipos\n\
de Datos'."

$set 9        Redefine set#

$ *****************************************************************************   
$ **   
$ ** Message set for  Identifying Characteristics dialog   
$ **   
$ *****************************************************************************   

10 "Características de Identificación"

11 "Incluir Todos"

12 "Archivos"

13 "Carpetas"

14 "Que Coincidan Con Lo Siguiente:"

20 "Patrón de Nombre:"

21 "Patrón de Permiso:"

22 "Legible"

24 "Escribible"

25 "Ejecutable"

27 "AND"

28 "Contenido:"

29 "Patrón:"

30 "Tipo:"

31 "Serie"

32 "Byte (Hex)"

33 "Corto (Decimal)"

34 "Largo (Decimal)"

35 "Byte de Inicio:"

40 "Lectura:"

41 "Escritura:"

42 "Ejecución:"

43 "Activar"

44 "Desactivar"

45 "Ambos"

$set 10        Redefine set#

$ *****************************************************************************   
$ **   
$ ** Error and Information message set for Identifying Characteristics dialog   
$ **   
$ *****************************************************************************   

10 "Falta el Patrón de Nombre.\n\
Si se selecciona Patrón de Nombre, se debe entrar\n\
un patrón de nombre."
$    

15 "Falta la opción de permisos.\n\
Si se selecciona Patrón de Permiso, se deben \n\
seleccionar una o más de las opciones que se\n\
encuentran bajo Patrón de Permiso."
$    

20 "Falta Patrón de Contenido.\n\
Si se selecciona Contenido, se debe cumplimentar \n\
el campo Patrón que se encuentra bajo Contenido."
$    

25 "Falta el valor del byte de inicio.\n\
Si se selecciona Contenido, se debe entrar \n\
un número en el campo 'Byte de Inicio'."
$    

30 "No se ha seleccionado ninguna característica de identificación.\n\
Seleccione una o más de las características (Patrón\n\
de Nombre, Patrón de Permiso o Contenido)."
$    

45 "El Patrón de Nombre contiene uno o más caracteres\n\
no permitidos.\n\
Los caracteres siguientes no se deben utilizar en\n\
un Patrón de Nombre:\n\
  / \\ \" ' ;  (  ) ~ ! $ { } < > # @ | &"
$    

46 "El Patrón de Contenido no coincide con el formato para \n\
el Tipo de contenido que ha seleccionado.\n\
Vuelva a escribir el Patrón de Contenido con formato correcto o\n\
cambie el Tipo para que coincida con el Patrón de Contenido."
$    

47 "El campo Byte de Inicio contiene caracteres no permitidos.\n\
Este campo sólo puede aceptar números en formato decimal."
$    

48 "La utilización del Patrón de Contenido puede reducir\n\
significativamente el rendimiento del Gestor de archivos.\n\
Cuando se utiliza el Patrón de Contenido, el Gestor de Archivos\n\
debe buscar el contenido de todos los archivos de una carpeta \n\
antes de poder abrir dicha carpeta. Esto significa que las \n\
carpetas se abrirán con más lentitud.\n\
Por consiguiente, es aconsejable utilizar únicamente \n\
patrones de nombres y de permisos."

$set 11        Redefine set#

$ *****************************************************************************   
$ **   
$ ** Message set for  Icon Selection Dialog   
$ **   
$ *****************************************************************************   

10 "Directorios"

11 "Archivos de Iconos"

12 "Entrar Nombre Archivo Iconos"

20 "Cargando iconos.  Espere, por favor."

30 "Carpetas Icono"

50 "El Archivo de Iconos Seleccionado no es válido.\n\
Entre un nombre de archivo de iconos válido en \n\
el campo 'Entrar Nombre Archivo Iconos'."
$ ONLY TRANSLATE  "Empty".   Do not translate the brackets "[" and "]".   

60 "[Vacío]"

$set 12        Redefine set#

$ *****************************************************************************   
$ **   
$ ** Message set for  Open File Dialog   
$ **   
$ *****************************************************************************   

10 "Filtro"

11 "Directorios"

12 "Archivos"

13 "Selección"

20 "Abrir Archivo"

25 "Crear Acción - Abrir"

$set 13
$ *****************************************************************************
$ **
$ ** Message set for  DieFromToolTalkError Dialog
$ **
$ *****************************************************************************
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Create Action - Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
$ "-NEW"
3 OK
