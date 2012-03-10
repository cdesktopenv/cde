/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: QueryGroup.hh /main/5 1996/08/30 17:21:35 drk $
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


class QueryTerm;

template <class T> class Stack;

class QueryGroup : public FolioObject
{
public:
  QueryGroup()
    : f_term_list (NULL)
    { }
  ~QueryGroup();
  QueryTerm *term_list()
    { return (f_term_list); }

  enum query_type_t { QUERY_TYPE_FULCRUM, QUERY_TYPE_INFIX };
#ifdef UseQSearch
  const NTerminal *generate_query ();
  const char* generate_readable();
#else
  const char *generate_query (query_type_t query_type = QUERY_TYPE_FULCRUM);
#endif

private: // functions
  void gen_query_internal (query_type_t query_type = QUERY_TYPE_INFIX);
#if 0
  void reduce (query_type_t, int * &operand_stack, 
	       connective_t * &operator_stack,
	       char ** &proximity_stack);
#else
  void reduce (query_type_t, Stack<int> &,
	       Stack<connective_t> &,
	       Stack<char *> &);
#endif
  int format (query_type_t, QueryTerm *term);

private:
  friend class QueryTerm;
  QueryTerm *f_term_list;
  static Buffer g_buffer;
};
