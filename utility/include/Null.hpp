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
