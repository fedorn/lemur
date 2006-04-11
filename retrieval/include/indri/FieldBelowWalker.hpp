#include "indri/Walker.hpp"

#ifndef INDRI_FIELDBELOWWALKER_HPP
#define INDRI_FIELDBELOWWALKER_HPP

namespace indri
{
  namespace lang
  {
    
    // Identifies where there's a field at or below the curent node in the query spec.
    // This is used by InferenceNetworkBuilder to determine whether the nested operators
    // are needed.

    class FieldBelowWalker : public indri::lang::Walker { 
    private:
     
      bool _seenField;
      
    public:
      FieldBelowWalker() : _seenField(false) {}
      
      
      bool fieldBelow() {
	return _seenField;
      }

      void before( indri::lang::Field* f ) {
	_seenField = true;
      }
    };
  }
}
#endif //INDRI_FIELDBELOWWALKER_HPP
