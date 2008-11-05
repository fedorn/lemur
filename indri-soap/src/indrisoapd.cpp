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


/*      Indri Web API
        07/2006 -- dmf
 */


#include "soapH.h"
#include "soapindrisoapObject.h"
#include "indrisoap.nsmap"
#include <set>
#include "indri/IndexEnvironment.hpp"
#include "indri/QueryEnvironment.hpp"

indri::api::QueryEnvironment queryEnv;
indri::api::IndexEnvironment *indexEnv = NULL;
bool readWrite;
char hostname[255];
int port;

static std::string _buildCachedURL(int docno) {
  std::stringstream str;
  str << "http://" << hostname << ":" << port << "/?docno=" << docno;
  return str.str();
}
                  
/*
  soap api calls
*/

int indrisoap__doRunQueryRequest(struct soap *soap, 
                                 struct indrisoap__QueryRequest request,
                                 struct indrisoap__doRunQueryResponse &response) {

  indri::api::QueryRequest req;
  req.query = request.query;
  req.resultsRequested = request.resultsRequested;
  req.startNum = request.startNum;
  req.options = (indri::api::QueryRequest::Options)request.options;
  // docset
  if (request.docSet.__size > 0) {
    for (int i = 0; i < request.docSet.__size; i++)
      req.docSet.push_back(request.docSet.__ptr[i]);
  }
  // formulators
  for (int i = 0; i < request.formulators.__size; i++) {
    req.formulators.push_back(request.formulators.__ptr[i]);
  }
  //metadata
  for (int i = 0; i < request.metadata.__size; i++) {
    req.metadata.push_back(request.metadata.__ptr[i]);
  }
  indri::api::QueryResults res;
  try {    
    res = queryEnv.runQuery(req);
  } catch (lemur::api::Exception e) {
    // do something here
    return soap_receiver_fault(soap, e.what().c_str(), "");
  }
  // build the response.
  response._return.parseTime = res.parseTime;  
  response._return.executeTime = res.executeTime;
  response._return.documentsTime = res.documentsTime;
  response._return.estimatedMatches = res.estimatedMatches;
  int count = res.results.size();
  response._return.results.__size = count;
  response._return.results.__ptr = soap_new_indrisoap__Result(soap, count);
  for (int i = 0; i < count; i++) {
    indrisoap__Result &result = response._return.results.__ptr[i];
    result.score = res.results[i].score;
    result.docid = res.results[i].docid;
    result.documentName =res.results[i].documentName;
    result.begin = res.results[i].begin;
    result.end = res.results[i].end;
    result.snippet = res.results[i].snippet;
    result.metadata.__size = res.results[i].metadata.size();
    if ( result.metadata.__size > 0) {
      result.metadata.__ptr = soap_new_indrisoap__MetadataPair(soap, result.metadata.__size);
      for (unsigned int j = 0; j < result.metadata.__size; j++) {
        indrisoap__MetadataPair &meta = result.metadata.__ptr[j];
        meta.key = res.results[i].metadata[j].key;
        meta.value = res.results[i].metadata[j].value;
      }
    }
    result.cachedURL = _buildCachedURL(result.docid);
  }
  return SOAP_OK;
}

int indrisoap__doAddDocument(struct soap *soap,
                             xsd__string doc, 
                             xsd__string fileClass,
                             struct MetadataPairArray meta,
                             struct indrisoap__doAddDocumentResponse &response) 
{
  if (! readWrite) {
    return soap_receiver_fault(soap, "Index is read only mode.", "");
  }
  
  try {
    // optional fileClass parameter (defualt to xml.
    // update environment to index the fields in the index.
    // might make sense to cache whether or not we've augmented this
    // so we only do the work once.
    std::vector<std::string> fields = queryEnv.fieldList();
    indri::parse::FileClassEnvironmentFactory::Specification *spec = indexEnv->getFileClassSpec(fileClass);
    std::vector<std::string> & vec = spec->index;
    bool changed = false;
    for (std::vector<std::string>::iterator i1 = fields.begin(); i1 != fields.end(); i1++) {
      std::string &name = *i1;
      if( std::find( vec.begin(), vec.end(), name ) == vec.end() ) {
        vec.push_back(name);
        changed = true;
      }
    }
    if (changed) indexEnv->addFileClass(*spec);
    // setup the metadata
    std::vector<indri::parse::MetadataPair> metadata;
    indri::parse::MetadataPair pair;

    for (int i = 0; i < meta.__size; i++ ) {
      pair.key = (char *)meta.__ptr[i].key.c_str();
      pair.value = (char *)meta.__ptr[i].value.c_str();
      pair.valueLength = meta.__ptr[i].value.length() + 1;
      metadata.push_back(pair);
    }

    // no file class, just xml, or text?
    response.docid = indexEnv->addString(doc, fileClass, metadata);
  } catch (lemur::api::Exception e) {
    // do something here
    return soap_receiver_fault(soap, e.what().c_str(), "");
  }
  return SOAP_OK;
}

