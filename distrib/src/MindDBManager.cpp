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

#include "MindDBManager.hpp"
#include "Exception.hpp"

struct Namespace namespaces[] =
{
  { "SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", 
    "http://schemas.xmlsoap.org/soap/envelope/"},
  { "SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", 
    "http://schemas.xmlsoap.org/soap/encoding/"},
  { "xsi", "http://www.w3.org/1999/XMLSchema-instance", 
    "http://www.w3.org/*/XMLSchema-instance" },
  { "xsd", "http://www.w3.org/1999/XMLSchema", 
    "http://www.w3.org/*/XMLSchema" },
  { "mindtype", "urn:MINDType", "urn:MINDType"},
  { "mindcall", "urn:Registry", "urn:*"},
  { NULL, NULL, NULL }
};


MindDBManager::MindDBManager() {
  soap = soap_new();
  soap_init(soap);
  soap->enable_null = true;
  srand(time(NULL));
}

MindDBManager::~MindDBManager() {
  soap_end(soap);
}

// Open the database.
void
MindDBManager::open(char * dbname) {
  MindParameter::get();
  registry = strdup(MindParameter::registry.c_str());
  proxyName = strdup(dbname);
  proxyName = strtok(proxyName, ";");
  proxy = strtok(NULL, ";");
  interfaceName = strtok(NULL, ";");
  constructionName = strtok(NULL, ";");
  char * numDBDocStr = strtok(NULL, ";");
  int numDBDocs = atoi(numDBDocStr);


  // look up URLs

  changeNamespace("mindcall", "urn:Registry", "urn:*");

  //  if (soap_call_mindcall__retrieveURL(soap, registry, NULL, proxyName,
  //				      "", &proxy) == SOAP_OK) {
  //    printf("Proxy %s: %s\n", proxyName, proxy);
  //  } else {
  //      soap_print_fault(soap, stderr);
  //      exit(-1);
  //    }

  if (soap_call_mindcall__retrieveURL(soap, registry, NULL, interfaceName,
				      "", &interface) == SOAP_OK) {
    printf("Interface %s: %s\n", interfaceName, interface);
  } else {
    soap_print_fault(soap, stderr);
    exit(-1);
  }

  if (soap_call_mindcall__retrieveURL(soap, registry, NULL, constructionName,
				      "", &construction) == SOAP_OK) {
    printf("Construction-text %s: %s\n", constructionName, construction);
  } else {
    soap_print_fault(soap, stderr);
    exit(-1);
  }


  // get schema

  changeNamespace("mindcall", interfaceName, "urn:*");
  
  mindcall__getSchemaResponse getSchemaResponse;
  if (soap_call_mindcall__getSchema(soap, interface, NULL,
				    &getSchemaResponse) == SOAP_OK) {

    schema = getSchemaResponse.mindtype__return;
    schemaName = strdup(schema->name);
    printf ("Schema name: %s\n", schemaName);

    // find the attribute to sample
    for (int j = 0; j < schema->attributes->__size; j++) {
      if (schema->attributes->schemaAttribute[j]->sample == true) {
        sampleAttribute = strdup(schema->attributes->schemaAttribute[j]->name);
        printf ("Sample on: %s\n", sampleAttribute);
      }
    }
  } else {
    soap_print_fault(soap, stderr);
    exit(-1);
  }
  

  // notify proxy to clear the resource description
  changeNamespace("mindcall", proxyName, "urn:*");
  
  mindcall__clearDocumentsResponse clearDocResp;
  if (soap_call_mindcall__clearDocuments(soap, proxy, NULL,
					 &clearDocResp) == SOAP_OK) {
  } else {
    soap_print_fault(soap, stderr);
    exit(-1);
  }  
  
  // notify proxy of number docs in collection
  mindcall__setCollectionNumDocsResponse setCollDocsResp;
  if (soap_call_mindcall__setCollectionNumDocs(soap, proxy, NULL, numDBDocs,
					       &setCollDocsResp) == SOAP_OK) {
  } else {
    soap_print_fault(soap, stderr);
    exit(-1);
  }  


  
}

// Return the parser.
MemParser *
MindDBManager::getParser() {
  return this;
}


