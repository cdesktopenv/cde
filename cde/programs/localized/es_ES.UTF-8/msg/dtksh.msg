$ $XConsortium: dtksh.msg /main/2 1995/11/08 12:12:04 rswiston $
$ ENGLISH_SOURCE_VERSION= 1.5    
$quote "

$set 1

1 "DtkshCvtStringToPixel: El color '%s' no está definido"

$set 5        Redefine set#


1 "Se han especificado demasiados parámetros de recurso; se saltan '%s'"

2 "Especificación de recurso errónea; debe tener el formato 'nombre:valor' : %s"

3 "El separador de recursos es ':' pero ha utilizado '=' : %s"

4 "Ya se ha inicializado el kit de utilidades"

5 "Uso:   XtInitialize variable Nombreaplicación Claseaplicación [args ...]"

6 "Uso:   %s variable nombre clase padre [arg:val ...]"

7 "Uso:   XtCreateApplicationShell variable nombre clase [arg:val ...]"

8 "Uso:   XtPopup widget GrabNone|GrabNonexclusive|GrabExclusive"

9 "No se reconoce el tipo de control '%s'; se usa 'GrabNone'"

10 "Uso:   XtIsShell widget"

11 "Uso:   XtMapWidget widget"

12 "Uso:   XtUnmapWidget widget"

13 "Uso:   %s variable widget"

14 "Uso:   XtScreen variable widget"

15 "Uso:   XtWindow variable widget"

16 "Uso:   XtCallCallbacks widget Nombrecallback"

17 "Uso:   XtHasCallbacks variable widget Nombrecallback"

18 "Uso:   XtAddCallback widget Nombrecallback mandato-ksh"

19 "Uso:   XtRemoveCallback widget Nombrecallback mandato-ksh"

20 "Lo que sigue es un manejador de callback no válido: %s"

21 "La callback especificada no está registrada"

22 "Uso:   XtAddEventHandler widget máscara Noenmascarable mandato-ksh"

23 "Uso:   XtRemoveEventHandler widget máscara Noenmascarable mandato-ksh"

24 "El manejador de sucesos especificado no está registrado"

25 "Uso:   XtGetValues widget recurso:variable ..."

26 "El siguiente parámetro de recurso está incorrectamente formado: %s"

27 "Uso:   XtSetValues widget arg:val ..."

28 "Uso:   XtAddWorkProc variable mandato"

29 "Uso:   XtRemoveWorkProc IdProctrabajo"

30 "El parámetro IdProctrabajo debe ser un número hexadecimal: %s"

31 "Uso:   XtAddTimeOut variable milisegundos mandato"

32 "El parámetro milisegundos debe ser mayor que cero"

33 "Uso:   XtRemoveTimeOut Idintervalo"

34 "El parámetro Idintervalo debe ser un número hexadecimal: %s"

35 "Uso:   %s widget ..."

36 "Uso:   XBell pantalla volumen"

37 "Uso:   %s variable pantalla"

38 "El parámetro pantalla no es válido: %s"

39 "Uso:   XDefineCursor pantalla ventana Idcursor"

40 "El parámetro Idcursor no es válido: %s"

41 "Uso:   XUndefineCursor pantalla ventana"

42 "Uso:   XtRemoveAllCallbacks widget Nombrecallback"

43 "Uso:   XTextWidth variable Nombrefont serie"

44 "Uso:   %s pantalla dibujable [args ...]"

45 "El parámetro dibujable no es válido: %s"

46 "El parámetro destino no es válido: %s"

47 "Nombre de función de gráficos no reconocido: %s"

48 "Estilo de línea no reconocido: %s"

49 "Opción de dibujo no reconocida: %s"

50 "Habían pixels sobrantes que se han ignorado"

51 "Uso: XCopyArea pantalla origen dest origenX origenY anchura altura destX destY [args ...]"

52 "No se puede convertir el tipo de recurso '%s' a 'Serie'"

53 "No se puede convertir el tipo de recurso 'Serie' a tipo '%s'"

54 "Uso:   XtAddInput variable [-r] Descriptorarchivo Mandatoksh"

56 "Uso:   XtRemoveInput Identrada"

57 "El parámetro Identrada debe ser un número hexadecimal: %s"

59 "Uso:   XFlush pantalla"

