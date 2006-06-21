/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

//
// IndriQueryLexer
//
// 17 February 2004 -- tds
//

header "pre_include_hpp" {
  #include "indri/QuerySpec.hpp"
  #include "indri/DateParse.hpp"
  #include "indri/delete_range.hpp"
  #include "indri/QueryLexer.hpp"
}

options {
  language = "Cpp";
  namespace = "indri::lang";
}

class QueryLexer extends Lexer;

options {
  k=2;
  charVocabulary = '\u0001'..'\u00ff'; // UTF-8 format
  testLiterals = false;
  defaultErrorHandler = false;
}

tokens {
  // core operators
  SUM = "#sum";
  WSUM = "#wsum";
  WAND = "#wand";
  OD = "#od";
  OR = "#or";
  NOT = "#not";
  UW = "#uw";
  COMBINE = "#combine";
  WEIGHT = "#weight";
  MAX = "#max";
  FILREQ = "#filreq";
  FILREJ = "#filrej";
  ANY = "#any";
  BAND = "#band";
  WSYN = "#wsyn";
  SYN = "#syn";
  // numerics
  PRIOR = "#prior";
  DATEAFTER = "#date:after";
  DATEBEFORE = "#date:before";
  DATEBETWEEN = "#date:between";
  LESS = "#less";
  GREATER = "#greater";
  BETWEEN = "#between";
  EQUALS = "#equals";
  
  // pseudo-tokens
  NUMBER;
  NEGATIVE_NUMBER;
  FLOAT;
}

{
private:
  bool _numbers;

public:
  void init() {
    _numbers = false;
  }

  void setNumbers(bool f) {
    _numbers = f;
  } 
} 

STAR:     '*';
O_PAREN:  '(';
C_PAREN:  ')';
O_ANGLE:  '<';
C_ANGLE:  '>';
O_SQUARE: '[';
C_SQUARE: ']';
O_BRACE:  '{';
C_BRACE:   '}';
DBL_QUOTE: '\"';
QUOTE:     '\'';
DOT:       '.';
COMMA:     ',';
SLASH:     '/';
B_SLASH:   '\\';

protected DASH:      '-';
COLON:     ':';

protected TAB:       '\t';
protected CR:        '\n';
protected LF:        '\r';
protected SPACE:     ' ';

protected HIGH_CHAR:         '\u0080'..'\u00ff';
protected DIGIT:             ('0'..'9');
protected ASCII_LETTER:      ('a'..'z' | 'A'..'Z');
protected SAFE_LETTER:       ('a'..'z' | 'A'..'Z' | '-' | '_');
protected SAFE_CHAR:         ('a'..'z' | 'A'..'Z' | '0'..'9' | '-' | '_');
protected BASESIXFOUR_CHAR:  ('a'..'z' | 'A'..'Z' | '0'..'9' | '+' | '/' | '=');

//
// Within the ASCII range, we only accept a restricted
// set of characters (letters and numbers).  However,
// we allow any Unicode character (composed of high
// chars) so that we can support UTF-8 input.
//

protected TEXT_TERM:        ( HIGH_CHAR | SAFE_CHAR )+;
protected NUMBER:           ( '0'..'9' )+;
protected NEGATIVE_NUMBER:  DASH ( '0'..'9' )+;
protected FLOAT:            (DASH)? ( '0'..'9' )+ DOT ( '0'..'9' )+;

TERM:     ( (DIGIT)+ (SAFE_LETTER | HIGH_CHAR) ) => TEXT_TERM |
          ( FLOAT ) => FLOAT { $setType(FLOAT); } |
          ( NUMBER ) => NUMBER { $setType(NUMBER); } |
          ( NEGATIVE_NUMBER ) => NEGATIVE_NUMBER { $setType(NEGATIVE_NUMBER); } |
          TEXT_TERM;
          
protected ENCODED_QUOTED_TERM:    "#base64quote"! O_PAREN! (TAB! | SPACE!)* (BASESIXFOUR_CHAR)+ (TAB! | SPACE!)* C_PAREN!;
protected ENCODED_TERM:           "#base64"! O_PAREN! (TAB! | SPACE!)* (BASESIXFOUR_CHAR)+ (TAB! | SPACE!)* C_PAREN!;

