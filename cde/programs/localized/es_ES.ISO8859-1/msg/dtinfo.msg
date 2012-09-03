
$quote "
$ $TOG: dtinfo.msg /main/24 1999/09/20 13:34:56 mgreess $
$ 
$ (c) Copyright 1996 Digital Equipment Corporation.
$ (c) Copyright 1996 Hewlett-Packard Company.
$ (c) Copyright 1996 International Business Machines Corp.
$ (c) Copyright 1996 Sun Microsystems, Inc.
$ (c) Copyright 1996 Novell, Inc.
$ (c) Copyright 1996 FUJITSU LIMITED.
$ (c) Copyright 1996 Hitachi.
$ 
$ ******************************************************************************
$ 

$set	1
$ 
$ messages specific to message-catalog manager
$ 
1 "No se encontró el mensaje predeterminado en el catálogo."


$set	2
$ 
$ messages specific to UAS/DtSearch modules
$ 
1 "La función de búsqueda ``%s'' no está soportada."
2 "frase entre comillas"
3 "peso"
4 "proximidad"
$       5       "xor" == Exclusive OR (Boolean)
5 "xor"


$set	4
$ 
$ messages specific to booklist window
$ 
3 "Dtinfo: Lista de libros de la biblioteca de información"
63 "Bienvenido a Dtinfo!"
65 "Sesión actual"


$set	5
$ 
$ messages specific to browser window
$ 
1 "Dtinfo: Examinador"
2 "Para ver el destino de un enlace, haga clic y mantenga el botón del ratón presionado en dicho enlace."
3 "Biblioteca de información"

$       4       "The locator for this section is %s"
$       This message is the result of the "Show Locator" operation.
4 "Localizador de la sección: %s"

$ line-break rules (reserved)
5 ""
6 ""

7 "El texto seleccionado ha sido insertado dinámicamente y no puede ser utilizado\n\
por si solo para crear una referencia de marcador de libros.\n\
Tiene la opción de seleccionar otro texto cerca del punto que desea marcar o de incluir\n\
más texto adyacente."

$set	6
$ 
$ messages specific to add information library dialog
$ Note: "Infolib" is a CDE glossary term.
$ 

1 "Dtinfo: Añadir biblioteca de información"

$ 2     "Please enter an absolute path to the Infolib."
$       (Path must exist and include "/".)
2 "Por favor introduzca una ruta de acceso absoluta hacia la infobiblioteca."

$ 3     "Infolib open failed. Check the path specified."
$	(Path passed basic format check, but could not open target found.)
3 "Falló la operación para abrir la infobiblioteca. Verifique la ruta especificada."

$ 4     "Infolib open failed. Check entry syntax or Infolib integrity."
$       (An unidentified error occurred on attempt to open.)
4 "Falló la operación para abrir la infobiblioteca. Verifique la sintaxis de la anotación o la integridad de la infobiblioteca."

$ 5     "Infolib specification format error."
5 "Error de formato de especificación de la infobiblioteca."

$ 6     "No Infolib specified."
6 "No se ha especificado ninguna infobiblioteca."

$ 7     "No Infolib found or specified."
7 "No se ha encontrado ni especificado ninguna infobiblioteca."

$ 10    "Dtinfo: Infolib Selection"
$       (For title to the File Selection Dialog)
10 "Dtinfo: Selección de biblioteca de información"

$ 11    "No Infolibs Present"
$       (This string will be shown in a directory listing where there
$        are no Infolibs found in the directory.)
11 "No hay bibliotecas de información"


$set	7
$ 
$ dtinfo GUI label strings
$ 
1 "Archivo"
2 "A"
4 "Ver"
5 "V"

$	6	"Clone"
$ "Clone" is applied to the selected entry in the booklist, and
$ has the effect of opening another (secondary) window with a
$ copy of the booklist hierarchy below the selection, and rooted
$ (based) at the selected item.
$ [Currently, this functionality is deferred beyond CDE 2.0.]
6 "Duplicado"
$	7	"N" for "N" in "Clone"
7 "D"

