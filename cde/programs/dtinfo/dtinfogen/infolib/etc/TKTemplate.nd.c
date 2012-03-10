/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: TKTemplate.nd.c /main/2 1996/08/21 15:47:40 drk $ */

/* << AT&T C++ Translator 3.0.2 - ObjectCenter Version 2.1>> */
char __cfront_version_302_xxxxxxxx;
char __centerline_cfront;
/* < TKTemplate.cc > */
typedef int (*__vptp)();
struct __mptr {short d; short i; __vptp f; };
extern struct __mptr* __ptbl_vec__TKTemplate_cc_[];


static char ______dummy1_ (){ 
extern int ______setjmp ();
extern int _______setjmp ();
extern int ______sigsetjmp ();
}

typedef int jmp_buf [9];

typedef int sigjmp_buf [10];

#pragma unknown_control_flow(______sigsetjmp , ______setjmp , _______setjmp )

static char ______dummy2_ (){ 
extern int setjmp ();
extern int _setjmp ();
extern int sigsetjmp ();
}

#pragma unknown_control_flow(setjmp,_setjmp,sigsetjmp)

extern int errno ;

extern int sys_nerr ;
extern char *sys_errlist [];

typedef int sigset_t ;

typedef unsigned int speed_t ;
typedef unsigned long tcflag_t ;
typedef unsigned char cc_t ;
typedef int pid_t ;

typedef unsigned short mode_t ;
typedef short nlink_t ;

typedef long clock_t ;
typedef long time_t ;

typedef int ______size_t ;
typedef int ptrdiff_t ;

typedef unsigned short wchar_t ;

typedef unsigned int size_t ;

struct _iobuf {	/* sizeof _iobuf == 20 */
int _cnt__6_iobuf ;
unsigned char *_ptr__6_iobuf ;
unsigned char *_base__6_iobuf ;
int _bufsiz__6_iobuf ;
short _flag__6_iobuf ;
char _file__6_iobuf ;
};

extern struct _iobuf _iob [];

typedef long fpos_t ;

typedef unsigned char u_char ;
typedef unsigned short u_short ;
typedef unsigned int u_int ;
typedef unsigned long u_long ;
typedef unsigned short ushort ;
typedef unsigned int uint ;

typedef struct _physadr_t *physadr_t ;

typedef struct _quad_t quad_t ;
typedef long daddr_t ;
typedef char *caddr_t ;
typedef unsigned long ino_t ;
typedef short dev_t ;
typedef long off_t ;
typedef unsigned short uid_t ;
typedef unsigned short gid_t ;
typedef long key_t ;
typedef char *addr_t ;

typedef long fd_mask ;

extern unsigned int _mb_cur_max ;

extern char *optarg ;
extern int optind ;

extern int opterr ;
extern char **environ ;
extern int end ;

extern int etext ;

extern int edata ;

typedef char *malloc_t ;

extern char _ctype_ [];

typedef char (* __Q2_10Exceptions15error_handler_t )();

typedef unsigned int CC_Boolean ;

typedef long streampos ;
typedef long streamoff ;
enum __Q2_3ios8io_state { goodbit__Q2_3ios8io_state = 0, eofbit__Q2_3ios8io_state = 1, failbit__Q2_3ios8io_state = 2, badbit__Q2_3ios8io_state = 4, hardfail__Q2_3ios8io_state = 128} ;
enum __Q2_3ios9open_mode { in__Q2_3ios9open_mode = 1, out__Q2_3ios9open_mode = 2, ate__Q2_3ios9open_mode = 4, app__Q2_3ios9open_mode = 8, trunc__Q2_3ios9open_mode = 16, nocreate__Q2_3ios9open_mode = 32, noreplace__Q2_3ios9open_mode = 64} ;
enum __Q2_3ios8seek_dir { beg__Q2_3ios8seek_dir = 0, cur__Q2_3ios8seek_dir = 1, end__Q2_3ios8seek_dir = 2} ;
enum __Q2_3ios4__E9 { skipws__Q2_3ios4__E9 = 1, left__Q2_3ios4__E9 = 2, right__Q2_3ios4__E9 = 4, internal__Q2_3ios4__E9 = 8, dec__Q2_3ios4__E9 = 16, oct__Q2_3ios4__E9 = 32, hex__Q2_3ios4__E9 = 64, showbase__Q2_3ios4__E9 = 128, showpoint__Q2_3ios4__E9 = 256, uppercase__Q2_3ios4__E9 = 512, showpos__Q2_3ios4__E9 = 1024, scientific__Q2_3ios4__E9 = 2048, fixed__Q2_3ios4__E9 = 4096, unitbuf__Q2_3ios4__E9 = 8192,
stdio__Q2_3ios4__E9 = 16384} ;
enum __Q2_3ios5__E10 { skipping__Q2_3ios5__E10 = 512, tied__Q2_3ios5__E10 = 1024} ;
struct ios {	/* sizeof ios == 60 */

int nuser__3ios ;

union ios_user_union *x_user__3ios ;

struct streambuf *bp__3ios ;

int state__3ios ;
int ispecial__3ios ;
int ospecial__3ios ;
int isfx_special__3ios ;
int osfx_special__3ios ;
int delbuf__3ios ;
struct ostream *x_tie__3ios ;
long x_flags__3ios ;
short x_precision__3ios ;
char x_fill__3ios ;
short x_width__3ios ;

int assign_private__3ios ;

struct __mptr *__vptr__3ios ;
};

extern long basefield__3ios ;

extern long adjustfield__3ios ;

extern long floatfield__3ios ;

extern long nextbit__3ios ;
extern long nextword__3ios ;

extern char (*stdioflush__3ios )();

struct istream {	/* sizeof istream == 72 */

int x_gcount__7istream ;

struct __mptr *__vptr__7istream ;
struct ios *Pios;
struct ios Oios;
};

struct istream_withassign {	/* sizeof istream_withassign == 72 */

int x_gcount__7istream ;

struct __mptr *__vptr__7istream ;
struct ios *Pios;
struct ios Oios;
};

extern struct istream_withassign cin ;

struct ostream {	/* sizeof ostream == 68 */

struct __mptr *__vptr__7ostream ;
struct ios *Pios;
struct ios Oios;
};

struct ostream_withassign {	/* sizeof ostream_withassign == 68 */

struct __mptr *__vptr__7ostream ;
struct ios *Pios;
struct ios Oios;
};

extern struct ostream_withassign cout ;
extern struct ostream_withassign cerr ;
extern struct ostream_withassign clog ;

struct Iostream_init {	/* sizeof Iostream_init == 1 */

char __W29__13Iostream_init ;
};

extern int stdstatus__13Iostream_init ;
extern int initcount__13Iostream_init ;

struct Iostream_init *__ct__13Iostream_initFv ();
char __dt__13Iostream_initFv ();
static struct Iostream_init iostream_init ;

typedef struct Stack__pt__2_i _f1_ ;
typedef struct CC_TPtrSlist__pt__11_9CC_String _f2_ ;
typedef struct hashTable__pt__26_9CC_String13BTCollectable _f3_ ;
typedef struct hashTable__pt__12_9CC_Stringi _f4_ ;
typedef struct hashTableIterator__pt__26_9CC_String13BTCollectable _f5_ ;
typedef struct hashTableIterator__pt__12_9CC_Stringi _f6_ ;

struct hashTableIterator__pt__12_9CC_Stringi {	/* sizeof hashTableIterator__pt__12_9CC_Stringi == 16 */

size_t f_bucket_num__37hashTableIterator__pt__12_9CC_Stringi ;
size_t f_pos__37hashTableIterator__pt__12_9CC_Stringi ;
struct kv_pair__pt__12_9CC_Stringi *f_rec__37hashTableIterator__pt__12_9CC_Stringi ;
struct hashTable__pt__12_9CC_Stringi *f_hashTable__37hashTableIterator__pt__12_9CC_Stringi ;
};

extern char *__nw__FUi ();

struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi {	/* sizeof pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi == 12 */

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi **f_array__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ;
size_t f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ;
size_t f_items__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ;
};

struct hashTable__pt__12_9CC_Stringi {	/* sizeof hashTable__pt__12_9CC_Stringi == 20 */

struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi f_buckets__29hashTable__pt__12_9CC_Stringi ;
unsigned int (*f_hash_func_ptr__29hashTable__pt__12_9CC_Stringi )();
size_t f_items__29hashTable__pt__12_9CC_Stringi ;
};

struct hashTableIterator__pt__12_9CC_Stringi *__ct__37hashTableIterator__pt__12_9CC_StringiFR29hashTable__pt__12_9CC_Stringi (__0this , __1b )struct hashTableIterator__pt__12_9CC_Stringi *__0this ;

struct hashTable__pt__12_9CC_Stringi *__1b ;

{ if (__0this || (__0this = (struct hashTableIterator__pt__12_9CC_Stringi *)__nw__FUi ( sizeof (struct hashTableIterator__pt__12_9CC_Stringi)) ))( ( ( (__0this -> f_bucket_num__37hashTableIterator__pt__12_9CC_Stringi = 0 ),
(__0this -> f_pos__37hashTableIterator__pt__12_9CC_Stringi = 0 )) , (__0this -> f_rec__37hashTableIterator__pt__12_9CC_Stringi = 0 )) , (__0this -> f_hashTable__37hashTableIterator__pt__12_9CC_Stringi = __1b )) ;

return __0this ;
}

extern char __dl__FPv ();

char __dt__37hashTableIterator__pt__12_9CC_StringiFv (__0this , __0__free )struct hashTableIterator__pt__12_9CC_Stringi *__0this ;

int __0__free ;

{ if (__0this )
if (__0this )if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;

}

struct CC_Listbase {	/* sizeof CC_Listbase == 12 */

struct CC_Link_base *f_head__11CC_Listbase ;
struct CC_Link_base *f_tail__11CC_Listbase ;
size_t f_length__11CC_Listbase ;
};

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi **__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi ();

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi {	/* sizeof CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi == 20 */

struct CC_Link_base *f_head__11CC_Listbase ;
struct CC_Link_base *f_tail__11CC_Listbase ;
size_t f_length__11CC_Listbase ;

CC_Boolean destructed__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ;

struct __mptr *__vptr__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ;
};

CC_Boolean _findNonEmptyBucket__37hashTableIterator__pt__12_9CC_StringiFv (__0this )register struct hashTableIterator__pt__12_9CC_Stringi *__0this ;
{ 
struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__1b ;

__1b = 0 ;

for(;__0this -> f_bucket_num__37hashTableIterator__pt__12_9CC_Stringi < ( ((struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& ((*__0this -> f_hashTable__37hashTableIterator__pt__12_9CC_Stringi )). f_buckets__29hashTable__pt__12_9CC_Stringi ))-> f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ) ;__0this -> f_bucket_num__37hashTableIterator__pt__12_9CC_Stringi ++ ) { 
if
((__1b = ((*__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi ( & ((*__0this -> f_hashTable__37hashTableIterator__pt__12_9CC_Stringi )). f_buckets__29hashTable__pt__12_9CC_Stringi , __0this -> f_bucket_num__37hashTableIterator__pt__12_9CC_Stringi ) )))&& (( ((struct CC_Listbase *)__1b )-> f_length__11CC_Listbase ) >
0 )){ 
__0this -> f_pos__37hashTableIterator__pt__12_9CC_Stringi = 0 ;
return (unsigned int )1 ;
}
}

return (unsigned int )0 ;
}

struct kv_pair__pt__12_9CC_Stringi *at__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiCFUi ();

CC_Boolean _findNextRecord__37hashTableIterator__pt__12_9CC_StringiFv ();

CC_Boolean __pp__37hashTableIterator__pt__12_9CC_StringiFv (__0this )register struct hashTableIterator__pt__12_9CC_Stringi *__0this ;
{ 
if (__0this -> f_rec__37hashTableIterator__pt__12_9CC_Stringi == 0 ){ 
if (_findNonEmptyBucket__37hashTableIterator__pt__12_9CC_StringiFv ( __0this ) == 0 )
return (unsigned int )0 ;

}
else 
{ 
if (_findNextRecord__37hashTableIterator__pt__12_9CC_StringiFv ( __0this ) == 0 ){ 
__0this -> f_bucket_num__37hashTableIterator__pt__12_9CC_Stringi ++ ;
if (_findNonEmptyBucket__37hashTableIterator__pt__12_9CC_StringiFv ( __0this ) == 0 )
return (unsigned int )0 ;
}
}

__0this -> f_rec__37hashTableIterator__pt__12_9CC_Stringi = at__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiCFUi ( (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)((*__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi ( & ((*__0this -> f_hashTable__37hashTableIterator__pt__12_9CC_Stringi )). f_buckets__29hashTable__pt__12_9CC_Stringi , __0this -> f_bucket_num__37hashTableIterator__pt__12_9CC_Stringi ) )), __0this -> f_pos__37hashTableIterator__pt__12_9CC_Stringi )
;
return (unsigned int )1 ;
}

CC_Boolean _findNextRecord__37hashTableIterator__pt__12_9CC_StringiFv (__0this )register struct hashTableIterator__pt__12_9CC_Stringi *__0this ;
{ 
struct CC_Listbase *__0__X78 ;

__0this -> f_pos__37hashTableIterator__pt__12_9CC_Stringi ++ ;

if (( (__0__X78 = (struct CC_Listbase *)((*__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi ( & ((*__0this -> f_hashTable__37hashTableIterator__pt__12_9CC_Stringi )). f_buckets__29hashTable__pt__12_9CC_Stringi , __0this -> f_bucket_num__37hashTableIterator__pt__12_9CC_Stringi ) ))), ( __0__X78 ->
f_length__11CC_Listbase ) ) <= __0this -> f_pos__37hashTableIterator__pt__12_9CC_Stringi )
return (unsigned int )0 ;
else 
return (unsigned int )1 ;
}

struct kv_pair__pt__12_9CC_Stringi {	/* sizeof kv_pair__pt__12_9CC_Stringi == 8 */

struct CC_String *f_key__27kv_pair__pt__12_9CC_Stringi ;
int *f_value__27kv_pair__pt__12_9CC_Stringi ;
};

extern CC_Boolean f_needRemove__27kv_pair__pt__12_9CC_Stringi ;

struct CC_String *key__37hashTableIterator__pt__12_9CC_StringiFv (__0this )struct hashTableIterator__pt__12_9CC_Stringi *__0this ;
{ 
return __0this -> f_rec__37hashTableIterator__pt__12_9CC_Stringi -> f_key__27kv_pair__pt__12_9CC_Stringi ;
}

int *value__37hashTableIterator__pt__12_9CC_StringiCFv (__0this )struct hashTableIterator__pt__12_9CC_Stringi *__0this ;
{ 
return __0this -> f_rec__37hashTableIterator__pt__12_9CC_Stringi -> f_value__27kv_pair__pt__12_9CC_Stringi ;
}

struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable {	/* sizeof pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable == 12 */

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable **f_array__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ;
size_t f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ;
size_t f_items__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ;
};

struct hashTable__pt__26_9CC_String13BTCollectable {	/* sizeof hashTable__pt__26_9CC_String13BTCollectable == 20 */

struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable f_buckets__43hashTable__pt__26_9CC_String13BTCollectable ;
unsigned int (*f_hash_func_ptr__43hashTable__pt__26_9CC_String13BTCollectable )();
size_t f_items__43hashTable__pt__26_9CC_String13BTCollectable ;
};

struct hashTableIterator__pt__26_9CC_String13BTCollectable {	/* sizeof hashTableIterator__pt__26_9CC_String13BTCollectable == 16 */

size_t f_bucket_num__51hashTableIterator__pt__26_9CC_String13BTCollectable ;
size_t f_pos__51hashTableIterator__pt__26_9CC_String13BTCollectable ;
struct kv_pair__pt__26_9CC_String13BTCollectable *f_rec__51hashTableIterator__pt__26_9CC_String13BTCollectable ;
struct hashTable__pt__26_9CC_String13BTCollectable *f_hashTable__51hashTableIterator__pt__26_9CC_String13BTCollectable ;
};

struct hashTableIterator__pt__26_9CC_String13BTCollectable *__ct__51hashTableIterator__pt__26_9CC_String13BTCollectableFR43hashTable__pt__26_9CC_String13BTCollectable (__0this , __1b )struct hashTableIterator__pt__26_9CC_String13BTCollectable *__0this ;

struct hashTable__pt__26_9CC_String13BTCollectable *__1b ;

{ if (__0this || (__0this = (struct hashTableIterator__pt__26_9CC_String13BTCollectable *)__nw__FUi ( sizeof (struct hashTableIterator__pt__26_9CC_String13BTCollectable)) ))( ( ( (__0this -> f_bucket_num__51hashTableIterator__pt__26_9CC_String13BTCollectable = 0 ),
(__0this -> f_pos__51hashTableIterator__pt__26_9CC_String13BTCollectable = 0 )) , (__0this -> f_rec__51hashTableIterator__pt__26_9CC_String13BTCollectable = 0 )) , (__0this -> f_hashTable__51hashTableIterator__pt__26_9CC_String13BTCollectable = __1b )) ;

return __0this ;
}

char __dt__51hashTableIterator__pt__26_9CC_String13BTCollectableFv (__0this , __0__free )struct hashTableIterator__pt__26_9CC_String13BTCollectable *__0this ;

int __0__free ;

{ if (__0this )
if (__0this )if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;

}

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable **__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi ();

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable {	/* sizeof CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable == 20 */

struct CC_Link_base *f_head__11CC_Listbase ;
struct CC_Link_base *f_tail__11CC_Listbase ;
size_t f_length__11CC_Listbase ;

CC_Boolean destructed__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ;

struct __mptr *__vptr__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ;
};

CC_Boolean _findNonEmptyBucket__51hashTableIterator__pt__26_9CC_String13BTCollectableFv (__0this )register struct hashTableIterator__pt__26_9CC_String13BTCollectable *__0this ;
{ 
struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__1b ;

__1b = 0 ;

for(;__0this -> f_bucket_num__51hashTableIterator__pt__26_9CC_String13BTCollectable < ( ((struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& ((*__0this -> f_hashTable__51hashTableIterator__pt__26_9CC_String13BTCollectable )). f_buckets__43hashTable__pt__26_9CC_String13BTCollectable ))-> f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ) ;__0this -> f_bucket_num__51hashTableIterator__pt__26_9CC_String13BTCollectable ++ ) { 
if
((__1b = ((*__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi ( & ((*__0this -> f_hashTable__51hashTableIterator__pt__26_9CC_String13BTCollectable )). f_buckets__43hashTable__pt__26_9CC_String13BTCollectable , __0this -> f_bucket_num__51hashTableIterator__pt__26_9CC_String13BTCollectable ) )))&& (( ((struct CC_Listbase *)__1b )-> f_length__11CC_Listbase ) >
0 )){ 
__0this -> f_pos__51hashTableIterator__pt__26_9CC_String13BTCollectable = 0 ;
return (unsigned int )1 ;
}
}

return (unsigned int )0 ;
}

struct kv_pair__pt__26_9CC_String13BTCollectable *at__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableCFUi ();

CC_Boolean _findNextRecord__51hashTableIterator__pt__26_9CC_String13BTCollectableFv ();

CC_Boolean __pp__51hashTableIterator__pt__26_9CC_String13BTCollectableFv (__0this )register struct hashTableIterator__pt__26_9CC_String13BTCollectable *__0this ;
{ 
if (__0this -> f_rec__51hashTableIterator__pt__26_9CC_String13BTCollectable == 0 ){ 
if (_findNonEmptyBucket__51hashTableIterator__pt__26_9CC_String13BTCollectableFv ( __0this ) == 0 )
return (unsigned int )0 ;

}
else 
{ 
if (_findNextRecord__51hashTableIterator__pt__26_9CC_String13BTCollectableFv ( __0this ) == 0 ){ 
__0this -> f_bucket_num__51hashTableIterator__pt__26_9CC_String13BTCollectable ++ ;
if (_findNonEmptyBucket__51hashTableIterator__pt__26_9CC_String13BTCollectableFv ( __0this ) == 0 )
return (unsigned int )0 ;
}
}

__0this -> f_rec__51hashTableIterator__pt__26_9CC_String13BTCollectable = at__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableCFUi ( (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)((*__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi ( & ((*__0this -> f_hashTable__51hashTableIterator__pt__26_9CC_String13BTCollectable )). f_buckets__43hashTable__pt__26_9CC_String13BTCollectable , __0this -> f_bucket_num__51hashTableIterator__pt__26_9CC_String13BTCollectable ) )), __0this -> f_pos__51hashTableIterator__pt__26_9CC_String13BTCollectable )
;
return (unsigned int )1 ;
}

