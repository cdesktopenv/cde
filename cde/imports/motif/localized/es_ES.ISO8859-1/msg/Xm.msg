$ $XConsortium: Xm.msg /main/9 1996/10/19 14:47:51 rws $
$ 
$  @OSF_COPYRIGHT@
$  COPYRIGHT NOTICE
$  Copyright (c) 1990, 1991, 1992, 1993 Open Software Foundation, Inc.
$  ALL RIGHTS RESERVED (MOTIF). See the file named COPYRIGHT.MOTIF for
$  the full copyright text.
$ 

$ This message catalog contains messages issued by Motif toolkit library.
$ Consult X-Window systems, OSF/MOTIF Programmer's Guide and OSF/MOTIF
$ Programmer's Reference Manual for technical terms if you have any
$ doubts about their meanings.
$ Do not translate variables such as %s, %d %s ... etc.
$ Do not translate Motif resource name, such as XmNlabelType.
$ Special terms with capital letter(s) should not be translated.
$ "False" and "True" are keywords. Do not translate.
 
$quote " 


$set MS_ArrowButton

MSG_ArrowButton_1000 "La dirección de la flecha no es correcta."
$ MSG_ArrowButton_1000 message is obsolete - DO NOT localize this message.

$set MS_BulletinB

MSG_BulletinB_1000 "Política de redimensión incorrecta."
$ MSG_BulletinB_1000 message is obsolete - DO NOT localize this message.

$ MSG_BulletinB_0001 "Estilo de diálogo incorrecto."
MSG_BulletinB_0001 "El estilo de diálogo debe ser XmDIALOG_MODELESS."

MSG_BulletinB_1001 "Tipo de sombra incorrecto."
$ MSG_BulletinB_1001 message is obsolete - DO NOT localize this message.

MSG_BulletinB_1002 "Lista de fonts nula (no hay shell proveedor predeterminado)"
$ MSG_BulletinB_1002 message is obsolete - DO NOT localize this message.


$set MS_CascadeB

MSG_CascadeB_1000 "XmCascadeButton debe tener el tipo de \n\
padre XmRowColumnWidgetClass correcto."
$ MSG_CascadeB_1000 message is obsolete - DO NOT localize this message.

$ MSG_CascadeB_0001 "Solamente XmMENU_PULLDOWN XmRowColumnWidgets pueden ser submenús."
MSG_CascadeB_0001 "Solamente widgets XmRowColumn de tipo XmMENU_PULLDOWN pueden ser submenus."
$ please do not translate submenus. 

MSG_CascadeB_0002 "XmNmappingDelay debe ser mayor o igual que 0."

MSG_CascadeB_0000 "XmCascadeButton[Gadget] debe contar con un padre XmRowColumn con \n\
XmNrowColumnType XmMENU_PULLDOWN, XmMENU_POPUP, XmMENU_BAR o XmMENU_OPTION."

MSG_CascadeB_0003 "XtGrabPointer ha fallado."

MSG_RowColText_0024 "XtGrabKeyboard ha fallado."


$set MS_Command

MSG_Command_0000 "El tipo de diálogo debe ser XmDIALOG_COMMAND."

MSG_Command_0001 "Tipo de hijo no válido.\n\
El widget Command no tiene este hijo."
$ please do not translate Command.

MSG_Command_0002 "XmString NULL o vacía."

MSG_Command_0003 "Se ha enviado una XmString NULL o vacía a XmCommandAppendValue."

MSG_Command_0004 "XmNmustMatch es siempre False para un widget Command."
$ please do not translate False or Command. 

MSG_Command_0005 "XmNhistoryMaxItems debe ser un entero positivo mayor que cero."


$set MS_CutPaste

MSG_CutPaste_0000 "Debe llamarse a XmClipboardStartCopy() antes que a XmClipboardCopy()"

MSG_CutPaste_0001 "Debe llamarse a XmClipboardStartCopy() antes que a XmClipboardEndCopy()"

MSG_CutPaste_0002 "Demasiados formatos en XmClipboardCopy()"

MSG_CutPaste_0003 "ClipboardBadDataType"
$ please do not translate ClipboardBadDataType.

MSG_CutPaste_0004 "Tipo de datos incorrecto"

MSG_CutPaste_0005 "ClipboardCorrupt"
$ please do not translate ClipboardCorrupt.

MSG_CutPaste_0006 "Error interno. La estructura de datos está dañada"

MSG_CutPaste_0007 "ClipboardBadFormat"
$ please do not translate ClipboardBadFormat

MSG_CutPaste_0008 "Error. La longitud de formato registrado debe ser 8, 16 ó 32"

MSG_CutPaste_0009 "Error. El nombre de formato registrado no debe ser nulo"


$set MS_DialogS

MSG_DialogS_0000 "El widget DialogShell sólo soporta un hijo RectObj"
$ please do not translate DialogShell and RectObj.

MSG_DialogS_1000 "Los gadgets no se permiten en Shell"
$ MSG_DialogS_1000 message is obsolete - DO NOT localize this message.


$set MS_DrawingA

MSG_DrawingA_1001 "La anchura o altura del margen no puede ser negativa."
$ MSG_DrawingA_1001 message is obsolete - DO NOT localize this message.

MSG_DrawingA_1002 "Política de redimensión incorrecta."
$ MSG_DrawingA_1002 message is obsolete - DO NOT localize this message.


$set MS_Form

MSG_Form_0000 "La base de la fracción no puede ser cero."

MSG_Form_1000 "Tipo de conexión de formato incorrecto."
$ MSG_Form_1000 message is obsolete - DO NOT localize this message.

MSG_Form_1001 "No se pueden establecer restricciones para un widget no redimensionable."
$ MSG_Form_1001 message is obsolete - DO NOT localize this message.

MSG_Form_1002 "El widget Attachment no debe ser nulo."
$ MSG_Form_1002 message is obsolete - DO NOT localize this message.
$ please do not translate Attachment

MSG_Form_0002 "Dependencia circular de los hijos del widget Form.\n\
Verifique si existen enlaces circulares entre los hijos."
$ please do not translate Form. 

MSG_Form_1006 "Borde conectado a un widget pero no se ha especificado ningún widget."
$ MSG_Form_1006 message is obsolete - DO NOT localize this message.

MSG_Form_0003 "Se abandonó la sincronización del borde después de 10000 iteraciones.\n\
Verifique si existen limitaciones contradictorias para los hijos de este widget Form."
$ please do not translate Form.

