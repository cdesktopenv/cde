$ $TOG: SUNW_TOOLTALK.msg /main/5 1999/09/16 13:43:40 mgreess $
$quote "

$set 1

3 "clnt_create para rpc.ttdbserverd en %s ha fallado%s"

4 "Error: rpc.ttdbserverd en %s no está en ejecución"

6 "versión servidor (%s) no coincide con la versión de las tablas de base de datos (%s). Ejecutar una versión %s de servidor de bases de datos"

7 "Ha fallado el intento de abrir la tabla de base de datos %s:%s"

14 "no se pudo suprimir la propiedad de ventana raíz X %s que controla esta sesión"

15 "¡la versión de dirección es %d, pero yo sólo puedo entender %d! (dirección: <%s>)"

17 "no se pudo analizar el nombre de pantalla X: \"%s\""

18 "no se pudo poner en cola un mensaje en el archivo \"%s\" a causa de un error interno %d"

19 "no se pudo suprimir una sesión desusada <%s> de la lista de interés del archivo \"%s\" a causa de un error interno %d"

20 "no se pudo suprimir una sesión no interesada <%s> de la lista de interés del archivo \"%s\" a causa de un error interno %d"

21 "error de sintaxis de tttracefile en la línea:\n\
%s"

22 "No se pudo procesar tttracefile en $%s"

23 "No se permite el rastreo si el UID real no es igual al UID efectivo"

24 "Aceptando petición"

25 "documento vacío y sin archivo"

26 "no se puede obtener messageID; puede que la operación no sea cancelable, a causa de tt_message_arg_val()"

27 "no se puede obtener título; el documento no tendrá título a causa de tt_message_arg_val()"

28 "desplazamiento opnum desconocido"

$set 2


2 "se han encontrado certificaciones para ptype %s que no se pueden distinguir -"

3 "Sin transporte orientado a conexión"

4 "Se ha perdido un cliente ToolTalk antes de poderle indicar que recuperase un mensaje que estaba vencido"

5 "Se ha perdido la conexión con el cliente ToolTalk cuando se le indicaba que recuperase un mensaje"

6 "Se ha encontrado otra sesión en ejecución (host=%s, pid=%d)"

7 "No se puede establecer contacto con la sesión determinada (host=%s, pid=%d); se asume que se ha interrumpido..."

8 "No se puede analizar el id de sesión controlada; se sobreescribe con el mio..."

9 "%s es una versión %d de base de datos y esta versión sólo puede leer las versiones %d y anteriores"

10 "no se ha podido descodificar los tipos de la base de datos de tipos: %s. Puede estar dañada."

11 "Se ha sobreescrito %s"

12 "Se ignora atributo desconocido <%s> de certificado de ToolTalk..."

13 "ttce2xdr ha fallado (estado=%d); tipos de base de datos de \"usuario\" de Classing Engine no convertidos..."

14 "registro ISAM duplicado"

15 "archivo ISAM no abierto"

16 "argumento ISAM no válido"

17 "descriptor de clave ISAM no válido"

18 "demasiados archivos ISAM abiertos"

19 "formato de archivo ISAM erróneo"

20 "acceso no exclusivo a ISAM"

21 "registro ISAM bloqueado"

22 "la clave ISAM ya existe"

23 "la clave ISAM es la clave primaria"

24 "fin o principio del archivo ISAM"

25 "no se ha encontrado ningún registro ISAM"

26 "no hay registro ISAM actual"

27 "archivo ISAM bloqueado"

28 "nombre del archivo ISAM demasiado largo"

29 "ISAM no puede asignar memoria"

30 "tiempo de espera ISAM RPC"

31 "Conexión TCP/IP ISAM interrumpida"

32 "No se puede conectar al servidor de ISAM"

33 "No se pueden importar datos ISAM"

34 "no hay ningún daemon SAM local"

35 "error grave interno de ISAM"

36 "No coincide Entorno nacional ISAM/LANG"

$set 3


2 "se ha salido del hijo (%d) a causa de la señal %d%s"

3 " (imagen de memoria volcada)"

4 "¡no se ha salido del hijo (%d) ni se ha señalado!"

5 "no se puede maximizar clientes a causa de %m"

6 "Ningún ámbito a gestionar. Utilice -c, -p, -d o establezca $DISPLAY."

7 "No se puede utilizar la opción -S con -c"

8 "se ha salido de ttsession hija con el estado %d"

9 "se ha salido de ttsession hija a causa de la señal %d%s"

10 " (imagen de memoria volcada)"

11 "¡no se ha salido de ttsession hija ni se ha señalado!"

12 "iniciando"

13 "tiene tipos de relectura"

14 "error en los tipos; guardando los tipos antiguos"

15 "$OPENWINHOME no establecido"

16 "\n\
Uso: ttsession [-a unix|des][-d pantalla][-spStvhNX"

17 "][-c mandato]\n\
 -c [mandato]\tinicia una sesión de árbol de procesos y ejecuta \n\
             \tel mandato en ella.\n\
\t\tLas opciones subsiguientes se pasan al mandato. Predeterminado: $SHELL\n\
 -p\t\tinicia una sesión de árbol de procesos e imprime su id\n\
 -d pantalla\tinicia una sesión X en la pantalla\n\n\
 -a unix|des\testablece el nivel de autentificación del servidor\n\
 -s\t\tsilent. No se imprime ningún aviso\n\
 -S\t\tno se ejecuta fork en el fondo\n\
 -N\t\tmaximiza el número de clientes permitidos\n\
 -t\t\tactiva el rastreo de mensajes\n\
 -X\t\tutiliza bases de datos XDR para tipos estáticos (predeterminado)\n"

18 " -E\t\tutiliza Classing Engine para tipos estáticos\n"

19 "\n\
 -v\t\timprime el número de versión\n\
 -h\t\timprime este mensaje\n\n\
Interfaz de señales:\n\
 kill -USR1 ttsession_pid\tconmuta el rastreo de mensajes\n\
 kill -USR2 ttsession_pid\tvuelve a leer los tipos estáticos"

20 "saliendo"

$set 4


2 "Uso:\n\
tt_type_comp [-s] [-d db] [-mM] archivo_fuente\n\
tt_type_comp [-s] [-d db] -r tipo ...\n\
-M\tfusiona los tipos fuente en la base de datos especificada, sin  \n\
  \tactualizar los tipos existentes\n\
-m\tfusiona, pero actualiza los tipos existentes. Predeterminado.\n\
-r\telimina tipos fuente de la base de datos especificada\n\
-d db\tbase de datos con la que trabajar. Una de: usuario, sistema o red. \n\
     \tPredeterminado: usuario\n\
-G\trealiza recolección de basura en el servidor de bases de datos ToolTalk.\n\n\
tt_type_comp [-sE] -p|O|P [-d db]\n\
tt_type_comp [-s]  -p|O|P archivo_compilado\n\
-O\tenumera en stdout los nombres de todos los otypes leídos\n\
-P\tenumera en stdout los nombres de todos los ptypes leídos\n\
-p\timpresión de calidad en stdout de todos los tipos de ToolTalk leídos\n\
-E\tutiliza la(s) base(s) de datos Classing Engine en lugar de la(s) \n\
  \tbase(s) de datos XDR\n\
-d db\tbase de datos en la que leer. Una de: usuario, sistema o red. \n\
     \tPredeterminada: todas\n\n\
tt_type_comp [-s] -x [-o archivo_compilado] archivo_fuente\n\
-x\tcompila tipos del archivo_fuente (o stdin, si el archivo es \"-\")\n\
-o\tgraba tipos compilados en archivo_compilado (o stdout, si \n\
  \tel archivo es \"-\")\n\
\tPredeterminado: archivo_fuente.xdr o \"types.xdr\" si el fuente es stdin\n\n\
tt_type_comp [-hv]\n\
-v\timprime el número de versión\n\
-h\timprime este mensaje\n\
-s\tno imprime ningún mensaje de estado.\n\n\
Estas opciones cpp se pasarán mediante:\n\
        -undef -Dname -Idirectory -Uname -Ydirectory"

3 "Error semántico en archivo de tipos"

4 "No es un archivo de tipos ToolTalk válido"

5 "No se puede leer ningún tipo ToolTalk en la base de datos de Classing Engine"

6 "Ya no se da soporte a la fusión de las tablas de Classing Engine"

7 "No se puede leer los tipos de la base de datos %s - versiones no coincidentes"

8 "No se pueden leer los tipos de la base de datos %s"

9 "$OPENWINHOME no establecido"

10 "No se puede inicializar la base de datos %s para grabación"

11 "No se ha podido eliminar la definición antigua de %s"

12 "Sobreescribiendo"

13 "Grabando"

14 "No se ha podido añadir una nueva definición para %s"

15 "No se ha podido eliminar la definición antigua de %s"

16 "Sobreescribiendo"

17 "Grabando"

18 "No se ha podido añadir una nueva definición para %s"

19 "Versiones no coincidentes en tipos compilados"

20 "No se puede leer los tipos de la base de datos"

21 "$OPENWINHOME no establecido"

22 "No se puede leer los tipos de la base de datos %s - versiones no coincidentes"

23 "No se puede leer los tipos de la base de datos %s"

24 "$OPENWINHOME no establecido"

25 "Eliminando otype %s\n"

26 "Eliminando ptype %s\n"

27 "salida grabada en %s\n"

28 "Base de datos no válida: %s"

29 "Especifique una sola de las opciones -O -P -m -M -p -r -x"

30 "no se ha realizado ningún preproceso porque: %s: %s"