CC_Boolean _findNextRecord__51hashTableIterator__pt__26_9CC_String13BTCollectableFv (__0this )register struct hashTableIterator__pt__26_9CC_String13BTCollectable *__0this ;
{ 
struct CC_Listbase *__0__X79 ;

__0this -> f_pos__51hashTableIterator__pt__26_9CC_String13BTCollectable ++ ;

if (( (__0__X79 = (struct CC_Listbase *)((*__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi ( & ((*__0this -> f_hashTable__51hashTableIterator__pt__26_9CC_String13BTCollectable )). f_buckets__43hashTable__pt__26_9CC_String13BTCollectable , __0this -> f_bucket_num__51hashTableIterator__pt__26_9CC_String13BTCollectable ) ))), ( __0__X79 ->
f_length__11CC_Listbase ) ) <= __0this -> f_pos__51hashTableIterator__pt__26_9CC_String13BTCollectable )
return (unsigned int )0 ;
else 
return (unsigned int )1 ;
}

struct kv_pair__pt__26_9CC_String13BTCollectable {	/* sizeof kv_pair__pt__26_9CC_String13BTCollectable == 8 */

struct CC_String *f_key__41kv_pair__pt__26_9CC_String13BTCollectable ;
struct BTCollectable *f_value__41kv_pair__pt__26_9CC_String13BTCollectable ;
};

extern CC_Boolean f_needRemove__41kv_pair__pt__26_9CC_String13BTCollectable ;

struct CC_String *key__51hashTableIterator__pt__26_9CC_String13BTCollectableFv (__0this )struct hashTableIterator__pt__26_9CC_String13BTCollectable *__0this ;
{ 
return __0this -> f_rec__51hashTableIterator__pt__26_9CC_String13BTCollectable -> f_key__41kv_pair__pt__26_9CC_String13BTCollectable ;
}

struct BTCollectable *value__51hashTableIterator__pt__26_9CC_String13BTCollectableCFv (__0this )struct hashTableIterator__pt__26_9CC_String13BTCollectable *__0this ;
{ 
return __0this -> f_rec__51hashTableIterator__pt__26_9CC_String13BTCollectable -> f_value__41kv_pair__pt__26_9CC_String13BTCollectable ;
}

struct ostream *__ls__7ostreamFPCc ();

struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__ct__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFRC75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ();

extern struct ostream *endl__FR7ostream ();

extern char exit ();

struct hashTable__pt__12_9CC_Stringi *__ct__29hashTable__pt__12_9CC_StringiFRC29hashTable__pt__12_9CC_Stringi (__0this , __1h )register struct hashTable__pt__12_9CC_Stringi *__0this ;

struct hashTable__pt__12_9CC_Stringi *__1h ;

{ 
struct ostream *__0__X80 ;

struct ostream *(*__2__X81 )();

if (__0this || (__0this = (struct hashTable__pt__12_9CC_Stringi *)__nw__FUi ( sizeof (struct hashTable__pt__12_9CC_Stringi)) )){ ( ( __ct__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFRC75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ( & __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi ,
(struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& ((*__1h )). f_buckets__29hashTable__pt__12_9CC_Stringi )) , (__0this -> f_hash_func_ptr__29hashTable__pt__12_9CC_Stringi = ((*__1h )). f_hash_func_ptr__29hashTable__pt__12_9CC_Stringi )) , (__0this -> f_items__29hashTable__pt__12_9CC_Stringi = ((*__1h )). f_items__29hashTable__pt__12_9CC_Stringi ))
;
( (__0__X80 = (struct ostream *)__ls__7ostreamFPCc ( (struct ostream *)(& cerr ), (char *)"Warning: hashTable(const hashTable&) called") ),
( (__2__X81 = (endl__FR7ostream )), ( ((*__2__X81 ))( (struct ostream *)__0__X80 ) ) ) ) ;
exit ( -1) ;
} 
return __0this ;
}

struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__ct__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUiP50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ();

struct hashTable__pt__12_9CC_Stringi *__ct__29hashTable__pt__12_9CC_StringiFPFRC9CC_String_UiUi (__0this , __1f , __1init_bucket_num )struct hashTable__pt__12_9CC_Stringi *__0this ;

unsigned int (*__1f )();

size_t __1init_bucket_num ;

{ if (__0this || (__0this = (struct hashTable__pt__12_9CC_Stringi *)__nw__FUi ( sizeof (struct hashTable__pt__12_9CC_Stringi)) ))( ( __ct__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUiP50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ( & __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi ,
__1init_bucket_num , (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)0 ) , (__0this -> f_hash_func_ptr__29hashTable__pt__12_9CC_Stringi = __1f )) , (__0this -> f_items__29hashTable__pt__12_9CC_Stringi = 0 )) ;

return __0this ;
}

char __dt__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFv ();

char __dt__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFv ();

char __dt__27kv_pair__pt__12_9CC_StringiFv ();

struct CC_Link_base {	/* sizeof CC_Link_base == 8 */

struct CC_Link_base *f_next__12CC_Link_base ;
struct CC_Link_base *f_prev__12CC_Link_base ;
};

struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi {	/* sizeof CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi == 12 */

struct CC_Link_base *f_next__12CC_Link_base ;
struct CC_Link_base *f_prev__12CC_Link_base ;

struct kv_pair__pt__12_9CC_Stringi *f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi ;
};

struct CC_Link_base *removeFirst__11CC_ListbaseFv ();

char __dt__29hashTable__pt__12_9CC_StringiFv (__0this , __0__free )register struct hashTable__pt__12_9CC_Stringi *__0this ;

int __0__free ;

{ 
struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__1b ;
struct kv_pair__pt__12_9CC_Stringi *__1r ;

if (__0this ){ 
__1b = 0 ;
__1r = 0 ;

{ { int __1i ;

__1i = 0 ;

for(;__1i < ( ((struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi ))-> f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ) ;__1i ++ ) { 
__1b = ((*__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi ( & __0this ->
f_buckets__29hashTable__pt__12_9CC_Stringi , (unsigned int )__1i ) ));

if (__1b ){ 
struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xt00athucfm ;

struct kv_pair__pt__12_9CC_Stringi *__3__Xret00athucfm ;

while (__1r = ( (__2__Xt00athucfm = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)removeFirst__11CC_ListbaseFv ( (struct CC_Listbase *)__1b ) ))))), (__2__Xt00athucfm ?( (__3__Xret00athucfm = __2__Xt00athucfm -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi ),
( __dl__FPv ( (char *)__2__Xt00athucfm ) , __3__Xret00athucfm ) ) :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) )
__dt__27kv_pair__pt__12_9CC_StringiFv ( __1r , 3) ;

__dt__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFv ( __1b , 3) ;
}
}

}

}

if (__0this ){ __dt__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFv ( & __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi , 2) ;

if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} } 
}

char clearAndDestroy__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFv ();

char clearAndDestroy__29hashTable__pt__12_9CC_StringiFv (__0this )register struct hashTable__pt__12_9CC_Stringi *__0this ;
{ 
f_needRemove__27kv_pair__pt__12_9CC_Stringi = 1 ;

{ struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__1b ;

__1b = 0 ;

{ { int __1i ;

__1i = 0 ;

for(;__1i < ( ((struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi ))-> f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ) ;__1i ++ ) { 
__1b = ((*__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi ( & __0this ->
f_buckets__29hashTable__pt__12_9CC_Stringi , (unsigned int )__1i ) ));
if (__1b ){ 
((*(((char (*)())(__1b -> __vptr__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi [1]).f))))( ((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)((((char *)__1b ))+ (__1b -> __vptr__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi [1]).d))) ;
__dt__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFv ( __1b , 3) ;
((*__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi ( & __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi , (unsigned int )__1i ) ))= 0 ;
}
}

__0this -> f_items__29hashTable__pt__12_9CC_Stringi = 0 ;

f_needRemove__27kv_pair__pt__12_9CC_Stringi = 0 ;

}

}

}
}

int *findValue__29hashTable__pt__12_9CC_StringiCFPC9CC_String ();

CC_Boolean contains__29hashTable__pt__12_9CC_StringiCFPC9CC_String (__0this , __1k )struct hashTable__pt__12_9CC_Stringi *__0this ;

struct CC_String *__1k ;
{ 
if (findValue__29hashTable__pt__12_9CC_StringiCFPC9CC_String ( __0this , __1k ) )
return (unsigned int )1 ;
else 
return (unsigned int )0 ;

}

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiCFUi ();
enum __Q2_9CC_String11caseCompare { exact__Q2_9CC_String11caseCompare = 0, ignoreCase__Q2_9CC_String11caseCompare = 1} ;

struct CC_String {	/* sizeof CC_String == 8 */

char *f_string__9CC_String ;
struct __mptr *__vptr__9CC_String ;
};

struct kv_pair__pt__12_9CC_Stringi *find__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiCFPC27kv_pair__pt__12_9CC_Stringi ();

struct kv_pair__pt__12_9CC_Stringi *_find__29hashTable__pt__12_9CC_StringiCFPC9CC_String (__0this , __1k )register struct hashTable__pt__12_9CC_Stringi *__0this ;

struct CC_String *__1k ;
{ 
size_t __1i ;

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__1b ;

struct kv_pair__pt__12_9CC_Stringi *__0_result ;

__1i = (((*__0this -> f_hash_func_ptr__29hashTable__pt__12_9CC_Stringi ))( (struct CC_String *)__1k ) % ( ((struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi ))-> f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ) );

__1b = __vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiCFUi ( (struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi ), __1i ) ;

if (__1b == 0 )
return (struct kv_pair__pt__12_9CC_Stringi *)0 ;

{ struct kv_pair__pt__12_9CC_Stringi __1key ;

( (( ((& __1key )-> f_key__27kv_pair__pt__12_9CC_Stringi = (((struct CC_String *)(((struct CC_String *)__1k ))))), ((& __1key )-> f_value__27kv_pair__pt__12_9CC_Stringi = ((int *)0 ))) ),
(& __1key )) ;

{ { __0_result = find__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiCFPC27kv_pair__pt__12_9CC_Stringi ( (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__1b , (struct kv_pair__pt__12_9CC_Stringi *)(& __1key )) ;

__dt__27kv_pair__pt__12_9CC_StringiFv ( & __1key , 2) ;
} 
return __0_result ;
} 
}
}

int *findValue__29hashTable__pt__12_9CC_StringiCFPC9CC_String (__0this , __1k )struct hashTable__pt__12_9CC_Stringi *__0this ;

struct CC_String *__1k ;
{ 
struct kv_pair__pt__12_9CC_Stringi *__1p ;

__1p = _find__29hashTable__pt__12_9CC_StringiCFPC9CC_String ( __0this , __1k ) ;
if (__1p )
return __1p -> f_value__27kv_pair__pt__12_9CC_Stringi ;
else 
return (int *)0 ;
}

struct CC_String *findKeyAndValue__29hashTable__pt__12_9CC_StringiCFPC9CC_StringRPi (__0this , __1k , __1v )register struct hashTable__pt__12_9CC_Stringi *__0this ;

struct CC_String *__1k ;

int **__1v ;
{ 
struct kv_pair__pt__12_9CC_Stringi *__1p ;

__1p = _find__29hashTable__pt__12_9CC_StringiCFPC9CC_String ( __0this , __1k ) ;
if (__1p ){ 
((*__1v ))= __1p -> f_value__27kv_pair__pt__12_9CC_Stringi ;
return __1p -> f_key__27kv_pair__pt__12_9CC_Stringi ;
}
else return (struct CC_String *)0 ;
}

char insert__11CC_ListbaseFP12CC_Link_base ();

char insertKeyAndValue__29hashTable__pt__12_9CC_StringiFP9CC_StringPi (__0this , __1k , __1v )register struct hashTable__pt__12_9CC_Stringi *__0this ;

struct CC_String *__1k ;

int *__1v ;
{ 
size_t __1i ;

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__1b ;

__1i = (((*__0this -> f_hash_func_ptr__29hashTable__pt__12_9CC_Stringi ))( (struct CC_String *)__1k ) % ( ((struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi ))-> f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ) );

__1b = ((*__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi ( & __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi , __1i ) ));

if (__1b == 0 ){ 
struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0__X82 ;

struct CC_Listbase *__0__X72 ;

((*__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi ( & __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi , __1i ) ))= ( (__0__X82 = 0 ), ( ((__0__X82 || (__0__X82 = (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__nw__FUi (
sizeof (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi)) ))?( ( (__0__X82 = (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)( (__0__X72 = (((struct CC_Listbase *)__0__X82 ))), ( ((__0__X72 || (__0__X72 = (struct
CC_Listbase *)__nw__FUi ( sizeof (struct CC_Listbase)) ))?( ( (__0__X72 -> f_head__11CC_Listbase = 0 ), (__0__X72 -> f_tail__11CC_Listbase = 0 )) , (__0__X72 -> f_length__11CC_Listbase =
0 )) :0 ), __0__X72 ) ) ), (__0__X82 -> __vptr__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[0])) , (__0__X82 -> destructed__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi = 0 )) :0 ),
__0__X82 ) ) ;
}

{ struct kv_pair__pt__12_9CC_Stringi *__1p ;

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0__X83 ;

struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__0__X76 ;

struct CC_Link_base *__0__X71 ;

struct CC_Link_base *__2__X77 ;

struct kv_pair__pt__12_9CC_Stringi *__0__X84 ;

__1p = ( (__0__X84 = 0 ), ( ((__0__X84 || (__0__X84 = (struct kv_pair__pt__12_9CC_Stringi *)__nw__FUi ( sizeof (struct kv_pair__pt__12_9CC_Stringi)) ))?( (__0__X84 -> f_key__27kv_pair__pt__12_9CC_Stringi =
__1k ), (__0__X84 -> f_value__27kv_pair__pt__12_9CC_Stringi = __1v )) :0 ), __0__X84 ) ) ;
( (__0__X83 = ((*__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi ( & __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi , __1i ) ))), ( ( (__2__X77 = (struct CC_Link_base *)( (__0__X76 =
0 ), ( ((__0__X76 || (__0__X76 = (struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)__nw__FUi ( sizeof (struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi)) ))?( (__0__X76 = (struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( (__0__X71 = (((struct
CC_Link_base *)__0__X76 ))), ( ((__0__X71 || (__0__X71 = (struct CC_Link_base *)__nw__FUi ( sizeof (struct CC_Link_base)) ))?( (__0__X71 -> f_next__12CC_Link_base = 0 ), (__0__X71 -> f_prev__12CC_Link_base =
0 )) :0 ), __0__X71 ) ) ), (__0__X76 -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi = __1p )) :0 ), __0__X76 ) ) ), ( insert__11CC_ListbaseFP12CC_Link_base (
((struct CC_Listbase *)__0__X83 ), __2__X77 ) ) ) ) ) ;

__0this -> f_items__29hashTable__pt__12_9CC_Stringi ++ ;

}
}

struct kv_pair__pt__12_9CC_Stringi *remove__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFPC27kv_pair__pt__12_9CC_Stringi ();

struct CC_String *remove__29hashTable__pt__12_9CC_StringiFPC9CC_String (__0this , __1k )register struct hashTable__pt__12_9CC_Stringi *__0this ;

struct CC_String *__1k ;
{ 
size_t __1i ;

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__1b ;

struct CC_String *__0_result ;

__1i = (((*__0this -> f_hash_func_ptr__29hashTable__pt__12_9CC_Stringi ))( (struct CC_String *)__1k ) % ( ((struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi ))-> f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ) );

__1b = ((*__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi ( & __0this -> f_buckets__29hashTable__pt__12_9CC_Stringi , __1i ) ));

if (__1b == 0 )
return (struct CC_String *)0 ;

{ struct kv_pair__pt__12_9CC_Stringi __1key ;
struct kv_pair__pt__12_9CC_Stringi *__1result ;

( (( ((& __1key )-> f_key__27kv_pair__pt__12_9CC_Stringi = (((struct CC_String *)(((struct CC_String *)__1k ))))), ((& __1key )-> f_value__27kv_pair__pt__12_9CC_Stringi = ((int *)0 ))) ),
(& __1key )) ;
__1result = remove__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFPC27kv_pair__pt__12_9CC_Stringi ( __1b , (struct kv_pair__pt__12_9CC_Stringi *)(& __1key )) ;

if (__1result == 0 )
{ { __0_result = 0 ;

__dt__27kv_pair__pt__12_9CC_StringiFv ( & __1key , 2) ;
} 
return __0_result ;
} 
{ struct CC_String *__1kr ;

__1kr = __1result -> f_key__27kv_pair__pt__12_9CC_Stringi ;

__dt__27kv_pair__pt__12_9CC_StringiFv ( __1result , 3) ;

__0this -> f_items__29hashTable__pt__12_9CC_Stringi -- ;

{ 
__dt__27kv_pair__pt__12_9CC_StringiFv ( & __1key , 2) ;

return __1kr ;
} 
}

}
}

struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__ct__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFRC89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ();

struct hashTable__pt__26_9CC_String13BTCollectable *__ct__43hashTable__pt__26_9CC_String13BTCollectableFRC43hashTable__pt__26_9CC_String13BTCollectable (__0this , __1h )register struct hashTable__pt__26_9CC_String13BTCollectable *__0this ;

struct hashTable__pt__26_9CC_String13BTCollectable *__1h ;

{ 
struct ostream *__0__X85 ;

struct ostream *(*__2__X86 )();

if (__0this || (__0this = (struct hashTable__pt__26_9CC_String13BTCollectable *)__nw__FUi ( sizeof (struct hashTable__pt__26_9CC_String13BTCollectable)) )){ ( ( __ct__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFRC89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ( & __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable ,
(struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& ((*__1h )). f_buckets__43hashTable__pt__26_9CC_String13BTCollectable )) , (__0this -> f_hash_func_ptr__43hashTable__pt__26_9CC_String13BTCollectable = ((*__1h )). f_hash_func_ptr__43hashTable__pt__26_9CC_String13BTCollectable )) , (__0this -> f_items__43hashTable__pt__26_9CC_String13BTCollectable = ((*__1h )). f_items__43hashTable__pt__26_9CC_String13BTCollectable ))
;
( (__0__X85 = (struct ostream *)__ls__7ostreamFPCc ( (struct ostream *)(& cerr ), (char *)"Warning: hashTable(const hashTable&) called") ),
( (__2__X86 = (endl__FR7ostream )), ( ((*__2__X86 ))( (struct ostream *)__0__X85 ) ) ) ) ;
exit ( -1) ;
} 
return __0this ;
}

struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__ct__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUiP64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ();

struct hashTable__pt__26_9CC_String13BTCollectable *__ct__43hashTable__pt__26_9CC_String13BTCollectableFPFRC9CC_String_UiUi (__0this , __1f , __1init_bucket_num )struct hashTable__pt__26_9CC_String13BTCollectable *__0this ;

unsigned int (*__1f )();

size_t __1init_bucket_num ;

{ if (__0this || (__0this = (struct hashTable__pt__26_9CC_String13BTCollectable *)__nw__FUi ( sizeof (struct hashTable__pt__26_9CC_String13BTCollectable)) ))( ( __ct__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUiP64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ( & __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable ,
__1init_bucket_num , (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)0 ) , (__0this -> f_hash_func_ptr__43hashTable__pt__26_9CC_String13BTCollectable = __1f )) , (__0this -> f_items__43hashTable__pt__26_9CC_String13BTCollectable = 0 )) ;

return __0this ;
}

char __dt__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFv ();

char __dt__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFv ();

char __dt__41kv_pair__pt__26_9CC_String13BTCollectableFv ();

struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable {	/* sizeof CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable == 12 */

struct CC_Link_base *f_next__12CC_Link_base ;
struct CC_Link_base *f_prev__12CC_Link_base ;

struct kv_pair__pt__26_9CC_String13BTCollectable *f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ;
};

char __dt__43hashTable__pt__26_9CC_String13BTCollectableFv (__0this , __0__free )register struct hashTable__pt__26_9CC_String13BTCollectable *__0this ;

int __0__free ;

{ 
struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__1b ;
struct kv_pair__pt__26_9CC_String13BTCollectable *__1r ;

if (__0this ){ 
__1b = 0 ;
__1r = 0 ;

{ { int __1i ;

__1i = 0 ;

for(;__1i < ( ((struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable ))-> f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ) ;__1i ++ ) { 
__1b = ((*__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi ( & __0this ->
f_buckets__43hashTable__pt__26_9CC_String13BTCollectable , (unsigned int )__1i ) ));

if (__1b ){ 
struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xt00al4pcbp ;

struct kv_pair__pt__26_9CC_String13BTCollectable *__3__Xret00al4pcbp ;

while (__1r = ( (__2__Xt00al4pcbp = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)removeFirst__11CC_ListbaseFv ( (struct CC_Listbase *)__1b ) ))))), (__2__Xt00al4pcbp ?( (__3__Xret00al4pcbp = __2__Xt00al4pcbp -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ),
( __dl__FPv ( (char *)__2__Xt00al4pcbp ) , __3__Xret00al4pcbp ) ) :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) )
__dt__41kv_pair__pt__26_9CC_String13BTCollectableFv ( __1r , 3) ;

__dt__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFv ( __1b , 3) ;
}
}

}

}

if (__0this ){ __dt__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFv ( & __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable , 2) ;

if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} } 
}

char clearAndDestroy__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFv ();

