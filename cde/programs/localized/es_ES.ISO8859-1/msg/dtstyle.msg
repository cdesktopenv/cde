$ "$XConsortium: dtstyle.msg /main/9 1996/10/19 14:46:52 rws $"    
$ *************************************<+>*************************************    
$ *****************************************************************************    
$ **    
$ **  File:        dtstyle.msg    
$ **    
$ **  Project:     DT Style Manager    
$ **    
$ **  Description:    
$ **  -----------    
$ **  This file is the source for the message catalog for dtstyle    
$ **    
$ **    
$ *****************************************************************************    
$ **    
$ **     (c) Copyright 1990 Hewlett-Packard Company    
$ **     All Rights reserved    
$ **    
$ **    
$ *****************************************************************************    
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
$ 2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).    
$     
$     The localization of these messages is OPTIONAL.  These messages are    
$     identified by the following:    
$     
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE    
$     
$ 3. Messages that should not be localized.      
$     
$     These messages are identified by the following:    
$     
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz    
$     
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****    
$     
$ ******************************************************************************    
$ *************************************<L>*************************************    
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------    
$ * Comment lines begin with a $ except $set which indicates the start of a     
$ * new set.    
$ *    
$ * Do not delete any comments; you may add comments for your use.    
$ *    
$ *     
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------    
$ *************************************<L>*************************************    
$quote "

$set 2
$ Message 1 from set 2 was deleted from the message catalog    
$ _DtMessage 2 is the title of all Information dialog boxes    

2 "Aviso"
$ _DtMessage 3 is the title of all Error dialog boxes    

3 "Error"
$ _DtMessage 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE    

4 "No se puede comunicar con el gestor\n\
de sesiones... Saliendo."
$  MESSAGES 5-6 IN SET 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE    

5 "El Gestor de estilos ya está en ejecución,\n\
se ha cancelado el segundo intento."

6 "Se ha producido un error del Kit\n\
de Utilidades de X... Se sale."
$ _DtMessage 7 is the title of all Help dialog boxes    

7 "Gestor de estilos - Ayuda"
$ _DtMessage 8 is an error message that appears when you are using Itemh Help    
$ and select on something outside of the Style Manager    

8 "Debe de seleccionar un elemento\n\
en el Gestor de estilos."
$ _DtMessage 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE    

9 "No se pudo obtener información del protector\n\
de pantalla desde el gestor de sesiones. La \n\
configuración de arranque puede ser incorrecta."

$set 4        Redefine set#

$ _DtMessage set 4 is for the Style Manager main window    
$ Messages 6,8-26,34,39 from set 4 were deleted from the message catalog    
$ _DtMessage 1 is the title of the main dtstyle window    

1 "Gestor de estilos"
$ mnemonic and label for the File menubar item.    

2 "A"

3 "Archivo"
$ mnemonic and label for the Help menubar item.    

4 "y"

5 "Ayuda"
$ _DtMessage 7 is the label for the Exit button in the File pulldown    
$ the mnenomic for the Exit button is set 4 _DtMessage 40    

7 "Salir"
$ Messages 27-33,35  are the labels on the buttons in the dtstyle main window.    

27 "Color"

28 "Font"

29 "Fondo"

30 "Teclado"

31 "Ratón"

32 "Pitido"

33 "Pantalla"

35 "Arranque"
$  MESSAGE 36 IN SET 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE    

36 "No hay información de restauración de Dtstyle disponible para la sesión actual"
$ _DtMessage 37 is the label on the dtstyle icon    

37 "Estilo"
$ Message 38 is the label on the Window button in the dtstyle main window.    

38 "Ventana"
$ _DtMessage 40 is replaced with message 53    
$ 40 E    
$ *    
$ * mnemonic and label for the Help menu items.    
$ *     
$ Messages 41-42 have been replaced with messages 54-55    
$ Overview... menu item    
$ 41 I    
$ 42 Introduction...    
$ Tasks... menu item     

43 "T"
$ Message 44 has been replaced with messages 56    
$ 44 Tasks...    
$ Reference... menu item    
$ 45 R    
$ Message 46 has been replaced with messages 57    
$ 46 Reference...    
$ On Item menu item    
$ Keybaord menu item    

