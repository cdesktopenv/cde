/* 
 *  @OSF_COPYRIGHT@
 *  COPYRIGHT NOTICE
 *  Copyright (c) 1990, 1991, 1992, 1993 Open Software Foundation, Inc.
 *  ALL RIGHTS RESERVED (MOTIF). See the file named COPYRIGHT.MOTIF for
 *  the full copyright text.
*/ 
/* 
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: xmbind.c /main/10 1997/06/18 17:34:48 samborn $"
#endif
#endif
#include <stdio.h>
#include <Xm/Xm.h>

/* Internal routines "borrowed" from libXm.  Don't try this at home! */
extern Boolean _XmVirtKeysLoadFileBindings(char *fileName, String *binding);
extern int _XmVirtKeysLoadFallbackBindings(Display *display, String *binding);

void main(argc, argv)
    int argc;
    char **argv;
{
    enum { XA_MOTIF_DEFAULT_BINDINGS, XA_MOTIF_BINDINGS };
    static char *atom_names[] = {
      "_MOTIF_DEFAULT_BINDINGS", "_MOTIF_BINDINGS" };

    Atom atoms[XtNumber(atom_names)];
    XtAppContext  app_context;
    Display *display;
    String bindings = NULL;

    XtToolkitInitialize();
    app_context = XtCreateApplicationContext();
    display = XtOpenDisplay(app_context, NULL, argv[0], "Xmbind",
			NULL, 0, &argc, argv);
    
    if (display == NULL) {
	fprintf(stderr, "%s:  Can't open display\n", argv[0]);
	exit(1);
    }

    XInternAtoms(display, atom_names, XtNumber(atom_names), False, atoms);
    if (argc == 2) {
	if (_XmVirtKeysLoadFileBindings (argv[1], &bindings) == True) {
	    XDeleteProperty (display, RootWindow (display, 0),
			     atoms[XA_MOTIF_DEFAULT_BINDINGS]);
	    XChangeProperty (display, RootWindow(display, 0),
			     atoms[XA_MOTIF_BINDINGS],
			     XA_STRING, 8, PropModeReplace,
			     (unsigned char *)bindings, strlen(bindings));
	    XFlush (display);
	    XtFree (bindings);
	    exit(0);
	}
	else {
	    fprintf(stderr, "%s:  Can't open %s\n", argv[0], argv[1]);
	    exit(1);
	}
    }

    XDeleteProperty (display, RootWindow (display, 0),
		     atoms[XA_MOTIF_BINDINGS]);
    XDeleteProperty (display, RootWindow (display, 0),
		     atoms[XA_MOTIF_DEFAULT_BINDINGS]);

    _XmVirtKeysLoadFallbackBindings (display, &bindings);

    XFlush (display);
    XtFree (bindings);
   
    exit(0);
}

