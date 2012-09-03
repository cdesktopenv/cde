$ "$XConsortium: dtlogin.msg /main/12 1996/12/20 15:05:02 rws $"    
$ *************************************<+>*************************************    
$ *****************************************************************************    
$ **    
$ **  File:        dtlogin.msg    
$ **    
$ **  Project:     CDE dtlogin    
$ **    
$ **  Description:    
$ **  -----------    
$ **  This file is the source for the message catalog for dtlogin    
$ **    
$ **    
$ *****************************************************************************    
$ **    
$ **     (c) Copyright 1990 Hewlett-Packard Company    
$ **     All Rights reserved    
$ **    
$ **    
$ *****************************************************************************    
$ **    
$ **    
$ **  ------------------------- MODIFICATION RECORD --------------------------    
$ *    
$ *     
$ **  ----------------------- MODIFICATION RECORD END ------------------------    
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
$quote "

$set 1
$ set 1 is labels    
$     
$ messages 1 through 6 are button labels on the login screen    

1 "Por favor, entre su nombre de usuario"

2 "Por favor, entre su contraseña"

3 "OK"

4 "Iniciar "

5 "Opciones"

6 "Ayuda"
$     
$ messages 7 through 11 are options button menu items    
$     

7 "Restablecer pantalla inicio de sesión"

8 "Inicio de sesión línea de mandatos"

9 "Versión..."

10 "Sesión de ventana única"

11 "Idioma"
$     
$ messages 12 through 14 are miscellaneous button labels    
$     

12 "Iniciar DT"

13 "Cancelar"

14 "Ayuda"
$     
$ message 15, 17 are more option button menu items     
$ message 16 is the welcome message    
$     

15 "Sesión Lite DT "

16 "Bienvenido a"

17 "Escritorio normal"
$     
$ message 18-24    
$     

18 "* Suspendiendo inicio de sesión del escritorio...\r\n\
*\r\n"

19 "* Pulse [Intro] para obtener una solicitud de inicio de sesión.\r\n\
*\r\n"

20 "* Inicio de sesión. El inicio de sesión del escritorio se reanudará \r\n\
* en breve después de finalizar la sesión.\r\n\
*\r\n"

21 "* Reanudando el inicio de sesión del escritorio...\r\n\
*\r\n"

22 "* Iniciando la sesión del escritorio en la pantalla %1$s...\r\n\
*\r\n"

23 "* Espere a la pantalla de inicio de sesión del escritorio antes de iniciar la sesión.\r\n\
*\r\n"

24 "* El servidor de X no se puede iniciar en la pantalla %1$s...\r\n\
*\r\n"
$ messages 25 is a options button menu item    

25 "Sesión"
$     
$ 26 replaces 16 as the greeting after a user name has been     
$ entered. The %s is replaced by the user name.    
$     

26 "Bienvenido, %s"

$ 
$ 27 replaces 10 as the label for the failsafe option menu item
$ 
$quote

27 Sesión a prueba de fallos