47 "S"

48 "Sobre el tema"
$ Using Help... menu item    

49 "U"
$ Message 50 has been replaced with messages 58    
$ 50 Using Help...     
$ Messages 51-52 have been replaced with messages 59-60    
$ Overview menu item    
$ 51 V    
$ 52 Version...    
$ Message 53 is replaced with message 40    

53 "x"
$ Messages 54-55 are replacements for messages 41-42    

54 "g"

55 "Información general"
$ Message 56 is a replacement for messages 44    

56 "Tareas"
$ Message 57 is a replacement for messages 46    

57 "Referencia"
$ Message 58 is a replacement for messages 50    

58 "Uso de la ayuda"
$ Messages 59-60 are replacements for messages 51-52    

59 "A"

60 "Acerca del Gestor de estilos"

$set 5        Redefine set#

$ _DtMessage set 5 is for the Font dialog    
$ Messages 1-6, 7-16 from set 5 were deleted from the message catalog    
$ _DtMessage 17 is the label of the titlebox of the preview font area    

17 "Previsualizar"
$ _DtMessage 18 appears in a Notice dialog after the user selects a new font    
$ and presses Ok - when the resource *WriteXrdbImmediate is True    

18 "En algunas aplicaciones los cambios de los fonts se\n\
mostrarán la próxima vez que se arranquen.\n\
Otras aplicaciones, tales como el gestor de archivos\n\
y el gestor de aplicaciones, no mostrarán los cambios\n\
de fonts hasta que Salga del escritotorio y, después,\n\
de volver a arrancar."
$ _DtMessage 19 appears in a Notice dialog after the user selects a new font    
$ and presses Ok - when the resource *WriteXrdbImmediate is False    

19 "El font seleccionado se usará\n\
cuando reinicie esta sesión."
$ _DtMessage 20 and 21 is the text that initially appears in the Font Preview     
$ area displaying sample user and system fonts.    

20 "AaBbCcDdEeFfGg0123456789"

21 "AaBbCcDdEeFfGg0123456789"
$ _DtMessage 22 is the label of the titlebox containing the list of font sizes    

22 "Tamaño"
$ _DtMessage 23 is the title of the Fonts dialog    

23 "Gestor de estilos - Font"
$ _DtMessage 24 is an informative message used by the Fonts dialog     

24 "El font que se utiliza actualmente para el escritorio no estM-a\n\
disponible en la lista TamaM-qo. Si se selecciona y aplica un nuevo\n\
font, no podrM-a volver al font actual utilizando el diM-alogo\n\
Gestor de estilos - Font."

$set 6        Redefine set#

$ _DtMessage set 6 is for the Beep dialog    
$ _DtMessage 1,6 from set 6 have been deleted from the message catalog    
$ Messages 2 through 5 are the default buttons and labels of the scales.      

2 "Predeterminado"

3 "Volumen"

4 "Tono"

5 "Duración"
$ _DtMessage 7 is the title of the Beep dialog box    

7 "Gestor de estilos - Pitido"

$set 7        Redefine set#

$ _DtMessage set 7 is for the Startup dialog    
$ Messages 1-2 from set 7 have been deleted from the message catalog    
$ Messages 3 through 9 are labels of the buttons and text visible in the     
$ Session Startup dialog box.     

3 "En Inicio de sesión:"

4 "Reanudar sesión actual"

5 "Volver a sesión Inicio"

$ Message 6 is obsolete - DO NOT localize message #6 from this set
$ 6 "OBS- Preguntar al finalizar la sesión"

7 "Diálogo de Confirmación de Fin de sesión:"

8 "Activar"

9 "Desactivar"

10 "Establecer sesión de inicio ..."
$ _DtMessage 11 is a warning message when the user pushes Set Home Session...    
$ Notice the spaces before Continue .. the spaces make it centered.    

11 "Esto reemplazará la sesión de inicio por\n\
la sesión actual.\n\n\
                  ¿Continuar?"
$ _DtMessage 12 is the title of the warning dialog box when Set Home Session is     
$ pushed.     

12 "Aviso"
$ _DtMessage 13 is an error message    

