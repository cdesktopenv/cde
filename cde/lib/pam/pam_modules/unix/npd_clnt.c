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
/* $TOG: npd_clnt.c /main/5 1999/10/15 17:23:53 mgreess $ */
/*
 *	npd_clnt.c
 *	Contains all the client-side routines to communicate
 *	with the NIS+ passwd update deamon.
 *
 *	Copyright (c) 1992-5 Sun Microsystems Inc
 *	All Rights Reserved.
 */

#ident  "@(#)npd_clnt.c 1.11     95/09/19 SMI"

#ifdef PAM_NISPLUS

#include <syslog.h>
#include <string.h>
#include <shadow.h>
#include <rpc/rpc.h>
#include <rpc/xdr.h>
#include <rpc/des_crypt.h>
#include <mp.h>
#include <rpc/key_prot.h>
#include <rpcsvc/nis.h>
#include <rpcsvc/nispasswd.h>
#include <memory.h>
#include <sys/time.h>

#define	_NPD_PASSMAXLEN	16

extern int	pow();		/* from libmp.so (undocumented) */
extern int	_mp_move();	/* from libmp.so (undocumented) */
extern int	sdiv();		/* from libmp.so (undocumented) */
extern bool_t	__npd_ecb_crypt();
extern bool_t	__npd_cbc_crypt();
extern void	*calloc();
extern void	free();

/*
 * given the domain return the client handle to the rpc.nispasswdd
 * that I need to contact and the master_servers' publickey.
 * returns TRUE on success and FALSE on failure.
 */
bool_t
npd_makeclnthandle(domain, clnt, srv_pubkey, keysize)
char	*domain;
CLIENT	**clnt;		/* returned */
char	*srv_pubkey;	/* buf to hold the pubkey; returned */
int	keysize;	/* size of buffer */
{
	nis_server	**srvs;		/* servers that serve 'domain' */
	nis_server	*master_srv;
	char		buf[NIS_MAXNAMELEN];
	CLIENT		*tmpclnt = NULL;

	if (domain == NULL || *domain == '\0')
		domain = nis_local_directory();

	/* strlen("org_dir.") + null + "." = 10 */
	if ((strlen(domain) + 10) > (size_t) NIS_MAXNAMELEN)
		return (FALSE);
	(void) sprintf(buf, "org_dir.%s", domain);
	if (buf[strlen(buf) - 1] != '.')
		(void) strcat(buf, ".");

	srvs = nis_getservlist(buf);
	if (srvs == NULL) {
		/* can't find any of the servers that serve this domain */
		/* something is very wrong ! */
		syslog(LOG_ERR,
			"can't get a list of servers for %s domain",
			domain);
		return (FALSE);
	}
	master_srv = srvs[0];	/* the first one is always the master */

	/*
	 * copy the publickey -- should be safe to assume there is one,
	 * BUT just in case lets check first !
	 */
	switch (master_srv->key_type) {
	case NIS_PK_DH:
		if (keysize < master_srv->pkey.n_len) {
			syslog(LOG_ERR,
			"buf is %d bytes only, need %d bytes for the key",
			keysize, master_srv->pkey.n_len);

			(void) nis_freeservlist(srvs);
			return (FALSE);
		}
		(void) strcpy(srv_pubkey, master_srv->pkey.n_bytes);
		break;
	case NIS_PK_NONE:
	default:
		/* server does not have a D-H key-pair */
		syslog(LOG_ERR, "no publickey for %s", master_srv->name);
		(void) nis_freeservlist(srvs);
		return (FALSE);
	}

	/*
	 * now that we have the universal addr for the master server,
	 * lets create the client handle to rpc.nispasswdd.
	 * always use VC and attempt to create an authenticated handle.
	 * nis_make_rpchandle() will attempt to use auth_des first,
	 * if user does not have D-H keys, then it will try auth_sys.
	 * sendsz and recvsz are 0 ==> choose defaults.
	 */
	tmpclnt = nis_make_rpchandle(master_srv, 0, NISPASSWD_PROG,
			NISPASSWD_VERS, ZMH_VC+ZMH_AUTH, 0, 0);

	/* done with server list */
	(void) nis_freeservlist(srvs);
	if (tmpclnt == NULL) {
		/*
		 * error syslog'd by nis_make_rpchandle()
		 */
		return (FALSE);
	}
	*clnt = tmpclnt;
	return (TRUE);
}

/*
 * generate a common DES key with the publickey I give and
 * my secret key -- used by the client-side because the secret
 * key is not cached with keyserv.
 * returns TRUE on success and FALSE on failure.
 */
