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
// PowerPointDocumentExtractor
//
// 14 June 2004 -- tds
//

#ifdef WIN32
// windows.h has to be included first
// in case someone comes along and
// defines WIN32_LEAN_AND_MEAN
#include <windows.h>


#include "indri/PowerPointDocumentExtractor.hpp"
#include "indri/Buffer.hpp"
#include "lemur/lemur-compat.hpp"
#include "lemur/Exception.hpp"

//
// PowerPoint.Application:
//    doc = Documents.Open( filename )
//    foreach PowerPoint.Slide slide in doc.Slides:
//      foreach PowerPoint.Shape shape in slide.Shapes:
//        text = shape.TextFrame.TextRange.Text;
//

//
// copy_bstr_to_buffer
//

void copy_bstr_to_buffer( Buffer& docBuffer, BSTR bstr ) {
  UINT textLength = ((UINT*)bstr)[-1];
  
  if( !textLength )
    return;

  // get rid of any trailing nulls
  if( docBuffer.position() > 0 ) {
    docBuffer.unwrite(1);
    *docBuffer.write(1) = ' ';
  }
 
  // multibyte could potentially have 3 bytes for every 1 double-byte char
  UINT convertedLength = 3*(textLength+1);

  // convert the document text into a multibyte representation
  int trueLength = ::WideCharToMultiByte( CP_UTF8, 0, bstr, -1,
                                         docBuffer.write( convertedLength ), convertedLength,
                                         NULL, NULL );

  docBuffer.unwrite( convertedLength-trueLength );
}

//
// com_get_dispatch_id
//

DISPID com_get_dispatch_id( IDispatch* dispatch, LPOLESTR name ) {
  DISPID dispatchID;
  HRESULT hr;

  hr = dispatch->GetIDsOfNames( IID_NULL,
                                &name,
                                1,
                                LOCALE_SYSTEM_DEFAULT,
                                &dispatchID );
  
  if( FAILED(hr) ) {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Failed to get the ID of method." );
  }

  return dispatchID;
}

//
// com_property_get
//

void com_property_get( VARIANT* result, IDispatch* dispatch, LPOLESTR name ) {
  DISPID dispatchID = com_get_dispatch_id( dispatch, name );
  DISPPARAMS nullParameters = { NULL, NULL, 0, 0 };

  HRESULT hr;
  hr = dispatch->Invoke( dispatchID,
                         IID_NULL,
                         LOCALE_SYSTEM_DEFAULT,
                         DISPATCH_PROPERTYGET,
                         &nullParameters,
                         result,
                         NULL,
                         NULL );

  if( FAILED(hr) ) {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Failed to get the property." );
  }
}

//
// com_method_execute
//

void com_method_execute( VARIANT* result, IDispatch* dispatch, LPOLESTR name, DISPPARAMS* parameters ) {
  HRESULT hr;
  DISPID dispatchID = com_get_dispatch_id( dispatch, name );
  EXCEPINFO exceptionInfo;

  hr = dispatch->Invoke( dispatchID,
                         IID_NULL,
                         LOCALE_SYSTEM_DEFAULT,
                         DISPATCH_METHOD,
                         parameters,
                         result,
                         &exceptionInfo,
                         NULL );

  if( FAILED(hr) ) {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Failed to execute method." );
  }
}

//
// com_method_execute
//

void com_method_execute( VARIANT* result, IDispatch* dispatch, LPOLESTR name, VARIANT* parameter = 0 ) {
  DISPPARAMS oneParameter;

  oneParameter.cArgs = 1;
  oneParameter.cNamedArgs = 0;
  oneParameter.rgdispidNamedArgs = NULL;
  oneParameter.rgvarg = parameter;

  DISPPARAMS noParameters = { 0, 0, NULL, NULL };

  DISPPARAMS* parameters;

  if( parameter )
    parameters = &oneParameter;
  else
    parameters = &noParameters;

  com_method_execute( result, dispatch, name, parameters );
}

void com_property_put( VARIANT* result, IDispatch* dispatch, LPOLESTR name, VARIANT* parameter ) {
  HRESULT hr;
  DISPID dispatchID = com_get_dispatch_id( dispatch, name );
  EXCEPINFO exceptionInfo;
  unsigned int badArgument;

  DISPPARAMS parameters;
  DISPID putID = DISPID_PROPERTYPUT;

  parameters.cArgs = 1;
  parameters.cNamedArgs = 1;
  parameters.rgdispidNamedArgs = &putID;
  parameters.rgvarg = parameter;

  hr = dispatch->Invoke( dispatchID,
                         IID_NULL,
                         LOCALE_SYSTEM_DEFAULT,
                         DISPATCH_PROPERTYPUT,
                         &parameters,
                         NULL,
                         &exceptionInfo,
                         &badArgument );

  if( FAILED(hr) ) {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Failed to put property." );
  }
}

