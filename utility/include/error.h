#ifndef _ERRORH_
#define _ERRORH_

#include <stdarg.h>

void error(char *fmt, ...);
void claim(int ex, char * string);

#endif
