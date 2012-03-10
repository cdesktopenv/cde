#
# $XConsortium: mmdb.infolib.spec /main/3 1996/07/18 14:48:22 drk $
# 
# Copyright (c) 1993 HAL Computer Systems International, Ltd.
# All rights reserved.  Unpublished -- rights reserved under
# the Copyright Laws of the United States.  USE OF A COPYRIGHT
# NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
# OR DISCLOSURE.
# 
# THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
# SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
# DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
# PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
# INTERNATIONAL, LTD.
# 
#                         RESTRICTED RIGHTS LEGEND
# Use, duplication, or disclosure by the Government is subject
# to the restrictions as set forth in subparagraph (c)(l)(ii)
# of the Rights in Technical Data and Computer Software clause
# at DFARS 252.227-7013.
#
#          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
#                  1315 Dell Avenue
#                  Campbell, CA  95008
# 
# 

store	page_store:
	page_sz=8192:
	cached_pages=50:
	nm=$.dbd:
	mode=r:
	byte_order=big_endian

store	page_store:
	page_sz=8192:
	cached_pages=50:
	nm=$.dbi:
	mode=r:
	byte_order=big_endian

inv	inv: nm=$.node.inv:store_nm=$.dbi 
inv	inv: nm=$.graphic.inv:store_nm=$.dbi 
inv	inv: nm=$.toc.inv:store_nm=$.dbi 
inv	inv: nm=$.loc.inv:store_nm=$.dbi 
inv	inv: nm=$.doc1.inv:store_nm=$.dbi 
inv	inv: nm=$.doc2.inv:store_nm=$.dbi 
inv	inv: nm=$.dlp.inv:store_nm=$.dbi 
inv	inv: nm=$.stylesheet.inv:store_nm=$.dbi 

compress	huffman: nm=$.sgml.huffman:store_nm=$.dbd 
compress	huffman: nm=$.ps.huffman:store_nm=$.dbd 
compress	dict: nm=$.sgml.dict:store_nm=$.dbd 
compress	dict: nm=$.ps.dict:store_nm=$.dbd 

index_agent     mphf: nm=$.node.agent: store_nm=$.dbi 
index_agent     mphf: nm=$.graphic.agent: store_nm=$.dbi 
index_agent     mphf: nm=$.toc.agent: store_nm=$.dbi 
index_agent     mphf: nm=$.loc.agent: store_nm=$.dbi 
index_agent     mphf: nm=$.doc1.agent: store_nm=$.dbi 
index_agent     mphf: nm=$.doc2.agent: store_nm=$.dbi 
index_agent     mphf: nm=$.dlp.agent: store_nm=$.dbi 
index_agent     mphf: nm=$.stylesheet.agent: store_nm=$.dbi 

index	mphf_index: nm = $.node_index: store_nm=$.dbi: position=1: inv_nm=$.node.inv: agent_nm=$.node.agent 
index	mphf_index: nm = $.graphic_index: store_nm=$.dbi: position=1: inv_nm=$.graphic.inv: agent_nm=$.graphic.agent 
index	mphf_index: nm = $.toc_index: store_nm=$.dbi: position=1: inv_nm=$.toc.inv: agent_nm=$.toc.agent 
index	mphf_index: nm = $.loc_index: store_nm=$.dbi: position=1: inv_nm=$.loc.inv: agent_nm=$.loc.agent 
index	mphf_index: nm = $.doc1_index: store_nm=$.dbi: position=1: inv_nm=$.doc1.inv: agent_nm=$.doc1.agent 
index	mphf_index: nm = $.doc2_index: store_nm=$.dbi: position=4: inv_nm=$.doc2.inv: agent_nm=$.doc2.agent 
index	mphf_index: nm = $.dlp_index: store_nm=$.dbi: position=0: inv_nm=$.dlp.inv: agent_nm=$.dlp.agent 
index	mphf_index: nm = $.stylesheet_index: store_nm=$.dbi: position=1: inv_nm=$.stylesheet.inv: agent_nm=$.stylesheet.agent 

container	set: nm=$.node: store_nm=$.dbd: index_nm=$.node_index 
container	set: nm=$.graphic: store_nm=$.dbd: index_nm=$.graphic_index 
container	set: nm=$.toc: store_nm=$.dbd: index_nm=$.toc_index 
container	set: nm=$.loc: store_nm=$.dbd: index_nm=$.loc_index 
container	set: nm=$.doc: store_nm=$.dbd: index_nm=$.doc1_index: index_nm=$.doc2_index 
container	set: nm=$.stylesheet: store_nm=$.dbd: index_nm=$.stylesheet_index 
container	list: nm=$.dlp: store_nm=$.dbd: index_nm=$.dlp_index 
container	set: nm=$.stylesheet: store_nm=$.dbd: index_nm=$.stylesheet_index 

