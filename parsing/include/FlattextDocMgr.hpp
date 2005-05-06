/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
 */

#ifndef _FLATTEXTDOCMGR_HPP
#define _FLATTEXTDOCMGR_HPP

#include "common_headers.hpp"
#include "TextHandlerManager.hpp"
#include "Exception.hpp"
#include "DocumentManager.hpp"
namespace lemur 
{
  namespace parse 
  {
    
#define FT_SUFFIX ".flat"
#define FT_LOOKUP ".lookup"
#define FT_FID    ".fid"

    class FlattextDocMgr : public lemur::api::DocumentManager, public lemur::api::TextHandler {
    public:

      struct lookup_e {
        int fid;
        long offset;
        long bytes;
      };

      struct abc {
        bool operator() (char* s1, char* s2) const {
          return strcmp(s1, s2) < 0;
        }
      };

      /// constructor (for build) 
      ///             name = what to name this manager
      ///             mode = type of parser to use 
      ///             source = file with list of files this will manage
      FlattextDocMgr(string name, string mode, string source);  

      /// constructor (for open)
      ///             name = toc file for this manager (same as getMyID) 
      FlattextDocMgr(const string &name);

      virtual ~FlattextDocMgr();

      /// open previously created manager
      virtual bool open(const string &manname);

      virtual lemur::api::Parser* getParser() const {
        return lemur::api::TextHandlerManager::createParser(parseMode);
      }
      /// get the ID string to use for opening this later
      virtual const string &getMyID() const;
  
      /// return the document associated with this ID
      virtual char* getDoc(const string &docID) const;

      virtual void buildMgr();

      char* handleDoc(char * docno);
      void  handleEndDoc();

    protected:
      lemur::api::Parser* myparser;

    private:
      /// open the file with all the sources and save internally
      bool readinSources(const string &fn);

      /// save info we'll need to open this Manager
      void writeTOC();

      bool loadTOC(const string &fn);
      bool loadFTLookup(const string &fn);
      bool loadFTFiles(const string &fn, int num);

      int numdocs;              // how many docs we have
      string parseMode;           // what type of parser we have
      long prevpos;              // pos of previous doc beginning
      string IDname;            // my name
      string IDnameext;         // my name with type extension
      vector<string> sources;   // list of all source files
      int fileid;                           // fileid of current file being processed
      ofstream writefpos;       // stream for writing out file positions
      mutable map<string, lookup_e*, less<string> > table; 
      lookup_e* entries;        // array of lookup entries
    };
  }
}

#endif
