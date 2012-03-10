/* $XConsortium: AppSpecific.c /main/4 1996/05/09 03:39:11 drk $ */
#include "PrintDemo.h"

/*
 * ------------------------------------------------------------------------
 * Name: AppObject_new
 *
 * Description:
 *
 *     Allocates a new AppObject data structure. If parent is non-NULL,
 *     an application-specific main window work region is created.
 *
 * Return value:
 *
 *     A pointer to the new AppObject structure.
 *
 */
#define LABEL_FMTSTR "Filename: %s   Size: %d"

static char * 
ReadFile(char * filename, int * filesize)
{
   char *buffer = NULL;
   FILE * file;
   *filesize = 0 ;

   if ((file = fopen(filename, "r")) == NULL) return NULL ;

   fseek(file, 0L, SEEK_END);
   *filesize = ftell(file);
   
   rewind(file);
   buffer = (char *) malloc(*filesize+1);
   if (fread(buffer, 1, *filesize, file) == *filesize ) {
      buffer[*filesize] = '\0';
      return buffer;
   }

   free(buffer);
   return NULL;
}

AppObject*
AppObject_new(
	      Widget parent,
	      String file_name)
{
    AppObject* me = (AppObject*)XtCalloc(1, sizeof(AppObject));
    int filesize ;

    me->main_window = parent ;

    if(file_name != (String)NULL)
	me->file_name = XtNewString(file_name);
    else
	me->file_name = XtNewString("README.txt");

    me->file_buffer = ReadFile(me->file_name, &filesize);
    if (me->file_buffer == NULL)  {
	me->file_buffer = XtNewString("abcdefghijklmnopqrstuvwxyz");
	filesize = strlen("abcdefghijklmnopqrstuvwxyz");
    }

    if(parent != (Widget)NULL)
    {
	XmString label;
	String buf;
	
	buf = XtCalloc(strlen(LABEL_FMTSTR)+strlen(me->file_name)+10,
		       sizeof(char));
	sprintf(buf, LABEL_FMTSTR, me->file_name, filesize);
	label = XmStringCreateLocalized(buf);
	XtFree(buf);
	me->widget =
	    XtVaCreateManagedWidget("AppWorkArea",
				    xmLabelWidgetClass,
				    parent,
				    XmNlabelString, label,
				    NULL);
	XmStringFree(label);
    }

    return me;
}

/*
 * ------------------------------------------------------------------------
 * Name: AppObject_customizePrintSetupBox
 *
 * Description:
 *
 *     Adds application specific items to the passed print setup box.
 *
 *     The document file name is presented in the top work area.
 *
 * Return value:
 *
 *     None.
 *
 */
void
AppObject_customizePrintSetupBox(
				 AppObject* me,
				 Widget print_dialog)
{
    Widget row;
    XmString label;
    Widget w;
    /*
     * create the app-specific top work area
     */
    XtVaSetValues(print_dialog,
		  DtNworkAreaLocation, DtWORK_AREA_TOP,
		  NULL);
    row = XtVaCreateManagedWidget(
				  "DocumentNameRow",
				  xmRowColumnWidgetClass,
				  print_dialog,
				  XmNorientation, XmHORIZONTAL,
				  NULL);
    /*
     * create the document name label
     */
    label = XmStringCreateLocalized("Document:");
    w = XtVaCreateManagedWidget("DocumentNameLabel",
			 xmLabelGadgetClass,
			 row,
			 XmNlabelString, label,
			 NULL);
    XmStringFree(label);
    /*
     * create the document name
     */
    label = XmStringCreateLocalized(me->file_name);
    w = XtVaCreateManagedWidget("DocumentName",
			 xmLabelGadgetClass,
			 row,
			 XmNlabelString, label,
			 NULL);
    XmStringFree(label);
}