char clearAndDestroy__43hashTable__pt__26_9CC_String13BTCollectableFv (__0this )register struct hashTable__pt__26_9CC_String13BTCollectable *__0this ;
{ 
f_needRemove__41kv_pair__pt__26_9CC_String13BTCollectable = 1 ;

{ struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__1b ;

__1b = 0 ;

{ { int __1i ;

__1i = 0 ;

for(;__1i < ( ((struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable ))-> f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ) ;__1i ++ ) { 
__1b = ((*__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi ( & __0this ->
f_buckets__43hashTable__pt__26_9CC_String13BTCollectable , (unsigned int )__1i ) ));
if (__1b ){ 
((*(((char (*)())(__1b -> __vptr__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable [1]).f))))( ((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)((((char *)__1b ))+ (__1b -> __vptr__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable [1]).d))) ;
__dt__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFv ( __1b , 3) ;
((*__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi ( & __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable , (unsigned int )__1i ) ))= 0 ;
}
}

__0this -> f_items__43hashTable__pt__26_9CC_String13BTCollectable = 0 ;

f_needRemove__41kv_pair__pt__26_9CC_String13BTCollectable = 0 ;

}

}

}
}

struct BTCollectable *findValue__43hashTable__pt__26_9CC_String13BTCollectableCFPC9CC_String ();

CC_Boolean contains__43hashTable__pt__26_9CC_String13BTCollectableCFPC9CC_String (__0this , __1k )struct hashTable__pt__26_9CC_String13BTCollectable *__0this ;

struct CC_String *__1k ;
{ 
if (findValue__43hashTable__pt__26_9CC_String13BTCollectableCFPC9CC_String ( __0this , __1k ) )
return (unsigned int )1 ;
else 
return (unsigned int )0 ;

}

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableCFUi ();

struct kv_pair__pt__26_9CC_String13BTCollectable *find__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableCFPC41kv_pair__pt__26_9CC_String13BTCollectable ();

struct kv_pair__pt__26_9CC_String13BTCollectable *_find__43hashTable__pt__26_9CC_String13BTCollectableCFPC9CC_String (__0this , __1k )register struct hashTable__pt__26_9CC_String13BTCollectable *__0this ;

struct CC_String *__1k ;
{ 
size_t __1i ;

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__1b ;

struct kv_pair__pt__26_9CC_String13BTCollectable *__0_result ;

__1i = (((*__0this -> f_hash_func_ptr__43hashTable__pt__26_9CC_String13BTCollectable ))( (struct CC_String *)__1k ) % ( ((struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable ))-> f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ) );

__1b = __vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableCFUi ( (struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable ), __1i ) ;

if (__1b == 0 )
return (struct kv_pair__pt__26_9CC_String13BTCollectable *)0 ;

{ struct kv_pair__pt__26_9CC_String13BTCollectable __1key ;

( (( ((& __1key )-> f_key__41kv_pair__pt__26_9CC_String13BTCollectable = (((struct CC_String *)(((struct CC_String *)__1k ))))), ((& __1key )-> f_value__41kv_pair__pt__26_9CC_String13BTCollectable = ((struct BTCollectable *)0 ))) ),
(& __1key )) ;

{ { __0_result = find__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableCFPC41kv_pair__pt__26_9CC_String13BTCollectable ( (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__1b , (struct kv_pair__pt__26_9CC_String13BTCollectable *)(& __1key )) ;

__dt__41kv_pair__pt__26_9CC_String13BTCollectableFv ( & __1key , 2) ;
} 
return __0_result ;
} 
}
}

struct BTCollectable *findValue__43hashTable__pt__26_9CC_String13BTCollectableCFPC9CC_String (__0this , __1k )struct hashTable__pt__26_9CC_String13BTCollectable *__0this ;

struct CC_String *__1k ;
{ 
struct kv_pair__pt__26_9CC_String13BTCollectable *__1p ;

__1p = _find__43hashTable__pt__26_9CC_String13BTCollectableCFPC9CC_String ( __0this , __1k ) ;
if (__1p )
return __1p -> f_value__41kv_pair__pt__26_9CC_String13BTCollectable ;
else 
return (struct BTCollectable *)0 ;
}

struct CC_String *findKeyAndValue__43hashTable__pt__26_9CC_String13BTCollectableCFPC9CC_StringRP13BTCollectable (__0this , __1k , __1v )register struct hashTable__pt__26_9CC_String13BTCollectable *__0this ;

struct CC_String *__1k ;

struct BTCollectable **__1v ;
{ 
struct kv_pair__pt__26_9CC_String13BTCollectable *__1p ;

__1p = _find__43hashTable__pt__26_9CC_String13BTCollectableCFPC9CC_String ( __0this , __1k ) ;
if (__1p ){ 
((*__1v ))= __1p -> f_value__41kv_pair__pt__26_9CC_String13BTCollectable ;
return __1p -> f_key__41kv_pair__pt__26_9CC_String13BTCollectable ;
}
else return (struct CC_String *)0 ;
}

char insertKeyAndValue__43hashTable__pt__26_9CC_String13BTCollectableFP9CC_StringP13BTCollectable (__0this , __1k , __1v )register struct hashTable__pt__26_9CC_String13BTCollectable *__0this ;

struct CC_String *__1k ;

struct BTCollectable *__1v ;
{ 
size_t __1i ;

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__1b ;

__1i = (((*__0this -> f_hash_func_ptr__43hashTable__pt__26_9CC_String13BTCollectable ))( (struct CC_String *)__1k ) % ( ((struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable ))-> f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ) );

__1b = ((*__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi ( & __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable , __1i ) ));

if (__1b == 0 ){ 
struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0__X87 ;

struct CC_Listbase *__0__X65 ;

((*__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi ( & __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable , __1i ) ))= ( (__0__X87 = 0 ), ( ((__0__X87 || (__0__X87 = (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__nw__FUi (
sizeof (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable)) ))?( ( (__0__X87 = (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( (__0__X65 = (((struct CC_Listbase *)__0__X87 ))), ( ((__0__X65 || (__0__X65 = (struct
CC_Listbase *)__nw__FUi ( sizeof (struct CC_Listbase)) ))?( ( (__0__X65 -> f_head__11CC_Listbase = 0 ), (__0__X65 -> f_tail__11CC_Listbase = 0 )) , (__0__X65 -> f_length__11CC_Listbase =
0 )) :0 ), __0__X65 ) ) ), (__0__X87 -> __vptr__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[1])) , (__0__X87 -> destructed__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = 0 )) :0 ),
__0__X87 ) ) ;
}

{ struct kv_pair__pt__26_9CC_String13BTCollectable *__1p ;

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0__X88 ;

struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0__X69 ;

struct CC_Link_base *__0__X64 ;

struct CC_Link_base *__2__X70 ;

struct kv_pair__pt__26_9CC_String13BTCollectable *__0__X89 ;

__1p = ( (__0__X89 = 0 ), ( ((__0__X89 || (__0__X89 = (struct kv_pair__pt__26_9CC_String13BTCollectable *)__nw__FUi ( sizeof (struct kv_pair__pt__26_9CC_String13BTCollectable)) ))?( (__0__X89 -> f_key__41kv_pair__pt__26_9CC_String13BTCollectable =
__1k ), (__0__X89 -> f_value__41kv_pair__pt__26_9CC_String13BTCollectable = __1v )) :0 ), __0__X89 ) ) ;
( (__0__X88 = ((*__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi ( & __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable , __1i ) ))), ( ( (__2__X70 = (struct CC_Link_base *)( (__0__X69 =
0 ), ( ((__0__X69 || (__0__X69 = (struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__nw__FUi ( sizeof (struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable)) ))?( (__0__X69 = (struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( (__0__X64 = (((struct
CC_Link_base *)__0__X69 ))), ( ((__0__X64 || (__0__X64 = (struct CC_Link_base *)__nw__FUi ( sizeof (struct CC_Link_base)) ))?( (__0__X64 -> f_next__12CC_Link_base = 0 ), (__0__X64 -> f_prev__12CC_Link_base =
0 )) :0 ), __0__X64 ) ) ), (__0__X69 -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = __1p )) :0 ), __0__X69 ) ) ), ( insert__11CC_ListbaseFP12CC_Link_base (
((struct CC_Listbase *)__0__X88 ), __2__X70 ) ) ) ) ) ;

__0this -> f_items__43hashTable__pt__26_9CC_String13BTCollectable ++ ;

}
}

struct kv_pair__pt__26_9CC_String13BTCollectable *remove__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFPC41kv_pair__pt__26_9CC_String13BTCollectable ();

struct CC_String *remove__43hashTable__pt__26_9CC_String13BTCollectableFPC9CC_String (__0this , __1k )register struct hashTable__pt__26_9CC_String13BTCollectable *__0this ;

struct CC_String *__1k ;
{ 
size_t __1i ;

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__1b ;

struct CC_String *__0_result ;

__1i = (((*__0this -> f_hash_func_ptr__43hashTable__pt__26_9CC_String13BTCollectable ))( (struct CC_String *)__1k ) % ( ((struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable ))-> f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ) );

__1b = ((*__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi ( & __0this -> f_buckets__43hashTable__pt__26_9CC_String13BTCollectable , __1i ) ));

if (__1b == 0 )
return (struct CC_String *)0 ;

{ struct kv_pair__pt__26_9CC_String13BTCollectable __1key ;
struct kv_pair__pt__26_9CC_String13BTCollectable *__1result ;

( (( ((& __1key )-> f_key__41kv_pair__pt__26_9CC_String13BTCollectable = (((struct CC_String *)(((struct CC_String *)__1k ))))), ((& __1key )-> f_value__41kv_pair__pt__26_9CC_String13BTCollectable = ((struct BTCollectable *)0 ))) ),
(& __1key )) ;
__1result = remove__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFPC41kv_pair__pt__26_9CC_String13BTCollectable ( __1b , (struct kv_pair__pt__26_9CC_String13BTCollectable *)(& __1key )) ;

if (__1result == 0 )
{ { __0_result = 0 ;

__dt__41kv_pair__pt__26_9CC_String13BTCollectableFv ( & __1key , 2) ;
} 
return __0_result ;
} 
{ struct CC_String *__1kr ;

__1kr = __1result -> f_key__41kv_pair__pt__26_9CC_String13BTCollectable ;

__dt__41kv_pair__pt__26_9CC_String13BTCollectableFv ( __1result , 3) ;

__0this -> f_items__43hashTable__pt__26_9CC_String13BTCollectable -- ;

{ 
__dt__41kv_pair__pt__26_9CC_String13BTCollectableFv ( & __1key , 2) ;

return __1kr ;
} 
}

}
}

static char __dt__9CC_StringFv (__0this , __0__free )struct CC_String *__0this ;

int __0__free ;

{ if (__0this ){ 
__0this -> __vptr__9CC_String = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[2];

__dl__FPv ( (char *)__0this -> f_string__9CC_String ) ;

if (__0this )if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} 
}

char __dt__27kv_pair__pt__12_9CC_StringiFv (__0this , __0__free )struct kv_pair__pt__12_9CC_Stringi *__0this ;

int __0__free ;

{ if (__0this ){ 
if (f_needRemove__27kv_pair__pt__12_9CC_Stringi == 1 ){ 
__0this -> f_key__27kv_pair__pt__12_9CC_Stringi ?((*(((char (*)())(__0this -> f_key__27kv_pair__pt__12_9CC_Stringi -> __vptr__9CC_String [1]).f))))( ((struct CC_String *)((((char *)__0this ->
f_key__27kv_pair__pt__12_9CC_Stringi ))+ (__0this -> f_key__27kv_pair__pt__12_9CC_Stringi -> __vptr__9CC_String [1]).d)), 3) :0 ;
__dl__FPv ( (char *)__0this -> f_value__27kv_pair__pt__12_9CC_Stringi ) ;
}

if (__0this )if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} 
}

struct Destructable {	/* sizeof Destructable == 4 */

struct __mptr *__vptr__12Destructable ;
};

extern char *g_stack_start__12Destructable ;

extern unsigned short g_size__12Destructable ;

struct Exception {	/* sizeof Exception == 20 */

struct __mptr *__vptr__12Destructable ;

unsigned int f_thrown__9Exception :1;
unsigned int f_thrown_as_pointer__9Exception :1;
unsigned int f_temporary__9Exception :1;

char *f_file__9Exception ;
unsigned short f_line__9Exception ;
struct Exception *f_previous_exception__9Exception ;
};
extern struct Exception *g_current_exception__9Exception ;
extern char g_temp_space__9Exception [];
extern char *g_next_avail__9Exception ;

char do_throw__9ExceptionFUiPCci ();

char __dl__9ExceptionSFPv ();

struct ccException {	/* sizeof ccException == 20 */

struct __mptr *__vptr__12Destructable ;

unsigned int f_thrown__9Exception :1;
unsigned int f_thrown_as_pointer__9Exception :1;
unsigned int f_temporary__9Exception :1;

char *f_file__9Exception ;
unsigned short f_line__9Exception ;
struct Exception *f_previous_exception__9Exception ;
};

struct Unwind_Stack {	/* sizeof Unwind_Stack == 2 */

unsigned short f_bottom__12Unwind_Stack ;
};
extern unsigned short g_top__12Unwind_Stack ;
extern struct Unwind_Record g_stack__12Unwind_Stack [512];

struct Jump_Environment {	/* sizeof Jump_Environment == 52 */

struct __mptr *__vptr__12Destructable ;

jmp_buf f_env__16Jump_Environment ;

struct Exception *f_active_exception__16Jump_Environment ;
struct Jump_Environment *f_next__16Jump_Environment ;
unsigned char f_unwinding__16Jump_Environment ;
struct Unwind_Stack f_unwind_stack__16Jump_Environment ;
};

extern struct Jump_Environment *g_jump_env_stack__16Jump_Environment ;
extern struct Jump_Environment *g_used_jump_env_stack__16Jump_Environment ;

int in_stack_set_size__12DestructableFv ();

struct Unwind_Record {	/* sizeof Unwind_Record == 8 */

struct Destructable *f_object__13Unwind_Record ;
unsigned short f_size__13Unwind_Record ;
};
enum __Q2_10Exceptions12error_type_t { INTERNAL_ERROR__Q2_10Exceptions12error_type_t = 0, APPLICATION_ERROR__Q2_10Exceptions12error_type_t = 1, THROW_MESSAGE__Q2_10Exceptions12error_type_t = 2} ;

struct Exceptions {	/* sizeof Exceptions == 1 */

char __W2__10Exceptions ;
};

extern  __Q2_10Exceptions15error_handler_t g_error_handler__10Exceptions ;

extern char *f_msg_internal_error__10Exceptions ;
extern char *f_msg_application_error__10Exceptions ;
extern char *f_msg_throw_message__10Exceptions ;

extern char *f_msg_not_initialized__10Exceptions ;
extern char *f_msg_initialized_twice__10Exceptions ;
extern char *f_msg_not_caught__10Exceptions ;
extern char *f_msg_no_current_exception__10Exceptions ;
extern char *f_msg_throw_from_terminate__10Exceptions ;
extern char *f_msg_throw_from_error_handler__10Exceptions ;
extern char *f_msg_throw_from_destructor__10Exceptions ;
extern char *f_msg_throw_ptr_to_stack__10Exceptions ;

extern char *f_msg_out_of_exception_memory__10Exceptions ;
extern char *f_msg_out_of_obj_stack_memory__10Exceptions ;
extern char *f_msg_memory_already_freed__10Exceptions ;

char error__10ExceptionsSFPCcQ2_10Exceptions12error_type_t ();

extern char terminate__Fv ();

static struct Exception *__ct__9ExceptionFRC9Exception (__0this , __0__A37 )register struct Exception *__0this ;

struct Exception *__0__A37 ;

{ 
struct Destructable *__0__X38 ;

struct Unwind_Record *__0__X5 ;

struct Unwind_Record *__0__X6 ;

struct Unwind_Record *__0__X3 ;

if (__0this || (__0this = (struct Exception *)( (((char *)__nw__FUi ( (sizeof (struct Exception))) ))) )){ ( ( (
( ( ( ( (__0this = (struct Exception *)( (__0__X38 = (((struct Destructable *)__0this ))), ( ((__0__X38 || (__0__X38 = (struct
Destructable *)__nw__FUi ( sizeof (struct Destructable)) ))?( (__0__X38 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[3]), ( (in_stack_set_size__12DestructableFv ( __0__X38 ) ?( ( (
((( (g_top__12Unwind_Stack == ((struct Unwind_Stack *)(& g_jump_env_stack__16Jump_Environment -> f_unwind_stack__16Jump_Environment ))-> f_bottom__12Unwind_Stack )) || (( (g_size__12Destructable == 0 )) ?((((unsigned int )__0__X38 ))<
( (__0__X6 = (struct Unwind_Record *)( (((struct Unwind_Record *)(& (g_stack__12Unwind_Stack [(g_top__12Unwind_Stack - 1 )]))))) ), ( (((unsigned int )__0__X6 -> f_object__13Unwind_Record )))
) ):((((unsigned int )__0__X38 ))> ( (__0__X5 = (struct Unwind_Record *)( (((struct Unwind_Record *)(& (g_stack__12Unwind_Stack [(g_top__12Unwind_Stack - 1 )]))))) ), (
((((unsigned int )__0__X5 -> f_object__13Unwind_Record ))+ __0__X5 -> f_size__13Unwind_Record )) ) )))?( ( ( ((g_top__12Unwind_Stack > 512 )?( ( error__10ExceptionsSFPCcQ2_10Exceptions12error_type_t (
(char *)f_msg_out_of_obj_stack_memory__10Exceptions , 0) , terminate__Fv ( ) ) , 0 ) :( 0 ) ), ( (
(__0__X3 = (& (g_stack__12Unwind_Stack [(g_top__12Unwind_Stack ++ )]))), ( ( (__0__X3 -> f_object__13Unwind_Record = __0__X38 ), (__0__X3 -> f_size__13Unwind_Record = ((unsigned int )g_size__12Destructable )))
, (((char )0 ))) ) ) ) ) , 0 ) :( 0 ) )) ) ,
0 ) :( 0 ) ), 0 ) ) :0 ), __0__X38 ) ) ), (__0this -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]))
, (__0this -> f_thrown__9Exception = ((*__0__A37 )). f_thrown__9Exception )) , (__0this -> f_thrown_as_pointer__9Exception = ((*__0__A37 )). f_thrown_as_pointer__9Exception )) , (__0this -> f_temporary__9Exception = ((*__0__A37 )).
f_temporary__9Exception )) , (__0this -> f_file__9Exception = ((*__0__A37 )). f_file__9Exception )) , (__0this -> f_line__9Exception = ((*__0__A37 )). f_line__9Exception )) , (__0this -> f_previous_exception__9Exception =
((*__0__A37 )). f_previous_exception__9Exception )) ;

;
} 
return __0this ;

}

struct ccStringException {	/* sizeof ccStringException == 24 */

struct __mptr *__vptr__12Destructable ;

unsigned int f_thrown__9Exception :1;
unsigned int f_thrown_as_pointer__9Exception :1;
unsigned int f_temporary__9Exception :1;

char *f_file__9Exception ;
unsigned short f_line__9Exception ;
struct Exception *f_previous_exception__9Exception ;

char *msg__17ccStringException ;
};

int in_stack__12DestructableFv ();

char *__nw__9ExceptionSFUii ();

static char __dt__12DestructableFv (__0this , __0__free )struct Destructable *__0this ;

int __0__free ;

{ if (__0this ){ 
__0this -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[3];

if (in_stack__12DestructableFv ( __0this ) )
( ( (((! ( (g_top__12Unwind_Stack == ((struct Unwind_Stack *)(& g_jump_env_stack__16Jump_Environment -> f_unwind_stack__16Jump_Environment ))-> f_bottom__12Unwind_Stack )) )&&
(((*( (((struct Unwind_Record *)(& (g_stack__12Unwind_Stack [(g_top__12Unwind_Stack - 1 )]))))) )). f_object__13Unwind_Record == __0this ))?( ( ( ( (g_top__12Unwind_Stack -- ), 0 )
, (((char )0 ))) ) , 0 ) :( 0 ) )) ) ;
if (__0this )if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} 
}

static char __dt__9ExceptionFv (__0this , __0__free )struct Exception *__0this ;

int __0__free ;

{ if (__0this ){ __0this -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4];

if (__0this ){ __dt__12DestructableFv ( ((struct Destructable *)__0this ), 0 ) ;

if (__0__free & 1)__dl__9ExceptionSFPv ( (char *)__0this ) ;
} } 
}

static char __dt__11ccExceptionFv (__0this , __0__free )struct ccException *__0this ;

int __0__free ;

{ if (__0this ){ 
__0this -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5];

if (__0this ){ ( ((((struct Exception *)__0this ))?( ( ((((struct Exception *)__0this ))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)__0this ))?( (
__dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)__0this ))), 0 ) , (( 0 ) )) , 0 ) :( 0 ) ))
, 0 ) :( 0 ) )) ;

if (__0__free & 1)__dl__9ExceptionSFPv ( (char *)__0this ) ;
} } 
}

static char __dt__17ccStringExceptionFv (__0this , __0__free )struct ccStringException *__0this ;

int __0__free ;