MSG_Form_0004 "El widget Attachment debe tener el mismo padre como widget."
$ please do not translate Attachment
$ MSG_Form_0004 message is obsolete - DO NOT localize this message.


$set MS_Frame

MSG_Frame_1000 "En un marco solamente debe insertarse un hijo."
$ please do not translate Frame.
$ MSG_Frame_1000 message is obsolete - DO NOT localize this message.

MSG_Frame_1001 "Anchura de margen no válida."
$ MSG_Frame_1001 message is obsolete - DO NOT localize this message.

MSG_Frame_1002 "Altura de margen no válida."
$ MSG_Frame_1002 message is obsolete - DO NOT localize this message.


$set MS_Gadget

MSG_Gadget_1000 "Grosor de resaltado no válido."
$ MSG_Gadget_1000 message is obsolete - DO NOT localize this message.

MSG_Gadget_1001 "El tipo de unidad no es correcto."
$ MSG_Gadget_1001 message is obsolete - DO NOT localize this message.

MSG_Gadget_1002 "Grosor de sombra no válido."
$ MSG_Gadget_1002 message is obsolete - DO NOT localize this message.

MSG_Gadget_1003 "No se puede establecer el recurso de mapa de pixels en no especificado."
$ MSG_Gadget_1003 message is obsolete - DO NOT localize this message.

MSG_Gadget_0000 "No se puede cambiar XmNlayoutDirection después de la inicialización."


$set MS_Label

MSG_Label_1001 "XmNlabeType no válido"
$ MSG_Label_1001 message is obsolete - DO NOT localize this message.

MSG_Label_1002 "Valor no válido en XmNalignment"
$ MSG_Label_1002 message is obsolete - DO NOT localize this message.

MSG_Label_1003 "Valor no válido en XmNstringDirection"
$ MSG_Label_1003 message is obsolete - DO NOT localize this message.

MSG_Label_0003 "XmNlabelString debe ser una Compound String."
$ please do not translate Compound String.

MSG_Label_0004 "XmNacceleratorText debe ser una Compound String."
$ please do not translate Compound String.


$set MS_List

MSG_List_0000 "Si se cambia, XmNvisibleItemCount debe ser como mínimo 1."

MSG_List_1001 "Política de Selección no válida."
$ MSG_List_1001 message is obsolete - DO NOT localize this message.

MSG_List_1002 "Política de Tamaño no válida."
$ MSG_List_1002 message is obsolete - DO NOT localize this message.

MSG_List_1003 "Política de Mostrar Barra de desplazamiento no válida."
$ MSG_List_1003 message is obsolete - DO NOT localize this message.

MSG_List_1004 "Dirección de cadena no válida."
$ MSG_List_1004 message is obsolete - DO NOT localize this message.

MSG_List_0005 "No se puede cambiar XmNlistSizePolicy después de la inicialización."

MSG_List_0006 "Cuando se cambia, XmNitemCount debe ser cero o positivo."
$ non-negative means zero or positive.

MSG_List_1007 "Ignorado font nulo de SetValues."
$ MSG_List_1007 message is obsolete - DO NOT localize this message.
$ please do not translate SetValues

MSG_List_0007 "Los elementos a suprimir no se encuentran en la lista."

MSG_List_1009 "No hay Barra de desplazamiento horizontal a establecer."
$ MSG_List_1009 message is obsolete - DO NOT localize this message.

MSG_List_1010 "Valor de margen no válido."
$ MSG_List_1010 message is obsolete - DO NOT localize this message.

MSG_List_0008 "XmNlistSpacing debe ser cero o positivo."

MSG_List_0009 "No se puede fijar XmNitems en NULL cuando XmNitemCount es positivo."

MSG_List_0010 "XmNselectedItemCount no debe ser negativo."

MSG_List_0011 "No se puede fijar XmNselectedItems en NULL cuando XmNselectedItemCount \
es positivo."

MSG_List_0012 "XmNtopItemPosition debe ser cero o positivo."

MSG_List_0013 "XmNitems y XmNitemCount ¡no coinciden!"

MSG_List_1017 "El modo de adición no se puede abandonar en selección múltiple."
$ MSG_List_1017 message is obsolete - DO NOT localize this message.

MSG_List_0014 "XmNselectedPositionCount no debe ser negativo."

MSG_List_0015 "No se puede fijar XmNselectedPositions en NULL cuando XmNselectedPositionCount \
es positivo."


$set MS_MainW

MSG_MainW_0000 "MenuBar no se puede cambiar a NULL."
$ please do not translate MenuBar.

MSG_MainW_0001 "CommandWindow no se puede cambiar a NULL."
$ please do not translate CommandWindow.

MSG_MainW_1003 "Valor de margen negativo ignorado."
$ MSG_MainW_1003 message is obsolete - DO NOT localize this message.


$set MS_MenuShell

MSG_MenuShell_0000 "Los widgets MenuShell solamente aceptan hijos XmRowColumn."
$ please do not translate MenuShell.

MSG_MenuShell_0001 "Se intenta administrar un menú desplegable que no está anexado \n\
a un botón Cascade."

MSG_MenuShell_0002 "XmPopup requiere la subclase shellWidgetClass"
$ please do not translate shellWidgetClass.

MSG_MenuShell_0003 "XmPopdown requiere la subclase shellWidgetClass"
$ please do not translate shellWidgetClass.

MSG_MenuShell_0004 "XtMenuPopup requiere un sólo argumento."

MSG_MenuShell_0005 "XtMenuPopup solamente soporta eventos ButtonPress, KeyPress y EnterNotify."
$ please do not translate ButtonPress, KeyPress or EnterNotify.

MSG_MenuShell_0006 "No se puede encontrar el widget emergente \"%s\" en XtMenuPopup."

MSG_MenuShell_0007 "No se puede encontrar el widget \"%s\" en XtMenuPopdown."

MSG_MenuShell_0008 "XtMenuPopdown se llamó con más de un argumento."

MSG_MenuShell_0009 "No se puede cambiar XmNlayoutDirection después de la inicialización."


$set MS_MessageB

MSG_MessageB_1001 "Tipo de Diálogo no válido."
$ MSG_MessageB_1001 message is obsolete - DO NOT localize this message.

MSG_MessageB_1002 "Tipo de Botón Predeterminado no válido."
$ MSG_MessageB_1002 message is obsolete - DO NOT localize this message.

MSG_MessageB_1003 "Tipo de Alineación no válido."
$ MSG_MessageB_1003 message is obsolete - DO NOT localize this message.

