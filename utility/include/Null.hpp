/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#ifndef _NULLH_
#define _NULLH_

#ifndef NULL
#define NULL ((void *)0)
#endif

inline void *NULLADDR() { return ((void *)0); }

#define nonNull(x) (&x != NULLADDR())
#define Null(x) (&(x) == NULLADDR())
#define null(Type) (*(Type *)NULLADDR())

#endif
