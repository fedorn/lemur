 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "BitArray.hpp"

short BitArray::Get(BITNUM from) const {
    if (from >= size) {
      assert(0);
    }
    return GetBit(pBit, from); 
}

short BitArray::IsIntersectionEmpty(const BitArray &ba) const
{
  int numWords = (Size() < ba.Size()) ? Words() : ba.Words();
  int bit = 0;
  for (int word = 0; word<numWords-1; word++, bit+=BitsPerWord)
    if ((pBit[word] != 0) || (ba.pBit[word] != 0))
      for (int i=0; i<BitsPerWord; i++)
	if (Get(bit+i) && ba.Get(bit+i))
	  return 1;
  for (;(unsigned int) bit < size; bit++)
    if (Get(bit) && ba.Get(bit))
      return 1;
  return 0;
}

void BitArray::Print(ostream &os) const
{
  for (BITNUM b=0; b<size; ++b) 
    os << Get(b) ? '1' : '0';
  os << endl;
}

BitArray::BitArray(BITNUM sz) 
  : pBit(new BITS[BitsToWords(sz)])
{
  size = sz;
  allocated = BitsToWords(size);
  Clear();
}

BitArray::BitArray(const BitArray &ba) 
  : pBit(new BITS[BitsToWords(ba.size)])
{
  size = ba.size;
  allocated = BitsToWords(size);
  CopyBits(pBit, ba.pBit, size);
}

BitArray::BitArray(const char *pBitString) 
  : pBit((BITS *)NULL), allocated(0), size(0)
{
  (*this) = pBitString;
}

BitArray &BitArray::operator=(const BitArray &ba)
{
  Grow(ba.size);
  allocated = BitsToWords(size);
  CopyBits(pBit, ba.pBit, size);
  return *this;
}

BitArray &BitArray::operator=(const char *pBitString)
{
  if (allocated)
    delete [] pBit;
  size = strlen(pBitString);
  allocated = BitsToWords(size);
  pBit = new BITS[allocated];
  for (int i=0; (unsigned int) i<size; i++)
    if (pBitString[i] == '0')
      Clear(i);
    else if (pBitString[i] == '1')
      Sett(i);
    else
      {
	cerr << "ERROR: Bad bit " << pBitString[i] << " in bitstring "
	     << pBitString << endl;
	Clear(i);
      }
	return *this;
}

void BitArray::Grow(BITNUM newsize, int copy)
{
  unsigned int newalloc = BitsToWords(newsize);
  if (newalloc > allocated)
    {
      unsigned int oldalloc = allocated;
      unsigned int oldsize = size;
      BITS *old = pBit;
      pBit = new BITS[newalloc];
      allocated = newalloc;
      size = newsize;	      // so subsequent Clear clears all new bits
      Clear();
      if (copy) CopyBits(pBit, old, oldsize);
      if (oldalloc) delete [] old;
    }
  else
  {
    size = newsize;
  }
}

BitArray &BitArray::operator<<(const BitArray &ba)
{
  if (ba.size == 0)
    return *this;
  BITNUM start = size;
  Grow(size+ba.size);
  BITNUM alignedEnd = (BitsToWords(ba.size)-1) * BitsPerWord;
  BITNUM bn = 0;
  for (bn=0; bn<alignedEnd; bn += BitsPerWord)
    Sett(start+bn, BitsPerWord, ba.Get(bn, BitsPerWord));
  Sett(start+bn, (ba.size-bn), ba.Get(bn,(ba.size-bn)));
  return *this;
}

void BitArray::Read(istream & is)
{
  is.read(pBit, BitsToBytes(size));
}

void BitArray::Write(ostream & os) const
{
  os.write(pBit, BitsToBytes(size));
}

// Bitmasks for bit operations
BITS notSingleBitMask[BitsPerWord] =
{
  ~0x1, ~0x2, ~0x4, ~0x8,
  ~0x10, ~0x20, ~0x40, ~0x80,
  ~0x100, ~0x200, ~0x400, ~0x800,
  ~0x1000, ~0x2000, ~0x4000, ~0x8000,
  ~0x10000, ~0x20000, ~0x40000, ~0x80000,
  ~0x100000, ~0x200000, ~0x400000, ~0x800000,
  ~0x1000000, ~0x2000000, ~0x4000000, ~0x8000000,
  ~0x10000000, ~0x20000000, ~0x40000000, ~0x80000000
  };

BITS singleBitMask[BitsPerWord] =
{
  0x1, 0x2, 0x4, 0x8,
  0x10, 0x20, 0x40, 0x80,
  0x100, 0x200, 0x400, 0x800,
  0x1000, 0x2000, 0x4000, 0x8000,
  0x10000, 0x20000, 0x40000, 0x80000,
  0x100000, 0x200000, 0x400000, 0x800000,
  0x1000000, 0x2000000, 0x4000000, 0x8000000,
  0x10000000, 0x20000000, 0x40000000, 0x80000000
  };

BITS reverseBitMask[BitsPerWord] =
{
  0xFFFFFFFF, 0x7FFFFFFF, 0x3FFFFFFF, 0x1FFFFFFF,
  0xFFFFFFF,  0x7FFFFFF,  0x3FFFFFF, 0x1FFFFFF,
  0xFFFFFF,   0x7FFFFF,   0x3FFFFF, 0x1FFFFF,
  0xFFFFF,    0x7FFFF,    0x3FFFF, 0x1FFFF,
  0xFFFF,     0x7FFF,     0x3FFF, 0x1FFF,
  0xFFF,      0x7FF,      0x3FF, 0x1FF,
  0xFF,       0x7F,       0x3F, 0x1F,
  0xF,        0x7,        0x3, 0x1,
  };

BITS bitMask[BitsPerWord+1] = {
  0,0x1, 0x3, 0x7, 0xF,
  0x1F, 0x3F, 0x7F, 0xFF,
  0x1FF, 0x3FF, 0x7FF, 0xFFF,
  0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF,
  0x1FFFF, 0x3FFFF, 0x7FFFF, 0xFFFFF,
  0x1FFFFF, 0x3FFFFF, 0x7FFFFF, 0xFFFFFF,
  0x1FFFFFF, 0x3FFFFFF, 0x7FFFFFF, 0xFFFFFFF,
  0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
  };

BITS notBitMask[BitsPerWord+1] = {
  ~0x0,
  ~0x1, ~0x3, ~0x7, ~0xF,
  ~0x1F, ~0x3F, ~0x7F, ~0xFF,
  ~0x1FF, ~0x3FF, ~0x7FF, ~0xFFF,
  ~0x1FFF, ~0x3FFF, ~0x7FFF, ~0xFFFF,
  ~0x1FFFF, ~0x3FFFF, ~0x7FFFF, ~0xFFFFF,
  ~0x1FFFFF, ~0x3FFFFF, ~0x7FFFFF, ~0xFFFFFF,
  ~0x1FFFFFF, ~0x3FFFFFF, ~0x7FFFFFF, ~0xFFFFFFF,
  ~0x1FFFFFFF, ~0x3FFFFFFF, ~0x7FFFFFFF, ~0xFFFFFFFF
  };