{ if (__0this ){ 
__0this -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[6];

if (__0this ){ ( ((((struct ccException *)__0this ))?( ( ((((struct ccException *)__0this ))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)__0this ))?( (
( ((((struct Exception *)(((struct ccException *)__0this ))))?( ( ((((struct Exception *)(((struct ccException *)__0this ))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)__0this ))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)__0this ))))), 0 ) , (( 0 ) )) , 0 ) :(
0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 ) :(
0 ) )) , 0 ) :( 0 ) )) ;

if (__0__free & 1)__dl__9ExceptionSFPv ( (char *)__0this ) ;
} } 
}

struct Exception *__ct__9ExceptionFv ();

static char *__opPCc__9CC_StringCFv ();

int compareTo__9CC_StringCFPCcQ2_9CC_String11caseCompare ();

unsigned int __eq__27kv_pair__pt__12_9CC_StringiFRC27kv_pair__pt__12_9CC_Stringi (__0this , __1kv )register struct kv_pair__pt__12_9CC_Stringi *__0this ;

struct kv_pair__pt__12_9CC_Stringi *__1kv ;
{ 
struct ccException *__0__X44 ;

register struct ccStringException *__0__X91 ;

register struct ccStringException *__0__X92 ;

struct Exception *__2__Xtemp002sf3bmd ;

struct ccStringException *__0__X43 ;

struct ccException *__0__X42 ;

struct CC_String *__0__X93 ;

if ((__0this -> f_key__27kv_pair__pt__12_9CC_Stringi == 0 )|| (((*__1kv )). f_key__27kv_pair__pt__12_9CC_Stringi == 0 )){ 
{ 
struct ccStringException __0__V90 ;

( (__0__X92 = ( (__0__X91 = ( ( (( ( (( (__0__X44 = (((struct ccException *)(& __0__V90 )))), (
((__0__X44 || (__0__X44 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( (__0__X44 = (struct ccException *)__ct__9ExceptionFv ( ((struct
Exception *)__0__X44 )) ), (__0__X44 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) :0 ), __0__X44 ) ) ), ((& __0__V90 )-> __vptr__12Destructable = (struct
__mptr *) __ptbl_vec__TKTemplate_cc_[6])) , ((& __0__V90 )-> msg__17ccStringException = ((char *)"kv_pair::operator==(): null pointer(s)."))) ), (& __0__V90 ))
, (& __0__V90 )) ), ( ((! __0__X91 -> f_thrown__9Exception )?( (__0__X91 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X91 )
) ), ( (in_stack__12DestructableFv ( (struct Destructable *)__0__X92 ) ?( (__2__Xtemp002sf3bmd = (struct Exception *)( (__0__X43 = (struct ccStringException *)__nw__9ExceptionSFUii ( sizeof
(struct ccStringException ), 0 ) ), ( ((__0__X43 || (__0__X43 = (struct ccStringException *)( (((char *)__nw__FUi ( (sizeof (struct ccStringException))) )))
))?( ( ( (__0__X43 = (struct ccStringException *)( (__0__X42 = (((struct ccException *)__0__X43 ))), ( ((__0__X42 || (__0__X42 = (struct ccException *)(
(((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( ( (__0__X42 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X42 ), (struct Exception *)(((struct
Exception *)(((struct Exception *)(((struct ccException *)(((struct ccException *)(((struct ccException *)(((struct ccStringException *)__0__X92 ))))))))))))) ), (__0__X42 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ),
__0__X42 ) ) ), (__0__X43 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[6])) , (__0__X43 -> msg__17ccStringException = ((*((struct ccStringException *)__0__X92 ))). msg__17ccStringException )) ,
0 ) :0 ), __0__X43 ) ) ), (((struct Exception *)((((struct ccStringException *)(((struct ccStringException *)__2__Xtemp002sf3bmd ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp002sf3bmd = (struct Exception *)__0__X92 )),
do_throw__9ExceptionFUiPCci ( __2__Xtemp002sf3bmd , ((unsigned int )28 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/cc_hdict.cc"), 0 ) ) )
;

( (( ( ((& __0__V90 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[6]), (( ( ( ((((struct ccException *)(& __0__V90 )))?(
( ((((struct ccException *)(& __0__V90 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)(& __0__V90 )))?( ( ( ((((struct Exception *)(((struct
ccException *)(& __0__V90 )))))?( ( ((((struct Exception *)(((struct ccException *)(& __0__V90 )))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)(& __0__V90 )))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)(& __0__V90 )))))), 0 ) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
)) , 0 ) )) ;
} } 
return (unsigned int )(( (__0__X93 = __0this -> f_key__27kv_pair__pt__12_9CC_Stringi ), ( (((unsigned int )((( compareTo__9CC_StringCFPCcQ2_9CC_String11caseCompare (
((struct CC_String *)__0__X93 ), __opPCc__9CC_StringCFv ( (struct CC_String *)((struct CC_String *)((*__1kv )). f_key__27kv_pair__pt__12_9CC_Stringi )) , ((int )0)) ) == 0 )?1 :0 )))) )
?1 :0 );
}

CC_Boolean f_needRemove__27kv_pair__pt__12_9CC_Stringi = 0 ;

struct BTCollectable {	/* sizeof BTCollectable == 12 */

char *f_name__13BTCollectable ;
int line_num__13BTCollectable ;
char *value__13BTCollectable ;
};

char __dt__13BTCollectableFv ();

char __dt__41kv_pair__pt__26_9CC_String13BTCollectableFv (__0this , __0__free )struct kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

int __0__free ;

{ if (__0this ){ 
if (f_needRemove__41kv_pair__pt__26_9CC_String13BTCollectable == 1 ){ 
__0this -> f_key__41kv_pair__pt__26_9CC_String13BTCollectable ?((*(((char (*)())(__0this -> f_key__41kv_pair__pt__26_9CC_String13BTCollectable -> __vptr__9CC_String [1]).f))))( ((struct CC_String *)((((char *)__0this ->
f_key__41kv_pair__pt__26_9CC_String13BTCollectable ))+ (__0this -> f_key__41kv_pair__pt__26_9CC_String13BTCollectable -> __vptr__9CC_String [1]).d)), 3) :0 ;
__dt__13BTCollectableFv ( __0this -> f_value__41kv_pair__pt__26_9CC_String13BTCollectable , 3) ;
}

if (__0this )if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} 
}

static char *__opPCc__9CC_StringCFv ();

unsigned int __eq__41kv_pair__pt__26_9CC_String13BTCollectableFRC41kv_pair__pt__26_9CC_String13BTCollectable (__0this , __1kv )register struct kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

struct kv_pair__pt__26_9CC_String13BTCollectable *__1kv ;
{ 
struct ccException *__0__X44 ;

register struct ccStringException *__0__X95 ;

register struct ccStringException *__0__X96 ;

struct Exception *__2__Xtemp002sf3bmd ;

struct ccStringException *__0__X43 ;

struct ccException *__0__X42 ;

struct CC_String *__0__X97 ;

if ((__0this -> f_key__41kv_pair__pt__26_9CC_String13BTCollectable == 0 )|| (((*__1kv )). f_key__41kv_pair__pt__26_9CC_String13BTCollectable == 0 )){ 
{ 
struct ccStringException __0__V94 ;

( (__0__X96 = ( (__0__X95 = ( ( (( ( (( (__0__X44 = (((struct ccException *)(& __0__V94 )))), (
((__0__X44 || (__0__X44 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( (__0__X44 = (struct ccException *)__ct__9ExceptionFv ( ((struct
Exception *)__0__X44 )) ), (__0__X44 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) :0 ), __0__X44 ) ) ), ((& __0__V94 )-> __vptr__12Destructable = (struct
__mptr *) __ptbl_vec__TKTemplate_cc_[6])) , ((& __0__V94 )-> msg__17ccStringException = ((char *)"kv_pair::operator==(): null pointer(s)."))) ), (& __0__V94 ))
, (& __0__V94 )) ), ( ((! __0__X95 -> f_thrown__9Exception )?( (__0__X95 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X95 )
) ), ( (in_stack__12DestructableFv ( (struct Destructable *)__0__X96 ) ?( (__2__Xtemp002sf3bmd = (struct Exception *)( (__0__X43 = (struct ccStringException *)__nw__9ExceptionSFUii ( sizeof
(struct ccStringException ), 0 ) ), ( ((__0__X43 || (__0__X43 = (struct ccStringException *)( (((char *)__nw__FUi ( (sizeof (struct ccStringException))) )))
))?( ( ( (__0__X43 = (struct ccStringException *)( (__0__X42 = (((struct ccException *)__0__X43 ))), ( ((__0__X42 || (__0__X42 = (struct ccException *)(
(((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( ( (__0__X42 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X42 ), (struct Exception *)(((struct
Exception *)(((struct Exception *)(((struct ccException *)(((struct ccException *)(((struct ccException *)(((struct ccStringException *)__0__X96 ))))))))))))) ), (__0__X42 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ),
__0__X42 ) ) ), (__0__X43 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[6])) , (__0__X43 -> msg__17ccStringException = ((*((struct ccStringException *)__0__X96 ))). msg__17ccStringException )) ,
0 ) :0 ), __0__X43 ) ) ), (((struct Exception *)((((struct ccStringException *)(((struct ccStringException *)__2__Xtemp002sf3bmd ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp002sf3bmd = (struct Exception *)__0__X96 )),
do_throw__9ExceptionFUiPCci ( __2__Xtemp002sf3bmd , ((unsigned int )28 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/cc_hdict.cc"), 0 ) ) )
;

( (( ( ((& __0__V94 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[6]), (( ( ( ((((struct ccException *)(& __0__V94 )))?(
( ((((struct ccException *)(& __0__V94 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)(& __0__V94 )))?( ( ( ((((struct Exception *)(((struct
ccException *)(& __0__V94 )))))?( ( ((((struct Exception *)(((struct ccException *)(& __0__V94 )))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)(& __0__V94 )))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)(& __0__V94 )))))), 0 ) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
)) , 0 ) )) ;
} } 
return (unsigned int )(( (__0__X97 = __0this -> f_key__41kv_pair__pt__26_9CC_String13BTCollectable ), ( (((unsigned int )((( compareTo__9CC_StringCFPCcQ2_9CC_String11caseCompare (
((struct CC_String *)__0__X97 ), __opPCc__9CC_StringCFv ( (struct CC_String *)((struct CC_String *)((*__1kv )). f_key__41kv_pair__pt__26_9CC_String13BTCollectable )) , ((int )0)) ) == 0 )?1 :0 )))) )
?1 :0 );
}

CC_Boolean f_needRemove__41kv_pair__pt__26_9CC_String13BTCollectable = 0 ;

struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__ct__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFRC75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi (__0this , __1pv )register struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__1pv ;

{ 
unsigned int __0__A98 ;

if (__0this || (__0this = (struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__nw__FUi ( sizeof (struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi)) )){ ( ( (__0this -> f_array__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi = (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi **)(((struct
CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi **)__nw__FUi ( (sizeof (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *))* (__0__A98 = ((*__1pv )). f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi )) ))), (__0this -> f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi = ((*__1pv )). f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi )) , (__0this ->
f_items__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi = ((*__1pv )). f_items__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi )) ;
{ { int __1i ;

struct ostream *__0__X99 ;

struct ostream *(*__2__X100 )();

__1i = 0 ;

for(;__1i < ((*__1pv )). f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ;__1i ++ ) 
(__0this -> f_array__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi [__1i ])= (((*__1pv )). f_array__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi [__1i ]);

( (__0__X99 = (struct ostream *)__ls__7ostreamFPCc ( (struct ostream *)(& cerr ), (char *)"Warning: pointer_vector(const pointer_vector&) called") ),
( (__2__X100 = (endl__FR7ostream )), ( ((*__2__X100 ))( (struct ostream *)__0__X99 ) ) ) ) ;
exit ( -1) ;

}

}
} 
return __0this ;

}

struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__ct__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUiP50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi (__0this , __1n , __1t )struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

size_t __1n ;

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__1t ;

{ 
unsigned int __0__A101 ;

if (__0this || (__0this = (struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__nw__FUi ( sizeof (struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi)) )){ ( ( (__0this -> f_array__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi = (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi **)(((struct
CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi **)__nw__FUi ( (sizeof (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *))* (__0__A101 = __1n )) ))), (__0this -> f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi = __1n )) , (__0this -> f_items__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi = 0 ))
;
{ { int __1i ;

__1i = 0 ;

for(;__1i < __0this -> f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ;__1i ++ ) 
(__0this -> f_array__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi [__1i ])= __1t ;

}

}
} 
return __0this ;

}

char __dt__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFv (__0this , __0__free )struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

int __0__free ;

{ if (__0this ){ 
__dl__FPv ( (char *)__0this -> f_array__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ) ;

if (__0this )if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} 
}

struct ccBoundaryException {	/* sizeof ccBoundaryException == 32 */

struct __mptr *__vptr__12Destructable ;

unsigned int f_thrown__9Exception :1;
unsigned int f_thrown_as_pointer__9Exception :1;
unsigned int f_temporary__9Exception :1;

char *f_file__9Exception ;
unsigned short f_line__9Exception ;
struct Exception *f_previous_exception__9Exception ;

int low__19ccBoundaryException ;
int high__19ccBoundaryException ;
int index__19ccBoundaryException ;
};

static char __dt__19ccBoundaryExceptionFv (__0this , __0__free )struct ccBoundaryException *__0this ;

int __0__free ;

{ if (__0this ){ 
__0this -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7];

if (__0this ){ ( ((((struct ccException *)__0this ))?( ( ((((struct ccException *)__0this ))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)__0this ))?( (
( ((((struct Exception *)(((struct ccException *)__0this ))))?( ( ((((struct Exception *)(((struct ccException *)__0this ))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)__0this ))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)__0this ))))), 0 ) , (( 0 ) )) , 0 ) :(
0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 ) :(
0 ) )) , 0 ) :( 0 ) )) ;

if (__0__free & 1)__dl__9ExceptionSFPv ( (char *)__0this ) ;
} } 
}

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiCFUi (__0this , __1i )register struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

size_t __1i ;
{ 
struct ccException *__0__X49 ;

register struct ccBoundaryException *__0__X103 ;

register struct ccBoundaryException *__0__X104 ;

struct Exception *__2__Xtemp00ere3blb ;

register struct ccBoundaryException *__0__X48 ;

struct ccException *__0__X47 ;

if ((__1i < 0 )|| (__1i >= __0this -> f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi )){ 
{ 
struct ccBoundaryException __0__V102 ;

( (__0__X104 = ( (__0__X103 = ( ( (( ( ( ( (( (__0__X49 = (((struct ccException *)(&
__0__V102 )))), ( ((__0__X49 || (__0__X49 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( (__0__X49 = (struct
ccException *)__ct__9ExceptionFv ( ((struct Exception *)__0__X49 )) ), (__0__X49 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) :0 ), __0__X49 ) ) ), ((& __0__V102 )->
__vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , ((& __0__V102 )-> low__19ccBoundaryException = 0 )) , ((& __0__V102 )-> high__19ccBoundaryException = ((int )(__0this ->
f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi - 1 )))) , ((& __0__V102 )-> index__19ccBoundaryException = ((int )__1i ))) ), (& __0__V102 )) , (& __0__V102 ))
), ( ((! __0__X103 -> f_thrown__9Exception )?( (__0__X103 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X103 ) ) ), (
(in_stack__12DestructableFv ( (struct Destructable *)__0__X104 ) ?( (__2__Xtemp00ere3blb = (struct Exception *)( (__0__X48 = (struct ccBoundaryException *)__nw__9ExceptionSFUii ( sizeof (struct ccBoundaryException ), 0 )
), ( ((__0__X48 || (__0__X48 = (struct ccBoundaryException *)( (((char *)__nw__FUi ( (sizeof (struct ccBoundaryException))) ))) ))?( ( (
( ( (__0__X48 = (struct ccBoundaryException *)( (__0__X47 = (((struct ccException *)__0__X48 ))), ( ((__0__X47 || (__0__X47 = (struct ccException *)( (((char
*)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( ( (__0__X47 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X47 ), (struct Exception *)(((struct Exception *)(((struct
Exception *)(((struct ccException *)(((struct ccException *)(((struct ccException *)(((struct ccBoundaryException *)__0__X104 ))))))))))))) ), (__0__X47 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ), __0__X47 )
) ), (__0__X48 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , (__0__X48 -> low__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X104 ))). low__19ccBoundaryException )) , (__0__X48 ->
high__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X104 ))). high__19ccBoundaryException )) , (__0__X48 -> index__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X104 ))). index__19ccBoundaryException )) , 0 ) :0 ), __0__X48 )
) ), (((struct Exception *)((((struct ccBoundaryException *)(((struct ccBoundaryException *)__2__Xtemp00ere3blb ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp00ere3blb = (struct Exception *)__0__X104 )), do_throw__9ExceptionFUiPCci ( __2__Xtemp00ere3blb , ((unsigned
int )33 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/cc_pvect.cc"), 0 ) ) ) ;

( (( ( ((& __0__V102 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7]), (( ( ( ((((struct ccException *)(& __0__V102 )))?(
( ((((struct ccException *)(& __0__V102 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)(& __0__V102 )))?( ( ( ((((struct Exception *)(((struct
ccException *)(& __0__V102 )))))?( ( ((((struct Exception *)(((struct ccException *)(& __0__V102 )))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)(& __0__V102 )))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)(& __0__V102 )))))), 0 ) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
)) , 0 ) )) ;
} } else 
return (__0this -> f_array__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi [__1i ]);
}

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi **__vc__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi (__0this , __1i )register struct pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

size_t __1i ;
{ 
struct ccException *__0__X49 ;

register struct ccBoundaryException *__0__X106 ;

register struct ccBoundaryException *__0__X107 ;

struct Exception *__2__Xtemp00ere3blb ;

register struct ccBoundaryException *__0__X48 ;

struct ccException *__0__X47 ;

if ((__1i < 0 )|| (__1i >= __0this -> f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi )){ 
{ 
struct ccBoundaryException __0__V105 ;

( (__0__X107 = ( (__0__X106 = ( ( (( ( ( ( (( (__0__X49 = (((struct ccException *)(&
__0__V105 )))), ( ((__0__X49 || (__0__X49 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( (__0__X49 = (struct
ccException *)__ct__9ExceptionFv ( ((struct Exception *)__0__X49 )) ), (__0__X49 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) :0 ), __0__X49 ) ) ), ((& __0__V105 )->
__vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , ((& __0__V105 )-> low__19ccBoundaryException = 0 )) , ((& __0__V105 )-> high__19ccBoundaryException = ((int )(__0this ->
f_size__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi - 1 )))) , ((& __0__V105 )-> index__19ccBoundaryException = ((int )__1i ))) ), (& __0__V105 )) , (& __0__V105 ))
), ( ((! __0__X106 -> f_thrown__9Exception )?( (__0__X106 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X106 ) ) ), (
(in_stack__12DestructableFv ( (struct Destructable *)__0__X107 ) ?( (__2__Xtemp00ere3blb = (struct Exception *)( (__0__X48 = (struct ccBoundaryException *)__nw__9ExceptionSFUii ( sizeof (struct ccBoundaryException ), 0 )
), ( ((__0__X48 || (__0__X48 = (struct ccBoundaryException *)( (((char *)__nw__FUi ( (sizeof (struct ccBoundaryException))) ))) ))?( ( (
( ( (__0__X48 = (struct ccBoundaryException *)( (__0__X47 = (((struct ccException *)__0__X48 ))), ( ((__0__X47 || (__0__X47 = (struct ccException *)( (((char
*)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( ( (__0__X47 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X47 ), (struct Exception *)(((struct Exception *)(((struct
Exception *)(((struct ccException *)(((struct ccException *)(((struct ccException *)(((struct ccBoundaryException *)__0__X107 ))))))))))))) ), (__0__X47 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ), __0__X47 )
) ), (__0__X48 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , (__0__X48 -> low__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X107 ))). low__19ccBoundaryException )) , (__0__X48 ->
high__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X107 ))). high__19ccBoundaryException )) , (__0__X48 -> index__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X107 ))). index__19ccBoundaryException )) , 0 ) :0 ), __0__X48 )
) ), (((struct Exception *)((((struct ccBoundaryException *)(((struct ccBoundaryException *)__2__Xtemp00ere3blb ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp00ere3blb = (struct Exception *)__0__X107 )), do_throw__9ExceptionFUiPCci ( __2__Xtemp00ere3blb , ((unsigned
int )42 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/cc_pvect.cc"), 0 ) ) ) ;

( (( ( ((& __0__V105 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7]), (( ( ( ((((struct ccException *)(& __0__V105 )))?(
( ((((struct ccException *)(& __0__V105 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)(& __0__V105 )))?( ( ( ((((struct Exception *)(((struct
ccException *)(& __0__V105 )))))?( ( ((((struct Exception *)(((struct ccException *)(& __0__V105 )))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)(& __0__V105 )))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)(& __0__V105 )))))), 0 ) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
)) , 0 ) )) ;
} } else 
return (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi **)(& (__0this -> f_array__75pointer_vector__pt__53_50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi [__1i ]));
}

struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__ct__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFRC89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable (__0this , __1pv )register struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__1pv ;

{ 
unsigned int __0__A108 ;

if (__0this || (__0this = (struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__nw__FUi ( sizeof (struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable)) )){ ( ( (__0this -> f_array__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable **)(((struct
CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable **)__nw__FUi ( (sizeof (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *))* (__0__A108 = ((*__1pv )). f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable )) ))), (__0this -> f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = ((*__1pv )). f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable )) , (__0this ->
f_items__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = ((*__1pv )). f_items__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable )) ;
{ { int __1i ;

struct ostream *__0__X109 ;

struct ostream *(*__2__X110 )();

__1i = 0 ;

for(;__1i < ((*__1pv )). f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ;__1i ++ ) 
(__0this -> f_array__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable [__1i ])= (((*__1pv )). f_array__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable [__1i ]);

( (__0__X109 = (struct ostream *)__ls__7ostreamFPCc ( (struct ostream *)(& cerr ), (char *)"Warning: pointer_vector(const pointer_vector&) called") ),
( (__2__X110 = (endl__FR7ostream )), ( ((*__2__X110 ))( (struct ostream *)__0__X109 ) ) ) ) ;
exit ( -1) ;

}

}
} 
return __0this ;

}

struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__ct__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUiP64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable (__0this , __1n , __1t )struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

size_t __1n ;

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__1t ;

{ 
unsigned int __0__A111 ;

if (__0this || (__0this = (struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__nw__FUi ( sizeof (struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable)) )){ ( ( (__0this -> f_array__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable **)(((struct
CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable **)__nw__FUi ( (sizeof (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *))* (__0__A111 = __1n )) ))), (__0this -> f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = __1n )) , (__0this -> f_items__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = 0 ))
;
{ { int __1i ;

__1i = 0 ;

for(;__1i < __0this -> f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ;__1i ++ ) 
(__0this -> f_array__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable [__1i ])= __1t ;

}

}
} 
return __0this ;

}

char __dt__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFv (__0this , __0__free )struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

int __0__free ;

{ if (__0this ){ 
__dl__FPv ( (char *)__0this -> f_array__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ) ;

if (__0this )if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} 
}

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableCFUi (__0this , __1i )register struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

size_t __1i ;
{ 
struct ccException *__0__X49 ;

register struct ccBoundaryException *__0__X113 ;

register struct ccBoundaryException *__0__X114 ;

struct Exception *__2__Xtemp00ere3blb ;

register struct ccBoundaryException *__0__X48 ;

struct ccException *__0__X47 ;

if ((__1i < 0 )|| (__1i >= __0this -> f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable )){ 
{ 
struct ccBoundaryException __0__V112 ;

( (__0__X114 = ( (__0__X113 = ( ( (( ( ( ( (( (__0__X49 = (((struct ccException *)(&
__0__V112 )))), ( ((__0__X49 || (__0__X49 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( (__0__X49 = (struct
ccException *)__ct__9ExceptionFv ( ((struct Exception *)__0__X49 )) ), (__0__X49 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) :0 ), __0__X49 ) ) ), ((& __0__V112 )->
__vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , ((& __0__V112 )-> low__19ccBoundaryException = 0 )) , ((& __0__V112 )-> high__19ccBoundaryException = ((int )(__0this ->
f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable - 1 )))) , ((& __0__V112 )-> index__19ccBoundaryException = ((int )__1i ))) ), (& __0__V112 )) , (& __0__V112 ))
), ( ((! __0__X113 -> f_thrown__9Exception )?( (__0__X113 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X113 ) ) ), (
(in_stack__12DestructableFv ( (struct Destructable *)__0__X114 ) ?( (__2__Xtemp00ere3blb = (struct Exception *)( (__0__X48 = (struct ccBoundaryException *)__nw__9ExceptionSFUii ( sizeof (struct ccBoundaryException ), 0 )
), ( ((__0__X48 || (__0__X48 = (struct ccBoundaryException *)( (((char *)__nw__FUi ( (sizeof (struct ccBoundaryException))) ))) ))?( ( (
( ( (__0__X48 = (struct ccBoundaryException *)( (__0__X47 = (((struct ccException *)__0__X48 ))), ( ((__0__X47 || (__0__X47 = (struct ccException *)( (((char
*)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( ( (__0__X47 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X47 ), (struct Exception *)(((struct Exception *)(((struct
Exception *)(((struct ccException *)(((struct ccException *)(((struct ccException *)(((struct ccBoundaryException *)__0__X114 ))))))))))))) ), (__0__X47 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ), __0__X47 )
) ), (__0__X48 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , (__0__X48 -> low__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X114 ))). low__19ccBoundaryException )) , (__0__X48 ->
high__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X114 ))). high__19ccBoundaryException )) , (__0__X48 -> index__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X114 ))). index__19ccBoundaryException )) , 0 ) :0 ), __0__X48 )
) ), (((struct Exception *)((((struct ccBoundaryException *)(((struct ccBoundaryException *)__2__Xtemp00ere3blb ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp00ere3blb = (struct Exception *)__0__X114 )), do_throw__9ExceptionFUiPCci ( __2__Xtemp00ere3blb , ((unsigned
int )33 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/cc_pvect.cc"), 0 ) ) ) ;

( (( ( ((& __0__V112 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7]), (( ( ( ((((struct ccException *)(& __0__V112 )))?(
( ((((struct ccException *)(& __0__V112 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)(& __0__V112 )))?( ( ( ((((struct Exception *)(((struct
ccException *)(& __0__V112 )))))?( ( ((((struct Exception *)(((struct ccException *)(& __0__V112 )))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)(& __0__V112 )))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)(& __0__V112 )))))), 0 ) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
)) , 0 ) )) ;
} } else 
return (__0this -> f_array__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable [__1i ]);
}

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable **__vc__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi (__0this , __1i )register struct pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

size_t __1i ;
{ 
struct ccException *__0__X49 ;

register struct ccBoundaryException *__0__X116 ;

register struct ccBoundaryException *__0__X117 ;

struct Exception *__2__Xtemp00ere3blb ;

register struct ccBoundaryException *__0__X48 ;

struct ccException *__0__X47 ;

if ((__1i < 0 )|| (__1i >= __0this -> f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable )){ 
{ 
struct ccBoundaryException __0__V115 ;

( (__0__X117 = ( (__0__X116 = ( ( (( ( ( ( (( (__0__X49 = (((struct ccException *)(&
__0__V115 )))), ( ((__0__X49 || (__0__X49 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( (__0__X49 = (struct
ccException *)__ct__9ExceptionFv ( ((struct Exception *)__0__X49 )) ), (__0__X49 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) :0 ), __0__X49 ) ) ), ((& __0__V115 )->
__vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , ((& __0__V115 )-> low__19ccBoundaryException = 0 )) , ((& __0__V115 )-> high__19ccBoundaryException = ((int )(__0this ->
f_size__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable - 1 )))) , ((& __0__V115 )-> index__19ccBoundaryException = ((int )__1i ))) ), (& __0__V115 )) , (& __0__V115 ))
), ( ((! __0__X116 -> f_thrown__9Exception )?( (__0__X116 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X116 ) ) ), (
(in_stack__12DestructableFv ( (struct Destructable *)__0__X117 ) ?( (__2__Xtemp00ere3blb = (struct Exception *)( (__0__X48 = (struct ccBoundaryException *)__nw__9ExceptionSFUii ( sizeof (struct ccBoundaryException ), 0 )
), ( ((__0__X48 || (__0__X48 = (struct ccBoundaryException *)( (((char *)__nw__FUi ( (sizeof (struct ccBoundaryException))) ))) ))?( ( (
( ( (__0__X48 = (struct ccBoundaryException *)( (__0__X47 = (((struct ccException *)__0__X48 ))), ( ((__0__X47 || (__0__X47 = (struct ccException *)( (((char
*)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( ( (__0__X47 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X47 ), (struct Exception *)(((struct Exception *)(((struct
Exception *)(((struct ccException *)(((struct ccException *)(((struct ccException *)(((struct ccBoundaryException *)__0__X117 ))))))))))))) ), (__0__X47 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ), __0__X47 )
) ), (__0__X48 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , (__0__X48 -> low__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X117 ))). low__19ccBoundaryException )) , (__0__X48 ->
high__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X117 ))). high__19ccBoundaryException )) , (__0__X48 -> index__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X117 ))). index__19ccBoundaryException )) , 0 ) :0 ), __0__X48 )
) ), (((struct Exception *)((((struct ccBoundaryException *)(((struct ccBoundaryException *)__2__Xtemp00ere3blb ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp00ere3blb = (struct Exception *)__0__X117 )), do_throw__9ExceptionFUiPCci ( __2__Xtemp00ere3blb , ((unsigned
int )42 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/cc_pvect.cc"), 0 ) ) ) ;

( (( ( ((& __0__V115 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7]), (( ( ( ((((struct ccException *)(& __0__V115 )))?(
( ((((struct ccException *)(& __0__V115 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)(& __0__V115 )))?( ( ( ((((struct Exception *)(((struct
ccException *)(& __0__V115 )))))?( ( ((((struct Exception *)(((struct ccException *)(& __0__V115 )))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)(& __0__V115 )))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)(& __0__V115 )))))), 0 ) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
)) , 0 ) )) ;
} } else 
return (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable **)(& (__0this -> f_array__89pointer_vector__pt__67_64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable [__1i ]));
}

struct CC_TValSlist__pt__2_i {	/* sizeof CC_TValSlist__pt__2_i == 12 */

struct CC_Link_base *f_head__11CC_Listbase ;
struct CC_Link_base *f_tail__11CC_Listbase ;
size_t f_length__11CC_Listbase ;
};

struct Stack__pt__2_i {	/* sizeof Stack__pt__2_i == 8 */

struct __mptr *__vptr__12Destructable ;

struct CC_TValSlist__pt__2_i *Items__14Stack__pt__2_i ;
};

struct Stack__pt__2_i *__ct__14Stack__pt__2_iFv (__0this )register struct Stack__pt__2_i *__0this ;
{ 
struct Destructable *__0__X118 ;

struct Unwind_Record *__0__X5 ;

struct Unwind_Record *__0__X6 ;

struct Unwind_Record *__0__X3 ;

struct CC_TValSlist__pt__2_i *__0__X119 ;

struct CC_Listbase *__0__X52 ;

if (__0this || (__0this = (struct Stack__pt__2_i *)__nw__FUi ( sizeof (struct Stack__pt__2_i)) )){ ( (__0this = (struct Stack__pt__2_i *)( (__0__X118 = (((struct
Destructable *)__0this ))), ( ((__0__X118 || (__0__X118 = (struct Destructable *)__nw__FUi ( sizeof (struct Destructable)) ))?( (__0__X118 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[3]), (
(in_stack_set_size__12DestructableFv ( __0__X118 ) ?( ( ( ((( (g_top__12Unwind_Stack == ((struct Unwind_Stack *)(& g_jump_env_stack__16Jump_Environment -> f_unwind_stack__16Jump_Environment ))-> f_bottom__12Unwind_Stack )) || ((
(g_size__12Destructable == 0 )) ?((((unsigned int )__0__X118 ))< ( (__0__X6 = (struct Unwind_Record *)( (((struct Unwind_Record *)(& (g_stack__12Unwind_Stack [(g_top__12Unwind_Stack - 1 )]))))) ),
( (((unsigned int )__0__X6 -> f_object__13Unwind_Record ))) ) ):((((unsigned int )__0__X118 ))> ( (__0__X5 = (struct Unwind_Record *)( (((struct
Unwind_Record *)(& (g_stack__12Unwind_Stack [(g_top__12Unwind_Stack - 1 )]))))) ), ( ((((unsigned int )__0__X5 -> f_object__13Unwind_Record ))+ __0__X5 -> f_size__13Unwind_Record )) ) )))?( (
( ((g_top__12Unwind_Stack > 512 )?( ( error__10ExceptionsSFPCcQ2_10Exceptions12error_type_t ( (char *)f_msg_out_of_obj_stack_memory__10Exceptions , 0) , terminate__Fv ( ) ) , 0 )
:( 0 ) ), ( ( (__0__X3 = (& (g_stack__12Unwind_Stack [(g_top__12Unwind_Stack ++ )]))), ( ( (__0__X3 -> f_object__13Unwind_Record = __0__X118 ),
(__0__X3 -> f_size__13Unwind_Record = ((unsigned int )g_size__12Destructable ))) , (((char )0 ))) ) ) ) ) , 0 )
:( 0 ) )) ) , 0 ) :( 0 ) ), 0 ) ) :0 ), __0__X118 ) )
), (__0this -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[8])) ;
__0this -> Items__14Stack__pt__2_i = ( (__0__X119 = 0 ), ( ((__0__X119 || (__0__X119 = (struct CC_TValSlist__pt__2_i *)__nw__FUi ( sizeof (struct CC_TValSlist__pt__2_i)) ))?(__0__X119 = (struct
CC_TValSlist__pt__2_i *)( (__0__X52 = (((struct CC_Listbase *)__0__X119 ))), ( ((__0__X52 || (__0__X52 = (struct CC_Listbase *)__nw__FUi ( sizeof (struct CC_Listbase)) ))?( ( (__0__X52 ->
f_head__11CC_Listbase = 0 ), (__0__X52 -> f_tail__11CC_Listbase = 0 )) , (__0__X52 -> f_length__11CC_Listbase = 0 )) :0 ), __0__X52 ) ) ):0 ), __0__X119 )
) ;
} 
return __0this ;
}

char __dt__21CC_TValSlist__pt__2_iFv ();

char __dt__14Stack__pt__2_iFv (__0this , __0__free )struct Stack__pt__2_i *__0this ;

int __0__free ;

{ if (__0this ){ 
__0this -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[8];

__dt__21CC_TValSlist__pt__2_iFv ( __0this -> Items__14Stack__pt__2_i , 3) ;

if (__0this ){ ( ((((struct Destructable *)__0this ))?( ( ((((struct Destructable *)__0this ))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[3]), ( (in_stack__12DestructableFv ( (((struct
Destructable *)__0this ))) ?( ( ( (((! ( (g_top__12Unwind_Stack == ((struct Unwind_Stack *)(& g_jump_env_stack__16Jump_Environment -> f_unwind_stack__16Jump_Environment ))-> f_bottom__12Unwind_Stack )) )&& (((*(
(((struct Unwind_Record *)(& (g_stack__12Unwind_Stack [(g_top__12Unwind_Stack - 1 )]))))) )). f_object__13Unwind_Record == (((struct Destructable *)__0this ))))?( ( ( ( (g_top__12Unwind_Stack -- ), 0 )
, (((char )0 ))) ) , 0 ) :( 0 ) )) ) , 0 ) :( 0 )
), ((((struct Destructable *)__0this ))?( (( 0 ) ), 0 ) :( 0 ) )) ) , 0 ) :(
0 ) )) ;

if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} } 
}

struct CC_Link__pt__2_i {	/* sizeof CC_Link__pt__2_i == 12 */

struct CC_Link_base *f_next__12CC_Link_base ;
struct CC_Link_base *f_prev__12CC_Link_base ;

int *f_element__16CC_Link__pt__2_i ;
};

static char append__21CC_TValSlist__pt__2_iFRCi (__0this , __2t )struct CC_TValSlist__pt__2_i *__0this ;

int *__2t ;

{ 
int *__2new_element ;

int *__0__N53 ;

struct CC_Link__pt__2_i *__0__X54 ;

struct CC_Link_base *__0__X51 ;

struct CC_Link_base *__2__X55 ;

__2new_element = ( ( (__0__N53 = (((int *)__nw__FUi ( sizeof (int )) ))), (((*__0__N53 ))= (*__2t ))) , __0__N53 )
;
( (__2__X55 = (struct CC_Link_base *)( (__0__X54 = 0 ), ( ((__0__X54 || (__0__X54 = (struct CC_Link__pt__2_i *)__nw__FUi ( sizeof (struct CC_Link__pt__2_i)) ))?(
(__0__X54 = (struct CC_Link__pt__2_i *)( (__0__X51 = (((struct CC_Link_base *)__0__X54 ))), ( ((__0__X51 || (__0__X51 = (struct CC_Link_base *)__nw__FUi ( sizeof (struct CC_Link_base)) ))?(
(__0__X51 -> f_next__12CC_Link_base = 0 ), (__0__X51 -> f_prev__12CC_Link_base = 0 )) :0 ), __0__X51 ) ) ), (__0__X54 -> f_element__16CC_Link__pt__2_i = (((int *)__2new_element ))))
:0 ), __0__X54 ) ) ), ( insert__11CC_ListbaseFP12CC_Link_base ( ((struct CC_Listbase *)__0this ), __2__X55 ) ) ) ;
}

char push__14Stack__pt__2_iFCi (__0this , __1newItem )struct Stack__pt__2_i *__0this ;

int __1newItem ;
{ 
append__21CC_TValSlist__pt__2_iFRCi ( __0this -> Items__14Stack__pt__2_i , (int *)(& __1newItem )) ;
}

struct CC_Link_base *removeLast__11CC_ListbaseFv ();

char throw_it__9ExceptionFUiPCci ();

int pop__14Stack__pt__2_iFv (__0this )register struct Stack__pt__2_i *__0this ;

{ 
struct CC_Link__pt__2_i *__1last_elem ;

__1last_elem = (((struct CC_Link__pt__2_i *)(((struct CC_Link__pt__2_i *)removeLast__11CC_ListbaseFv ( (struct CC_Listbase *)__0this -> Items__14Stack__pt__2_i ) ))));

if (! __1last_elem ){ 
register struct Exception *__0__X121 ;

{ 
struct Exception __0__V120 ;

throw_it__9ExceptionFUiPCci ( ( (__0__X121 = ( __ct__9ExceptionFv ( & __0__V120 ) , (& __0__V120 )) ), ( ((! __0__X121 ->
f_thrown__9Exception )?( (__0__X121 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X121 ) ) , (unsigned int )30 , (char *)"/VOB/olias/library/Common_Class/dti_cc/CC_Stack.cc",
0 ) ;

( (( ( ((& __0__V120 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), (( ( __dt__12DestructableFv ( ((struct Destructable *)(& __0__V120 )),
0 ) , (( 0 ) )) , 0 ) )) , 0 ) )) ;
} }

{ int *__1ret ;

__1ret = __1last_elem -> f_element__16CC_Link__pt__2_i ;
__dl__FPv ( (char *)__1last_elem ) ;

{ int __1ret_value ;

__1ret_value = ((*__1ret ));
__dl__FPv ( (char *)__1ret ) ;

return __1ret_value ;

}

}
}

int top__14Stack__pt__2_iCFv (__0this )register struct Stack__pt__2_i *__0this ;
{ 
struct CC_Link__pt__2_i *__1last_elem ;

__1last_elem = (((struct CC_Link__pt__2_i *)(((struct CC_Link__pt__2_i *)( ((struct CC_Listbase *)__0this -> Items__14Stack__pt__2_i )-> f_tail__11CC_Listbase ) ))));
if (! __1last_elem ){ 
register struct Exception *__0__X123 ;

{ 
struct Exception __0__V122 ;

throw_it__9ExceptionFUiPCci ( ( (__0__X123 = ( __ct__9ExceptionFv ( & __0__V122 ) , (& __0__V122 )) ), ( ((! __0__X123 ->
f_thrown__9Exception )?( (__0__X123 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X123 ) ) , (unsigned int )49 , (char *)"/VOB/olias/library/Common_Class/dti_cc/CC_Stack.cc",
0 ) ;

( (( ( ((& __0__V122 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), (( ( __dt__12DestructableFv ( ((struct Destructable *)(& __0__V122 )),
0 ) , (( 0 ) )) , 0 ) )) , 0 ) )) ;
} }

return ((*__1last_elem -> f_element__16CC_Link__pt__2_i ));
}

struct CC_List_Iterator_base {	/* sizeof CC_List_Iterator_base == 12 */

struct CC_Listbase *f_list__21CC_List_Iterator_base ;
struct CC_Link_base *f_previous__21CC_List_Iterator_base ;
struct CC_Link_base *f_current__21CC_List_Iterator_base ;
};

struct CC_TValSlistIterator__pt__2_i {	/* sizeof CC_TValSlistIterator__pt__2_i == 12 */

struct CC_Listbase *f_list__21CC_List_Iterator_base ;
struct CC_Link_base *f_previous__21CC_List_Iterator_base ;
struct CC_Link_base *f_current__21CC_List_Iterator_base ;
};

CC_Boolean __pp__21CC_List_Iterator_baseFv ();

int key__29CC_TValSlistIterator__pt__2_iCFv ();

struct CC_List_Iterator_base *__ct__21CC_List_Iterator_baseFP11CC_Listbase ();

struct CC_TValSlist__pt__2_i *__ct__21CC_TValSlist__pt__2_iFRC21CC_TValSlist__pt__2_i (__0this , __1sval_list )register struct CC_TValSlist__pt__2_i *__0this ;

struct CC_TValSlist__pt__2_i *__1sval_list ;
{ 
struct CC_TValSlistIterator__pt__2_i __1slist_val_iter ;

struct CC_Listbase *__0__X125 ;

if (__0this || (__0this = (struct CC_TValSlist__pt__2_i *)__nw__FUi ( sizeof (struct CC_TValSlist__pt__2_i)) )){ __0this = (struct CC_TValSlist__pt__2_i *)( (__0__X125 = (((struct CC_Listbase *)__0this ))),
( ((__0__X125 || (__0__X125 = (struct CC_Listbase *)__nw__FUi ( sizeof (struct CC_Listbase)) ))?( ( (__0__X125 -> f_head__11CC_Listbase = 0 ), (__0__X125 -> f_tail__11CC_Listbase =
0 )) , (__0__X125 -> f_length__11CC_Listbase = 0 )) :0 ), __0__X125 ) ) ;
( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1slist_val_iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TValSlist__pt__2_i *)(((struct CC_TValSlist__pt__2_i *)(((struct CC_TValSlist__pt__2_i *)(__1sval_list ))))))))))) ), (& __1slist_val_iter ))
;
while (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1slist_val_iter )) ){ 
{ 
int __0__I124 ;

append__21CC_TValSlist__pt__2_iFRCi ( __0this , (int *)( (__0__I124 = key__29CC_TValSlistIterator__pt__2_iCFv ( (struct CC_TValSlistIterator__pt__2_i *)(& __1slist_val_iter )) ), (& __0__I124 )) ) ;
} 
}
} 
return __0this ;

}

struct CC_Link_base *remove__11CC_ListbaseFR21CC_List_Iterator_base ();

char __dt__21CC_TValSlist__pt__2_iFv (__0this , __0__free )register struct CC_TValSlist__pt__2_i *__0this ;

int __0__free ;

{ 
struct CC_TValSlistIterator__pt__2_i __1iter ;

if (__0this ){ 
( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TValSlist__pt__2_i *)__0this )))))) ), (& __1iter ))
;
if (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ){ 
while (1 ){ 
struct CC_Link__pt__2_i *__3elem ;

__3elem = (((struct CC_Link__pt__2_i *)(((struct CC_Link__pt__2_i *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , ((struct CC_List_Iterator_base *)(((struct CC_List_Iterator_base *)(& __1iter ))))) ))));
if (__3elem ){ 
if (__3elem -> f_element__16CC_Link__pt__2_i ){ __dl__FPv ( (char *)__3elem -> f_element__16CC_Link__pt__2_i ) ;

}
__dl__FPv ( (char *)__3elem ) ;
}
else { break ;

}
}
}

if (__0this )if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} 
}

struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi {	/* sizeof CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi == 12 */

struct CC_Listbase *f_list__21CC_List_Iterator_base ;
struct CC_Link_base *f_previous__21CC_List_Iterator_base ;
struct CC_Link_base *f_current__21CC_List_Iterator_base ;
};

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__ct__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFRC50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi (__0this , __1slist )register struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__1slist ;
{ 
struct CC_Listbase *__0__X126 ;

if (__0this || (__0this = (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__nw__FUi ( sizeof (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi)) )){ ( (__0this = (struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)( (__0__X126 = (((struct
CC_Listbase *)__0this ))), ( ((__0__X126 || (__0__X126 = (struct CC_Listbase *)__nw__FUi ( sizeof (struct CC_Listbase)) ))?( ( (__0__X126 -> f_head__11CC_Listbase = 0 ), (__0__X126 ->
f_tail__11CC_Listbase = 0 )) , (__0__X126 -> f_length__11CC_Listbase = 0 )) :0 ), __0__X126 ) ) ), (__0this -> __vptr__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[0]))
;

__0this -> destructed__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi = 0 ;
{ struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi __1slist_iter ;

struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1slist_iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(__1slist ))))))))))) ), (& __1slist_iter ))
;

while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1slist_iter )) ?( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(&
__1slist_iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) ){ 
struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

struct kv_pair__pt__12_9CC_Stringi *__2__X127 ;

struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__0__X76 ;

struct CC_Link_base *__0__X71 ;

struct CC_Link_base *__2__X77 ;

( (__2__X127 = ( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __1slist_iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf ->
f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) ), ( ( (__2__X77 = (struct CC_Link_base *)( (__0__X76 = 0 ), ( ((__0__X76 || (__0__X76 = (struct
CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)__nw__FUi ( sizeof (struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi)) ))?( (__0__X76 = (struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( (__0__X71 = (((struct CC_Link_base *)__0__X76 ))), ( ((__0__X71 || (__0__X71 = (struct
CC_Link_base *)__nw__FUi ( sizeof (struct CC_Link_base)) ))?( (__0__X71 -> f_next__12CC_Link_base = 0 ), (__0__X71 -> f_prev__12CC_Link_base = 0 )) :0 ), __0__X71 ) ) ),
(__0__X76 -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi = __2__X127 )) :0 ), __0__X76 ) ) ), ( insert__11CC_ListbaseFP12CC_Link_base ( ((struct CC_Listbase *)__0this ), __2__X77 ) ) )
) ) ;
}

}
} 
return __0this ;

}

struct kv_pair__pt__12_9CC_Stringi *at__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiCFUi (__0this , __1pos )register struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

size_t __1pos ;
{ 
struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi __1iter ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__0this )))))))))) ), (& __1iter ))
;
{ { int __1i ;

struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

__1i = 0 ;

for(;__1i <= __1pos ;__1i ++ ) { 
if (! __pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ){ 
struct ccException *__0__X49 ;

register struct ccBoundaryException *__0__X129 ;

register struct ccBoundaryException *__0__X130 ;

struct Exception *__2__Xtemp00ere3blb ;

register struct ccBoundaryException *__0__X48 ;

struct ccException *__0__X47 ;

{ 
struct ccBoundaryException __0__V128 ;

( (__0__X130 = ( (__0__X129 = ( ( (( ( ( ( (( (__0__X49 = (((struct ccException *)(&
__0__V128 )))), ( ((__0__X49 || (__0__X49 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( (__0__X49 = (struct
ccException *)__ct__9ExceptionFv ( ((struct Exception *)__0__X49 )) ), (__0__X49 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) :0 ), __0__X49 ) ) ), ((& __0__V128 )->
__vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , ((& __0__V128 )-> low__19ccBoundaryException = 0 )) , ((& __0__V128 )-> high__19ccBoundaryException = 0 )) ,
((& __0__V128 )-> index__19ccBoundaryException = __1i )) ), (& __0__V128 )) , (& __0__V128 )) ), ( ((! __0__X129 ->
f_thrown__9Exception )?( (__0__X129 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X129 ) ) ), ( (in_stack__12DestructableFv ( (struct Destructable *)__0__X130 ) ?(
(__2__Xtemp00ere3blb = (struct Exception *)( (__0__X48 = (struct ccBoundaryException *)__nw__9ExceptionSFUii ( sizeof (struct ccBoundaryException ), 0 ) ), ( ((__0__X48 || (__0__X48 =
(struct ccBoundaryException *)( (((char *)__nw__FUi ( (sizeof (struct ccBoundaryException))) ))) ))?( ( ( ( ( (__0__X48 = (struct
ccBoundaryException *)( (__0__X47 = (((struct ccException *)__0__X48 ))), ( ((__0__X47 || (__0__X47 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) )))
))?( ( (__0__X47 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X47 ), (struct Exception *)(((struct Exception *)(((struct Exception *)(((struct ccException *)(((struct ccException *)(((struct ccException *)(((struct
ccBoundaryException *)__0__X130 ))))))))))))) ), (__0__X47 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ), __0__X47 ) ) ), (__0__X48 -> __vptr__12Destructable =
(struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , (__0__X48 -> low__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X130 ))). low__19ccBoundaryException )) , (__0__X48 -> high__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X130 ))). high__19ccBoundaryException ))
, (__0__X48 -> index__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X130 ))). index__19ccBoundaryException )) , 0 ) :0 ), __0__X48 ) ) ), (((struct Exception *)((((struct
ccBoundaryException *)(((struct ccBoundaryException *)__2__Xtemp00ere3blb ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp00ere3blb = (struct Exception *)__0__X130 )), do_throw__9ExceptionFUiPCci ( __2__Xtemp00ere3blb , ((unsigned int )30 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/CC_Slist.cc"),
0 ) ) ) ;

( (( ( ((& __0__V128 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7]), (( ( ( ((((struct ccException *)(& __0__V128 )))?(
( ((((struct ccException *)(& __0__V128 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)(& __0__V128 )))?( ( ( ((((struct Exception *)(((struct
ccException *)(& __0__V128 )))))?( ( ((((struct Exception *)(((struct ccException *)(& __0__V128 )))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)(& __0__V128 )))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)(& __0__V128 )))))), 0 ) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
)) , 0 ) )) ;
} }
}

return ( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf ->
f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) ;

}

}
}

struct kv_pair__pt__12_9CC_Stringi *removeAt__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFUi (__0this , __1pos )register struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

size_t __1pos ;

{ 
struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi __1iter ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__0this )))))) ), (& __1iter )) ;
{ { int __1i ;

__1i = 0 ;

for(;__1i <= __1pos ;__1i ++ ) { 
if (! __pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ){ 
struct ccException *__0__X49 ;

register struct ccBoundaryException *__0__X132 ;

register struct ccBoundaryException *__0__X133 ;

struct Exception *__2__Xtemp00ere3blb ;

register struct ccBoundaryException *__0__X48 ;

struct ccException *__0__X47 ;

{ 
struct ccBoundaryException __0__V131 ;

( (__0__X133 = ( (__0__X132 = ( ( (( ( ( ( (( (__0__X49 = (((struct ccException *)(&
__0__V131 )))), ( ((__0__X49 || (__0__X49 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( (__0__X49 = (struct
ccException *)__ct__9ExceptionFv ( ((struct Exception *)__0__X49 )) ), (__0__X49 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) :0 ), __0__X49 ) ) ), ((& __0__V131 )->
__vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , ((& __0__V131 )-> low__19ccBoundaryException = 0 )) , ((& __0__V131 )-> high__19ccBoundaryException = 0 )) ,
((& __0__V131 )-> index__19ccBoundaryException = __1i )) ), (& __0__V131 )) , (& __0__V131 )) ), ( ((! __0__X132 ->
f_thrown__9Exception )?( (__0__X132 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X132 ) ) ), ( (in_stack__12DestructableFv ( (struct Destructable *)__0__X133 ) ?(
(__2__Xtemp00ere3blb = (struct Exception *)( (__0__X48 = (struct ccBoundaryException *)__nw__9ExceptionSFUii ( sizeof (struct ccBoundaryException ), 0 ) ), ( ((__0__X48 || (__0__X48 =
(struct ccBoundaryException *)( (((char *)__nw__FUi ( (sizeof (struct ccBoundaryException))) ))) ))?( ( ( ( ( (__0__X48 = (struct
ccBoundaryException *)( (__0__X47 = (((struct ccException *)__0__X48 ))), ( ((__0__X47 || (__0__X47 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) )))
))?( ( (__0__X47 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X47 ), (struct Exception *)(((struct Exception *)(((struct Exception *)(((struct ccException *)(((struct ccException *)(((struct ccException *)(((struct
ccBoundaryException *)__0__X133 ))))))))))))) ), (__0__X47 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ), __0__X47 ) ) ), (__0__X48 -> __vptr__12Destructable =
(struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , (__0__X48 -> low__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X133 ))). low__19ccBoundaryException )) , (__0__X48 -> high__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X133 ))). high__19ccBoundaryException ))
, (__0__X48 -> index__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X133 ))). index__19ccBoundaryException )) , 0 ) :0 ), __0__X48 ) ) ), (((struct Exception *)((((struct
ccBoundaryException *)(((struct ccBoundaryException *)__2__Xtemp00ere3blb ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp00ere3blb = (struct Exception *)__0__X133 )), do_throw__9ExceptionFUiPCci ( __2__Xtemp00ere3blb , ((unsigned int )44 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/CC_Slist.cc"),
0 ) ) ) ;

( (( ( ((& __0__V131 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7]), (( ( ( ((((struct ccException *)(& __0__V131 )))?(
( ((((struct ccException *)(& __0__V131 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)(& __0__V131 )))?( ( ( ((((struct Exception *)(((struct
ccException *)(& __0__V131 )))))?( ( ((((struct Exception *)(((struct ccException *)(& __0__V131 )))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)(& __0__V131 )))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)(& __0__V131 )))))), 0 ) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
)) , 0 ) )) ;
} }
}

{ struct kv_pair__pt__12_9CC_Stringi *__1key_val ;
struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__1elem ;

struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

__1key_val = ( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct
kv_pair__pt__12_9CC_Stringi *)0 )))) ;
__1elem = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , (struct CC_List_Iterator_base *)(& __1iter )) ))));
__dl__FPv ( (char *)__1elem ) ;

return __1key_val ;

}

}

}

}

struct kv_pair__pt__12_9CC_Stringi *find__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiCFPC27kv_pair__pt__12_9CC_Stringi (__0this , __1elem )register struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

struct kv_pair__pt__12_9CC_Stringi *__1elem ;
{ 
struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi __1iter ;

struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__0this )))))))))) ), (& __1iter ))
;

while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ?( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) ){ 
struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

if (__eq__27kv_pair__pt__12_9CC_StringiFRC27kv_pair__pt__12_9CC_Stringi ( ( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf ->
f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) , (struct kv_pair__pt__12_9CC_Stringi *)__1elem ) )
return ( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) ;
}

return (struct kv_pair__pt__12_9CC_Stringi *)0 ;
}

struct kv_pair__pt__12_9CC_Stringi *find__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiCFPFP27kv_pair__pt__12_9CC_StringiPv_UiPv (__0this , __1testFunc , __1d )register struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

CC_Boolean (*__1testFunc )();
char *__1d ;
{ 
struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi __1iter ;

struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__0this )))))))))) ), (& __1iter ))
;
while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ?( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) ){ 
struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

if ((*(__1testFunc ))( ( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf ->
f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) , __1d ) ){ 
struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

return ( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct
kv_pair__pt__12_9CC_Stringi *)0 )))) ;
}
}

return (struct kv_pair__pt__12_9CC_Stringi *)0 ;
}

CC_Boolean contains__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiCFPC27kv_pair__pt__12_9CC_Stringi (__0this , __1elem )register struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

struct kv_pair__pt__12_9CC_Stringi *__1elem ;
{ 
struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi __1iter ;

struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__0this )))))))))) ), (& __1iter ))
;
while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ?( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) ){ 
struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

if (__eq__27kv_pair__pt__12_9CC_StringiFRC27kv_pair__pt__12_9CC_Stringi ( ( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf ->
f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) , (struct kv_pair__pt__12_9CC_Stringi *)__1elem ) )
return (unsigned int )1 ;
}

return (unsigned int )0 ;
}

struct kv_pair__pt__12_9CC_Stringi *remove__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFPC27kv_pair__pt__12_9CC_Stringi (__0this , __1elem )register struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

struct kv_pair__pt__12_9CC_Stringi *__1elem ;
{ 
struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi __1iter ;

struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__0this )))))) ), (& __1iter )) ;
while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ?( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) ){ 
struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__2__Xlink_item00qpg5ckf ;

if (__eq__27kv_pair__pt__12_9CC_StringiFRC27kv_pair__pt__12_9CC_Stringi ( ( (__2__Xlink_item00qpg5ckf = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00qpg5ckf ?__2__Xlink_item00qpg5ckf ->
f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi :(((struct kv_pair__pt__12_9CC_Stringi *)0 )))) , (struct kv_pair__pt__12_9CC_Stringi *)__1elem ) ){ 
struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__3key_rec ;
struct kv_pair__pt__12_9CC_Stringi *__3ret ;

__3key_rec = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , (struct CC_List_Iterator_base *)(& __1iter )) ))));
__3ret = __3key_rec -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi ;
__dl__FPv ( (char *)__3key_rec ) ;

return __3ret ;
}
}

return (struct kv_pair__pt__12_9CC_Stringi *)0 ;
}

char __dt__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFv (__0this , __0__free )register struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;

int __0__free ;

{ if (__0this ){ 
__0this -> __vptr__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[0];

if (! __0this -> destructed__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi ){ 
struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi __2iter ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __2iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__0this )))))) ), (& __2iter )) ;
if (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __2iter )) ){ 
while (1 ){ 
struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__4elem ;

__4elem = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , ((struct CC_List_Iterator_base *)(((struct CC_List_Iterator_base *)(& __2iter ))))) ))));
if (__4elem ){ 
__dl__FPv ( (char *)__4elem ) ;
}
else { break ;

}
}
}
}

if (__0this )if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} 
}

char clearAndDestroy__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFv (__0this )register struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *__0this ;
{ 
__0this -> destructed__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi = 1 ;
{ struct CC_TPtrSlistIterator__pt__30_27kv_pair__pt__12_9CC_Stringi __1iter ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi *)__0this )))))) ), (& __1iter )) ;
if (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ){ 
while (1 ){ 
struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *__3elem ;

__3elem = (((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)(((struct CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , ((struct CC_List_Iterator_base *)(((struct CC_List_Iterator_base *)(& __1iter ))))) ))));
if (__3elem ){ 
__dt__27kv_pair__pt__12_9CC_StringiFv ( __3elem -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi , 3) ;
__3elem -> f_element__45CC_Link__pt__30_27kv_pair__pt__12_9CC_Stringi = 0 ;
__dl__FPv ( (char *)__3elem ) ;
}
else { break ;

}
}
}

}
}

struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable {	/* sizeof CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable == 12 */

struct CC_Listbase *f_list__21CC_List_Iterator_base ;
struct CC_Link_base *f_previous__21CC_List_Iterator_base ;
struct CC_Link_base *f_current__21CC_List_Iterator_base ;
};

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__ct__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFRC64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable (__0this , __1slist )register struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__1slist ;
{ 
struct CC_Listbase *__0__X134 ;

if (__0this || (__0this = (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__nw__FUi ( sizeof (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable)) )){ ( (__0this = (struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( (__0__X134 = (((struct
CC_Listbase *)__0this ))), ( ((__0__X134 || (__0__X134 = (struct CC_Listbase *)__nw__FUi ( sizeof (struct CC_Listbase)) ))?( ( (__0__X134 -> f_head__11CC_Listbase = 0 ), (__0__X134 ->
f_tail__11CC_Listbase = 0 )) , (__0__X134 -> f_length__11CC_Listbase = 0 )) :0 ), __0__X134 ) ) ), (__0this -> __vptr__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[1]))
;

__0this -> destructed__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = 0 ;
{ struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable __1slist_iter ;

struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1slist_iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(__1slist ))))))))))) ), (& __1slist_iter ))
;

while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1slist_iter )) ?( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(&
__1slist_iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) ){ 
struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

struct kv_pair__pt__26_9CC_String13BTCollectable *__2__X135 ;

struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0__X69 ;

struct CC_Link_base *__0__X64 ;

struct CC_Link_base *__2__X70 ;

( (__2__X135 = ( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __1slist_iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda ->
f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) ), ( ( (__2__X70 = (struct CC_Link_base *)( (__0__X69 = 0 ), ( ((__0__X69 || (__0__X69 = (struct
CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__nw__FUi ( sizeof (struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable)) ))?( (__0__X69 = (struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( (__0__X64 = (((struct CC_Link_base *)__0__X69 ))), ( ((__0__X64 || (__0__X64 = (struct
CC_Link_base *)__nw__FUi ( sizeof (struct CC_Link_base)) ))?( (__0__X64 -> f_next__12CC_Link_base = 0 ), (__0__X64 -> f_prev__12CC_Link_base = 0 )) :0 ), __0__X64 ) ) ),
(__0__X69 -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = __2__X135 )) :0 ), __0__X69 ) ) ), ( insert__11CC_ListbaseFP12CC_Link_base ( ((struct CC_Listbase *)__0this ), __2__X70 ) ) )
) ) ;
}

}
} 
return __0this ;

}

struct kv_pair__pt__26_9CC_String13BTCollectable *at__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableCFUi (__0this , __1pos )register struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

size_t __1pos ;
{ 
struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable __1iter ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__0this )))))))))) ), (& __1iter ))
;
{ { int __1i ;

struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

__1i = 0 ;

for(;__1i <= __1pos ;__1i ++ ) { 
if (! __pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ){ 
struct ccException *__0__X49 ;

register struct ccBoundaryException *__0__X137 ;

register struct ccBoundaryException *__0__X138 ;

struct Exception *__2__Xtemp00ere3blb ;

register struct ccBoundaryException *__0__X48 ;

struct ccException *__0__X47 ;

{ 
struct ccBoundaryException __0__V136 ;

( (__0__X138 = ( (__0__X137 = ( ( (( ( ( ( (( (__0__X49 = (((struct ccException *)(&
__0__V136 )))), ( ((__0__X49 || (__0__X49 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( (__0__X49 = (struct
ccException *)__ct__9ExceptionFv ( ((struct Exception *)__0__X49 )) ), (__0__X49 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) :0 ), __0__X49 ) ) ), ((& __0__V136 )->
__vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , ((& __0__V136 )-> low__19ccBoundaryException = 0 )) , ((& __0__V136 )-> high__19ccBoundaryException = 0 )) ,
((& __0__V136 )-> index__19ccBoundaryException = __1i )) ), (& __0__V136 )) , (& __0__V136 )) ), ( ((! __0__X137 ->
f_thrown__9Exception )?( (__0__X137 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X137 ) ) ), ( (in_stack__12DestructableFv ( (struct Destructable *)__0__X138 ) ?(
(__2__Xtemp00ere3blb = (struct Exception *)( (__0__X48 = (struct ccBoundaryException *)__nw__9ExceptionSFUii ( sizeof (struct ccBoundaryException ), 0 ) ), ( ((__0__X48 || (__0__X48 =
(struct ccBoundaryException *)( (((char *)__nw__FUi ( (sizeof (struct ccBoundaryException))) ))) ))?( ( ( ( ( (__0__X48 = (struct
ccBoundaryException *)( (__0__X47 = (((struct ccException *)__0__X48 ))), ( ((__0__X47 || (__0__X47 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) )))
))?( ( (__0__X47 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X47 ), (struct Exception *)(((struct Exception *)(((struct Exception *)(((struct ccException *)(((struct ccException *)(((struct ccException *)(((struct
ccBoundaryException *)__0__X138 ))))))))))))) ), (__0__X47 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ), __0__X47 ) ) ), (__0__X48 -> __vptr__12Destructable =
(struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , (__0__X48 -> low__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X138 ))). low__19ccBoundaryException )) , (__0__X48 -> high__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X138 ))). high__19ccBoundaryException ))
, (__0__X48 -> index__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X138 ))). index__19ccBoundaryException )) , 0 ) :0 ), __0__X48 ) ) ), (((struct Exception *)((((struct
ccBoundaryException *)(((struct ccBoundaryException *)__2__Xtemp00ere3blb ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp00ere3blb = (struct Exception *)__0__X138 )), do_throw__9ExceptionFUiPCci ( __2__Xtemp00ere3blb , ((unsigned int )30 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/CC_Slist.cc"),
0 ) ) ) ;

( (( ( ((& __0__V136 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7]), (( ( ( ((((struct ccException *)(& __0__V136 )))?(
( ((((struct ccException *)(& __0__V136 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)(& __0__V136 )))?( ( ( ((((struct Exception *)(((struct
ccException *)(& __0__V136 )))))?( ( ((((struct Exception *)(((struct ccException *)(& __0__V136 )))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)(& __0__V136 )))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)(& __0__V136 )))))), 0 ) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
)) , 0 ) )) ;
} }
}

return ( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda ->
f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) ;

}

}
}

struct kv_pair__pt__26_9CC_String13BTCollectable *removeAt__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFUi (__0this , __1pos )register struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

size_t __1pos ;

{ 
struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable __1iter ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__0this )))))) ), (& __1iter )) ;
{ { int __1i ;

__1i = 0 ;

for(;__1i <= __1pos ;__1i ++ ) { 
if (! __pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ){ 
struct ccException *__0__X49 ;

register struct ccBoundaryException *__0__X140 ;

register struct ccBoundaryException *__0__X141 ;

struct Exception *__2__Xtemp00ere3blb ;

register struct ccBoundaryException *__0__X48 ;

struct ccException *__0__X47 ;

{ 
struct ccBoundaryException __0__V139 ;

( (__0__X141 = ( (__0__X140 = ( ( (( ( ( ( (( (__0__X49 = (((struct ccException *)(&
__0__V139 )))), ( ((__0__X49 || (__0__X49 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( (__0__X49 = (struct
ccException *)__ct__9ExceptionFv ( ((struct Exception *)__0__X49 )) ), (__0__X49 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) :0 ), __0__X49 ) ) ), ((& __0__V139 )->
__vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , ((& __0__V139 )-> low__19ccBoundaryException = 0 )) , ((& __0__V139 )-> high__19ccBoundaryException = 0 )) ,
((& __0__V139 )-> index__19ccBoundaryException = __1i )) ), (& __0__V139 )) , (& __0__V139 )) ), ( ((! __0__X140 ->
f_thrown__9Exception )?( (__0__X140 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X140 ) ) ), ( (in_stack__12DestructableFv ( (struct Destructable *)__0__X141 ) ?(
(__2__Xtemp00ere3blb = (struct Exception *)( (__0__X48 = (struct ccBoundaryException *)__nw__9ExceptionSFUii ( sizeof (struct ccBoundaryException ), 0 ) ), ( ((__0__X48 || (__0__X48 =
(struct ccBoundaryException *)( (((char *)__nw__FUi ( (sizeof (struct ccBoundaryException))) ))) ))?( ( ( ( ( (__0__X48 = (struct
ccBoundaryException *)( (__0__X47 = (((struct ccException *)__0__X48 ))), ( ((__0__X47 || (__0__X47 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) )))
))?( ( (__0__X47 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X47 ), (struct Exception *)(((struct Exception *)(((struct Exception *)(((struct ccException *)(((struct ccException *)(((struct ccException *)(((struct
ccBoundaryException *)__0__X141 ))))))))))))) ), (__0__X47 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ), __0__X47 ) ) ), (__0__X48 -> __vptr__12Destructable =
(struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , (__0__X48 -> low__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X141 ))). low__19ccBoundaryException )) , (__0__X48 -> high__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X141 ))). high__19ccBoundaryException ))
, (__0__X48 -> index__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X141 ))). index__19ccBoundaryException )) , 0 ) :0 ), __0__X48 ) ) ), (((struct Exception *)((((struct
ccBoundaryException *)(((struct ccBoundaryException *)__2__Xtemp00ere3blb ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp00ere3blb = (struct Exception *)__0__X141 )), do_throw__9ExceptionFUiPCci ( __2__Xtemp00ere3blb , ((unsigned int )44 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/CC_Slist.cc"),
0 ) ) ) ;

( (( ( ((& __0__V139 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7]), (( ( ( ((((struct ccException *)(& __0__V139 )))?(
( ((((struct ccException *)(& __0__V139 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)(& __0__V139 )))?( ( ( ((((struct Exception *)(((struct
ccException *)(& __0__V139 )))))?( ( ((((struct Exception *)(((struct ccException *)(& __0__V139 )))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)(& __0__V139 )))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)(& __0__V139 )))))), 0 ) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
)) , 0 ) )) ;
} }
}

{ struct kv_pair__pt__26_9CC_String13BTCollectable *__1key_val ;
struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__1elem ;

struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

__1key_val = ( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct
kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) ;
__1elem = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , (struct CC_List_Iterator_base *)(& __1iter )) ))));
__dl__FPv ( (char *)__1elem ) ;

return __1key_val ;

}

}

}

}

struct kv_pair__pt__26_9CC_String13BTCollectable *find__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableCFPC41kv_pair__pt__26_9CC_String13BTCollectable (__0this , __1elem )register struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

struct kv_pair__pt__26_9CC_String13BTCollectable *__1elem ;
{ 
struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable __1iter ;

struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__0this )))))))))) ), (& __1iter ))
;

while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ?( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) ){ 
struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

if (__eq__41kv_pair__pt__26_9CC_String13BTCollectableFRC41kv_pair__pt__26_9CC_String13BTCollectable ( ( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda ->
f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) , (struct kv_pair__pt__26_9CC_String13BTCollectable *)__1elem ) )
return ( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) ;
}

return (struct kv_pair__pt__26_9CC_String13BTCollectable *)0 ;
}

struct kv_pair__pt__26_9CC_String13BTCollectable *find__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableCFPFP41kv_pair__pt__26_9CC_String13BTCollectablePv_UiPv (__0this , __1testFunc , __1d )register struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

CC_Boolean (*__1testFunc )();
char *__1d ;
{ 
struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable __1iter ;

struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__0this )))))))))) ), (& __1iter ))
;
while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ?( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) ){ 
struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

if ((*(__1testFunc ))( ( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda ->
f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) , __1d ) ){ 
struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

return ( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct
kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) ;
}
}

return (struct kv_pair__pt__26_9CC_String13BTCollectable *)0 ;
}

CC_Boolean contains__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableCFPC41kv_pair__pt__26_9CC_String13BTCollectable (__0this , __1elem )register struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

struct kv_pair__pt__26_9CC_String13BTCollectable *__1elem ;
{ 
struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable __1iter ;

struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__0this )))))))))) ), (& __1iter ))
;
while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ?( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) ){ 
struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

if (__eq__41kv_pair__pt__26_9CC_String13BTCollectableFRC41kv_pair__pt__26_9CC_String13BTCollectable ( ( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda ->
f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) , (struct kv_pair__pt__26_9CC_String13BTCollectable *)__1elem ) )
return (unsigned int )1 ;
}