OPERATOR
  options { testLiterals = true; }:
    ( "#base64quote" ) => ENCODED_QUOTED_TERM { $setType(ENCODED_QUOTED_TERM); } |
    ( "#base64" ) => ENCODED_TERM { $setType(ENCODED_TERM); } |
    ('#' ASCII_LETTER) => '#' (ASCII_LETTER)* (COLON (ASCII_LETTER)+)? |
    '#';

JUNK:      ( TAB | CR | LF | SPACE )
           { $setType(antlr::Token::SKIP); };
     
class QueryParser extends Parser;
options {
  defaultErrorHandler = false;
}

{
private:
  // storage for allocated nodes
  std::vector<indri::lang::Node*> _nodes;
  // makes sure nodes go away when parser goes away
  indri::utility::VectorDeleter<indri::lang::Node*> _deleter;
    
  indri::lang::RawExtentNode * innerMost( indri::lang::ScoredExtentNode* sr ) {
    indri::lang::RawExtentNode * ou = 0;
    // set the new outer node we need to pass down (the innermost of field or field list of 
    // of this restriction)
    indri::lang::ExtentRestriction * er = dynamic_cast<indri::lang::ExtentRestriction *>(sr);
    if (er != 0) {
      indri::lang::RawExtentNode * f = er->getField();
      indri::lang::ExtentInside * ei = dynamic_cast<indri::lang::ExtentInside *>(f);
      while (ei != 0) {
        f = ei->getInner();
        ei = dynamic_cast<indri::lang::ExtentInside *>(f);
      }       
      ou = f;
    }
    return ou;         
  }
  
public:
  void init( QueryLexer* lexer ) {
    _deleter.setVector( _nodes );
  }
}

query returns [ indri::lang::ScoredExtentNode* q ] {
    indri::lang::CombineNode* c = 0;
    indri::lang::ScoredExtentNode* s = 0;
    q = 0;
  } :
  q=scoredExtentNode[0]
  ( options { greedy=true; } : s=scoredExtentNode[0]
    {
      c = new CombineNode;
      c->addChild(q);
      c->addChild(s);
      _nodes.push_back(c);
      q = c;
    }
    ( options { greedy=true; } : s=scoredExtentNode[0]
      {
        c->addChild(s);
      }
    )*
  )? EOF;

scoredExtentNode [ indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* s ] :
    ( WEIGHT ) => s=weightNode[ou]
  | ( COMBINE ) => s=combineNode[ou]
  | ( OR ) => s=orNode[ou]
  | ( NOT ) => s=notNode[ou]
  | ( WAND ) => s=wandNode[ou]
  | ( WSUM ) => s=wsumNode[ou]
  | ( MAX ) => s=maxNode[ou]
  | ( PRIOR ) => s=priorNode
  | ( FILREJ ) => s=filrejNode[ou]
  | ( FILREQ ) => s=filreqNode[ou]
  | s=scoredRaw[ou]
  ;

//
// There are 4 cases to scoredRaw when where really should only need to be 2,
// but for some reason I can't make the "( DOT context_list )?" work right.
//

scoredRaw [ indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* sn ]
  {
    RawExtentNode* raw = 0;
    RawExtentNode* contexts = 0;
    sn = 0;
  } :
    ( qualifiedTerm DOT ) => raw=qualifiedTerm DOT contexts=context_list[ou]
  {
    sn = new indri::lang::RawScorerNode( raw, contexts );
    _nodes.push_back(sn);
  }
  | ( qualifiedTerm ) => raw=qualifiedTerm
  {
    sn = new indri::lang::RawScorerNode( raw, contexts );
    _nodes.push_back(sn);
  }
  | ( unqualifiedTerm DOT ) => raw=unqualifiedTerm DOT contexts=context_list[ou]
  {
    sn = new indri::lang::RawScorerNode( raw, contexts );
    _nodes.push_back(sn);
  }
  | raw=unqualifiedTerm
  {
    sn = new indri::lang::RawScorerNode( raw, contexts );
    _nodes.push_back(sn);
  };