60 "Uso:   XSync pantalla descartar"

61 "Uso:   XRaiseWindow pantalla ventana"

62 "Uso:   %s widget [True|False]"

63 "Uso:   %s widget conversiones"

64 "Uso:   XtUninstallTranslations widget"

65 "Uso:   XtParent variable widget"

66 "Uso:   XtLastTimestampProcessed variable pantalla"

67 "Uso:   DtSessionRestorePath widget Variablevía Archivoguardar"

68 "Uso:   DtSessionSavePath widget Variablevía Variablearchivo"

69 "Uso:   DtShellIsIconified widget"

70 "Uso:   DtSetStartupCommand widget mandato"

71 "Uso:   DtSetIconifyHint widget booleano"

73 "El parámetro Ventanaraíz no es válido: %s"

74 "Uso:   DtWsmSetCurrentWorkspace widget átomo"

75 "El átomo del espacio de trabajo no es válido: %s"

78 "Uso:   DtWsmSetWorkspacesOccupied pantalla ventana Listaespaciotrabajo"

79 "Uso:   DtGetHourGlassCursor variable pantalla"

80 "Uso:   DtWsmAddCurrentWorkspaceCallback variable widget mandato-ksh"

81 "Uso:   DtWsmRemoveCurrentWorkspaceCallback widget mandato-ksh manejador"

82 "Uso:   XtNameToWidget variable Widgetreferencia nombres"

83 "Uso:   DtDbLoad"

84 "Uso:   DtDbReloadNotify mandato-ksh"

85 "Uso:   DtActionExists Nombreacción"

86 "Uso:   DtActionLabel variable Nombreacción"

87 "Uso:   DtActionDescription variable Nombreacción"

88 "Uso: DtActionInvokeOnFiles widget Nombreacción Parmsterm Hostejec Hostcontext Dircontext Indicadoruso [nombre tipo host] ..."

89 "Uso:   DtLoadDataTypes"

90 "Uso:   DtDtsFileToDataType variable Nombrearchivo"

91 "Uso:   DtDtsFileToAttributeValue variable Nombrearchivo Nombreatrib"

92 "Uso:   DtDtsFileToAttributeList variable Nombrearchivo"

93 "Uso:   DtDtsDataTypeToAttributeValue variable Tipodatos Nombreatrib Nombreopc"

94 "Uso:   DtDtsDataTypeToAttributeList variable Tipodatos Nombreopc"

95 "Uso:   DtDtsFindAttribute variable nombre valor"

96 "Uso:   DtDtsDataTypeNames variable"

97 "Uso:   DtDtsSetDataType variable Nombrearchivo Tipodatos override"

98 "Uso:   DtDtsDataTypeIsAction Tipodatos"

99 "Uso: ttdt_open variable estado variable2 nombherramienta proveedor versión sendStarted"

100 "Uso:   tttk_Xt_input_handler Idproc origen id"

101 "El parámetro origen debe ser un entero: %s"

102 "El parámetro id debe ser un número hexadecimal: %s"

103 "Uso:   ttdt_close estado Idproc IdProcnuevo enviarDetenido"

104 "Uso:   ttdt_session_join variable estado Idsesión ManejadorWidgetshell join"

105 "Uso:   ttdt_session_quit estado Idsesión Patrones-sesión quit"

106 "El parámetro Patrones-sesión no es válido: %s"

107 "Usage: ttdt_file_event estado op patrones send"

108 "Uso: ttdt_file_quit estado patrones quit"

109 "Uso:   ttdt_Get_Modified Nombrevía ámbito tiempoexcedido"

110 "Uso:   %s status Nombrevía ámbito tiempoexcedido"

112 "Uso:   tt_error_pointer variable Estadott"

113 "Uso:   %s estado mens"

114 "Uso:   %s estado mens Estadomens SerieEstadomens destroy"

115 "Uso:   ttdt_file_join variable estado Nombrevía ámbito join mandato-ksh"

116 "Uso:   tt_file_netfile variable estado nombarchivo"

117 "Uso:   tt_netfile_file variable estado nombarchivored"

118 "Uso:   tt_host_file_netfile variable estado host nombarchivo"

119 "Uso:   tt_host_netfile_file variable estado host nombarchivored"

