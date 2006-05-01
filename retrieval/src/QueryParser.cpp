/* $ANTLR 2.7.4: "indrilang.g" -> "QueryParser.cpp"$ */
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
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,1)
{
}

QueryParser::QueryParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

QueryParser::QueryParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,1)
{
}

QueryParser::QueryParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,1)
{
}

 indri::lang::ScoredExtentNode*  QueryParser::query() {
#line 183 "indrilang.g"
	 indri::lang::ScoredExtentNode* q ;
#line 37 "QueryParser.cpp"
#line 183 "indrilang.g"
	
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
	case ANY:
	case BAND:
	case WSYN:
	case SYN:
	case PRIOR:
	case DATEAFTER:
	case DATEBEFORE:
	case DATEBETWEEN:
	case LESS:
	case GREATER:
	case BETWEEN:
	case EQUALS:
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
#line 190 "indrilang.g"
			
			c = new CombineNode;
			c->addChild(q);
			c->addChild(s);
			_nodes.push_back(c);
			q = c;
			
#line 93 "QueryParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				s=scoredExtentNode(0);
				if ( inputState->guessing==0 ) {
#line 198 "indrilang.g"
					
					c->addChild(s);
					
#line 104 "QueryParser.cpp"
				}
			}
			else {
				goto _loop86;
			}
			
		}
		_loop86:;
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
#line 204 "indrilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 135 "QueryParser.cpp"
	
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
	case OD:
	case UW:
	case ANY:
	case BAND:
	case WSYN:
	case SYN:
	case DATEAFTER:
	case DATEBEFORE:
	case DATEBETWEEN:
	case LESS:
	case GREATER:
	case BETWEEN:
	case EQUALS:
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
		s=scoredRaw();
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
#line 300 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 228 "QueryParser.cpp"
#line 300 "indrilang.g"
	
	indri::lang::WeightNode* wn = new indri::lang::WeightNode;
	_nodes.push_back(wn);
	
