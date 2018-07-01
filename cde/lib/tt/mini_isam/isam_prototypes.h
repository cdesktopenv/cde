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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

/* JET: This code was generated using cproto, plus some fixes that
 * having proper prototype definitions pointed out.
 */

/* isadd1key.c */
int _add1key(Fcb *fcb, Keydesc2 *pkeydesc2, char *record, Recno recnum, char *newkey);

/* isaddindex.c */
int isaddindex(int isfd, struct keydesc *keydesc);

/* isaddprimary.c */
int isaddprimary(int isfd, struct keydesc *keydesc);

/* isalloc.c */
char *_ismalloc(unsigned int nbytes);
char *_isrealloc(char *oldaddr, unsigned int nbytes);
char *_isallocstring(char *str);
void _isfreestring(char *str);

/* isamaddindex.c */
int _amaddindex(Bytearray *isfhandle, struct keydesc *keydesc, struct errcode *errcode);
int _amaddprimary(Bytearray *isfhandle, struct keydesc *keydesc, struct errcode *errcode);
int _create_index(Fcb *fcb, Keydesc2 *pkeydesc2);

/* isamdelcurr.c */

/* isamdelrec.c */
int _amdelrec(Bytearray *isfhandle, Recno recnum, struct errcode *errcode);
void _delkeys(Fcb *fcb, char *record, Recno recnum);

/* isamerror.c */
void _amseterrcode(struct errcode *errcode, int is_errno);

/* isamhooks.c */
void _isam_entryhook(void);
void _isam_exithook(void);

/* isamopen.c */
int _amopen(char *isfname, enum openmode openmode, Bool *varflag, int *minlen, int *maxlen, Bytearray *isfhandle, Bytearray *curpos, struct errcode *errcode);
Fcb *_openfcb(Bytearray *isfhandle, struct errcode *errcode);
char *_getisfname(Bytearray *isfhandle);
Bytearray _makeisfhandle(char *isfname);

/* isamwrite.c */
int _amwrite(Bytearray *isfhandle, char *record, int reclen, Bytearray *curpos, Recno *recnum, struct errcode *errcode);
int _addkeys(Fcb *fcb, char *record, Recno recnum);

/* isapplmr.c */
int _isapplmr(int isfd, char *buffer);

/* isapplmw.c */
int _isapplmw(int isfd, char *magicstring);

/* isbsearch.c */
char *_isbsearch(char *key, char *table, int nelems, int keylen,
                 int (*cmpf)(char *, char *));

/* isbtree2.c */
void _isbtree_insert(Btree *btree, char *key);
void leftkey_up(Btree *btree, int level);

/* isbtree3.c */
void _isbtree_remove(Btree *btree);

/* isbtree.c */
Btree *_isbtree_create(Fcb *fcb, Keydesc2 *pkeydesc2);
void _isbtree_destroy(Btree *btree);
void _isbtree_search(Btree *btree, char *key);
char *_isbtree_current(Btree *btree);
char *_isbtree_next(Btree *btree);

/* isbuild.c */
int isbuild(char *isfname, int recordlength, struct keydesc *primkey, int mode);

/* isbytearray.c */
Bytearray _bytearr_getempty(void);
Bytearray _bytearr_new(u_short len, char *data);
Bytearray _bytearr_dup(Bytearray *old);
void _bytearr_free(Bytearray *barray);
int _bytearr_cmp(Bytearray *l, Bytearray *r);

/* ischange1key.c */
int _change1key(Fcb *fcb, Keydesc2 *pkeydesc2, char *record, char *oldrecord, Recno recnum, char *newkey);

/* isclose.c */
int isclose(int isfd);

/* iscntl.c */
int iscntl(int isfd, int func, ...);

/* iscurpos.c */
int isgetcurpos(int isfd, int *len, char **buf);
int issetcurpos(int isfd, char *buf);

/* isdatconv.c */
long ldlong(char *p);
int stlong(long val, char *p);
short ldint(char *p);
u_short ldunshort(char *p);
int stint(short val, char *p);
int ldchar(char *src, int len, char *dst);
int stchar(char *src, char *dst, int len);
int ldchar2(char *src, int len, char *dst);
int stchar2(char *src, char *dst, int len);
float ldfloat(char *p);
int stfloat(float f, char *p);
double lddbl(char *p);
int stdbl(double val, char *p);

