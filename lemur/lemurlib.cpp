/*==========================================================================
 * Copyright (c) 2006 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/
/*
	Force aggregation of the component libraries into a unified lemur.lib.
*/
#ifdef WIN32
#pragma comment (lib, "utility.lib")
#pragma comment (lib, "parsing.lib")
#pragma comment (lib, "index.lib")
#pragma comment (lib, "retrieval.lib") 
#pragma comment (lib, "langmod.lib")
#pragma comment (lib, "cluster.lib")
#pragma comment (lib, "summarization.lib")
#pragma comment (lib, "distrib.lib")
#endif