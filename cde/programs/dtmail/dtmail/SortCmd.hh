/* $TOG: SortCmd.hh /main/4 1998/09/21 18:52:24 mgreess $ */
#include "Sort.hh"

// commands for sort menu items

class SortCmd : public ToggleButtonCmd {
  public:
    virtual void doit();   
    SortCmd( char *, char *, int, RoamMenuWindow *, enum sortBy);
    //SortCmd( char *, int, RoamMenuWindow * );
    //virtual const char *const className () { return "SortCmd"; }
  private:
    RoamMenuWindow	*_sortparent;
    enum sortBy		_sortstyle;
    Sort		*_sorter;
};