/* isdel1key.c */
void _del1key(Fcb *fcb, Keydesc2 *pkeydesc2, char *record, Recno recnum);

/* isdelcurr.c */
int isdelcurr(int isfd);

/* isdelrec.c */
int isdelrec(int isfd, long recnum);

/* isdiskbufs2.c */
Bufhdr *_isdisk_fix(Fcb *fcb, int unixfd, Blkno blkno, int mode);
void _isdisk_unfix(Bufhdr *p);
void _isdisk_commit1(Bufhdr *p);
void _isdisk_commit(void);
void _isdisk_rollback(void);
Bufhdr *_isdisk_refix(Bufhdr *p, int newmode);
void _isdisk_sync(void);
void _isdisk_inval(void);

/* isdlink.c */
void _isdln_base_insert(char *base, struct dlink *l, struct dlink *e);
void _isdln_base_append(char *base, struct dlink *l, struct dlink *e);
void _isdln_base_remove(char *base, struct dlink *e);
struct dlink *_isdln_base_first(char *base, struct dlink *l);
struct dlink *_isdln_base_next(char *base, struct dlink *l);
struct dlink *_isdln_base_prev(char *base, struct dlink *l);
void _isdln_base_makeempty(char *base, struct dlink *l);
int _isdln_base_isempty(char *base, struct dlink *l);

/* iserase.c */
int iserase(char *isfname);

/* iserror.c */
void _isfatal_error(char *msg);
void _isfatal_error1(char *msg);
void _isam_warning(char *msg);
int _isfatal_error_set_func(int (*func)(void));
void _setiserrno2(int error, int is1, int is2);
void _seterr_errcode(struct errcode *errcode);

/* isfab.c */
Fab *_fab_new(char *isfname, enum openmode openmode, Bool varlen, int minreclen, int maxreclen);
void _fab_destroy(Fab *fab);

/* isfcb.c */
Fcb *_isfcb_create(char *isfname, int crdat, int crind, int crvar, int owner, int group, int u_mask, struct errcode *errcode);
void _isfcb_setreclength(Fcb *fcb, Bool varflag, int minreclen, int maxreclen);
Fcb *_isfcb_open(char *isfname, struct errcode *errcode);
int _isfcb_nfds(Fcb *fcb);
void _isfcb_remove(Fcb *fcb);
void _isfcb_close(Fcb *fcb);
int _isfcb_cntlpg_w(Fcb *fcb);
int _isfcb_cntlpg_w2(Fcb *fcb);
int _isfcb_cntlpg_r(Fcb *fcb);
int _isfcb_cntlpg_r2(Fcb *fcb);
int _check_isam_magic(Fcb *fcb);
int _open2_indfile(Fcb *fcb);

/* isfcbindex.c */
int _isfcb_primkeyadd(Fcb *fcb, Keydesc2 *keydesc2);
int _isfcb_primkeydel(Fcb *fcb);
int _isfcb_altkeyadd(Fcb *fcb, Keydesc2 *keydesc2);
Keydesc2 *_isfcb_findkey(Fcb *fcb, Keydesc2 *keydesc2);
int _isfcb_altkeydel(Fcb *fcb, Keydesc2 *keydesc2);
Keydesc2 *_isfcb_indfindkey(Fcb *fcb, int keyid);

/* isfcbwatchfd.c */
int _watchfd_incr(int n);
int _watchfd_decr(int n);
int _watchfd_check(void);
int _watchfd_max_set(int n);
int _watchfd_max_get(void);

/* isfd.c */
Isfd _isfd_insert(Fab *fab);
Fab *_isfd_find(Isfd isfd);
void _isfd_delete(Isfd isfd);

/* isfileio.c */
void _cp_tofile(Fcb *fcb, int unixfd, char *data, long pos, int len);
void _cp_fromfile(Fcb *fcb, int unixfd, char *data, long pos, int len);
Blkno _extend_file(Fcb *fcb, int unixfd, Blkno oldsize);

/* isfixrec.c */
int _flrec_write(Fcb *fcb, char *record, Recno *recnum, int reclen);
int _flrec_read(Fcb *fcb, char *record, Recno recnum, int *reclen);
long _fl_getpos(Fcb *fcb, Recno recnum);
int _flrec_rewrite(Fcb *fcb, char *record, Recno recnum, int reclen);
int _flrec_delete(Fcb *fcb, Recno recnum);
int _flrec_wrrec(Fcb *fcb, char *record, Recno recnum, int reclen);

