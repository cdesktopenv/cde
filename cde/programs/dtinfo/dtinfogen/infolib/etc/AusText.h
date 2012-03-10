/* $XConsortium: AusText.h /main/2 1996/07/18 16:38:54 drk $ */
// AusText.h - Specific implementation for AusText search engine

#ifndef AUSTEXT_HEADER
#define AUSTEXT_HEADER

#include "SearchEng.h"

class Token;
class AusTextStore;
class NodeData;
class DataRepository;

class AusText : public SearchEngine {

private:
  DataRepository *store;
  AusTextStore   *f_search_store;

protected:
  void write_start_tag ( const Token &);
  void write_end_tag ( const Token & );
  void write_terms ( FlexBuffer *termsbuf );
  void write_buffer();

public:
  void markup( const Token & );
  void data( const char *, size_t );

public:
  AusText(NodeData *, const Token &);
  ~AusText();

};

#endif