return (unsigned int )0 ;
}

struct kv_pair__pt__26_9CC_String13BTCollectable *remove__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFPC41kv_pair__pt__26_9CC_String13BTCollectable (__0this , __1elem )register struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

struct kv_pair__pt__26_9CC_String13BTCollectable *__1elem ;
{ 
struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable __1iter ;

struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__0this )))))) ), (& __1iter )) ;
while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ?( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) ){ 
struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__2__Xlink_item00ak2cdda ;

if (__eq__41kv_pair__pt__26_9CC_String13BTCollectableFRC41kv_pair__pt__26_9CC_String13BTCollectable ( ( (__2__Xlink_item00ak2cdda = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00ak2cdda ?__2__Xlink_item00ak2cdda ->
f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable :(((struct kv_pair__pt__26_9CC_String13BTCollectable *)0 )))) , (struct kv_pair__pt__26_9CC_String13BTCollectable *)__1elem ) ){ 
struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__3key_rec ;
struct kv_pair__pt__26_9CC_String13BTCollectable *__3ret ;

__3key_rec = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , (struct CC_List_Iterator_base *)(& __1iter )) ))));
__3ret = __3key_rec -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ;
__dl__FPv ( (char *)__3key_rec ) ;

return __3ret ;
}
}

return (struct kv_pair__pt__26_9CC_String13BTCollectable *)0 ;
}