31 "ciclo de herencia de otype en relación a %s"

32 "ancestro %s de otype %s no existe"

33 "ptype %s no existe, pero se ha denominado como implementorby otype %s"

34 "múltiples descriptores definidos en otype %s"

35 "certificado heredado en otype %s no existe en el padre"

36 "dos ptypes, %s y %s, con el mismo descriptor"

37 "ancestro %s de otype %s no existe"

38 "jerarquía heredada de otype cíclico -\n\
  {"

39 "definiciones duplicadas de otype %s"

40 "definiciones duplicadas de ptype %s"

$set 5


3 "Violación de seguridad: La llamada a RPC quería abrir un archivo que no es una base de datos de ToolTalk"

4 "Se ignorarán los datos grabados utilizando un rpc.ttdbserverd antiguo (<= 1.0.2) después de utilizar un rpc.ttdbserverd nuevo (>= 1.1)"

5 "Se ignorará cualquier dato grabado utilizando un rpc.ttdbserverd antiguo (<= 1.0.2) después de utilizar un rpc.ttdbserverd nuevo (>= 1.1)"

6 "la versión de rpc.ttdbserverd (%s) no coincide con la versión (%s) de las tablas de bases de datos. Por favor, instale una versión %s (o posterior) de rpc.ttdbserverd"

7 "Uso:\n\
rpc.ttdbserverd [-S] [-n] [-m DTMOUNTPOINT]\n\
rpc.ttdbserverd [-S] [-v]"

8 "No es posible iniciar el paso de recolección de basura. thr_create()\n"

9 "No es posible ejecutar el proceso fork() para la recolección de basura.\n"

$set 6


2 "Uso:\n\
ttdbck [-f archivo] [-k claveobj] [-t tipo] [-bx] \n\
[-impa] [-IZ] [-F nombrearchivonuevo] [-T tiponuevo] [puntosmontaje]\n"

3 "ttdbck: debe especificar una opción de selección [-fkt] o una opción de\n\
diagnóstico [-bx] si se especifica una opción de reparación [-FTZ]\n"

4 "Versiones no coincidentes en tipos compilados"

5 "No se pueden leer los tipos de la base de datos"

6 "ttdbck: intente 'ttdbck -I'.\n"

7 "Seleccionar por nombre de archivo: %s\n"

8 "Seleccionar por clave de idobj:"

9 "Seleccionar por tipo: %s\n"

10 "Diagnosticar entidades formadas erróneamente\n"

11 "Diagnosticar referencias a entidades no existentes\n"

12 "Mostrar los id\n"

13 "Mostrar datos obligatorios\n"

14 "Mostrar datos de propiedades y valores\n"

15 "Invocar la función NetISAM isrepair() antes de inspeccionar\n"

16 "Reparar estableciendo en tipo: %s\n"

17 "Reparar suprimiendo\n"

18 "Nivel de impresión de depuración %d\n"

19 "Reparar estableciendo en archivo: %s\n"

20 "Error: ningún archivo para spec.\n"

21 "Error: ningún tipo para spec.\n"

22 "Error: \"%s\" no es un otype instalado.\n"

23 "Error: la especificación tiene múltiples valores para la propiedad de tipo.\n"

24 "Error: "

25 "ttdbck: no se han encontrado errores.\n"

26 "Error: "

$set 7


2 "Uso: : %s {ctx}[fhpPv[v]] [archivotar] nombreruta ...\n\
       %s {ctx}fL[hpPRv[v]] archivotttar nombreruta ...\n\
       %s -v\n\
       %s -h\n"

3 "\tc       crea un archivo archivador\n\
\tt       lista el contenido de un archivo archivador\n\
\tx       extrae de un archivo archivador\n\
\tf       utiliza arg <archivotar> siguiente como archivo archivador\n\
\th       sigue enlaces simbólicos\n\
\tL       no invoca tar(1)\n\
\tp       conserva modos de archivo\n\
\tP       (root) no conserva el propietario, la hora de modificación, etc. \n\
\t         del objeto\n\
\tR       no repite en directorios\n\
\tv       detallado\n\
\tvv      muy detallado\n\
\t-v      imprime el número de versión y abandona\n\
\t-h[elp] imprime este mensaje\n"

4 "%s: No se ha podido leer la clase de objeto en el flujo de datos de archivar.\n"

5 "%s: No se ha podido leer la versión del archivo archivador en el flujo de datos de archivar.\n"

6 "%s: Se ha encontrado la versión %d de archivo archivador, pero se esperaba la versión %d.\n"

7 "%s: se ha encontrado objeto de clase no conocida %d en archivo archivador.\n"

8 "%s: No se ha podido leer la clase de objeto en el flujo de datos de archivar.\n"

9 "%s: No se ha podido leer la versión del archivo archivador en el flujo de datos de archivar.\n"

10 "%s: Se ha encontrado la versión %d de archivo archivador, pero se esperaba la versión %d.\n"

11 "%s: se ha encontrado objeto de clase desconocida %d en archivo archivador.\n"

$set 8


2 "%s: No se han podido eliminar los objetos ToolTalk de %s a causa de %s\n"

3 "%s: No se han podido desplazar los objetos ToolTalk de \"%s\" a \"%s\" a causa de %s\n"

4 "%s: No se intentará desplazar los objetos ToolTalk de:\n"

5 "Uso:   %s [-] [-fL] ruta1 ruta2\n\
       %s [-] [-fL] ruta1 [ruta2 ...] dir\n\
       %s -v\n\
       %s -h\n"

6 "\t-L      no realiza un mv(1)\n\
\t-v      imprime el número de versión y abandona\n\
\t-h      imprime este mensaje\n"

7 "%s: No se han podido eliminar los objetos ToolTalk de %s a causa de %s\n"

8 "Uso:   %s [-] [-%s] %s ...\n\
       %s -v\n\
       %s -h\n"

9 "archivo"

10 "dir"

11 "\t-L      no realiza un %s(1)\n\
\t-v      imprime el número de versión y abandona\n\
\t-h[elp] imprime este mensaje\n"

12 "%s: No se han podido eliminar los objetos ToolTalk de %s a causa de %s\n"

13 "Uso:   %s [-pL] archivo1 archivo2\n\
       %s [-prRL] ruta1 [ruta2 ...] dir\n\
       %s -v\n\
       %s -h\n"

14 "\t-L      no realiza un cp(1)\n\
\t-v      imprime el número de versión y abandona\n\
\t-h      imprime este mensaje\n"

$set 9


2 "Uso:   %s [-0FCa][-o archivosalida] [-S sesión | mandato [opciones]]\n\
       %s [-e script | -f archivoscript][-S sesión | mandato [opciones]]\n\
 -0\t\tDesactiva el rastreo de mensajes en la sesión o ejecuta el mandato\n\
\t\tsin el rastreo de mensajes (p.e. sólo rastreo de API)\n\
 -F\t\tSigue todos los hijos en los que el mandato ha ejecutado fork\n\
\t\to que ttsession(1) ha iniciado con posterioridad en la sesión\n\
 -C\t\tNo rastrea las llamadas a la API de ToolTalk\n\
 -a\t\tImprime todos los atributos, argumentos y ranuras de contexto de\n\
\t\tmensajes rastreados. Está predeterminado un resumen de una sola línea.\n\
 -e script\tLee los valores de tttracefile(4) del script\n\
 -f archivoscript\tLee los valores de tttracefile(4) en el archivoscript. \n\
                 \t\"-\": stdin.\n\
 -o archivosalida\tSalida. \"-\": stdout. predeterminada: stdout para \n\
                 \trastreo de sesión.\n\
\t\tstderr (de tttrace) para rastreo de mandatos\n\
 -S sesión\tSesión a rastrear. Predeterminada: consulte el mandato \n\
          \ttt_default_session()\n\
\tmandato de cliente ToolTalk a invocar y rastrear\n"

3 "%s: la sesión <%s> no da soporte a Session_Trace. Utilice kill -USR1 en su lugar. Consulte ttsession(1).\n"

$set 10


2 "Ignorando Tt_address desconocida pasada a Tts_address::add()\n"

3 "Ignorando Tt_address desconocida pasada a Tts_address::remove()\n"

4 "Ignorando Dirección desconocida pasada a tts_address::tts_set()\n"

5 "Ignorando Dirección desconocida pasada a tts_address::tts_unset()\n"

6 "Ignorando Dirección desconocida leída por tts_address::load()\n"

7 "Tts_arglist::Tts_arglist - error al leer argumento - \n\
\t"

8 "Tts_arglist::load - Encontrado tipo de arg desconocido - utilizando BARG\n"

9 "LLamada clase abstracta Tts_arg::assign (Tt_pattern) - se ignora\n"

10 "Llamada clase abstracta Tts_arg::assign(Tt_message) - se ignora\n"

11 "LLamada clase abstracta Tts_arg::generate - se ignora\n"

12 "Llamada clase abstracta Tts_arg::save - se ignora\n"

13 "Llamada clase abstracta Tts_arg::load - se ignora\n"

14 "Modo Tts_arg::mode_to_chars no válido - se utiliza TT_INOUT\n"

15 "Modo Tts_arg::chars_to_mode no válido - se utiliza TT_INOUT\n"

16 "Se ignora Tt_category desconocida pasada a Tts_category::set()\n"

17 "Tts_category::generate() llamada para un mensaje - ignorando\n"

18 "Se ignora categoría desconocida leída por tts_category::load()\n"

19 "Se ignora Tt_class desconocida pasada a Tts_class::add()\n"

