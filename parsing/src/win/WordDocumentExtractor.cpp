/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.cs.cmu.edu/~lemur/license.html 
  as well as the conditions below.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

  3. The names "Indri", "Center for Intelligent Information Retrieval", 
  "CIIR", and "University of Massachusetts" must not be used to
  endorse or promote products derived from this software without
  prior written permission. To obtain permission, contact
  indri-info@ciir.cs.umass.edu.

  4. Products derived from this software may not be called "Indri" nor 
  may "Indri" appear in their names without prior written permission of 
  the University of Massachusetts. To obtain permission, contact 
  indri-info@ciir.cs.umass.edu.

  THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF MASSACHUSETTS AND OTHER
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGE.
  ==========================================================================
*/


//
// OfficeDocumentExtractor
//
// 14 June 2004 -- tds
//
// Code is based in part on the AutoWord class
// by Poonam Bajaj.
//

#ifdef WIN32
// windows.h has to be included first
// in case someone comes along and
// defines WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "indri/WordDocumentExtractor.hpp"

#ifdef WIN32


// using this wde_internal structure for windows types
// so that we keep them out of the headers
// this makes cross platform builds a little easier

struct wde_internal {
  IUnknown* _wordUnknown;
  IDispatch* _wordDispatch;
  IDispatch* _documentsDispatch;
  DISPID _openDispatchID;
  DISPID _closeDispatchID;
};

#define v_internal ( (wde_internal*) _internal )

WordDocumentExtractor::WordDocumentExtractor() {
  _internal = new wde_internal;

  v_internal->_documentsDispatch = 0;
  v_internal->_wordDispatch = 0;
  v_internal->_wordDispatch = 0;

  initialize();
  _documentWaiting = false;
}

WordDocumentExtractor::~WordDocumentExtractor() {
  uninitialize();
}

void WordDocumentExtractor::uninitialize() {
  if( v_internal ) {
    // close application too?
    if( v_internal->_documentsDispatch )
      v_internal->_documentsDispatch->Release();
    if( v_internal->_wordUnknown )
      v_internal->_wordUnknown->Release();
    if( v_internal->_wordDispatch )
      v_internal->_wordDispatch->Release();
    delete v_internal;
  }
}

void WordDocumentExtractor::initialize() {
  ::CoInitialize( NULL );

  HRESULT hr;
	CLSID wordClsid;
	CLSIDFromProgID( L"Word.Application", &wordClsid );  

  hr = ::CoCreateInstance( wordClsid,
                            NULL,
                            CLSCTX_LOCAL_SERVER,
                            IID_IUnknown,
                            (void**) &v_internal->_wordUnknown );

  if( FAILED(hr) ) {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Couldn't get Unknown interface pointer to Word application." );
  }

  hr = v_internal->_wordUnknown->QueryInterface( IID_IDispatch, (void**) &v_internal->_wordDispatch );

  if( FAILED(hr) ) {
    v_internal->_wordUnknown->Release();
    v_internal->_wordUnknown = 0;
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Couldn't get Dispatch interface pointer to Word application." );
  }

  LPOLESTR documentsPropertyName = L"Documents";
  DISPID documentsDispatchID;

  hr = v_internal->_wordDispatch->GetIDsOfNames( IID_NULL,
                                      &documentsPropertyName,
                                      1,
                                      LOCALE_SYSTEM_DEFAULT,
                                      &documentsDispatchID );

  DISPPARAMS parameters = { NULL, NULL, 0, 0 };
  VARIANT result;

  hr = v_internal->_wordDispatch->Invoke( documentsDispatchID,
                              IID_NULL,
                              LOCALE_SYSTEM_DEFAULT,
                              DISPATCH_PROPERTYGET,
                              &parameters,
                              &result,
                              NULL,
                              NULL );
                                      
  v_internal->_documentsDispatch = result.pdispVal;

  LPOLESTR openMethodName = L"Open";

  hr = v_internal->_documentsDispatch->GetIDsOfNames( IID_NULL,
                                          &openMethodName,
                                          1,
                                          LOCALE_SYSTEM_DEFAULT,
                                          &v_internal->_openDispatchID );
}