//
// Score operators start here:
//    #weight = weightNode
//    #combine = combineNode
//    #max = maxNode
//    #wsum = wsumNode
//    #wand = wandNode
//    #or = orNode
//    #not = notNode
//

weightedList[ indri::lang::WeightedCombinationNode* wn, indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* sr ] 
  {
    double w = 0;
    ScoredExtentNode* n = 0;
    sr = wn;
  } :
  ( sr=extentRestriction[wn, ou] { ou = innerMost(sr); } )? 
  O_PAREN 
    (
      w=floating
      n=scoredExtentNode[ou]
      { wn->addChild( w, n ); }
    )+
  C_PAREN
  ;

sumList[ indri::lang::WSumNode* wn, indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* sr ] 
  {
    double w = 0;
    ScoredExtentNode* n = 0;
    sr = wn;
  } :
  ( sr=extentRestriction[wn, ou] { ou = innerMost(sr); } )?
  O_PAREN 
    ( options { greedy=true; } : n=scoredExtentNode[ou] { wn->addChild( 1.0, n ); } )+
  C_PAREN
  ;

unweightedList[ indri::lang::UnweightedCombinationNode* cn, indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* sr ]
  {
    ScoredExtentNode* n = 0;
    sr = cn;
  } :
  ( sr=extentRestriction[cn, ou] { ou = innerMost(sr); } )?
  O_PAREN
    ( options { greedy=true; } : n=scoredExtentNode[ou] { cn->addChild( n ); } )+
  C_PAREN
  ;

weightNode [indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* r ] 
  {
    indri::lang::WeightNode* wn = new indri::lang::WeightNode;
    _nodes.push_back(wn);
  } :
  WEIGHT r=weightedList[wn, ou];

combineNode [indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* r ]
  { 
    indri::lang::CombineNode* cn = new indri::lang::CombineNode;
    _nodes.push_back(cn);
  } :
  COMBINE r=unweightedList[cn, ou];

sumNode [indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* r ]
  {
    indri::lang::WSumNode* wn = new indri::lang::WSumNode;
    _nodes.push_back(wn);
  } :
  SUM r=sumList[wn, ou];

wsumNode [indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* r ] 
  {
    indri::lang::WSumNode* wn = new indri::lang::WSumNode;
    _nodes.push_back(wn);
  } :
  WSUM r=weightedList[wn, ou];

wandNode [indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* r ]
  {
    indri::lang::WAndNode* wn = new indri::lang::WAndNode;
    _nodes.push_back(wn);
  } :
  WAND r=weightedList[wn, ou];
  
orNode [indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* r ]
  {
    indri::lang::OrNode* on = new indri::lang::OrNode;
    _nodes.push_back(on);
  } :
  OR r=unweightedList[on, ou];

maxNode [indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* r ]
  {
    indri::lang::MaxNode* mn = new indri::lang::MaxNode;
    _nodes.push_back(mn);
  } :
  MAX r=unweightedList[mn, ou];
  
notNode [indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* r ]
  {
    indri::lang::NotNode* n = new indri::lang::NotNode;
    indri::lang::ScoredExtentNode* c = 0;
    _nodes.push_back(n);
    r = n;
  } :
  NOT (r=extentRestriction[r, ou])? O_PAREN c=scoredExtentNode[ou] C_PAREN
  {
    n->setChild(c);
  };
  
priorNode returns [ indri::lang::PriorNode* p ]
  {
    indri::lang::Field* field = 0;
    p = 0;
  } :
  PRIOR O_PAREN name:TERM C_PAREN {
    p = new indri::lang::PriorNode( name->getText() );
    _nodes.push_back(p);
  };
  
//
// Extent operators start here:
//    #wsyn = wsynNode
//    #odn = odNode
//    #uwn = uwNode
//    #band = bandNode
//    #filrej = filrejNode
//    #filreq = filreqNode
//
  
// wsynNode : WSYN O_PAREN ( weight unscoredTerm )+ C_PAREN
wsynNode returns [ indri::lang::WeightedExtentOr* ws ]
  {
    ws = new indri::lang::WeightedExtentOr;
    _nodes.push_back(ws);

    double w = 0;
    RawExtentNode* n = 0;
  } :
  WSYN O_PAREN
       ( options { greedy=true; } : w=floating n=unscoredTerm { ws->addChild( w, n ); } )+
       C_PAREN;
  