13 "Esta sesión se inició desde un script de arranque del\n\
Sistema X Window.  La configuración de arranque no es\n\
válida, ya que la sesión no se puede restaurar."
$ _DtMessage 14 is the title of the Session Startup dialog box.    

14 "Gestor de estilos - Arranque"
$ _DtMessage 15 is an error message    

15 "El Gestor de sesiones no está en ejecución.\n\
La configuración de arranque es válida."

$ Message 16 is used in the Sessin Startup dialog.  It is the
$   string that appears to right of the Startup dialog icon IF
$   the user selects a display-specfic session at the login screen.
16 "Sesión específica para la pantalla:"

$set 8        Redefine set#

$ All of the messages from set 8 have been deleted from the message catalog    

$set 9        Redefine set#

$ _DtMessage set 9 is for the Mouse dialog    
$ Message 7 from set 9 have been deleted from the message catalog    
$ Messages 1-4 are labels of toggle buttons in the Mouse Dialog.     

1 "Izquierdo"

2 "Derecho"

3 "Sentido:"

4 "Botón 2:"
$ _DtMessage 5 is the label of default button.    

5 "Predeterminado"
$ Messages 6,8,9 are labels of the scales.    

6 "Doble-Pulsación"

8 "Aceleración"

9 "Umbral"
$  MESSAGE 10 IN SET 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE    

10 "ERROR: al establecer correspondencia de puntero\n"

11 "Transferir"

12 "Ajustar"
$ _DtMessage 13 appears in a Notice dialog after the user selects the     
$ left-handed or right-handed toggle    

13 "Los botones derecho e izquierdo del ratón intercambiarán\n\
las funciones inmediatamente depués de pulsar Bien.\n\
El gráfico `Prueba Doble-Pulsación' de la ventana \n\
Ratón muestra el sentido actual mediante las \n\
posiciones del Botón 1 y el Botón 3."
$ _DtMessage 14 appears in a Notice dialog after the user selects the     
$ transfer toggle to change the function of the middle mouse button.    

14 "Si pulsa en Bien, el botón 2 del ratón se establecerá\n\
en `transferir' (arrastrar) en la siguiente sesión."
$ double-click time and presses Ok (and the resource     
$ *WriteXrdbImmediate is True)    

15 "El tiempo nuevo de doble-pulsación entrará en\n\
vigor cuando se reinicien las aplicaciones."
$ _DtMessage 16 appears in a Notice dialog after the user selects a new     
$ double-click time and presses Ok (and the resource     
$ *WriteXrdbImmediate is False)    

16 "El tiempo nuevo de doble-pulsación entrará en \n\
vigor en la próxima sesión.  "
$ _DtMessage 17 is the text below the mouse icon. Notice the two lines.    

17 "Prueba\n\
Doble-Pulsación"
$ _DtMessage 18 is the Mouse Dialog title    

18 "Gestor de estilos - Ratón"
$ _DtMessage 19 appears in a Notice dialog after the user selects the     
$ adjust toggle to change the function of the middle mouse button.    

19 "Si pulsa en Bien, el botón 2 del ratón se establecerá\n\
en `ajustar' en la siguiente sesión. El botón 2 se\n\
puede utilizar para ajustar texto y selecciones de listas.\n\
Las acciones de transferencia (arrastrar) sobre texto y \n\
listas requerirán el uso del Botón 1."

$set 10        Redefine set#

$ _DtMessage set 10 is for the SaveRestore functionality    
$ Messages 2,4 from set 10 have been deleted from the message catalog    
$  MESSAGE 1,3 IN SET 10 WILL ONLY APPEAR IN THE DT ERRORLOG FILE    
$ Messages 1,3 make up one error message.  Put together, they say     
$ "<filename> <_DtMessage 2> <_DtMessage 1>    

1 "Verificar permisos de archivo."

3 "%s es el archivo que se tendría que haber usado para guardar la sesión\n"

$set 11        Redefine set#

$ _DtMessage set 11 is for the Backdrop dialog    
$  MESSAGE 1 IN SET 11 WILL ONLY APPEAR IN THE DT ERRORLOG FILE    
$ _DtMessage 1 - could not open a backdrop directory    