120 "Uso: XtIsSubclass widget clase"

121 "%s no es un nombre válido de clase de widget"

122 "Uso: XtClass variable widget"

$set 6        Redefine set#


1 "DtkshCvtWindowToString: El valor 'desde' tiene un tamaño no válido"

2 "DtkshCvtHexIntToString: El valor 'desde' tiene un tamaño no válido"

3 "DtkshCvtIntToString: El valor 'desde' tiene un tamaño no válido"

4 "DtkshCvtBooleanToString: El valor 'desde' tiene un tamaño no válido"

5 "DtkshCvtStringToWidget: El valor 'desde' tiene un tamaño no válido"

6 "DtkshCvtStringToWidget: No se puede hallar un widget denominado '%s'"

7 "DtkshCvtStringToCallback: El valor 'desde' tiene un tamaño no válido"

8 "DtkshCvtCallbackToString: El valor 'desde' tiene un tamaño no válido"

9 "DtkshCvtWidgetToString: El valor 'desde' tiene un tamaño no válido"

10 "DtkshCvtWidgetToString: No se puede hallar un nombre para el widget"

11 "DtkshCvtWidgetClassToString: El valor 'desde' tiene un tamaño no válido"

12 "DtkshCvtWidgetClassToString: Clase de widget desconocida"

13 "DtkshCvtStringToWidgetClass: Nombre de clase de widget desconocido"

14 "DtkshCvtScreenToString: El valor 'desde' tiene un tamaño no válido."

$set 11        Redefine set#


1 "No se puede hallar un campo denominado '%s' en la estructura '%s'"

2 "DtkshCvtCallbackToString: Se ha desbordado un almac. intermedio de conversión interna"

3 "Error de hash para el recurso '%s' en la clase de widget '%s'"

4 "No se ha suministrado nombre de función"

5 "No se puede ejecutar el mandato; no se ha inicializado el kit de utilidades"

6 "Ha fallado la creación del widget '%s'"

7 "El parámetro pantalla no es válido: %s"

8 "El átomo de propiedad siguiente no es válido: %s"

9 "El recurso siguiente no está definido para el '%s': %s"

10 "La posición especificada no es válida: %s"

11 "El font especificado no es válido: %s"

12 "El manejador de widget '%s' no hace referencia a un widget existente"

13 "El widget debe ser un widget 'mandato'"

14 "El widget debe ser un widget 'Ventanaprincipal' "

15 "El widget debe ser un widget 'escala' "

16 "El widget debe ser un widget 'Barradesplazamiento'"

17 "El widget debe ser un widget o gadget 'Botónconmutar'"

18 "El parámetro ventana no es válido: %s"

19 "No se puede asignar la memoria requerida; se sale"

20 "No se puede hallar el widget padre"

21 "No se puede localizar el símbolo '%s'"

22 "No se puede localizar el tipo '%s'"

23 "No se puede analizar la declaración '%s'; se utiliza 'largo sin signo'"

24 "Tipo de hijo desconocido: %s"

25 "Distintivo de opción no reconocido: %s"

26 "Uso:   %s widget posición"

27 "Uso:   %s widget hora"

28 "Uso:   %s widget"

29 "error de dtksh"

30 "aviso de dtksh"

31 "Uso:   %s pantalla ventana"

32 "Uso:   %s pantalla ventana variable"

33 "Uso:   %s pantalla Ventanaraíz variable"

34 "El parámetro tiempoexcedido no es válido: %s"

35 "El parámetro mens no es válido: %s"

36 "El parámetro patrones no es válido: %s"

$set 14        Redefine set#


1 "Error en tabla de hash interna durante inicialización de clase de widget; se sale"

2 "No se pudo hallar una clase de widget denominada '%s'"

3 "El identidicador '%s' no es un manejador de widget válido"

4 "No se puede hallar la clase de widget"

5 "Uso:   DtLoadWidget NombreClasewidget NombreRegistroClasewidget"

6 "No se puede localizar el registro de una clase de widget nombrada '%s'"

7 "Error en tabla de hash interna durante inicialización de la clase de widget '%s'"

8 "\n\
%sRESOURCES FOR %s%s%s:\n"

9 "CONSTRAINT "

10 "R"

11 "M"

12 "S"