MSG_MessageB_0003 "Tipo de hijo no válido. El widget no cuenta con este hijo."

MSG_MessageB_0004 "El botón Cancelar no se puede cambiar."

MSG_MessageB_1006 "Use XmNdefaultButtonType para establecer el botón predeterminado de MessageBox."
$ MSG_MessageB_1006 message is obsolete - DO NOT localize this message.


$set MS_PanedW

MSG_PanedW_0000 "El valor mínimo debe ser mayor que 0."

MSG_PanedW_0001 "El valor máximo debe ser mayor que 0."

MSG_PanedW_0002 "El valor mínimo debe ser menor que el valor máximo."

MSG_PanedW_0003 "Las restricciones no permiten dar la dimensión adecuada."
$ MSG_PanedW_0003 message is obsolete - DO NOT localize this message.

MSG_PanedW_0004 "Demasiados pocos parámetros de la devolución de llamada sash."

MSG_PanedW_0005 "El primer parámetro de devolución de llamada sash no es válido."


$set MS_PWidget

MSG_PWI_1001 "fontList no está definido"
$ MSG_PWI_1001 message is obsolete - DO NOT localize this message.
$ please do not translate fontList.


$set MS_Protocols

MSG_Protocols_0000 "El widget debe ser un VendorShell"
$ please do not translate VendorShell.

MSG_Protocols_0001 "El gestor de protocolos ya existe"

MSG_Protocols_0002 "Existen más protocolos que los que puede manipular el widget.\n\
Límite: 32."


$set MS_PushB

MSG_PushB_1001 "No hay suficiente memoria"
$ MSG_PushB_1001 message is obsolete - DO NOT localize this message.


$set MS_RowColumn

$ In this set, do not translate the word that starts with "XmN" or "Xm". 

MSG_RowColumn_1001 "Intento de establecer la anchura en cero.\n\
Establecida en el valor predeterminado 16."
$ MSG_RowColumn_1001 message is obsolete - DO NOT localize this message.

MSG_RowColumn_0000 "Se ha ignorado el intento de fijar el ancho en cero"

MSG_RowColumn_1003 "Intento de establecer la altura en cero.\n\
Establecida en el valor predeterminado 16."
$ MSG_RowColumn_1003 message is obsolete - DO NOT localize this message.

MSG_RowColumn_0001 "Se ha ignorado el intento de fijar la altura en cero"

MSG_RowColumn_0002 "Se ha forzado el valor NULL en XmNhelpWidgetforced ya que los menús emergentes no lo utilizan."

MSG_RowColumn_0003 "Se ha forzado el valor NULL en XmNhelpWidget ya que los menús desplegables no lo utilizan."

MSG_RowColumn_0004 "Se ha forzado el valor NULL en XmNhelpWidget ya que los menús de opciones no lo utilizan."

MSG_RowColumn_0005 "Se ha forzado el valor NULL en XmNhelpWidget ya que las áreas de trabajo no lo utilizan."

MSG_RowColumn_1009 "Valor de XmNrowColumnType desconocido.\n\
Está establecido en WorkArea."
$ MSG_RowColumn_1009 message is obsolete - DO NOT localize this message.

MSG_RowColumn_0007 "La jerarquía de widget no es apropiada para este XmNrowColumnType.\n\
Se utilizará el valor predeterminado XmWORK_AREA."

MSG_RowColumn_0008 "Se ha ignorado el intento de cambiar XmNrowColumnType después de la inicialización"

MSG_RowColumn_1012 "Valor de XmNorientation desconocido.\n\
Se utiliza el valor predeterminado."
$ MSG_RowColumn_1012 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1013 "Intento de establecer XmNorientation en un valor desconocido.\n\
El valor se ignora."
$ MSG_RowColumn_1013 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1014 "Valor de XmNpacking desconocido.\n\
Se utiliza el valor predeterminado."
$ MSG_RowColumn_1014 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1015 "Intento de establecer XmNpacking en un valor desconocido.\n\
El valor se ignora."
$ MSG_RowColumn_1015 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1016 "Valor de XmNentryAlignment desconocido.\n\
Se utiliza el valor predeterminado."
$ MSG_RowColumn_1016 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1017 "Intento de establecer XmNentryAlignment en un valor desconocido.\n\
El valor se ignora."
$ MSG_RowColumn_1017 message is obsolete - DO NOT localize this message.

MSG_RowColumn_0015 "Se ha ignorado el intento de fijar FALSE en XmNisHomogenous por un widget de tipo RowColumn \
XmMENU_BAR."
$ please do not translate FALSE and RowColumn.

MSG_RowColumn_0016 "Se ha ignorado el intento de cambiar XmNentryClass por un widget de tipo RowColumn \
XmMENU_BAR."
$ please do not translate RowColumn.

MSG_RowColumn_0017 "Se ha ignorado el intento de cambiar XmNwhichButton a través de XtSetValues por un widget RowColumn \
de tipo XmMENU_PULLDOWN."
$ please do not translate RowColumn.

MSG_RowColumn_0018 "Se ha ignorado el intento de cambiar XmNmenuPost a través de XtSetValues por un widget RowColumn \
de tipo XmMENU_PULLDOWN."
$ please do not translate RowColumn.

MSG_RowColumn_0019 "Se ha ignorado el intento de fijar un valor no válido en XmNmenuPost."

MSG_RowColumn_0020 "Se ha ignorado el intento de cambiar XmNshadowThickness por un widget RowColumn no de tipo \
XmMENU_PULLDOWN o XmMENU_POPUP."
$ please do not translate RowColumn.

MSG_RowColumn_1024 "Intento de cambiar XmNorientation para un widget RowColumn \n\
de tipo XmMENU_OPTION.\n\
El valor se ignora."
$ MSG_RowColumn_1024 message is obsolete - DO NOT localize this message.

MSG_RowColumn_0022 "Intento de añadir un hijo de tipo erróneo a \n\
un widget de menú (p.ej. RowColumn)"

MSG_RowColumn_0023 "Intento de añadir un hijo de tipo erróneo a\n\
un widget RowColumn homogéneo"

MSG_RowColumn_0025 "Intento de cambiar XmNisHomogeneous para un widget RowColumn \n\
\t de tipo XmMENU_OPTION ignorado"
$ please do not translate RowColumn.

MSG_RowColumn_0026 "No se recomienda activar la función para arrancar un panel de menú compartido"

MSG_RowColumn_0027 "Carácter mnemotécnico no válido. No se ha podido convertir X KEYSYM \n\
\t en un código de tecla."
$ please do not translate X KEYSYM.


