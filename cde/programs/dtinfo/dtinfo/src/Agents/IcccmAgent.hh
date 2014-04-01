// $XConsortium: IcccmAgent.hh /main/3 1996/06/11 16:12:41 cde-hal $
/*	Copyright (c) 1994,1995 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

#ifndef ICCCM_AGENT_HH
#define ICCCM_AGENT_HH

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Xmu/Atoms.h>

class IcccmAgent;

typedef void (IcccmAgent::* data_handler_t)(const char*, unsigned long);
// Arguments will be required to export non-texural data in the future
typedef char* (IcccmAgent::* data_exporter_t)();
typedef void (IcccmAgent::* own_success_CB_t)();
typedef void (IcccmAgent::* own_fail_CB_t)();

class IcccmAgent
{
public:
    // NOTE: constructor take string handlers, that means, you have
    // to set other handlers and/or exporters later.
    IcccmAgent(void*, data_handler_t, data_exporter_t);
    IcccmAgent(void*, data_handler_t);
    IcccmAgent(void*, data_exporter_t);
	
    virtual ~IcccmAgent();
    data_handler_t  set_string_handler(data_handler_t);
    data_exporter_t set_string_exporter(data_exporter_t);

protected:
    // target_handler is assumed to do:
    // 	1. calls pickup_target to select the best target
    //     if it could not find the best target, returns None
    //  2. make a hook to get valueCB called
    virtual Atom target_handler(Widget, XtPointer, Atom*, Atom*, XtPointer,
							unsigned long*, int*);
    // target_handler is assumed to dispatch each task to appropriate
    // effective handlers according to type
    // It returns None, if it could not dispatch a task
    virtual Atom value_handler(Widget, XtPointer, Atom*, Atom*, XtPointer,
							unsigned long*, int*);
    virtual Atom pickup_target(Widget, Atom*, unsigned long);

    static void get_selection_value(Widget, Atom, IcccmAgent*, Time);

#define GET_SELECTION_VALUE(widget, selection, as_of) \
    get_selection_value(widget, selection, (IcccmAgent*)this, as_of)

    // targetCB simply calls target_handler
    static void targetCB(Widget, XtPointer, Atom*, Atom*, XtPointer,
						unsigned long*, int*);
    // valueCB simply calls value_handler
    static void valueCB (Widget, XtPointer, Atom*, Atom*, XtPointer,
						unsigned long*, int*);

    void own_selection(Widget, Atom, IcccmAgent*, Time,
					own_success_CB_t, own_fail_CB_t);

#define OWN_SELECTION(widget, selection, as_of, success_handler, fail_handler) \
    own_selection(widget, selection, (IcccmAgent*)this, as_of, \
		(own_success_CB_t)success_handler, (own_fail_CB_t)fail_handler)

    // convertCB is a static wrapper to call convert_handler
    static Boolean convertCB(Widget, Atom*, Atom*, Atom*, XtPointer*,
						unsigned long*, int*);
    virtual Boolean convert_handler(Widget, Atom, Atom,
				Atom&, XtPointer&, unsigned long&, int&);

    // NOTE: derived classes MUST provide this, although not pure virtual.
    //	     lose_selection of derived class will be called when selection
    //	     is robbed by another application.
    virtual void lose_selection(Atom);

    // turn_over_selection will be called when selection is turned over to
    // another (including himself) IcccmAgent.
    virtual void turn_over_selection(Atom) { };

    // True if I am the selection owner
    Boolean is_selection_owner();

    // True if another (including me) IcccmAgent is about to get selection
    Boolean ia_getting_selection();

    // Disown selection voluntarily, if I am the owner
    void disown_selection(Boolean call_app_handler = True);

#if 0
    void set_eps_handler(data_handler_t);
    void set_epsi_handler(data_handler_t);
    void set_all_handlers(data_handler_t);
#endif

private:

    // loseCB is a static wrapper to call lose_handler
    static void loseCB(Widget, Atom*);

    void reset_selection_info();
    IcccmAgent* selection_owner();

    static const char* extract_ascii(const char*);

    void* f_real_object;
    data_handler_t	f_string_handler; 
#if 0
    data_handler_t	f_eps_handler; 
    data_handler_t	f_epsi_handler; 
#endif
    data_exporter_t	f_string_exporter;

    static Time f_paste_activated_time;

    // Need this flag to know if we lost the selection to ourself or
    // to another application in the lose_selection procedure.
    static int f_getting_selection;
    static int f_voluntary_disown;

    // who owns the selection
    static IcccmAgent*		f_selection_owner;
    static Atom			f_owning_selection;
    static Widget		f_owning_widget;

};

#endif