1 "No se pudo abrir el directorio %s. "
$ _DtMessage 2 - error opening the backdrop files    

2 "La porción de fondo del Gestor de estilos no\n\
funcionará porque no hay disponibles archivos\n\
de fondo. Compruebe $HOME/.dt/errorlog."
$ _DtMessage 12 is the title of the Backdrop dialog box    

12 "Gestor de estilos - Fondo"

$set 12        Redefine set#

$ _DtMessage set 12 is for the Screen dialog    
$ Message 2 from set 12 has been deleted from the message catalog    
$ _DtMessage 1 is the title of the Screen dialog box.     

1 "Gestor de estilos - Pantalla"
$ Messages 3-14 are the labels and buttons for the Screen dialog    

3 "Predeterminado"

4 "Protector de pantalla"

5 "Iniciar protector"

6 "Tiempo por protector"

7 "minutos"

8 "Bloquear pantalla"
$ Message 9 has been removed     

10 "Activar"

11 "Desactivar"

12 "Protector de pantalla:"

13 "Bloquear pantalla:"

14 "Iniciar bloqueo"

15 "Borrado automático de pantalla"

16 "Borrador de pantalla:"

17 "Iniciar borrado"

18 "Bloquear panel frontal"
$ Message 19 has been replaced by message 21    

19 "Usar fondos de bloqueo"

20 "Duración por fondo"
$ Message 21 is a replacement for message 19    

21 "Uso de fondos para bloqueo"

22 "Bloqueo transparente"

$set 13        Redefine set#

$ _DtMessage set 13 is for the Keyboard dialog    
$ _DtMessage 1,5 from set 13 have been deleted from the message catalog    
$ _DtMessage 2 through 4 are labels of scales, buttons, and text visible in the    
$ Keyboard dialog box.    

2 "Predeterminado"

3 "Auto repetición"

4 "Volumen pulsación"
$ _DtMessage 6 is the title of the Keyboard dialog box    

6 "Gestor de estilos - Teclado"

$set 14        Redefine set#

$ _DtMessage set 14 is for the Color dialog    
$ _DtMessage 1-4,9,14-15,17,19,20,22,24 from set 14 have been deleted from the message catalog    
$ Messages 5 through 8 are labels and buttons visible in the Color dialog    

5 "Paletas"

6 "Añadir..."

7 "Suprimir..."

8 "Modificar..."
$ _DtMessage 10 is the title above the text area in the Add Palette dialog box    

10 "Nuevo nombre de paleta:"
$ _DtMessage 11 is the title of the Add Palette dialog box.    

11 "Añadir paleta"
$ Messages 12 through 14 are error messages.    

12 "El nombre de la paleta no puede\n\
contener estos caracteres:\n\n\
<espacio> * : ( ) [ ] { } < > ! | \" / //"
$ TO SAY THE PALETTE NAME MUST BE 10 BYTES OR LESS    

13 "El nombre de la paleta debe ser de\n\
10 caracteres o menos.\n"
$ _DtMessage 16 is the title of the Delete Palette dialog box.  See screen 4a (2).    

16 "Suprimir paleta"
$ _DtMessage 18 appears in an information dialog.    

18 "No se puede suprimir la última paleta.\n"
$ _DtMessage 21 is an error message    

21 "Aviso"
$ _DtMessage 23 appears in an information dialog    

23 "¿Suprimir paleta '%s'?\n"
$ _DtMessage 25 is an error message that appears when you press the color    
$ button and the color server is not running for some reason.    

25 "La porción de color del Gestor de estilos no\n\
operará porque el servidor de colores no está\n\
en ejecución. Verificar $HOME/.dt/errorlog."
$ _DtMessage 26 can be caused by setting the resource *useColorObj to False    
$ and then running dtstyle and trying to post the Color Dialog.    

26 "La porción de color del Gestor de estilos\n\
no operará porque el recurso 'useColorObj' \n\
se ha establecido en Falso (False)."
$ _DtMessage 27 is for the new ColorUse dialog when you press ok.    

27 "El valor nuevo de Usar Color entrará en \n\
vigor en la próxima sesión."
$ _DtMessage 28 is a warning message that appears on Black and White systems    