char __dt__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFv (__0this , __0__free )register struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;

int __0__free ;

{ if (__0this ){ 
__0this -> __vptr__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[1];

if (! __0this -> destructed__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable ){ 
struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable __2iter ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __2iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__0this )))))) ), (& __2iter )) ;
if (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __2iter )) ){ 
while (1 ){ 
struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__4elem ;

__4elem = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , ((struct CC_List_Iterator_base *)(((struct CC_List_Iterator_base *)(& __2iter ))))) ))));
if (__4elem ){ 
__dl__FPv ( (char *)__4elem ) ;
}
else { break ;

}
}
}
}

if (__0this )if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} 
}

char clearAndDestroy__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFv (__0this )register struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__0this ;
{ 
__0this -> destructed__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = 1 ;
{ struct CC_TPtrSlistIterator__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable __1iter ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)__0this )))))) ), (& __1iter )) ;
if (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ){ 
while (1 ){ 
struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *__3elem ;

__3elem = (((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)(((struct CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , ((struct CC_List_Iterator_base *)(((struct CC_List_Iterator_base *)(& __1iter ))))) ))));
if (__3elem ){ 
__dt__41kv_pair__pt__26_9CC_String13BTCollectableFv ( __3elem -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable , 3) ;
__3elem -> f_element__59CC_Link__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable = 0 ;
__dl__FPv ( (char *)__3elem ) ;
}
else { break ;

}
}
}

}
}

