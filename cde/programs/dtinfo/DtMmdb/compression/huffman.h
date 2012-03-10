/*
 * $XConsortium: huffman.h /main/3 1996/06/11 17:15:11 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
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


#ifndef _huff_h
#define _huff_h 1

#include "compression/abs_agent.h"
#include "compression/trie.h"

////////////////////////////////////////
//
////////////////////////////////////////
class htr_node 
{
public:
   htr_node* parent;
   htr_node* left;
   htr_node* right;
   unsigned long freq; 
   encoding_unit* eu;

public:
   htr_node(encoding_unit* eu, htr_node* lt = 0, htr_node* rt = 0);
   htr_node(unsigned long freq, htr_node* lt = 0, htr_node* rt = 0);
   ~htr_node();
};


////////////////////////////////////////
//
////////////////////////////////////////
class huff : public compress_agent
{

protected:
   htr_node* htr_root;
   encoding_unit** e_units;
   trie* tri;
   unsigned int cts ;

protected:
   void build_tree();
   void calculate_code();
   encoding_unit* get_e_unit(unsigned char*& data, int len);

public:
   huff();
   virtual ~huff() ;

   virtual void compress(const buffer& uncompressed, buffer& compressed) ;
   virtual void decompress(buffer& compressed, buffer& uncompressed) ;

   ostream& print_alphabet(ostream& out);

   MMDB_SIGNATURES(huff);

   // compacted disk representation In and Out functions
   virtual int cdr_sizeof();
   virtual io_status cdrOut(buffer&);
   virtual io_status cdrIn(buffer&);

// get data to compute the alphabet
   virtual io_status build_dict(lex_func_t f_lex, getchar_func_t f_getchar);
};

extern huff g_huff_agent;

#endif