20 "Se ignora Tt_class desconocida pasada a Tts_class::remove()\n"

21 "Se ignora clase desconocida pasada a tts_class::tts_set()\n"

22 "Se ignora clase desconocida pasada a tts_class::tts_unset()\n"

23 "Se ignora clase desconocida leída por tts_class::load()\n"

24 "Tts_connection::universal_callback - ¡no hay modelo de patrón para patrón!\n"

25 "Tts_cntxtlist::Tts_cntxtlist - error al leer contexto -\n\
\t"

26 "Tts_contextlist::load - Encontrado tipo cntxt desconocido - se utiliza BCNTXT\n"

27 "Llamada clase abstracta Tts_context::assign (Tt_pattern) - se ignora\n"

28 "LLamada clase abstracta Tts_context::assign (Tt_pattern) - se ignora\n"

29 "Lllamada clase abstracta Tts_context::generate - se ignora\n"

30 "Llamada clase abstracta Tts_context::save - se ignora\n"

31 "Llamada clase abstracta Tts_context::load llamada - se ignora\n"

32 "Se ignora Tt_disposition desconocida pasada a Tts_disposition::add()\n"

33 "Se ignora Tt_disposition desconocida pasada a Tts_disposition::remove()\n"

34 "Se ignora Disposición desconocida pasada a tts_disposition::tts_set()\n"

35 "Se ignora Disposición desconocida pasada a tts_disposition::tts_unset()\n"

36 "Se ignora Disposición desconocida leída por tts_disposition::load()\n"

37 "%s llamada\n"

38 "Error de ToolTalk: %s\n\
        Origen: Archivo Fuente %s Línea %d\n"

39 "Aviso: %s\n\
        Origen: Archivo Fuente %s Línea %d\n"

40 " Tts_message_molding::send - Intento de enviar modelo de mensaje no inicializado\n"

41 "Se ignora Tt_scope desconocido pasado a Tts_scope::add()\n"

42 "Se ignora Tt_scope desconocido pasado a Tts_scope::remove()\n"

43 "Se ignora Ámbito desconocido pasado a tts_scope::tts_set()\n"

44 "Se ignora Ámbito desconocido pasado a tts_scope::tts_unset()\n"

45 "Se ignora Ámbito desconocido leído por tts_scope::load()\n"

46 "Se ignora Tt_state desconocido pasado a Tts_state::add()\n"

47 "Se ignora Tt_state desconocido pasado a Tts_state::remove()\n"

48 "Se ignora Estado desconocido pasado a tts_state::tts_set()\n"

49 "Se ignora Estado desconocido pasado a tts_state::tts_unset()\n"

50 "Tts_state::generate() llamada para un mensaje - se ignora\n"

51 "Se ignora estado Desconocido leído por tts_state::load()\n"

82 "Obtención de Ayuda TTSnoop"

83 "--------------------------"

84 "La ayuda para botones y configuraciones individuales se puede obtener"

85 "pulsando la tecla de ayuda mientras el ratón está encima del botón o"

86 "la configuración interés. Para ver que llamadas de api utiliza"

87 "ttsnoop, utilice el conmutador -t al invocar a ttsnoop."

88 ""

89 "Utilización de TTSnoop"

90 "----------------------"

91 "Para observar los mensajes seleccione la opción Iniciar configuración."

92 ""

93 "Para desactivar la observación de mensajes seleccione la opción Detener configuración."

94 ""

95 "Para limitar los tipos de mensajes:"

96 "      1) Seleccione el botón Patrones"

97 "      2) Entre el tipo de patrón que desee observar"

98 "      3) Seleccione el botón Aplicar"

99 ""

100 "Para resaltar la información que se muestra:"

101 "      1) Seleccione el botón Mostrar"

102 "      2) Marque los elementos que desee resaltar"

103 "      3) Seleccione el botón Aplicar"

104 ""

105 "Para enviar un mensaje:"

106 "      1) Seleccione el botón Mensajes"

107 "      2) Componga el mensaje"

108 "      3) Seleccione el botón Enviar mensaje"

109 ""

110 "Para almacenar un mensaje:"

111 "      1) Seleccione el botón Mensaje"

112 "      2) Componga el mensaje"

113 "      3) Seleccione el botón Añadir mensaje"

114 ""

115 "Para enviar un mensaje almacenado:"

116 "      Seleccione el mensaje en el menú Enviar mensaje"

117 ""

118 "Para borrar la ventana de salida de mensajes seleccione el botón Borrar."

119 ""

120 "Información General de ToolTalk"

121 "-------------------------------"

122 "Antes de enviar o de recibir mensajes orientados"

123 "a objetos mediante ToolTalk, se debe registrar"

124 "un proceso con el transmisor de mensajes. Al registrarse,"

125 "el proceso le indica a ToolTalk:"

126 ""

127 "      El tipo de proceso (ptype) del proceso."

128 "      Esto permite al transmisor de mensajes dirigir"

129 "      los mensajes implantados por dicho ptype a"

130 "      este proceso."

131 ""

132 "      Las sesiones en las que participa el proceso."

133 ""

134 "      Los documentos que el proceso está"

135 "      observando. Entonces, los mensajes para los objetos de"

136 "      estos documentos se pueden direccionar al"

137 "      proceso."

138 ""

139 "Tenga en cuenta que estos registros suelen modificar"

140 "los patrones generados. Sería posible utilizar"

141 "las llamadas a la API de manipulación de patrones para observar o"

142 "para tratar los mensajes orientados a objetos, pero los métodos"

143 "que se describen aquí son más cortos ya que"

144 "aprovechan la información declarada en las definiciones"

145 "de tipos."

146 ""

147 "Utilice tt_session_join para entrar a formar parte de una sesión."

148 "Al unirse, debe proporcionar el id de sesión de"

149 "la sesión que se ha de unir."

150 ""

151 "Cuando la herramienta ya no necesite los servicios de ToolTalk,"

152 "abandone la sesión con tt_session_quit."

153 ""

154 "Cuando la herramienta carga un archivo, debe unirse a la"

155 "colección de procesos interesados en dicho archivo"

156 "llamando a tt_file_join(); cuando haya terminado con el"

157 "archivo, abandone la colección llamando"

158 "a tt_file_quit(). Algunas herramientas pueden tener varios archivos"

159 "abiertos a la vez; otras herramientas sólo pueden abrir uno."

160 "Utilización de TTSnoop"

161 "----------------------"

162 "Para observar mensajes seleccione la opción Iniciar configuración."

163 "Para desactivar la observación de mensajes seleccione la opción Detener configuración."

164 ""

165 "Para limitar los tipos de mensajes:"

166 "        1) Seleccione el botón Patrones"

167 "        2) Entre el tipo de patrón que desee observar"

168 "        3) Seleccione el botón Aplicar"

169 ""

170 "Para resaltar la información que se muestra:"

171 "        1) Seleccione el botón Mostrar"

172 "        2) Marque los elementos que desee resaltar"

173 "        3) Seleccione el botón Aplicar"

174 ""

175 "Para enviar un mensaje:"

176 "        1) Seleccione el botón Mensajes"

177 "        2) Componga el mensaje"

178 "        3) Seleccione el botón Enviar mensaje"

179 ""

180 "Para almacenar un mensaje:"

181 "        1) Seleccione el botón Mensaje"

182 "        2) Componga el mensaje"

183 "        3) Seleccione el botón Añadir mensaje"

184 ""

185 "Para enviar un mensaje almacenado:"

186 "        Seleccione el mensaje en el menú Enviar mensaje"

187 ""

188 "Para borrar la ventana de salida de mensaje seleccione el botón Borrar."

189 ""

190 "Información General de ToolTalk"

191 "-------------------------------"

192 "Antes de enviar o de recibir mensajes orientados a objetos mediante"

193 "ToolTalk, se debe registrar un proceso con el transmisor de mensajes."

194 "Al registrarse, el proceso indica a ToolTalk:"

195 ""

196 "        El tipo de proceso (ptype) del proceso. Esto permite"

197 "        al transmisor de mensajes dirigir los mensajes implantados por"

198 "        dicho ptype a este proceso."

199 ""

200 "        Las sesiones en las que participa el proceso."

201 ""

202 "        Los documentos que el proceso está observando. Los mensajes"

203 "        para los objetos de estos documentos se pueden direccionar al"

204 "        proceso."

205 ""

206 "Tenga en cuenta que estos registros suelen modificar los patrones generados."

207 "Sería posible utilizar las llamadas a la API de manipulación de patrones para"

208 "observar o para manejar los mensajes orientados a objetos, pero los métodos"

209 "que se describen aquí son más cortos ya que aprovechan la"

210 "información declarada en las definiciones de tipos."

211 ""

212 "Utilice tt_session_join para entrar a formar parte de una sesión. Al unirse,"

213 "debe proporcionar el id de sesión de la sesión a la que se une."

214 ""

215 "Cuando la herramienta ya no necesite los servicios de ToolTalk, abandone la sesión"

216 "con tt_session_quit."

217 ""

218 "Cuando la herramienta carga un archivo, debe unirse a la colección de"

219 "procesos interesados en dicho archivo llamando a tt_file_join(); cuando"

220 "haya terminado con el archivo, abandone la colección llamando a tt_file_quit()."

221 "Algunas herramientas pueden tener varios archivos abiertos a la vez; otras"

222 "herramientas sólo pueden tener uno."

223 "Iniciar/Detener el recibir y mostrar mensajes."

224 "Borrar la ventana de mensajes."