bool_t
__get_cmnkey(public, secret, deskey)
char		*public;
char		*secret;
des_block	*deskey;
{
	MINT		*pub;
	MINT		*sec;
	MINT		*cmn;
	MINT		*tmp;
	short		reg;
	short		base = (1 << 8);
	int		i;
	char		*k;

	pub = xtom(public);
	sec = xtom(secret);

	if ((pub == NULL) || (sec == NULL))
		return (FALSE);

	cmn = itom(0);
	(void) pow(pub, sec, xtom(HEXMODULUS), cmn);

	/*
	 * Choose middle 64 bits of the common key to use as our des
	 * key, possibly overwriting the lower order bits by setting
	 * parity.
	 */

	tmp = itom(0);
	(void) _mp_move(cmn, tmp);
	for (i = 0; i < ((KEYSIZE - 64) / 2) / 8; i++) {
		(void) sdiv(tmp, base, tmp, &reg);
	}
	k = deskey->c;
	for (i = 0; i < 8; i++) {
		(void) sdiv(tmp, base, tmp, &reg);
		*k++ = reg;
	}
	des_setparity((char *)deskey);

	mfree(tmp);
	mfree(sec);
	mfree(pub);
	mfree(cmn);

	return (TRUE);
}

/* Default timeout can be changed using clnt_control() */
static	struct	timeval	TIMEOUT = { 55, 0 };

/*
 * initiate the passwd update request session by sending
 * username, domainname, the generated public key and
 * the callers' old passwd encrypted with the common DES key.
 * if it succeeds, decrypt the identifier and randval sent in
 * the response; otherwise return an appropriate error code.
 */
nispasswd_status
nispasswd_auth(user, domain, oldpass, u_pubkey, deskey, clnt,
			ident, randval, err)
char		*user;		/* user name */
char		*domain;	/* domain */
char		*oldpass;	/* clear old password */
unsigned char	*u_pubkey;	/* users' public key */
des_block	*deskey;	/* the common DES key */
CLIENT		*clnt;		/* client handle to rpc.nispasswdd */
unsigned long	*ident;		/* ID, returned on first attempt */
unsigned long	*randval;	/* R, returned on first attempt */
int		*err;		/* error code, returned */
{
	npd_request	req_arg;
	nispasswd_authresult	res;
	des_block	ivec;
	unsigned char	xpass[_NPD_PASSMAXLEN];
	des_block	cryptbuf;
	int		cryptstat;
	int		i;
	char		*keystr = "DES";

	if ((user == NULL || *user == '\0') ||
		(domain == NULL || *domain == '\0') ||
		(oldpass == NULL || *oldpass == '\0') ||
		(u_pubkey == NULL || *u_pubkey == '\0') ||
		(deskey == (des_block *) NULL) ||
		(clnt == (CLIENT *) NULL)) {
		*err = NPD_INVALIDARGS;
		return (NPD_FAILED);
	}
	(void) memset((char *)&req_arg, 0, sizeof (req_arg));
	(void) memset((char *)&res, 0, sizeof (res));

	/* encrypt the passwd with the common des key */
	if (strlen(oldpass) > (size_t) _NPD_PASSMAXLEN) {
		*err = NPD_BUFTOOSMALL;
		return (NPD_FAILED);
	}
	(void) strcpy((char *)xpass, oldpass);
	for (i = strlen(oldpass); i < _NPD_PASSMAXLEN; i++)
		xpass[i] = '\0';

	ivec.key.high = ivec.key.low = 0;
	cryptstat = cbc_crypt((char *) deskey, (char *) xpass,
			_NPD_PASSMAXLEN, DES_ENCRYPT | DES_HW,
			(char *) &ivec);

	if (DES_FAILED(cryptstat)) {
		*err = NPD_ENCRYPTFAIL;
		return (NPD_FAILED);
	}

	req_arg.username = user;
	req_arg.domain = domain;
	req_arg.key_type = keystr;
	req_arg.user_pub_key.user_pub_key_len =
			strlen((char *)u_pubkey) + 1;
	req_arg.user_pub_key.user_pub_key_val = u_pubkey;
	req_arg.npd_authpass.npd_authpass_len = _NPD_PASSMAXLEN;
	req_arg.npd_authpass.npd_authpass_val = xpass;
	req_arg.ident = *ident;		/* on re-tries ident is non-zero */

	if (clnt_call(clnt, NISPASSWD_AUTHENTICATE,
			(xdrproc_t) xdr_npd_request, (caddr_t) &req_arg,
			(xdrproc_t) xdr_nispasswd_authresult, (caddr_t) &res,
			TIMEOUT) != RPC_SUCCESS) {

		/* following msg is printed on stderr */
		(void) clnt_perror(clnt, "call to rpc.nispasswdd failed");
		*err = NPD_SRVNOTRESP;
		return (NPD_FAILED);
	}

	switch (res.status) {
	case NPD_SUCCESS:
	case NPD_TRYAGAIN:
		/*
		 * decrypt the ident & randval
		 */
		cryptbuf.key.high =
			ntohl(res.nispasswd_authresult_u.npd_verf.npd_xid);
		cryptbuf.key.low =
			ntohl(res.nispasswd_authresult_u.npd_verf.npd_xrandval);

		if (! __npd_ecb_crypt(ident, randval, &cryptbuf,
			sizeof (des_block), DES_DECRYPT, deskey)) {
			*err = NPD_DECRYPTFAIL;
			return (NPD_FAILED);
		}
		return (res.status);

	case NPD_FAILED:
		*err = res.nispasswd_authresult_u.npd_err;
		return (NPD_FAILED);
	default:
		/*
		 * should never reach this case !
		 */
		*err = NPD_SYSTEMERR;
		return (NPD_FAILED);
	}
	/* NOTREACHED */
}

