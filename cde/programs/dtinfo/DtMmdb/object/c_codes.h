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
/*
 * $XConsortium: c_codes.h /main/3 1996/06/11 17:23:29 cde-hal $
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


#ifndef _class_h
#define _class_h 1

// class code for system classes

#define ROOT_CODE		2 			

#define PRIMITIVE_CODE 		3
#define INTEGER_CODE 		4
#define FLOAT_CODE  		5
#define STRING_CODE 		6
#define OID_CODE 		7
#define LONG_STRING_CODE	8
#define OID_T_CODE 		9
#define DL_LIST_CELL_CODE	10
#define COMPRESSED_STRING_CODE	11

#define COMPOSITE_CODE 		101
#define TUPLE_CODE 		102
#define SET_CODE 		103
#define STATIC_SET_CODE		104
#define SHORT_LIST_CODE 	105
#define OID_LIST_CODE		106
#define STATIC_RECORDS_CODE	107
#define DL_LIST_CODE 		108

#define HASH_CODE 		201
#define STATIC_HASH_CODE 	202
#define DYN_HASH_CODE		203
#define FAST_MPHF_CODE 		204
#define GRAPH_OPMPHF_CODE	206
#define INV_LISTS_CODE		207
#define INDEX_CODE		208
#define MPHF_INDEX_CODE		209
#define IDMAP_MPHF_INDEX_CODE	210
#define DYN_INDEX_CODE		211
#define DYN_MEMORY_INDEX_CODE	212
#define DYN_DISK_INDEX_CODE	213

#define DYN_MEMORY_HASH_CODE	214
#define DYN_DISK_HASH_CODE	215

#define BTREE_CODE		216
#define BTREE_INDEX_CODE	217

#define ABS_STORAGE_CODE	301
#define UNIX_STORAGE_CODE	302
#define PAGE_STORAGE_CODE	303
#define MEM_STORAGE_CODE	304

#define NODE_CODE		401
#define ARC_CODE		402
#define LABELNODE_CODE		403
#define NODE_NODES_CODE		404

#define HUFFMAN_AGENT_CODE	501
#define GZIP_AGENT_CODE		502
#define DICT_AGENT_CODE		503

#endif
