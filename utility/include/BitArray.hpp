#ifndef _BITARRAYH_
#define _BITARRAYH_

// On the Sun, the storage order of bits is as follows:
//   imagine a word represented as a bit array stretching from 
//   0 on the left to 31 on the right. If "bits" is a BITS pointer
//   then bits[0] -> position 31 (end of 1st word)
//        bits[1] -> position 30, etc.
//        bits[32] -> position 63 (end of second word)
//

#include <iostream.h>
#include <assert.h>
#include <stdlib.h>
#include <memory.h>		// for memset
#include "ByteHash.hpp"
#include "MemList.hpp"

typedef unsigned int BITS;
typedef unsigned int BITNUM;

// MACHINE DEPENDENT VARIABLES
const short BitsPerByte = 8; 
const short BytesPerWord = 4;  
const short BitsPerWord = BitsPerByte * BytesPerWord;
const short BitsPerWordMask = BitsPerWord - 1;
const short LogBitsPerByte = 3;  // for doing shift operations
const short LogBitsPerWord = 5;  // for doing shift operations

extern BITS notSingleBitMask[BitsPerWord];
extern BITS singleBitMask[BitsPerWord];
extern BITS reverseBitMask[BitsPerWord];
extern BITS bitMask[BitsPerWord+1];
extern BITS notBitMask[BitsPerWord+1];


inline int BitsToWords(BITNUM numbits)
{ return numbits ? (numbits-1)/BitsPerWord + 1 : 0; }

inline int BitsToBytes(BITNUM numbits)
{ return BitsToWords(numbits)*BytesPerWord; }

inline BITS *CopyBits(BITS *dest, const BITS *source, BITNUM numbits)
{
  memcpy(dest, source, BitsToBytes(numbits));
  return dest;
}

inline short GetBit(const BITS *pBit, BITNUM idx)
{
  return (pBit[idx >> LogBitsPerWord] >> (idx & BitsPerWordMask)) & 1;
}

// NOTE: return value must fit in a word!
inline BITS GetBits(const BITS *pBit, BITNUM from, BITNUM numbits)
{
  assert(numbits <= (int) BitsPerWord);
  const BITS *pWord = &pBit[from >> LogBitsPerWord]; // pointer to start int
  BITNUM unused = from &  BitsPerWordMask; // number of bits not used from start int
  BITNUM used = BitsPerWord - unused;
  // shift off unused bits and mask off useful bits
  BITS result = ((*pWord++) >> unused) & bitMask[used];
  int rest = numbits - used;
  if (rest <= 0)	// if we've masked off too many bits
    result &= bitMask[numbits]; // mask off correct amount
  else
    result |= (*pWord & bitMask[rest]) << used;
  return result;
}

inline void ClearBit(BITS *pBit, BITNUM idx)
{ pBit[idx >> LogBitsPerWord] &= notSingleBitMask[idx & BitsPerWordMask]; }

inline void ClearBits(BITS *pBit, BITNUM numbits)
{ memset((char *)pBit, 0x00, BitsToBytes(numbits)); }

inline void SetBit(BITS *pBit, BITNUM idx)
{ pBit[idx >> LogBitsPerWord] |= singleBitMask[idx & BitsPerWordMask]; }

inline void SetBits(BITS *pBit, BITNUM numbits)
{ memset((char *)pBit, 0xFF, BitsToBytes(numbits)); }

inline BITS *SetBits(BITS *pBit, BITNUM from, BITNUM numbits, BITS value)
{
  BITS *pWord = &pBit[from >> LogBitsPerWord]; // pointer to start int
  BITNUM unset = from & BitsPerWordMask;
  BITNUM set = BitsPerWord - unset;
  int overlap = (numbits - set);
  if (overlap > 0)
    {
      *pWord = (*pWord & bitMask[unset]) | (value << unset);
      pWord++;
      *pWord = ((*pWord & notBitMask[overlap])|
		((value >> set) & bitMask[overlap]));
    }
  else
    if (overlap < 0)
      *pWord = ((*pWord & bitMask[unset]) |
		(*pWord & (bitMask[-overlap] << (unset + numbits))) |
		((value & bitMask[numbits]) << unset));
    else
      *pWord = ((*pWord & bitMask[unset]) |
		((value & bitMask[numbits]) << unset));
  return pBit;
}
  