28 "La paleta seleccionada entrará en vigor\n\
en la próxima sesión."
$ _DtMessage 29 appears in a warning dialog    

29 "Una paleta llamada '%s' ya existe.\n\
Esta nueva paleta sobregrabará la anterior.\n\
¿Es ésto lo que desea hacer?"
$ _DtMessage 30 is the title of the Color dialog box.    

30 "Gestor de estilos - Color"
$ _DtMessage 31-33 are the ColorUse options    
$ 31 High Color    
$ 32 Medium Color    
$ 33 Low Color    

34 "Negro y blanco"

35 "Predeterminado"
$ Dt Message 36 has been replaced by message 39    
$ _DtMessage 36 is the title of the Color Use dialog    
$ 36 DT Color Use    
$ Dt Message 37 has been replaced by message 40    
$ _DtMessage 37 is the Color Use button in the main Color Dialog     
$ 37 DT Color Use...    
$ _DtMessage 38 is an error message that appears when you press the color    
$ button and there are no palette files to be found    

38 "La porción de color del Gestor de estilos\n\
no operará porque no hay archivos de paletas\n\
disponibles. Verificar $HOME/.dt/errorlog."
$ DtMessage 39 is a replacement for DtMessage 36    
$ DtMessage 39 is the title of the Color Use dialog    

39 "Número de colores a utilizar:"
$ DtMessage 40 is a replacement for DtMessage 37    
$ DtMessage 40 is the title of the Color Use dialog    

40 "Número de colores..."
$ _DtMessages 41-43 are replacements for Dtmessages 31-33    
$ _DtMessage 41-43 are the ColorUse options    

41 "Más colores para el escritorio"

42 "Más colores para las aplicaciones"

43 "Máximo número de colores para las aplicaciones"

$set 15        Redefine set#

$ All of the messages from set 15 have been deleted from the message catalog    

$set 16        Redefine set#

$ _DtMessage set 16 is for the Color dialog    
$ _DtMessage 1-10,13-17 from set 16 have been deleted from the message catalog    
$ _DtMessage 11-12 form one error message that appears in an error dialog    

11 "No es posible suprimir '%s'.\n"

12 "Verificar $HOME/.dt/errorlog para sugerencias."
$  MESSAGE 18-21 IN SET 16 WILL ONLY APPEAR IN THE DT ERRORLOG FILE    

18 "Aviso, demasiados directorios listados en el recurso paletteDirectories.\n\
El número máximo es %d. "
$ _DtMessage 19 - could not open a palette directory    

19 "No se pudo abrir el directorio %s. "
$ _DtMessage 20 - error opening the palette file    

20 "No se pudo abrir %s. "
$ _DtMessage 21 - error reading a palette file    

21 "%s es un archivo de paletas no válido."

$set 17        Redefine set#

$ _DtMessage set 17 is for the Modify Color dialog    
$ Messages 1-3,13,16 from set 17 have been deleted from the message catalog    
$ Messages 4 through 12 are labels, buttons, and text visible in the    
$ Modify Color dialog box    

4 "Muestra de color"

5 "Antiguo"

6 "Nuevo"

7 "Tomar color"

8 "Editor de colores"

9 "Matiz"
$ We recommend that you don't translate R,G,B which stands for Red, Green,    
$ Blue unless there are standard single letter acronyms in your language.    

10 "R"

11 "G"

12 "B"
$  MESSAGE 14-15 IN SET 17 WILL ONLY APPEAR IN THE DT ERRORLOG FILE    

14 "Aviso, no se pudo tomar control del puntero.\n"

15 "Aviso, no se pudo tomar control del Teclado.\n"
$ _DtMessage 16 is the title of the Modify Color dialog box.    

16 "Gestor de estilos - Modificar color"

$set 18        Redefine set#

$ _DtMessage set 18 is for the Window dialog    
$ _DtMessage 1 is the title of the Window dialog box    

1 "Gestor de estilos - Ventana"
$ Messages 2 through 5 are labels on the Window dialog.    

2 "Predeterminado"

3 "Comportamiento de ventana"

4 "Iconos de ventana"
$ Messages 5-8 have been replaced by messages 13-16     
$ 5 Focus Follows Mouse    
$ 6 Click In Window For Focus    
$ 7 Raise Window With Focus    
$ 8 Opaque Move    

