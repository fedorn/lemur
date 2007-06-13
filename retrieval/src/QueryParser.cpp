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
#line 707 "indrilang.g"
	 ExtentOr* contexts ;
#line 694 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 707 "indrilang.g"
	
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
#line 715 "indrilang.g"
			
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
#line 721 "indrilang.g"
			
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
#line 727 "indrilang.g"
					
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
#line 732 "indrilang.g"
					
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
			goto _loop248;
		}
		
	}
	_loop248:;
	} // ( ... )*
	match(C_PAREN);
	return contexts ;
}

 indri::lang::RawExtentNode*  QueryParser::unqualifiedTerm() {
#line 528 "indrilang.g"
	 indri::lang::RawExtentNode* re ;
#line 804 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  wterm = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 528 "indrilang.g"
	
	re = 0;
	
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
		if (((LA(1) == TERM) && (LA(2) == STAR))) {
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
			wterm = LT(1);
			match(TERM);
			match(STAR);
			if ( inputState->guessing==0 ) {
#line 548 "indrilang.g"
				
				// wildcard support as an unqualified term
				// i.e. "term*"
				re=new indri::lang::WildcardTerm( wterm->getText() );
				_nodes.push_back(re);
				
#line 929 "QueryParser.cpp"
			}
		}
		else if ((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2)))) {
			re=rawText();
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return re ;
}

 indri::lang::ScoredExtentNode*  QueryParser::weightedList(
	 indri::lang::WeightedCombinationNode* wn, indri::lang::RawExtentNode * ou 
) {
#line 264 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 947 "QueryParser.cpp"
#line 264 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 954 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(wn, ou);
		if ( inputState->guessing==0 ) {
#line 270 "indrilang.g"
			ou = innerMost(sr);
#line 964 "QueryParser.cpp"
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
#line 988 "QueryParser.cpp"
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
#line 567 "indrilang.g"
	 indri::lang::ScoredExtentNode* er ;
#line 1008 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  passageWindowSize = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  inc = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 567 "indrilang.g"
	
	indri::lang::Field* f = 0;
	er = 0;
	indri::lang::ExtentInside * po = 0;
	
#line 1018 "QueryParser.cpp"
	
	bool synPredMatched222 = false;
	if (((LA(1) == O_SQUARE) && (LA(2) == TERM))) {
		int _m222 = mark();
		synPredMatched222 = true;
		inputState->guessing++;
		try {
			{
			match(O_SQUARE);
			match(TERM);
			match(COLON);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched222 = false;
		}
		rewind(_m222);
		inputState->guessing--;
	}
	if ( synPredMatched222 ) {
		match(O_SQUARE);
		passageWindowSize = LT(1);
		match(TERM);
		match(COLON);
		inc = LT(1);
		match(NUMBER);
		match(C_SQUARE);
		if ( inputState->guessing==0 ) {
#line 573 "indrilang.g"
			
			int startWindow;
			
			for( startWindow = 0; startWindow < passageWindowSize->getText().size(); startWindow++ ) {
			if( isdigit((unsigned char) passageWindowSize->getText()[startWindow] ) )
			break;
			}
			
			int increment = atoi(inc->getText().c_str());
			int windowSize = atoi(passageWindowSize->getText().c_str() + startWindow );
			
			er = new indri::lang::FixedPassage(sn, windowSize, increment);
			
#line 1061 "QueryParser.cpp"
		}
	}
	else {
		bool synPredMatched224 = false;
		if (((LA(1) == O_SQUARE) && (LA(2) == TERM))) {
			int _m224 = mark();
			synPredMatched224 = true;
			inputState->guessing++;
			try {
				{
				match(O_SQUARE);
				match(TERM);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched224 = false;
			}
			rewind(_m224);
			inputState->guessing--;
		}
		if ( synPredMatched224 ) {
			match(O_SQUARE);
			field = LT(1);
			match(TERM);
			match(C_SQUARE);
			if ( inputState->guessing==0 ) {
#line 587 "indrilang.g"
				
				f = new indri::lang::Field(field->getText());
				_nodes.push_back(f);
				er = new indri::lang::ExtentRestriction(sn, f);
				_nodes.push_back(er);
				
#line 1095 "QueryParser.cpp"
			}
		}
		else if ((LA(1) == O_SQUARE) && (LA(2) == DOT)) {
			match(O_SQUARE);
			match(DOT);
			po=path();
			match(C_SQUARE);
			if ( inputState->guessing==0 ) {
#line 594 "indrilang.g"
				
				
				if ( ou == 0 ) {
				throw new antlr::SemanticException("Use of a . in a extent restriction without a valid outer context.");
				}
				po->setOuter(ou);
				er = new indri::lang::ExtentRestriction(sn, po);
				_nodes.push_back(er);
				
#line 1114 "QueryParser.cpp"
			}
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return er ;
}

 double  QueryParser::floating() {
#line 891 "indrilang.g"
	 double d ;
#line 1127 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 891 "indrilang.g"
	
	d = 0;
	
#line 1134 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 894 "indrilang.g"
			
			d = atof(f->getText().c_str());
			
#line 1146 "QueryParser.cpp"
		}
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 897 "indrilang.g"
			
			d = atof(n->getText().c_str());
			
#line 1159 "QueryParser.cpp"
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
#line 1176 "QueryParser.cpp"
#line 280 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 1183 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(wn, ou);
		if ( inputState->guessing==0 ) {
#line 286 "indrilang.g"
			ou = innerMost(sr);
#line 1193 "QueryParser.cpp"
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
#line 1216 "QueryParser.cpp"
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
#line 1236 "QueryParser.cpp"
#line 292 "indrilang.g"
	
	ScoredExtentNode* n = 0;
	sr = cn;
	
#line 1242 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(cn, ou);
		if ( inputState->guessing==0 ) {
#line 297 "indrilang.g"
			ou = innerMost(sr);
#line 1252 "QueryParser.cpp"
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
#line 1275 "QueryParser.cpp"
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
#line 1295 "QueryParser.cpp"
#line 317 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 1301 "QueryParser.cpp"
	
	match(SUM);
	r=sumList(wn, ou);
	return r ;
}

 indri::lang::WeightedExtentOr*  QueryParser::wsynNode() {
#line 385 "indrilang.g"
	 indri::lang::WeightedExtentOr* ws ;
#line 1311 "QueryParser.cpp"
#line 385 "indrilang.g"
	
	ws = new indri::lang::WeightedExtentOr;
	_nodes.push_back(ws);
	
	double w = 0;
	RawExtentNode* n = 0;
	
#line 1320 "QueryParser.cpp"
	
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
#line 1333 "QueryParser.cpp"
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
#line 1351 "QueryParser.cpp"
#line 504 "indrilang.g"
	
	t = 0;
	
#line 1356 "QueryParser.cpp"
	
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
	else if ((_tokenSet_1.member(LA(1))) && (_tokenSet_6.member(LA(2)))) {
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
#line 1390 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 398 "indrilang.g"
	
	RawExtentNode* rn = 0;
	od = new indri::lang::ODNode;
	_nodes.push_back(od);
	
#line 1399 "QueryParser.cpp"
	
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
#line 1427 "QueryParser.cpp"
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
#line 1461 "QueryParser.cpp"
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
#line 1479 "QueryParser.cpp"
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
#line 1497 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 419 "indrilang.g"
	
	uw = new indri::lang::UWNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(uw);
	
#line 1505 "QueryParser.cpp"
	
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
#line 1533 "QueryParser.cpp"
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
#line 1556 "QueryParser.cpp"
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
#line 1574 "QueryParser.cpp"
#line 436 "indrilang.g"
	
	b = new indri::lang::BAndNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(b);
	
#line 1581 "QueryParser.cpp"
	
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
#line 1593 "QueryParser.cpp"
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
#line 1611 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  tt = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 491 "indrilang.g"
	
	f = 0;
	
#line 1618 "QueryParser.cpp"
	
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
			
#line 1648 "QueryParser.cpp"
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
				
#line 1681 "QueryParser.cpp"
			}
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return f ;
}

 indri::lang::ExtentAnd*  QueryParser::field_list() {
#line 687 "indrilang.g"
	 indri::lang::ExtentAnd* fields ;
#line 1694 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 687 "indrilang.g"
	
	fields = new ExtentAnd;
	_nodes.push_back( fields );
	
#line 1702 "QueryParser.cpp"
	
	first = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 693 "indrilang.g"
		
		Field* firstField = new indri::lang::Field( first->getText() );
		_nodes.push_back( firstField );
		fields->addChild( firstField );
		
#line 1713 "QueryParser.cpp"
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			additional = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 700 "indrilang.g"
				
				Field* additionalField = new Field(additional->getText());
				_nodes.push_back( additionalField );
				fields->addChild( additionalField );
				
#line 1728 "QueryParser.cpp"
			}
		}
		else {
			goto _loop241;
		}
		
	}
	_loop241:;
	} // ( ... )*
	return fields ;
}

 indri::lang::FieldLessNode*  QueryParser::dateBefore() {
#line 750 "indrilang.g"
	 indri::lang::FieldLessNode* extent ;
#line 1744 "QueryParser.cpp"
#line 750 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1751 "QueryParser.cpp"
	
	match(DATEBEFORE);
	match(O_PAREN);
	d=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 755 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldLessNode( dateField, d );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1765 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldGreaterNode*  QueryParser::dateAfter() {
#line 762 "indrilang.g"
	 indri::lang::FieldGreaterNode* extent ;
#line 1773 "QueryParser.cpp"
#line 762 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1780 "QueryParser.cpp"
	
	match(DATEAFTER);
	match(O_PAREN);
	d=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 767 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldGreaterNode( dateField, d );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1794 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldBetweenNode*  QueryParser::dateBetween() {
#line 774 "indrilang.g"
	 indri::lang::FieldBetweenNode* extent ;
#line 1802 "QueryParser.cpp"
#line 774 "indrilang.g"
	
	UINT64 low = 0;
	UINT64 high = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1810 "QueryParser.cpp"
	
	match(DATEBETWEEN);
	match(O_PAREN);
	low=date();
	high=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 780 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldBetweenNode( dateField, low, high );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1825 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldEqualsNode*  QueryParser::dateEquals() {
#line 787 "indrilang.g"
	 indri::lang::FieldEqualsNode* extent ;
#line 1833 "QueryParser.cpp"
#line 787 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1840 "QueryParser.cpp"
	
	match(DATEEQUALS);
	match(O_PAREN);
	d=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 792 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldEqualsNode( dateField, d );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1854 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list() {
#line 658 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1862 "QueryParser.cpp"
#line 658 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1869 "QueryParser.cpp"
	
	match(O_ANGLE);
	{ // ( ... )+
	int _cnt232=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 664 "indrilang.g"
				s->addChild(term);
#line 1880 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt232>=1 ) { goto _loop232; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt232++;
	}
	_loop232:;
	}  // ( ... )+
	match(C_ANGLE);
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_brace() {
#line 667 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1898 "QueryParser.cpp"
#line 667 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1905 "QueryParser.cpp"
	
	match(O_BRACE);
	{ // ( ... )+
	int _cnt235=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 673 "indrilang.g"
				s->addChild(term);
#line 1916 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt235>=1 ) { goto _loop235; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt235++;
	}
	_loop235:;
	}  // ( ... )+
	match(C_BRACE);
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_alt() {
#line 676 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1934 "QueryParser.cpp"
#line 676 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	// semantics of this node will change
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1942 "QueryParser.cpp"
	
	match(SYN);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt238=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 684 "indrilang.g"
				s->addChild(term);
#line 1954 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt238>=1 ) { goto _loop238; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt238++;
	}
	_loop238:;
	}  // ( ... )+
	match(C_PAREN);
	return s ;
}

 indri::lang::FieldLessNode*  QueryParser::lessNode() {
#line 923 "indrilang.g"
	 indri::lang::FieldLessNode* ln ;
#line 1972 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 923 "indrilang.g"
	
	ln = 0;
	Field* compareField = 0;
	INT64 high = 0;
	
#line 1980 "QueryParser.cpp"
	
	match(LESS);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	high=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 928 "indrilang.g"
		
		compareField = new Field(field->getText());
		ln = new FieldLessNode( compareField, high );
		_nodes.push_back( compareField );
		_nodes.push_back( ln );
		
#line 1996 "QueryParser.cpp"
	}
	return ln ;
}

 indri::lang::FieldGreaterNode*  QueryParser::greaterNode() {
#line 911 "indrilang.g"
	 indri::lang::FieldGreaterNode* gn ;
#line 2004 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 911 "indrilang.g"
	
	gn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	
#line 2012 "QueryParser.cpp"
	
	match(GREATER);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	low=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 916 "indrilang.g"
		
		compareField = new Field(field->getText());
		gn = new FieldGreaterNode( compareField, low );
		_nodes.push_back( compareField );
		_nodes.push_back( gn );
		
#line 2028 "QueryParser.cpp"
	}
	return gn ;
}

 indri::lang::FieldBetweenNode*  QueryParser::betweenNode() {
#line 935 "indrilang.g"
	 indri::lang::FieldBetweenNode* bn ;
#line 2036 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 935 "indrilang.g"
	
	bn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	INT64 high = 0;
	
#line 2045 "QueryParser.cpp"
	
	match(BETWEEN);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	low=number();
	high=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 941 "indrilang.g"
		
		compareField = new Field(field->getText());
		bn = new FieldBetweenNode( compareField, low, high );
		_nodes.push_back( compareField );
		_nodes.push_back( bn );
		
#line 2062 "QueryParser.cpp"
	}
	return bn ;
}

 indri::lang::FieldEqualsNode*  QueryParser::equalsNode() {
#line 948 "indrilang.g"
	 indri::lang::FieldEqualsNode* en ;
#line 2070 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 948 "indrilang.g"
	
	en = 0;
	Field* compareField = 0;
	INT64 eq = 0;
	
#line 2078 "QueryParser.cpp"
	
	match(EQUALS);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	eq=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 953 "indrilang.g"
		
		compareField = new Field(field->getText());
		en = new FieldEqualsNode( compareField, eq );
		_nodes.push_back( compareField );
		_nodes.push_back( en );
		
#line 2094 "QueryParser.cpp"
	}
	return en ;
}

 indri::lang::WildcardTerm*  QueryParser::wildcardOpNode() {
#line 556 "indrilang.g"
	 indri::lang::WildcardTerm* s ;
#line 2102 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 556 "indrilang.g"
	
	// wildcard operator "#wildcard( term )"
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::WildcardTerm;
	_nodes.push_back(s);
	
#line 2111 "QueryParser.cpp"
	
	match(WCARD);
	match(O_PAREN);
	{
	t = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 564 "indrilang.g"
		s->setTerm(t->getText());
#line 2121 "QueryParser.cpp"
	}
	}
	match(C_PAREN);
	return s ;
}

 indri::lang::IndexTerm*  QueryParser::rawText() {
#line 852 "indrilang.g"
	 indri::lang::IndexTerm* t ;
#line 2131 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nn = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  et = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  qet = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 852 "indrilang.g"
	
	t = 0;
	
#line 2142 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case TERM:
	{
		id = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 855 "indrilang.g"
			
			t = new indri::lang::IndexTerm(id->getText());
			_nodes.push_back(t);
			
#line 2155 "QueryParser.cpp"
		}
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 859 "indrilang.g"
			
			t = new indri::lang::IndexTerm(n->getText());
			_nodes.push_back(t);
			
#line 2169 "QueryParser.cpp"
		}
		break;
	}
	case NEGATIVE_NUMBER:
	{
		nn = LT(1);
		match(NEGATIVE_NUMBER);
		if ( inputState->guessing==0 ) {
#line 863 "indrilang.g"
			
			t = new indri::lang::IndexTerm(nn->getText());
			_nodes.push_back(t);
			
#line 2183 "QueryParser.cpp"
		}
		break;
	}
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 867 "indrilang.g"
			
			t = new indri::lang::IndexTerm(f->getText());
			_nodes.push_back(t);
			