/*
 * authenticated the caller, now send the identifier; and the
 * new password and the random value encrypted with the common
 * DES key. Send any other changed password information in the
 * clear.
 */
int
nispasswd_pass(clnt, ident, randval, deskey, newpass, gecos, shell, err, errlst)
CLIENT		*clnt;		/* client handle to rpc.nispasswdd */
unsigned long	ident;		/* ID */
unsigned long	randval;	/* R */
des_block	*deskey;	/* common DES key */
char		*newpass;	/* clear new password */
char		*gecos;		/* gecos */
char		*shell;		/* shell */
int		*err;		/* error code, returned */
nispasswd_error	**errlst;	/* error list on partial success, returned */
{
	npd_update	send_arg;
	nispasswd_updresult	result;
	npd_newpass	cryptbuf;
	unsigned long	tmp_xrval;
	nispasswd_error	*errl = NULL, *p;

	if ((clnt == (CLIENT *) NULL) ||
		(deskey == (des_block *) NULL) ||
		(newpass == NULL || *newpass == '\0')) {
		*err = NPD_INVALIDARGS;
		return (NPD_FAILED);
	}
	(void) memset((char *)&send_arg, 0, sizeof (send_arg));
	(void) memset((char *)&result, 0, sizeof (result));

	send_arg.ident = ident;

	if (! __npd_cbc_crypt(&randval, newpass, strlen(newpass),
		&cryptbuf, _NPD_PASSMAXLEN, DES_ENCRYPT, deskey)) {

		*err = NPD_ENCRYPTFAIL;
		return (NPD_FAILED);
	}
	tmp_xrval = cryptbuf.npd_xrandval;
	cryptbuf.npd_xrandval = htonl(tmp_xrval);
	send_arg.xnewpass = cryptbuf;

	/* gecos */
	send_arg.pass_info.pw_gecos = gecos;

	/* shell */
	send_arg.pass_info.pw_shell = shell;

	if (clnt_call(clnt, NISPASSWD_UPDATE,
		(xdrproc_t) xdr_npd_update, (caddr_t) &send_arg,
		(xdrproc_t) xdr_nispasswd_updresult, (caddr_t) &result,
		TIMEOUT) != RPC_SUCCESS) {

		/* printed to stderr */
		(void) clnt_perror(clnt, "call to rpc.nispasswdd failed");
		*err = NPD_SRVNOTRESP;
		return (NPD_FAILED);
	}
	switch (result.status) {
	case NPD_SUCCESS:
		return (NPD_SUCCESS);
	case NPD_PARTIALSUCCESS:
		/* need to assign field/err code */
		errl = &result.nispasswd_updresult_u.reason;
		if (errl == (struct nispasswd_error *) NULL) {
			*err = NPD_SYSTEMERR;
			return (NPD_FAILED);
		}
		*errlst = (nispasswd_error *)
				calloc(1, sizeof (nispasswd_error));
		if (*errlst == (struct nispasswd_error *) NULL) {
			*err = NPD_SYSTEMERR;
			return (NPD_FAILED);
		}

		for (p = *errlst; errl != NULL; errl = errl->next) {
			p->npd_field = errl->npd_field;
			p->npd_code = errl->npd_code;
			if (errl->next != NULL) {
				p->next = (nispasswd_error *)
					calloc(1, sizeof (nispasswd_error));
				p = p->next;
			} else
			p->next = (nispasswd_error *) NULL;
		}
		return (NPD_PARTIALSUCCESS);
	case NPD_FAILED:
		*err = result.nispasswd_updresult_u.npd_err;
		return (NPD_FAILED);
	default:
		/*
		 * should never reach this case !
		 */
		*err = NPD_SYSTEMERR;
		return (NPD_FAILED);
	}
}

void
__npd_free_errlist(list)
nispasswd_error *list;
{
	nispasswd_error *p;

	if (list == NULL)
		return;
	for (; list != NULL; list = p) {
		p = list->next;
		free(list);
	}
	list = NULL;
}
#else
#if defined(sun)
/* Quiet warnings */
static int dummy;
#endif /* sun */
#endif /* PAM_NISPLUS */