struct CC_TPtrSlist__pt__11_9CC_String {	/* sizeof CC_TPtrSlist__pt__11_9CC_String == 20 */

struct CC_Link_base *f_head__11CC_Listbase ;
struct CC_Link_base *f_tail__11CC_Listbase ;
size_t f_length__11CC_Listbase ;

CC_Boolean destructed__31CC_TPtrSlist__pt__11_9CC_String ;

struct __mptr *__vptr__31CC_TPtrSlist__pt__11_9CC_String ;
};

struct CC_TPtrSlistIterator__pt__11_9CC_String {	/* sizeof CC_TPtrSlistIterator__pt__11_9CC_String == 12 */

struct CC_Listbase *f_list__21CC_List_Iterator_base ;
struct CC_Link_base *f_previous__21CC_List_Iterator_base ;
struct CC_Link_base *f_current__21CC_List_Iterator_base ;
};

struct CC_Link__pt__11_9CC_String {	/* sizeof CC_Link__pt__11_9CC_String == 12 */

struct CC_Link_base *f_next__12CC_Link_base ;
struct CC_Link_base *f_prev__12CC_Link_base ;

struct CC_String *f_element__26CC_Link__pt__11_9CC_String ;
};

struct CC_TPtrSlist__pt__11_9CC_String *__ct__31CC_TPtrSlist__pt__11_9CC_StringFRC31CC_TPtrSlist__pt__11_9CC_String (__0this , __1slist )register struct CC_TPtrSlist__pt__11_9CC_String *__0this ;

struct CC_TPtrSlist__pt__11_9CC_String *__1slist ;
{ 
struct CC_Listbase *__0__X142 ;

if (__0this || (__0this = (struct CC_TPtrSlist__pt__11_9CC_String *)__nw__FUi ( sizeof (struct CC_TPtrSlist__pt__11_9CC_String)) )){ ( (__0this = (struct CC_TPtrSlist__pt__11_9CC_String *)( (__0__X142 = (((struct
CC_Listbase *)__0this ))), ( ((__0__X142 || (__0__X142 = (struct CC_Listbase *)__nw__FUi ( sizeof (struct CC_Listbase)) ))?( ( (__0__X142 -> f_head__11CC_Listbase = 0 ), (__0__X142 ->
f_tail__11CC_Listbase = 0 )) , (__0__X142 -> f_length__11CC_Listbase = 0 )) :0 ), __0__X142 ) ) ), (__0this -> __vptr__31CC_TPtrSlist__pt__11_9CC_String = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[9]))
;

__0this -> destructed__31CC_TPtrSlist__pt__11_9CC_String = 0 ;
{ struct CC_TPtrSlistIterator__pt__11_9CC_String __1slist_iter ;

struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1slist_iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__11_9CC_String *)(((struct CC_TPtrSlist__pt__11_9CC_String *)(((struct CC_TPtrSlist__pt__11_9CC_String *)(__1slist ))))))))))) ), (& __1slist_iter ))
;

while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1slist_iter )) ?( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(&
__1slist_iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl -> f_element__26CC_Link__pt__11_9CC_String :(((struct CC_String *)0 )))) :(((struct CC_String *)0 )))) ){ 
struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

struct CC_String *__2__X143 ;

struct CC_Link__pt__11_9CC_String *__0__X62 ;

struct CC_Link_base *__0__X57 ;

struct CC_Link_base *__2__X63 ;

( (__2__X143 = ( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(& __1slist_iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl ->
f_element__26CC_Link__pt__11_9CC_String :(((struct CC_String *)0 )))) ), ( ( (__2__X63 = (struct CC_Link_base *)( (__0__X62 = 0 ), ( ((__0__X62 || (__0__X62 = (struct
CC_Link__pt__11_9CC_String *)__nw__FUi ( sizeof (struct CC_Link__pt__11_9CC_String)) ))?( (__0__X62 = (struct CC_Link__pt__11_9CC_String *)( (__0__X57 = (((struct CC_Link_base *)__0__X62 ))), ( ((__0__X57 || (__0__X57 = (struct
CC_Link_base *)__nw__FUi ( sizeof (struct CC_Link_base)) ))?( (__0__X57 -> f_next__12CC_Link_base = 0 ), (__0__X57 -> f_prev__12CC_Link_base = 0 )) :0 ), __0__X57 ) ) ),
(__0__X62 -> f_element__26CC_Link__pt__11_9CC_String = __2__X143 )) :0 ), __0__X62 ) ) ), ( insert__11CC_ListbaseFP12CC_Link_base ( ((struct CC_Listbase *)__0this ), __2__X63 ) ) )
) ) ;
}

}
} 
return __0this ;

}

struct CC_String *at__31CC_TPtrSlist__pt__11_9CC_StringCFUi (__0this , __1pos )register struct CC_TPtrSlist__pt__11_9CC_String *__0this ;