// Query the database.
results_t *
MindDBManager::query(char * query, int numDocs) {
  
  cout << "Query: " << query << endl;


  results_t * results;

  const char * empty = "";
  const char * mandatory = "mandatory";
  const char * contains = "contains";
  const char * complete = "complete";
  const char * textPlain = "text/plain";

  // build the query
  mindtype__PropQuery qry;
  
  char id[7];
  sprintf(id, "%d", rand() % 1000000);
  qry.id = id;
  
  // clear set up schema mediatype
  qry.schema = new mindtype__Schema(*schema);
  for (int i = 0; i < qry.schema->attributes->__size; i++) {
    qry.schema->attributes->schemaAttribute[i]->mediatype = strdup(empty);
  }

  qry.userQuery = NULL;
  qry.numDocs = numDocs;
  qry.resulttype = (char *) complete;
  qry.propConditions.propCondition = new mindtype__PropQueryCondition[1];
  qry.propConditions.__size = 1;
  qry.propConditions.propCondition[0].type = (char *) mandatory;
  qry.propConditions.propCondition[0].weight = 1.0;
  qry.propConditions.propCondition[0].attributename = sampleAttribute;
  qry.propConditions.propCondition[0].predicate = (char *) contains;
  qry.propConditions.propCondition[0].content.type = (char *) textPlain;
  qry.propConditions.propCondition[0].content.value = query;
  qry.propConditions.propCondition[0].mapping = (char *) empty;

  
  // run the query
  changeNamespace("mindcall", interfaceName, "urn:*");  
  mindcall__queryResponse response;
  if (soap_call_mindcall__query(soap, (const char *) interface,
                                NULL, qry, numDocs,
                                &response) == SOAP_OK) {

    mindResult = response.mindtype__return;
    //    printf ("ResultType %s\n", mindResult->resulttype);
    //    printf ("Num schemas %d\n", mindResult->schemas.__size);
    //    printf ("Num documents %d\n", mindResult->propDocuments.__size);

  } else {
    soap_print_fault(soap, stderr);
    exit(-1);
  }
  
  // build the returned object
  int numReturned = mindResult->propDocuments.__size;
  results = new results_t;
  results->numDocs = numReturned;
  results->ids = new docid_t[numReturned];
  for (int i = 0; i < numReturned; i++) {
    results->ids[i] = 
      strdup(mindResult->propDocuments.__ptr[i].documentID.internalID);    
  }

  // free memory
  delete qry.schema;
  delete [] qry.propConditions.propCondition;

  return results;
}

void 
MindDBManager::parse(doc_t * doc) {

  // look up schema indexes

  int schemaInd;
  int sampleAttributeInd;

  int numSchemas = mindResult->schemas.__size;
    for (int l = 0; l < numSchemas; l++) {
    if (!strcmp(schemaName, mindResult->schemas.__ptr[l].name)) {
      schemaInd = l;
      break;
    }
  }

  int numSchemaAttributes = mindResult->schemas.
    __ptr[schemaInd].attributes->__size;

  for (int k=0; k < numSchemaAttributes; k++) {
    if (!strcmp(sampleAttribute, (const char *)
		mindResult->schemas.__ptr[schemaInd].
		attributes->schemaAttribute[k]->name)) {
      sampleAttributeInd = k;
      break;
    }
  }


  // lookup the document attribute to parse

  mindtype__PropDocumentAttribute * attribute;

  int numDocs = mindResult->propDocuments.__size;
  
  for (int docInd = 0; docInd < numDocs; docInd++) {

    // check for the right doc
    if (!strcmp(mindResult->propDocuments.__ptr[docInd].documentID.internalID,
		doc->id)) {

      int numAttributes = mindResult->propDocuments.__ptr[docInd].
	attributes.__size;
      
      for (int attributeInd=0; attributeInd < numAttributes; attributeInd++) {

	// check for the right attribute
	if (!strcmp(sampleAttribute, (const char *)
		    *(mindResult->propDocuments.__ptr[docInd].
		      attributes.__ptr[attributeInd].attributename))) {
	  
	  // copy the document attribute

	  attribute = new mindtype__PropDocumentAttribute();
	  
	  attribute->attribute 
	    = new mindtype__SchemaAttribute
	    (*(mindResult->schemas.__ptr[schemaInd].
	       attributes->schemaAttribute[sampleAttributeInd]));

	  attribute->attributename = NULL;
	  
	  attribute->content.type = 
	    mindResult->propDocuments.__ptr[docInd].
	    attributes.__ptr[attributeInd].content.type;

	  attribute->content.value = 
	    mindResult->propDocuments.__ptr[docInd].
	    attributes.__ptr[attributeInd].content.value;

	  break;
	}
      }
      break;
    }
  }
  
  // tell the proxy construction to parse the doc
  soap->enable_null = false;
  changeNamespace("mindcall", constructionName, "urn:*");

  mindcall__extractQBSFeaturesResponse featuresResponse;
  mindtype__FeatureList * list;
  if (soap_call_mindcall__extractQBSFeatures(soap, construction, NULL,
					     *attribute,
					     &featuresResponse) == SOAP_OK) {
      list = featuresResponse.mindtype__return;
  } else {
    soap_print_fault(soap, stderr);
    throw Exception("MindDBManager", 
		    "Proxy-construction-text could not parse document");
  }
  
  int numFeatures = list->__size;
  cout << "Doc id: " << doc->id << " (" << numFeatures << ")" << endl;

  // pass the info on
  if (textHandler != NULL) textHandler->foundDoc(doc->id);
  for (int i = 0; i < numFeatures; i++) {
    if (textHandler != NULL) textHandler->foundWord(list->__ptr[i]);
  }



  // free memory
  if (attribute) {
    delete attribute->attribute;
    delete attribute;
  }

}


