// $TOG: cc_hdict.C /main/5 1998/04/17 11:45:00 mgreess $

#include "dti_cc/cc_exceptions.h"

#if !defined(__osf__)
template <class K, class V> CC_Boolean kv_pair<K, V>::f_needRemove = FALSE;
#endif

template <class K, class V>
kv_pair<K, V>::~kv_pair()
{
   if ( f_needRemove == TRUE ) {
      delete f_key;
      delete f_value;
   }
}

#ifdef DEBUG
template <class K, class V>
ostream& kv_pair<K, V>::print(ostream& out)
{
   return out << *f_key << " " << *f_value << "\n";
}
#endif

template <class K, class V>
unsigned int kv_pair<K, V>::operator==(const kv_pair<K, V>& kv)
{
   if ( f_key ==  0 || kv.f_key == 0 )
      throw(ccStringException("kv_pair::operator==(): null pointer(s)."));

   return ( *f_key == (*kv.f_key) ) ? 1 : 0;
}

///////////////////////////////////
//
///////////////////////////////////

template <class K, class V>
hashTable<K,V>::hashTable(const hashTable<K, V>& h) :
  f_hash_func_ptr(h.f_hash_func_ptr), f_buckets(h.f_buckets),
  f_items(h.f_items)
{
   cerr << "Warning: hashTable(const hashTable&) called" << endl;
   exit(-1);
}

template <class K, class V>
hashTable<K,V>::hashTable(unsigned (*f)(const K&), size_t init_bucket_num) :
  f_hash_func_ptr(f), f_buckets(init_bucket_num),
  f_items(0)
{
}

template <class K, class V>
hashTable<K,V>::~hashTable()
{
   CC_TPtrSlist<kv_pair<K, V> > * b = 0;
   kv_pair<K, V>  * r = 0;

   for (size_t i=0; i<f_buckets.length(); i++ ) {

      b  = f_buckets[i];

      if ( b ) {
         while ( (r = b -> removeFirst()) ) 
            delete r;

         delete b;
      }
   }
}

template <class K, class V>
void hashTable<K,V>::clearAndDestroy()
{
   kv_pair<K, V>::f_needRemove = TRUE;

   CC_TPtrSlist<kv_pair<K, V> >* b = 0;

   for (size_t i=0; i<f_buckets.length(); i++ ) {

     b = f_buckets[i];
     if ( b ) {
        b -> clearAndDestroy();
        delete b;
        f_buckets[i] = 0;
     }
   }

   f_items = 0;

   kv_pair<K, V>::f_needRemove = FALSE;
}

template <class K, class V>
CC_Boolean hashTable<K,V>::contains(const K* k) const
{
   if ( findValue(k) )
     return TRUE;
   else
     return FALSE;
   
}

template <class K, class V>
kv_pair<K, V>* hashTable<K,V>::_find(const K* k) const
{
   size_t i = (*f_hash_func_ptr)(*k) % f_buckets.length();

   CC_TPtrSlist<kv_pair<K, V> >* b = f_buckets[i];

   if ( b == 0 )
     return 0;

   kv_pair<K, V> key((K*)k);

   return b -> find(&key);
}

template <class K, class V>
V* hashTable<K,V>::findValue(const K* k) const
{
   kv_pair<K, V>* p = _find(k);
   if ( p )
     return p -> f_value;
   else
     return 0;
}

template <class K, class V>
K* hashTable<K,V>::findKeyAndValue(const K* k, V*& v) const
{
   kv_pair<K, V>* p = _find(k);
   if ( p ) {
     v = p -> f_value;
     return p -> f_key;
   } else
     return 0;
}

template <class K, class V>
void hashTable<K,V>::insertKeyAndValue(K* k, V* v)
{
   size_t i = (*f_hash_func_ptr)(*k) % f_buckets.length();

   CC_TPtrSlist<kv_pair<K, V> >* b = f_buckets[i];

   if ( b == 0 ) {
     f_buckets[i] = new CC_TPtrSlist<kv_pair<K, V> >;
   }

   kv_pair<K, V>* p = new kv_pair<K, V>(k, v);
   f_buckets[i] -> insert(p);

   f_items ++;
}

template <class K, class V>
K* hashTable<K,V>::remove(const K* k)
{
   size_t i = (*f_hash_func_ptr)(*k) % f_buckets.length();

   CC_TPtrSlist<kv_pair<K, V> >* b = f_buckets[i];

   if ( b == 0 )
      return 0;

   kv_pair<K, V> key((K*)k, 0);
   kv_pair<K, V>* result = b -> remove(&key);

   if ( result == 0 )
     return 0;
   
   K* kr = result -> f_key;

   delete result; 

   f_items --;

   return kr;
}

#ifdef DEBUG
template <class K, class V> 
ostream& hashTable<K,V>::print(ostream& out)
{
   CC_TPtrSlist<kv_pair<K, V> >* b = 0;

   for (int i=0; i<f_buckets.length(); i++ ) {

     b = f_buckets[i];
     if ( b ) {
        cerr << "bucket num = " << i << "\n";

        CC_TPtrSlistIterator<kv_pair<K, V> > next(*b);

        while (++next) {
           out << ' ' << *next.key() ;
        }
     }
   }

   return out;
}
#endif

////////////////////////////////////////
template <class K, class V> 
hashTableIterator<K,V>::hashTableIterator(hashTable<K, V>& b) :
   f_bucket_num(0), f_pos(0), f_rec(0), f_hashTable(b)
{
}

template <class K, class V> 
hashTableIterator<K,V>::~hashTableIterator()
{
}

template <class K, class V> 
CC_Boolean hashTableIterator<K,V>::_findNonEmptyBucket()
{
   CC_TPtrSlist<kv_pair<K, V> >* b = 0;

   for (; f_bucket_num<f_hashTable.f_buckets.length(); f_bucket_num++ ) {
      if ( (b=f_hashTable.f_buckets[f_bucket_num]) && b->entries() > 0 ) {
         f_pos = 0;
         return TRUE;
      }
   }

   return FALSE;
}

template <class K, class V> 
CC_Boolean hashTableIterator<K,V>::operator++()
{
   if ( f_rec == 0 ) { // first call to this op.
       if (  _findNonEmptyBucket() == FALSE )
         return FALSE;

   } else {
       if (  _findNextRecord() == FALSE ) {
          f_bucket_num++;
          if (  _findNonEmptyBucket() == FALSE )
             return FALSE;
       }
   }

//fprintf(stderr, "in operator++: f_bucket_num= %d, f_pos = %d\n", 
//f_bucket_num, f_pos);

   f_rec = f_hashTable.f_buckets[f_bucket_num] -> at(f_pos);
   return TRUE;
}

template <class K, class V> 
CC_Boolean hashTableIterator<K,V>::_findNextRecord()
{
   f_pos++;

//fprintf(stderr, "f_bucket_num= %d, f_pos = %d, entries() = %d\n", f_bucket_num, f_pos, f_hashTable.f_buckets[f_bucket_num] -> entries() );

   if ( f_hashTable.f_buckets[f_bucket_num] -> entries() <= f_pos )
      return FALSE;
   else
      return TRUE;
}

template <class K, class V> 
K* hashTableIterator<K,V>::key()
{
   return f_rec -> f_key;
}

template <class K, class V> 
V* hashTableIterator<K,V>::value() const
{
   return f_rec -> f_value;
}

