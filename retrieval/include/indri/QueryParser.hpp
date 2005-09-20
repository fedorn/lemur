#ifndef INC_QueryParser_hpp_
#define INC_QueryParser_hpp_

#line 18 "indrilang.g"

  #include "indri/QuerySpec.hpp"
  #include "indri/DateParse.hpp"
  #include "indri/delete_range.hpp"
  #include "indri/QueryLexer.hpp"

#line 12 "QueryParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.4: "indrilang.g" -> "QueryParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "QueryLexerTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

ANTLR_BEGIN_NAMESPACE(indri)
ANTLR_BEGIN_NAMESPACE(lang)
class CUSTOM_API QueryParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public QueryLexerTokenTypes
{
#line 151 "indrilang.g"

private:
  // storage for allocated nodes
  std::vector<indri::lang::Node*> _nodes;
  // makes sure nodes go away when parser goes away
  indri::utility::VectorDeleter<indri::lang::Node*> _deleter;
  
public:
  void init( QueryLexer* lexer ) {
    _deleter.setVector( _nodes );
  }
#line 23 "QueryParser.hpp"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	QueryParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	QueryParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	QueryParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	QueryParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	QueryParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return QueryParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return QueryParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return QueryParser::tokenNames;
	}
	public:  indri::lang::ScoredExtentNode*  query();
	public:  indri::lang::ScoredExtentNode*  scoredExtentNode();
	public:  indri::lang::ScoredExtentNode*  weightNode();
	public:  indri::lang::ScoredExtentNode*  combineNode();
	public:  indri::lang::ScoredExtentNode*  orNode();
	public:  indri::lang::ScoredExtentNode*  notNode();
	public:  indri::lang::ScoredExtentNode*  wandNode();
	public:  indri::lang::ScoredExtentNode*  wsumNode();
	public:  indri::lang::ScoredExtentNode*  maxNode();
	public:  indri::lang::PriorNode*  priorNode();
	public:  indri::lang::FilRejNode*  filrejNode();
	public:  indri::lang::FilReqNode*  filreqNode();
	public:  indri::lang::ScoredExtentNode*  scoredRaw();
	public:  RawExtentNode*  qualifiedTerm();
	public:  ExtentOr*  context_list();
	public:  indri::lang::RawExtentNode*  unqualifiedTerm();
	public:  indri::lang::ScoredExtentNode*  weightedList(
		 indri::lang::WeightedCombinationNode* wn 
	);
	public:  indri::lang::ScoredExtentNode*  extentRestriction(
		 indri::lang::ScoredExtentNode* sn 
	);
	public:  double  floating();
	public:  indri::lang::ScoredExtentNode*  sumList(
		 indri::lang::WSumNode* wn 
	);
	public:  indri::lang::ScoredExtentNode*  unweightedList(
		 indri::lang::UnweightedCombinationNode* cn 
	);
	public:  indri::lang::ScoredExtentNode*  sumNode();
	public:  indri::lang::WeightedExtentOr*  wsynNode();
	public:  RawExtentNode*  unscoredTerm();
	public:  indri::lang::ODNode*  odNode();
	public:  indri::lang::UWNode*  uwNode();
	public:  indri::lang::BAndNode*  bandNode();
	public:  indri::lang::Field*  anyField();
	public:  indri::lang::ExtentAnd*  field_list();
	public:  indri::lang::FieldLessNode*  dateBefore();
	public:  indri::lang::FieldGreaterNode*  dateAfter();
	public:  indri::lang::FieldBetweenNode*  dateBetween();
	public:  indri::lang::ExtentOr*  synonym_list();
	public:  indri::lang::ExtentOr*  synonym_list_brace();
	public:  indri::lang::ExtentOr*  synonym_list_alt();
	public:  indri::lang::FieldLessNode*  lessNode();
	public:  indri::lang::FieldGreaterNode*  greaterNode();
	public:  indri::lang::FieldBetweenNode*  betweenNode();
	public:  indri::lang::FieldEqualsNode*  equalsNode();
	public:  indri::lang::IndexTerm*  rawText();
	public:  indri::lang::Field*  field_restriction();
	public:  UINT64  date();
	public:  UINT64  slashDate();
	public:  UINT64  spaceDate();
	public:  UINT64  dashDate();
	public:  INT64  number();
public:
	ANTLR_USE_NAMESPACE(antlr)RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	ANTLR_USE_NAMESPACE(antlr)RefAST returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 63;
#else
	enum {
		NUM_TOKENS = 63
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
};

ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
#endif /*INC_QueryParser_hpp_*/
