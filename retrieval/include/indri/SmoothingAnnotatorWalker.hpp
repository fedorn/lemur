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
// SmoothingAnnotatorWalker
//
// 27 April 2004 -- tds
//

#ifndef INDRI_SMOOTHINGANNOTATORWALKER_HPP
#define INDRI_SMOOTHINGANNOTATORWALKER_HPP

#include "indri/Parameters.hpp"

class SmoothingAnnotatorWalker : public indri::lang::Walker {
private:
  struct rule_type {
    std::string field;
    std::string op;
    std::string smoothing;
  };

  std::vector<rule_type*> _rules;
  std::string _defaultSmoothing;

  void _loadSmoothingRules( Parameters& parameters ) {
    if( !parameters.exists("rule") )
      return;

    Parameters rules = parameters["rule"];

    for(size_t i=0; i<rules.size(); i++) {
      std::string ruleText = rules[i];

      int nextComma = 0;
      int nextColon = 0;
      int location = 0;

      rule_type* rule = new rule_type;
      rule->op = "*";
      rule->field = "*";

      for( location = 0; location < ruleText.length(); ) {
        nextComma = ruleText.find( ',', location );
        nextColon = ruleText.find( ':', location );

        std::string key = ruleText.substr( location, nextColon-location );
        std::string value = ruleText.substr( nextColon+1, nextComma-nextColon-1 );

        if( key == "field" ) {
          rule->field = value;
        } else if( key == "operator" ) {
          rule->op = value;
        } else {
          if( rule->smoothing.size() ) rule->smoothing += ",";
          rule->smoothing += key + ":" + value;
        }

        if( nextComma > 0 )
          location = nextComma+1;
        else
          location = ruleText.size();
      }

      _rules.push_back(rule);
    }
  }

  const std::string& _matchSmoothingRule( const std::string& field, const std::string& op ) {
    for( int i=signed(_rules.size())-1; i >= 0; i-- ) {
      const rule_type& rule = *_rules[i];

      if( ( rule.field == field || rule.field == "*" ) &&
        ( rule.op == op || rule.op == "*" ) ) {
        return rule.smoothing;
      }
    }

    return _defaultSmoothing;
  }

public:
  SmoothingAnnotatorWalker( Parameters& parameters ) {
    _loadSmoothingRules( parameters );
    _defaultSmoothing = "method:dirichlet,mu:2500";
  }

  ~SmoothingAnnotatorWalker( ) {
    delete_vector_contents<rule_type*>( _rules );
  }

  void after( indri::lang::RawScorerNode* scorer ) {
    indri::lang::Node* context = scorer->getContext();
    indri::lang::Field* contextField = dynamic_cast<indri::lang::Field*>(context);
    indri::lang::ExtentAnd* contextExtAnd = dynamic_cast<indri::lang::ExtentAnd*>(context);
    std::string fieldName;

    // there may be an ExtentAnd around the field, so descend into it if necessary
    if( contextExtAnd && contextExtAnd->getChildren().size() == 1 ) {
      contextField = dynamic_cast<indri::lang::Field*>(contextExtAnd->getChildren()[0]);
    }

    // if there's a field here, record its name
    if( contextField ) {
      fieldName = contextField->getFieldName();
    } else {
      fieldName = "?";
    }
    
    indri::lang::Node* raw = scorer->getRawExtent();
    indri::lang::Node* rawTerm = dynamic_cast<indri::lang::IndexTerm*>(raw);
    indri::lang::Node* rawODNode = dynamic_cast<indri::lang::ODNode*>(raw);
    indri::lang::Node* rawUWNode = dynamic_cast<indri::lang::UWNode*>(raw);
    std::string op;

    if( rawODNode || rawUWNode ) {
      op = "window";
    } else if( rawTerm ) {
      op = "term";
    } else {
      op = "?";
    }

    scorer->setSmoothing( _matchSmoothingRule( fieldName, op ) );
  }
};

#endif // INDRI_SMOOTHINGANNOTATORWALKER_HPP