8 "Imprimir"
9 "P"
10 "Imprimir..."
11 "r"
12 "Cerrar"
13 "C"
14 "Salir"
15 "S"
16 "Editar"
17 "E"
18 "Copiar"
19 "C"
20 "Añadir..."
21 "A"
22 "Quitar"
23 "r"
24 "Opciones"
25 "O"
26 "Preferencias..."
27 "P"
28 "Seguimiento automático"
29 "A"
30 "Mostrar barra de herramientas"
31 "t"
32 "Mostrar área de búsqueda"
33 "s"
34 "Ventanas"
35 "V"
36 "Mapa gráfico"
37 "g"
38 "Historial de secciones"
39 "H"
40 "Lista de marcas"
41 "m"

$	42	"Search History " presents a dialog listing
$ a history of prior search results, as provided in the Search
$ Results Dialog.
42 "Historial de búsqueda"
43 "S"
44 "Editor de consultas"
45 "E"
46 "Editor de amplitud de búsqueda"
47 "d"
48 "Ayuda"
49 "u"
50 "Sobre el tema..."
51 "o"
59 "Buscar:"
61 "Editor de amplitud de búsqueda"

$  Messages 62-77 for "Browser" pulldown menu entries
$ 
62 "Examinador"
63 "x"
64 "Nueva ventana"
65 "N"
66 "Abrir localizador..."
67 "o"
68 "Mostrar localizador"
69 "z"
70 "Imprimir"
71 "p"
72 "Imprimir..."
73 "r"
74 "Cerrar"
75 "C"
76 "Salir"
77 "S"

$	78	"On Selection", where selection is a text
$ selection that will be used directly as a search query string
78 "Selección"
79 "l"

80 "Editor de consultas"
81 "E"
82 "Editor de amplitud de búsqueda"
83 "s"
84 "Historial de búsqueda"
85 "H"
86 "Borrar elementos encontrados"
87 "c"
88 "Atrás (historial)"
89 "A"
90 "Adelante (historial)"
91 "d"
92 "Historial..."
93 "H"
94 "Sección anterior"
95 "t"
96 "Próxima sección"
97 "n"

$   Messages 98-101 are for traversal of the document view in order
$ to present and highlight the Previous or Next "search hit" (if any)
$ in the section. Presumably, all search hits have already been
$ highlighted to some degree, and this will add the equivalent of a
$ keyboard traversal highlight to the appropriate one when brought
$ into view.
98 "Búsqueda anterior"
99 "r"
100 "Búsqueda siguiente"
101 "g"

102 "Buscar"
103 "s"

$	104	"Go" is a pulldown menu label, where menu items are
$ various methods by which the user may "go" to a specific document.
104 "Ir"
105 "I"

$   Message 106-119 are for the "Marks" pulldown menu
106 "Marcas"
107 "M"
108 "Crear marcador de libro"
109 "b"
110 "Crear nota..."
111 "N"

$	112	"List Marks" means to show all saved Marks as a list
$ in a separate dialog window.
112 "Listar marcas"
113 "L"

114 "Editar selección..."
115 "E"
116 "Mover selección"
117 "M"
118 "Eliminar selección"
119 "l"

$   Messages 120-129 are for "Options" pulldown menu entries
120 "Opciones"
121 "O"
122 "Preferencias..."
123 "P"

$	124	"Detach Graphic" removes a selected graphic from the
$ document flow and places it in its own window. A "placeholder"
$ remains in the document where the graphic originally appeared.
124 "Desvincular gráfico"
125 "D"

126 "Mostrar barra de herramientas"
127 "t"
128 "Mostrar área búsqueda"
129 "s"

$   Messages 130-135 are for "Windows" pulldown menu entries
130 "Ventanas"
131 "V"
132 "Lista de libros"
133 "s"
134 "Mapa gráfico"
135 "g"

$   Messages 136-142	are believed to be *unused*/obsolete
136 "Autor"
137 "A"
138 "Obtener localizador"
139 "Depurar"
140 "D"
141 "Generar lista de base de información ficticia"
142 "Procesador de comandos"

