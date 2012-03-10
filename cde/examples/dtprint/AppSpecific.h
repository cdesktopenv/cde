/* $XConsortium: AppSpecific.h /main/4 1996/05/09 03:39:24 drk $ */
/*
 * constant definitions
 */
#define APP_CLASS "Dtprint"

/*
 * Application-specific "object"
 */
typedef struct _AppObject
{
    Widget main_window ;
    Widget widget;
    String file_name;
    char * file_buffer ;
    
} AppObject;

/*
 * public AppObject functions
 */
extern AppObject* AppObject_new(
				Widget parent,
				String file_name);
extern void AppObject_customizePrintSetupBox(
					     AppObject* me,
					     Widget print_dialog);
