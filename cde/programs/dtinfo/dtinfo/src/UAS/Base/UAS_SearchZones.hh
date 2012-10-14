// $XConsortium: UAS_SearchZones.hh /main/5 1996/09/27 10:21:49 cde-hal $
#ifndef _UAS_SearchZones_hh_
#define _UAS_SearchZones_hh_

#include "unistd.h"

#include "UAS_String.hh"

//typedef unsigned char mbool;

class UAS_SearchZones
{
  public:
    
    UAS_SearchZones() : f_zones(0), f_search_section(NULL) {}
    ~UAS_SearchZones() {}

    void titles(bool b) {
	if (b) f_zones |= f_titles;
	else f_zones &= ~f_titles; }
    bool titles() const { return (bool) (f_zones&f_titles); }

    void bodies(bool b) {
	if (b) f_zones |= f_bodies;
	else f_zones &= ~f_bodies; }
    bool bodies() const { return (bool) (f_zones&f_bodies); }

    void examples(bool b) {
	if (b) f_zones |= f_examples;
	else f_zones &= ~f_examples; }
    bool examples() const { return (bool) (f_zones&f_examples); }

    void indexes(bool b) {
	if (b) f_zones |= f_indexes;
	else f_zones &= ~f_indexes; }
    bool indexes() const { return (bool) (f_zones&f_indexes); }

    void tables(bool b) {
	if (b) f_zones |= f_tables;
	else f_zones &= ~f_tables; }
    bool tables() const { return (bool) (f_zones&f_tables); }

    void graphics(bool b) {
	if (b) f_zones |= f_graphics;
	else f_zones &= ~f_graphics; }
    bool graphics() const { return (bool) (f_zones&f_graphics); }

    void all(bool b) {
	if (b) f_zones |= f_all;
	else f_zones &= ~f_all; }
    bool all() const { return (bool) ((f_zones&f_titles)   &&
                                      (f_zones&f_bodies)   &&
                                      (f_zones&f_examples) &&
                                      (f_zones&f_indexes)  &&
                                      (f_zones&f_tables)   &&
                                      (f_zones&f_graphics)) ; }

    void section(const char * s) {
	f_search_section = s;
	f_zones = f_section; }	// resets other flags
    bool section() const { return (bool) (f_zones & f_section); }
    const UAS_String search_section() const { return f_search_section; }

    void zones(u_int zones) { f_zones = zones; }
    u_int zones() { return f_zones; }

    enum uas_zones { uas_inv = -1,
		     uas_titles = 0, uas_bodies, uas_examples, uas_indexes,
		     uas_tables, uas_graphics, uas_section,
		     uas_all = uas_section };

  protected:
    enum flags { f_titles   = 0x1 << uas_titles,
		 f_bodies   = 0x1 << uas_bodies,
		 f_examples = 0x1 << uas_examples,
		 f_indexes  = 0x1 << uas_indexes,
		 f_tables   = 0x1 << uas_tables,
		 f_graphics = 0x1 << uas_graphics,
		 f_section  = 0x1 << uas_section,
		 f_all	    = f_section - 1 };

  private:
    unsigned int	f_zones;
    UAS_String          f_search_section;

};

#endif