PowerPointDocumentExtractor::PowerPointDocumentExtractor() {
  ::CoInitialize( NULL );

  HRESULT hr;
  VARIANT result;
	CLSID pptClsid;
	CLSIDFromProgID( L"PowerPoint.Application", &pptClsid );  

  hr = ::CoCreateInstance( pptClsid,
                           NULL,
                           CLSCTX_LOCAL_SERVER,
                           IID_IUnknown,
                           (void**) &_powerPointUnknown );

  if( FAILED(hr) ) {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Couldn't get IUnknown interface pointer to PowerPoint application." );
  }

  hr = _powerPointUnknown->QueryInterface( IID_IDispatch, (void**) &_powerPointDispatch );

  if( FAILED(hr) ) {
    _powerPointUnknown->Release();
    _powerPointUnknown = 0;
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Couldn't get Dispatch interface pointer to PowerPoint application." );
  }

  VARIANT param;
  param.vt = VT_INT;
  param.intVal = -1;
  com_property_put( &result, _powerPointDispatch, L"Visible", &param );

  com_property_get( &result, _powerPointDispatch, L"Presentations" );                                          
  _presentationsDispatch = result.pdispVal;

  _documentWaiting = false;
}

PowerPointDocumentExtractor::~PowerPointDocumentExtractor() {
  _presentationsDispatch->Release();
  _powerPointDispatch->Release();
  _powerPointUnknown->Release();
}

void PowerPointDocumentExtractor::open( const std::string& filename ) {
  HRESULT hr;
  VARIANT result;

  _documentPath = filename;
  _documentBuffer.clear();
  *_documentBuffer.write(1) = 0;

  // copy the path into a form powerpoint will understand
  WCHAR widePathCopy[ MAX_PATH ];
  ::MultiByteToWideChar( CP_UTF8, 0, filename.c_str(), filename.length()+1, widePathCopy, MAX_PATH );

  // open the document in powerpoint
  DISPPARAMS parameters;
  VARIANTARG arguments[4];

  parameters.cArgs = 4;
  parameters.cNamedArgs = 0;
  parameters.rgdispidNamedArgs = NULL;
  parameters.rgvarg = arguments;

  arguments[3].vt = VT_BSTR;
  arguments[3].bstrVal = ::SysAllocString( widePathCopy ); // filename

  arguments[2].vt = VT_I4;
  arguments[2].lVal = -1; // readonly

  arguments[1].vt = VT_I4;
  arguments[1].lVal = 0; // untitled

  arguments[0].vt = VT_I4;
  arguments[0].lVal = -1; // withwindow

  com_method_execute( &result, _presentationsDispatch, L"Open", &parameters );
  IDispatch* documentDispatch = result.pdispVal;

  // get rid of the filename parameter
  ::SysFreeString( arguments[3].bstrVal );

  // get the Slides dispatch
  com_property_get( &result, documentDispatch, L"Slides" );
  IDispatch* slidesDispatch = result.pdispVal;

  // get the count of slides
  com_property_get( &result, slidesDispatch, L"Count" );
  long slidesCount = result.lVal;
  VARIANT index;
  memset( &index, 0, sizeof index );
  index.vt = VT_I2;
  
  for( int i=1; i<=slidesCount; i++ ) {
    index.lVal = i;

    // fetch the i^th slide
    com_method_execute( &result, slidesDispatch, L"Item", &index );
    IDispatch* slideDispatch = result.pdispVal;

    // fetch the shapes
    com_property_get( &result, slideDispatch, L"Shapes" );
    IDispatch* shapesDispatch = result.pdispVal;

    // fetch shapes count
    com_property_get( &result, shapesDispatch, L"Count" );
    long shapesCount = result.lVal;

    for( int j=1; j<=shapesCount; j++ ) {
      index.lVal = j;

      // fetch the j^th shape
      com_method_execute( &result, shapesDispatch, L"Item", &index );
      IDispatch* shapeDispatch = result.pdispVal;

      // fetch the textframe
      com_property_get( &result, shapeDispatch, L"TextFrame" );
      IDispatch* textFrameDispatch = result.pdispVal;

      // fetch the textrange
      IDispatch* textRangeDispatch = 0;
      BSTR textResult = 0;

      try {
        com_property_get( &result, textFrameDispatch, L"TextRange" );
        IDispatch* textRangeDispatch = result.pdispVal;

        // fetch the text itself
        com_property_get( &result, textRangeDispatch, L"Text" );
        BSTR textResult = result.bstrVal;

        copy_bstr_to_buffer( _documentBuffer, textResult );
      } catch( Exception& e ) {
        if( textResult )
          ::SysFreeString( textResult );
        if( textRangeDispatch )
          textRangeDispatch->Release();
      }

      // clean up
      textFrameDispatch->Release();
      shapeDispatch->Release();
    }

    // clean up
    shapesDispatch->Release();
    slideDispatch->Release();
  }

  // clean up: release the slides
  slidesDispatch->Release();

  // add metadata to identify this file
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
  pair.value = (void*) "MSPPT";
  pair.valueLength = 6;
  _unparsedDocument.metadata.push_back( pair );

  _unparsedDocument.text = _documentBuffer.front();
  _unparsedDocument.textLength = _documentBuffer.position();

  // close the document
  com_method_execute( &result, documentDispatch, L"Close" );
  documentDispatch->Release();
  _documentWaiting = true;
}

UnparsedDocument* PowerPointDocumentExtractor::nextDocument() {
  if( _documentWaiting ) {
    _documentWaiting = false;
    return &_unparsedDocument;
  }
  return 0;
}

void PowerPointDocumentExtractor::close() {
}
#endif