225 "Mostrar"

226 "-------"

227 "Cuando llega un mensaje para el proceso, se activa un descriptor de archivo."

228 "Dependiendo de la estructura determinada de la herramienta, puede"

229 "disponer que el kit de herramientas del sistema de ventanas llame a información"

230 "de retorno cuando el descriptor de archivo se active o incluir el descriptor de archivo"

231 "en una llamada select(3) que bloquee varios descriptores de archivo."

232 "En cualquier caso, después de que se active un descriptor de archivo, se llama"

233 "a tt_message_receive() para obtener un manejador de un mensaje que contenga"

234 "el mensaje de entrada. Puede utilizar las llamadas tt_message_*() para examinar"

235 "los atributos del mensaje para determinar la acción que debe llevar a cabo."

236 "Puede reconocer las respuestas a los mensajes que envíe comparando"

237 "los manejadores, colocando la información significativa para la aplicación en"

238 "el mensaje con la llamada tt_message_user_set() o colocando"

239 "información de retorno específica de mensajes y patrones con las"

240 "llamadas tt_message_callback_add y tt_pattern_callback_add."

241 "Mensajes"

242 "--------"

243 "Para enviar un mensaje, primero asigne un nuevo mensaje, especifique la información"

244 "adecuada, después envíela. Cualquier respuesta aparecerá en el mismo mensaje."

245 "Cuando haya terminado con el mensaje, libere el almacenamiento para el mensaje. (para"

246 "peticiones de retorno de valores, es después de que cualquier valor de retorno"

247 "necesario se copie; para notificaciones, es después de que se hayan enviado)."

248 ""

249 "Para asignar el nuevo mensaje, utilice tt_message_create(). Devuelve un"

250 "\"selector\" o \"puntero opaco\" al mensaje; utilice este selector en"

251 "llamadas satisfactorias para hacer referencia a este mensaje."

252 ""

253 "Para especificar la información del mensaje, utilice las llamadas tt_message_*_set();"

254 "hay una de ellas para cada atributo del mensaje."

255 ""

256 "Debe establecer Clase, Dirección, Op y Args."

257 ""

258 "Envíe el mensaje con tt_message_send()."

259 "Patrones"

260 "--------"

261 "Como el proceso remitente no diríge explícitamente los mensajes a un destinatario"

262 "en particular, se utiliza un método \"coincidencia de patrones\" para determinar"

263 "el destinatario. Las herramientas registran las descripciones de los mensajes en los"

264 "que están interesadas y ToolTalk utiliza estos descriptores para inferir el"

265 "direccionamiento del mensaje. Se guardan conjuntos separados de patrones para describir"

266 "los mensajes que la herramienta desea manejar y los mensajes que la herramienta desea"

267 "observar. Las herramientas que desean recibir mensajes declaran su interés"

268 "dinámicamente en tiempo de ejecución o estáticamente en tiempo de instalación."

269 "Un registro dinámico consiste en un conjunto de patrones con los que se comparan todos"

270 "los mensajes. Si un mensaje coincide con el patrón, la herramienta que ha "

271 "registrado el patrón se puede elegir para recibir el mensaje."

272 ""

273 "Antes de recibir mensajes de procedimiento mediante ToolTalk, debe registrarse"

274 "un proceso con el transmisor de mensajes. Al registrarse, el proceso da"

275 "patrones a ToolTalk; entonces, este entrega los mensajes que coinciden con"

276 "dichos patrones al proceso. Los patrones se pueden crear de dos maneras:"

277 ""

278 "Estáticamente. Mediante un ptype. Un proceso puede declarar su ptype al"

279 "transmisor de mensajes; entonces, ToolTalk genera patrones de cada certificado"

280 "en el ptype. Estos patrones generados se pueden modificar uniendo y"

281 "abandonando sesiones y archivos, como en la transmisión de mensajes orientados a objetos."

282 ""

283 "Dinámicamente. Un proceso puede crear patrones \"al vuelo\" y registrarlos"

284 "con ToolTalk. Una utilización típica de esta característica sería una utilidad"

285 "de registro de mensajes que simplemente observa todos los mensajes que pasan, "

286 "mostrándolos en una ventana; dicha utilidad sería útil en los protocolos de depuración"

287 "de mensajes."

288 ""

289 "Esta aplicación sólo utiliza los patrones Dinámicos."

290 ""

291 "Para registrar un patrón, primero debe asignar un nuevo patrón, rellenar"

292 "la información adecuada, después registrarlo. Cuando haya terminado con"

293 "el patrón (que sólo será cuando no esté interesado en mensajes"

294 "que coincidan con él), libere el almacenamiento para el patrón."

295 ""

296 "Para asignar el nuevo patrón, utilice tt_pattern_create(). Devuelve un"

297 "\"selector\" o \"puntero opaco\" al patrón; utilice este selector en"

298 "las llamadas satisfactorias para hacer referencia a este patrón."

299 ""

300 "Para especificar la información del patrón, utilice las llamadas tt_pattern_*_add();"

301 "hay una de ellas para cada atributo del patrón. Es posible"

302 "suministrar múltiples valores para cada atributo del patrón; el atributo de"

303 "patrón se empareja con un atributo de mensaje si ningún valor del"

304 "patrón coincide con el valor del mensaje."

305 ""

306 "Esta aplicación utiliza una configuración no exclusiva y valores separados por comas"

307 "para los múltiples atributos."

308 ""

309 "Para emparejar mensajes sin tener en cuenta el valor de un atributo en particular,"

310 "omita simplemente el atributo del patrón."

311 ""

312 "Si no se especifica ningún atributo de patrón, el servicio de ToolTalk cuenta"

313 "el atributo de mensaje como emparejado. Cuantos menos atributos de patrón especifique,"

314 "podrá ser elegido para recibir más mensajes."

315 ""

316 "Si se han especificado varios valores para un atributo de patrón, uno de los"

317 "valores debe coincidir con el valor de atributo de mensaje. Si ningún valor coincide,"

318 "el servicio de ToolTalk no considerará la aplicación como un destinatario."

319 ""

320 "Siempre se deben suministrar los atributos Categoría y Ámbito."

321 ""

322 "Cuando el patrón esté completado, registre el patrón con"

323 "tt_pattern_register() y después una sesiones o archivos cuando lo necesite"

324 "utilizando SELECT (selección) en el botón Aplicar."

325 ""

326 "Como el primer efecto de unir archivos y sesiones es la actualización"

327 "de los patrones registrados actualmente, los patrones que se registran después de"

328 "de las uniones no se actualizarán. Debe registrar todos los patrones"

329 "antes de unir o volver a realizar las uniones después de registrar nuevos patrones."

330 "Esto también se aplica a los patrones registrados implícitamente por la unión de ptypes."

331 "Al añadir mensajes mediante la ventana Mensajes, el menú conectado a este"

332 "botón crece. Puede enviar un mensaje seleccionándolo."

333 "Dirección:"

334 "Como hay muchos tipos de herramientas y los distintos usuarios utilizarán"

335 "conjuntos diferentes de herramientas en momentos distintos, a menudo es imposible"

336 "que el remitente de un mensaje identifique al destinatario exacto de un mensaje."

337 "En su lugar, el remitente da un nombre de operación que especifia el significado"

338 "del mensaje y puede dar un objeto al cual va dirigido el mensaje"

339 "o el tipo de objeto (otype)."

340 ""

341 "Utilice el objeto u otype para los mensajes orientados a objetos. Si la dirección"

342 "es un descriptor, especifique el descriptor."

343 ""

344 "        TT_PROCEDURE: Op y Args suministrados."

345 ""

346 "        TT_OBJECT:    Objeto, Op y Args suministrados."

347 ""

348 "        TT_HANDLER:   Manejador, Op y Args suministrados."

349 ""

350 "        TT_OTYPE:     Otype, Op y Args suministrados."

351 "Manejador:"

352 "Si conoce el id de proceso exacto del descriptor, puede direccionarle mensajes"

353 "directamente. La manera normal sería que un proceso"

354 "hiciese una petición general y después sacar el atributo Manejador fuera de la"

355 "respuesta, dirigiendo los mensajes posteriores al mismo manejador; esto permite"

356 "que se encuentren dos procesos mediante la transmisión de mensajes generales y"

357 "después ir a un diálogo."

358 "Handler_Ptype:"

359 "Si conoce el ptype del proceso que manejará el mensaje, rellénelo."

360 "Conocería el ptype si consultase la definición de ptype"

361 "para buscar los detalles del protocolo."

362 "Objeto:"

363 "Especifique el id del objeto (objid) al cual se ha de enviar el mensaje."

364 "OType:"

365 "Tipo del objeto."

366 "Op:"

367 "Especifique la operación que describe la notificación o petición"

368 "que se está realizando. Consulte en la definición de otype el objeto de destino"

369 "para determinar el nombre de la operación."

370 "Opnum:"

371 "Número de operación. Se utiliza para distinguir entre operaciones con"

372 "carga excesiva (operaciones con el mismo nombre pero con tipos de argumentos"

373 "diferentes) y para simplificar el trabajo de envío \"interno\" de un cliente"

374 "(invocando el procedimiento interno adecuado para un mensaje determinado.)"

375 "Ambito:"

376 "Utilice la sesión de ámbito si desea mensajes de otros procesos en la misma"

377 "sesión que el proceso; utilice el archivo de ámbito si desea mensajes acerca"

378 "de un archivo."

379 ""

380 "      TT_SESSION:          Recibe mensajes de otros procesos en"

381 "                           la sesión."

