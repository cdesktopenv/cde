/*
 * $XConsortium: HashTbl.hh /main/3 1996/06/11 16:19:03 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */
// NOTE: To be renamed to HashTable upon removal of other

class HashBucket;

class HashTbl : public FolioObject
{
public: // functions
  HashTbl (u_int num_buckets = 256);
  ~HashTbl();

  void add (Hashable &);
  void add (Hashable *h)
    { add (*h); }
  void remove (Hashable &);
  void remove_all (bool delete_elements = FALSE);
  int find (Hashable &) const;

protected: // variables
  HashBucket **f_hash_bucket;
  u_int        f_num_buckets;
};