$   Messages 143-158 are Help pulldown menu entries, and
$ should of course read the same across all CDE applications,
$ within a localization. They always mean the same thing:
$ provide "help On []"
143 "Ayuda"
144 "u"
145 "Sobre el tema..."
146 "S"
147 "Sobre la ventana"
148 "v"
149 "Sobre la aplicación"
150 "a"
151 "Indice"
152 "I"
153 "Sobre la ayuda"
154 "u"
155 "Sobre teclas"
156 "t"
157 "Sobre la versión"
158 "v"

$	159	"Retain" is believed to be unused
159 "Retener"

160 "Ruta de infobiblioteca:"
161 "Ok"
162 "Cancelar"
163 "Nombre"
164 "Notas"
165 "Mostrar"

$	166	"Panner" is a graphical user interface control for
$ selecting that part of a larger area which the associated view
$ window is to display.
166 "Panorámica"

167 "Vincular"
168 "Mostrar sección"

$   Messages 169-182 are for the window which is used to present a
$ detached graphic.
$	169	Fit >graphic< to window
169 "Ajustar a la ventana"
$	170	Fit >window< to graphic
170 "Ajustar al gráfico"
171 "25 %"
172 "50 %"
173 "100 %"
174 "125 %"
175 "150 %"
176 "200 %"
$	177	"Custom" value entry for re-scaling of graphic
177 "Personalizado..."

$   Messages 178-182 refer to where the panner is to be placed within
$ the detached graphic dialog window. Is a "panner" masculine or
$ feminine ? "Beats" me ;-).
178 "Ninguno"
179 "Superior izquierdo"
180 "Superior derecho"
181 "Inferior izquierdo"
182 "Inferior derecho"

183 "Suprimir"
184 "Libro"
185 "Sección"
186 "Preferencias para"
187 "Examinador"
188 "Mapa"
189 "Búsqueda"
190 "Geometría del examinador"
191 ". . ."
192 "Escala de font"

$	193	"Pin []" is a verb
193 "Sujetar ventana"
194 "Archivar un problema"
195 "Geometría de mapa"
196 "Actualización automática"
197 "Tamaño de historial de secciones"
198 "Tamaño de historial de búsqueda"
199 "Máximo elementos encontrados"
200 "Mostrar auto. primer elemento encontrado"
201 "Aplicar"
202 "Restablecer"
203 "Activado"
204 "Desactivado"
205 "Cantidad de secciones"
206 "Seleccionadas:"
207 "A imprimir:"
208 "Error."
209 "A imprimir"
210 "Secciones"
211 "Jerarquía"
212 "Nombre de la amplitud"
213 "Sin nombre"
214 "Libros"
215 "Componentes"
216 "Nueva"
217 "Guardar"
218 "Renombrar"
219 "Recuperados:"
220 "Amplitud:"
221 "Consulta:"
222 "Introducir un localizador único:"
223 "Borrar"
224 "Cortar"
225 "Pegar"
226 "Agrupar"
227 "Desagrupar"
228 "Deshacer"
229 "Rehacer"
230 "Consulta"
231 "Borrar todo"

$   Messages 232-242 relate to operators available in a query
$ expression, and appear in the Query Editor on option menu buttons.

$       232 "Matches" means the entry in the adjacent entry field will
$           be compared exactly to find a search index database entry.
232 "Coincide"
233 "No coincide"
234 "Comienza con"
235 "No comienza con"
$	236	Boolean operator OR
236 "O"
$	237	"Xor" is for Boolean logic "Exclusive OR"
237 "Xor"
$	238	Boolean operator AND
238 "Y"
239 "Cerca"
240 "Antes"
241 "Peso"
242 "Proximidad"
243 "Amplitud"
244 "Sí"
245 "No"
246 "Coincidencias"
247 "Introducción"
248 "I"
249 "Tareas"
250 "T"
251 "Referencia"
252 "R"
253 "Uso de la ayuda"
254 "U"
255 "Acerca de Gestor de información"
256 "A"
257 "Descripción general"
258 "D"
259 "o"
260 "Historial"
261 "Lista de historial"
262 "La sección ya no está disponible."
263 "Haga clic en un gráfico para desvincularlo."
264 "Levantar gráfico"
265 "Vincular gráfico"



