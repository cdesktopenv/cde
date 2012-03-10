
# mmdb
# mark base spec template.
#

store	page_store:
	page_sz=1024:
	cached_pages=50:
	nm=$.dbd:
	mode=r:
	byte_order=big_endian

store	page_store:
	page_sz=1024:
	cached_pages=50:
	nm=$.dbi:
	mode=r:
	byte_order=big_endian

store	page_store:
	page_sz=1024:
	cached_pages=50:
	nm=$.dbh:
	mode=r:
	byte_order=big_endian

inv inv: nm=$.mark.inv:store_nm=$.dbi

index smphf_index: nm = $.mark_index: store_nm=$.dbi: position=1: inv_nm=$.mark.inv: agent_nm=$.dbh

container set: nm=$.mark: store_nm=$.dbd: index_nm=$.mark_index