// odNode : OD DECIMAL O_PAREN ( qualifiedTerm )+ C_PAREN
odNode returns [ indri::lang::ODNode* od ] 
  {
    RawExtentNode* rn = 0;
    od = new indri::lang::ODNode;
    _nodes.push_back(od);
  } :
  // operator
  (
    // #od5 syntax
      (OD NUMBER) => (OD n1:NUMBER  { od->setWindowSize( n1->getText() ); } )
    // #od( term ) syntax
    | (OD) => (OD)
    // #5 syntax
    | (OPERATOR n2:NUMBER { od->setWindowSize( n2->getText() ); } )
  )
  // contents
  O_PAREN
    ( options { greedy=true; } : rn=unscoredTerm { od->addChild( rn ); } )+
  C_PAREN;

// uwNode : UW DECIMAL O_PAREN ( qualifiedTerm )+ C_PAREN
uwNode returns [ indri::lang::UWNode* uw ]
  {
    uw = new indri::lang::UWNode;
    RawExtentNode* rn = 0;
    _nodes.push_back(uw);
  } :
  (
      // operator (#uw2)
      (UW NUMBER) => (UW n:NUMBER { uw->setWindowSize( n->getText() ); } )
      // operator #uw( term )
    | (UW)
  )
  // contents
  O_PAREN
    ( options { greedy=true; } : rn=unscoredTerm { uw->addChild( rn ); } )+
  C_PAREN;

bandNode returns [ indri::lang::BAndNode* b ] 
  {
    b = new indri::lang::BAndNode;
    RawExtentNode* rn = 0;
    _nodes.push_back(b);
  } :
  BAND
  O_PAREN
    ( options { greedy=true; } : rn=unscoredTerm { b->addChild( rn ); } )+
  C_PAREN;
  
filrejNode [ indri::lang::RawExtentNode * ou ] returns [ indri::lang::FilRejNode* fj ]
  {
    RawExtentNode* filter = 0;
    ScoredExtentNode* disallowed = 0;
  } :
  FILREJ
  O_PAREN filter=unscoredTerm disallowed=scoredExtentNode[ou] C_PAREN {
    fj = new FilRejNode( filter, disallowed );
    _nodes.push_back(fj);
  }; 
  
filreqNode[ indri::lang::RawExtentNode * ou ] returns [ indri::lang::FilReqNode* fq ]
  {
    RawExtentNode* filter = 0;
    ScoredExtentNode* required = 0;
  } :
  FILREQ
  O_PAREN filter=unscoredTerm required=scoredExtentNode[ou] C_PAREN {
    fq = new FilReqNode( filter, required );
    _nodes.push_back(fq);
  }; 
 
anyField returns [ indri::lang::Field* f ]
  {
    f = 0;
  } :
  ANY COLON t:TERM {
    f = new Field(t->getText());
    _nodes.push_back(f);
  };

unscoredTerm returns [ RawExtentNode* t ]
  {
    t = 0;
  } :
    ( qualifiedTerm ) => t=qualifiedTerm
  | t=unqualifiedTerm;
  
qualifiedTerm returns [ RawExtentNode* t ] 
  {
    RawExtentNode* synonyms = 0;
    RawExtentNode* fields = 0;
    t = 0;
  } :
  synonyms=unqualifiedTerm DOT fields=field_list
  {
    if( fields ) {
      t = new indri::lang::ExtentInside( synonyms, fields );
      _nodes.push_back(t);
      synonyms = t;
    } else {
      t = synonyms;
    }
  };

unqualifiedTerm returns [ indri::lang::RawExtentNode* re ] :
    ( OD ) => re=odNode
  | ( UW ) => re=uwNode
  | ( BAND ) => re=bandNode
  | ( DATEBEFORE ) => re=dateBefore
  | ( DATEAFTER ) => re=dateAfter
  | ( DATEBETWEEN ) => re=dateBetween
  | ( O_ANGLE ) => re=synonym_list
  | ( O_BRACE ) => re=synonym_list_brace
  | ( SYN ) => re=synonym_list_alt
  | ( WSYN ) => re=wsynNode
  | ( ANY ) => re=anyField
  | ( LESS ) => re=lessNode
  | ( GREATER ) => re=greaterNode
  | ( BETWEEN ) => re=betweenNode
  | ( EQUALS ) => re=equalsNode
  | re=rawText;
          