$       270  "..." is a button label indicating a list from which an
$            entry may be selected to fill a preceeding text field.
$            In this case, the user may enter a path into a text field
$            or activate the button with "..." to be presented with
$            a File Selection Box. The object is to select an Infolib
$            path.
270 "..."



$set	8
$ 
$ dtinfo quick help messages (to be exhibited on status display area)
$ 
1 "Ir a la sección anterior de la historia"
2 "Ir a la próxima sección de la historia"
3 "Ir a la sección anterior del documento"
4 "Ir a la próxima sección del documento"

$ A search "match" is the same as a search "hit". "Match" is more
$ descriptive.
5 "Ir a la coincidencia anterior de la búsqueda"
6 "Ir a la próxima coincidencia de la búsqueda"

7 "Imprimir sección actual"
8 "Mostrar lista de libros"
9 "Mostrar mapa gráfico"
10 "Sujetar (liberar) para evitar (permitir) que sea utilizada para nuevos documentos."
11 "Especificar amplitud de búsqueda mediante el Editor de amplitud de búsqueda"
52 "Mostrar sección seleccionada"
53 "Mostrar sección seleccionada en una lista de libros individual"
54 "Imprimir sección seleccionada"
55 "Abrir una biblioteca de información adicional"
56 "Cerrar la biblioteca de información seleccionada"
57 "Efectuar consulta mediante el Editor de consultas"
58 "Mostrar marcadores de libros y lista de notas"
60 "Especificar amplitud de búsqueda"
62 "Especificar amplitud de búsqueda mediante el Editor de amplitud de búsqueda"
64 "Buscar %s"


$set	9
$ 
$ messages specific to bookmark edit dialog
$ 
1 "Dtinfo: Editor de marcador de libros"
2 "La actualización del marcador de libros ha fallado."


$set	10
$ 
$ messages specific to detached graphic window
$ 
1 "Dtinfo: Gráfico desvinculado"
2 "Gráfico desvinculado"
3 "Ilustración rasterizada (de escala fija)"
4 "Ilustración de vectores (de escala ajustable)"
5 "Ajustando la escala de la ilustración. Espere por favor..."
$ 
$ Dtinfo*SetZoom
$ 
6 "Introduzca el nuevo porcentaje de escala:"
$ 
$ Dtinfo*SetZoomTitle
$ 
7 "Dtinfo: Fijar escala"
$ 
8 "Dtinfo: "
9 "Ilustración rasterizada (de escala ajustable)"


$set	11
$ 
$ messages specific to mark list window
$ 

$	1 is the title of a window containing a list of the
$ user's bookmarks
1 "Dtinfo: Lista de marcas de usuario"


$set	12
$ 
$ messages specific to mark selection dialog
$ 
$   Messages 1 & 2: "Select" and "Edit" are verbs
1 "Dtinfo: Seleccionar marca"
2 "Dtinfo: Editar marca"


$set	13
$ 
$ messages specific to section history window
$ 
1 "Dtinfo: Historial de secciones"


$set	14
$ 
$ messages specific to preferences window
$ 
1 "Dtinfo: Preferencias"
2 "Dtinfo: Obtener geometría"


$set	15
$ 
$ messages specific to printing
$ 
1 "Dtinfo: Imprimir"
$          Messages 2 - 5 are obsolete, and have been replaced by 12
2 No se pudo encontrar ningún Administrador de diálogo de impresión en el entorno 
3 No se puede iniciar el Administrador de diálogo de impresión
4 El Administrador de diálogo de impresión no está autorizado para conectarse con la pantalla de video
5 El Administrador del diálogo de impresión no está autorizado a conectarse con la pantalla de impresión
6 No se puede poner en pantalla el Administrador del diálogo de impresión
7 No se pueden copiar datos de impresión predeterminados
8 Error al imprimir en el archivo
9 La tarea de impresión ya se está ejecutando
10 Dtinfo: Estado de la impresión
11 La tarea de impresión ha fallado.\n\nEl servidor de impresión X está temporalmente sin recursos.
$       Message 12 is used to handle errors from the PDM
12	"Error del Administrador de diálogo de impresión (PDM) - Falló la configuración."