#line 2197 "QueryParser.cpp"
		}
		break;
	}
	case DBL_QUOTE:
	{
		match(DBL_QUOTE);
		t=rawText();
		match(DBL_QUOTE);
		if ( inputState->guessing==0 ) {
#line 871 "indrilang.g"
			
			// if a text term appears in quotes, consider it stemmed
			t->setStemmed(true);
			
#line 2212 "QueryParser.cpp"
		}
		break;
	}
	case ENCODED_TERM:
	{
		et = LT(1);
		match(ENCODED_TERM);
		if ( inputState->guessing==0 ) {
#line 875 "indrilang.g"
			
			std::string decodedString; 
			base64_decode_string(decodedString, et->getText());
			t = new indri::lang::IndexTerm( decodedString );
			_nodes.push_back(t);
			
#line 2228 "QueryParser.cpp"
		}
		break;
	}
	case ENCODED_QUOTED_TERM:
	{
		qet = LT(1);
		match(ENCODED_QUOTED_TERM);
		if ( inputState->guessing==0 ) {
#line 881 "indrilang.g"
			
			std::string decodedString; 
			base64_decode_string(decodedString, qet->getText());
			t = new indri::lang::IndexTerm( decodedString );
			t->setStemmed(true);
			_nodes.push_back(t);
			
#line 2245 "QueryParser.cpp"
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
#line 605 "indrilang.g"
	 indri::lang::ExtentInside* r ;
#line 2260 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldRestricted = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 605 "indrilang.g"
	
	r = 0;
	indri::lang::Field * f = 0;
	indri::lang::ExtentInside * po = 0;
	indri::lang::ExtentInside * lastPo = 0;
	
#line 2269 "QueryParser.cpp"
	
	{ // ( ... )+
	int _cnt227=0;
	for (;;) {
		if ((LA(1) == O_BRACE || LA(1) == SLASH || LA(1) == B_SLASH)) {
			po=pathOperator();
			fieldRestricted = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 611 "indrilang.g"
				
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
				
#line 2293 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt227>=1 ) { goto _loop227; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt227++;
	}
	_loop227:;
	}  // ( ... )+
	return r ;
}

 indri::lang::ExtentInside*  QueryParser::pathOperator() {
#line 627 "indrilang.g"
	 indri::lang::ExtentInside* r ;
#line 2310 "QueryParser.cpp"
#line 627 "indrilang.g"
	
	r = 0;
	indri::lang::DocumentStructureNode * ds = 0;
	
#line 2316 "QueryParser.cpp"
	
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
#line 631 "indrilang.g"
				
				ds = new indri::lang::DocumentStructureNode;
				_nodes.push_back(ds);
				r = new indri::lang::ExtentDescendant(NULL, NULL, ds);
				_nodes.push_back(r);
				
#line 2335 "QueryParser.cpp"
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
#line 636 "indrilang.g"
			
			if (r == 0) {
			ds = new indri::lang::DocumentStructureNode;
			_nodes.push_back(ds);
			r = new indri::lang::ExtentChild(NULL, NULL, ds);
			_nodes.push_back(r);
			}
			
#line 2359 "QueryParser.cpp"
		}
		break;
	}
	case B_SLASH:
	{
		match(B_SLASH);
		if ( inputState->guessing==0 ) {
#line 644 "indrilang.g"
			
			ds = new indri::lang::DocumentStructureNode;
			_nodes.push_back(ds);
			r = new indri::lang::ExtentParent(NULL, NULL, ds);
			_nodes.push_back(r);
			
#line 2374 "QueryParser.cpp"
		}
		break;
	}
	case O_BRACE:
	{
		match(O_BRACE);
		if ( inputState->guessing==0 ) {
#line 650 "indrilang.g"
			
			r = new indri::lang::ExtentInside(NULL, NULL);   
			_nodes.push_back(r);
			
#line 2387 "QueryParser.cpp"
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
#line 741 "indrilang.g"
	 indri::lang::Field* extent ;
#line 2402 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	match(O_SQUARE);
	fieldName = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 743 "indrilang.g"
		
		extent = new Field( fieldName->getText() );
		_nodes.push_back( extent );
		
#line 2414 "QueryParser.cpp"
	}
	match(C_SQUARE);
	return extent ;
}

 UINT64  QueryParser::date() {
#line 814 "indrilang.g"
	 UINT64 d ;
#line 2423 "QueryParser.cpp"
	
	bool synPredMatched256 = false;
	if (((LA(1) == NUMBER) && (LA(2) == SLASH))) {
		int _m256 = mark();
		synPredMatched256 = true;
		inputState->guessing++;
		try {
			{
			match(NUMBER);
			match(SLASH);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched256 = false;
		}
		rewind(_m256);
		inputState->guessing--;
	}
	if ( synPredMatched256 ) {
		d=slashDate();
	}
	else {
		bool synPredMatched258 = false;
		if (((LA(1) == NUMBER || LA(1) == TERM) && (LA(2) == NUMBER || LA(2) == TERM))) {
			int _m258 = mark();
			synPredMatched258 = true;
			inputState->guessing++;
			try {
				{
				match(TERM);
				match(NUMBER);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched258 = false;
			}
			rewind(_m258);
			inputState->guessing--;
		}
		if ( synPredMatched258 ) {
			d=spaceDate();
		}
		else {
			bool synPredMatched260 = false;
			if (((LA(1) == NUMBER || LA(1) == TERM) && (LA(2) == NUMBER || LA(2) == TERM))) {
				int _m260 = mark();
				synPredMatched260 = true;
				inputState->guessing++;
				try {
					{
					match(NUMBER);
					match(TERM);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched260 = false;
				}
				rewind(_m260);
				inputState->guessing--;
			}
			if ( synPredMatched260 ) {
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
#line 834 "indrilang.g"
	 UINT64 d ;
#line 2500 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 834 "indrilang.g"
	
	d = 0;
	
#line 2508 "QueryParser.cpp"
	
	month = LT(1);
	match(NUMBER);
	match(SLASH);
	day = LT(1);
	match(NUMBER);
	match(SLASH);
	year = LT(1);
	match(NUMBER);
	if ( inputState->guessing==0 ) {
#line 837 "indrilang.g"
		
		d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() ); 
		
#line 2523 "QueryParser.cpp"
	}
	return d ;
}

 UINT64  QueryParser::spaceDate() {
#line 841 "indrilang.g"
	 UINT64 d ;
#line 2531 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  m = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  dd = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  y = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 841 "indrilang.g"
	
	d = 0;
	
#line 2542 "QueryParser.cpp"
	
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
#line 844 "indrilang.g"
			
			d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() );
			
#line 2558 "QueryParser.cpp"
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
#line 847 "indrilang.g"
			
			d = indri::parse::DateParse::convertDate( y->getText(), m->getText(), dd->getText() );
			
#line 2575 "QueryParser.cpp"
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
#line 820 "indrilang.g"
	 UINT64 d ;
#line 2590 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  dmy = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 820 "indrilang.g"
	
	d = 0;
	
#line 2596 "QueryParser.cpp"
	
	dmy = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 823 "indrilang.g"
		
		const std::string& text = dmy->getText();
		int firstDash = text.find('-');
		int secondDash = text.find('-', firstDash+1);
		std::string day = text.substr( 0, firstDash ); 
		std::string month = text.substr( firstDash+1, secondDash-firstDash-1 );
		std::string year = text.substr( secondDash+1 );
		
		d = indri::parse::DateParse::convertDate( year, month, day ); 
		
#line 2612 "QueryParser.cpp"
	}
	return d ;
}

 INT64  QueryParser::number() {
#line 901 "indrilang.g"
	 INT64 v ;
#line 2620 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nn = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 901 "indrilang.g"
	
	v = 0;
	
#line 2627 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 904 "indrilang.g"
			
			v = string_to_i64(n->getText());
			
#line 2639 "QueryParser.cpp"
		}
		break;
	}
	case NEGATIVE_NUMBER:
	{
		nn = LT(1);
		match(NEGATIVE_NUMBER);
		if ( inputState->guessing==0 ) {
#line 907 "indrilang.g"
			
			v = string_to_i64(nn->getText());
			
#line 2652 "QueryParser.cpp"
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
const unsigned long QueryParser::_tokenSet_5_data_[] = { 4294967266UL, 2147507431UL, 7UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#scoreif" "#scoreifnot" "#any" "#band" "#wsyn" 
// "#syn" "#prior" "#dateafter" "#datebefore" "#datebetween" "#dateequals" 
// "#less" "#greater" "#between" "#equals" "#wildcard" NUMBER NEGATIVE_NUMBER 
// FLOAT C_PAREN O_ANGLE C_ANGLE O_BRACE C_BRACE DBL_QUOTE DOT TERM ENCODED_QUOTED_TERM 
// ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_5(_tokenSet_5_data_,8);
const unsigned long QueryParser::_tokenSet_6_data_[] = { 4294967264UL, 2148015359UL, 7UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#scoreif" "#scoreifnot" "#any" "#band" "#wsyn" 
// "#syn" "#prior" "#dateafter" "#datebefore" "#datebetween" "#dateequals" 
// "#less" "#greater" "#between" "#equals" "#wildcard" NUMBER NEGATIVE_NUMBER 
// FLOAT STAR O_PAREN C_PAREN O_ANGLE C_ANGLE O_BRACE C_BRACE DBL_QUOTE 
// COLON TERM ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_6(_tokenSet_6_data_,8);


ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