int indrisoap__doDeleteDocument( struct soap *soap, xsd__int doc,
                                 struct indrisoap__doDeleteDocumentResponse &response) {
  if (! readWrite) {
    return soap_receiver_fault(soap, "Index is read only mode.", "");
  }

  try {
    indexEnv->deleteDocument(doc);
  } catch (lemur::api::Exception e) {
    return soap_receiver_fault(soap, e.what().c_str(), "");
  }
  return SOAP_OK;
}

static void _buildDocumentVector(struct soap* soap, 
                                 indri::api::DocumentVector *source,
                                 struct indrisoap__DocumentVector *target) {
  target->stems.__size = source->stems().size();
  target->stems.__ptr = soap_new_std__string(soap, target->stems.__size);
  for (int i = 0; i < target->stems.__size; i++) {
    target->stems.__ptr[i] = source->stems()[i];
  }

  target->positions.__size = source->positions().size();
  target->positions.__ptr = (int *)soap_malloc(soap, sizeof(int) * target->positions.__size);
  for (int i = 0; i < target->positions.__size; i++) {
    target->positions.__ptr[i] = source->positions()[i];
  }

  target->fields.__size = source->fields().size();
  target->fields.__ptr = soap_new_indrisoap__Field(soap, target->fields.__size);
  for (int i = 0; i < target->fields.__size; i++) {
    target->fields.__ptr[i].name = source->fields()[i].name;
    target->fields.__ptr[i].begin = source->fields()[i].begin;
    target->fields.__ptr[i].end = source->fields()[i].end;
    target->fields.__ptr[i].number = source->fields()[i].number;
  }
  
}

static void _buildDocumentVectorsResponse(struct soap* soap, 
                                          std::vector<indri::api::DocumentVector *> _results,
                                          struct DocumentVectorArray &response) {
  // allocate vector
  response.__size = _results.size();
  response.__ptr = soap_new_indrisoap__DocumentVector(soap, response.__size);
  // copy all of the contents.
  for (int i = 0; i < _results.size(); i++) {
    _buildDocumentVector(soap, _results[i], &response.__ptr[i]);
    // delete input.
    delete(_results[i]);
  }
}

int indrisoap__doDocumentVectors(struct soap* soap, struct intArray ids, 
                                 struct DocumentVectorArray &response) {
  std::vector<lemur::api::DOCID_T> docs;
  for (int i = 0; i < ids.__size; i++)
    docs.push_back(ids.__ptr[i]);
  std::vector<indri::api::DocumentVector*> _results;
  try {    
    _results = queryEnv.documentVectors( docs );
  } catch (lemur::api::Exception e) {
    // do something here
    return soap_receiver_fault(soap, e.what().c_str(), "");
  }
  _buildDocumentVectorsResponse(soap, _results, response);
  return SOAP_OK;
}

static int soap_encode_string(const char *s, char *t, size_t len)
{ register int c;
 register size_t n = len;
 while ((c = *s++) && n-- > 1)
   { if (c == ' ') 
     *t++ = '+';
   else if (c == '!'
            || c == '$'
            || (c >= '(' && c <= '.')
            || (c >= '0' && c <= '9')
            || (c >= 'A' && c <= 'Z')
            || c == '_'
            || (c >= 'a' && c <= 'z'))
     *t++ = (char)c;
   else if (n > 2)
     { *t++ = '%';
     *t++ = (char)((c >> 4) + (c > 159 ? '7' : '0'));
     c &= 0xF;
     *t++ = (char)(c + (c > 9 ? '7' : '0'));
     n -= 2;
     }
   else
     break;
   }
 *t = '\0';
 return len - n;
}

static const char* soap_decode_string(char *buf, size_t len, const char *val)
{ const char *s;
 char *t;
 for (s = val; *s; s++)
   if (*s != ' ' && *s != '=')
     break;
 if (*s == '"')
   { t = buf;
   s++;
   while (*s && *s != '"' && --len)
     *t++ = *s++;
   *t = '\0';
   do s++;
   while (*s && *s != '&' && *s != '=');
   }
 else
   { t = buf;
   while (*s && *s != '&' && *s != '=' && --len)
     { switch (*s)
       { case '+':
           *t++ = ' ';
       case ' ':
         s++;
         break;
       case '%':
         *t++ = ((s[1] >= 'A' ? (s[1]&0x7) + 9 : s[1] - '0') << 4) + (s[2] >= 'A' ? (s[2]&0x7) + 9 : s[2] - '0');
         s += 3;
         break;
       default:
         *t++ = *s++;
       }
     }
   *t = '\0';
   }
 return s;
}

static char *query(struct soap *soap)
{ return strchr(soap->path, '?');
}

static char *query_key(struct soap *soap, char **s)
{ char *t = *s;
 if (t && *t)
   { *s = (char*)soap_decode_string(t, strlen(t), t + 1);
   return t;
   }
 return *s = NULL;
}