382 ""

383 "      TT_FILE:             Recibe mensajes acerca del archivo al que se ha unido."

384 ""

385 "      TT_BOTH:             Recibe mensajes sobre un archivo y"

386 "                           la sesión."

387 ""

388 "      TT_FILE_IN_SESSION:  Recibe mensajes para el archivo al que se ha unido"

389 "                           durante esta sesión."

390 "Sesión:"

391 "Para una sesión de ámbito, especifique la sesión (tt_default_session()"

392 "predeterminada)."

393 "Archivo:"

394 "Para ámbito de archivo, especifique el nombre del archivo."

395 "Categoría:"

396 "Utilice observar categorías si sólo desea mirar los mensajes; utilice"

397 "selector de categorías para ser el manejador exclusivo del mensaje."

398 ""

399 "    TT_OBSERVE: La observación de procesos sólo obtiene copias del mensaje para"

400 "                su propia información. Pueden observar un mensaje cualquier"

401 "                cantidad de procesos. Incluso si el mensaje es una petición,"

402 "                los observadores no pueden devolver valores al remitente. A menudo,"

403 "                la acción que llevan a cabo los observadores sólo afecta a la"

404 "                pantalla interactiva de los datos de la herramienta básica."

405 ""

406 "    TT_HANDLE:  Los procesos de manejo efectúan realmente una acción basada en"

407 "                el mensaje. Sólo un proceso manejará un mensaje"

408 "                determinado. Si el mensaje es una petición, el proceso"

409 "                de manejo es el proceso que devuelve los valores."

410 "                Generalmente, la acción que lleva a cabo el manejador afecta a la"

411 "                representación almacenada permanentemente de los datos de la herramienta."

412 "Clase:"

413 "Utilice la petición de clase para los mensajes que devuelven valores o para los"

414 "que desee que devuelvan información indicando cuándo se maneja o se pone en cola el"

415 "mensaje o cuando se inicia un proceso para manejar la petición. Utilice la notificación"

416 "de clase para los mensajes que sólo notifican otros procesos de sucesos."

417 ""

418 "    TT_NOTICE:   Las notas son mensajes que informan a otras herramientas que se ha"

419 "                 producido un suceso. Las herramientas que envían notas no esperan"

420 "                 respuestas a las mismas; el remitente sólo da a conocer"

421 "                 a las demás herramientas lo que está sucediendo."

422 ""

423 "     TT_REQUEST: Las peticiones son mensajes que piden a otra herramienta que"

424 "                 lleven a cabo una acción. A menudo, pero no siempre,"

425 "                 la herramienta que la pide espera la devolución de algún valor."

426 "                 Este valor se devuelve como una respuesta a la petición. Incluso"

427 "                 si no se devuelve ningún valor, la herramienta que procesa"

428 "                 la petición envía una respuesta indicando el éxito o el fracaso."

429 "Disposición:"

430 "Especifica la acción a llevar a cabo si ningún proceso en ejecución puede"

431 "manejar el mensaje. Cola si el mensaje debe ponerse en cola hasta que un proceso"

432 "Handler_Ptype registre. Iniciar si se debe iniciar un proceso del"

433 "Handler_Ptype."

434 ""

435 "    TT_QUEUE: Pone en cola el mensaje hasta que un proceso del ptype adecuado"

436 "              reciba el mensaje."

437 ""

438 "    TT_START: Intenta iniciar un proceso del ptype adecuado"

439 "              si no hay ninguno ejecutándose."

440 ""

441 "Observe que los valores de Tt_disposition se pueden sumar, de modo"

442 "que TT_QUEUE+TT_START significa poner en cola el mensaje e intentar iniciar"

443 "un proceso. Puede ser útil si el inicio puede fracasar (o que el usuario"

444 "lo vete), para asegurarse de que se procesa el mensaje en cuanto se inicia"

445 "un proceso elegible."

446 "Estado:"

447 "Valores para el atributo Estado de un mensaje. Los valores posibles y sus"

448 "significados son:"

449 ""

450 "    TT_CREATED:  Se ha creado el mensaje pero todavía no se ha envíado. Sólo"

451 "                 el remitente de un mensaje verá un mensaje en este estado."

452 ""

453 "    TT_SENT:     Se ha enviado el mensaje pero todavía no se ha manejado."

454 ""

455 "    TT_HANDLED:  Se ha manejado el mensaje, los valores de retorno son válidos."

456 ""

457 "    TT_FAILED:   No se ha podido entregar el mensaje a un manejador."

458 ""

459 "    TT_QUEUED:   Se ha puesto en cola el mensaje para una entrega posterior."

460 ""

461 "    TT_STARTED:  Se intenta iniciar un proceso para manejar el mensaje."

462 ""

463 "    TT_REJECTED: Un posible manejador ha rechazado el mensaje."

464 "                 Este estado sólo lo ve el proceso del rechazo;"

465 "                 ToolTalk vuelve a establecer el estado en TT_SENT antes"

466 "                 de entregar el mensaje a otro posible manejador."

467 "Estado:"

468 "Un número exclusivo indicando el resultado del mensaje que devuelve"

469 "la respuesta. Este número debe ser mayor que TT_ERR_LAST (2047)."

470 "Cadena de estado:"

471 "Un texto descriptivo del Estado del mensaje."

472 "Remitente:"

473 "Identifica el proceso que ha enviado el mensaje."

474 "Ptype del remitente:"

475 "El ptype del proceso que ha enviado el mensaje."

476 "Uid:"

477 "El identificador del usuario efectivo del proceso que ha enviado el mensaje."

478 "Gid:"

479 "El identificador del grupo efectivo del proceso que ha enviado el mensaje."

480 "Modo:"

481 "Especifica el modo de un argumento de mensaje. Los posibles valores y"

482 "significados son:"

483 ""

484 "    TT_IN:    El remitenter escribe el argumento y lo lee el manejador"

485 "              y cualquier observador."

486 ""

487 "    TT_OUT:   El manejador escribe el argumento y lo lee"

488 "              el remitente y cualquier observador de la respuesta."

489 ""

490 "    TT_INOUT: El remitente escribe y lee el argumento y"

491 "              el manejador y cualquier observador."

492 "Tipo:"

493 "Cadena que indica el tipo. Los valores comunes son \"stringa\" e \"int\""

494 "aunque los programadores pueden crear nuevos."

495 "Valor:"

496 "Datos reales del valor que depende del tipo."

497 "Cuando se pulsa el botón aplicar, se mostrarán los valores de la configuración"

498 "especificada para cada mensaje recibido."

499 "Cuando se pulsa, los valores de la pantalla se restablecen en el estado de la última"

500 "acción de aplicar o en el valor inicial, el que sea el último."

501 "Editar contexto de recepción:"

502 "Añadir, suprimir o cambiiar el contexto que desea recibir."

503 "Emparejador de patrones"

504 "-----------------------"

505 "Como el proceso remitente no direcciona explícitamente los mensajes a un"

506 "destinatario en particular, se utiliza un método de \"coincidencia de patrones\" para"

507 "determinar el destinatario. Las herramientas registran descripciones de los"

508 "mensajes en los que están interesadas y ToolTalk utiliza dichas descripciones para"

509 "deducir el direccionamineto del mensaje. Se guardan conjuntos de patrones independientes se"

510 "para describir los mensajes que la herramienta desea manejar y los mensajes"

511 "que la herramienta desea observar. Las herramientas que desean recibir mensajes"

512 "declaran su interés dinámicamente en tiempo de ejecución o estáticamente"

513 "en el tiempo de instalación. Un registro dinámico consiste en un conjunto de"

514 "patrones con los que se comparan todos los mensajes. Si un mensaje coincide"

515 "con el patrón, se puede elegir la herramienta que ha registrado el patrón para"

516 "que reciba el mensaje."

517 ""

518 "Antes de recibir los mensajes de procedimiento a través de ToolTalk, debe registrarse"

519 "un proceso con el transmisor de mensajes. Al registrarse, el proceso da"

520 "los patrones a ToolTalk; entonces, este entrega los mensajes que coinciden con"

521 "los patrones al proceso. Los patrones se pueden crear de dos maneras:"

522 ""

523 "Estáticamente. Mediante un ptype. Un proceso puede declarar su ptype al"

524 "transmisor de mensajes; entonces, ToolTalk genera patrones de cada certificación"

525 "en el ptype. Estos patrones generados se puede modificar uniéndose y"

526 "abandonando sesiones y archivos, como en la transmisión de mensajes orientados a objetos."

527 ""

528 "Dinámicamente. Un proceso puede crear patrones \"al vuelo\" y registrarlos"

529 "con ToolTalk. Una utilización típica de este recurso sería una utilidad de registro"

530 "de mensajes que simplemente observa todos los mensajes que pasan, mostrándolos"

531 "en una ventana; dicha utilidad sería útil en la depuración de los protocolos"

532 "de mensajes."

533 ""

534 "Esta aplicación únicamente utiliza los patrones Dinámicos."

535 ""

536 "Para registrar un patrón, primero debe asignar un nuevo patrón, especificar"

537 "la información adecuada y después registrarlo. Cuando haya terminado con el"

538 "patrón (que sólo será cuando ya no esté interesado en los mensajes que coincidan"

539 "con él), libere el alamcenamiento para del patrón."

540 ""

541 "Para asignar un nuevo patrón, utilice tt_pattern_create(). Devuelve"

542 "un \"selector\" o \"puntero opaco\" al patrón; utilice este selector en"

543 "llamadas satisfactorias para hacer referencia a este patrón."