$set MS_Scale

MSG_Scale_0000 "El valor mínimo de escala es mayor o igual que\n\
el valor máximo de escala."

MSG_Scale_0001 "El valor de escala especificado es menor que\n\
el valor mínimo de escala."

MSG_Scale_0002 "El valor de escala especificado es mayor que\n\
el valor máximo de escala."

MSG_Scale_1004 "Orientación incorrecta."
$ MSG_Scale_1004 message is obsolete - DO NOT localize this message.

MSG_ScaleScrBar_0004 "Dirección de proceso incorrecta."

MSG_Scale_0005 "Grosor de resaltado no válido."
$ MSG_Scale_0005 message is obsolete - DO NOT localize this message.

MSG_Scale_0006 "XmNscaleMultiple no válido; es mayor que (máximo - mínimo)"

MSG_Scale_0007 "XmNscaleMultiple no válido; es menor que cero"

MSG_Scale_0008 "(Máximo - mínimo) no puede ser mayor que \n\
\tINT_MAX dividido por 2. El mínimo se ha establecido en cero. \n\
\t El máximo puede haberse establecido en (INT_MAX/2)."

MSG_Scale_0009 "El valor de XmNshowValue no es correcto"


$set MS_ScrollBar

MSG_ScrollBar_0000 "El valor mínimo de barra de desplazamiento es mayor\n\
o igual que el valor máximo de barra de desplazamiento."

MSG_ScrollBar_0001 "El tamaño de control deslizante especificado es menor que 1."

MSG_ScrollBar_0002 "El valor de barra de desplazamiento especificado es\n\
menor que el valor mínimo de barra de desplazamiento."

MSG_ScrollBar_0003 "El valor de barra de desplazamiento especificado es mayor que\n\
el valor máximo de barra de desplazamiento menos el tamaño de\n\
control deslizante de la barra de desplazamiento."

MSG_ScrollBar_1005 "Orientación incorrecta."
$ MSG_ScrollBar_1005 message is obsolete - DO NOT localize this message.

MSG_ScrollBar_1006 "Dirección de proceso incorrecta."
$ MSG_ScrollBar_1006 message is obsolete - DO NOT localize this message.

MSG_ScrollBar_0004 "El incremento de barra de desplazamiento es menor que 1."

MSG_ScrollBar_0005 "El incremento de barra de desplazamiento es menor que 1."

MSG_ScrollBar_0006 "El retardo inicial de barra de desplazamiento es menor que 1."

MSG_ScrollBar_0007 "El retardo de repetición de barra de desplazamiento es menor que 1."

MSG_ScrollBar_1011 "Error en gestor de contexto; los fondos de la barra\n\
de desplazamiento no se pueden establecer correctamente"
$ MSG_ScrollBar_1011 message is obsolete - DO NOT localize this message.

MSG_ScrollBar_1012 "Error en gestor de contexto; los primeros planos de la barra\n\
de desplazamiento no se pueden establecer correctamente"
$ MSG_ScrollBar_1012 message is obsolete - DO NOT localize this message.

MSG_ScrollBar_0008 "El tamaño de control deslizante especificado es mayor que\n\
el valor máximo de barra de desplazamiento menos el valor\n\
mínimo de barra de desplazamiento."


$set MS_ScrolledW

MSG_ScrolledW_1001 "Política de Mostrar Barra de desplazamiento no válida."
$ MSG_ScrolledW_1001 message is obsolete - DO NOT localize this message.

MSG_ScrolledW_1002 "Política de desplazamiento no válida."
$ MSG_ScrolledW_1002 message is obsolete - DO NOT localize this message.

MSG_ScrolledW_1003 "Política Visual no válida."
$ MSG_ScrolledW_1003 message is obsolete - DO NOT localize this message.

MSG_ScrolledW_1004 "Política de colocación no válida."
$ MSG_ScrolledW_1004 message is obsolete - DO NOT localize this message.

MSG_ScrolledW_0004 "No se puede cambiar la política de desplazamiento\n\
después de la inicialización."

MSG_ScrolledW_0005 "No se puede cambiar la política visual después de la inicialización."

MSG_ScrolledW_0006 "No se puede establecer la política de \n\
barra de desplazamiento AS_NEEDED con una\n\
política visual de VARIABLE."
$ please do not translate AS_NEEDED or VARIABLE.

MSG_ScrolledW_0007 "No se puede cambiar la anchura de la barra de \n\
desplazamiento en modo AUTOMATIC."
$ please do not translate AUTOMATIC.

MSG_ScrolledW_0008 "No se puede cambiar la ventana del portapapeles"

MSG_ScrolledW_0009 "No se puede establecer la política visual de CONSTANT en modo APPLICATION_DEFINED."
$ please do not translate CONSTANT or APPLICATION_DEFINED.

MSG_ScrollVis_0000 "Se han enviado parámetros incorrectos a la función XmScrollVisible."


$set MS_SelectioB

MSG_SelectioB_1001 "Tipo de diálogo incorrecto."
$ MSG_SelectioB_1001 message is obsolete - DO NOT localize this message.

MSG_SelectioB_0001 "El tipo de diálogo no se puede modificar."
$ please do not translate Dialog.

MSG_SelectioB_1003 "Solamente se permite un hijo de área de trabajo."
$ MSG_SelectioB_1003 message is obsolete - DO NOT localize this message.

MSG_SelectioB_0002 "El widget no soporta este tipo de hijo."


$set MS_Separator

MSG_Separator_1001 "Tipo de separador no válido."
$ MSG_Separator_1001 message is obsolete - DO NOT localize this message.

MSG_Separator_1002 "Orientación no válida."
$ MSG_Separator_1002 message is obsolete - DO NOT localize this message.


$set MS_Text

MSG_Text_0000 "Se ignorará el texto de origen incorrecto."

MSG_Text_1003 "Modo de edición no válido."
$ MSG_Text_1003 message is obsolete - DO NOT localize this message.

MSG_Text_0002 "El widget de texto es modificable. XmNtraversalOn debe ser True."
$ please do not translate XmNtraversalOn and True.
$ MSG_Text_0002 message is obsolete - DO NOT localize this message.

MSG_TextIn_0000 "No se puede encontrar la posición mientras se intenta mover a la línea anterior."

MSG_TextOut_0000 "La cantidad de filas debe ser mayor que 0."

MSG_TextF_0002 "XmFontListInitFontContext ha fallado."
$ please do not translate XmFontListInitFontContext.