13 "%-15s %-6s %-6s %-18s %-6s %s\n"

14 "VARIABLE ENT    MANEJ. PADRE  CLASE              ESTADO NOMBRE\n"

15 "\n\
Uso:  \tDtWidgetInfo [Manejadowidget]\n\
\tDtWidgetInfo -r <Manejadorwidget|Nombreclase>\n\
\tDtWidgetInfo -R <Manejadorwidget|Nombreclase>\n\
\tDtWidgetInfo -c [Nombreclase]\n\
\tDtWidgetInfo -h [Manejadorwidget]"

$set 15        Redefine set#


1 "No se puede inicializar el Kit de Utilidades"

2 "El recurso siguiente no se puede establecer en el momento de creación \n\
del widget. En su lugar, use XtSetValues después de la creación: %s"

3 "Uso:   %s variable padre nombre [argumento:valor ...]"

4 "Uso:   DtHelpReturnSelectedWidgetId variable widget variable"

5 "Uso:   DtHelpSetCatalogName Nombrecatálogo"

6 "Uso:   DtHelpQuickDialogGetChild variable WidgetAyudarápida hijo"

7 "El widget debe ser un widget 'Ayudarápida' "

8 "El widget debe ser un widget 'lista' "

9 "Uso:   %s widget posición elemento"

10 "Uso:   %s widget posición Listaelemento"

11 "Uso:   %s widget elemento"

12 "Uso:   XmListDeleteItemsPos widget cuenta posición"

13 "Uso:   XmListDeleteItems widget Listaelemento"

14 "Uso:   XmListDeletePositions widget Listaposición"

15 "Uso:   XmListGetKbdItemPos variable widget"

16 "Uso:   XmListItemExists widget elemento"

17 "Uso:   XmListItemPos variable widget elemento"

18 "Uso:   XmListPosSelected widget posición"

19 "Uso:   XmListPosToBounds widget posición variable variable variable variable"

20 "Uso:   %s widget posición Distintivonotificar"

21 "Uso:   %s widget elemento Distintivonotificar"

22 "Uso:   XmListSetAddMode widget booleano"

23 "Uso:   XmListSetKbdItemPos widget posición"

24 "Uso:   XmMainWindowSetAreas ventanaprincipal menú mandato desplh deslpv trabajo"

25 "El manejador 'Ventanaprincipal' es NULO"

26 "Uso:   %s variable ventanaprincipal"

27 "Uso:   XmProcessTraversal widget dirección"

28 "Dirección transversal desconocida: %s"

29 "Uso:   XmInternAtom variable pantalla nombre sóloSiExiste"

30 "Uso:   XmGetAtomName variable pantalla átomo"

31 "El átomo especificado no es válido: %s"

32 "Uso:   XmGetColors widget fondo primerplano sombrasup sombrainf seleccionar"

33 "El pixel de fondo no es válido: %s"

34 "Uso:   XmUpdateDisplay widget"

35 "Uso:   %s widget protocolo [protocolo ...]"

36 "El átomo especificado no es válido: %s"

37 "Uso:   XmAddWMProtocolCallback widget protocolo mandato-ksh"

38 "Uso:   XmRemoveWMProtocolCallback widget protocolo mandato-ksh"

39 "Uso:   XmMenuPosition menú suceso"

40 "El suceso especificado no es válido: %s"

41 "Uso:   XmCommandAppendValue Widgetmandato serie"

42 "Uso:   XmCommandError Widgetmandato Mensajeerror"

43 "Uso:   XmCommandSetValue Widgetmandato mandato"

44 "Uso:   XmCommandGetChild variable Widgetmandato hijo"

45 "Uso:   XmMessageBoxGetChild variable Widgetmandato hijo"

46 "El widget debe ser un widget 'Cuadromensaje' "

47 "Uso:   XmFileSelectionBoxGetChild variable widget hijo"

48 "El widget debe ser un 'cuadro selección archivo' "

49 "Uso:   XmSelectionBoxGetChild variable widget hijo"

50 "El widget debe ser un widget 'cuadro selección' "

51 "Uso:   XmScaleGetValue Widgetescala variable"

52 "Uso:   XmScaleSetValue Widgetescala valor"

53 "El valor de escala especificado no es válido: %s"