static char *query_val(struct soap *soap, char **s)
{ char *t = *s;
 if (t && *t == '=')
   { *s = (char*)soap_decode_string(t, strlen(t), t + 1);
   return t;
   }
 return NULL;
}

/* fetch a cached document link */
/* as plain content */
int http_get(struct soap *soap)
{
  char *s;
  int docid;  

  s = query(soap);
  if (!s )
    return SOAP_GET_METHOD; // bad request.

  while (s) { 
    char *key = query_key(soap, &s);
    char *val = query_val(soap, &s);
    if (key && ! strcmp(key, "docno")) {
      docid = atoi(val);
      break;
    }
  }

  std::vector<lemur::api::DOCID_T> docs;
  docs.push_back(docid);
  std::vector<indri::api::ParsedDocument*> documents;
  try {    
    documents = queryEnv.documents( docs );
  } catch (lemur::api::Exception e) {
    // do something here
    return 404;
  }
  // if the docid is bad
  if (documents.size() == 0) return 404;
  
  soap_response(soap, SOAP_HTML);
  soap_send(soap, documents[0]->content);
  soap_end_send(soap);
  delete(documents[0]);
  return SOAP_OK;
}

/*server*/

static void *process_request(struct soap *soap) { 
  pthread_detach(pthread_self());
  // serve request (or multiple requests with keep-alive enabled)
  soap_serve(soap);
  // cleanup class instances and allocated memory.
  soap_dealloc(soap, NULL);
  
  soap_free(soap);
  
  soap_destroy(soap);
  // cleanup
  soap_end(soap);
  // detach thread's copy of soap environment
  soap_done(soap);
  // free soap environment
  free(soap);
}

#include <signal.h>
volatile sig_atomic_t gotsig = 0;
void handler(int signo) {
  gotsig = signo;
  queryEnv.close();
  // this works for C-c interrupt.
  // probably ought to handle a few others (TERM. QUIT, ???).
  // clean up.
  if (indexEnv) {
    indexEnv->close();
    delete(indexEnv);
  }
  signal(SIGINT, SIG_DFL);
  kill(getpid(), SIGINT);
}


int main(int argc, char **argv)
{
  signal(SIGINT, handler);
  
  indri::api::Parameters& param = indri::api::Parameters::instance();
  // initialize environments
  try {
    param.loadCommandLine( argc, argv );
    readWrite = param.get("readwrite", false);
    if( param.exists( "index" ) ) {
      indri::api::Parameters indexes = param["index"];
      int count = indexes.size();
      // even if they said readwrite
      if (count > 1) readWrite = false;
      for( unsigned int i = 0; i < count; i++ ) {
        std::string repoPath = indexes[i];

        if (readWrite) {
          indexEnv = new indri::api::IndexEnvironment();
          indexEnv->open(repoPath);
          queryEnv.addIndex(*indexEnv);
        }  else {
          queryEnv.addIndex(repoPath);
        }
      }
    }
  } catch (lemur::api::Exception e) {
    std::cerr << e.what() << std::endl;
    exit(-1);
  }

  indrisoapService *soap;
  int m, s;
  soap = new indrisoapService();
  // run as stand-alone server on machine 
  struct soap *tsoap;
  pthread_t tid;
  port = param.get("port", 9696);
  gethostname(hostname, 255);
  
  // bind to current host and specified port
  m = soap_bind(soap, NULL, port, 100);
  // if we could not bind, exit
  if (m < 0) { 
    soap_print_fault(soap, stderr);
    exit(1);
  }
  fprintf(stderr, "Socket connection successful %d\n", m);
  // die after 24 hrs waiting for activity on port
  //  soap->accept_timeout = 24*60*60;
  // IO timeouts
  soap->send_timeout = 30;
  soap->recv_timeout = 30;
  
  soap->fget = http_get;
  
  // needs a terminate message.
  // loop through requests
  int i = 1;
  while (gotsig == 0) { // accept request
    s = soap_accept(soap);
    // if timeout or error, report
    if (s < 0) {
      if (soap->errnum)
        soap_print_fault(soap, stderr);
      else
        fprintf(stderr, "Server timed out\n");
      break;
    }
    fprintf(stderr, "Thread %d accepts socket %d connection from IP %d.%d.%d.%d\n", i, s, (int)(soap->ip>>24)&0xFF, (int)(soap->ip>>16)&0xFF, (int)(soap->ip>>8)&0xFF, (int)soap->ip&0xFF);
    // copy soap environment and spawn thread
    tsoap = soap_copy(soap);
    tsoap->fget = http_get;
    pthread_create(&tid, NULL, (void*(*)(void*))process_request, (void*)tsoap);
    i++;
  }
  queryEnv.close();
  // clean up.
  if (indexEnv) {
    indexEnv->close();
    delete(indexEnv);
  }
  delete(soap);
  return 0;
}