544 ""

545 "Para especificar la información del patrón, utilice las llamadas tt_pattern_*_add();"

546 "hay una de ellas para cada atributo del patrón. Es posible"

547 "suministrar varios valores para cada atributo del patrón;"

548 "el atributo del patrón coincide con un atributo del mensaje si cualquier valor"

549 "del patrón coincide con el valor del mensaje."

550 ""

551 "Esta aplicación utiliza una configuración no exclusiva y valores separados por comas"

552 "para múltiples atributos."

553 ""

554 "Para emparejar mensajes sin tener en cuenta el valor de un atributo en particular,"

555 "omita simplemente el atributo del patrón."

556 ""

557 "Si no se especifica ningún atributo de patrón, el servicio ToolTalk cuenta el"

558 "atributo de mensaje como emparejado. Cuantos menos atributos de patrón especifique,"

559 "más mensajes podrán ser elegidos para recibir."

560 ""

561 "Si hay varios valores especificados para un atributo de patrón, uno de"

562 "los valores debe coincidir con el valor del atributo del mensaje. Si no coincide"

563 "ningún valor, el servicio ToolTalk no considerará destinataria a su aplicación."

564 ""

565 "Los atributos Categoría y Ámbito deben suministrarse siempre."

566 ""

567 "Cuando esté completo el patrón, registre el patrón con"

568 "tt_pattern_register() y después una las sesiones o los archivos necesarios"

569 "mediante la utilización de SELECT (seleccionar) el botón Aplicar."

570 ""

571 "Como el efecto primario de unir archivos y sesiones es la actualización"

572 "de los patrones registrados actualmente, los patrones que se registran después de"

573 "las uniones no se actualizarán. Debe registrar todos los patrones"

574 "antes de unir o de volver a realizar las uniones después de registrar nuevos patrones."

575 "Esto se aplica a los patrones registrados implícitamente uniendo ptypes,"

576 "también."

577 "Dirección:"

578 "Como hay muchos tipos de herramientas y distintos usuarios utilizarán"

579 "conjuntos de herramientas diferentes en distintos momentos, a menudo es imposible"

580 "que el remitente de un mensaje identifique al destinatario exacto de un mensaje."

581 "En su lugar, el remitente da un nombre de operación que especifica el significado"

582 "del mensaje y puede dar un objeto al que se dirigir el mensaje"

583 "o el tipo de objeto (otype)."

584 ""

585 "Utilice object o otype para los mensajes orientados a objetos. Si la dirección"

586 "es un manejador, especifique el manejador."

587 ""

588 "    TT_PROCEDURE: Op y Args suministrados."

589 ""

590 "    TT_OBJECT:    Objeto, Op y Args suministrados."

591 ""

592 "    TT_HANDLER:   Manejador, Op y Args suministrados."

593 ""

594 "    TT_OTYPE:     Otype, Op y Args suministrados."

595 "Objeto:"

596 "Especifique el id del objeto (objid) al que se ha de enviar el mensaje."

597 "OType:"

598 "Tipo del objeto."

599 "Op:"

600 "Especifique la operación que describe la notificación o petición"

601 "que se está llevando a cabo. Consulte en la definición de otype el objeto de destino"

602 "para determinar el nombre de la operación."

603 "Ambito:"

604 "Utilice la sesión de ámbito si se desean los mensajes de otros procesos en la misma"

605 "sesión que el proceso; utilice el archivo de ámbito si se desean los mensajes"

606 "acerca de un archivo."

607 ""

608 "    TT_SESSION:          Recibe mensajes de otros procesos en"

609 "                         la sesión."

610 ""

611 "    TT_FILE:             Recibe mensajes acerca del archivo unido."

612 ""

613 "    TT_BOTH:             Recibe mensajes acerca de un archivo y de la sesión."

614 ""

615 "    TT_FILE_IN_SESSION:  Recibe los mensajes para el archivo unido"

616 "                         durante esta sesión."

617 "Archivo:"

618 "Para ámbito de archivo, especifique el nombre del archivo."

619 "Sesión:"

620 "Para sesión de ámbito, especifique la sesión (tt_default_session()"

621 "predeterminada)."

622 "Categoría:"

623 "Utilice observar categorías si sólo desea mirar los mensajes; utilice"

624 "manejar categorías para prestarse a ser el manejador exclusivo del mensaje."

625 ""

626 "    TT_OBSERVE: Los procesos de observación sólo obtienen copias del mensaje para"

627 "                información. Pueden observar un mensaje cualquier cantidad"

628 "                de procesos. Incluso cuando el mensaje es una petición,"

629 "                los observadores no pueden devolver valores al remitente. A menudo,"

630 "                la acción que llevan a cabo los observadores sólo afecta a la"

631 "                pantalla interactiva de los datos de la herramienta básica."

632 ""

633 "    TT_HANDLE:  Los procesos de manejo efectúan realmente una acción basada en"

634 "                el mensaje. Sólo un proceso manejará un mensaje"

635 "                determinado. Si el mensaje es una petición, el proceso"

636 "                de manejo es el proceso que devuelve los valores."

637 "                Generalmente, la acción que lleva a cabo el manejador afecta a la"

638 "                representación almacenada permanentemente de los datos de la herramienta."

639 "Clase:"

640 "Utilice la petición de clase en los mensajes que devuelven valores o para los que"

641 "desea que devuelvan información indicando si se maneja o se pone en cola el mensaje o"

642 "cuando se inicia un proceso para manejar la petición. Utilice la notificación de clase"

643 "en los mensajes que sólo notifican otros procesos de sucesos."

644 ""

645 "    TT_NOTICE:  Las notas son mensajes que informan a otras herramientas que"

646 "                se ha producido algún suceso. Las herramientas que envían notas"

647 "                no esperan respuestas; el remitente sólo da a conocer a las demás"

648 "                herramientas lo que está sucediendo."

649 ""

650 "    TT_REQUEST: Las peticiones son mensajes que piden a otra herramienta que"

651 "                ejecute una acción. A menudo, pero no siempre, la herramienta que realiza"

652 "                la petición espera la devolución de algún valor. Este valor se devuelve"

653 "                como una respuesta a la petición. Incluso si no se devuelve ningún"

654 "                valor, la herramienta que procesa la petición envía una respuesta"

655 "                indicando el éxito o el fracaso."

656 "Estado:"

657 "Valores para el atributo Estado de un mensaje. Los posibles valores y"

658 "sus significados son:"

659 ""

660 "    TT_CREATED:  Se ha creado el mensaje pero todavía no se ha enviado. Sólo"

661 "                 el remitente de un mensaje verá un mensaje en este estado."

662 ""

663 "    TT_SENT:     Se ha enviado el mensaje pero todavía no se ha manejado."

664 ""

665 "    TT_HANDLED:  Se ha manejado el mensaje, los valores de retorno son válidos."

666 ""

667 "    TT_FAILED:   No se ha podido entregar el mensaje al manejador."

668 ""

669 "    TT_QUEUED:   Se ha puesto en cola el mensaje para una entrega posterior."

670 ""

671 "    TT_STARTED:  Se intenta iniciar un proceso para manejar el mensaje."

672 ""

673 "    TT_REJECTED: Un posible manejador ha rechazado el mensaje."

674 "                 Este estado sólo lo ve el proceso de rechazo;"

675 "                 ToolTalk devuelve el estado a TT_SENT antes"

676 "                 de entregar el mensaje a otro posible manejador."

677 "Disposición:"

678 "Especifica la acción a llevar a cabo si ningún proceso en ejecución puede"

679 "manejar el mensaje. Cola si el mensaje debe ponerse en cola hasta que un proceso"

680 "del Handler_Ptype lo registre. Iniciar si debe iniciarse un proceso del"

681 "Handler_Ptype."

682 ""

683 "    TT_QUEUE: Pone en cola el mensaje hasta que un proceso del ptype adecuado"

684 "              recibe el mensaje."

685 ""

686 "    TT_START: Intenta iniciar un proceso del ptype adecuado si no hay"

687 "              ninguno en ejecución."

688 ""

689 "Tenga en cuenta que los valores de Tt_disposition pueden sumarse, de modo que"

690 "TT_QUEUE+TT_START significa poner en cola el mensaje e intentar iniciar"

691 "un proceso. Puede ser útil si el inicio puede fallar (o que el usuario"

692 "lo vete), para asegurarse de que se procesa el mensaje en cuanto se inicie"

693 "un proceso elegible."

694 "Remitente:"

695 "Identifica el proceso que ha enviado el mensaje."

696 "Ptype del remitente:"

697 "El ptype del proceso que ha enviado el mensaje."

698 "Args:"

699 "Cumplimente los argumentos específicos para la operación. Utilice tt_message_arg_add"

700 "para añadir uno a uno los argumentos. Para cada argumento, debe especificar: modo"

701 "(in, out o inout), tipo, y si el modo es in o inout, el valor."

702 "Modo:"

703 "Especifica el modo de un argumento de mensaje. Los posibles valores y"

704 "significados son:"

705 ""

706 "    TT_IN:    El remitente escribe el argumento y lo leen"

707 "              el manejador y cualquier observador."

708 ""

709 "    TT_OUT:   El manejador escribe el argumento y lo leen"

710 "              el remitente y cualquier observador de la respuesta."

711 ""

712 "    TT_INOUT: El remitente escribe y lee el argumento y el"

713 "              manejador y cualquier observador."

714 "Tipo:"

715 "Cadena que indica el tipo. Los valores comunes son \"string\" e \"int\""