$ 
$  Sun specific message catalog id numbers in MC_LABEL_SET messages.
$  (Starting at a random uneven higher number, 73, to allow some room  
$  for some shared sample source message additions. 
$ 
73 Ultimo escritorio del usuario
$ 
$  Following used in Language menu.  
$  "to" used in phrases like "ja to zh" locales
74 a


$set 2

$ set 2 is error messages    
$     

$quote "
1 "Inicio de sesión incorrecto; inténtelo de nuevo."

2 "No se puede cambiar al directorio inicio."

3 "Lo siento. Ya están conectados el número máximo de usuarios."
$ message 4 is a lengthy message that should be localized to correspond to     
$ the language.      

4 "\n\
Tiene que elegir una contraseña inicial para su cuenta,\n\
o bien su contraseña actual ha caducado.\n\n\
Después de establecer su contraseña, debe iniciar de nuevo \n\
la sesión utilizando la nueva contraseña."

5 "Error de inicio de sesión, id de usuario no válido."

6 "Error de inicio de sesión, id de grupo no válido."

7 "Error de inicio de sesión, id de comprobación no válido."

8 "Error de inicio de sesión, distintivo de comprobación no válido."

9 "Los inicios de sesión están inhabilitados en este momento."

10 "Sólo root desea ejecutar %1$s\n"

11 "Reconstruyendo la lista de idiomas por omisión a partir de %1$s\n"

12 "Volviendo a explorar los archivos de configuración y de servidores\n"

13 "Releyendo el archivo de configuración %1$s\n"

14 "Releyendo el archivo de servidores %1$s\n"

15 "Releyendo el archivo de acceso %1$s\n"

16 "dtlogin: sin memoria en la rutina\n"

$set 3

$ set 3 is help text    
$     
$ message 1 is generated when the help button on the login screen is clicked.
$ Comments for messsage #1:
$ 
$ Under [Options] there is an item called "Session".  Under
$ Session there are five items and these items have been
$ translated in other files.  To be consistent with the other
$ translations, the translations used below should be the same
$ as the other translations.  The items and their file are
$ listed below:
$ 
$   1. Current   - _common.session.tmsg ; set 1, message 2
$ 
$   2. Home      - _common.session.tmsg ; set 1, message 1
$ 
$   3. <display-name> - Current   - <display-name> on the options
$         menu will be replaced with the workstations display
$         name so does not need to be translated.  "Current" should
$         be the same as _common.session.tmsg ; set 1, message 2
$ 
$   4. <display-name> - Home      - <display-name> on the options
$         menu will be replaced with the workstations display
$         name so does not need to be translated.  "Home" should
$         be the same as _common.session.tmsg ; set 1, message 1
$ 
$ 
1 "\n\
Ayuda para Inicio de sesión:\n\
\n\
Utilice la pantalla conexión para identificarse frente al sistema.\n\
\n\
Para iniciar la sesión:\n\
       1) Escriba su nombre y pulse <Intro> o haga clic [OK].\n\
          (si no ha creado un usuario, utilice \"root\" como\n\
          nombre de usuario)\n\
       2) Escriba su contraseña y pulse <Intro> o haga clic [OK].\n\
\n\
[Iniciar]\n\
   Borra las entradas de modo que pueda volver a introducir el nombre\n\
  de usuario e intentar la operación de nuevo\n\
\n\
[Opciones]\n\
   Le permite modificar las opciones de sesión:\n\
\n\
   Con el puntero del ratón sobre el botón [Optiones], mantenga\n\
   pulsado el botón 1 del ratón. Arrastre el puntero a través de\n\
   las opciones. Al liberar el botón del ratón con el puntero\n\
   sobre un elemento, éste se seleccionará. Puede escoger:\n\
\n\
   Idioma - para cambiar el idioma de sesión\n\
   Sesión - para seleccionar un entorno gráfico\n\
      Actual - Activa su sesión más reciente\n\
      Inicio - Activa su sesión de inicio (si ha especificado una)\n\
      <nombre de pantalla> - Actual - Activa su sesión más reciente para\n\
          la pantalla especificada\n\
      <nombre de pantalla> - Inicio - Activa su sesión de inicio para\n\
          la pantalla especificada\n\
      Sesión a prueba de fallos - Activa una sesión a prueba de fallos.\n\
 Inicio de Sesión de Línea de Mandatos - para conmutar a un entorno de línea de mandatos.\n\
 Restaurar Pantalla de Inicio de Sesión - para restaurar el servidor X.\n\
\n\
Pulse sobre cualquier lugar del fondo con cualquier botón del ratón\n\
para renovar la pantalla, por ejemplo, en caso de que los mensajes\n\
del sistema hagan que la pantalla resulte ilegible.\n\
\n\
Para obtener más información de ayuda, primero inicie la sesión y luego\n\
pulse el icono Gestor de Ayuda del Panel Frontal (son los libros con\n\
interrogante).\n\
A continuación, busque el volumen de ayuda del Gestor de Inicio de sesión.\n"


$ message 2 is generated when you attempt to start a system whose id is    
$ set to unknown or if it is not configured for networking.    

2 "\n\
La estación de trabajo no tiene un nombre exclusivo. Actualmente está\n\
establecido en \"desconocida\". Es aconsejable darle un nombre a esta \n\
estación de trabajo.\n\n"
$ message 3 is generated when the help button on the chooser screen is clicked.    

3 "\n\
Ayuda  para el selector:\n\n\
Utilice la pantalla del selector para elegir el servidor en\n\
el que va a iniciar la sesión.\n\n\
La pantalla del selector contiene una lista con 2 partes para cada\n\
entrada. La primera parte (Nombre de servidor) contiene el nombre\n\
en la red para cada servidor disponible. La segunda parte (Información\n\
de servidor) contendrá información diferente según el gestor de inicio\n\
de sesión que se ejecute en cada servidor. Si el servidor está ejecutando\n\
el escritorio CDE, debe proporcionar información acerca de los usuarios\n\
(o dispositivos tty) que se utilizan en este momento y de los promedios de\n\
carga (promedio de utilización de CPU) de los últimos 5, 10 y 15 minutos.\n\n\
Para elegir un servidor para el inicio de sesión, seleccione la línea\n\
de la lista que contenga el nombre del servidor que desee.\n\n\
Cuando haya realizado su selección, pulse el botón OK.\n\
Se mostrará una pantalla de inicio de sesión del servidor \n\
que haya seleccionado.\n\n\
[Actualizar lista]\n\
   Borrará la lista y volverá a crearla consultanto a los servidores\n\
   de la red si van a permitir inicios de sesión.\n\n\
[Opciones]\n\
   Permite cambiar las opciones de la sesión:\n\n\
   Con el puntero del ratón sobre el botón [Opciones], pulse y mantenga\n\
   pulsado el botón 1 del ratón. Arrastre el puntero sobre las opciones.\n\
   Cuando libere el botón del ratón con el puntero sobre un elemento,\n\
   se seleccionará dicho elemento. Puede elegir:\n\n\
   Idioma - para cambiar el idioma de la sesión\n\
   Restablecer pantalla de inicio de sesión - para rearrancar el servidor de X\n\n\
Pulse cualquier botón del ratón en el fondo para refrescar\n\
la pantalla, por ejemplo, si no se puede leer a causa de \n\
los mensajes del sistema.\n\n"

$set 4

$ set 4 is log error messages    
$     

1 "Archivo de acceso \"%1$s\", host \"%2$s\" no hallado\n"

2 "ReadHostEntry\n"

3 "Archivo de acceso %1$s, pantalla %2$s desconocida\n"

4 "No se puede abrir el archivo control de acceso %1$s, no se otorgarán peticiones XDMCP\n"

5 "Error de autentificación de Domain/OS: %1$s (%2$s/%3$s)"

6 "No se puede abrir el archivo de autorizaciones de servidor %1$s\n"

7 "No se puede grabar el archivo de autorizaciones de servidor %1$s\n"

8 "saveAddr"

9 "Problemas al obtener la configuración de la interfaz de red"

10 "setAuthNumber"

11 "No se puede bloquear el archivo de autorizaciones %1$s o la copia de seguridad %2$s\n"

12 "No se puede mover la autorización al sitio \n"

13 "No se puede crear/bloquear el archivo de pid %1$s\n"

14 "No se puede bloquear el archivo de pid %1$s, se está ejecutando otro Dtlogin (pid %2$d)\n"

15 "No se puede acceder al archivo de servidores %1$s\n"

16 "NewDisplay"

17 "No se puede abrir el Archivo de Registro de Errores %1$s\n"

18 "TrimErrorLog(): el tamaño máximo del archivo de registro de errores es de 200Kb.\n"

19 "Falta nombre de pantalla en el archivo de servidores\n"

20 "Falta tipo de pantalla para %1$s\n"

21 "El nombre de host %1$s no es válido. Se establece DISPLAY en ':0'\n"

22 "Tipo de pantalla %1$s inaceptable para la pantalla %2$s\n"

23 "Tiempo excedido de pseudo-restauración.\n"

24 "GetResource"

25 "no hay espacio para reasignar argumento\n"

26 "No se puede abrir el archivo de configuración %1$s\n"

27 "Argumentos adicionales en la línea de mandatos:"

28 "StartServer(): sin argumentos\n"

29 "No se puede ejecutar el servidor %1$s\n"

30 "Ha fallado fork de StartServer(), latente\n"

31 "El servidor murió inesperadamente\n"

32 "Se cuelga en intento #%2$d de XOpenDisplay(%1$s), se cancela.\n"

33 "Ha fallado el intento #%1$d de abrir el servidor para %2$s, abandonando\n"

34 "Error grave de E/S %d (%1$s)\n"

35 "Error X \n"

36 "Ha fallado inicio de sesión\n"

37 "AVISO: no se pudo proteger la pantalla %1$s \n"

38 "AVISO: no se pudo proteger el teclado de la pantalla %1$s\n"

39 "BLS - No se pudo obtener información de contabilidad.\n"

40 "BLS - No se pudo obtener información de contabilidad protegida.\n"

41 "BLS - No se pudo verificar el nivel de seguridad.\n"

42 "BLS - No se pudo cambiar al usuario: %1$s.\n"

43 "AFS - anomalía de setgroups() al iniciar la sesión\n"

44 "Ha fallado setuid para el usuario %s, númerr = %d\n"

45 "No hay directorio inicio %1$s para el usuario %2$s, se usa /\n"

46 "Ha fallado la ejecución de la sesión %1$s\n"

47 "La sesión no tiene mandato/argumentos\n"

48 "No se puede iniciar una sesión para %s, ha fallado fork\n"

49 "Dtlogin no puede matar un cliente\n"

50 "No se puede ejecutar %1$s\n"

51 "No se puede hacer fork para ejecutar %1$s\n"

52 "Nombre de autentificación \"%1$s\" inadecuado. Use AFS, Kerberos o el nombre por omisión.\n"

53 "No se puede ejecutar Dtgreet %1$s para %2$s\n"

54 "No se puede hacer fork para ejecutar Dtgreet %1$s\n"

55 "Ha fallado la creación de zócalo en la boca %1$d\n"

56 "error al vincular la dirección de zócalo %1$d, númerr = %2$d\n"

57 "makeEnv"

58 "setEnv"

59 "parseArgs"

60 "Exploración de directorio %1$s supera los %2$d segundos. Termina la exploración...\n"

61 "%s pertenece a más de %1$d grupos, se ignora %2$s \n"

62 "error de audwrite(): el llamador no es un superusuario\n"

63 "error de audwrite(): número de suceso no válido en registro de comprobación.\n"

64 "error de audwrite(): númerr = %1$d\n"

65 "anomalía en autentificación de AFS: %1$s\n"

66 "La cuenta para %1$s ha caducado. Consulte al Administrador del Sistema.\n"

67 "Servidor muerto  %1$s\n"

68 "No se puede abrir el archivo de mapa de bits de logo: %1$s\n"

69 "no se puede leer el catálogo de mensajes \"%1$s\". Se usa el idioma por omisión."

70 "no se pudo inicializar la pantalla %1$s, el servidor puede estar bajo control.\n"

71 "(%1$s): La anchura de reborde de inicio de sesión sobrepasa la pantalla, se usa la anchura por omisión.  Las unidades de reborde son píxels.\n"

72 "(%1$s) La altura de reborde del inicio de sesión sobrepasa la pantalla, se usa la altura por omisión.  Las unidades de reborde son píxels.\n"

$set 5

$ set 5 is for other stuff specific to chooser.    

1 "Seleccionar un servidor para el inicio de sesión:"
$ 1 is the title/instructions for the chooser dialog    

2 "Nombre de servidor             Información de servidor"
$                                ^    
$                                | This needs to start in this column    
$ 2 is the headings for the list    

3 "Actualizar lista"

$set 99

$    DO NOT TRANSLATE THESE MESSAGES    
$    DO NOT CHANGE THESE MESSAGES    
$    DO NOT LOCALIZE THESE MESSAGES    
$    These messages are used for the version information.    

1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $XConsortium: dtlogin.msg /main/12 1996/12/20 15:05:02 rws $"
