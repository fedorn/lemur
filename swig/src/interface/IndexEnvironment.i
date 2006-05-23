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

  void setDocumentRoot( const std::string& documentRoot ) throw (lemur::api::Exception);  
  void setAnchorTextPath( const std::string& anchorTextRoot ) throw (lemur::api::Exception);
  void setOffsetMetadataPath( const std::string& offsetMetadataRoot ) throw (lemur::api::Exception);
  void setOffsetAnnotationsPath( const std::string& offsetAnnotationsRoot ) throw (lemur::api::Exception);

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
  indri::parse::FileClassEnvironmentFactory::Specification *getFileClassSpec( const std::string& name) throw (lemur::api::Exception);
  void addFileClass( const indri::parse::FileClassEnvironmentFactory::Specification &spec) throw (lemur::api::Exception);

#endif
  void deleteDocument( int documentID ) throw (lemur::api::Exception);
  void setIndexedFields( const std::vector<std::string>& fieldNames ) throw (lemur::api::Exception);
  void setNumericField( const std::string& fieldName, bool isNumeric, const std::string & parserName="" ) throw (lemur::api::Exception);
  void setMetadataIndexedFields( const std::vector<std::string>& forward, const std::vector<std::string>& backward ) throw (lemur::api::Exception);
  void setStopwords( const std::vector<std::string>& stopwords ) throw (lemur::api::Exception);
  void setStemmer( const std::string& stemmer ) throw (lemur::api::Exception);
  void setMemory( UINT64 memory ) throw (lemur::api::Exception);
  void setNormalization( bool normalize ) throw (lemur::api::Exception);

  void create( const std::string& repositoryPath, indri::api::IndexStatus* callback = 0 ) throw (lemur::api::Exception);
  void open( const std::string& repositoryPath, indri::api::IndexStatus* callback = 0 ) throw (lemur::api::Exception);
  void close() throw (lemur::api::Exception);

  void addFile( const std::string& fileName ) throw (lemur::api::Exception);
  void addFile( const std::string& fileName, const std::string& fileClass ) throw (lemur::api::Exception);

  int addString( const std::string& fileName, const std::string& fileClass, const std::vector<indri::parse::MetadataPair>& metadata ) throw (lemur::api::Exception);
  int addParsedDocument( indri::api::ParsedDocument* document ) throw (lemur::api::Exception);
  int documentsIndexed() throw (lemur::api::Exception);
  int documentsSeen() throw (lemur::api::Exception);
};
}}
