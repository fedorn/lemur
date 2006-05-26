%typemap(javaimports) indri::api::IndexEnvironment "import java.util.Map;"
namespace indri {
  namespace api {
    %feature("director") IndexStatus;
    struct IndexStatus {
      enum action_code {
        FileOpen,
        FileSkip,
        FileError,
        FileClose,
        DocumentCount
      };
      virtual ~IndexStatus();
      virtual void status( int code, const std::string& documentPath, const std::string& error, int documentsIndexed, int documentsSeen ) = 0;
    };

    class IndexEnvironment {
    public:

      IndexEnvironment();
      ~IndexEnvironment();
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Set the document root path
       @param documentRoot path to document root.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void setDocumentRoot( const std::string& documentRoot ) throw (lemur::api::Exception);  
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
 Set anchor text root path.
@param anchorTextRoot path to anchor text root.
@throws Exception if 
*/
public";
#endif

      void setAnchorTextPath( const std::string& anchorTextRoot ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
      Set offset metadata root path.
      @param offsetMetadataRoot path to offset metadata root.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void setOffsetMetadataPath( const std::string& offsetMetadataRoot ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Set offset annotations root path.
       @param offsetAnnotationsRoot path to offset annotations root.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void setOffsetAnnotationsPath( const std::string& offsetAnnotationsRoot ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Add parsing information for a file class. Data for these parameters
       is passed into the FileClassEnvironmentFactory
       @param name name of this file class, eg trecweb
       @param iterator document iterator for this file class
       @param parser document parser for this file class
       @param tokenizer document tokenizer for this file class
       @param startDocTag tag indicating start of a document
       @param endDocTag tag indicating the end of a document
       @param endMetadataTag tag indicating the end of the metadata fields
       @param include default tags whose contents should be included in the index
       @param exclude tags whose contents should be excluded from the index
       @param index tags that should be forwarded to the index for tag extents
       @param metadata tags whose contents should be indexed as metadata
       @param conflations tags that should be conflated

@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void addFileClass( const std::string& name, 
                         const std::string& iterator,
                         const std::string& parser,
                         const std::string& tokenizer,
                         const std::string& startDocTag,
                         const std::string& endDocTag,
                         const std::string& endMetadataTag,
                         const std::vector<std::string>& include,
                         const std::vector<std::string>& exclude,
                         const std::vector<std::string>& index,
                         const std::vector<std::string>& metadata, 
                         const std::map<indri::parse::ConflationPattern *,std::string>& conflations ) throw (lemur::api::Exception);

#ifdef SWIGCSHARP
      indri::parse::Specification *getFileClassSpec( const std::string& name) throw (lemur::api::Exception);
      void addFileClass( const indri::parse::Specification &spec) throw (lemur::api::Exception);
#else 
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Get a named file class.
       @param name The name of the file class to retrieve.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      indri::parse::FileClassEnvironmentFactory::Specification *getFileClassSpec( const std::string& name) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Add a file class.
       @param spec The file class to add.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void addFileClass( const indri::parse::FileClassEnvironmentFactory::Specification &spec) throw (lemur::api::Exception);

#endif
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Delete an existing document.
       @param documentID The document to delete.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void deleteDocument( int documentID ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Set names of fields to be indexed.  This call indicates to the index that information about
       these fields should be stored in the index so they can be used in queries.  This does not
       affect whether or not the text in a particular field is stored in an index.
       @param fieldNames the list of fields.

@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void setIndexedFields( const std::vector<std::string>& fieldNames ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Set the numeric property of a field. 
       @param fieldName the field.
       @param isNumeric true if the field is a numeric field, false if not.
       @param parserName The name of the Transformation to use to compute the numeric value of the field. Repository currently recognizes the name NumericFieldAnnotator.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void setNumericField( const std::string& fieldName, bool isNumeric, const std::string & parserName="" ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Set names of metadata fields to be indexed for fast retrieval.
       The forward fields are indexed in a B-Tree mapping (documentID, metadataValue).
       If a field is not forward indexed, the documentMetadata calls will still work, but they
       will be slower (the document has to be retrieved, decompressed and parsed to get the metadata back,
       instead of just a B-Tree lookup).  The backward indexed fields store a mapping of (metadataValue, documentID).
       If a field is not backward indexed, the documentIDsFromMetadata and documentFromMetadata calls will not work.
       @param forward the list of fields to forward index.
       @param backward the list of fields to backward index.

@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void setMetadataIndexedFields( const std::vector<std::string>& forward, const std::vector<std::string>& backward ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       set the list of stopwords
       @param stopwords the list of stopwords
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void setStopwords( const std::vector<std::string>& stopwords ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       set the stemmer to use
       @param stemmer the stemmer to use. One of krovetz, porter

@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void setStemmer( const std::string& stemmer ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       set the amount of memory to use for internal structures
       @param memory the number of bytes to use.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void setMemory( UINT64 memory ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       set normalization of case and some punctuation; default is true (normalize during indexing and at query time)
       @param normalize True, if text should be normalized, false otherwise.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void setNormalization( bool normalize ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       create a new index and repository
       @param repositoryPath the path to the repository
       @param callback IndexStatus object to be notified of indexing progress.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void create( const std::string& repositoryPath, indri::api::IndexStatus* callback = 0 ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       open an existing index and repository
       @param repositoryPath the path to the repository
       @param callback IndexStatus object to be notified of indexing progress.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void open( const std::string& repositoryPath, indri::api::IndexStatus* callback = 0 ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       close the index and repository
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void close() throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Add the text in a file to the index and repository.  The fileClass of this file
       will be chosen based on the file extension.  If the file has no extension, it will
       be skipped.  Information about indexing progress will be passed to the callback.
       @param fileName the file to add
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void addFile( const std::string& fileName ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       add a file of the specified file class to the index and repository
       @param fileName the file to add
       @param fileClass the file class to add (eg trecweb).
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      void addFile( const std::string& fileName, const std::string& fileClass ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**

       Adds a string to the index and repository.  The documentString is assumed to contain the kind of
       text that would be found in a file of type fileClass.
       @param fileName the document to add
       @param fileClass the file class to add (eg trecweb).
       @param metadata the metadata pairs associated with the string.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif


      int addString( const std::string& fileName, const std::string& fileClass, const std::vector<indri::parse::MetadataPair>& metadata ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       add an already parsed document to the index and repository
       @param document the document to add
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      int addParsedDocument( indri::api::ParsedDocument* document ) throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Returns the number of documents indexed so far in this session.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      int documentsIndexed() throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**

       Returns the number of documents considered for indexing,
       which is the sum of the documents indexed and the documents
       skipped.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      int documentsSeen() throw (lemur::api::Exception);
    };
  }
}