void WordDocumentExtractor::open( const std::string& filename ) {
  // open the document
  HRESULT hr;
  VARIANT result;
  DISPPARAMS parameters;
  VARIANT fileNameParameter;

  _documentPath = filename;

  WCHAR widePathCopy[ MAX_PATH ];
  ::MultiByteToWideChar( CP_UTF8, 0, filename.c_str(), filename.length()+1, widePathCopy, MAX_PATH );

  fileNameParameter.vt = VT_BSTR;
  fileNameParameter.bstrVal = ::SysAllocString( widePathCopy );

  parameters.cArgs = 1;
  parameters.cNamedArgs = 0;
  parameters.rgdispidNamedArgs = NULL;
  parameters.rgvarg = &fileNameParameter; 

  hr = v_internal->_documentsDispatch->Invoke( v_internal->_openDispatchID,
                                    IID_NULL,
                                    LOCALE_SYSTEM_DEFAULT,
                                    DISPATCH_METHOD,
                                    &parameters,
                                    &result,
                                    NULL,
                                    NULL );
  
  if( FAILED(hr) ) {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Couldn't open file " + filename + "." );
  }

  IDispatch* documentDispatch = result.pdispVal;

  // get content dispatch ID
  DISPID contentDispatchID;
  LPOLESTR contentPropertyName = L"Content";

  hr = documentDispatch->GetIDsOfNames( IID_NULL,
                                        &contentPropertyName,
                                        1,
                                        LOCALE_SYSTEM_DEFAULT,
                                        &contentDispatchID );

  // fetch the content property
  DISPPARAMS noParameters = { NULL, NULL, 0, 0 };

  hr = documentDispatch->Invoke( contentDispatchID,
                                  IID_NULL,
                                  LOCALE_SYSTEM_DEFAULT,
                                  DISPATCH_PROPERTYGET,
                                  &noParameters,
                                  &result,
                                  NULL,
                                  NULL );

  if( FAILED(hr) ) {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Couldn't get content for file " + filename + "." );
  }

  IDispatch* contentDispatch = result.pdispVal;

  // get the Text property dispatch ID
  DISPID textDispatchID;
  LPOLESTR textPropertyName = L"Text";

  hr = contentDispatch->GetIDsOfNames( IID_NULL,
                                        &textPropertyName,
                                        1,
                                        LOCALE_SYSTEM_DEFAULT,
                                        &textDispatchID );

  if( FAILED(hr) ) {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Couldn't get text for file " + filename + "." );
  }

  // call the text method on the content
  hr = contentDispatch->Invoke( textDispatchID,
                                IID_NULL,
                                LOCALE_SYSTEM_DEFAULT,
                                DISPATCH_PROPERTYGET,
                                &noParameters,
                                &result,
                                NULL,
                                NULL );

  if( FAILED(hr) ) {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Couldn't get text for file " + filename + "." );
  }

  // get the document text as a string
  BSTR textResult = result.bstrVal;
  UINT textLength = ((UINT*)textResult)[-1];

  // multibyte could potentially have 3 bytes for every 1 double-byte char
  UINT convertedLength = 3*textLength;
  _documentTextBuffer.clear();

  // convert the document text into a multibyte representation
  int trueLength = ::WideCharToMultiByte( CP_UTF8, 0, textResult, -1,
                                         _documentTextBuffer.write( convertedLength ), convertedLength,
                                         NULL, NULL );
  _documentTextBuffer.unwrite( convertedLength-trueLength );

  MetadataPair pair;

  _unparsedDocument.metadata.clear();

  pair.key = "docno";
  pair.value = _documentPath.c_str();
  pair.valueLength = _documentPath.length()+1;
  _unparsedDocument.metadata.push_back( pair );

  pair.key = "path";
  pair.value = _documentPath.c_str();
  pair.valueLength = _documentPath.length()+1;
  _unparsedDocument.metadata.push_back( pair );

  pair.key = "filetype";
  pair.value = (void*) "MSWORD";
  pair.valueLength = 7;
  _unparsedDocument.metadata.push_back( pair );

  _unparsedDocument.text = _documentTextBuffer.front();
  _unparsedDocument.textLength = _documentTextBuffer.position();

  // do something with this string here
  ::SysFreeString( textResult );
  ::SysFreeString( fileNameParameter.bstrVal );

  contentDispatch->Release();
  
  // close the document
  hr = documentDispatch->Invoke( v_internal->_closeDispatchID,
                                  IID_NULL,
                                  LOCALE_SYSTEM_DEFAULT,
                                  DISPATCH_METHOD,
                                  &noParameters,
                                  &result,
                                  NULL,
                                  NULL );
  documentDispatch->Release();
  _documentWaiting = true;
}

UnparsedDocument* WordDocumentExtractor::nextDocument() {
  if( _documentWaiting ) {
    _documentWaiting = false;
    return &_unparsedDocument;
  }

  return 0;
}

void WordDocumentExtractor::close() {
  _documentWaiting = false;
}

#endif // WIN32


