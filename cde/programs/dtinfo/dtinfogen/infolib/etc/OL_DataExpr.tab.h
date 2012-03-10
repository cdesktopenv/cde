/* $XConsortium: OL_DataExpr.tab.h /main/3 1996/08/21 15:57:56 drk $ */
#define Reference 257
#define Id 258
#define Literal 259
#define Content 260
#define Concat 261
#define Attr 262
#define FirstOf 263
typedef union {
  int           name;
  char          *string;
  OL_Expression *eptr;
} YYSTYPE;
extern YYSTYPE ol_datalval;