$set	16
$ 
$ messages specofic to search scope editor window
$ 
$	1	Edit the Scope for for use in a Search
1 "Dtinfo: Editor de amplitud de búsqueda"
$ 
$ Dtinfo*SaveScope
$ 
2 "Introducir el nombre de la nueva amplitud:"
$ 
$ Dtinfo*SaveScopeTitle
$ 
3 "Dtinfo: Guardar amplitud"
$ 
4 "Todas las Infolibs"


$set	17
$ 
$ messages specific to search result window
$ 
$	1	"Search Results" is for the results of a search
1 "Dtinfo: Resultado de la búsqueda"


$set	18
$ 
$ messages specific to open locator dialog, and locator
$ input formats in general.
$ 
$ 1	"Dtinfo: Open Locator"
1 "Dtinfo: Abrir localizador"

$ 2     "Locator specification format error."
2 "Error de formato de especificación de localizador."

$ 3     "Locator resolution failed."
3 "Falló la resolución del localizador."

$ 4     "Locator resolution failed within the loaded infolibs."
4 "La resolución del localizador ha fallado en las infolibs cargadas."

$ 5     "Document not found."
5 "No se encontró el documento."

$ 6     "No locator value provided."
6 "No se suministró ningún valor de localizador."


$set	19
$ 
$ messages specific to query editor window
$ 
1 "Dtinfo: Editor de consultas"


$set	20
$ 
$ dtinfo general messages
$ 
$ 
$ Dtinfo*PixmapAllocationError
$ 
1 "No hay suficiente memoria de servidor\n\
disponible para poner el gráfico en pantalla."
$ 
$ Dtinfo*AbandonChanges
$ 
2 "¿Desea abandonar las modificaciones?"
$ 
$ Dtinfo*NoHelp
$ 
3 "No se pudo encontrar información de ayuda."
$ 
$ Dtinfo*NoSelection
$ 
4 "No hay texto seleccionado.\n\
Seleccione texto e intente nuevamente."
$ 
$ Dtinfo*AbandonBookmarks
$ 
5 "Hay marcas de libro abiertas no guardadas.\n\
¿Desea salir sin guardarlas?"
$ 
$ Dtinfo*ConfirmQuit
$ 
6 "¿Desea salir de Dtinfo?"
$ 
$ Dtinfo*BadTocHierarchy
$ 
7 "Archivar un problema"
$ 
$ Dtinfo*LinkToBookTitle
$ 
8 "Enlazar con %s: %s"
$ 
$ Dtinfo*LinkToTitle
$ 
9 "Enlazar con %s"
$ 
$ Dtinfo*MarkCreateFailed
$ 
10 "Dtinfo no puede crear esta marca de libro."
$ 
$ Dtinfo*MarkDeleteFailed
$ 
11 "Dtinfo no puede eliminar esta marca de libro."
$ 
$ Dtinfo*MarkMoveFailed
$ 
12 "Dtinfo no puede quitar esta marca de libro."
$ 
$ Dtinfo*MaxDepthExceeded
$ 
13 "Archivar un problema"
$ 
$ Dtinfo*MarkIconInvisible
$ 
14 "La marca de libro seleccionada no puede ponerse en pantalla.\n\
Se encuentra fuera del área visible de la sección."
$ 
$ Dtinfo*MViewNoBase
$ 
15 "La marca seleccionada no se encuentra\n\
en un documento al que se puede obtener acceso.\n\
Cargue la Infolib correspondiente."
$ 
$ Dtinfo*GetGeometry
$ 
16 "Haga clic dentro de una ventana Dtinfo cuyo\n\
tamaño es el que se desea guardar como predeterminado."
$ 
$ Dtinfo*SaveModifiedScope
$ 
17 "¿Desea guardar los cambios de la amplitud actual?"
$ 
$ Dtinfo*BadScopeName
$ 
18 "No se pueden utilizar los caracteres dos puntos (:), coma (,),\n\
y punto y coma (\;)\n\
en nombres de amplitud de búsqueda."
$ 
$ Dtinfo*EmptyScopeName
$ 
19 "Por favor introduzca un nombre de amplitud válido."
$ 
$ Dtinfo*ScopeNameTooLong
$ 
20 "Los nombres de amplitud de búsqueda no deben exceder 30 caracteres.\n\
Por favor introduzca un nombre de amplitud más corto."
$ 
$ Dtinfo*DuplicateScopeName
$ 
21 "El nombre que ha introducido ya está en uso.\n\
Por favor introduzca un nombre de amplitud único."
$ 
$ Dtinfo*Everything
$	22	"Everything" refers to section components like
$		titles, body, and related.
$ 
22 "Todo"
$ 
$ Dtinfo*Titles
$ 
23 "Títulos"
$ 
$ Dtinfo*Body
$	24	"Body" as in the body of text.
$ 
24 "Cuerpo"
$ 
$ Dtinfo*Examples
$ 
25 "Ejemplos"
$ 
$ Dtinfo*Index
$ 
26 "Indice"
$ 
$ Dtinfo*Tables
$ 
27 "Tablas"
$ 
$ Dtinfo*Graphics
$ 
28 "Gráficos"
$ 
$ Dtinfo*ScopeDeleteConfirm
$ 
29 "¿Desea eliminar la amplitud que aparece en pantalla?"
$ 
$ Dtinfo*CurrentSection
$ 
30 "Sección actual"
$ 
$ Dtinfo*IgnoredScopes
$ 
31 "Se ignoran las amplitudes guardadas\n\
no válidas para estas Infolibs."
$ 
$ Dtinfo*MarkBaseOpenFailed
$ 
32 "No se puede abrir la base de datos de marcas del usuario."
$ 
$ Dtinfo*NoOpenMarkBases
$ 
33 "No se encontró la base de datos de marcas Dtinfo.\n\
Dtinfo no puede crear marcas de libro."
$ 
$ Dtinfo*MarkBaseReadOnly
$ 
34 "La base de datos de marcas actual cuenta solamente con permisos de sólo lectura."
$ 
35 "Error desconocido en el documento: Por favor notifique a personal de respaldo del producto."

