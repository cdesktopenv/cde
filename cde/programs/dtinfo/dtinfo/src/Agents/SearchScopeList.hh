/*
 * $XConsortium: SearchScopeList.hh /main/5 1996/09/27 10:16:44 cde-hal $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


class SearchScopeAgent;
class SearchScope;

class SimpleBuffer {
  public:
    SimpleBuffer ();
    virtual ~SimpleBuffer ();

  public:
    void write (char *data);
    char *data ();
  private:
    char *f_data;
    int   f_data_len;
};


class SearchScopeList : public xList<UAS_SearchScope *>,
			public UAS_SearchZones
{
public:
  SearchScopeList (SearchScopeAgent *);
  ~SearchScopeList ();

  void save();
  void restore();
  void create_named_scopes();

  void add_handle(BitHandle, int);
  void remove_handle(UAS_SearchScope *);
  BitHandle lookup_handle(UAS_SearchScope *);
  UAS_BookcaseEntry *create_bcase_entry(UAS_List<UAS_String> &);

private:
  UAS_Pointer<UAS_Common> validate_bookcase(UAS_String &);

private:
  SearchScopeAgent      *f_search_scope_agent;
  SimpleBuffer          *f_buffer;
  xList<BitHandle>       f_handle_list;
};