MSG_TextF_0003 "XmFontListGetNextFont ha fallado."
$ please do not translate XmFontListGetNextFont.

MSG_TextF_0004 "El font no soporta el carácter '%s'.  Se ha descartado."

MSG_TextFWcs_0000 "El font no soporta el carácter '%s'.  Se ha descartado."

MSG_TextFWcs_0001 "No se puede utilizar un lugar con bites múltiples sin ningún fontset.  Se ha descartado el valor."
$ MSG_TextFWcs_0001 message is obsolete - DO NOT localize this message.

$set MS_TextF

MSG_TextF_0000 "La posición del cursor debe ser mayor o igual que 0."

MSG_TextF_0001 "La cantidad de columnas debe ser mayor que 0."

MSG_TextF_0005 "XmNtraversalOn siempre debe ser True."
$ please do not translate XmNtraversalOn and True.
$ MSG_TextF_0005 message is obsolete - DO NOT localize this message.

MSG_TextF_0006 "La cantidad de columnas debe ser mayor o igual que 0."


$set MS_ToggleB

MSG_ToggleB_1001 "El tipo de indicador debe ser XmONE_OF_MANY o XmN_OF_MANY"
$ MSG_ToggleB_1001 message is obsolete - DO NOT localize this message.


$set MS_Traversal

MSG_Traversal_1001 "Valor no válido para navigation_type"
$ MSG_Traversal_1001 message is obsolete - DO NOT localize this message.

MSG_Traversal_1002 "¡¡Valor anterior erróneo para navigation_type!!"
$ MSG_Traversal_1002 message is obsolete - DO NOT localize this message.

MSG_Traversal_1003 "Situación de rutina de carga transversal con parámetros erróneos"
$ MSG_Traversal_1003 message is obsolete - DO NOT localize this message.

MSG_Traversal_1004 "Intento de colocar un nuevo tabulador usando parámetros erróneos"
$ MSG_Traversal_1004 message is obsolete - DO NOT localize this message.

MSG_Traversal_1005 "startWidget no está en la lista de hijos"
$ MSG_Traversal_1005 message is obsolete - DO NOT localize this message.
$ please do not translate startWidget

MSG_Traversal_1006 "Parámetros erróneos para TraverseToChild"
$ MSG_Traversal_1006 message is obsolete - DO NOT localize this message.
$ please do not translate TraverseToChild.


$set MS_Vendor

MSG_Vendor_0000 Valor no válido para XmNdeleteResponse

MSG_Vendor_1002 "XmNpreeditType no válido, valor predeterminado para OverTheSpot"
$ MSG_Vendor_1002 message is obsolete - DO NOT localize this message.

MSG_Vendor_0001 "Valor no válido para XmNinputPolicy"

MSG_Vendor_0002 "XmNlayoutDirection no puede cambiarse."

MSG_Vendor_0003 "Fatal Error: \n\
_XmGetDefaultDisplay no puede utilizarse antes de VendorS.Initialize ya que devolverá un NULL"


$set MS_VendorE

MSG_VendorE_1001 "FetchUnitType: clase de widget errónea"
$ MSG_VendorE_1001 message is obsolete - DO NOT localize this message.

MSG_VendorE_0000 "String to no-op conversion needs no extra arguments"
$ MSG_VendorE_0000 message is obsolete - DO NOT localize this message.

MSG_VendorE_0005 "FetchUnitType llamado sin un widget de referencia."
$ please do not translate FetchUnitType.
$ MSG_VendorE_0005 message is obsolete - DO NOT localize this message.


$set MS_VirtKeys

MSG_VirtKey_1001 "No se ha llamado a la inicialización de vínculos virtuales"
$ MSG_VirtKey_1001 message is obsolete - DO NOT localize this message.


$set MS_Visual

MSG_Visual_0000 "Solicitado un color no válido desde _XmAccessColorData."

MSG_Visual_0001 "No se puede asignar la anotación colormap para el fondo predeterminado."

MSG_Visual_0002 "No se puede efectuar el análisis sintáctico de la especificación del color de fondo predeterminado."


$set MS_XmIm

MSG_XmIm_0000 "No se puede abrir el método de introducción. Se utilizará XLookupString."

MSG_XmIm_1002 "No se puede crear el Objeto de Método de Entrada"
$ MSG_XmIm_1002 message is obsolete - DO NOT localize this message.

MSG_XmIm_1003 "XmIMFocus invocado con widget NULL."
$ MSG_XmIm_1003 message is obsolete - DO NOT localize this message.

MSG_XmIm_1004 "XmIMMove invocado sin el foco de Método de Entrada."
$ MSG_XmIm_1004 message is obsolete - DO NOT localize this message.

  
$set MS_Resources
  
MSG_Resource_0001 "OK"
$ "OK" as in "Go ahead"/"Confirmed to proceed as instructed"/"Fine"

MSG_Resource_0002 "Cancelar"
$ ... as in "Never mind"/"abort this operation"/"Stop current request"

MSG_Resource_0003 "Seleccionar"
$ ... as in "choosing from a list of items"

MSG_Resource_0004 "Aplicar"
$ ... as in "To do something with the things selected"

MSG_Resource_0005 "Ayuda"
$ ... as in "Need more guidance on how to deal with this dialog box"

MSG_Resource_0006 "Filtro"
$ ... the noun "Filter", as in "a sifter/a filter/a qualifier to screen out"
$ files with

MSG_Resource_0007 "Archivos"
$ ... as in "a list of the files"

MSG_Resource_0008 "Directorios"
$ ... as in "a list of the directories" 

MSG_Resource_0009 "Elementos"
$ ... as in "a list of things to choose from"

MSG_Resource_0010 "Filtrar"
$ ... the verb "Filter", as in "To screen out some files"
$ Note this has a slightly different semantics from MSG_Res_1006.
$ In English, it is the same word, but MSG_Res_1006 means "a sifter", and
$ MSG_Res_1010 means "Push this button to filter out some files"
$ MSG_Res_1006 is a label indicating "entry below is qualifier to filter out" 
$ files with

MSG_Resource_0011 "Directorio"

MSG_Resource_0012 ">"
$ command-line prompt in the Command widget. 


$set MS_BaseClass

MSG_BaseClass_0000 "No se ha encontrado ningún contexto para extensión."

MSG_BaseClass_0001 "_XmPopWidgetExtData: no se ha encontrado ninguna extensión con XFindContext."