9 "Usar cuadro de icono"

10 "Colocar en espacio de trabajo"

11 "Permitir ventanas principales en parte superior"

12 "El gestor de espacios de trabajos debe reiniciarse\n\
para que los cambios surtan efecto.\n\
Pulse OK para reiniciar el gestor de espacios de \n\
trabajos o Cancelar para restablecer los cambios."
$ Messages 13-16 are replacements for messages 5-8    

13 "Señalar en ventana para hacerla activa"

14 "Pulsar en ventana para hacerla activa"

15 "Activar la ventana cuando esté activa"

16 "Mostrar contenido durante mover"

$set 19
$ The set 19 contains messages for the I18N dialog

$ 
$ Messsages 1 to 11 are labels displayed in the dialog.
$ 

$quote

$ Message 1 is the title of the Dialog.
1 Gestor de estilos - Internacionalización

$ Message 2 appears on the button used to reset the defailt values.
2 Predeterminado

$ Message 3 appears as the title of the title box containing Input
$ Methods information.
3 Método de introducción de datos

$ Message 4 appears as the title of the title box containing Input
$ Method Start Mode choice.
4 Método inicial de introducción de datos

$ Message 5 appears as the title of the title box containing PreeditType
$ information.
5 Tipo de preedición

$ Message 6 is the label presenting the server host names
6 Anfitrión servidor:

$ Message 7 is the label presenting the Input Method list
7 Método de introducción de datos

$ Message 8 and 9 are the toggle buttons for the start mode selection:
$ "Ask at login" will cause dtimsstart to present the list of Input
$ Methods at next login; "Resume Current Input Method" will cause
$ dtimsstart to use the selected IM at next login, without asking the
$ user.
8 Solicitar al conectar

9 Reanudar método de introducción de datos actual

$ Message 10 is the label displayed on the button used to move up a
$ selected element in the preedit Type list (meaning at the previous
$ position).
10 Mover hacia arriba

$ Message 11 is the label displayed on the button used to move down a
$ selected element in the preedit Type list (meaning at the next
$ position).
11 Mover hacia abajo

$ 
$ Messages 20-21 are displayed in an warning dialog when the user
$ hits the OK button.
$ 

$ Message 20 appears when the user modifies the XmNpreeditType
$ resource value, and the resource *writeXrdbImmediate is True.
$ Do NOT localize the word XmNpreeditType.
20 El nuevo valor de XmNpreeditType entrará en vigor\n\
cuando se reactiven las aplicaciones.

$ Message 21 appears when the userselects a new Input Method Server,
$ or modifies the IMS Start Mode, or modifies the XmNpreeditType
$ resource value, and the resource *writeXrdbImmediate is False.
21 La selección entrará en efecto\n\
en la próxima sesión.

$ 
$ Messages 30 to 41 appear as errors in Error Dialog.
$ 

$ Do NOT localize the word HOME.
30 No se ha definido la variable de entorno HOME.

$ Do NOT localize the word LANG.
31 No se ha definido la variable de entorno LANG.

32 El escritorio no soporta este lugar.

33 No se puede crear el archivo de método de introducción de datos del usuario.

34 No se puede encontrar el archivo de selección del método de introducción de datos del usuario.

35 No se puede guardar el archivo de selección de método de introducción de datos del usuario.

36 Anfitrión desconocido

$ Do NOT translate dtimsstart.
37 No se encontró dtimsstart.\n\
Es posible que el entorno de escritorio\n\
no esté instalado correctamente.

38 Ha fallado la ejecución remota en el\n anfitrión seleccionado.

39 No se puede inicializar la ejecución remota.

40 No se puede obtener información acerca del método de introducción de datos del anfitrión seleccionado.

41 Error desconocido



$set 99        Redefine set#

$    DO NOT TRANSLATE THESE MESSAGES    
$    DO NOT CHANGE THESE MESSAGES    
$    DO NOT LOCALIZE THESE MESSAGES    
$    These messages are used for the version information.    

$quote "
1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $XConsortium: dtstyle.msg /main/9 1996/10/19 14:46:52 rws $"