#line 234 "QueryParser.cpp"
	
	match(WEIGHT);
	r=weightedList(wn, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::combineNode(
	indri::lang::RawExtentNode * ou 
) {
#line 307 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 246 "QueryParser.cpp"
#line 307 "indrilang.g"
	
	indri::lang::CombineNode* cn = new indri::lang::CombineNode;
	_nodes.push_back(cn);
	
#line 252 "QueryParser.cpp"
	
	match(COMBINE);
	r=unweightedList(cn, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::orNode(
	indri::lang::RawExtentNode * ou 
) {
#line 335 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 264 "QueryParser.cpp"
#line 335 "indrilang.g"
	
	indri::lang::OrNode* on = new indri::lang::OrNode;
	_nodes.push_back(on);
	
#line 270 "QueryParser.cpp"
	
	match(OR);
	r=unweightedList(on, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::notNode(
	indri::lang::RawExtentNode * ou 
) {
#line 349 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 282 "QueryParser.cpp"
#line 349 "indrilang.g"
	
	indri::lang::NotNode* n = new indri::lang::NotNode;
	indri::lang::ScoredExtentNode* c = 0;
	_nodes.push_back(n);
	r = n;
	
#line 290 "QueryParser.cpp"
	
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
#line 357 "indrilang.g"
		
		n->setChild(c);
		
#line 318 "QueryParser.cpp"
	}
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::wandNode(
	indri::lang::RawExtentNode * ou 
) {
#line 328 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 328 "QueryParser.cpp"
#line 328 "indrilang.g"
	
	indri::lang::WAndNode* wn = new indri::lang::WAndNode;
	_nodes.push_back(wn);
	
#line 334 "QueryParser.cpp"
	
	match(WAND);
	r=weightedList(wn, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::wsumNode(
	indri::lang::RawExtentNode * ou 
) {
#line 321 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 346 "QueryParser.cpp"
#line 321 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 352 "QueryParser.cpp"
	
	match(WSUM);
	r=weightedList(wn, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::maxNode(
	indri::lang::RawExtentNode * ou 
) {
#line 342 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 364 "QueryParser.cpp"
#line 342 "indrilang.g"
	
	indri::lang::MaxNode* mn = new indri::lang::MaxNode;
	_nodes.push_back(mn);
	
#line 370 "QueryParser.cpp"
	
	match(MAX);
	r=unweightedList(mn, ou);
	return r ;
}

 indri::lang::PriorNode*  QueryParser::priorNode() {
#line 361 "indrilang.g"
	 indri::lang::PriorNode* p ;
#line 380 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 361 "indrilang.g"
	
	indri::lang::Field* field = 0;
	p = 0;
	
#line 387 "QueryParser.cpp"
	
	match(PRIOR);
	match(O_PAREN);
	name = LT(1);
	match(TERM);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 366 "indrilang.g"
		
		p = new indri::lang::PriorNode( name->getText() );
		_nodes.push_back(p);
		
#line 400 "QueryParser.cpp"
	}
	return p ;
}

 indri::lang::FilRejNode*  QueryParser::filrejNode(
	 indri::lang::RawExtentNode * ou 
) {
#line 444 "indrilang.g"
	 indri::lang::FilRejNode* fj ;
#line 410 "QueryParser.cpp"
#line 444 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* disallowed = 0;
	
#line 416 "QueryParser.cpp"
	
	match(FILREJ);
	match(O_PAREN);
	filter=unscoredTerm();
	disallowed=scoredExtentNode(ou);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 450 "indrilang.g"
		
		fj = new FilRejNode( filter, disallowed );
		_nodes.push_back(fj);
		
#line 429 "QueryParser.cpp"
	}
	return fj ;
}

 indri::lang::FilReqNode*  QueryParser::filreqNode(
	 indri::lang::RawExtentNode * ou 
) {
#line 455 "indrilang.g"
	 indri::lang::FilReqNode* fq ;
#line 439 "QueryParser.cpp"
#line 455 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* required = 0;
	
#line 445 "QueryParser.cpp"
	
	match(FILREQ);
	match(O_PAREN);
	filter=unscoredTerm();
	required=scoredExtentNode(ou);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 461 "indrilang.g"
		
		fq = new FilReqNode( filter, required );
		_nodes.push_back(fq);
		
#line 458 "QueryParser.cpp"
	}
	return fq ;
}

 indri::lang::ScoredExtentNode*  QueryParser::scoredRaw() {
#line 223 "indrilang.g"
	 indri::lang::ScoredExtentNode* sn ;
#line 466 "QueryParser.cpp"
#line 223 "indrilang.g"
	
	RawExtentNode* raw = 0;
	RawExtentNode* contexts = 0;
	sn = 0;
	
#line 473 "QueryParser.cpp"
	
	bool synPredMatched110 = false;
	if (((_tokenSet_1.member(LA(1))))) {
		int _m110 = mark();
		synPredMatched110 = true;
		inputState->guessing++;
		try {
			{
			qualifiedTerm();
			match(DOT);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched110 = false;
		}
		rewind(_m110);
		inputState->guessing--;
	}
	if ( synPredMatched110 ) {
		raw=qualifiedTerm();
		match(DOT);
		contexts=context_list();
		if ( inputState->guessing==0 ) {
#line 230 "indrilang.g"
			
			sn = new indri::lang::RawScorerNode( raw, contexts );
			_nodes.push_back(sn);
			
#line 502 "QueryParser.cpp"
		}
	}
	else {
		bool synPredMatched112 = false;
		if (((_tokenSet_1.member(LA(1))))) {
			int _m112 = mark();
			synPredMatched112 = true;
			inputState->guessing++;
			try {
				{
				qualifiedTerm();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched112 = false;
			}
			rewind(_m112);
			inputState->guessing--;
		}
		if ( synPredMatched112 ) {
			raw=qualifiedTerm();
			if ( inputState->guessing==0 ) {
#line 235 "indrilang.g"
				
				sn = new indri::lang::RawScorerNode( raw, contexts );
				_nodes.push_back(sn);
				
#line 530 "QueryParser.cpp"
			}
		}
		else {
			bool synPredMatched114 = false;
			if (((_tokenSet_1.member(LA(1))))) {
				int _m114 = mark();
				synPredMatched114 = true;
				inputState->guessing++;
				try {
					{
					unqualifiedTerm();
					match(DOT);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched114 = false;
				}
				rewind(_m114);
				inputState->guessing--;
			}
			if ( synPredMatched114 ) {
				raw=unqualifiedTerm();
				match(DOT);
				contexts=context_list();
				if ( inputState->guessing==0 ) {
#line 240 "indrilang.g"
					
					sn = new indri::lang::RawScorerNode( raw, contexts );
					_nodes.push_back(sn);
					
#line 561 "QueryParser.cpp"
				}
			}
			else if ((_tokenSet_1.member(LA(1)))) {
				raw=unqualifiedTerm();
				if ( inputState->guessing==0 ) {
#line 245 "indrilang.g"
					
					sn = new indri::lang::RawScorerNode( raw, contexts );
					_nodes.push_back(sn);
					
#line 572 "QueryParser.cpp"
				}
			}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}}
	return sn ;
}

 RawExtentNode*  QueryParser::qualifiedTerm() {
#line 482 "indrilang.g"
	 RawExtentNode* t ;
#line 585 "QueryParser.cpp"
#line 482 "indrilang.g"
	
	RawExtentNode* synonyms = 0;
	RawExtentNode* fields = 0;
	t = 0;
	
#line 592 "QueryParser.cpp"
	
	synonyms=unqualifiedTerm();
	match(DOT);
	fields=field_list();
	if ( inputState->guessing==0 ) {
#line 489 "indrilang.g"
		
		if( fields ) {
		t = new indri::lang::ExtentInside( synonyms, fields );
		_nodes.push_back(t);
		synonyms = t;
		} else {
		t = synonyms;
		}
		
#line 608 "QueryParser.cpp"
	}
	return t ;
}

 ExtentOr*  QueryParser::context_list() {
#line 638 "indrilang.g"
	 ExtentOr* contexts ;
#line 616 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 638 "indrilang.g"
	
	contexts = new ExtentOr;
	_nodes.push_back( contexts );
	
#line 624 "QueryParser.cpp"
	
	match(O_PAREN);
	first = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 644 "indrilang.g"
		
		Field* firstField = new indri::lang::Field( first->getText() );
		_nodes.push_back( firstField );
		contexts->addChild( firstField );
		
#line 636 "QueryParser.cpp"
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			additional = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 651 "indrilang.g"
				
				Field* additionalField = new Field(additional->getText());
				_nodes.push_back( additionalField );
				contexts->addChild( additionalField );
				
#line 651 "QueryParser.cpp"
			}
		}
		else {
			goto _loop221;
		}
		
	}
	_loop221:;
	} // ( ... )*
	match(C_PAREN);
	return contexts ;
}

 indri::lang::RawExtentNode*  QueryParser::unqualifiedTerm() {
#line 499 "indrilang.g"
	 indri::lang::RawExtentNode* re ;
#line 668 "QueryParser.cpp"
	
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
	case NUMBER:
	case NEGATIVE_NUMBER:
	case FLOAT:
	case DBL_QUOTE:
	case TERM:
	case ENCODED_QUOTED_TERM:
	case ENCODED_TERM:
	{
		re=rawText();
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return re ;
}

 indri::lang::ScoredExtentNode*  QueryParser::weightedList(
	 indri::lang::WeightedCombinationNode* wn, indri::lang::RawExtentNode * ou 
) {
#line 261 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 771 "QueryParser.cpp"
#line 261 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 778 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(wn, ou);
		if ( inputState->guessing==0 ) {
#line 267 "indrilang.g"
			ou = innerMost(sr);
#line 788 "QueryParser.cpp"
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
	int _cnt118=0;
	for (;;) {
		if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
			w=floating();
			n=scoredExtentNode(ou);
			if ( inputState->guessing==0 ) {
#line 272 "indrilang.g"
				wn->addChild( w, n );
#line 812 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt118>=1 ) { goto _loop118; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt118++;
	}
	_loop118:;
	}  // ( ... )+
	match(C_PAREN);
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::extentRestriction(
	 indri::lang::ScoredExtentNode* sn, indri::lang::RawExtentNode * ou 
) {
#line 517 "indrilang.g"
	 indri::lang::ScoredExtentNode* er ;
#line 832 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  passageWindowSize = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  inc = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldRestricted = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 517 "indrilang.g"
	
	indri::lang::Field* f = 0;
	er = 0;
	indri::lang::ExtentInside * po = 0;
	
#line 843 "QueryParser.cpp"
	
	bool synPredMatched202 = false;
	if (((LA(1) == O_SQUARE))) {
		int _m202 = mark();
		synPredMatched202 = true;
		inputState->guessing++;
		try {
			{
			match(O_SQUARE);
			match(TERM);
			match(COLON);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched202 = false;
		}
		rewind(_m202);
		inputState->guessing--;
	}
	if ( synPredMatched202 ) {
		match(O_SQUARE);
		passageWindowSize = LT(1);
		match(TERM);
		match(COLON);
		inc = LT(1);
		match(NUMBER);
		match(C_SQUARE);
		if ( inputState->guessing==0 ) {
#line 523 "indrilang.g"
			
			int startWindow;
			
			for( startWindow = 0; startWindow < passageWindowSize->getText().size(); startWindow++ ) {
			if( isdigit( passageWindowSize->getText()[startWindow] ) )
			break;
			}
			
			int increment = atoi(inc->getText().c_str());
			int windowSize = atoi(passageWindowSize->getText().c_str() + startWindow );
			
			er = new indri::lang::FixedPassage(sn, windowSize, increment);
			
#line 886 "QueryParser.cpp"
		}
	}
	else {
		bool synPredMatched204 = false;
		if (((LA(1) == O_SQUARE))) {
			int _m204 = mark();
			synPredMatched204 = true;
			inputState->guessing++;
			try {
				{
				match(O_SQUARE);
				match(TERM);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched204 = false;
			}
			rewind(_m204);
			inputState->guessing--;
		}
		if ( synPredMatched204 ) {
			match(O_SQUARE);
			field = LT(1);
			match(TERM);
			match(C_SQUARE);
			if ( inputState->guessing==0 ) {
#line 537 "indrilang.g"
				
				f = new indri::lang::Field(field->getText());
				_nodes.push_back(f);
				er = new indri::lang::ExtentRestriction(sn, f);
				_nodes.push_back(er);
				
#line 920 "QueryParser.cpp"
			}
		}
		else if ((LA(1) == O_SQUARE)) {
			match(O_SQUARE);
			match(DOT);
			po=pathOperator();
			fieldRestricted = LT(1);
			match(TERM);
			match(C_SQUARE);
			if ( inputState->guessing==0 ) {
#line 544 "indrilang.g"
				
				
				if ( ou == 0 ) {
				throw new antlr::SemanticException("Use of a . in a extent restriction without a valid outer context.");
				}
				f = new indri::lang::Field(fieldRestricted->getText());
				_nodes.push_back(f);
				po->setInner(f);
				po->setOuter(ou);
				er = new indri::lang::ExtentRestriction(sn, po);
				_nodes.push_back(er);
				
#line 944 "QueryParser.cpp"
			}
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return er ;
}

 double  QueryParser::floating() {
#line 792 "indrilang.g"
	 double d ;
#line 957 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 792 "indrilang.g"
	
	d = 0;
	
#line 964 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 795 "indrilang.g"
			
			d = atof(f->getText().c_str());
			
#line 976 "QueryParser.cpp"
		}
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 798 "indrilang.g"
			
			d = atof(n->getText().c_str());
			
#line 989 "QueryParser.cpp"
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
#line 277 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 1006 "QueryParser.cpp"
#line 277 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 1013 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(wn, ou);
		if ( inputState->guessing==0 ) {
#line 283 "indrilang.g"
			ou = innerMost(sr);
#line 1023 "QueryParser.cpp"
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
	int _cnt122=0;
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			n=scoredExtentNode(ou);
			if ( inputState->guessing==0 ) {
#line 285 "indrilang.g"
				wn->addChild( 1.0, n );
#line 1046 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt122>=1 ) { goto _loop122; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt122++;
	}
	_loop122:;
	}  // ( ... )+
	match(C_PAREN);
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::unweightedList(
	 indri::lang::UnweightedCombinationNode* cn, indri::lang::RawExtentNode * ou 
) {
#line 289 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 1066 "QueryParser.cpp"
#line 289 "indrilang.g"
	
	ScoredExtentNode* n = 0;
	sr = cn;
	
#line 1072 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(cn, ou);
		if ( inputState->guessing==0 ) {
#line 294 "indrilang.g"
			ou = innerMost(sr);
#line 1082 "QueryParser.cpp"
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
	int _cnt126=0;
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			n=scoredExtentNode(ou);
			if ( inputState->guessing==0 ) {
#line 296 "indrilang.g"
				cn->addChild( n );
#line 1105 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt126>=1 ) { goto _loop126; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt126++;
	}
	_loop126:;
	}  // ( ... )+
	match(C_PAREN);
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::sumNode(
	indri::lang::RawExtentNode * ou 
) {
#line 314 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 1125 "QueryParser.cpp"
#line 314 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 1131 "QueryParser.cpp"
	
	match(SUM);
	r=sumList(wn, ou);
	return r ;
}

 indri::lang::WeightedExtentOr*  QueryParser::wsynNode() {
#line 382 "indrilang.g"
	 indri::lang::WeightedExtentOr* ws ;
#line 1141 "QueryParser.cpp"
#line 382 "indrilang.g"
	
	ws = new indri::lang::WeightedExtentOr;
	_nodes.push_back(ws);
	
	double w = 0;
	RawExtentNode* n = 0;
	
#line 1150 "QueryParser.cpp"
	
	match(WSYN);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt139=0;
	for (;;) {
		if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
			w=floating();
			n=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 391 "indrilang.g"
				ws->addChild( w, n );
#line 1163 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt139>=1 ) { goto _loop139; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt139++;
	}
	_loop139:;
	}  // ( ... )+
	match(C_PAREN);
	return ws ;
}

 RawExtentNode*  QueryParser::unscoredTerm() {
#line 475 "indrilang.g"
	 RawExtentNode* t ;
#line 1181 "QueryParser.cpp"
#line 475 "indrilang.g"
	
	t = 0;
	
#line 1186 "QueryParser.cpp"
	
	bool synPredMatched167 = false;
	if (((_tokenSet_1.member(LA(1))))) {
		int _m167 = mark();
		synPredMatched167 = true;
		inputState->guessing++;
		try {
			{
			qualifiedTerm();
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched167 = false;
		}
		rewind(_m167);
		inputState->guessing--;
	}
	if ( synPredMatched167 ) {
		t=qualifiedTerm();
	}
	else if ((_tokenSet_1.member(LA(1)))) {
		t=unqualifiedTerm();
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return t ;
}

 indri::lang::ODNode*  QueryParser::odNode() {
#line 395 "indrilang.g"
	 indri::lang::ODNode* od ;
#line 1220 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 395 "indrilang.g"
	
	RawExtentNode* rn = 0;
	od = new indri::lang::ODNode;
	_nodes.push_back(od);
	
#line 1229 "QueryParser.cpp"
	
	{
	bool synPredMatched143 = false;
	if (((LA(1) == OD))) {
		int _m143 = mark();
		synPredMatched143 = true;
		inputState->guessing++;
		try {
			{
			match(OD);
			match(NUMBER);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched143 = false;
		}
		rewind(_m143);
		inputState->guessing--;
	}
	if ( synPredMatched143 ) {
		{
		match(OD);
		n1 = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 404 "indrilang.g"
			od->setWindowSize( n1->getText() );
#line 1257 "QueryParser.cpp"
		}
		}
	}
	else {
		bool synPredMatched146 = false;
		if (((LA(1) == OD))) {
			int _m146 = mark();
			synPredMatched146 = true;
			inputState->guessing++;
			try {
				{
				match(OD);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched146 = false;
			}
			rewind(_m146);
			inputState->guessing--;
		}
		if ( synPredMatched146 ) {
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
#line 408 "indrilang.g"
				od->setWindowSize( n2->getText() );
#line 1291 "QueryParser.cpp"
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
	int _cnt150=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 412 "indrilang.g"
				od->addChild( rn );
#line 1309 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt150>=1 ) { goto _loop150; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt150++;
	}
	_loop150:;
	}  // ( ... )+
	match(C_PAREN);
	return od ;
}

 indri::lang::UWNode*  QueryParser::uwNode() {
#line 416 "indrilang.g"
	 indri::lang::UWNode* uw ;
#line 1327 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 416 "indrilang.g"
	
	uw = new indri::lang::UWNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(uw);
	
#line 1335 "QueryParser.cpp"
	
	{
	bool synPredMatched154 = false;
	if (((LA(1) == UW))) {
		int _m154 = mark();
		synPredMatched154 = true;
		inputState->guessing++;
		try {
			{
			match(UW);
			match(NUMBER);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched154 = false;
		}
		rewind(_m154);
		inputState->guessing--;
	}
	if ( synPredMatched154 ) {
		{
		match(UW);
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 424 "indrilang.g"
			uw->setWindowSize( n->getText() );
#line 1363 "QueryParser.cpp"
		}
		}
	}
	else if ((LA(1) == UW)) {
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
	int _cnt158=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 430 "indrilang.g"
				uw->addChild( rn );
#line 1386 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt158>=1 ) { goto _loop158; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt158++;
	}
	_loop158:;
	}  // ( ... )+
	match(C_PAREN);
	return uw ;
}

 indri::lang::BAndNode*  QueryParser::bandNode() {
#line 433 "indrilang.g"
	 indri::lang::BAndNode* b ;
#line 1404 "QueryParser.cpp"
#line 433 "indrilang.g"
	
	b = new indri::lang::BAndNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(b);
	
#line 1411 "QueryParser.cpp"
	
	match(BAND);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt161=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 441 "indrilang.g"
				b->addChild( rn );
#line 1423 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt161>=1 ) { goto _loop161; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt161++;
	}
	_loop161:;
	}  // ( ... )+
	match(C_PAREN);
	return b ;
}

 indri::lang::Field*  QueryParser::anyField() {
#line 466 "indrilang.g"
	 indri::lang::Field* f ;
#line 1441 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 466 "indrilang.g"
	
	f = 0;
	
#line 1447 "QueryParser.cpp"
	
	match(ANY);
	match(COLON);
	t = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 470 "indrilang.g"
		
		f = new Field(t->getText());
		_nodes.push_back(f);
		
#line 1459 "QueryParser.cpp"
	}
	return f ;
}

 indri::lang::ExtentAnd*  QueryParser::field_list() {
#line 618 "indrilang.g"
	 indri::lang::ExtentAnd* fields ;
#line 1467 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 618 "indrilang.g"
	
	fields = new ExtentAnd;
	_nodes.push_back( fields );
	
#line 1475 "QueryParser.cpp"
	
	first = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 624 "indrilang.g"
		
		Field* firstField = new indri::lang::Field( first->getText() );
		_nodes.push_back( firstField );
		fields->addChild( firstField );
		
#line 1486 "QueryParser.cpp"
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			additional = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 631 "indrilang.g"
				
				Field* additionalField = new Field(additional->getText());
				_nodes.push_back( additionalField );
				fields->addChild( additionalField );
				
#line 1501 "QueryParser.cpp"
			}
		}
		else {
			goto _loop218;
		}
		
	}
	_loop218:;
	} // ( ... )*
	return fields ;
}

 indri::lang::FieldLessNode*  QueryParser::dateBefore() {
#line 667 "indrilang.g"
	 indri::lang::FieldLessNode* extent ;
#line 1517 "QueryParser.cpp"
#line 667 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1524 "QueryParser.cpp"
	
	match(DATEBEFORE);
	match(O_PAREN);
	d=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 672 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldLessNode( dateField, d );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1538 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldGreaterNode*  QueryParser::dateAfter() {
#line 679 "indrilang.g"
	 indri::lang::FieldGreaterNode* extent ;
#line 1546 "QueryParser.cpp"
#line 679 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1553 "QueryParser.cpp"
	
	match(DATEAFTER);
	match(O_PAREN);
	d=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 684 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldGreaterNode( dateField, d );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1567 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldBetweenNode*  QueryParser::dateBetween() {
#line 691 "indrilang.g"
	 indri::lang::FieldBetweenNode* extent ;
#line 1575 "QueryParser.cpp"
#line 691 "indrilang.g"
	
	UINT64 low = 0;
	UINT64 high = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1583 "QueryParser.cpp"
	
	match(DATEBETWEEN);
	match(O_PAREN);
	low=date();
	match(COMMA);
	high=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 697 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldBetweenNode( dateField, low, high );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1599 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list() {
#line 589 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1607 "QueryParser.cpp"
#line 589 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1614 "QueryParser.cpp"
	
	match(O_ANGLE);
	{ // ( ... )+
	int _cnt209=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 595 "indrilang.g"
				s->addChild(term);
#line 1625 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt209>=1 ) { goto _loop209; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt209++;
	}
	_loop209:;
	}  // ( ... )+
	match(C_ANGLE);
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_brace() {
#line 598 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1643 "QueryParser.cpp"
#line 598 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1650 "QueryParser.cpp"
	
	match(O_BRACE);
	{ // ( ... )+
	int _cnt212=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 604 "indrilang.g"
				s->addChild(term);
#line 1661 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt212>=1 ) { goto _loop212; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt212++;
	}
	_loop212:;
	}  // ( ... )+
	match(C_BRACE);
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_alt() {
#line 607 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1679 "QueryParser.cpp"
#line 607 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	// semantics of this node will change
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1687 "QueryParser.cpp"
	
	match(SYN);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt215=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 615 "indrilang.g"
				s->addChild(term);
#line 1699 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt215>=1 ) { goto _loop215; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt215++;
	}
	_loop215:;
	}  // ( ... )+
	match(C_PAREN);
	return s ;
}

 indri::lang::FieldLessNode*  QueryParser::lessNode() {
#line 824 "indrilang.g"
	 indri::lang::FieldLessNode* ln ;
#line 1717 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 824 "indrilang.g"
	
	ln = 0;
	Field* compareField = 0;
	INT64 high = 0;
	
#line 1725 "QueryParser.cpp"
	
	match(LESS);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	high=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 829 "indrilang.g"
		
		compareField = new Field(field->getText());
		ln = new FieldLessNode( compareField, high );
		_nodes.push_back( compareField );
		_nodes.push_back( ln );
		
#line 1741 "QueryParser.cpp"
	}
	return ln ;
}

 indri::lang::FieldGreaterNode*  QueryParser::greaterNode() {
#line 812 "indrilang.g"
	 indri::lang::FieldGreaterNode* gn ;
#line 1749 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 812 "indrilang.g"
	
	gn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	
#line 1757 "QueryParser.cpp"
	
	match(GREATER);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	low=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 817 "indrilang.g"
		
		compareField = new Field(field->getText());
		gn = new FieldGreaterNode( compareField, low );
		_nodes.push_back( compareField );
		_nodes.push_back( gn );
		
#line 1773 "QueryParser.cpp"
	}
	return gn ;
}

 indri::lang::FieldBetweenNode*  QueryParser::betweenNode() {
#line 836 "indrilang.g"
	 indri::lang::FieldBetweenNode* bn ;
#line 1781 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 836 "indrilang.g"
	
	bn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	INT64 high = 0;
	
#line 1790 "QueryParser.cpp"
	
	match(BETWEEN);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	low=number();
	high=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 842 "indrilang.g"
		
		compareField = new Field(field->getText());
		bn = new FieldBetweenNode( compareField, low, high );
		_nodes.push_back( compareField );
		_nodes.push_back( bn );
		
#line 1807 "QueryParser.cpp"
	}
	return bn ;
}

 indri::lang::FieldEqualsNode*  QueryParser::equalsNode() {
#line 849 "indrilang.g"
	 indri::lang::FieldEqualsNode* en ;
#line 1815 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 849 "indrilang.g"
	
	en = 0;
	Field* compareField = 0;
	INT64 eq = 0;
	
#line 1823 "QueryParser.cpp"
	
	match(EQUALS);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	eq=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 854 "indrilang.g"
		
		compareField = new Field(field->getText());
		en = new FieldEqualsNode( compareField, eq );
		_nodes.push_back( compareField );
		_nodes.push_back( en );
		
#line 1839 "QueryParser.cpp"
	}
	return en ;
}

 indri::lang::IndexTerm*  QueryParser::rawText() {
#line 753 "indrilang.g"
	 indri::lang::IndexTerm* t ;
#line 1847 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nn = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  et = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  qet = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 753 "indrilang.g"
	
	t = 0;
	
#line 1858 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case TERM:
	{
		id = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 756 "indrilang.g"
			
			t = new indri::lang::IndexTerm(id->getText());
			_nodes.push_back(t);
			
#line 1871 "QueryParser.cpp"
		}
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 760 "indrilang.g"
			
			t = new indri::lang::IndexTerm(n->getText());
			_nodes.push_back(t);
			
#line 1885 "QueryParser.cpp"
		}
		break;
	}
	case NEGATIVE_NUMBER:
	{
		nn = LT(1);
		match(NEGATIVE_NUMBER);
		if ( inputState->guessing==0 ) {
#line 764 "indrilang.g"
			
			t = new indri::lang::IndexTerm(nn->getText());
			_nodes.push_back(t);
			
#line 1899 "QueryParser.cpp"
		}
		break;
	}
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 768 "indrilang.g"
			
			t = new indri::lang::IndexTerm(f->getText());
			_nodes.push_back(t);
			
#line 1913 "QueryParser.cpp"
		}
		break;
	}
	case DBL_QUOTE:
	{
		match(DBL_QUOTE);
		t=rawText();
		match(DBL_QUOTE);
		if ( inputState->guessing==0 ) {
#line 772 "indrilang.g"
			
			// if a text term appears in quotes, consider it stemmed
			t->setStemmed(true);
			
#line 1928 "QueryParser.cpp"
		}
		break;
	}
	case ENCODED_TERM:
	{
		et = LT(1);
		match(ENCODED_TERM);
		if ( inputState->guessing==0 ) {
#line 776 "indrilang.g"
			
			std::string decodedString; 
			base64_decode_string(decodedString, et->getText());
			t = new indri::lang::IndexTerm( decodedString );
			_nodes.push_back(t);
			
#line 1944 "QueryParser.cpp"
		}
		break;
	}
	case ENCODED_QUOTED_TERM:
	{
		qet = LT(1);
		match(ENCODED_QUOTED_TERM);
		if ( inputState->guessing==0 ) {
#line 782 "indrilang.g"
			
			std::string decodedString; 
			base64_decode_string(decodedString, qet->getText());
			t = new indri::lang::IndexTerm( decodedString );
			t->setStemmed(true);
			_nodes.push_back(t);
			
#line 1961 "QueryParser.cpp"
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

 indri::lang::ExtentInside*  QueryParser::pathOperator() {
#line 558 "indrilang.g"
	 indri::lang::ExtentInside* r ;
#line 1976 "QueryParser.cpp"
#line 558 "indrilang.g"
	
	r = 0;
	indri::lang::DocumentStructureNode * ds = 0;
	
#line 1982 "QueryParser.cpp"
	
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
#line 562 "indrilang.g"
				
				ds = new indri::lang::DocumentStructureNode;
				_nodes.push_back(ds);
				r = new indri::lang::ExtentDescendant(NULL, NULL, ds);
				_nodes.push_back(r);
				
#line 2001 "QueryParser.cpp"
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
#line 567 "indrilang.g"
			
			if (r == 0) {
			ds = new indri::lang::DocumentStructureNode;
			_nodes.push_back(ds);
			r = new indri::lang::ExtentChild(NULL, NULL, ds);
			_nodes.push_back(r);
			}
			
#line 2025 "QueryParser.cpp"
		}
		break;
	}
	case B_SLASH:
	{
		match(B_SLASH);
		if ( inputState->guessing==0 ) {
#line 575 "indrilang.g"
			
			ds = new indri::lang::DocumentStructureNode;
			_nodes.push_back(ds);
			r = new indri::lang::ExtentParent(NULL, NULL, ds);
			_nodes.push_back(r);
			
#line 2040 "QueryParser.cpp"
		}
		break;
	}
	case O_BRACE:
	{
		match(O_BRACE);
		if ( inputState->guessing==0 ) {
#line 581 "indrilang.g"
			
			r = new indri::lang::ExtentInside(NULL, NULL);   
			_nodes.push_back(r);
			
#line 2053 "QueryParser.cpp"
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
#line 659 "indrilang.g"
	 indri::lang::Field* extent ;
#line 2068 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	match(O_SQUARE);
	fieldName = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 661 "indrilang.g"
		
		extent = new Field( fieldName->getText() );
		_nodes.push_back( extent );
		
#line 2080 "QueryParser.cpp"
	}
	match(C_SQUARE);
	return extent ;
}

 UINT64  QueryParser::date() {
#line 719 "indrilang.g"
	 UINT64 d ;
#line 2089 "QueryParser.cpp"
	
	bool synPredMatched228 = false;
	if (((LA(1) == NUMBER))) {
		int _m228 = mark();
		synPredMatched228 = true;
		inputState->guessing++;
		try {
			{
			match(NUMBER);
			match(SLASH);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched228 = false;
		}
		rewind(_m228);
		inputState->guessing--;
	}
	if ( synPredMatched228 ) {
		d=slashDate();
	}
	else {
		bool synPredMatched230 = false;
		if (((LA(1) == NUMBER))) {
			int _m230 = mark();
			synPredMatched230 = true;
			inputState->guessing++;
			try {
				{
				match(NUMBER);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched230 = false;
			}
			rewind(_m230);
			inputState->guessing--;
		}
		if ( synPredMatched230 ) {
			d=spaceDate();
		}
		else if ((LA(1) == TERM)) {
			d=dashDate();
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return d ;
}

 UINT64  QueryParser::slashDate() {
#line 738 "indrilang.g"
	 UINT64 d ;
#line 2144 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 738 "indrilang.g"
	
	d = 0;
	
#line 2152 "QueryParser.cpp"
	
	month = LT(1);
	match(NUMBER);
	match(SLASH);
	day = LT(1);
	match(TERM);
	match(SLASH);
	year = LT(1);
	match(NUMBER);
	if ( inputState->guessing==0 ) {
#line 741 "indrilang.g"
		
		d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() ); 
		
#line 2167 "QueryParser.cpp"
	}
	return d ;
}

 UINT64  QueryParser::spaceDate() {
#line 745 "indrilang.g"
	 UINT64 d ;
#line 2175 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 745 "indrilang.g"
	
	d = 0;
	
#line 2183 "QueryParser.cpp"
	
	day = LT(1);
	match(NUMBER);
	month = LT(1);
	match(TERM);
	year = LT(1);
	match(NUMBER);
	if ( inputState->guessing==0 ) {
#line 748 "indrilang.g"
		
		d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() );
		
#line 2196 "QueryParser.cpp"
	}
	return d ;
}

 UINT64  QueryParser::dashDate() {
#line 724 "indrilang.g"
	 UINT64 d ;
#line 2204 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  dmy = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 724 "indrilang.g"
	
	d = 0;
	
#line 2210 "QueryParser.cpp"
	
	dmy = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 727 "indrilang.g"
		
		const std::string& text = dmy->getText();
		int firstDash = text.find('-');
		int secondDash = text.find('-', firstDash+1);
		std::string day = text.substr( 0, firstDash ); 
		std::string month = text.substr( firstDash+1, secondDash-firstDash-1 );
		std::string year = text.substr( secondDash );
		
		d = indri::parse::DateParse::convertDate( year, month, day ); 
		
#line 2226 "QueryParser.cpp"
	}
	return d ;
}

 INT64  QueryParser::number() {
#line 802 "indrilang.g"
	 INT64 v ;
#line 2234 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nn = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 802 "indrilang.g"
	
	v = 0;
	
#line 2241 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 805 "indrilang.g"
			
			v = string_to_i64(n->getText());
			
#line 2253 "QueryParser.cpp"
		}
		break;
	}
	case NEGATIVE_NUMBER:
	{
		nn = LT(1);
		match(NEGATIVE_NUMBER);
		if ( inputState->guessing==0 ) {
#line 808 "indrilang.g"
			
			v = string_to_i64(nn->getText());
			
#line 2266 "QueryParser.cpp"
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
	"\"#any\"",
	"\"#band\"",
	"\"#wsyn\"",
	"\"#syn\"",
	"\"#prior\"",
	"\"#date:after\"",
	"\"#date:before\"",
	"\"#date:between\"",
	"\"#less\"",
	"\"#greater\"",
	"\"#between\"",
	"\"#equals\"",
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

const unsigned long QueryParser::_tokenSet_0_data_[] = { 2147483616UL, 2013266244UL, 0UL, 0UL };
// "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#band" "#wsyn" "#syn" "#prior" "#date:after" 
// "#date:before" "#date:between" "#less" "#greater" "#between" "#equals" 
// NUMBER NEGATIVE_NUMBER FLOAT O_ANGLE O_BRACE DBL_QUOTE TERM ENCODED_QUOTED_TERM 
// ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long QueryParser::_tokenSet_1_data_[] = { 2146370688UL, 2013266244UL, 0UL, 0UL };
// "#od" "#uw" "#any" "#band" "#wsyn" "#syn" "#date:after" "#date:before" 
// "#date:between" "#less" "#greater" "#between" "#equals" NUMBER NEGATIVE_NUMBER 
// FLOAT O_ANGLE O_BRACE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM 
// OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_1(_tokenSet_1_data_,4);


ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