54 "Uso:   XmScrollBarGetValues barradespl variable variable variable variable"

55 "Uso: XmScrollBarSetValues barradespl valor Tamañodeslizador incremento Incrementopágina notificar"

56 "El valor especificado no es válido: %s"

57 "El tamaño del deslizador especificado no es válido: %s"

58 "El incremento especificado no es válido: %s"

59 "El incremento de página especificado no es válido: %s"

60 "Uso:   XmScrollVisible Vendesplazable widget MargenDerIzqu MargenSupInfer"

61 "El widget debe ser un widget 'Ventanadesplazable' "

62 "El widget que se desea hacer visible no existe."

63 "El margen izquierdo/derecho especificado no es válido: %s"

64 "El margen superior/inferior especificado no es válido: %s"

65 "Uso:   %s widget estado notificar"

66 "Uso:   catopen variable Nombrecat"

67 "Uso:   catclose Idcat"

68 "Uso:   catgets variable Idcat Númconj Númmens MensPredet"

69 "El widget debe ser un widget 'texto' o 'campoTexto' "

70 "Uso:   %s variable widget"

71 "Uso:   %s widget booleano"

72 "El parámetro especificado no es válido: %s"

73 "Uso:   %s widget líneas"

74 "Uso:   %s widget Longmáx"

75 "Uso:   %s widget serie"

76 "Uso:   %s widget variable variable"

77 "Uso:   %s widget posición serie"

78 "Uso:   %s widget posición variable variable"

79 "Uso:   %s widget Posicióndesde PosiciónA serie"

80 "La posición 'desde' especificada no es válida: %s"

81 "La posición 'a' especificada no es válida: %s"

82 "Uso:   %s widget Primeraposición Ultimaposición hora"

83 "La primera posición especificada no es válida: %s"

84 "La última posición especificada no es válida: %s"

85 "La hora especificada no es válida: %s"

86 "Uso:   %s variable widget x y"

87 "La posición x especificada no es válida: %s"

88 "La posición y especificada no es válida: %s"

89 "Uso:   %s widget izquierda derecha modalidad"

90 "La posición izquierda especificada no es válida: %s"

91 "La posición derecha especificada no es válida: %s"

92 "Uso:   %s widget inicio serie dirección variable"

93 "La posición inicio especificada no es válida: %s"

94 "Uso:   XmListGetSelectedPos variable widget"

95 "Uso:   XmListGetMatchPos variable widget elemento"

96 "Uso: XmOptionLabelGadget variable widget"

97 "Uso: XmOptionButtonGadget variable widget"

98 "Uso: XmGetVisibility variable widget"

99 "Uso: XmGetTearOffControl variable widget"

100 "Uso: XmGetTabGroup variable widget"

101 "Uso: XmGetPostedFromWidget variable widget"

102 "Uso: XmGetFocusWidget variable widget"

103 "Uso: XmFileSelectionDoSearch widget directoryMask"

$set 16        Redefine set#


1 "DtkshCvtNamedValueToString: No se ha hallado coincidencia"

2 "DtkshCvtStringToNamedValue: No se puede convertir la serie '%s'"
$ The following messages are for ksh93 itself.  The message numbers must    
$ match those specified in the MsgStr array allmsgs in     
$ ksh93/src/cmd/ksh93/sh/init.c, as must the set number.    

$set 25        Redefine set#


1 "Fin"

3 "Ejecución"

4 "[-n] [arg...]"

5 "[arg...]"

6 "[dir] [list]"

7 "[trabajo...]"

8 "[n]"

9 "[nombre [vía] ]"

10 "[nombre]"

11 "[sup.] [base]"

12 "expr..."

13 "format [arg...]"

14 "es una función"

15 "es una palabra clave"

16 "es un shell incorporado"

17 "es una función exportada"

18 "es una función indefinida"

19 "name [arg...]"

20 "optstring name [args...]"

21 "segundos"

23 "${HOME:-.}/.profile"

24 "%c: carácter no válido en expresión - %s"

25 "%c: especificador de formato desconocido"

26 "%d-%d: rango no válido"

27 "%d: versión script binaria no válida"

28 "%s es un alias para"

29 "%s es una alias exportado para"

30 "%s falta"

31 "%s base desconocida"

