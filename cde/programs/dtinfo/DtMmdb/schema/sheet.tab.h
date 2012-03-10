/* $XConsortium: sheet.tab.h /main/3 1996/06/11 17:46:46 cde-hal $ */
#define TOKEN 257
#define CONTAINER 258
#define SET 259
#define LIST 260
#define INDEX_NAME 261
#define INV 262
#define COMPRESS 263
#define INV_NAME 264
#define AGENT_NAME 265
#define STORE_NAME 266
#define POSITION 267
#define INDEX 268
#define MPHF_INDEX 269
#define SMPHF_INDEX 270
#define BTREE_INDEX 271
#define INDEX_AGENT 272
#define MPHF 273
#define SMPHF 274
#define BTREE 275
#define HUFFMAN 276
#define DICT 277
#define EQUAL 278
#define NUMBER 279
#define STORE 280
#define PAGE_STORE 281
#define NM 282
#define V_OID 283
#define MODE 284
#define PAGE_SZ 285
#define CACHED_PAGES 286
#define BYTE_ORDER 287
#define SEPARATOR 288
typedef union
{
 char   *string;
 int    integer;
 desc*  trans;
 page_store_desc*  ps_trans;
} YYSTYPE;
extern YYSTYPE  schemalval;