36 "El motor de búsqueda ha enviado un mensaje\n\
inesperado (en UAS_SearchEngine):\n\
Por favor notifique al personal de respaldo del producto."
$ 
$ Dtinfo*TooManyIbases
$ 
37 "Debido a la limitación del motor de búsqueda, sólo se podrá buscar \n\
en los primeros estantes de libros %d de la amplitud actual.\n\
¿Desea continuar?"
$ 
$ Dtinfo*DemoFailure
$ 
38 "Esta es una versión de evaluación del examinador.\n\
No cuenta con permiso para obtener acceso a esta Infolib."
$ 
$ Dtinfo*StyleSheetSyntaxError
$ 
39 "La hoja de estilo de este documento contiene un error.\n\
Se ha aplicado valores predeterminados."
$ 
$ Dtinfo*NoHits
$ 
40 "No se encontraron coincidencias para esta búsqueda."
$ 
$ Dtinfo*BadQuery
$ 
41 "Dtinfo no entiende la consulta. \n\
Por favor introduzca una consulta válida."
$ 
$ Dtinfo*SearchError
$ 
42 "Se ha detectado un error interno de motor de búsqueda. Todas\n\
las búsquedas subsiguientes de esta sesión de examinador fallarán.\n\n\
Este problema se ha registrado y se corregirá en una versión\n\
posterior del examinador. Como solución temporal, intente evitar\n\
introducir consultas que producen el error en cuestión.\n\
Para reactivar la búsqueda, salga y vuelva a iniciar el examinador."
$ 
$ Dtinfo*KernelError
$ 
43 "Se ha detectado un error interno del motor de búsqueda.\n\
Es probable que el directorio /usr/tmp de esta computadora\n\
se encuentre en (o esté enlazado con) un sistema de archivos tmpfs. Fallarán\n\
todas las búsquedas subsiguientes de esta sesión de examinador.\n\n\
Este problema es conocido y se ha registrado. La solución temporal consiste en reconfigurar el directorio /usr/tmp de esta computadora\n\
de manera que no se encuentre en (o esté enlazada con) un sistema de archivos tmpfs.\n\
Después de efectuar la operación mencionada anteriormente y de reactivar el examinador, se deberá\n\
haber reactivado la búsqueda que deberá funcionar correctamente. Para obtener información adicional\n\
refiérase al administrador del sistema."
$ 
$ Dtinfo*FulcrumNoMemory
$ 
44 "El motor de búsqueda no pudo completar la solicitud de búsqueda\n\
debido a que no pudo asignar memoria. Antes de intentar\n\
buscar nuevamente, intente cerrar otras aplicaciones o aumente la memoria\n\
física o virtual del sistema."
$ 
$ Dtinfo*FulcrumError
$ 
45 "Se produjo un error del motor de búsqueda. \n\
Por favor informe el error de búsqueda %d al personal de respaldo del producto."
$ 
$ Dtinfo*BadHelp
$ 
46 "Este botón enlaza con un documento Dtinfo que no se encuentra\n\
en las Infolibs cargadas actualmente."
$ 
$ Dtinfo*query_is_read_only
$ 
47 "Este es un campo para mostrar solamente.  Para modificar la búsqueda\n\
utilice el Editor de consultas o el campo de búsqueda rápida de\n\
las ventanas de lectura o lista de libros."
$ 
$ Dtinfo*ExceedRdWindows
$ 
48 "Ya se ha llegado a la cantidad máxima de ventanas de lectura activas (%d)\n\
de acuerdo con lo configurado bajo Preferencias del usuario."
$ 
$ Dtinfo*NoPrefBase
$ 
49 "No hay ninguna base de datos de preferencias del usuario disponible."
$ 
$ Dtinfo*PrefBaseReadOnly
$ 
50 "La base de datos de preferencias del usuario se ha abierto en\n\
modo de sólo lectura. No se pueden guardar nuevas preferencias en el disco."
$ 
$ Dtinfo*DeadEndLink
$ 
51 "El documento o sección solicitado no está disponible."
$ 
$ Dtinfo*MarkPositionFailed
$ 
52 "Dtinfo no puede mostrar la marca llamada\n\
\42%s\42\n\
debido a que el párrafo con el cual estaba asociada\n\
ya no existe."
$ 
$ Dtinfo*ScopeChangeAbandon
$ 
53 "¿Desea abandonar los cambios de la amplitud actual?"
$ 
$ Dtinfo*InformationLibrary
$ 
63 "Biblioteca de información"
$ 
$ Dtinfo*On
$ 
64 "Activada"
$ 
$ Dtinfo*Off
$ 
65 "Desactivada"
$ 
67 "No se pudo crear la Infolib."

