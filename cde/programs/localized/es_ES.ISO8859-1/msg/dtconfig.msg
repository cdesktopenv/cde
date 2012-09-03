$ $XConsortium: dtconfig.msg /main/6 1996/10/19 14:45:51 rws $
$ *****************************************************************************   
$ **   
$ **  File:        dtconfig.msg   
$ **   
$ **  Project:     CDE Session Management (dtconfig)   
$ **   
$ **  Description:   
$ **  -----------   
$ **  This file is the source for the message catalog for dtconfig   
$ **   
$ **  Message set 10: IBM   
$ **  Message set 20: Sun   
$ **   
$ *****************************************************************************   
$ *****************************************************************************   
$ **  Message set 10: For IBM platforms   
$ *****************************************************************************   
$quote "

$set 10

1 "sólo el superusuario puede ejecutarla.\n"
$    

2 "El entorno CDE está establecido en este momento como interfaz de \n\
usuario por omisión. Esta interfaz aparecerá al iniciar la sesión para\n\
todos los usuarios del sistema. Para ver que este cambio se produce\n\
realmente, deberá cerrar el sistema y rearrancarlo. \n"
$    

3 "La línea de Mandatos está establecida en este momento como interfaz \n\
de usuario por omisión. Esta interfaz aparecerá al iniciar la sesión\n\
para todos los usuarios del sistema. Para ver que este cambio se\n\
produce realmente, deberá cerrar el sistema y rearrancarlo.\n"

4 "El entorno CDE ya está establecido como Interfaz de Usuario por omisión.\n"
$    

5 "La línea de Mandatos ya está establecida como Interfaz de Usuario por\n\
omisión.\n"
$    

6 "uso:   %s\n\
  -e (habilitar inicio automático de dtlogin)\n\
  -d (inhabilitar inicio automático de dtlogin)\n\
  -kill (eliminar dtlogin)\n\
  -reset (restaurar dtlogin - volver a leer los archivos de configuración)\n"
$    

7 "Demasiados argumentos\n"
$    

8 "Necesita un argumento\n"
$    

9 "No se pudo inhabilitar xdm; por favor, consulte las páginas man de xdm \n\
para inhabilitar xdm e inténtelo de nuevo."
$    

10 "Dtlogin no está en ejecución \n"
$ *****************************************************************************   
$ **  Message set 20: For Sun Platforms   
$ *****************************************************************************   

$set 20

$    

1 "Utilidad de configuración de CDE"
$    

2 "%s -d     (inhabilitar inicio automático)"

3 "%s -e     (habilitar inicio automático)"

4 "%s -kill  (eliminar dtlogin)"

5 "%s -reset (restaurar dtlogin)"

6 "%s -p     (actualizar acción de impresora)"
$    
$ In above messages 2 through 6, only translate words between   
$ left "(" and right ").   
$    

7 "%s: este script debe ejecutarse como root"

8 "Uso:   Demasiados argumentos"

9 "Uso:   Se necesita un argumento"

10 "la opción ha fallado"

11 "no se ha podido ejecutar %s"

12 "la inhabilitación ha fallado"

13 "no se ha podido eliminar %s"

14 "hecho"

15 "inhabilitado inicio automático del escritorio."

16 "la habilitación ha fallado"

17 "no se ha podido encontrar %s"

18 "no se ha podido crear %s"

19 "no se ha podido crear %s"

20 "inhabilitado inicio automático del escritorio."

21 "eliminación de dtlogin completada."

22 "se han vuelto a cargar los recursos de configuración de dtlogin."

23 "la actualización de la acción de impresora se ha completado"

24 "Uso:  "
$ 
25 " %s -inetd    (daemons inetd.conf /usr/dt)"
26 " %s -inetd.ow (daemons inetd.conf /usr/openwin)"
$ 
$	In above messages 25 and 26, only translate words between
$	left "(" and right ").
$ 
27 "La próxima inicialización del sistema se ejecutará desde /usr/dt/bin"
$ 
28 "    rpc.ttdbserverd         (ToolTalk)"
29 "    rpc.cmsd                (Gestor del calendario)"
30 "    dtspcd                  (Control de subprocesos)"
$ 
$  Keep column allignment on messages 28 thru 30.  
$  Only translate what's beteen ( )'s
$ 
31 "La próxima inicialización del sistema se ejecutará desde /usr/openwin/bin"
$ 
32 "    rpc.ttdbserverd         (ToolTalk)"
33 "    rpc.cmsd                (Gestor del calendario)"
$ 
$  Keep column allignment on messages 32 thru 33.  
$  Only translate what's beteen ( )'s
$ 