32 "%s: ':' esperado para operador '?' "

33 "%s: Ambiguo"

34 "%s: Los argumentos deben ser %job o ids de procesos"

35 "%s: alias no encontrado\n"

36 "%s: error sintaxis aritmética"

37 "%s: asignación requiere lvalue"

38 "%s: número unitario de archivo erróneo"

39 "%s: formato erróneo"

40 "%s: número erróneo"

41 "%s: opción(es) errónea(s)"

42 "%s: sustitución errónea"

43 "%s: ruptura errónea"

44 "%s: no se puede crear"

45 "%s: no se puede ejecutar"

46 "%s: no se puede abrir"

47 "%s: dividir por cero"

48 "%s: excepción de dominio"

49 "%s: %s falla"

50 "%s: el archivo ya existe"

51 "%s: nombre de función no permitido"

52 "%s: nombre de alias no válido"

53 "%s: función de disciplina no válida"

54 "%s: nombre exportar no válido"

55 "%s: nombre de función no válido"

56 "%s: nombre no válido"

57 "%s: expresión regular no válida"

58 "%s: auto referencia no válida"

59 "%s: uso no válido de :"

60 "%s: nombre de variable no válido"

61 "%s: no es un identificador"

62 "%s: es de sólo lectura"

63 "%s: etiqueta no implantada"

64 "%s: límite excedido"

65 "%s: esperadas más señales"

66 "%s: sin padre"

67 "%s: sin nombre de referencia"

68 "%s: no encontrado"

69 "%s: no implantado"

70 "%s: operandos con tipos incompatibles"

71 "%s: excepción de desbordamiento"

72 "%s: parámetro no establecido"

73 "%s: parámetro nulo o no establecido"

74 "%s: repetición demasiado profunda"

75 "%s: la variable de referencia no puede ser una matriz"

76 "%s: requiere argumento de vía de acceso"

77 "%s: restringido"

78 "%s: excepción de singularidad"

79 "%s: subíndice fuera de rango"

80 "%s: paréntesis no compensados"

81 "%s: función desconocida"

82 "%s: entorno local desconocido"

83 "%s: operador desconocido"

84 "%s: nombre de señal desconocido"

85 "%s: ocasionaría un bucle"

86 "(coredump)"

87 "-c requiere argumento"

88 "-e - requiere argumento simple"

89 "/vpix"

90 "<mandato desconocido>"

91 "ACE#?F#?H:[nombre]L#?R#?Z#?fi#?[base]lnprtux [name=[valor]...]"

92 "AE#?F#?HL#?R#?Z#?fi#?[base]lnprtux [name=[valor]...]"

93 "Cancelar"

94 "Ad:[delim]prst#[timeout]u#[númarch] [nombre...]"

95 "Llamada a alarma"

96 "Especificación de nodo raíz errónea"

97 "Llamada al sistema errónea"

98 "Conducto roto"

99 "Error de bus"

100 "No se puede iniciar el control de trabajo"

101 "Configuración opción actual"

102 "Señal DIL"

103 "Muerte del hijo"

104 "DircabefhkmnpstuvxCI:[archivo]o:?[opción]A:[nombre] [arg...]"

105 "DircabefhkmnpstuvxCo:?[opción]A:[nombre] [arg...]"

106 "Ruptura EMT"

107 "Excedido límite tiempo de CPU"

108 "Excedido límite tamaño de archivo"

109 "Excepción flotante"

110 "HSacdfmnstv [límite]"

111 "En suspenso"

112 "Señal IO"

113 "Instrucción no permitida"

114 "Interrumpir"

115 "Matado"

116 "LP [dir] [cambiar]"

117 "Error memoria"

118 "Proceso de migrar"

119 "Sin control de trabajos"

120 "Interrumpir teléfono"

121 "Alarma sondeo"

122 "Anomalía alimentación"

123 "Alarma de tiempo de perfilación"

124 "Abandonar"

125 "Recursos perdidos"

126 "Volviendo a unidad tty antigua..."

127 "S [máscara]"

129 "Cambiado nivel de seguridad"

130 "Interrupción de zócalo"

131 "Sonido completado"

132 "Detenido (señal)"

133 "Detenido (entrada tty)"

134 "Proceso detenido continua"

135 "Detenido"