/* isfname.c */
void _makedat_isfname(char *isfname);
void _makeind_isfname(char *isfname);
void _makevar_isfname(char *isfname);
void _removelast(char *path);
char *_lastelement(char *path);

/* isgarbage.c */
int isgarbage(char *isfname);

/* isindexconv.c */
void stkey(Keydesc2 *pkeydesc2, char *p);
void ldkey(struct keydesc2 *pkeydesc2, char *p);

/* isindexinfo.c */
int isindexinfo(int isfd, struct keydesc *buffer, int number);

/* isindfreel.c */
Blkno _isindfreel_alloc(Fcb *fcb);
void _isindfreel_free(Fcb *fcb, Blkno blkno);

/* iskeyaux.c */
void _iskey_extract(Keydesc2 *pkeydesc2, char *recp, char *keyp);
Bufhdr *_allockpage(Fcb *fcb, int capac, int level, Blkno *blkno);

/* iskeycalc.c */
int getkeysperleaf(int keylen);
int getkeyspernode(int keylen);
int le_odd(int n);

/* iskeycmp.c */
void _iskeycmp_set(Keydesc2 *pkeydesc2, int nparts);
int _iskeycmp(char *lkey, char *rkey);

/* iskeyconv.c */
void _iskey_itox(struct keydesc2 *pikdesc, struct keydesc *pxkdesc);
void _iskey_xtoi(struct keydesc2 *pikdesc, struct keydesc *pxkdesc);

/* iskeyvalid.c */
int _validate_keydesc(struct keydesc *keydesc, int minreclen);

/* isminmax.c */
void _iskey_fillmax(struct keydesc2 *pkeydesc2, char *keybuf);
void _iskey_fillmin(struct keydesc2 *pkeydesc2, char *keybuf);

/* ismngfcb.c */
void _mngfcb_insert(Fcb *fcb, Bytearray *isfhandle);
Fcb *_mngfcb_find(Bytearray *isfhandle);
void _mngfcb_delete(Bytearray *isfhandle);
Bytearray *_mngfcb_victim(void);

/* isopen.c */
int isopen(char *isfname, int mode);

/* ispageio.c */
void _isseekpg(int fd, Blkno pgno);
void _isreadpg(int fd, char *buf);
void _iswritepg(int fd, char *buf);

/* isperm.c */
enum openmode _getopenmode(int mode);
enum readmode _getreadmode(int mode);

/* isread.c */
int isread(int isfd, char *record, int mode);

/* isrename.c */
int isrename(char *oldname, char *newname);

/* isrepair.c */
int isrepair(char *isfname, int verbose);

/* isrewcurr.c */
int isrewcurr(int isfd, char *record);

/* isrewrec.c */
int isrewrec(int isfd, long recnum, char *record);

/* issignals.c */
int _issignals_cntl(int opt);
void _issignals_mask(void);
void _issignals_unmask(void);

/* issort.c */
Issort *_issort_create(int reclen, int nrecs, int (*compfunc)(char *, char *));
void _issort_destroy(Issort *srt);
void _issort_insert(Issort *srt, char *record);
void _issort_sort(Issort *srt);
void _issort_rewind(Issort *srt);
char *_issort_read(Issort *srt);

/* isstart.c */
int isstart(int isfd, struct keydesc *keydesc, int length, char *record, int mode);

/* issync.c */
int issync(void);
int isfsync(int isfd);
int _issync(void);
int _isfsync(int isfd);

/* isvarrec.c */
int _vlrec_write(Fcb *fcb, char *record, Recno *recnum, int reclen);
int _vlrec_read(Fcb *fcb, char *record, Recno recnum, int *reclen);
long _vl_getpos(Fcb *fcb, Recno recnum);
int _vlrec_rewrite(Fcb *fcb, char *record, Recno recnum, int reclen);
int _vlrec_delete(Fcb *fcb, Recno recnum);
int _vlrec_wrrec(Fcb *fcb, char *record, Recno recnum, int reclen);
long _istail_insert(Fcb *fcb, char *tailp, int taillen);
int _istail_modify(Fcb *fcb, long offset, char *tailp, int taillen);

/* isvars.c */

/* isversion.c */

/* iswrite.c */
int iswrite(int isfd, char *record);

/* iswrrec.c */
int iswrrec(int isfd, long recnum, char *record);