MSG_BaseClass_0002 "XmFreeWidgetExtData no es una rutina a la que se dé soporte."
$ MSG_BaseClass_0002 message is obsolete - DO NOT localize this message.

MSG_GetSecRes_0000 "getLabelSecResData: no hay suficiente memoria."
$ please do not translate getLabelSecResData.
$ MSG_GetSecRes_0000 message is obsolete - DO NOT localize this message.


$set MS_Display

MSG_Display_0001 "Creando múltiples XmDisplays para la misma pantalla de X.\n\
\tSolamente se puede hacer referencia, mediante llamadas a XmGetXmDisplay,\n\
\tal primer XmDisplay creado para una pantalla de X específica."
$ please do not translate XmDisplay or XmGetXmDisplay

MSG_Display_0002 "Recibido TOP_LEVEL_LEAVE sin DragContext activo."
$ please do not translate TOP_LEVEL_LEAVE or DragContext

MSG_Display_0003 "No se puede establecer la clase de XmDisplay en una subclase\n\
que no sea de XmDisplay."
$ please do not translate XmDisplay


$set MS_DragBS

MSG_DragBS_0000 "El _MOTIF_DRAG_WINDOW se ha destruido."
$ please do not translate _MOTIF_DRAG_WINDOW

MSG_DragBS_0001 "Los niveles de versión del protocolo no coinciden."

MSG_DragBS_0002 "No es posible abrir la pantalla."

MSG_DragBS_0003 "La tabla de átomos está vacía."

MSG_DragBS_0004 "La tabla destino está vacía."

MSG_DragBS_0005 "La tabla destino tiene una propiedad incoherente."

MSG_DragBS_0006 "Índice de tabla destino no válido."


$set MS_DragC

MSG_DragC_0001 "GenerateCallback no espera XmCR_DROP_SITE_ENTER como una razón."
$ please do not translate GenerateCallback or XmCR_DROP_SITE_ENTER

MSG_DragC_0002 "Selección no válida en DropConvertCallback."
$ please do not translate DropConvertCallback

MSG_DragC_0003 "La selección de soltar se ha perdido."

MSG_DragC_0004 "XGrabPointer ha fallado."

MSG_DragC_0005 "ExternalNotifyHandler: la razón de retorno de flujo de información\n\
no es aceptable."
$ please do not translate ExternalNotifyHandler

MSG_DragC_0006 "XmDragStart debe llamarse debido a que se ha presionado un botón o a un evento de movimiento."
$ please do not translate XmDragStart


$set MS_DragICC

MSG_DragICC_0000 "Tipo de mensaje de arrastrar y soltar desconocido."

MSG_DragICC_0001 "Los niveles de versión de protocolo no coinciden."


$set MS_DragIcon

MSG_DragIcon_0000 "No se ha especificado ninguna geometría para el mapa de pixels de dragIcon."
$ please do not translate dragIcon

MSG_DragIcon_0001 "dragIcon creado sin mapa de pixels."
$ please do not translate dragIcon

MSG_DragIcon_0002 "String to Bitmap converter needs Screen argument"
$ MSG_DragIcon_0002 message is obsolete - DO NOT localize this message.


$set MS_DragOverS

MSG_DragOverS_0000 "La profundidad no coincide."

MSG_DragOverS_0001 "Anexo de icono desconocido."

MSG_DragOverS_0002 "Estado de arrastrar desconocido."

MSG_DragOverS_0003 "XmNblendModel desconocido."
$ please do not translate XmNblendModel 


$set MS_DragUnder

MSG_DragUnder_0000 "No es posible obtener la geometría de ventana de dropSite."

MSG_DragUnder_0001 "animationPixmapDepth no válido."
$ please do not translate animationPixmapDepth


$set MS_DropSMgr

MSG_DropSMgr_0001 "No se pueden crear lugares de soltar que sean hijos de un único lugar de soltar."

MSG_DropSMgr_0002 "Recibiendo Sucesos de Movimiento sin un contexto de arrastrar activo."

MSG_DropSMgr_0003 "Recibiendo operación cambiada sin un contexto de arrastrar activo."

MSG_DropSMgr_0004 "Creando un lugar de soltar activo sin ningún procedimiento de soltar."

MSG_DropSMgr_0005 "No se pueden establecer rectángulos o números de rectángulo\n\
de lugares de soltar compuestos."

MSG_DropSMgr_0006 "Registrando un widget como un lugar de soltar fuera de secuencia.\n\
\tLos antecesores deben estar registrados antes de que lo estén \n\
\tcualquiera de sus descendientes."

MSG_DropSMgr_0007 "No se puede registrar un widget como un lugar de soltar más de una vez."

MSG_DropSMgr_0008 "El tipo de DropSite solamente se puede establecer en el momento de crearlo."
$ please do not translate DropSite

MSG_DropSMgr_0009 "No se pueden cambiar los rectángulos de un lugar de soltar que no sea único."

MSG_DropSMgr_0010 "No se puede registrar un Shell como lugar para soltar."

MSG_DropSMgrI_0001 "No se puede registrar un lugar de soltar que sea descendiente de\n\
un lugar de soltar único."

MSG_DropSMgrI_0002 "No se puede crear una lista de hijos discontinuos para un lugar\n\
de soltar compuesto."

MSG_DropSMgrI_0003 "%s no es un hijo del lugar de soltar de %s."


$set MS_GeoUtils

MSG_GeoUtils_0000 "Falló la solicitud de geometría a la respuesta \"almost\""
$ please do not translate "almost"
$ MSG_GeoUtils_0000 message is obsolete - DO NOT localize this message.

MSG_GeoUtils_0001 "Invalid layout of children found"
$ MSG_GeoUtils_0001 message is obsolete - DO NOT localize this message.

MSG_GeoUtils_0002 "Invalid order found in XmSelectionBox"
$ MSG_GeoUtils_0002 message is obsolete - DO NOT localize this message.


$set MS_Region

MSG_Region_0000 "Error de memoria."


$set MS_RepType

MSG_RepType_0001 "ID de tipo de representación no válido."

MSG_RepType_0002 "Valor (%d) no válido para tipo de respuesta de XmR%s."
$ please do not translate XmR%s

MSG_RepType_0000 "Conversión inversa de "


$set MS_ResConvert

MSG_ResConvert_0000 "FetchUnitType: Clase de widget errónea."
$ please do not translate FetchUnitType.
$ MSG_ResConvert_0000 message is obsolete - DO NOT localize this message.

MSG_ResConvert_0001 "La lista predeterminada no se ha definido correctamente. Saliendo..."