716 "aunque los programadores pueden crear nuevos."

717 "Valor:"

718 "Datos reales del valor que depende el tipo."

719 "Botón Añadir:"

720 "Añade un argumento con el Modo, Tipo y Valor."

721 "Botón Suprimir:"

722 "Suprime el argumento seleccionado."

723 "Botón Cambiar:"

724 "Cambia el argumento seleccionado a los valores actuales de Modo, Tipo"

725 "y Valor."

726 "Botón Aplicar:"

727 "Registra la recepción de mensajes que coinciden con este patrón."

728 "Nombre:"

729 "Nombre de este contexto"

730 "Valor:"

731 "Un valor adecuado para el tipo,"

732 ""

733 "p.e. 55 para un tipo int, \"fred\" para un tipo char o ^V^L^X para"

734 "un tipo bytes."

735 "Añadir contexto:"

736 "Añade los valores actuales de los campos Nombre y Valor a la lista de"

737 "contextos."

738 "Suprimir contexto:"

739 "Suprime la entrada seleccionada de la lista de contextos."

740 "Cambiar contexto:"

741 "Cambia la entrada seleccionada por el contenido de los campos Nombre y Valor."

742 "Editar contextos de envío:"

743 "Ventana emergente para editar la lista de contextos que desee enviar."

744 "Mensaje"

745 "-------"

746 "Para enviar un mensaje, asigne primero un nuevo mensaje, rellene la información"

747 "adecuada y después envíelo. Cualquier respuesta aparecerá en el mismo mensaje."

748 "Cuando haya terminado con el mensaje libere el almacenamiento para el mensaje. (Para"

749 "las peticiones que devuelven valores, es después de que se copie cualquier valor de"

750 "retorno necesario; para las notificaciones, es en cuanto se envían),"

751 ""

752 "Para asignar el nuevo mensaje, utilice tt_message_create(). Devuelve"

753 "un \"selector\" o \"puntero opaco\" al mensaje; utilice este selector"

754 "en las llamadas satisfactorias para hacer referencia a este mensaje."

755 ""

756 "Para especificar la información del mensaje, utilice las llamadas tt_message_*_set();"

757 "hay una de ellas para cada atributo del mensaje."

758 ""

759 "Debe establecer Clase, Dirección, Op y Args."

760 ""

761 "Envíe el mensaje con tt_message_send()."

762 "Dirección:"

763 "Debido a que hay muchos tipos de herramientas y que distintos usuarios utilizarán"

764 "conjuntos diferentes de herramientas en distintos momentos, a menudo es imposible que"

765 "el remitente de un mensaje identifique el destinatario exacto de un mensaje."

766 "En su lugar, el remitente da un nombre de operación que especifica el significado"

767 "del mensaje y puede dar un objeto al cual dirigir el mensaje"

768 "o el tipo de objeto (otype)."

769 ""

770 "Utilice object o otype para mensajes orientados a objetos. Si la dirección es"

771 "un manejador, especifique el manejador."

772 ""

773 "    TT_PROCEDURE: Op y Args suministrados."

774 ""

775 "    TT_OBJECT:    Objeto, Op y Args suministrados."

776 ""

777 "    TT_HANDLER:   Manejador, Op y Args suministrados."

778 ""

779 "    TT_OTYPE:     Otype, Op y Args suministrados."

780 "Manejador:"

781 "Si conoce el id de proceso (procid) exacto del manejador, puede dirigirle mensajes"

782 "directamente. La forma normal en que se produciría sería el de que un"

783 "proceso realizase una petición general y después sacase el atributo Manejador"

784 "fuera de la respuesta, dirigiendo los demás mensajes al mismo manejador; esto"

785 "permite que se encuentren dos procesos mediante la transmisión de mensajes generales"

786 "y ir a un diálogo."

787 "Handler_Ptype:"

788 "Si conoce el ptype del proceso que manejará el mensaje,"

789 "especifíquelo. Conocerá el ptype si consulta la definición de"

790 "ptype para buscar detalles del protocolo."

791 "Objeto:"

792 "Cumplimente el id del objeto al cual se ha de enviar el mensaje."

793 "OType:"

794 "Tipo del objeto."

795 "Op:"

796 "Especifique la operación que describe la notificación o la petición que se está"

797 "realizando. Consulte en la definición de otype el objeto de destino para determinar"

798 "el nombre de la operación."

799 "Ambito:"

800 "Utilice la sesión de ámbito si desea los mensajes de otros procesos en la misma"

801 "sesión que el proceso; utilice el archivo de ámbito si desea los mensajes acerca"

802 "de un archivo."

803 ""

804 "    TT_SESSION:          Recibe mensajes de otros procesos en"

805 "                         la sesión."

806 ""

807 "    TT_FILE:             Recibe mensajes acerca del archivo unido."

808 ""

809 "    TT_BOTH:             Recibe mensajes acerca de un archivo y de la sesión."

810 ""

811 "    TT_FILE_IN_SESSION:  Recibe mensajes para el archivo unido"

812 "                         durante esta sesión."

813 "Sesión:"

814 "Para sesión de ámbito, especifique la sesión (tt_default_session()"

815 "predeterminada)."

816 "Archivo:"

817 "Para ámbito de archivo, especifique el nombre del archivo."

818 "Clase:"

819 "Utilice la petición de clase para los mensajes que devuelven valores o para los que"

820 "desee que devuelvan información indicando si se maneja o se pone en cola el mensaje o"

821 "cuando se inicia un proceso para manejar la petición. Utilice la notificación"

822 "de clase en los mensajes que sólo notifican otros procesos de sucesos."

823 ""

824 "    TT_NOTICE:  Las notas son mensajes que informan a otras herramientas que se ha"

825 "                producido algún suceso. Las herramientas que envían notas no esperan"

826 "                respuestas a las mismas; el remitente sólo da a conocer"

827 "                a las demás herramientas lo que está sucediendo."

828 ""

829 "    TT_REQUEST: Las peticiones son mensajes que piden a otra herramienta que ejecute"

830 "                una acción. A menudo, pero no siempre, la herramienta que realiza la"

831 "                petición espera la devolución de algún valor. Este valor se devuelve"

832 "                como respuesta a la petición. Si no se devuelve ningún valor,"

833 "                la herramienta que procesa la petición envía una respuesta"

834 "                indicando el éxito o el fracaso."

835 "Disposición:"

836 "Especifica la acción a llevar a cabo si ningún proceso en ejecución puede manejar el"

837 "mensaje. Cola si el mensaje debe ponerse en cola hasta que un proceso del"

838 "Handler_Ptype lo registre. Iniciar si debe iniciarse un proceso del"

839 "Handler_Ptype."

840 ""

841 "    TT_QUEUE: Pone en cola el mensaje hasta que un proceso del ptype adecuado"

842 "              recibe el mensaje."

843 ""

844 "    TT_START: Intenta iniciar un proceso del ptype adecuado si no hay"

845 "              ninguno ejecutándose."

846 ""

847 "Tenga en cuenta que los valores de Tt_disposition se pueden sumar, de modo que"

848 "TT_QUEUE+TT_START significa poner en cola el mensaje e intentar iniciar"

849 "un proceso. Puede ser útil si el inicio puede fallar (o que el usuario lo"

850 "vete), para asegurarse de que se procesa el mensaje en cuanto se inicia"

851 "un proceso elegible."

852 "Sender_Ptype:"

853 "El ptype del proceso que ha enviado el mensaje."

854 "Estado:"

855 "Un número exclusivo que indica el resultado del mensaje que devuelve"

856 "el que responde. Este número debe ser mayor que TT_ERR_LAST (2047)."

857 "Cadena de estado:"

858 "Un texto descriptivo del Estado del mensaje."

859 "Args:"

860 "Cumplimente los argumentos específicos para la operación. Utilice"

861 "tt_message_arg_add para añadir uno a uno los argumentos. Para cada argumento,"

862 "debe especificar, modo (in, out o inout), tipo, y si el modo es"

863 "in o inout, el valor."

864 "Modo:"

865 "Especifica el modo de un argumento de mensaje. Los posibles valores y"

866 "significados son:"

867 ""

868 "    TT_IN:    El remitente escribe el argumento y lo leen"

869 "              el manejador y cualquier observador."

870 ""

871 "    TT_OUT:   El manejador escribe el argumento y lo leen"

872 "              el remitente y cualquier observador de la respuesta."

873 ""

874 "    TT_INOUT: El remitente escribe y lee el argumento y "

875 "              el manejador y cualquier observador."

876 "Tipo:"

877 "Cadena que indica el tipo. Los valores comunes son \"string\" e \"int\""

878 "aunque los programadores pueden crear otros nuevos."

879 "Valor:"

880 "Datos reales del valor dependiente del tipo."

881 "Botón Añadir:"

882 "Añade un argumento con el Modo, Tipo y Valor."

883 "Botón Suprimir:"

884 "Suprime el argumento seleccionado."

885 "Botón Cambiar:"

886 "Cambia el argumento seleccionado por los valores actuales de Modo, Tipo"

887 "y Valor."

888 "Botón Enviar:"

889 "Envía este mensaje. También imprime en salida estándar las llamadas a"

890 "ToolTalk que se realizarán para crear y enviar este mensaje."

891 "Nombre:"

892 "Nombre de este contexto."

893 "Valor:"

894 "Valor de este contexto, interpretado como Tipo."

895 "Añadir:"

896 "Añade los valores de los campos de texto como un nuevo contexto."

897 "Suprimir:"

898 "Suprime el contexto seleccionado actualmente."

