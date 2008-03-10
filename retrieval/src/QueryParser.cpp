/* $ANTLR 2.7.3 (20060307-1): "indrilang.g" -> "QueryParser.cpp"$ */
#include "indri/QueryParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
ANTLR_BEGIN_NAMESPACE(indri)
ANTLR_BEGIN_NAMESPACE(lang)
#line 1 "indrilang.g"
#line 8 "QueryParser.cpp"
QueryParser::QueryParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

QueryParser::QueryParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,2)
{
}

QueryParser::QueryParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

QueryParser::QueryParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,2)
{
}

QueryParser::QueryParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,2)
{
}

 indri::lang::ScoredExtentNode*  QueryParser::query() {
#line 189 "indrilang.g"
	 indri::lang::ScoredExtentNode* q ;
#line 37 "QueryParser.cpp"
#line 189 "indrilang.g"
	
	indri::lang::CombineNode* c = 0;
	indri::lang::ScoredExtentNode* s = 0;
	q = 0;
	
#line 44 "QueryParser.cpp"
	
	q=scoredExtentNode(0);
	{
	switch ( LA(1)) {
	case WSUM:
	case WAND:
	case OD:
	case OR:
	case NOT:
	case UW:
	case COMBINE:
	case WEIGHT:
	case MAX:
	case FILREQ:
	case FILREJ:
	case SCOREIF:
	case SCOREIFNOT:
	case ANY:
	case BAND:
	case WSYN:
	case SYN:
	case PRIOR:
	case DATEAFTER:
	case DATEBEFORE:
	case DATEBETWEEN:
	case DATEEQUALS:
	case LESS:
	case GREATER:
	case BETWEEN:
	case EQUALS:
	case WCARD:
	case NUMBER:
	case NEGATIVE_NUMBER:
	case FLOAT:
	case O_ANGLE:
	case O_BRACE:
	case DBL_QUOTE:
	case TERM:
	case ENCODED_QUOTED_TERM:
	case ENCODED_TERM:
	case OPERATOR:
	{
		s=scoredExtentNode(0);
		if ( inputState->guessing==0 ) {
#line 196 "indrilang.g"
			
			c = new CombineNode;
			c->addChild(q);
			c->addChild(s);
			_nodes.push_back(c);
			q = c;
			
#line 97 "QueryParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				s=scoredExtentNode(0);
				if ( inputState->guessing==0 ) {
#line 204 "indrilang.g"
					
					c->addChild(s);
					
#line 108 "QueryParser.cpp"
				}
			}
			else {
				goto _loop88;
			}
			
		}
		_loop88:;
		} // ( ... )*
		break;
	}
	case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE);
	return q ;
}

 indri::lang::ScoredExtentNode*  QueryParser::scoredExtentNode(
	 indri::lang::RawExtentNode * ou 
) {
#line 210 "indrilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 139 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case WEIGHT:
	{
		s=weightNode(ou);
		break;
	}
	case COMBINE:
	{
		s=combineNode(ou);
		break;
	}
	case OR:
	{
		s=orNode(ou);
		break;
	}
	case NOT:
	{
		s=notNode(ou);
		break;
	}
	case WAND:
	{
		s=wandNode(ou);
		break;
	}
	case WSUM:
	{
		s=wsumNode(ou);
		break;
	}
	case MAX:
	{
		s=maxNode(ou);
		break;
	}
	case PRIOR:
	{
		s=priorNode();
		break;
	}
	case FILREJ:
	{
		s=filrejNode(ou);
		break;
	}
	case FILREQ:
	{
		s=filreqNode(ou);
		break;
	}
	case SCOREIF:
	{
		s=scoreifNode(ou);
		break;
	}
	case SCOREIFNOT:
	{
		s=scoreifnotNode(ou);
		break;
	}
	case OD:
	case UW:
	case ANY:
	case BAND:
	case WSYN:
	case SYN:
	case DATEAFTER:
	case DATEBEFORE:
	case DATEBETWEEN:
	case DATEEQUALS:
	case LESS:
	case GREATER:
	case BETWEEN:
	case EQUALS:
	case WCARD:
	case NUMBER:
	case NEGATIVE_NUMBER:
	case FLOAT:
	case O_ANGLE:
	case O_BRACE:
	case DBL_QUOTE:
	case TERM:
	case ENCODED_QUOTED_TERM:
	case ENCODED_TERM:
	case OPERATOR:
	{
		s=scoredRaw(ou);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return s ;
}

 indri::lang::ScoredExtentNode*  QueryParser::weightNode(
	indri::lang::RawExtentNode * ou 
) {
#line 303 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 244 "QueryParser.cpp"
#line 303 "indrilang.g"
	
	indri::lang::WeightNode* wn = new indri::lang::WeightNode;
	_nodes.push_back(wn);
	
#line 250 "QueryParser.cpp"
	
	match(WEIGHT);
	r=weightedList(wn, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::combineNode(
	indri::lang::RawExtentNode * ou 
) {
#line 310 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 262 "QueryParser.cpp"
#line 310 "indrilang.g"
	
	indri::lang::CombineNode* cn = new indri::lang::CombineNode;
	_nodes.push_back(cn);
	
#line 268 "QueryParser.cpp"
	
	match(COMBINE);
	r=unweightedList(cn, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::orNode(
	indri::lang::RawExtentNode * ou 
) {
#line 338 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 280 "QueryParser.cpp"
#line 338 "indrilang.g"
	
	indri::lang::OrNode* on = new indri::lang::OrNode;
	_nodes.push_back(on);
	
#line 286 "QueryParser.cpp"
	
	match(OR);
	r=unweightedList(on, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::notNode(
	indri::lang::RawExtentNode * ou 
) {
#line 352 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 298 "QueryParser.cpp"
#line 352 "indrilang.g"
	
	indri::lang::NotNode* n = new indri::lang::NotNode;
	indri::lang::ScoredExtentNode* c = 0;
	_nodes.push_back(n);
	r = n;
	
#line 306 "QueryParser.cpp"
	
	match(NOT);
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		r=extentRestriction(r, ou);
		break;
	}
	case O_PAREN:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(O_PAREN);
	c=scoredExtentNode(ou);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 360 "indrilang.g"
		
		n->setChild(c);
		
#line 334 "QueryParser.cpp"
	}
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::wandNode(
	indri::lang::RawExtentNode * ou 
) {
#line 331 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 344 "QueryParser.cpp"
#line 331 "indrilang.g"
	
	indri::lang::WAndNode* wn = new indri::lang::WAndNode;
	_nodes.push_back(wn);
	
#line 350 "QueryParser.cpp"
	
	match(WAND);
	r=weightedList(wn, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::wsumNode(
	indri::lang::RawExtentNode * ou 
) {
#line 324 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 362 "QueryParser.cpp"
#line 324 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 368 "QueryParser.cpp"
	
	match(WSUM);
	r=weightedList(wn, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::maxNode(
	indri::lang::RawExtentNode * ou 
) {
#line 345 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 380 "QueryParser.cpp"
#line 345 "indrilang.g"
	
	indri::lang::MaxNode* mn = new indri::lang::MaxNode;
	_nodes.push_back(mn);
	
#line 386 "QueryParser.cpp"
	
	match(MAX);
	r=unweightedList(mn, ou);
	return r ;
}

 indri::lang::PriorNode*  QueryParser::priorNode() {
#line 364 "indrilang.g"
	 indri::lang::PriorNode* p ;
#line 396 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 364 "indrilang.g"
	
	indri::lang::Field* field = 0;
	p = 0;
	
#line 403 "QueryParser.cpp"
	
	match(PRIOR);
	match(O_PAREN);
	name = LT(1);
	match(TERM);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 369 "indrilang.g"
		
		p = new indri::lang::PriorNode( name->getText() );
		_nodes.push_back(p);
		
#line 416 "QueryParser.cpp"
	}
	return p ;
}

 indri::lang::FilRejNode*  QueryParser::filrejNode(
	 indri::lang::RawExtentNode * ou 
) {
#line 447 "indrilang.g"
	 indri::lang::FilRejNode* fj ;
#line 426 "QueryParser.cpp"
#line 447 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* disallowed = 0;
	
#line 432 "QueryParser.cpp"
	
	match(FILREJ);
	match(O_PAREN);
	filter=unscoredTerm();
	disallowed=scoredExtentNode(ou);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 453 "indrilang.g"
		
		fj = new FilRejNode( filter, disallowed );
		_nodes.push_back(fj);
		
#line 445 "QueryParser.cpp"
	}
	return fj ;
}

 indri::lang::FilReqNode*  QueryParser::filreqNode(
	 indri::lang::RawExtentNode * ou 
) {
#line 458 "indrilang.g"
	 indri::lang::FilReqNode* fq ;
#line 455 "QueryParser.cpp"
#line 458 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* required = 0;
	
#line 461 "QueryParser.cpp"
	
	match(FILREQ);
	match(O_PAREN);
	filter=unscoredTerm();
	required=scoredExtentNode(ou);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 464 "indrilang.g"
		
		fq = new FilReqNode( filter, required );
		_nodes.push_back(fq);
		
#line 474 "QueryParser.cpp"
	}
	return fq ;
}

 indri::lang::FilReqNode*  QueryParser::scoreifNode(
	 indri::lang::RawExtentNode * ou 
) {
#line 480 "indrilang.g"
	 indri::lang::FilReqNode* fq ;
#line 484 "QueryParser.cpp"
#line 480 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* required = 0;
	
#line 490 "QueryParser.cpp"
	
	match(SCOREIF);
	match(O_PAREN);
	filter=unscoredTerm();
	required=scoredExtentNode(ou);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 486 "indrilang.g"
		
		fq = new FilReqNode( filter, required );
		_nodes.push_back(fq);
		
#line 503 "QueryParser.cpp"
	}
	return fq ;
}

 indri::lang::FilRejNode*  QueryParser::scoreifnotNode(
	 indri::lang::RawExtentNode * ou 
) {
#line 469 "indrilang.g"
	 indri::lang::FilRejNode* fj ;
#line 513 "QueryParser.cpp"
#line 469 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* disallowed = 0;
	
#line 519 "QueryParser.cpp"
	
	match(SCOREIFNOT);
	match(O_PAREN);
	filter=unscoredTerm();
	disallowed=scoredExtentNode(ou);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 475 "indrilang.g"
		
		fj = new FilRejNode( filter, disallowed );
		_nodes.push_back(fj);
		
#line 532 "QueryParser.cpp"
	}
	return fj ;
}

 indri::lang::ScoredExtentNode*  QueryParser::scoredRaw(
	 indri::lang::RawExtentNode * ou 
) {
#line 226 "indrilang.g"
	 indri::lang::ScoredExtentNode* sn ;
#line 542 "QueryParser.cpp"
#line 226 "indrilang.g"
	
	RawExtentNode* raw = 0;
	RawExtentNode* contexts = 0;
	sn = 0;
	
#line 549 "QueryParser.cpp"
	
	bool synPredMatched116 = false;
	if (((_tokenSet_1.member(LA(1))) && (_tokenSet_2.member(LA(2))))) {
		int _m116 = mark();
		synPredMatched116 = true;
		inputState->guessing++;
		try {
			{
			qualifiedTerm();
			match(DOT);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched116 = false;
		}
		rewind(_m116);
		inputState->guessing--;
	}
	if ( synPredMatched116 ) {
		raw=qualifiedTerm();
		match(DOT);
		contexts=context_list(ou);
		if ( inputState->guessing==0 ) {
#line 233 "indrilang.g"
			
			sn = new indri::lang::RawScorerNode( raw, contexts );
			_nodes.push_back(sn);
			
#line 578 "QueryParser.cpp"
		}
	}
	else {
		bool synPredMatched118 = false;
		if (((_tokenSet_1.member(LA(1))) && (_tokenSet_2.member(LA(2))))) {
			int _m118 = mark();
			synPredMatched118 = true;
			inputState->guessing++;
			try {
				{
				qualifiedTerm();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched118 = false;
			}
			rewind(_m118);
			inputState->guessing--;
		}
		if ( synPredMatched118 ) {
			raw=qualifiedTerm();
			if ( inputState->guessing==0 ) {
#line 238 "indrilang.g"
				
				sn = new indri::lang::RawScorerNode( raw, contexts );
				_nodes.push_back(sn);
				
#line 606 "QueryParser.cpp"
			}
		}
		else {
			bool synPredMatched120 = false;
			if (((_tokenSet_1.member(LA(1))) && (_tokenSet_2.member(LA(2))))) {
				int _m120 = mark();
				synPredMatched120 = true;
				inputState->guessing++;
				try {
					{
					unqualifiedTerm();
					match(DOT);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched120 = false;
				}
				rewind(_m120);
				inputState->guessing--;
			}
			if ( synPredMatched120 ) {
				raw=unqualifiedTerm();
				match(DOT);
				contexts=context_list(ou);
				if ( inputState->guessing==0 ) {
#line 243 "indrilang.g"
					
					sn = new indri::lang::RawScorerNode( raw, contexts );
					_nodes.push_back(sn);
					
#line 637 "QueryParser.cpp"
				}
			}
			else if ((_tokenSet_1.member(LA(1))) && (_tokenSet_3.member(LA(2)))) {
				raw=unqualifiedTerm();
				if ( inputState->guessing==0 ) {
#line 248 "indrilang.g"
					
					sn = new indri::lang::RawScorerNode( raw, contexts );
					_nodes.push_back(sn);
					
#line 648 "QueryParser.cpp"
				}
			}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}}
	return sn ;
}

 RawExtentNode*  QueryParser::qualifiedTerm() {
#line 511 "indrilang.g"
	 RawExtentNode* t ;
#line 661 "QueryParser.cpp"
#line 511 "indrilang.g"
	
	RawExtentNode* synonyms = 0;
	RawExtentNode* fields = 0;
	t = 0;
	
#line 668 "QueryParser.cpp"
	
	synonyms=unqualifiedTerm();
	match(DOT);
	fields=field_list();
	if ( inputState->guessing==0 ) {
#line 518 "indrilang.g"
		
		if( fields ) {
		t = new indri::lang::ExtentInside( synonyms, fields );
		_nodes.push_back(t);
		synonyms = t;
		} else {
		t = synonyms;
		}
		
#line 684 "QueryParser.cpp"
	}
	return t ;
}

 ExtentOr*  QueryParser::context_list(
	 indri::lang::RawExtentNode * ou 
) {
#line 732 "indrilang.g"
	 ExtentOr* contexts ;
#line 694 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 732 "indrilang.g"
	
	contexts = new ExtentOr;
	_nodes.push_back( contexts );
	indri::lang::ExtentInside * p = 0;
	indri::lang::ExtentInside * pAdditional = 0;
	
#line 704 "QueryParser.cpp"
	
	match(O_PAREN);
	{
	switch ( LA(1)) {
	case TERM:
	{
		first = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 740 "indrilang.g"
			
			Field* firstField = new indri::lang::Field( first->getText() );
			_nodes.push_back( firstField );
			contexts->addChild( firstField );
			
#line 720 "QueryParser.cpp"
		}
		break;
	}
	case DOT:
	{
		{
		match(DOT);
		p=path();
		if ( inputState->guessing==0 ) {
#line 746 "indrilang.g"
			
			p->setOuter( ou );
			contexts->addChild( p );
			
#line 735 "QueryParser.cpp"
		}
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			{
			switch ( LA(1)) {
			case TERM:
			{
				additional = LT(1);
				match(TERM);
				if ( inputState->guessing==0 ) {
#line 752 "indrilang.g"
					
					Field* additionalField = new Field(additional->getText());
					_nodes.push_back( additionalField );
					contexts->addChild( additionalField );
					
#line 763 "QueryParser.cpp"
				}
				break;
			}
			case DOT:
			{
				{
				match(DOT);
				pAdditional=path();
				if ( inputState->guessing==0 ) {
#line 757 "indrilang.g"
					
					pAdditional->setOuter( ou );
					contexts->addChild( pAdditional );
					
#line 778 "QueryParser.cpp"
				}
				}
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
		}
		else {
			goto _loop256;
		}
		
	}
	_loop256:;
	} // ( ... )*
	match(C_PAREN);
	return contexts ;
}

 indri::lang::RawExtentNode*  QueryParser::unqualifiedTerm() {
#line 528 "indrilang.g"
	 indri::lang::RawExtentNode* re ;
#line 804 "QueryParser.cpp"
#line 528 "indrilang.g"
	
	re = 0;
	indri::lang::IndexTerm* t = 0;
	
#line 810 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case OD:
	case OPERATOR:
	{
		re=odNode();
		break;
	}
	case UW:
	{
		re=uwNode();
		break;
	}
	case BAND:
	{
		re=bandNode();
		break;
	}
	case DATEBEFORE:
	{
		re=dateBefore();
		break;
	}
	case DATEAFTER:
	{
		re=dateAfter();
		break;
	}
	case DATEBETWEEN:
	{
		re=dateBetween();
		break;
	}
	case DATEEQUALS:
	{
		re=dateEquals();
		break;
	}
	case O_ANGLE:
	{
		re=synonym_list();
		break;
	}
	case O_BRACE:
	{
		re=synonym_list_brace();
		break;
	}
	case SYN:
	{
		re=synonym_list_alt();
		break;
	}
	case WSYN:
	{
		re=wsynNode();
		break;
	}
	case ANY:
	{
		re=anyField();
		break;
	}
	case LESS:
	{
		re=lessNode();
		break;
	}
	case GREATER:
	{
		re=greaterNode();
		break;
	}
	case BETWEEN:
	{
		re=betweenNode();
		break;
	}
	case EQUALS:
	{
		re=equalsNode();
		break;
	}
	case WCARD:
	{
		re=wildcardOpNode();
		break;
	}
	default:
		bool synPredMatched217 = false;
		if (((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2))))) {
			int _m217 = mark();
			synPredMatched217 = true;
			inputState->guessing++;
			try {
				{
				match(TERM);
				match(STAR);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched217 = false;
			}
			rewind(_m217);
			inputState->guessing--;
		}
		if ( synPredMatched217 ) {
			t=rawText();
			match(STAR);
			if ( inputState->guessing==0 ) {
#line 549 "indrilang.g"
				
				// wildcard support as an unqualified term
				// i.e. "term*"
				re=new indri::lang::WildcardTerm( t->getText() );
				_nodes.push_back(re);
				
#line 928 "QueryParser.cpp"
			}
		}
		else {
			bool synPredMatched219 = false;
			if (((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2))))) {
				int _m219 = mark();
				synPredMatched219 = true;
				inputState->guessing++;
				try {
					{
					match(NUMBER);
					match(STAR);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched219 = false;
				}
				rewind(_m219);
				inputState->guessing--;
			}
			if ( synPredMatched219 ) {
				t=rawText();
				match(STAR);
				if ( inputState->guessing==0 ) {
#line 555 "indrilang.g"
					
					// wildcard support as an unqualified term
					// i.e. "term*"
					re=new indri::lang::WildcardTerm( t->getText() );
					_nodes.push_back(re);
					
#line 960 "QueryParser.cpp"
				}
			}
			else {
				bool synPredMatched221 = false;
				if (((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2))))) {
					int _m221 = mark();
					synPredMatched221 = true;
					inputState->guessing++;
					try {
						{
						match(FLOAT);
						match(STAR);
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched221 = false;
					}
					rewind(_m221);
					inputState->guessing--;
				}
				if ( synPredMatched221 ) {
					t=rawText();
					match(STAR);
					if ( inputState->guessing==0 ) {
#line 561 "indrilang.g"
						
						// wildcard support as an unqualified term
						// i.e. "term*"
						re=new indri::lang::WildcardTerm( t->getText() );
						_nodes.push_back(re);
						
#line 992 "QueryParser.cpp"
					}
				}
				else {
					bool synPredMatched223 = false;
					if (((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2))))) {
						int _m223 = mark();
						synPredMatched223 = true;
						inputState->guessing++;
						try {
							{
							match(NEGATIVE_NUMBER);
							match(STAR);
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched223 = false;
						}
						rewind(_m223);
						inputState->guessing--;
					}
					if ( synPredMatched223 ) {
						t=rawText();
						match(STAR);
						if ( inputState->guessing==0 ) {
#line 567 "indrilang.g"
							
							// wildcard support as an unqualified term
							// i.e. "term*"
							re=new indri::lang::WildcardTerm( t->getText() );
							_nodes.push_back(re);
							
#line 1024 "QueryParser.cpp"
						}
					}
					else {
						bool synPredMatched225 = false;
						if (((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2))))) {
							int _m225 = mark();
							synPredMatched225 = true;
							inputState->guessing++;
							try {
								{
								match(TEXT_TERM);
								match(STAR);
								}
							}
							catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
								synPredMatched225 = false;
							}
							rewind(_m225);
							inputState->guessing--;
						}
						if ( synPredMatched225 ) {
							t=rawText();
							match(STAR);
							if ( inputState->guessing==0 ) {
#line 573 "indrilang.g"
								
								// wildcard support as an unqualified term
								// i.e. "term*"
								re=new indri::lang::WildcardTerm( t->getText() );
								_nodes.push_back(re);
								
#line 1056 "QueryParser.cpp"
							}
						}
						else if ((_tokenSet_4.member(LA(1))) && (_tokenSet_6.member(LA(2)))) {
							re=rawText();
						}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}}}}}
	return re ;
}

 indri::lang::ScoredExtentNode*  QueryParser::weightedList(
	 indri::lang::WeightedCombinationNode* wn, indri::lang::RawExtentNode * ou 
) {
#line 264 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 1074 "QueryParser.cpp"
#line 264 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 1081 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(wn, ou);
		if ( inputState->guessing==0 ) {
#line 270 "indrilang.g"
			ou = innerMost(sr);
#line 1091 "QueryParser.cpp"
		}
		break;
	}
	case O_PAREN:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(O_PAREN);
	{ // ( ... )+
	int _cnt124=0;
	for (;;) {
		if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
			w=floating();
			n=scoredExtentNode(ou);
			if ( inputState->guessing==0 ) {
#line 275 "indrilang.g"
				wn->addChild( w, n );
#line 1115 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt124>=1 ) { goto _loop124; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt124++;
	}
	_loop124:;
	}  // ( ... )+
	match(C_PAREN);
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::extentRestriction(
	 indri::lang::ScoredExtentNode* sn, indri::lang::RawExtentNode * ou 
) {
#line 592 "indrilang.g"
	 indri::lang::ScoredExtentNode* er ;
#line 1135 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  passageWindowSize = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  inc = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 592 "indrilang.g"
	
	indri::lang::Field* f = 0;
	er = 0;
	indri::lang::ExtentInside * po = 0;
	
#line 1145 "QueryParser.cpp"
	
	bool synPredMatched230 = false;
	if (((LA(1) == O_SQUARE) && (LA(2) == TERM))) {
		int _m230 = mark();
		synPredMatched230 = true;
		inputState->guessing++;
		try {
			{
			match(O_SQUARE);
			match(TERM);
			match(COLON);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched230 = false;
		}
		rewind(_m230);
		inputState->guessing--;
	}
	if ( synPredMatched230 ) {
		match(O_SQUARE);
		passageWindowSize = LT(1);
		match(TERM);
		match(COLON);
		inc = LT(1);
		match(NUMBER);
		match(C_SQUARE);
		if ( inputState->guessing==0 ) {
#line 598 "indrilang.g"
			
			int startWindow;
			
			for( startWindow = 0; startWindow < passageWindowSize->getText().size(); startWindow++ ) {
			if( isdigit((unsigned char) passageWindowSize->getText()[startWindow] ) )
			break;
			}
			
			int increment = atoi(inc->getText().c_str());
			int windowSize = atoi(passageWindowSize->getText().c_str() + startWindow );
			
			er = new indri::lang::FixedPassage(sn, windowSize, increment);
			
#line 1188 "QueryParser.cpp"
		}
	}
	else {
		bool synPredMatched232 = false;
		if (((LA(1) == O_SQUARE) && (LA(2) == TERM))) {
			int _m232 = mark();
			synPredMatched232 = true;
			inputState->guessing++;
			try {
				{
				match(O_SQUARE);
				match(TERM);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched232 = false;
			}
			rewind(_m232);
			inputState->guessing--;
		}
		if ( synPredMatched232 ) {
			match(O_SQUARE);
			field = LT(1);
			match(TERM);
			match(C_SQUARE);
			if ( inputState->guessing==0 ) {
#line 612 "indrilang.g"
				
				f = new indri::lang::Field(field->getText());
				_nodes.push_back(f);
				er = new indri::lang::ExtentRestriction(sn, f);
				_nodes.push_back(er);
				
#line 1222 "QueryParser.cpp"
			}
		}
		else if ((LA(1) == O_SQUARE) && (LA(2) == DOT)) {
			match(O_SQUARE);
			match(DOT);
			po=path();
			match(C_SQUARE);
			if ( inputState->guessing==0 ) {
#line 619 "indrilang.g"
				
				
				if ( ou == 0 ) {
				throw new antlr::SemanticException("Use of a . in a extent restriction without a valid outer context.");
				}
				po->setOuter(ou);
				er = new indri::lang::ExtentRestriction(sn, po);
				_nodes.push_back(er);
				
#line 1241 "QueryParser.cpp"
			}
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return er ;
}

 double  QueryParser::floating() {
#line 916 "indrilang.g"
	 double d ;
#line 1254 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 916 "indrilang.g"
	
	d = 0;
	
#line 1261 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 919 "indrilang.g"
			
			d = atof(f->getText().c_str());
			
#line 1273 "QueryParser.cpp"
		}
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 922 "indrilang.g"
			
			d = atof(n->getText().c_str());
			
#line 1286 "QueryParser.cpp"
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return d ;
}

 indri::lang::ScoredExtentNode*  QueryParser::sumList(
	 indri::lang::WSumNode* wn, indri::lang::RawExtentNode * ou 
) {
#line 280 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 1303 "QueryParser.cpp"
#line 280 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 1310 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(wn, ou);
		if ( inputState->guessing==0 ) {
#line 286 "indrilang.g"
			ou = innerMost(sr);
#line 1320 "QueryParser.cpp"
		}
		break;
	}
	case O_PAREN:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(O_PAREN);
	{ // ( ... )+
	int _cnt128=0;
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			n=scoredExtentNode(ou);
			if ( inputState->guessing==0 ) {
#line 288 "indrilang.g"
				wn->addChild( 1.0, n );
#line 1343 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt128>=1 ) { goto _loop128; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt128++;
	}
	_loop128:;
	}  // ( ... )+
	match(C_PAREN);
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::unweightedList(
	 indri::lang::UnweightedCombinationNode* cn, indri::lang::RawExtentNode * ou 
) {
#line 292 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 1363 "QueryParser.cpp"
#line 292 "indrilang.g"
	
	ScoredExtentNode* n = 0;
	sr = cn;
	
#line 1369 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(cn, ou);
		if ( inputState->guessing==0 ) {
#line 297 "indrilang.g"
			ou = innerMost(sr);
#line 1379 "QueryParser.cpp"
		}
		break;
	}
	case O_PAREN:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(O_PAREN);
	{ // ( ... )+
	int _cnt132=0;
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			n=scoredExtentNode(ou);
			if ( inputState->guessing==0 ) {
#line 299 "indrilang.g"
				cn->addChild( n );
#line 1402 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt132>=1 ) { goto _loop132; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt132++;
	}
	_loop132:;
	}  // ( ... )+
	match(C_PAREN);
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::sumNode(
	indri::lang::RawExtentNode * ou 
) {
#line 317 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 1422 "QueryParser.cpp"
#line 317 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 1428 "QueryParser.cpp"
	
	match(SUM);
	r=sumList(wn, ou);
	return r ;
}

 indri::lang::WeightedExtentOr*  QueryParser::wsynNode() {
#line 385 "indrilang.g"
	 indri::lang::WeightedExtentOr* ws ;
#line 1438 "QueryParser.cpp"
#line 385 "indrilang.g"
	
	ws = new indri::lang::WeightedExtentOr;
	_nodes.push_back(ws);
	
	double w = 0;
	RawExtentNode* n = 0;
	
#line 1447 "QueryParser.cpp"
	
	match(WSYN);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt145=0;
	for (;;) {
		if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
			w=floating();
			n=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 394 "indrilang.g"
				ws->addChild( w, n );
#line 1460 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt145>=1 ) { goto _loop145; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt145++;
	}
	_loop145:;
	}  // ( ... )+
	match(C_PAREN);
	return ws ;
}

 RawExtentNode*  QueryParser::unscoredTerm() {
#line 504 "indrilang.g"
	 RawExtentNode* t ;
#line 1478 "QueryParser.cpp"
#line 504 "indrilang.g"
	
	t = 0;
	
#line 1483 "QueryParser.cpp"
	
	bool synPredMatched179 = false;
	if (((_tokenSet_1.member(LA(1))) && (_tokenSet_2.member(LA(2))))) {
		int _m179 = mark();
		synPredMatched179 = true;
		inputState->guessing++;
		try {
			{
			qualifiedTerm();
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched179 = false;
		}
		rewind(_m179);
		inputState->guessing--;
	}
	if ( synPredMatched179 ) {
		t=qualifiedTerm();
	}
	else if ((_tokenSet_1.member(LA(1))) && (_tokenSet_7.member(LA(2)))) {
		t=unqualifiedTerm();
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return t ;
}

 indri::lang::ODNode*  QueryParser::odNode() {
#line 398 "indrilang.g"
	 indri::lang::ODNode* od ;
#line 1517 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 398 "indrilang.g"
	
	RawExtentNode* rn = 0;
	od = new indri::lang::ODNode;
	_nodes.push_back(od);
	
#line 1526 "QueryParser.cpp"
	
	{
	bool synPredMatched149 = false;
	if (((LA(1) == OD) && (LA(2) == NUMBER))) {
		int _m149 = mark();
		synPredMatched149 = true;
		inputState->guessing++;
		try {
			{
			match(OD);
			match(NUMBER);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched149 = false;
		}
		rewind(_m149);
		inputState->guessing--;
	}
	if ( synPredMatched149 ) {
		{
		match(OD);
		n1 = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 407 "indrilang.g"
			od->setWindowSize( n1->getText() );
#line 1554 "QueryParser.cpp"
		}
		}
	}
	else {
		bool synPredMatched152 = false;
		if (((LA(1) == OD) && (LA(2) == O_PAREN))) {
			int _m152 = mark();
			synPredMatched152 = true;
			inputState->guessing++;
			try {
				{
				match(OD);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched152 = false;
			}
			rewind(_m152);
			inputState->guessing--;
		}
		if ( synPredMatched152 ) {
			{
			match(OD);
			}
		}
		else if ((LA(1) == OPERATOR)) {
			{
			match(OPERATOR);
			n2 = LT(1);
			match(NUMBER);
			if ( inputState->guessing==0 ) {
#line 411 "indrilang.g"
				od->setWindowSize( n2->getText() );
#line 1588 "QueryParser.cpp"
			}
			}
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(O_PAREN);
	{ // ( ... )+
	int _cnt156=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 415 "indrilang.g"
				od->addChild( rn );
#line 1606 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt156>=1 ) { goto _loop156; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt156++;
	}
	_loop156:;
	}  // ( ... )+
	match(C_PAREN);
	return od ;
}

 indri::lang::UWNode*  QueryParser::uwNode() {
#line 419 "indrilang.g"
	 indri::lang::UWNode* uw ;
#line 1624 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 419 "indrilang.g"
	
	uw = new indri::lang::UWNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(uw);
	
#line 1632 "QueryParser.cpp"
	
	{
	bool synPredMatched160 = false;
	if (((LA(1) == UW) && (LA(2) == NUMBER))) {
		int _m160 = mark();
		synPredMatched160 = true;
		inputState->guessing++;
		try {
			{
			match(UW);
			match(NUMBER);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched160 = false;
		}
		rewind(_m160);
		inputState->guessing--;
	}
	if ( synPredMatched160 ) {
		{
		match(UW);
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 427 "indrilang.g"
			uw->setWindowSize( n->getText() );
#line 1660 "QueryParser.cpp"
		}
		}
	}
	else if ((LA(1) == UW) && (LA(2) == O_PAREN)) {
		{
		match(UW);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(O_PAREN);
	{ // ( ... )+
	int _cnt164=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 433 "indrilang.g"
				uw->addChild( rn );
#line 1683 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt164>=1 ) { goto _loop164; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt164++;
	}
	_loop164:;
	}  // ( ... )+
	match(C_PAREN);
	return uw ;
}

 indri::lang::BAndNode*  QueryParser::bandNode() {
#line 436 "indrilang.g"
	 indri::lang::BAndNode* b ;
#line 1701 "QueryParser.cpp"
#line 436 "indrilang.g"
	
	b = new indri::lang::BAndNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(b);
	
#line 1708 "QueryParser.cpp"
	
	match(BAND);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt167=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 444 "indrilang.g"
				b->addChild( rn );
#line 1720 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt167>=1 ) { goto _loop167; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt167++;
	}
	_loop167:;
	}  // ( ... )+
	match(C_PAREN);
	return b ;
}

 indri::lang::Field*  QueryParser::anyField() {
#line 491 "indrilang.g"
	 indri::lang::Field* f ;
#line 1738 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  tt = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 491 "indrilang.g"
	
	f = 0;
	
#line 1745 "QueryParser.cpp"
	
	bool synPredMatched174 = false;
	if (((LA(1) == ANY) && (LA(2) == COLON))) {
		int _m174 = mark();
		synPredMatched174 = true;
		inputState->guessing++;
		try {
			{
			match(ANY);
			match(COLON);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched174 = false;
		}
		rewind(_m174);
		inputState->guessing--;
	}
	if ( synPredMatched174 ) {
		match(ANY);
		match(COLON);
		t = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 495 "indrilang.g"
			
			f = new Field(t->getText());
			_nodes.push_back(f);
			
#line 1775 "QueryParser.cpp"
		}
	}
	else {
		bool synPredMatched176 = false;
		if (((LA(1) == ANY) && (LA(2) == O_PAREN))) {
			int _m176 = mark();
			synPredMatched176 = true;
			inputState->guessing++;
			try {
				{
				match(ANY);
				match(O_PAREN);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched176 = false;
			}
			rewind(_m176);
			inputState->guessing--;
		}
		if ( synPredMatched176 ) {
			match(ANY);
			match(O_PAREN);
			tt = LT(1);
			match(TERM);
			match(C_PAREN);
			if ( inputState->guessing==0 ) {
#line 499 "indrilang.g"
				
				f = new Field(tt->getText());
				_nodes.push_back(f);
				
#line 1808 "QueryParser.cpp"
			}
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return f ;
}

 indri::lang::ExtentAnd*  QueryParser::field_list() {
#line 712 "indrilang.g"
	 indri::lang::ExtentAnd* fields ;
#line 1821 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 712 "indrilang.g"
	
	fields = new ExtentAnd;
	_nodes.push_back( fields );
	
#line 1829 "QueryParser.cpp"
	
	first = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 718 "indrilang.g"
		
		Field* firstField = new indri::lang::Field( first->getText() );
		_nodes.push_back( firstField );
		fields->addChild( firstField );
		
#line 1840 "QueryParser.cpp"
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			additional = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 725 "indrilang.g"
				
				Field* additionalField = new Field(additional->getText());
				_nodes.push_back( additionalField );
				fields->addChild( additionalField );
				
#line 1855 "QueryParser.cpp"
			}
		}
		else {
			goto _loop249;
		}
		
	}
	_loop249:;
	} // ( ... )*
	return fields ;
}

 indri::lang::FieldLessNode*  QueryParser::dateBefore() {
#line 775 "indrilang.g"
	 indri::lang::FieldLessNode* extent ;
#line 1871 "QueryParser.cpp"
#line 775 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1878 "QueryParser.cpp"
	
	match(DATEBEFORE);
	match(O_PAREN);
	d=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 780 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldLessNode( dateField, d );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1892 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldGreaterNode*  QueryParser::dateAfter() {
#line 787 "indrilang.g"
	 indri::lang::FieldGreaterNode* extent ;
#line 1900 "QueryParser.cpp"
#line 787 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1907 "QueryParser.cpp"
	
	match(DATEAFTER);
	match(O_PAREN);
	d=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 792 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldGreaterNode( dateField, d );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1921 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldBetweenNode*  QueryParser::dateBetween() {
#line 799 "indrilang.g"
	 indri::lang::FieldBetweenNode* extent ;
#line 1929 "QueryParser.cpp"
#line 799 "indrilang.g"
	
	UINT64 low = 0;
	UINT64 high = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1937 "QueryParser.cpp"
	
	match(DATEBETWEEN);
	match(O_PAREN);
	low=date();
	high=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 805 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldBetweenNode( dateField, low, high );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1952 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldEqualsNode*  QueryParser::dateEquals() {
#line 812 "indrilang.g"
	 indri::lang::FieldEqualsNode* extent ;
#line 1960 "QueryParser.cpp"
#line 812 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1967 "QueryParser.cpp"
	
	match(DATEEQUALS);
	match(O_PAREN);
	d=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 817 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldEqualsNode( dateField, d );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1981 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list() {
#line 683 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1989 "QueryParser.cpp"
#line 683 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1996 "QueryParser.cpp"
	
	match(O_ANGLE);
	{ // ( ... )+
	int _cnt240=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 689 "indrilang.g"
				s->addChild(term);
#line 2007 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt240>=1 ) { goto _loop240; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt240++;
	}
	_loop240:;
	}  // ( ... )+
	match(C_ANGLE);
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_brace() {
#line 692 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 2025 "QueryParser.cpp"
#line 692 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 2032 "QueryParser.cpp"
	
	match(O_BRACE);
	{ // ( ... )+
	int _cnt243=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 698 "indrilang.g"
				s->addChild(term);
#line 2043 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt243>=1 ) { goto _loop243; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt243++;
	}
	_loop243:;
	}  // ( ... )+
	match(C_BRACE);
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_alt() {
#line 701 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 2061 "QueryParser.cpp"
#line 701 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	// semantics of this node will change
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 2069 "QueryParser.cpp"
	
	match(SYN);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt246=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 709 "indrilang.g"
				s->addChild(term);
#line 2081 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt246>=1 ) { goto _loop246; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt246++;
	}
	_loop246:;
	}  // ( ... )+
	match(C_PAREN);
	return s ;
}

 indri::lang::FieldLessNode*  QueryParser::lessNode() {
#line 948 "indrilang.g"
	 indri::lang::FieldLessNode* ln ;
#line 2099 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 948 "indrilang.g"
	
	ln = 0;
	Field* compareField = 0;
	INT64 high = 0;
	
#line 2107 "QueryParser.cpp"
	
	match(LESS);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	high=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 953 "indrilang.g"
		
		compareField = new Field(field->getText());
		ln = new FieldLessNode( compareField, high );
		_nodes.push_back( compareField );
		_nodes.push_back( ln );
		
#line 2123 "QueryParser.cpp"
	}
	return ln ;
}

 indri::lang::FieldGreaterNode*  QueryParser::greaterNode() {
#line 936 "indrilang.g"
	 indri::lang::FieldGreaterNode* gn ;
#line 2131 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 936 "indrilang.g"
	
	gn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	
#line 2139 "QueryParser.cpp"
	
	match(GREATER);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	low=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 941 "indrilang.g"
		
		compareField = new Field(field->getText());
		gn = new FieldGreaterNode( compareField, low );
		_nodes.push_back( compareField );
		_nodes.push_back( gn );
		
#line 2155 "QueryParser.cpp"
	}
	return gn ;
}

 indri::lang::FieldBetweenNode*  QueryParser::betweenNode() {
#line 960 "indrilang.g"
	 indri::lang::FieldBetweenNode* bn ;
#line 2163 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 960 "indrilang.g"
	
	bn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	INT64 high = 0;
	
#line 2172 "QueryParser.cpp"
	
	match(BETWEEN);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	low=number();
	high=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 966 "indrilang.g"
		
		compareField = new Field(field->getText());
		bn = new FieldBetweenNode( compareField, low, high );
		_nodes.push_back( compareField );
		_nodes.push_back( bn );
		
#line 2189 "QueryParser.cpp"
	}
	return bn ;
}

 indri::lang::FieldEqualsNode*  QueryParser::equalsNode() {
#line 973 "indrilang.g"
	 indri::lang::FieldEqualsNode* en ;
#line 2197 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 973 "indrilang.g"
	
	en = 0;
	Field* compareField = 0;
	INT64 eq = 0;
	
#line 2205 "QueryParser.cpp"
	
	match(EQUALS);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	eq=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 978 "indrilang.g"
		
		compareField = new Field(field->getText());
		en = new FieldEqualsNode( compareField, eq );
		_nodes.push_back( compareField );
		_nodes.push_back( en );
		
#line 2221 "QueryParser.cpp"
	}
	return en ;
}

 indri::lang::WildcardTerm*  QueryParser::wildcardOpNode() {
#line 581 "indrilang.g"
	 indri::lang::WildcardTerm* s ;
#line 2229 "QueryParser.cpp"
#line 581 "indrilang.g"
	
	// wildcard operator "#wildcard( term )"
	indri::lang::IndexTerm* t = 0;
	s = new indri::lang::WildcardTerm;
	_nodes.push_back(s);
	
#line 2237 "QueryParser.cpp"
	
	match(WCARD);
	match(O_PAREN);
	{
	t=rawText();
	if ( inputState->guessing==0 ) {
#line 589 "indrilang.g"
		s->setTerm(t->getText());
#line 2246 "QueryParser.cpp"
	}
	}
	match(C_PAREN);
	return s ;
}

 indri::lang::IndexTerm*  QueryParser::rawText() {
#line 877 "indrilang.g"
	 indri::lang::IndexTerm* t ;
#line 2256 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nn = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  et = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  qet = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 877 "indrilang.g"
	
	t = 0;
	
#line 2267 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case TERM:
	{
		id = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 880 "indrilang.g"
			
			t = new indri::lang::IndexTerm(id->getText());
			_nodes.push_back(t);
			
#line 2280 "QueryParser.cpp"
		}
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 884 "indrilang.g"
			
			t = new indri::lang::IndexTerm(n->getText());
			_nodes.push_back(t);
			
#line 2294 "QueryParser.cpp"
		}
		break;
	}
	case NEGATIVE_NUMBER:
	{
		nn = LT(1);
		match(NEGATIVE_NUMBER);
		if ( inputState->guessing==0 ) {
#line 888 "indrilang.g"
			
			t = new indri::lang::IndexTerm(nn->getText());
			_nodes.push_back(t);
			
#line 2308 "QueryParser.cpp"
		}
		break;
	}
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 892 "indrilang.g"
			
			t = new indri::lang::IndexTerm(f->getText());
			_nodes.push_back(t);
			
#line 2322 "QueryParser.cpp"
		}
		break;
	}
	case DBL_QUOTE:
	{
		match(DBL_QUOTE);
		t=rawText();
		match(DBL_QUOTE);
		if ( inputState->guessing==0 ) {
#line 896 "indrilang.g"
			
			// if a text term appears in quotes, consider it stemmed
			t->setStemmed(true);
			
#line 2337 "QueryParser.cpp"
		}
		break;
	}
	case ENCODED_TERM:
	{
		et = LT(1);
		match(ENCODED_TERM);
		if ( inputState->guessing==0 ) {
#line 900 "indrilang.g"
			
			std::string decodedString; 
			base64_decode_string(decodedString, et->getText());
			t = new indri::lang::IndexTerm( decodedString );
			_nodes.push_back(t);
			
#line 2353 "QueryParser.cpp"
		}
		break;
	}
	case ENCODED_QUOTED_TERM:
	{
		qet = LT(1);
		match(ENCODED_QUOTED_TERM);
		if ( inputState->guessing==0 ) {
#line 906 "indrilang.g"
			
			std::string decodedString; 
			base64_decode_string(decodedString, qet->getText());
			t = new indri::lang::IndexTerm( decodedString );
			t->setStemmed(true);
			_nodes.push_back(t);
			
#line 2370 "QueryParser.cpp"
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return t ;
}

 indri::lang::ExtentInside*  QueryParser::path() {
#line 630 "indrilang.g"
	 indri::lang::ExtentInside* r ;
#line 2385 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldRestricted = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 630 "indrilang.g"
	
	r = 0;
	indri::lang::Field * f = 0;
	indri::lang::ExtentInside * po = 0;
	indri::lang::ExtentInside * lastPo = 0;
	
#line 2394 "QueryParser.cpp"
	
	{ // ( ... )+
	int _cnt235=0;
	for (;;) {
		if ((LA(1) == O_BRACE || LA(1) == SLASH || LA(1) == B_SLASH)) {
			po=pathOperator();
			fieldRestricted = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 636 "indrilang.g"
				
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
				
#line 2418 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt235>=1 ) { goto _loop235; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt235++;
	}
	_loop235:;
	}  // ( ... )+
	return r ;
}

 indri::lang::ExtentInside*  QueryParser::pathOperator() {
#line 652 "indrilang.g"
	 indri::lang::ExtentInside* r ;
#line 2435 "QueryParser.cpp"
#line 652 "indrilang.g"
	
	r = 0;
	indri::lang::DocumentStructureNode * ds = 0;
	
#line 2441 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case SLASH:
	{
		match(SLASH);
		{
		switch ( LA(1)) {
		case SLASH:
		{
			match(SLASH);
			if ( inputState->guessing==0 ) {
#line 656 "indrilang.g"
				
				ds = new indri::lang::DocumentStructureNode;
				_nodes.push_back(ds);
				r = new indri::lang::ExtentDescendant(NULL, NULL, ds);
				_nodes.push_back(r);
				
#line 2460 "QueryParser.cpp"
			}
			break;
		}
		case TERM:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
#line 661 "indrilang.g"
			
			if (r == 0) {
			ds = new indri::lang::DocumentStructureNode;
			_nodes.push_back(ds);
			r = new indri::lang::ExtentChild(NULL, NULL, ds);
			_nodes.push_back(r);
			}
			
#line 2484 "QueryParser.cpp"
		}
		break;
	}
	case B_SLASH:
	{
		match(B_SLASH);
		if ( inputState->guessing==0 ) {
#line 669 "indrilang.g"
			
			ds = new indri::lang::DocumentStructureNode;
			_nodes.push_back(ds);
			r = new indri::lang::ExtentParent(NULL, NULL, ds);
			_nodes.push_back(r);
			
#line 2499 "QueryParser.cpp"
		}
		break;
	}
	case O_BRACE:
	{
		match(O_BRACE);
		if ( inputState->guessing==0 ) {
#line 675 "indrilang.g"
			
			r = new indri::lang::ExtentInside(NULL, NULL);   
			_nodes.push_back(r);
			
#line 2512 "QueryParser.cpp"
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return r ;
}

 indri::lang::Field*  QueryParser::field_restriction() {
#line 766 "indrilang.g"
	 indri::lang::Field* extent ;
#line 2527 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	match(O_SQUARE);
	fieldName = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 768 "indrilang.g"
		
		extent = new Field( fieldName->getText() );
		_nodes.push_back( extent );
		
#line 2539 "QueryParser.cpp"
	}
	match(C_SQUARE);
	return extent ;
}

 UINT64  QueryParser::date() {
#line 839 "indrilang.g"
	 UINT64 d ;
#line 2548 "QueryParser.cpp"
	
	bool synPredMatched264 = false;
	if (((LA(1) == NUMBER) && (LA(2) == SLASH))) {
		int _m264 = mark();
		synPredMatched264 = true;
		inputState->guessing++;
		try {
			{
			match(NUMBER);
			match(SLASH);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched264 = false;
		}
		rewind(_m264);
		inputState->guessing--;
	}
	if ( synPredMatched264 ) {
		d=slashDate();
	}
	else {
		bool synPredMatched266 = false;
		if (((LA(1) == NUMBER || LA(1) == TERM) && (LA(2) == NUMBER || LA(2) == TERM))) {
			int _m266 = mark();
			synPredMatched266 = true;
			inputState->guessing++;
			try {
				{
				match(TERM);
				match(NUMBER);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched266 = false;
			}
			rewind(_m266);
			inputState->guessing--;
		}
		if ( synPredMatched266 ) {
			d=spaceDate();
		}
		else {
			bool synPredMatched268 = false;
			if (((LA(1) == NUMBER || LA(1) == TERM) && (LA(2) == NUMBER || LA(2) == TERM))) {
				int _m268 = mark();
				synPredMatched268 = true;
				inputState->guessing++;
				try {
					{
					match(NUMBER);
					match(TERM);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched268 = false;
				}
				rewind(_m268);
				inputState->guessing--;
			}
			if ( synPredMatched268 ) {
				d=spaceDate();
			}
			else if ((LA(1) == TERM) && (LA(2) == NUMBER || LA(2) == C_PAREN || LA(2) == TERM)) {
				d=dashDate();
			}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}}
	return d ;
}

 UINT64  QueryParser::slashDate() {
#line 859 "indrilang.g"
	 UINT64 d ;
#line 2625 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 859 "indrilang.g"
	
	d = 0;
	
#line 2633 "QueryParser.cpp"
	
	month = LT(1);
	match(NUMBER);
	match(SLASH);
	day = LT(1);
	match(NUMBER);
	match(SLASH);
	year = LT(1);
	match(NUMBER);
	if ( inputState->guessing==0 ) {
#line 862 "indrilang.g"
		
		d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() ); 
		
#line 2648 "QueryParser.cpp"
	}
	return d ;
}

 UINT64  QueryParser::spaceDate() {
#line 866 "indrilang.g"
	 UINT64 d ;
#line 2656 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  m = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  dd = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  y = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 866 "indrilang.g"
	
	d = 0;
	
#line 2667 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case NUMBER:
	{
		day = LT(1);
		match(NUMBER);
		month = LT(1);
		match(TERM);
		year = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 869 "indrilang.g"
			
			d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() );
			
#line 2683 "QueryParser.cpp"
		}
		break;
	}
	case TERM:
	{
		m = LT(1);
		match(TERM);
		dd = LT(1);
		match(NUMBER);
		y = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 872 "indrilang.g"
			
			d = indri::parse::DateParse::convertDate( y->getText(), m->getText(), dd->getText() );
			
#line 2700 "QueryParser.cpp"
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return d ;
}

 UINT64  QueryParser::dashDate() {
#line 845 "indrilang.g"
	 UINT64 d ;
#line 2715 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  dmy = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 845 "indrilang.g"
	
	d = 0;
	
#line 2721 "QueryParser.cpp"
	
	dmy = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 848 "indrilang.g"
		
		const std::string& text = dmy->getText();
		int firstDash = text.find('-');
		int secondDash = text.find('-', firstDash+1);
		std::string day = text.substr( 0, firstDash ); 
		std::string month = text.substr( firstDash+1, secondDash-firstDash-1 );
		std::string year = text.substr( secondDash+1 );
		
		d = indri::parse::DateParse::convertDate( year, month, day ); 
		
#line 2737 "QueryParser.cpp"
	}
	return d ;
}

 INT64  QueryParser::number() {
#line 926 "indrilang.g"
	 INT64 v ;
#line 2745 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nn = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 926 "indrilang.g"
	
	v = 0;
	
#line 2752 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 929 "indrilang.g"
			
			v = string_to_i64(n->getText());
			
#line 2764 "QueryParser.cpp"
		}
		break;
	}
	case NEGATIVE_NUMBER:
	{
		nn = LT(1);
		match(NEGATIVE_NUMBER);
		if ( inputState->guessing==0 ) {
#line 932 "indrilang.g"
			
			v = string_to_i64(nn->getText());
			
#line 2777 "QueryParser.cpp"
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return v ;
}

void QueryParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* QueryParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"\"#sum\"",
	"\"#wsum\"",
	"\"#wand\"",
	"\"#od\"",
	"\"#or\"",
	"\"#not\"",
	"\"#uw\"",
	"\"#combine\"",
	"\"#weight\"",
	"\"#max\"",
	"\"#filreq\"",
	"\"#filrej\"",
	"\"#scoreif\"",
	"\"#scoreifnot\"",
	"\"#any\"",
	"\"#band\"",
	"\"#wsyn\"",
	"\"#syn\"",
	"\"#prior\"",
	"\"#dateafter\"",
	"\"#datebefore\"",
	"\"#datebetween\"",
	"\"#dateequals\"",
	"\"#less\"",
	"\"#greater\"",
	"\"#between\"",
	"\"#equals\"",
	"\"#wildcard\"",
	"NUMBER",
	"NEGATIVE_NUMBER",
	"FLOAT",
	"STAR",
	"O_PAREN",
	"C_PAREN",
	"O_ANGLE",
	"C_ANGLE",
	"O_SQUARE",
	"C_SQUARE",
	"O_BRACE",
	"C_BRACE",
	"DBL_QUOTE",
	"QUOTE",
	"DOT",
	"COMMA",
	"SLASH",
	"B_SLASH",
	"DASH",
	"COLON",
	"TAB",
	"CR",
	"LF",
	"SPACE",
	"HIGH_CHAR",
	"DIGIT",
	"ASCII_LETTER",
	"SAFE_LETTER",
	"SAFE_CHAR",
	"BASESIXFOUR_CHAR",
	"TEXT_TERM",
	"TERM",
	"ENCODED_QUOTED_TERM",
	"ENCODED_TERM",
	"OPERATOR",
	"JUNK",
	0
};

const unsigned long QueryParser::_tokenSet_0_data_[] = { 4294967264UL, 2147488839UL, 7UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#scoreif" "#scoreifnot" "#any" "#band" "#wsyn" 
// "#syn" "#prior" "#dateafter" "#datebefore" "#datebetween" "#dateequals" 
// "#less" "#greater" "#between" "#equals" "#wildcard" NUMBER NEGATIVE_NUMBER 
// FLOAT O_ANGLE O_BRACE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM 
// OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_0(_tokenSet_0_data_,8);
const unsigned long QueryParser::_tokenSet_1_data_[] = { 4290512000UL, 2147488839UL, 7UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "#od" "#uw" "#any" "#band" "#wsyn" "#syn" "#dateafter" "#datebefore" 
// "#datebetween" "#dateequals" "#less" "#greater" "#between" "#equals" 
// "#wildcard" NUMBER NEGATIVE_NUMBER FLOAT O_ANGLE O_BRACE DBL_QUOTE TERM 
// ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_1(_tokenSet_1_data_,8);
const unsigned long QueryParser::_tokenSet_2_data_[] = { 4290512000UL, 2148029535UL, 7UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "#od" "#uw" "#any" "#band" "#wsyn" "#syn" "#dateafter" "#datebefore" 
// "#datebetween" "#dateequals" "#less" "#greater" "#between" "#equals" 
// "#wildcard" NUMBER NEGATIVE_NUMBER FLOAT STAR O_PAREN O_ANGLE O_BRACE 
// DBL_QUOTE DOT COLON TERM ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_2(_tokenSet_2_data_,8);
const unsigned long QueryParser::_tokenSet_3_data_[] = { 4294967266UL, 2148013183UL, 7UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#scoreif" "#scoreifnot" "#any" "#band" "#wsyn" 
// "#syn" "#prior" "#dateafter" "#datebefore" "#datebetween" "#dateequals" 
// "#less" "#greater" "#between" "#equals" "#wildcard" NUMBER NEGATIVE_NUMBER 
// FLOAT STAR O_PAREN C_PAREN O_ANGLE O_BRACE DBL_QUOTE COLON TERM ENCODED_QUOTED_TERM 
// ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_3(_tokenSet_3_data_,8);
const unsigned long QueryParser::_tokenSet_4_data_[] = { 0UL, 2147487751UL, 3UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NUMBER NEGATIVE_NUMBER FLOAT DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_4(_tokenSet_4_data_,8);
const unsigned long QueryParser::_tokenSet_5_data_[] = { 0UL, 2147487759UL, 3UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NUMBER NEGATIVE_NUMBER FLOAT STAR DBL_QUOTE TERM ENCODED_QUOTED_TERM 
// ENCODED_TERM 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_5(_tokenSet_5_data_,8);
const unsigned long QueryParser::_tokenSet_6_data_[] = { 4294967266UL, 2147507431UL, 7UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#scoreif" "#scoreifnot" "#any" "#band" "#wsyn" 
// "#syn" "#prior" "#dateafter" "#datebefore" "#datebetween" "#dateequals" 
// "#less" "#greater" "#between" "#equals" "#wildcard" NUMBER NEGATIVE_NUMBER 
// FLOAT C_PAREN O_ANGLE C_ANGLE O_BRACE C_BRACE DBL_QUOTE DOT TERM ENCODED_QUOTED_TERM 
// ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_6(_tokenSet_6_data_,8);
const unsigned long QueryParser::_tokenSet_7_data_[] = { 4294967264UL, 2148015359UL, 7UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#scoreif" "#scoreifnot" "#any" "#band" "#wsyn" 
// "#syn" "#prior" "#dateafter" "#datebefore" "#datebetween" "#dateequals" 
// "#less" "#greater" "#between" "#equals" "#wildcard" NUMBER NEGATIVE_NUMBER 
// FLOAT STAR O_PAREN C_PAREN O_ANGLE C_ANGLE O_BRACE C_BRACE DBL_QUOTE 
// COLON TERM ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_7(_tokenSet_7_data_,8);


ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