extentRestriction [ indri::lang::ScoredExtentNode* sn, indri::lang::RawExtentNode * ou ] returns [ indri::lang::ScoredExtentNode* er ] {
    indri::lang::Field* f = 0;
    er = 0;
    indri::lang::ExtentInside * po = 0;
  } :
  ( O_SQUARE TERM COLON ) => O_SQUARE passageWindowSize:TERM COLON inc:NUMBER C_SQUARE
  {
    int startWindow;

    for( startWindow = 0; startWindow < passageWindowSize->getText().size(); startWindow++ ) {
      if( isdigit( passageWindowSize->getText()[startWindow] ) )
        break;
    }
  
    int increment = atoi(inc->getText().c_str());
    int windowSize = atoi(passageWindowSize->getText().c_str() + startWindow );
    
    er = new indri::lang::FixedPassage(sn, windowSize, increment);
  } |
  ( O_SQUARE TERM ) =>  O_SQUARE field:TERM C_SQUARE
  {
    f = new indri::lang::Field(field->getText());
    _nodes.push_back(f);
    er = new indri::lang::ExtentRestriction(sn, f);
    _nodes.push_back(er);
  } |
  O_SQUARE DOT po=path C_SQUARE 
  {

    if ( ou == 0 ) {
      throw new antlr::SemanticException("Use of a . in a extent restriction without a valid outer context.");
    }
    po->setOuter(ou);
    er = new indri::lang::ExtentRestriction(sn, po);
    _nodes.push_back(er);
  }
;

path returns [ indri::lang::ExtentInside* r ] {
    r = 0;
    indri::lang::Field * f = 0;
    indri::lang::ExtentInside * po = 0;
    indri::lang::ExtentInside * lastPo = 0;
} :
  (options{greedy=true;} : po=pathOperator fieldRestricted:TERM {
      if ( r == 0 ) {  // set the root
        r = po;
      }      
      f = new indri::lang::Field(fieldRestricted->getText());
      _nodes.push_back(f);      
      po->setInner(f);  // set the leaf's inner
      if ( lastPo != 0 ) {  // add this operator into the chain
        po->setOuter( lastPo->getInner() );
        lastPo->setInner( po );
      }
      lastPo = po;
    }
  )+
;

pathOperator returns [ indri::lang::ExtentInside* r ] {
    r = 0;
    indri::lang::DocumentStructureNode * ds = 0;
  } :
  SLASH (SLASH {    
    ds = new indri::lang::DocumentStructureNode;
    _nodes.push_back(ds);
    r = new indri::lang::ExtentDescendant(NULL, NULL, ds);
    _nodes.push_back(r);
  })? { 
    if (r == 0) {
      ds = new indri::lang::DocumentStructureNode;
      _nodes.push_back(ds);
      r = new indri::lang::ExtentChild(NULL, NULL, ds);
      _nodes.push_back(r);
    }
  } | 
  B_SLASH {
   ds = new indri::lang::DocumentStructureNode;
   _nodes.push_back(ds);
   r = new indri::lang::ExtentParent(NULL, NULL, ds);
   _nodes.push_back(r);
  } |
  O_BRACE {
    r = new indri::lang::ExtentInside(NULL, NULL);   
    _nodes.push_back(r);
  }
;        


        
synonym_list returns [ indri::lang::ExtentOr* s ] {
    indri::lang::RawExtentNode* term = 0;
    s = new indri::lang::ExtentOr;
    _nodes.push_back(s);
  } :
  O_ANGLE
    ( options { greedy=true; }: term=unscoredTerm { s->addChild(term); } )+
  C_ANGLE;

synonym_list_brace returns [ indri::lang::ExtentOr* s ] {
    indri::lang::RawExtentNode* term = 0;
    s = new indri::lang::ExtentOr;
    _nodes.push_back(s);
  } :
  O_BRACE
    ( options { greedy=true; }: term=unscoredTerm { s->addChild(term); } )+
  C_BRACE;

