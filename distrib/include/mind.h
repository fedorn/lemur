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

/// FOR USE WITH GSOAP 

typedef char * xsd__string;
typedef bool xsd__boolean;
typedef int xsd__int;
typedef double xsd__double;

int mindcall__getInternalStatus(xsd__string * return_);

class PredicateArray {
  int __size;
  xsd__string * predicate;
};

class mindtype__SchemaAttribute {
  xsd__string name;
  xsd__string datatype;
  xsd__string domain;
  xsd__boolean metadata;
  xsd__boolean sample;
  xsd__boolean surrogate;
  PredicateArray predicates;
  xsd__string mediatype;
  xsd__string description;
};

class SchemaAttributeArray {
  int __size;
  mindtype__SchemaAttribute ** schemaAttribute;
};

class mindtype__Schema {

  xsd__string name;
  SchemaAttributeArray * attributes;
};

class mindtype__Query {
};

class mindtype__CostParameters {
};

class mindtype__Content {
  xsd__string type;
  xsd__string value;
};

class mindtype__PropQueryCondition {
  xsd__string type;
  xsd__double weight;
  xsd__string attributename;
  xsd__string predicate;
  mindtype__Content content;
  xsd__string mapping;
};

class PropQueryConditionArray {
  int __size;
  mindtype__PropQueryCondition * propCondition;
};

class mindtype__PropQuery {
  xsd__string id;
  mindtype__Schema * schema;
  mindtype__Query * userQuery;
  xsd__int numDocs;
  xsd__string resulttype;
  mindtype__CostParameters costParameters;
  PropQueryConditionArray propConditions;
};

class mindtype__SchemaArray {
  mindtype__Schema * __ptr;
  int __size;
};

class mindtype__DocumentID {
  xsd__string proxyID;
  xsd__string internalID;
};

class mindtype__PropDocumentAttribute {
  xsd__string * attributename;
  mindtype__SchemaAttribute * attribute;
  mindtype__Content content;
};

class mindtype__PropDocumentAttributeArray {
  mindtype__PropDocumentAttribute * __ptr;
  int __size;
};

class mindtype__PropDocument {
  mindtype__DocumentID documentID;
  xsd__string * schemaname;
  mindtype__Schema * schema;
  mindtype__PropDocumentAttributeArray attributes;
  xsd__double weight;
  xsd__double originalWeight;
};

class mindtype__PropDocumentArray {
  mindtype__PropDocument * __ptr;
  int __size;
};

class mindtype__PropResult {
  mindtype__SchemaArray schemas;
  xsd__string resulttype;
  mindtype__PropDocumentArray propDocuments;
};

int mindcall__retrieveURL(xsd__string service, xsd__string callerService, xsd__string * return_);

class mindcall__queryResponse {
  mindtype__PropResult * mindtype__return;  
};

int mindcall__query(mindtype__PropQuery propQuery, xsd__int numDocs, 
		    mindcall__queryResponse * result);

class mindtype__FeatureList {
  xsd__string * __ptr;
  int __size;
};

class mindcall__extractQBSFeaturesResponse {
  mindtype__FeatureList * mindtype__return;
};

int mindcall__extractQBSFeatures(mindtype__PropDocumentAttribute propDocumentAttribute,
				 mindcall__extractQBSFeaturesResponse * result);

class mindcall__getSchemaResponse {
  mindtype__Schema * mindtype__return;
};

int mindcall__getSchema(mindcall__getSchemaResponse * result);

class mindcall__setCollectionNumDocsResponse {
};

int mindcall__setCollectionNumDocs(xsd__int nums, 
				   mindcall__setCollectionNumDocsResponse * result);

class mindcall__clearDocumentsResponse {
};

int mindcall__clearDocuments(mindcall__clearDocumentsResponse * result);

class mindcall__addDocumentResponse {
};

int mindcall__addDocument(mindtype__PropDocument propDocument, 
			  mindcall__addDocumentResponse * result);

class mindcall__constructResourceDescriptionResponse {
};

int mindcall__constructResourceDescription(mindcall__constructResourceDescriptionResponse * result);

