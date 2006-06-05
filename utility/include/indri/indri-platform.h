/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

/**
 * indri-platform.h
 *
 * The point of this header is to contain the kinds of things
 * that are stored in platform.h on Unix:
 *    * endian constants
 *    * type size constants
 *    * typedefs
 *
 * As this is a Indri header, it is also appropriate to put general
 * Indri-specific typedefs here.
 */

#ifndef INDRI_PLATFORM_H
#define INDRI_PLATFORM_H

#include "lemur-platform.h"

#ifdef __APPLE__
#include <tcpd.h>
#endif /* __APPLE__ */

#define INDRID_PORT ( 16743 )
#define INDRI_DISTRIBUTION "Indri development release 2.3"

#endif /* INDRI_PLATFORM_H */

