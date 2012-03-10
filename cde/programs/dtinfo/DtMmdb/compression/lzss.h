/* $XConsortium: lzss.h /main/3 1996/06/11 17:15:21 cde-hal $ */


#ifndef _lzss_h
#define _lzss_h 1

#include "compression/abs_agent.h"

class lzss : public compress_agent
{

public:
   lzss() : compress_agent(DICT_AGENT_CODE) {};
   virtual ~lzss() {};

   virtual void compress(const buffer& uncompressed, buffer& compressed) ;
   virtual void decompress(buffer& compressed, buffer& uncompressed) ;

   MMDB_SIGNATURES(lzss);

   virtual io_status build_dict(lex_func_t f_lex, getchar_func_t f_getchar);
};

extern lzss g_lzss_agent;

#endif