MSG_ResConvert_0002 "Faltan los dos puntos en la cadena de font \"%s\". No se efectuará el análisis sintáctico de los restantes fonts de la lista."

MSG_ResConvert_0003 "El rótulo \"%s\" contiene un delimitador no válido. No se efectuará el análisis sintáctico de los restantes fonts de la lista."

MSG_ResConvert_0004 "Unmatched quotation marks in string \"%s\"; any remaining fonts in list unparsed"
$ MSG_ResConvert_0004 message is obsolete - DO NOT localize this message.

MSG_ResConvert_0005 "El rótulo \"%s\" contiene pares de comillas no completos. No se efectuará el análisis sintáctico de los restantes fonts de la lista."

MSG_ResConvert_0006 "Se encontró un rótulo nulo al convertir al tipo %s. No se efectuará el análisis sintáctico de los restantes fonts de la lista."

MSG_ResConvert_0007 "No se puede convertir XmString en texto compuesto."

MSG_ResConvert_0008 "No hay suficiente memoria para XmbTextListToTextProperty"

MSG_ResConvert_0009 "El lugar no está soportado para XmbTextListToTextProperty"

MSG_ResConvert_0010 "Falló XmbTextListToTextProperty"

MSG_ResConvert_0011 "No se puede convertir el nombre del widget en Widget."
$ please do not translate Widget.

MSG_ResConvert_0012 "No se puede convertir texto compuesto en XmString"

MSG_ResConvert_0013 "No se puede convertir XmString en texto compuesto"

MSG_ResConvert_0014 "FetchUnitType called without a widget to reference"
$ please do not translate FetchUnitType.
$ MSG_ResConvert_0014 message is obsolete - DO NOT localize this message

MSG_ResConvert_0015 "FetchDisplayArg called without a widget to reference"
$ please do not translate FetchDisplayArg.
$ MSG_ResConvert_0015 message is obsolete - DO NOT localize this message

MSG_ResConvert_0016 "FetchWidgetArg called without a widget to reference"
$ please do not translate FetchWidgetArg.
$ MSG_ResConvert_0016 message is obsolete - DO NOT localize this message


$set MS_Screen

MSG_Screen_0000 "La pantalla de iconos no coincide."

MSG_Screen_0001 "No se puede obtener XmScreen porque no se ha encontrado XmDisplay."


$set MS_ColObj

MSG_ColObj_0001 "No se ha podido asignar memoria para los datos de objeto de color."

MSG_ColObj_0002 "Error en el número de pantalla de selección de servidor de color."

 
$set MS_IG
 
MSG_IG_1 "Alineación incorrecta."
$ MSG_IG_1 message is obsolete - DO NOT localize this message.

MSG_IG_2 "Comportamiento incorrecto."
$ MSG_IG_2 message is obsolete - DO NOT localize this message.

MSG_IG_3 "Modo de relleno incorrecto."
$ MSG_IG_3 message is obsolete - DO NOT localize this message.

MSG_IG_4 "Cadena de posición de mapa de pixels incorrecta."
$ MSG_IG_4 message is obsolete - DO NOT localize this message.

MSG_IG_5 "Anchura o altura de margen incorrecta."
$ MSG_IG_5 message is obsolete - DO NOT localize this message.

MSG_IG_6 "Tipo de sombra incorrecta."
$ MSG_IG_6 message is obsolete - DO NOT localize this message.


$set MS_ComboBox

MSG_ComboBox_0000 "Las aplicaciones no puede añadir hijos a widgets XmComboBox."

MSG_ComboBox_0001 "El recurso XmNcomboBoxType no puede cambiarse por XtSetValues."

MSG_ComboBox_0002 "XmFontListGetNextFont failed."
$ MSG_ComboBox_0002 message is obsolete - DO NOT localize this message.

MSG_ComboBox_0003 "XmFontListInitFontContext failed."
$ MSG_ComboBox_0003 message is obsolete - DO NOT localize this message.

MSG_ComboBox_0004 "Se ha destruido el widget interno.  El comportamiento no está definido."

MSG_ComboBox_0005 "Se ha cancelado el manipuleo del widget interno.  El comportamiento no está definido."

MSG_ComboBox_0006 "XmQUICK_NAVIGATE sólo es válido para ComboBoxes de XmNcomboBoxType XmDROP_DOWN_LIST"

MSG_ComboBox_0007 "La acción ha sido invocada mediante un número incorrecto de parámetros."

MSG_ComboBox_0008 "La rutina de acción se ha llamado desde un widget que no es descendiente de ComboBox"

MSG_ComboBox_0009 "Se ha llamado XmComboBoxSelectItem con un elemento que no se encuentra en ComboBox."

MSG_ComboBox_0010 "Se ha llamado XmComboBoxSetItem con un elemento que no se encuentra en ComboBox."

MSG_ComboBox_0011 "Se ha llamado XmComboBoxDeletePos con una posición no válida."

MSG_ComboBox_0012 "Se ha llamado la rutina de programa utilitario XmComboBox con un widget no válido."

MSG_ComboBox_0013 "Es posible que las aplicaciones no fijen los hijos automáticos del widget XmComboBox."

MSG_ComboBox_0014 "XmComboBox positionMode no se puede cambiar después de la creación."
$ please do not translate positionMode.


$set MS_Container

MSG_Container_0000 "Se ha invocado la acción mediante una cantidad incorrecta de parámetros."

MSG_Container_0001 "XmNdetailColumnHeading y XmNdetailColumnHeadingCount no coinciden."


$set MS_CSText

MSG_CSText_0000 "Invalid margin height; must be greater than or equal to 0."
$ MSG_CSText_0000 message is obsolete - DO NOT localize this message.

MSG_CSText_0001 "Invalid margin width; must be greater than or equal to 0."
$ MSG_CSText_0001 message is obsolete - DO NOT localize this message.

MSG_CSText_0002 "Invalid edit mode."
$ MSG_CSText_0002 message is obsolete - DO NOT localize this message.

MSG_CSText_0003 "XmNtraversalOn must always be true."
$ MSG_CSText_0003 message is obsolete - DO NOT localize this message.

MSG_CSText_0004 "Cannot change XmNscrollHorizontal after initialization."
$ MSG_CSText_0004 message is obsolete - DO NOT localize this message.

MSG_CSText_0005 "Cannot change XmNscrollVertical after initialization."
$ MSG_CSText_0005 message is obsolete - DO NOT localize this message.

