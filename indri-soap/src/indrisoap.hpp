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

//	Indri Web APIs


//gsoapopt i
//gsoapopt t
//gsoapopt e


//gsoap indrisoap service documentation: Provide indexing and querying of an indri Repository.
//gsoap indrisoap service name:	indrisoap
//gsoap indrisoap service style:	rpc
//gsoap indrisoap service encoding:	encoded
//gsoap indrisoap service location:	http://lemurproject.org:9696
//gsoap indrisoap service namespace:	http://lemurproject.org/indrisoap.wsdl
//gsoap indrisoap schema namespace:	urn:IndriSoap

// soap typedefs
typedef char *	xsd__string;

// C++ alternative for xsd:boolean:
typedef bool	xsd__boolean;

typedef int	xsd__int;

typedef double	xsd__double;

struct xsd__base64Binary
{	unsigned char *			__ptr;
	int				__size;
};

//gsoap indrisoap schema type-documentation: MetadataPair, a key value pair.
struct indrisoap__MetadataPair 
{
  std::string key;
  std::string value;
};

//gsoap indrisoap schema type-documentation: MetadataPairArray, array of Metadata pairs
struct MetadataPairArray 
{
  struct indrisoap__MetadataPair * __ptr;
  int __size;
};

//gsoap indrisoap schema type-documentation: StringArray, array of string
struct StringArray 
{
  std::string * __ptr;
  int __size;
};

//gsoap indrisoap schema type-documentation: intArray, array of int
struct intArray 
{
  xsd__int * __ptr;
  int __size;
};

//gsoap indrisoap schema type-documentation: QueryRequest contains the parameters for running a query.
struct indrisoap__QueryRequest {
  enum Options {
    HTMLSnippet = 1,
    TextSnippet = 2
  };

  std::string query;
  struct StringArray formulators;
  struct StringArray metadata;
  struct intArray docSet;
  int resultsRequested;
  int startNum;
  enum Options options;
};

//gsoap indrisoap schema type-documentation: Result of a query request.
struct indrisoap__Result 
{
  std::string snippet;
  std::string documentName;
  std::string cachedURL;  
  int docid;
  double score;
  int begin;
  int end;
  struct MetadataPairArray  metadata;
};

//gsoap indrisoap schema type-documentation: ResultArray, array of Result
struct ResultArray 
{
  struct indrisoap__Result * __ptr;
  int __size;
};

//gsoap indrisoap schema type-documentation: QueryResponse collection of query results
struct indrisoap__QueryResponse 
{

  float parseTime;
  float executeTime;
  float documentsTime;
  int estimatedMatches;
  struct ResultArray results;
};

//gsoap indrisoap schema type-documentation: doRunQueryResponse query response wrapper structure
struct indrisoap__doRunQueryResponse{
  struct indrisoap__QueryResponse _return;
};


//gsoap indrisoap service method-documentation: doRunQueryRequest returns the result of running a QueryRequest.
indrisoap__doRunQueryRequest
(	struct indrisoap__QueryRequest request,
	struct indrisoap__doRunQueryResponse &response
);

/* document vectors */
//gsoap indrisoap schema type-documentation: Field an indri field
struct indrisoap__Field {
  std::string name;
  int begin;
  int end;
  LONG64 number;
};

//gsoap indrisoap schema type-documentation: FieldArray an array of indri fields
struct FieldArray 
{
  struct indrisoap__Field *__ptr;
  int __size;
};
//gsoap indrisoap schema type-documentation: DocumentVector an indri document vector
struct indrisoap__DocumentVector 
{
  struct StringArray stems;
  struct intArray positions;
  struct FieldArray fields;
};

//gsoap indrisoap schema type-documentation: DocumentVectorArray an array of document vectors
struct DocumentVectorArray
{
  struct indrisoap__DocumentVector *__ptr;
  int __size;
};

//gsoap indrisoap service method-documentation: doDocumentVectors fetch
indrisoap__doDocumentVectors(struct intArray ids,
                             struct DocumentVectorArray &response);

//gsoap indrisoap service method-documentation: doAddDocument add
indrisoap__doAddDocument
( 
 xsd__string doc, 
 xsd__string fileClass,
 struct MetadataPairArray meta,
 struct indrisoap__doAddDocumentResponse {
   xsd__int docid;                      
 } &response
);

//gsoap indrisoap service method-documentation: doDeleteDocument delete
indrisoap__doDeleteDocument
( xsd__int doc,
  struct indrisoap__doDeleteDocumentResponse {
  } &response
);