// Return a document given its id.  
// Since this class is the "parser" too, we don't bother 
// passing the doc back.  (We've cached the results.)
doc_t *
MindDBManager::getDoc(docid_t docid) {
  doc_t * doc = new doc_t;
  doc->id = strdup(docid);
  doc->len = 0;
  doc->doc = NULL;
  return doc;
}

// Ignore - we don't store the docs locally with this one
void
MindDBManager::setOutputFile(char * filename) {
}

// Send a doc to the proxy
void
MindDBManager::output(docid_t docid) {

  // find the right schema
  int schemaInd;
  int numSchemas = mindResult->schemas.__size;
  for (int l = 0; l < numSchemas; l++) {
    if (!strcmp(schemaName, mindResult->schemas.__ptr[l].name)) {
      schemaInd = l;
      break;
    }
  }
      
  mindtype__PropDocument * document;

  // find the doc
  int numDocs = mindResult->propDocuments.__size;
  for (int docInd = 0; docInd < numDocs; docInd++) {

    // check for the right doc
    if (!strcmp(mindResult->propDocuments.__ptr[docInd].documentID.internalID,
		docid)) {
      
      // create the doc
      document = new mindtype__PropDocument();

      document->documentID.proxyID = 
	mindResult->propDocuments.__ptr[docInd].documentID.proxyID;
      document->documentID.internalID = 
	mindResult->propDocuments.__ptr[docInd].documentID.internalID;

      document->weight = mindResult->propDocuments.__ptr[docInd].weight;
      document->originalWeight = 
	mindResult->propDocuments.__ptr[docInd].originalWeight;

      document->schema = 
	new mindtype__Schema(mindResult->schemas.__ptr[schemaInd]);
      document->schemaname = NULL;
      
      document->attributes.__size = 
	mindResult->propDocuments.__ptr[docInd].attributes.__size;
      document->attributes.__ptr = 
	new mindtype__PropDocumentAttribute[document->attributes.__size];

      mindtype__PropDocumentAttribute * att = document->attributes.__ptr;      
      mindtype__PropDocumentAttribute * oldAtt = 
	mindResult->propDocuments.__ptr[docInd].attributes.__ptr;
      for (int i = 0; i < document->attributes.__size; i++) {
	att[i].attributename = oldAtt[i].attributename;
	att[i].attribute = oldAtt[i].attribute;
	att[i].content.type = oldAtt[i].content.type;
	att[i].content.value = oldAtt[i].content.value;
      }

      break;
    }
  }
  
  // tell the proxy to add the doc
  soap->enable_null = false;
  changeNamespace("mindcall", proxyName, "urn:*");
  mindcall__addDocumentResponse docResp;
  if (soap_call_mindcall__addDocument(soap, proxy, NULL,
				      *document,
				      &docResp) == SOAP_OK) {
  } else {
    soap_print_fault(soap, stderr);
    exit(-1);
  }


  // free memory
  if (document) {
    delete [] document->attributes.__ptr;
    delete document->schema;
    delete document;
  }
}


// Close the database and free memory.
void
MindDBManager::close() {
  // tell the proxy to construct the description
  soap->enable_null = false;
  changeNamespace("mindcall", proxyName, "urn:*");
  mindcall__constructResourceDescriptionResponse conResp;
  if (soap_call_mindcall__constructResourceDescription(soap, proxy, NULL,
						       &conResp) == SOAP_OK) {
  } else {
    soap_print_fault(soap, stderr);
    exit(-1);
  }


  free(registry);
  free(interface);
  free(construction);
  free(schemaName);
  free(sampleAttribute);
  free(proxyName);
}


void 
MindDBManager::changeNamespace(char * id, char * ns, char * re) {
  int i = 0;
  struct Namespace * names = soap->namespaces;
  while (names[i].id != NULL) {
    if (!strcmp(id, names[i].id)) {
      names[i].ns = ns;
      names[i].in = re;
      break;
    }
    i++;
  }
}