MSG_CSText_0006 "Cannot change XmNscrollTopSide after initialization."
$ MSG_CSText_0006 message is obsolete - DO NOT localize this message.

MSG_CSText_0007 "Cannot change XmNscrollLeftSide after initialization."
$ MSG_CSText_0007 message is obsolete - DO NOT localize this message.


$set MS_GrabShell

MSG_GrabS_0000 "XmPopup requires a subclass of shellWidgetClass."
$ please do not translate shellWidgetClass.
$ MSG_GrabS_0000 message is obsolete - DO NOT localize this message.


$set MS_Manager

MSG_Manager_0000 "La clase de widget %s contiene una anotación CompositeClassExtension no válida."
$ please do not translate CompositeClassExtension.

MSG_Manager_0001 "No se puede cambiar XmNlayoutDirection ni XmNstringDirection después de la inicialización."


$set MS_NavigMap

MSG_NavigMap_0000 "_XmNavigate called with invalid direction"
$ MSG_NavigMap_0000 message is obsolete - DO NOT localize this message.


$set MS_Notebook

MSG_Notebook_0000 "El recurso XmNnotebookChildType no se puede configurar mediante XtSetValues."


$set MS_PixConv

MSG_PixConv_0000 "La cantidad de parámetros de CvtStringToPixmap no es correcta"
$ please do not translate CvtStringToPixmap.


$set MS_Primitive

MSG_Primitive_0000 "No se puede cambiar XmNlayoutDirection después de la inicialización."


$set MS_ScrollFrameT

MSG_ScrollFrameT_0000 "AssocNavigator requiere una característica de navegante"
$ please do not translate AssocNavigator.

MSG_ScrollFrameT_0001 "DeAssocNavigator requiere una característica de navegante"
$ please do not translate DeAssocNavigator.


$set MS_SpinB

MSG_SpinB_0001 "Invalid value for XmNarrowLayout."
$ MSG_SpinB_0001 message is obsolete - DO NOT localize this message.

MSG_SpinB_0002 "XmNminimumValue equals XmNmaximumValue."
$ MSG_SpinB_0002 message is obsolete - DO NOT localize this message.

MSG_SpinB_0003 "No se han suministrado elementos para el hijo XmSTRING."

MSG_SpinB_0004 "XmNincrementValue no puede ser 0. Se utilizará el valor 1."

MSG_SpinB_0005 "La dirección de giro especificada por XmNincrementValue\n\
puede ser invertida para que coincida con \n\
XmNminimumValue y XmNmaximumValue especificados."

MSG_SpinB_0006 "XmNposition está fuera de intervalo. Se utilizará el XmNposition mínimo."

MSG_SpinB_0007 "XmNposition está fuera de intervalo. Se utilizará XmNposition máximo."

MSG_SpinB_0008 "El valor de XmNpositionType no es válido.  Se utilizará el valor predeterminado."

MSG_SpinB_0009 "XmNpositionType resource can only be set at creation time."
$ MSG_SpinB_0009 message is obsolete - DO NOT localize this message.


$set MS_Transfer

MSG_Transfer_0000 "Se llama SelectionCallbackWrapper cuando se deben finalizar las transferencias"
$ please do not translate SelectionCallbackWrapper.

MSG_Transfer_0001 "Cannot lock the clipboard; aborting transfer"
$ MSG_Transfer_0001 message is obsolete - DO NOT localize this message.

MSG_Transfer_0002 "El formato y tipo de los datos suministrador por la devolución de llamada no coinciden con los datos a fusionar."

MSG_Transfer_0003 "XmConvertCallbackStruct no se encuentra en estado XmCONVERT_MERGE."

MSG_Transfer_0004 "Todavía no se soporta CONVERT_MORE."
$ please do not translate CONVERT_MORE.

MSG_Transfer_0005 "Se encontró un valor atom incorrecto."

MSG_Transfer_0006 "Advertencia: Se ha intentado iniciar una transferencia MULTIPLE mientras otra estaba en curso."
$ please do not translate MULTIPLE.

MSG_Transfer_0007 "Advertencia: Se ha intentado enviar una transferencia MULTIPLE mientras otra no está en curso."
$ please do not translate MULTIPLE.


$set MS_VaSimple

MSG_VaSimple_0000 "La conversión XtVaTypedArg requiere un identificador de widget no nulo."

MSG_VaSimple_0001 "No se puede encontrar el tipo de recurso para convertir."

MSG_VaSimple_0002 "El tipo de conversión ha fallado."


$set MS_Xm

MSG_Motif_0000 "\nNombre: %s\nClase: %s\n"

MSG_Motif_0001 "La acción se ha invocado con una cantidad de parámetros incorrecta."


$set MS_XmRenderT

MSG_XmRenderT_0000 "XmNtag no puede ser NULL.  Se configurará como cadena vacía."

MSG_XmRenderT_0001 "La pantalla es NULL.  No se puede cargar el font."

MSG_XmRenderT_0002 "XmNfontType no es válido.  No se puede cargar el font."

MSG_XmRenderT_0003 "Ha fallado la conversión.  No se puede cargar el font."

MSG_XmRenderT_0004 "XmNfontType se ha fijado en XmAS_IS.  No se puede cargar el font."

MSG_XmRenderT_0005 "XmNloadModel es XmLOAD_IMMEDIATE pero no se ha especificado XmNfont ni XmNfontName.\n\
No se puede cargar el font."


$set MS_XmSelect

MSG_XmSelect_0000 "Internal error: no selection property context for display"
$ MSG_XmSelect_0000 message is obsolete - DO NOT localize this message.

MSG_XmSelect_0001 "Selection owner returned type INCR property with format != 32"
$ please do not translate INCR.
$ MSG_XmSelect_0001 message is obsolete - DO NOT localize this message.

MSG_XmSelect_0002 "XtGetSelectionRequest called for widget \"%s\" outside of ConvertSelection proc"
$ please do not translate ConvertSelection.
$ MSG_XmSelect_0002 message is obsolete - DO NOT localize this message.


$set MS_XmString

MSG_XmString_0000 "No se encontró ningún font."


$set MS_XmTabList

MSG_XmTabList_0000 "El valor de tabulación no puede ser negativo."


$set MS_SSpinB

MSG_SSpinB_0001 "No se puede configurar el recurso XmNtextField."

MSG_SSpinB_0002 "El recurso XmNpositionType sólo puede configurarse al crearse."

MSG_SSpinB_0003 "El elemento no existe.  XmNposition no se ha modificado."
