/* $XConsortium: ShutdownActions.hh /main/3 1996/04/21 19:46:48 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifndef _SHUTDOWNACTIONS_HH
#define _SHUTDOWNACTIONS_HH

class ShutdownActions {
  public:
    ShutdownActions(int num_actions = 32);
    ~ShutdownActions(void);

    typedef int (*ShutdownActionProc)(void *);

    void addAction(ShutdownActionProc, void * cb_data);
    void removeAction(ShutdownActionProc, void * cb_data);

    int doActions(void);

  private:
    struct ActionEntry {
	ShutdownActionProc	proc;
	void *			call_back_data;
    };

    ActionEntry		*_actions;
    int			_action_list_size;
    int			_action_count;

    void removeEntry(int);
};

#endif