68 "Ha fallado la creación de la Infolib."
$ 
69 "La operación para asignar peso al resultado de la búsqueda no está disponible."

$	70	The "qualified locator" is a "fully qualified
$		locator" as opposed to the simple format.
70 "El método de acceso en el localizador calificado (URL) no es válido."

71 "Ha fallado la creación del documento."

72 "Dtinfo: Desvincular gráfico"
73 "Haga clic en el gráfico para desvincularlo."


$set	21
$ 
$ messages specific to message dialogs
$ 

$ Dtinfo*ErrorDialogTitle
$ 
1 "Dtinfo: Error"

$ Dtinfo*WarningDialogTitle
$ 
2 "Dtinfo: Advertencia"

$ Dtinfo*InfoDialogTitle
$ 
3 "Dtinfo: Información"

$ Dtinfo*ProgressDialogTitle
$ 
$	4	"Progress" is a level of completion in regards
$		 to a task.
4 "Dtinfo: Progreso"

$ Dtinfo*QuitDialogTitle
$ 
5 "Dtinfo: Confirmar salida"

$ Dtinfo*QuestionDialogTitle
$ 
6 "Dtinfo: Pregunta"
$ 
7 "Dtinfo: Archivar un problema"


$set	22
$ 
$ dtinfo version information
$ 
$ 
$	Dtinfo*Version
$ 
1 "CDE Dtinfo Browser\n\
\n\
Copyright (c) 1996 Chisholm Technologies, Inc.,\n\
Austin, TX 78758, EE.UU.\n\
\n\
Copyright (c) 1994, 1995 HAL Computer Systems, Inc.\n\
\n\
Todos los derechos reservados.\n\
\n\
Derechos restringidos del gobierno de EE.UU.\n\
El uso, duplicación y divulgación por parte del gobierno están sujetos\n\
a las restricciones listadas en los subpárrafos del (a) al (d)\n\
de la cláusula Commercial Computer - Restricted Rights de FAR\n\
52.227-19 si corresponde, o en el subpárrafo (c)(1)(ii) \n\
de la cláusula Technical Data and Computer Software de\n\
DFARS 252.227-7013 y cláusulas similares del suplemento NASA FAR.\n\
Contratista y fabricante: HAL Computer\n\
Systems, 1315 Dell Avenue, Campbell, CA 95008 EE.UU."