899 "Cambiar:"

900 "Cambia el contexto seleccionado para reflejar el contenido actual de"

901 "los campos de texto."

902 "No se puede abrir el conducto para salida seleccionada.\n"

903 "Lllamada Tts_string_list:generate para mensaje - se ignora\n"

$set 11


2 "TT_OK\tPetición satisfactoria."

3 "TT_WRN_NOTFOUND\tNo se ha eliminado el objeto porque no se ha encontrado."

4 "TT_WRN_STALE_OBJID\tEl atributo de objeto del mensaje se ha sustituido por uno más reciente. Actualice el lugar del que se ha obtenido el id del objeto."

5 "TT_WRN_STOPPED\tEl procedimiento de filtro ha detenido la consulta."

6 "TT_WRN_SAME_OBJID\tEl objeto que se ha desplazado conserva el mismo objid."

7 "TT_WRN_START_MESSAGE\tEste mensaje ha hecho que se inicie este proceso. Debe responderse a este mensaje aunque sea una notificación."

8 "TT_WRN_APPFIRST\tEste código no debe ser utilizado."

9 "TT_WRN_LAST\tEste código no debe ser utilizado."

10 "TT_ERR_CLASS\tEl valor Tt_class pasado no es válido."

11 "TT_ERR_DBAVAIL\tNo está disponible una base de datos necesaria. La condición puede ser temporal, más tarde puede funcionar."

12 "TT_ERR_DBEXIST\tNo existe una base de datos necesaria. Se debe crear la base de datos para que funcione esta acción."

13 "TT_ERR_FILE\tNo se ha podido encontrar el objeto de archivo."

14 "TT_ERR_MODE\tEl valor Tt_mode no es válido."

15 "TT_ERR_ACCESS\tSe ha intentado acceder a un objeto de ToolTalk de una forma prohíbida por la protección del sistema."

16 "TT_ERR_NOMP\tNo hay ningún proceso de ttsession ejecutándose, probablemente debido a que todavía no se ha llamado a tt_open(). Si se devuelve este código desde tt_open() significa que no se ha podido iniciar la tsession, lo que generalmente significa que ToolTalk no está instalado en este sistema."

17 "TT_ERR_NOTHANDLER\tSólo el manejador del mensaje puede llevar a cabo esto."

18 "TT_ERR_NUM\tEl valor entero pasado no es válido."

19 "TT_ERR_OBJID\tEl id de objeto pasado no hace referencia a una especificación de objeto existente."

20 "TT_ERR_OP\tEl nombre de operación pasada no tiene una sintaxis válida."

21 "TT_ERR_OTYPE\tEl tipo de objeto pasado no es el nombre de un tipo de objeto instalado."

22 "TT_ERR_ADDRESS\tEl valor de Tt_address pasado no es válido."

23 "TT_ERR_PATH\tUno de los directorios de la ruta de archivos pasado no existe o no puede leerse."

24 "TT_ERR_POINTER\tEl puntero opaco (selector) pasado no indica un objeto del tipo correcto."

25 "TT_ERR_PROCID\tEl id de proceso pasado no es válido."

26 "TT_ERR_PROPLEN\tEl valor de propiedad pasado es demasiado largo."

27 "TT_ERR_PROPNAME\tEl nombre de propiedad pasado no tiene una sintaxis válida."

28 "TT_ERR_PTYPE\tEl tipo de proceso pasado no es el nombre de un tipo de proceso instalado."

29 "TT_ERR_DISPOSITION\tEl valor de Tt_disposition pasado no es válido."

30 "TT_ERR_SCOPE\tEl valor de Tt_scope pasado no es válido."

31 "TT_ERR_SESSION\tEl id de sesión pasado no es el nombre de una sesión activa."

32 "TT_ERR_VTYPE\tEl nombre del tipo de valor pasado no es válido."

33 "TT_ERR_NO_VALUE\tNo existe ningún valor de propiedad con el nombre y el número dados."

34 "TT_ERR_INTERNAL\tError interno (informar problema)"

35 "TT_ERR_READONLY\tEl atributo no se puede cambiar."

36 "TT_ERR_NO_MATCH\tNo se ha podidio encontrar ningún manejador para este mensaje y la disposición no se ha puesto en cola ni se ha iniciado. "

37 "TT_ERR_UNIMP\tFunción no implantada."

38 "TT_ERR_OVERFLOW\tDemasiados mensajes activos (vuelva a intentarlo más adelante)."

39 "TT_ERR_PTYPE_START\tHa fallado un intento de ejecutar un caso de ptype."

40 "TT_ERR_CATEGORY\tEl objeto de patrón no tiene establecida la categoría."

41 "TT_ERR_DBUPDATE\tLa base de datos no es coherente: otro tt_spec_write actualizó primero el objeto."

42 "TT_ERR_DBFULL\tLa base de datos de Tooltalk está llena."

43 "TT_ERR_DBCONSIST\tLa base de datos está dañada o la información de acceso es incompleta (ejecute ttdbck)."

44 "TT_ERR_STATE\tEl Tt_message está en Tt_state que no es válido para la operación que se ha intentado."

45 "TT_ERR_NOMEM\tNo hay más memoria."

46 "TT_ERR_SLOTNAME\tEl nombre de ranura no tiene una sintaxis válida."

47 "TT_ERR_XDR\tEl proceso XDR pasado ha evaluado la longitud en cero o ha fallado durante la invocación."

48 "TT_ERR_NETFILE\tEl nombre de archivo de red no tiene una sintáxis válida."

49 "TT_DESKTOP_UNMODIFIED\tLa operación no se aplica a entidades no modificadas"

50 "TT_MEDIA_ERR_SIZE\tEl tamaño especificado era demasiado grande o demasiado pequeño"

51 "TT_MEDIA_ERR_FORMAT\tLos datos no se ajustan a su supuesto formato"

52 "TT_ERR_APPFIRST\tEste código no debe ser utilizado."

53 "TT_ERR_LAST\tEste código no debe ser utilizado."

54 "TT_STATUS_LAST\tEste código no debe ser utilizado."

55 "Valor Tt_status desconocido."

56 "TT_DESKTOP_EPERM\tNo es superusuario"

57 "TT_DESKTOP_ENOENT\tNo existe dicho archivo o directorio"

58 "TT_DESKTOP_EINTR\tLlamada al sistema interrumpida"

59 "TT_DESKTOP_EIO\tError de E/S"

60 "TT_DESKTOP_EAGAIN\tNo hay más procesos"

61 "TT_DESKTOP_ENOMEM\tNo hay suficiente espacio"

62 "TT_DESKTOP_EACCES\tPermiso denegado"

63 "TT_DESKTOP_EFAULT\tDirección errónea"

64 "TT_DESKTOP_EEXIST\tEl archivo existe"

65 "TT_DESKTOP_ENODEV\tNo existe dicho dispositivo"

66 "TT_DESKTOP_ENOTDIR\tNo es un directorio"

67 "TT_DESKTOP_EISDIR\tEs un directorio"

68 "TT_DESKTOP_EINVAL\tArgumento no válido"

69 "TT_DESKTOP_ENFILE\tDesbordamiento de tabla de archivos"

70 "TT_DESKTOP_EMFILE\tDemasiados archivos abiertos"

71 "TT_DESKTOP_ETXTBSY\tArchivo de texto ocupado"

72 "TT_DESKTOP_EFBIG\tArchivo demasiado grande"

73 "TT_DESKTOP_ENOSPC\tNo queda espacio en el dispositivo"

74 "TT_DESKTOP_EROFS\tSistema de archivos de sólo lectura"

75 "TT_DESKTOP_EMLINK\tDemasiados enlaces"

76 "TT_DESKTOP_EPIPE\tConducto interrumpido"

77 "TT_DESKTOP_ENOMSG\tNo hay ningún mensaje del tipo deseado"

78 "TT_DESKTOP_EDEADLK\tCondición de punto muerto"

79 "TT_DESKTOP_ENODATA\tNo hay datos disponibles"

80 "TT_DESKTOP_EPROTO\tError de protocolo"

81 "TT_DESKTOP_ENOTEMPTY\tDirectorio no vacío"

82 "TT_DESKTOP_ETIMEDOUT\tTiempo excedido de conexión"

83 "TT_DESKTOP_EALREADY\tLa operación ya está en proceso"

84 "TT_DESKTOP_ECANCELED\tOperación cancelada"

85 "TT_DESKTOP_ENOTSUP\tNo soportado"

86	"TT_ERR_TOOLATE\tThis must be the first call made into the ToolTalk API and can therefore no longer be performed."

87	"TT_ERR_AUTHORIZATION\tThe user is not authorized to connect to this ToolTalk session."

88	"TT_AUTHFILE_ACCESS\tThe user is not authorized to access the ToolTalk authority file."

89	"TT_AUTHFILE_LOCK\tCannot lock the ToolTalk authority file."

90	"TT_AUTHFILE_LOCK_TIMEOUT\tCannot lock the ToolTalk authority file."

91	"TT_AUTHFILE_UNLOCK\tCannot unlock the ToolTalk authority file."

92	"TT_AUTHFILE_MISSING\tThe ToolTalk authority file is missing."

93	"TT_AUTHFILE_ENTRY_MISSING\tThe ToolTalk authority file is missing an entry for this session."

94	"TT_AUTHFILE_WRITE\tCannot write the ToolTalk authority file."

95	"TT_ERR_VERSION_MISMATCH\tThis client is using a different RPC or program version from the server of this ToolTalk session."
