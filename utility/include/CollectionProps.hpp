 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef _COLLECTIONPROPS_HPP
#define _COLLECTIONPROPS_HPP

//! this Abstract Class for set of Collection Properties

/*! this is a supporting class for classes that want to pass one object
around that would have access methods for properties that a collectiont might
have.  this class should be subclassed, adding access methods for the particular information needs of a particular index.

See BasicCollectionProps for example

*/

/*
 * NAME DATE - COMMENTS
 * tnt 03/01 - created
 ==========================================================================*/

class CollectionProps {
public:
  virtual ~CollectionProps() {};

};

#endif