$set	23
$ 
$ messages specific to CanvasRenderer (dtinfo I/F to DtCanvas/DtHelp)
$ 


$set	24
$ 
$ messages specific to UAS/Base modules
$ 
1 "El localizador (URL) no contiene datos de acceso."
2 "No se sabe cómo crear objetos %s:"

$ In the following, do not translate any part of "UAS_ObjList<T>::operator[]"
$ 
3 "Error de intervalo: UAS_ObjList<T>::operator[]"
4 "Error de intervalo: UAS_PtrList<T>::operator[]"


$set	27
$ 
$ messages specific to UAS/MMDB modules
$ 
1 "Los siguientes estantes de libros no son válidos:"
2 "Ruta de inforlib MMDB no válida: %s"
3 "Se ignora la ruta de inforlib MMDB no válida: %s"
4 "Biblioteca MMDB %d"
5 "Infolib MMDB: "
6 "Infolib %s"


$set	28
$ 
$ messages specific to graphical map window
$ 
1 "Dtinfo: Mapa local"
2 "Dtinfo: "


$set	29
$ 
$ messages specific to search history window
$ 
$	1	"Search Results History" is a history of the
$ search results already obtained under the dtinfo session, for
$ prior queries.
1 "Dtinfo: Historial de resultados de la búsqueda"


$set	30
$ 
$ messages specific to window system module
$ 
1 "Dtinfo: Error fatal"
2 "Deteniendo..."
3 No se puede abrir la pantalla.\n

$set	31
$ 
$ messages specific to printing
$ 
1 "Algún encabezamiento de página"
2 "Algún pie de página - página %d"


$set	40
$ 
$ messages specific to environmental interface or program context
$ 

$ 1     ""
1 "No implementado."
2 Argumento no válido
3 ERROR: La opción -sect tiene que ser especificada junto con la opción -print.
4 Uso: dtinfo...
5 Esta aplicación entiende todas las opciones de línea de comando estándar de X Toolkit.


$set	41
$ The messages in this set will only appear in the users
$   "errorlog" file.
$ 
$ ******************************************************************************
$ 
$ messages specific to tooltalk interface and operations.
$ Note that many errors defined by tooltalk are passed through
$ and therefore not repeated here.
$ 

$ 1     "Can't get tt session for X display %s"
1 No se puede obtener la sesión tt para la pantalla X:\n%s

$ 2     "Could not set default tooltalk session."
2 No se puede configurar la sesión predeterminada de tooltalk:\n%s

$ 3 Failed to open ToolTalk connection.
3 No se pudo abrir la conexión ToolTalk:\n%s

$ 4     "Could not declare ptype DtInfo."
4 No se pudo declarar ptype DtInfo:\n%s

$ Messages 5-10: These messages result from an error in a tt_* function
$    call.
$ 
5 El proceso ttsession(1) no se está ejecutando y el servicio ToolTalk\n\
no lo puede reactivar.
6 El identificador de proceso predeterminado está desactualizado o no es válido.
7 La sesión ToolTalk especificada está desactualizada o no es válida.

$ 
$ Message 8: Do not localize the word DtInfo.
$ 
8 El tipo de proceso 'DtInfo' no es el\n\
nombre de un tipo de proceso instalado.

$ 
$ Message 9: Do not localize the word DtInfoPrint.
$ 
9 El tipo de proceso 'DtInfoPrint' no es el nombre\n\
de un tipo de proceso instalado.

10 No se reconoce el número de error.

$set	42
$ 
$ messages specific to UAS/DtSR_SearchEngine modules
$ 
1 "DtSearch no soporta la consulta."
