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
/* $XConsortium: iso8601.h /main/1 1996/04/21 19:23:30 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef ISO8601_H
#define ISO8601_H

/*
 * iso8601.h
 * 
 * header file for functions to convert between tick and
 * ISO 8601 times, ranges and durations.
 */

/* extern functions */
extern int	_csa_iso8601_to_tick(char *, time_t *);
extern int	_csa_tick_to_iso8601(time_t, char *);
extern int	_csa_iso8601_to_range(char *, time_t *, time_t *);
extern int	_csa_range_to_iso8601(time_t, time_t, char *);
extern int	_csa_iso8601_to_duration(char *, time_t *);
extern int	_csa_duration_to_iso8601(time_t, char *);

#endif /* ISO8601_H */
