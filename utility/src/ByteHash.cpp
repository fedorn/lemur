/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#include <ByteHash.hpp>
namespace lemur 
{
  namespace utility
  {
    
    unsigned int randtab[65] = {
      0x3E7D744F, 0x00FB64D2, 0x54607916, 0x4C4376B0, 0x22F32509,
      0x2F4375DF, 0x0E4EAA75, 0x484BBE93, 0x5590EC32, 0x5488E93F,
      0x1EA2920D, 0x683FBDC9, 0x189FAE9F, 0x1D69FB15, 0x337150D5,
      0x348B664D, 0x1686AD76, 0x274F95E3, 0x4560CEFA, 0x4784D608,
      0x040C3119, 0x5557A624, 0x352B9D8F, 0x57A675BB, 0x3A8B24EB,
      0x2E52202B, 0x26B7E13A, 0x40DE49A3, 0x43E9D23F, 0x2095066A,
      0x4367090B, 0x59AD218B, 0x1DAC4FB6, 0x4E9EC3AB, 0x6EEF8BE9,
      0x112D2940, 0x45ED4CD3, 0x187BCE76, 0x2B8E8028, 0x0E2D5433,
      0x48961094, 0x2B1D48F7, 0x16529860, 0x6BA326B5, 0x5E7C6EFC,
      0x0F5CAFBC, 0x2340A5F0, 0x29313A19, 0x42D76CA5, 0x3AA68588,
      0x45467CC2, 0x1D0D91EB, 0x27DDED28, 0x44A0FC2A, 0x17A87A7D,
      0x46470FA6, 0x36E4F8D7, 0x0AF0DDF3, 0x3493ED44, 0x4361F473,
      0x5B3F7BA8, 0x1D6CEB8E, 0x5AF7C862, 0x6365C619, 0x3E7D744F
    };

    unsigned int ByteHash(const char *p, const int len, const int seed)
    {
      register int i=0;
      register int leftover = len % 7;
      register unsigned int *randp = &randtab[seed & 31];
      register const unsigned int *endrandp = &randtab[63];
      register unsigned int reg1, reg2, accum;

      accum = 0;

      while (i<leftover)
        {
          accum <<= 2;
          accum += p[i++] * (*randp++);
          if (randp >= endrandp)
            randp = randtab;
        }
      while (i<len)
        {
          // put 3.5 bytes into reg1 and 3.5 bytes in reg2
          reg1 = (p[i] << 20) + (p[i+1] << 12) + (p[i+2] << 4) + (p[i+3] >> 4) + (*randp++);
          reg2 = ((p[i+3] & 15) << 24) + (p[i+4] << 16) + (p[i+5] << 8) + p[i+6] + (*randp++);
          i += 7;
          accum += (reg1 * reg2) & 0x8FFFFFFF;
          // advance random table pointer
          if (randp >= endrandp)
            randp = randtab;
        }
      return accum;
    }
  }
}