synonym_list_alt returns [ indri::lang::ExtentOr* s ] {
    indri::lang::RawExtentNode* term = 0;
    // semantics of this node will change
    s = new indri::lang::ExtentOr;
    _nodes.push_back(s);
  } :
  SYN
  O_PAREN
    ( options { greedy=true; }: term=unscoredTerm { s->addChild(term); } )+
  C_PAREN;

field_list returns [ indri::lang::ExtentAnd* fields ]
  { 
    fields = new ExtentAnd;
    _nodes.push_back( fields );
  } :
  // first field
  first:TERM {
    Field* firstField = new indri::lang::Field( first->getText() );
    _nodes.push_back( firstField );
    fields->addChild( firstField );
  }
  // additional fields
  ( options { greedy=true; } :
      COMMA additional:TERM {
        Field* additionalField = new Field(additional->getText());
        _nodes.push_back( additionalField );
        fields->addChild( additionalField );
      }
  )*;

context_list [ indri::lang::RawExtentNode * ou ] returns [ ExtentOr* contexts ] {
    contexts = new ExtentOr;
    _nodes.push_back( contexts );
    indri::lang::ExtentInside * p = 0;
    indri::lang::ExtentInside * pAdditional = 0;
  } :
  O_PAREN
  // first field
  (first:TERM {
    Field* firstField = new indri::lang::Field( first->getText() );
    _nodes.push_back( firstField );
    contexts->addChild( firstField );
  }
  | 
  (DOT p=path {
    p->setOuter( ou );
    contexts->addChild( p );
  }))
  // additional fields
  ( options { greedy=true; } :
      COMMA ( additional:TERM {
        Field* additionalField = new Field(additional->getText());
        _nodes.push_back( additionalField );
        contexts->addChild( additionalField );
      } |
      (DOT pAdditional=path {
          pAdditional->setOuter( ou );
          contexts->addChild( pAdditional );
       }))
  )*
  C_PAREN;



field_restriction returns [ indri::lang::Field* extent ] :
  O_SQUARE
  fieldName:TERM {
    extent = new Field( fieldName->getText() );
    _nodes.push_back( extent );
  }
  C_SQUARE;

dateBefore returns [ indri::lang::FieldLessNode* extent ] {
    UINT64 d = 0;
    Field* dateField = 0;
    extent = 0;
  } :
  DATEBEFORE O_PAREN d=date C_PAREN {
    dateField = new Field("date");
    extent = new FieldLessNode( dateField, d );
    _nodes.push_back( dateField );
    _nodes.push_back( extent );
  };
  
dateAfter returns [ indri::lang::FieldGreaterNode* extent ] {
    UINT64 d = 0;
    Field* dateField = 0;
    extent = 0;
  } :
  DATEAFTER O_PAREN d=date C_PAREN {
    dateField = new Field("date");
    extent = new FieldGreaterNode( dateField, d );
    _nodes.push_back( dateField );
    _nodes.push_back( extent );
  };
  
dateBetween returns [ indri::lang::FieldBetweenNode* extent ] {
    UINT64 low = 0;
    UINT64 high = 0;
    Field* dateField = 0;
    extent = 0;
  } :
  DATEBETWEEN O_PAREN low=date COMMA high=date C_PAREN {
    dateField = new Field("date");
    extent = new FieldBetweenNode( dateField, low, high );
    _nodes.push_back( dateField );
    _nodes.push_back( extent );
  };

//
// want to handle the following date strings:
//  11 january 2004
//  11-JAN-04
//  11-JAN-2004
//  January 11 2004
//  01/11/04
//  01/11/2004
// also the following times:
//  4:25 pm
//  4:25p
//  16:25
//  16:25:00
//

date returns [ UINT64 d ] :
  ( NUMBER SLASH ) => d=slashDate |
  ( NUMBER ) => d=spaceDate |
  d=dashDate;
  
