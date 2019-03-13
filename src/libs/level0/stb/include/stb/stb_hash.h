#pragma once
#ifndef WYRD_STB_HASH_H
#define WYRD_STB_HASH_H

#include "core/core.h"

#define STB_EXTERN EXTERN_C

//////////////////////////////////////////////////////////////////////////////
//
//                               Hashing
//
//      typical use for this is to make a power-of-two hash table.
//
//      let N = size of table (2^n)
//      let H = stb_hash(str)
//      let S = stb_rehash(H) | 1
//
//      then hash probe sequence P(i) for i=0..N-1
//         P(i) = (H + S*i) & (N-1)
//
//      the idea is that H has 32 bits of hash information, but the
//      table has only, say, 2^20 entries so only uses 20 of the bits.
//      then by rehashing the original H we get 2^12 different probe
//      sequences for a given initial probe location. (So it's optimal
//      for 64K tables and its optimality decreases past that.)
//
//      ok, so I've added something that generates _two separate_
//      32-bit hashes simultaneously which should scale better to
//      very large tables.


STB_EXTERN unsigned int stb_hash(char const *str);
STB_EXTERN unsigned int stb_hashptr(void const *p);
STB_EXTERN unsigned int stb_hashlen(char const *str, int len);
STB_EXTERN unsigned int stb_rehash_improved(unsigned int v);
STB_EXTERN unsigned int stb_hash_fast(void const *p, int len);
STB_EXTERN unsigned int stb_hash2(char const *str, unsigned int *hash2_ptr);
STB_EXTERN unsigned int stb_hash_number(unsigned int hash);

#define stb_rehash(x)  ((x) + ((x) >> 6) + ((x) >> 19))

#ifdef STB_DEFINE
unsigned int stb_hash(char const *str)
{
   unsigned int hash = 0;
   while (*str)
      hash = (hash << 7) + (hash >> 25) + *str++;
   return hash + (hash >> 16);
}

unsigned int stb_hashlen(char const *str, int len)
{
   unsigned int hash = 0;
   while (len-- > 0 && *str)
      hash = (hash << 7) + (hash >> 25) + *str++;
   return hash + (hash >> 16);
}

unsigned int stb_hashptr(void const *p)
{
    unsigned int x = (unsigned int)(size_t) p;

   // typically lacking in low bits and high bits
   x = stb_rehash(x);
   x += x << 16;

   // pearson's shuffle
   x ^= x << 3;
   x += x >> 5;
   x ^= x << 2;
   x += x >> 15;
   x ^= x << 10;
   return stb_rehash(x);
}

unsigned int stb_rehash_improved(unsigned int v)
{
   return stb_hashptr((void *)(size_t) v);
}

unsigned int stb_hash2(char const *str, unsigned int *hash2_ptr)
{
   unsigned int hash1 = 0x3141592c;
   unsigned int hash2 = 0x77f044ed;
   while (*str) {
      hash1 = (hash1 << 7) + (hash1 >> 25) + *str;
      hash2 = (hash2 << 11) + (hash2 >> 21) + *str;
      ++str;
   }
   *hash2_ptr = hash2 + (hash1 >> 16);
   return       hash1 + (hash2 >> 16);
}

// Paul Hsieh hash
#define stb__get16(p) ((p)[0] | ((p)[1] << 8))

unsigned int stb_hash_fast(void const *p, int len)
{
   unsigned char *q = (unsigned char *) p;
   unsigned int hash = len;

   if (len <= 0 || q == NULL) return 0;

   /* Main loop */
   for (;len > 3; len -= 4) {
      unsigned int val;
      hash +=  stb__get16(q);
      val   = (stb__get16(q+2) << 11);
      hash  = (hash << 16) ^ hash ^ val;
      q    += 4;
      hash += hash >> 11;
   }

   /* Handle end cases */
   switch (len) {
      case 3: hash += stb__get16(q);
              hash ^= hash << 16;
              hash ^= q[2] << 18;
              hash += hash >> 11;
              break;
      case 2: hash += stb__get16(q);
              hash ^= hash << 11;
              hash += hash >> 17;
              break;
      case 1: hash += q[0];
              hash ^= hash << 10;
              hash += hash >> 1;
              break;
      case 0: break;
   }

   /* Force "avalanching" of final 127 bits */
   hash ^= hash << 3;
   hash += hash >> 5;
   hash ^= hash << 4;
   hash += hash >> 17;
   hash ^= hash << 25;
   hash += hash >> 6;

   return hash;
}

unsigned int stb_hash_number(unsigned int hash)
{
   hash ^= hash << 3;
   hash += hash >> 5;
   hash ^= hash << 4;
   hash += hash >> 17;
   hash ^= hash << 25;
   hash += hash >> 6;
   return hash;
}

#endif

#endif //WYRD_STB_HASH_H