// Number of bits set 
inline int NumSet(BITS *pBit, BITNUM numBits)
{
  int count = 0;
  int numWords = BitsToWords(numBits);

  for (int i=0; i<numWords; i++)
  {
    int bitsTested = 0;
    for (BITS bits = pBit[i]; (bits!=0) && (numBits>0); numBits--)
    {
      count += bits % 2;
      bits = bits >> 1;
      bitsTested++;
    }
    numBits -= (BitsPerWord - bitsTested);
  }

  return count;
}

class BitArray {
public:
  // Constructors
  BitArray(BITNUM sz=1);
  BitArray(const BitArray &ba);
  BitArray(const char *pBitString);
  BitArray(BITS *words, int numbits)
    : pBit(words), allocated(0), size(numbits) {}
  ~BitArray() { if (allocated) delete [] pBit; }

  operator const BITS *() const { return pBit; }
  
  BitArray &operator=(const BitArray &ba);
  BitArray &operator=(const char *pBitString);

  // For Set<BitArray>
  int operator==(const BitArray &ba) const
    { return (size == ba.size) && (memcmp(pBit, ba.pBit, Bytes()) == 0); }
  unsigned int Hash() const { return ByteHash((char *)pBit, Bytes(), 0); }

  // Append operator
  BitArray &operator<<(const BitArray &ba);
  const BitArray &operator>>(BitArray &ba) const { ba << *this; return *this;}

  // I/O operators
  void Read(istream& is);    // binary I/O (size not written)
  void Write(ostream& os) const;
  friend inline ostream &operator<<(ostream &os, const BitArray &ba);
  friend inline istream &operator>>(istream &is, const BitArray &ba);

  // BitArray maintenance functions
  void Grow(BITNUM newsize,int copy=1);
  unsigned int Size() const { return size; }
  unsigned int Words() const { return BitsToWords(size); }
  unsigned int Bytes() const { return BitsToWords(size)*sizeof(int); }

  // Bit accessor functions
  short Get(BITNUM from) const;
  //    { assert(from < size); return GetBit(pBit, from); }

  // Note that return value must fit in a word !
  BITS Get(BITNUM from, BITNUM numbits) const
    {
      assert(from < size);  
      assert(numbits <= (int) BitsPerWord);
      if (numbits+from > size) numbits = size - from;
      return GetBits(pBit, from, numbits);
    }

  void Clear() { ::ClearBits(pBit, Bytes()<<LogBitsPerByte); }
  void Clear(BITNUM from) { assert(from < size); ClearBit(pBit, from); }

  void Sett()   { ::SetBits(pBit, size); }
  void Sett(BITNUM from) { assert(from < size);  SetBit(pBit, from); }
  void Sett(BITNUM from, BITNUM numbits, BITS value)
    {
      assert(from < size); // from >= 0 because it's unsigned
      if (numbits+from > size) Grow(numbits+from);
      SetBits(pBit, from, numbits, value);
    }

  short IsIntersectionEmpty(const BitArray &ba) const;
  void Print(ostream &os) const;

private:
  BITS *pBit;
  unsigned int allocated;
  BITNUM size;
};	     

inline ostream &operator<<(ostream &os, const BitArray &ba)
{ os.write((char *)ba.pBit, BitsToBytes(ba.size)); return os; }

inline istream &operator>>(istream &is, const BitArray &ba)
{ is.read((char *)ba.pBit, BitsToBytes(ba.size)); return is; }

#endif