136 "Detenido(salida tty)"

137 "Conmutando a un nuevo controlador de tty..."

138 "El sistema caerá pronto"

139 "Terminado"

140 "Rastreo/Ruptura BPT"

141 "Versión no reconocida"

142 "Utilice 'salir' para terminar este shell"

143 "Señal usuario 1"

144 "Señal usuario 2"

145 "Versión no definida"

146 "Alarma tiempo virtual"

147 "Cambiar tamaño ventana"

148 "Tiene trabajos en ejecución"

149 "Ha detenido trabajos"

150 "[_[:alpha:]]*([_[:alnum:]])"

151 "\n\
@(#)Version 12/28/93\0\n"

152 "\n\
@(#)Version M-12/28/93\0\n"

153 "\n\
real"

154 "\r\n\
\007shell excederá el tiempo de espera dentro de 60 segundos por inactividad"

155 "un nombre..."

156 "a:c [mandato [args...] ]"

157 "nombre afpv..."

158 "alarm %s %.3f\n"

159 "alarm -r %s +%.3g\n"

160 "esperado argumento"

161 "directorio erróneo"

162 "número unitario de archivo erróneo"

163 "sustitución errónea"

164 "no se puede acceder a directorios padres"

165 "no se puede crear conducto"

166 "no se puede crear archivo temporal"

167 "no se puede efectuar fork"

168 "no se puede obtener %s"

169 "no se puede establecer %s"

170 "no se puede establecer alarma"

171 "condición(es) requerida(s)"

172 "dsf:[biblioteca] [nombre...]"

173 "e:[editor]lnrsN# [primero] [último]"

174 "fin de archivo"

175 "f:[formato]nprsu:[númarch] [arg...]"

176 "fnv nombre..."

177 "hist -e \"${VISUAL:-${EDITOR:-vi}}\" "

178 "no se puede abrir archivo histórico"

179 "sintaxis incorrecta"

180 "argumento de tipo %c no válido"

181 "es una versión de shell incorporada de"

182 "es un alias de seguimiento de vía para"

183 "kill"

184 "línea %d: $ no precedida por \n\
 "

185 "línea %d: %c dentro de ${} debe estar acotada"

186 "línea %d: etiqueta %s desconocida"

187 "línea %d: %s dentro [[...]] obsoleto, usar ((...))"

188 "línea %d: '=' obsoleto, usar '=='"

189 "línea %d: -a obsoleto, usar -e"

190 "línea %d: \\ delante de %c reservado para uso futuro"

191 "línea %d: `...` obsoleto, usar $(...)"

192 "línea %d: escape %c para evitar ambiguedades"

193 "línea %d: ignorada etiqueta %s"

194 "línea %d: acote %c para evitar ambiguedades"

195 "línea %d: conjunto %s obsoleto"

196 "línea %d: requeridos espacios para subshell anidada"

197 "línea %d: utilice corchetes para evitar ambiguedades con $id[...]"

198 "línea %d: utilice espacio o tab para separar los operadores %c y %c"

199 "ln#[númseñal]s:[nombseñal] sig..."

200 "inicio de sesión de shells setuid/setgid prohibido"

201 "establecer correspondencias"

202 "nueva línea"

203 "nlp [trabajo...]"

204 "sin archivo histórico"

205 "sin proceso de consulta"

206 "no existe dicho trabajo"

207 "no existe dicho proceso"

208 "no soportado"

209 "desactivar"

210 "activar"

211 "abrir archivo límite excedido"

212 "sin memoria"

213 "p [acción condición...]"

214 "p [name[=valor]...]"

215 "parámetro no establecido"

216 "permiso denegado"

217 "el proceso ya existe"

218 "ptx [name=[valor]...]"

219 "pvV nombre [arg]..."

220 "r [nombrevar segundos]"

221 "error sintaxis en línea %d: `%s' %s"

222 "error sintaxis en línea %d: etiqueta duplicada %s"

223 "error sintaxis: `%s' %s"

224 "sys"

225 "tiempo excedido esperando entrada"

226 "no esperado"

227 "universo no accesible"

228 "ilimitado"

229 "no coincidente"

230 "usuario"

231 "versiones"

232 "ha fallado grabar en %d"

233 "tiene correo en $_"

234 "byte cero"