dashDate returns [ UINT64 d ] {
    d = 0;
  } :
  dmy:TERM {
    const std::string& text = dmy->getText();
    int firstDash = text.find('-');
    int secondDash = text.find('-', firstDash+1);
    std::string day = text.substr( 0, firstDash ); 
    std::string month = text.substr( firstDash+1, secondDash-firstDash-1 );
    std::string year = text.substr( secondDash );

    d = indri::parse::DateParse::convertDate( year, month, day ); 
  };
  
slashDate returns [ UINT64 d ] {
    d = 0;
  } :
  month:NUMBER SLASH day:TERM SLASH year:NUMBER {
    d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() ); 
  };
  
spaceDate returns [ UINT64 d ] {
    d = 0;
  } :
  day:NUMBER month:TERM year:NUMBER {
    d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() );
  };

// rawText is something that can be considered a query term
rawText returns [ indri::lang::IndexTerm* t ] {
    t = 0;
  } :
  id:TERM {
    t = new indri::lang::IndexTerm(id->getText());
    _nodes.push_back(t);
  } |
  n:NUMBER {
    t = new indri::lang::IndexTerm(n->getText());
    _nodes.push_back(t);
  } |
  nn:NEGATIVE_NUMBER {
    t = new indri::lang::IndexTerm(nn->getText());
    _nodes.push_back(t);
  } |
  f:FLOAT {
    t = new indri::lang::IndexTerm(f->getText());
    _nodes.push_back(t);
  } |
  DBL_QUOTE t=rawText DBL_QUOTE {
    // if a text term appears in quotes, consider it stemmed
    t->setStemmed(true);
  } |
  et:ENCODED_TERM {
    std::string decodedString; 
    base64_decode_string(decodedString, et->getText());
    t = new indri::lang::IndexTerm( decodedString );
    _nodes.push_back(t);
  } |
  qet:ENCODED_QUOTED_TERM {
    std::string decodedString; 
    base64_decode_string(decodedString, qet->getText());
    t = new indri::lang::IndexTerm( decodedString );
    t->setStemmed(true);
    _nodes.push_back(t);
  };

// A "floating" is either an integer or a number with a decimal point followed by a number.
// For instance, 1 and 1.0 are floating, but 1. is not.  
floating returns [ double d ] {
    d = 0;
  } :
  f:FLOAT {
    d = atof(f->getText().c_str());
  } |
  n:NUMBER {
    d = atof(n->getText().c_str());
  };

number returns [ INT64 v ] {
    v = 0;
  } :
  n:NUMBER {
    v = string_to_i64(n->getText());
  } |
  nn:NEGATIVE_NUMBER {
    v = string_to_i64(nn->getText());
  };

greaterNode returns [ indri::lang::FieldGreaterNode* gn ] {
    gn = 0;
    Field* compareField = 0;
    INT64 low = 0;
  } :
  GREATER O_PAREN field:TERM low=number C_PAREN {
    compareField = new Field(field->getText());
    gn = new FieldGreaterNode( compareField, low );
    _nodes.push_back( compareField );
    _nodes.push_back( gn );
  };
  
lessNode returns [ indri::lang::FieldLessNode* ln ] {
    ln = 0;
    Field* compareField = 0;
    INT64 high = 0;
  } :
  LESS O_PAREN field:TERM high=number C_PAREN {
    compareField = new Field(field->getText());
    ln = new FieldLessNode( compareField, high );
    _nodes.push_back( compareField );
    _nodes.push_back( ln );
  };

betweenNode returns [ indri::lang::FieldBetweenNode* bn ] {
    bn = 0;
    Field* compareField = 0;
    INT64 low = 0;
    INT64 high = 0;
  } :
  BETWEEN O_PAREN field:TERM low=number high=number C_PAREN {
    compareField = new Field(field->getText());
    bn = new FieldBetweenNode( compareField, low, high );
    _nodes.push_back( compareField );
    _nodes.push_back( bn );
  };

equalsNode returns [ indri::lang::FieldEqualsNode* en ] {
    en = 0;
    Field* compareField = 0;
    INT64 eq = 0;
  } :
  EQUALS O_PAREN field:TERM eq=number C_PAREN {
    compareField = new Field(field->getText());
    en = new FieldEqualsNode( compareField, eq );
    _nodes.push_back( compareField );
    _nodes.push_back( en );
  };