size_t __1pos ;
{ 
struct CC_TPtrSlistIterator__pt__11_9CC_String __1iter ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__11_9CC_String *)(((struct CC_TPtrSlist__pt__11_9CC_String *)(((struct CC_TPtrSlist__pt__11_9CC_String *)__0this )))))))))) ), (& __1iter ))
;
{ { int __1i ;

struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

__1i = 0 ;

for(;__1i <= __1pos ;__1i ++ ) { 
if (! __pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ){ 
struct ccException *__0__X49 ;

register struct ccBoundaryException *__0__X145 ;

register struct ccBoundaryException *__0__X146 ;

struct Exception *__2__Xtemp00ere3blb ;

register struct ccBoundaryException *__0__X48 ;

struct ccException *__0__X47 ;

{ 
struct ccBoundaryException __0__V144 ;

( (__0__X146 = ( (__0__X145 = ( ( (( ( ( ( (( (__0__X49 = (((struct ccException *)(&
__0__V144 )))), ( ((__0__X49 || (__0__X49 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( (__0__X49 = (struct
ccException *)__ct__9ExceptionFv ( ((struct Exception *)__0__X49 )) ), (__0__X49 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) :0 ), __0__X49 ) ) ), ((& __0__V144 )->
__vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , ((& __0__V144 )-> low__19ccBoundaryException = 0 )) , ((& __0__V144 )-> high__19ccBoundaryException = 0 )) ,
((& __0__V144 )-> index__19ccBoundaryException = __1i )) ), (& __0__V144 )) , (& __0__V144 )) ), ( ((! __0__X145 ->
f_thrown__9Exception )?( (__0__X145 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X145 ) ) ), ( (in_stack__12DestructableFv ( (struct Destructable *)__0__X146 ) ?(
(__2__Xtemp00ere3blb = (struct Exception *)( (__0__X48 = (struct ccBoundaryException *)__nw__9ExceptionSFUii ( sizeof (struct ccBoundaryException ), 0 ) ), ( ((__0__X48 || (__0__X48 =
(struct ccBoundaryException *)( (((char *)__nw__FUi ( (sizeof (struct ccBoundaryException))) ))) ))?( ( ( ( ( (__0__X48 = (struct
ccBoundaryException *)( (__0__X47 = (((struct ccException *)__0__X48 ))), ( ((__0__X47 || (__0__X47 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) )))
))?( ( (__0__X47 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X47 ), (struct Exception *)(((struct Exception *)(((struct Exception *)(((struct ccException *)(((struct ccException *)(((struct ccException *)(((struct
ccBoundaryException *)__0__X146 ))))))))))))) ), (__0__X47 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ), __0__X47 ) ) ), (__0__X48 -> __vptr__12Destructable =
(struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , (__0__X48 -> low__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X146 ))). low__19ccBoundaryException )) , (__0__X48 -> high__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X146 ))). high__19ccBoundaryException ))
, (__0__X48 -> index__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X146 ))). index__19ccBoundaryException )) , 0 ) :0 ), __0__X48 ) ) ), (((struct Exception *)((((struct
ccBoundaryException *)(((struct ccBoundaryException *)__2__Xtemp00ere3blb ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp00ere3blb = (struct Exception *)__0__X146 )), do_throw__9ExceptionFUiPCci ( __2__Xtemp00ere3blb , ((unsigned int )30 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/CC_Slist.cc"),
0 ) ) ) ;

( (( ( ((& __0__V144 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7]), (( ( ( ((((struct ccException *)(& __0__V144 )))?(
( ((((struct ccException *)(& __0__V144 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)(& __0__V144 )))?( ( ( ((((struct Exception *)(((struct
ccException *)(& __0__V144 )))))?( ( ((((struct Exception *)(((struct ccException *)(& __0__V144 )))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)(& __0__V144 )))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)(& __0__V144 )))))), 0 ) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
)) , 0 ) )) ;
} }
}

return ( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl ->
f_element__26CC_Link__pt__11_9CC_String :(((struct CC_String *)0 )))) ;

}

}
}

struct CC_String *remove__31CC_TPtrSlist__pt__11_9CC_StringFPC9CC_String ();

struct CC_String *removeAt__31CC_TPtrSlist__pt__11_9CC_StringFUi (__0this , __1pos )register struct CC_TPtrSlist__pt__11_9CC_String *__0this ;

size_t __1pos ;

{ 
struct CC_TPtrSlistIterator__pt__11_9CC_String __1iter ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__11_9CC_String *)__0this )))))) ), (& __1iter )) ;
{ { int __1i ;

__1i = 0 ;

for(;__1i <= __1pos ;__1i ++ ) { 
if (! __pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ){ 
struct ccException *__0__X49 ;

register struct ccBoundaryException *__0__X148 ;

register struct ccBoundaryException *__0__X149 ;

struct Exception *__2__Xtemp00ere3blb ;

register struct ccBoundaryException *__0__X48 ;

struct ccException *__0__X47 ;

{ 
struct ccBoundaryException __0__V147 ;

( (__0__X149 = ( (__0__X148 = ( ( (( ( ( ( (( (__0__X49 = (((struct ccException *)(&
__0__V147 )))), ( ((__0__X49 || (__0__X49 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( (__0__X49 = (struct
ccException *)__ct__9ExceptionFv ( ((struct Exception *)__0__X49 )) ), (__0__X49 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) :0 ), __0__X49 ) ) ), ((& __0__V147 )->
__vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , ((& __0__V147 )-> low__19ccBoundaryException = 0 )) , ((& __0__V147 )-> high__19ccBoundaryException = 0 )) ,
((& __0__V147 )-> index__19ccBoundaryException = __1i )) ), (& __0__V147 )) , (& __0__V147 )) ), ( ((! __0__X148 ->
f_thrown__9Exception )?( (__0__X148 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X148 ) ) ), ( (in_stack__12DestructableFv ( (struct Destructable *)__0__X149 ) ?(
(__2__Xtemp00ere3blb = (struct Exception *)( (__0__X48 = (struct ccBoundaryException *)__nw__9ExceptionSFUii ( sizeof (struct ccBoundaryException ), 0 ) ), ( ((__0__X48 || (__0__X48 =
(struct ccBoundaryException *)( (((char *)__nw__FUi ( (sizeof (struct ccBoundaryException))) ))) ))?( ( ( ( ( (__0__X48 = (struct
ccBoundaryException *)( (__0__X47 = (((struct ccException *)__0__X48 ))), ( ((__0__X47 || (__0__X47 = (struct ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) )))
))?( ( (__0__X47 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X47 ), (struct Exception *)(((struct Exception *)(((struct Exception *)(((struct ccException *)(((struct ccException *)(((struct ccException *)(((struct
ccBoundaryException *)__0__X149 ))))))))))))) ), (__0__X47 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ), __0__X47 ) ) ), (__0__X48 -> __vptr__12Destructable =
(struct __mptr *) __ptbl_vec__TKTemplate_cc_[7])) , (__0__X48 -> low__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X149 ))). low__19ccBoundaryException )) , (__0__X48 -> high__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X149 ))). high__19ccBoundaryException ))
, (__0__X48 -> index__19ccBoundaryException = ((*((struct ccBoundaryException *)__0__X149 ))). index__19ccBoundaryException )) , 0 ) :0 ), __0__X48 ) ) ), (((struct Exception *)((((struct
ccBoundaryException *)(((struct ccBoundaryException *)__2__Xtemp00ere3blb ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp00ere3blb = (struct Exception *)__0__X149 )), do_throw__9ExceptionFUiPCci ( __2__Xtemp00ere3blb , ((unsigned int )44 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/CC_Slist.cc"),
0 ) ) ) ;

( (( ( ((& __0__V147 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[7]), (( ( ( ((((struct ccException *)(& __0__V147 )))?(
( ((((struct ccException *)(& __0__V147 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), ((((struct ccException *)(& __0__V147 )))?( ( ( ((((struct Exception *)(((struct
ccException *)(& __0__V147 )))))?( ( ((((struct Exception *)(((struct ccException *)(& __0__V147 )))))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(((struct ccException *)(& __0__V147 )))))?(
( __dt__12DestructableFv ( ((struct Destructable *)(((struct Exception *)(((struct ccException *)(& __0__V147 )))))), 0 ) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
:( 0 ) )) , 0 ) :( 0 ) )) , (( 0 ) )) , 0 )
)) , 0 ) )) ;
} }
}

{ struct CC_String *__1key_val ;
struct CC_Link__pt__11_9CC_String *__1elem ;

struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

__1key_val = ( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl -> f_element__26CC_Link__pt__11_9CC_String :(((struct
CC_String *)0 )))) ;
__1elem = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , (struct CC_List_Iterator_base *)(& __1iter )) ))));
__dl__FPv ( (char *)__1elem ) ;

return __1key_val ;

}

}

}

}

static char *__opPCc__9CC_StringCFv ();

struct CC_String *find__31CC_TPtrSlist__pt__11_9CC_StringCFPC9CC_String (__0this , __1elem )register struct CC_TPtrSlist__pt__11_9CC_String *__0this ;

struct CC_String *__1elem ;
{ 
struct CC_TPtrSlistIterator__pt__11_9CC_String __1iter ;

struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__11_9CC_String *)(((struct CC_TPtrSlist__pt__11_9CC_String *)(((struct CC_TPtrSlist__pt__11_9CC_String *)__0this )))))))))) ), (& __1iter ))
;

while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ?( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl -> f_element__26CC_Link__pt__11_9CC_String :(((struct CC_String *)0 )))) :(((struct CC_String *)0 )))) ){ 
struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

struct CC_String *__0__X150 ;

if (( (__0__X150 = ( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))),
(__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl -> f_element__26CC_Link__pt__11_9CC_String :(((struct CC_String *)0 )))) ), ( (((unsigned int )((( compareTo__9CC_StringCFPCcQ2_9CC_String11caseCompare ( ((struct CC_String *)__0__X150 ), __opPCc__9CC_StringCFv ( (struct CC_String *)((struct
CC_String *)__1elem )) , ((int )0)) ) == 0 )?1 :0 )))) ) )
return ( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)(
((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl -> f_element__26CC_Link__pt__11_9CC_String :(((struct CC_String *)0 )))) ;
}

return (struct CC_String *)0 ;
}

struct CC_String *find__31CC_TPtrSlist__pt__11_9CC_StringCFPFP9CC_StringPv_UiPv (__0this , __1testFunc , __1d )register struct CC_TPtrSlist__pt__11_9CC_String *__0this ;

CC_Boolean (*__1testFunc )();
char *__1d ;
{ 
struct CC_TPtrSlistIterator__pt__11_9CC_String __1iter ;

struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__11_9CC_String *)(((struct CC_TPtrSlist__pt__11_9CC_String *)(((struct CC_TPtrSlist__pt__11_9CC_String *)__0this )))))))))) ), (& __1iter ))
;
while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ?( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl -> f_element__26CC_Link__pt__11_9CC_String :(((struct CC_String *)0 )))) :(((struct CC_String *)0 )))) ){ 
struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

if ((*(__1testFunc ))( ( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl ->
f_element__26CC_Link__pt__11_9CC_String :(((struct CC_String *)0 )))) , __1d ) ){ 
struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

return ( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl -> f_element__26CC_Link__pt__11_9CC_String :(((struct
CC_String *)0 )))) ;
}
}

return (struct CC_String *)0 ;
}

static char *__opPCc__9CC_StringCFv ();

CC_Boolean contains__31CC_TPtrSlist__pt__11_9CC_StringCFPC9CC_String (__0this , __1elem )register struct CC_TPtrSlist__pt__11_9CC_String *__0this ;

struct CC_String *__1elem ;
{ 
struct CC_TPtrSlistIterator__pt__11_9CC_String __1iter ;

struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__11_9CC_String *)(((struct CC_TPtrSlist__pt__11_9CC_String *)(((struct CC_TPtrSlist__pt__11_9CC_String *)__0this )))))))))) ), (& __1iter ))
;
while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ?( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl -> f_element__26CC_Link__pt__11_9CC_String :(((struct CC_String *)0 )))) :(((struct CC_String *)0 )))) ){ 
struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

struct CC_String *__0__X151 ;

if (( (__0__X151 = ( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))),
(__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl -> f_element__26CC_Link__pt__11_9CC_String :(((struct CC_String *)0 )))) ), ( (((unsigned int )((( compareTo__9CC_StringCFPCcQ2_9CC_String11caseCompare ( ((struct CC_String *)__0__X151 ), __opPCc__9CC_StringCFv ( (struct CC_String *)((struct
CC_String *)__1elem )) , ((int )0)) ) == 0 )?1 :0 )))) ) )
return (unsigned int )1 ;
}

return (unsigned int )0 ;
}

static char *__opPCc__9CC_StringCFv ();

struct CC_String *remove__31CC_TPtrSlist__pt__11_9CC_StringFPC9CC_String (__0this , __1elem )register struct CC_TPtrSlist__pt__11_9CC_String *__0this ;

struct CC_String *__1elem ;
{ 
struct CC_TPtrSlistIterator__pt__11_9CC_String __1iter ;

struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__11_9CC_String *)__0this )))))) ), (& __1iter )) ;
while (( (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ?( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(&
__1iter )))-> f_current__21CC_List_Iterator_base ) ))))), (__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl -> f_element__26CC_Link__pt__11_9CC_String :(((struct CC_String *)0 )))) :(((struct CC_String *)0 )))) ){ 
struct CC_Link__pt__11_9CC_String *__2__Xlink_item00iqjfdkl ;

struct CC_String *__0__X152 ;

if (( (__0__X152 = ( (__2__Xlink_item00iqjfdkl = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)( ((struct CC_List_Iterator_base *)((struct CC_TPtrSlistIterator__pt__11_9CC_String *)(& __1iter )))-> f_current__21CC_List_Iterator_base ) ))))),
(__2__Xlink_item00iqjfdkl ?__2__Xlink_item00iqjfdkl -> f_element__26CC_Link__pt__11_9CC_String :(((struct CC_String *)0 )))) ), ( (((unsigned int )((( compareTo__9CC_StringCFPCcQ2_9CC_String11caseCompare ( ((struct CC_String *)__0__X152 ), __opPCc__9CC_StringCFv ( (struct CC_String *)((struct
CC_String *)__1elem )) , ((int )0)) ) == 0 )?1 :0 )))) ) ){ 
struct CC_Link__pt__11_9CC_String *__3key_rec ;
struct CC_String *__3ret ;

__3key_rec = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , (struct CC_List_Iterator_base *)(& __1iter )) ))));
__3ret = __3key_rec -> f_element__26CC_Link__pt__11_9CC_String ;
__dl__FPv ( (char *)__3key_rec ) ;

return __3ret ;
}
}

return (struct CC_String *)0 ;
}

char __dt__31CC_TPtrSlist__pt__11_9CC_StringFv (__0this , __0__free )register struct CC_TPtrSlist__pt__11_9CC_String *__0this ;

int __0__free ;

{ if (__0this ){ 
__0this -> __vptr__31CC_TPtrSlist__pt__11_9CC_String = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[9];

if (! __0this -> destructed__31CC_TPtrSlist__pt__11_9CC_String ){ 
struct CC_TPtrSlistIterator__pt__11_9CC_String __2iter ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __2iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__11_9CC_String *)__0this )))))) ), (& __2iter )) ;
if (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __2iter )) ){ 
while (1 ){ 
struct CC_Link__pt__11_9CC_String *__4elem ;

__4elem = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , ((struct CC_List_Iterator_base *)(((struct CC_List_Iterator_base *)(& __2iter ))))) ))));
if (__4elem ){ 
__dl__FPv ( (char *)__4elem ) ;
}
else { break ;

}
}
}
}

if (__0this )if (__0__free & 1)__dl__FPv ( (char *)__0this ) ;
} 
}

char clearAndDestroy__31CC_TPtrSlist__pt__11_9CC_StringFv (__0this )register struct CC_TPtrSlist__pt__11_9CC_String *__0this ;
{ 
__0this -> destructed__31CC_TPtrSlist__pt__11_9CC_String = 1 ;
{ struct CC_TPtrSlistIterator__pt__11_9CC_String __1iter ;

( (__ct__21CC_List_Iterator_baseFP11CC_Listbase ( ((struct CC_List_Iterator_base *)(& __1iter )), ((struct CC_Listbase *)(((struct CC_Listbase *)(((struct CC_TPtrSlist__pt__11_9CC_String *)__0this )))))) ), (& __1iter )) ;
if (__pp__21CC_List_Iterator_baseFv ( (struct CC_List_Iterator_base *)(& __1iter )) ){ 
while (1 ){ 
struct CC_Link__pt__11_9CC_String *__3elem ;

__3elem = (((struct CC_Link__pt__11_9CC_String *)(((struct CC_Link__pt__11_9CC_String *)remove__11CC_ListbaseFR21CC_List_Iterator_base ( (struct CC_Listbase *)__0this , ((struct CC_List_Iterator_base *)(((struct CC_List_Iterator_base *)(& __1iter ))))) ))));
if (__3elem ){ 
__3elem -> f_element__26CC_Link__pt__11_9CC_String ?((*(((char (*)())(__3elem -> f_element__26CC_Link__pt__11_9CC_String -> __vptr__9CC_String [1]).f))))( ((struct CC_String *)((((char *)__3elem -> f_element__26CC_Link__pt__11_9CC_String ))+ (__3elem -> f_element__26CC_Link__pt__11_9CC_String -> __vptr__9CC_String [1]).d)),
3) :0 ;
__3elem -> f_element__26CC_Link__pt__11_9CC_String = 0 ;
__dl__FPv ( (char *)__3elem ) ;
}
else { break ;

}
}
}

}
}

int key__29CC_TValSlistIterator__pt__2_iCFv (__0this )register struct CC_TValSlistIterator__pt__2_i *__0this ;
{ 
struct CC_Link__pt__2_i *__1link_item ;

__1link_item = (((struct CC_Link__pt__2_i *)(((struct CC_Link__pt__2_i *)( ((struct CC_List_Iterator_base *)__0this )-> f_current__21CC_List_Iterator_base ) ))));
if (__1link_item ){ 
return ((*__1link_item -> f_element__16CC_Link__pt__2_i ));
}
else { 
register struct ccException *__0__X154 ;

register struct ccException *__0__X155 ;

struct Exception *__2__Xtemp00m1g3bcg ;

struct ccException *__0__X39 ;

{ 
struct ccException __0__V153 ;

( (__0__X155 = ( (__0__X154 = ( ( (( (__ct__9ExceptionFv ( ((struct Exception *)(& __0__V153 ))) ), ((& __0__V153 )->
__vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) ), (& __0__V153 )) , (& __0__V153 )) ), ( ((! __0__X154 -> f_thrown__9Exception )?(
(__0__X154 -> f_thrown_as_pointer__9Exception = 0 ), 0 ) :0 ), __0__X154 ) ) ), ( (in_stack__12DestructableFv ( (struct Destructable *)__0__X155 ) ?( (__2__Xtemp00m1g3bcg =
(struct Exception *)( (__0__X39 = (struct ccException *)__nw__9ExceptionSFUii ( sizeof (struct ccException ), 0 ) ), ( ((__0__X39 || (__0__X39 = (struct
ccException *)( (((char *)__nw__FUi ( (sizeof (struct ccException))) ))) ))?( ( (__0__X39 = (struct ccException *)__ct__9ExceptionFRC9Exception ( ((struct Exception *)__0__X39 ), (struct
Exception *)(((struct Exception *)(((struct Exception *)(((struct ccException *)__0__X155 ))))))) ), (__0__X39 -> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5])) , 0 ) :0 ), __0__X39 ) )
), (((struct Exception *)((((struct ccException *)(((struct ccException *)__2__Xtemp00m1g3bcg ))))-> f_temporary__9Exception = 1 )))) :(__2__Xtemp00m1g3bcg = (struct Exception *)__0__X155 )), do_throw__9ExceptionFUiPCci ( __2__Xtemp00m1g3bcg , ((unsigned int
)191 ), ((char *)"/VOB/olias/library/Common_Class/dti_cc/CC_Slist.cc"), 0 ) ) ) ;

( (( ( ((& __0__V153 )-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[5]), (( ( ( ((((struct Exception *)(& __0__V153 )))?(
( ((((struct Exception *)(& __0__V153 )))-> __vptr__12Destructable = (struct __mptr *) __ptbl_vec__TKTemplate_cc_[4]), ((((struct Exception *)(& __0__V153 )))?( ( __dt__12DestructableFv ( ((struct Destructable *)(((struct
Exception *)(& __0__V153 )))), 0 ) , (( 0 ) )) , 0 ) :( 0 ) )) , 0 )
:( 0 ) )) , (( 0 ) )) , 0 ) )) , 0 ) )) ;
} 
}
}

char __sti__TKTemplate_cc_______dummy1__ ()
{ __ct__13Iostream_initFv ( & iostream_init ) ;

}

char __std__TKTemplate_cc_______dummy1__ ()
{ __dt__13Iostream_initFv ( & iostream_init , 2) ;

}
static struct __linkl { struct __linkl * next;
char (*ctor)(); char (*dtor)(); } __link = 
{ (struct __linkl *)0, __sti__TKTemplate_cc_______dummy1__ ,__std__TKTemplate_cc_______dummy1__ };
struct __mptr __vtbl__31CC_TPtrSlist__pt__11_9CC_String[] = {0,0,0,
0,0,(__vptp)clearAndDestroy__31CC_TPtrSlist__pt__11_9CC_StringFv ,
0,0,0};
struct __mptr __vtbl__14Stack__pt__2_i[] = {0,0,0,
0,0,(__vptp)__dt__14Stack__pt__2_iFv ,
0,0,0};
extern struct __mptr __vtbl__19ccBoundaryException[];
extern struct __mptr __vtbl__17ccStringException[];
extern struct __mptr __vtbl__11ccException[];
static char print_exception__9ExceptionFv ();
static char *class_name__9ExceptionFv ();
static int isa__9ExceptionFPCc ();
struct __mptr __vtbl__9Exception__TKTemplate_cc[] = {0,0,0,
0,0,(__vptp)__dt__9ExceptionFv ,
0,0,(__vptp)print_exception__9ExceptionFv ,
0,0,(__vptp)class_name__9ExceptionFv ,
0,0,(__vptp)isa__9ExceptionFPCc ,
0,0,0};
struct __mptr __vtbl__12Destructable__TKTemplate_cc[] = {0,0,0,
0,0,(__vptp)__dt__12DestructableFv ,
0,0,0};
struct __mptr __vtbl__9CC_String__TKTemplate_cc[] = {0,0,0,
0,0,(__vptp)__dt__9CC_StringFv ,
0,0,0};
struct __mptr __vtbl__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable[] = {0,0,0,
0,0,(__vptp)clearAndDestroy__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectableFv ,
0,0,0};
struct __mptr __vtbl__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi[] = {0,0,0,
0,0,(__vptp)clearAndDestroy__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_StringiFv ,
0,0,0};

static char *__opPCc__9CC_StringCFv (__0this )struct CC_String *__0this ;

{ return (char *)__0this -> f_string__9CC_String ;

}

int is__9ExceptionFPCcT1 ();

static int isa__9ExceptionFPCc (__0this , __2type )struct Exception *__0this ;

char *__2type ;

{ return is__9ExceptionFPCcT1 ( __0this , __2type , (char *)"Exception") ;

}

static char *class_name__9ExceptionFv (__0this )struct Exception *__0this ;
{ return (char *)"Exception";

}

static char print_exception__9ExceptionFv (__0this )struct Exception *__0this ;
{ }

int initcount__13Iostream_init ;

int stdstatus__13Iostream_init ;

char (*stdioflush__3ios )();

long nextword__3ios ;

long nextbit__3ios ;

long floatfield__3ios ;

long adjustfield__3ios ;

long basefield__3ios ;

struct Jump_Environment *g_used_jump_env_stack__16Jump_Environment ;

struct Jump_Environment *g_jump_env_stack__16Jump_Environment ;

struct Unwind_Record g_stack__12Unwind_Stack [512];

unsigned short g_top__12Unwind_Stack ;

char *f_msg_memory_already_freed__10Exceptions ;

char *f_msg_out_of_obj_stack_memory__10Exceptions ;

char *f_msg_out_of_exception_memory__10Exceptions ;

char *f_msg_throw_ptr_to_stack__10Exceptions ;

char *f_msg_throw_from_destructor__10Exceptions ;

char *f_msg_throw_from_error_handler__10Exceptions ;

char *f_msg_throw_from_terminate__10Exceptions ;

char *f_msg_no_current_exception__10Exceptions ;

char *f_msg_not_caught__10Exceptions ;

char *f_msg_initialized_twice__10Exceptions ;

char *f_msg_not_initialized__10Exceptions ;

char *f_msg_throw_message__10Exceptions ;

char *f_msg_application_error__10Exceptions ;

char *f_msg_internal_error__10Exceptions ;

 __Q2_10Exceptions15error_handler_t g_error_handler__10Exceptions ;

char *g_next_avail__9Exception ;

struct Exception *g_current_exception__9Exception ;

unsigned short g_size__12Destructable ;

char *g_stack_start__12Destructable ;
struct __mptr* __ptbl_vec__TKTemplate_cc_[] = {
__vtbl__50CC_TPtrSlist__pt__30_27kv_pair__pt__12_9CC_Stringi,
__vtbl__64CC_TPtrSlist__pt__44_41kv_pair__pt__26_9CC_String13BTCollectable,
__vtbl__9CC_String__TKTemplate_cc,
__vtbl__12Destructable__TKTemplate_cc,
__vtbl__9Exception__TKTemplate_cc,
__vtbl__11ccException,
__vtbl__17ccStringException,
__vtbl__19ccBoundaryException,
__vtbl__14Stack__pt__2_i,
__vtbl__31CC_TPtrSlist__pt__11_9CC_String,

};

/* Include file stack for ObjectCenter dependency info */

/* the end */
