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
/* $XConsortium: SmHftRing.c /main/5 1996/11/01 10:17:15 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmHftRing.c
 **
 **  Project:     DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  This file contains the AIX 3.2.x specific code required to enable and
 **  disable the HFT ring. This is used for session locking.
 **
 *****************************************************************************
 *************************************<+>*************************************/

#include <stdio.h>
#include <sys/hft.h>
#include <fcntl.h>
#include "Sm.h"

#include <X11/Xosdefs.h>
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

/*
 * Local functions.
 */

static int open_hft_file(char *);
static void close_hft_file(int);
static int get_hft_ring(void);
static int process_hft(int, unsigned int);
static int scan_hft_ring(int);
static int my_strpos(char *, char);
static int is_local_display(void);

#define HFTRFILE      "/dev/hft/0"
#define HFQSTAT       hftqstat
#define HFSMGRCMD     hftsmgrcmd
#define HFQEIO        HFQERROR
#define HFCSMGR       HFTCSMGR
#define HFQSMGR       HFTQSMGR
#define HFVTSTATUS    (HFVTACTIVE | HFVTNOHOTKEY)
#define HF_VTIODN     hf_vtid

/*
 * Global variables.
 */
int                   hft_file;
struct HFQSTAT        hft_ring;

static int
open_hft_file(char *hft_file_name)
{
  return(open(hft_file_name, O_RDONLY));
}

static void
close_hft_file(int hft_file)
{
  close(hft_file); 
}

static int
get_hft_ring(void)
{
  int rc = -1;
  struct hfbuf hft_buffer;

  hft_buffer.hf_bufp = (char *)&hft_ring;
  hft_buffer.hf_buflen = sizeof(hft_ring);


 /*
  * Open the hft queue manager.
  */ 
  if ((hft_file = open_hft_file(HFTRFILE)) >= 0)
  {
    rc = ioctl(hft_file, HFQSMGR, &hft_buffer);
    close_hft_file(hft_file);
  }

  return(rc);
}

static int
process_hft(int hft_action, unsigned int hft_vtid)
{
  struct HFSMGRCMD hft_manager;
  char hft_file_name[20];
  int rc = -1;

  hft_manager.hf_cmd = hft_action;
  hft_manager.hf_vtid = hft_vtid;

  sprintf(hft_file_name, "/dev/hft/%u", hft_vtid);
  if ((hft_file = open_hft_file(hft_file_name)) >= 0)
  {
    rc = ioctl(hft_file, HFCSMGR, &hft_manager);
    close_hft_file(hft_file);  
  }

  return(rc);
}

static int
scan_hft_ring(int hft_action)
{
  int hft;
  int rc = 0;

  if (get_hft_ring() != 0)
  {
    return(-1);
  }

  for (hft = 0; hft < hft_ring.hf_numvts; hft++)
  {
    if (!(hft_ring.hf_vtinfo[hft].hf_vtstate & HFVTSTATUS))
    {
      if (process_hft(hft_action, hft_ring.hf_vtinfo[hft].HF_VTIODN) != 0)
      {
        return(-1);
      }
    }
  }  
  return(0);
}

static int
my_strpos(char *s, char c)
{
  char *t;

  return((t=strchr(s,c)) == NULL ? -1 : t - s);
}

static int 
is_local_display(void)
{
  static int is_local = -1; /* display status */
  char *dpyname; /* display name */
  int n; /* position of colon char */
  char hostname[MAXHOSTNAMELEN]; /* host name */

  if (is_local == -1)
  {
    is_local =
      (((dpyname = getenv("DISPLAY")) != NULL) &&  /* name not null */
       ((n = my_strpos(dpyname, ':')) != -1) && /* name has a colon */
       ((n == 0) || /* name is :n */
        (strncmp(dpyname, "unix", n) == 0) || /* name is unix:n */
        (strncmp(dpyname, "localhost", n) == 0) || /* name is localhost:n */
        ((gethostname(hostname, MAXHOSTNAMELEN) == 0) && /* hostname ok */
         (strncmp(dpyname, hostname, n) == 0)))); /* name == hostname */
  }
   
  return(is_local);
}

void
AixEnableHftRing(int enable)
{
  if (is_local_display())
  {
    scan_hft_ring(enable ? SMUNHIDE : SMHIDE);
  }
}
