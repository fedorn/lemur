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
#line 168 "indrilang.g"
	 indri::lang::ScoredExtentNode* q ;
#line 37 "QueryParser.cpp"
#line 168 "indrilang.g"
	
	indri::lang::CombineNode* c = 0;
	indri::lang::ScoredExtentNode* s = 0;
	q = 0;
	
#line 44 "QueryParser.cpp"
	
	q=scoredExtentNode();
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
		s=scoredExtentNode();
		if ( inputState->guessing==0 ) {
#line 175 "indrilang.g"
			
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
				s=scoredExtentNode();
				if ( inputState->guessing==0 ) {
#line 183 "indrilang.g"
					
					c->addChild(s);
					
#line 104 "QueryParser.cpp"
				}
			}
			else {
				goto _loop83;
			}
			
		}
		_loop83:;
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

 indri::lang::ScoredExtentNode*  QueryParser::scoredExtentNode() {
#line 189 "indrilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 133 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case WEIGHT:
	{
		s=weightNode();
		break;
	}
	case COMBINE:
	{
		s=combineNode();
		break;
	}
	case OR:
	{
		s=orNode();
		break;
	}
	case NOT:
	{
		s=notNode();
		break;
	}
	case WAND:
	{
		s=wandNode();
		break;
	}
	case WSUM:
	{
		s=wsumNode();
		break;
	}
	case MAX:
	{
		s=maxNode();
		break;
	}
	case PRIOR:
	{
		s=priorNode();
		break;
	}
	case FILREJ:
	{
		s=filrejNode();
		break;
	}
	case FILREQ:
	{
		s=filreqNode();
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

 indri::lang::ScoredExtentNode*  QueryParser::weightNode() {
#line 285 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 224 "QueryParser.cpp"
#line 285 "indrilang.g"
	
	indri::lang::WeightNode* wn = new indri::lang::WeightNode;
	_nodes.push_back(wn);
	
#line 230 "QueryParser.cpp"
	
	match(WEIGHT);
	r=weightedList(wn);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::combineNode() {
#line 292 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 240 "QueryParser.cpp"
#line 292 "indrilang.g"
	
	indri::lang::CombineNode* cn = new indri::lang::CombineNode;
	_nodes.push_back(cn);
	
#line 246 "QueryParser.cpp"
	
	match(COMBINE);
	r=unweightedList(cn);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::orNode() {
#line 320 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 256 "QueryParser.cpp"
#line 320 "indrilang.g"
	
	indri::lang::OrNode* on = new indri::lang::OrNode;
	_nodes.push_back(on);
	
#line 262 "QueryParser.cpp"
	
	match(OR);
	r=unweightedList(on);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::notNode() {
#line 334 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 272 "QueryParser.cpp"
#line 334 "indrilang.g"
	
	indri::lang::NotNode* n = new indri::lang::NotNode;
	indri::lang::ScoredExtentNode* c = 0;
	_nodes.push_back(n);
	r = n;
	
#line 280 "QueryParser.cpp"
	
	match(NOT);
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		r=extentRestriction(r);
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
	c=scoredExtentNode();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 342 "indrilang.g"
		
		n->setChild(c);
		
#line 308 "QueryParser.cpp"
	}
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::wandNode() {
#line 313 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 316 "QueryParser.cpp"
#line 313 "indrilang.g"
	
	indri::lang::WAndNode* wn = new indri::lang::WAndNode;
	_nodes.push_back(wn);
	
#line 322 "QueryParser.cpp"
	
	match(WAND);
	r=weightedList(wn);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::wsumNode() {
#line 306 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 332 "QueryParser.cpp"
#line 306 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 338 "QueryParser.cpp"
	
	match(WSUM);
	r=weightedList(wn);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::maxNode() {
#line 327 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 348 "QueryParser.cpp"
#line 327 "indrilang.g"
	
	indri::lang::MaxNode* mn = new indri::lang::MaxNode;
	_nodes.push_back(mn);
	
#line 354 "QueryParser.cpp"
	
	match(MAX);
	r=unweightedList(mn);
	return r ;
}

 indri::lang::PriorNode*  QueryParser::priorNode() {
#line 346 "indrilang.g"
	 indri::lang::PriorNode* p ;
#line 364 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 346 "indrilang.g"
	
	indri::lang::Field* field = 0;
	p = 0;
	
#line 371 "QueryParser.cpp"
	
	match(PRIOR);
	match(O_PAREN);
	name = LT(1);
	match(TERM);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 351 "indrilang.g"
		
		p = _priorFactory->create( name->getText() );
		
		if( p->getFieldName().length() ) {
		field = new Field( p->getFieldName() );
		p->setField( field );
		_nodes.push_back(field);
		}
		
		_nodes.push_back(p);
		
#line 391 "QueryParser.cpp"
	}
	return p ;
}

 indri::lang::FilRejNode*  QueryParser::filrejNode() {
#line 436 "indrilang.g"
	 indri::lang::FilRejNode* fj ;
#line 399 "QueryParser.cpp"
#line 436 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* disallowed = 0;
	
#line 405 "QueryParser.cpp"
	
	match(FILREJ);
	match(O_PAREN);
	filter=unscoredTerm();
	disallowed=scoredExtentNode();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 442 "indrilang.g"
		
		fj = new FilRejNode( filter, disallowed );
		_nodes.push_back(fj);
		
#line 418 "QueryParser.cpp"
	}
	return fj ;
}

 indri::lang::FilReqNode*  QueryParser::filreqNode() {
#line 447 "indrilang.g"
	 indri::lang::FilReqNode* fq ;
#line 426 "QueryParser.cpp"
#line 447 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* required = 0;
	
#line 432 "QueryParser.cpp"
	
	match(FILREQ);
	match(O_PAREN);
	filter=unscoredTerm();
	required=scoredExtentNode();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 453 "indrilang.g"
		
		fq = new FilReqNode( filter, required );
		_nodes.push_back(fq);
		
#line 445 "QueryParser.cpp"
	}
	return fq ;
}

 indri::lang::ScoredExtentNode*  QueryParser::scoredRaw() {
#line 208 "indrilang.g"
	 indri::lang::ScoredExtentNode* sn ;
#line 453 "QueryParser.cpp"
#line 208 "indrilang.g"
	
	RawExtentNode* raw = 0;
	RawExtentNode* contexts = 0;
	sn = 0;
	
#line 460 "QueryParser.cpp"
	
	bool synPredMatched107 = false;
	if (((_tokenSet_1.member(LA(1))))) {
		int _m107 = mark();
		synPredMatched107 = true;
		inputState->guessing++;
		try {
			{
			qualifiedTerm();
			match(DOT);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched107 = false;
		}
		rewind(_m107);
		inputState->guessing--;
	}
	if ( synPredMatched107 ) {
		raw=qualifiedTerm();
		match(DOT);
		contexts=context_list();
		if ( inputState->guessing==0 ) {
#line 215 "indrilang.g"
			
			sn = new indri::lang::RawScorerNode( raw, contexts );
			_nodes.push_back(sn);
			
#line 489 "QueryParser.cpp"
		}
	}
	else {
		bool synPredMatched109 = false;
		if (((_tokenSet_1.member(LA(1))))) {
			int _m109 = mark();
			synPredMatched109 = true;
			inputState->guessing++;
			try {
				{
				qualifiedTerm();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched109 = false;
			}
			rewind(_m109);
			inputState->guessing--;
		}
		if ( synPredMatched109 ) {
			raw=qualifiedTerm();
			if ( inputState->guessing==0 ) {
#line 220 "indrilang.g"
				
				sn = new indri::lang::RawScorerNode( raw, contexts );
				_nodes.push_back(sn);
				
#line 517 "QueryParser.cpp"
			}
		}
		else {
			bool synPredMatched111 = false;
			if (((_tokenSet_1.member(LA(1))))) {
				int _m111 = mark();
				synPredMatched111 = true;
				inputState->guessing++;
				try {
					{
					unqualifiedTerm();
					match(DOT);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched111 = false;
				}
				rewind(_m111);
				inputState->guessing--;
			}
			if ( synPredMatched111 ) {
				raw=unqualifiedTerm();
				match(DOT);
				contexts=context_list();
				if ( inputState->guessing==0 ) {
#line 225 "indrilang.g"
					
					sn = new indri::lang::RawScorerNode( raw, contexts );
					_nodes.push_back(sn);
					
#line 548 "QueryParser.cpp"
				}
			}
			else if ((_tokenSet_1.member(LA(1)))) {
				raw=unqualifiedTerm();
				if ( inputState->guessing==0 ) {
#line 230 "indrilang.g"
					
					sn = new indri::lang::RawScorerNode( raw, contexts );
					_nodes.push_back(sn);
					
#line 559 "QueryParser.cpp"
				}
			}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}}
	return sn ;
}

 RawExtentNode*  QueryParser::qualifiedTerm() {
#line 474 "indrilang.g"
	 RawExtentNode* t ;
#line 572 "QueryParser.cpp"
#line 474 "indrilang.g"
	
	RawExtentNode* synonyms = 0;
	RawExtentNode* fields = 0;
	t = 0;
	
#line 579 "QueryParser.cpp"
	
	synonyms=unqualifiedTerm();
	match(DOT);
	fields=field_list();
	if ( inputState->guessing==0 ) {
#line 481 "indrilang.g"
		
		if( fields ) {
		t = new indri::lang::ExtentInside( synonyms, fields );
		_nodes.push_back(t);
		synonyms = t;
		} else {
		t = synonyms;
		}
		
#line 595 "QueryParser.cpp"
	}
	return t ;
}

 ExtentOr*  QueryParser::context_list() {
#line 584 "indrilang.g"
	 ExtentOr* contexts ;
#line 603 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 584 "indrilang.g"
	
	contexts = new ExtentOr;
	_nodes.push_back( contexts );
	
#line 611 "QueryParser.cpp"
	
	match(O_PAREN);
	first = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 590 "indrilang.g"
		
		Field* firstField = new indri::lang::Field( first->getText() );
		_nodes.push_back( firstField );
		contexts->addChild( firstField );
		
#line 623 "QueryParser.cpp"
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			additional = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 597 "indrilang.g"
				
				Field* additionalField = new Field(additional->getText());
				_nodes.push_back( additionalField );
				contexts->addChild( additionalField );
				
#line 638 "QueryParser.cpp"
			}
		}
		else {
			goto _loop214;
		}
		
	}
	_loop214:;
	} // ( ... )*
	match(C_PAREN);
	return contexts ;
}

 indri::lang::RawExtentNode*  QueryParser::unqualifiedTerm() {
#line 491 "indrilang.g"
	 indri::lang::RawExtentNode* re ;
#line 655 "QueryParser.cpp"
	
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
	 indri::lang::WeightedCombinationNode* wn 
) {
#line 246 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 758 "QueryParser.cpp"
#line 246 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 765 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(wn);
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
	int _cnt115=0;
	for (;;) {
		if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
			w=floating();
			n=scoredExtentNode();
			if ( inputState->guessing==0 ) {
#line 257 "indrilang.g"
				wn->addChild( w, n );
#line 794 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt115>=1 ) { goto _loop115; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt115++;
	}
	_loop115:;
	}  // ( ... )+
	match(C_PAREN);
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::extentRestriction(
	 indri::lang::ScoredExtentNode* sn 
) {
#line 509 "indrilang.g"
	 indri::lang::ScoredExtentNode* er ;
#line 814 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  passageWindowSize = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  inc = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 509 "indrilang.g"
	
	indri::lang::Field* f = 0;
	er = 0;
	
#line 823 "QueryParser.cpp"
	
	bool synPredMatched199 = false;
	if (((LA(1) == O_SQUARE))) {
		int _m199 = mark();
		synPredMatched199 = true;
		inputState->guessing++;
		try {
			{
			match(O_SQUARE);
			match(TERM);
			match(COLON);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched199 = false;
		}
		rewind(_m199);
		inputState->guessing--;
	}
	if ( synPredMatched199 ) {
		match(O_SQUARE);
		passageWindowSize = LT(1);
		match(TERM);
		match(COLON);
		inc = LT(1);
		match(NUMBER);
		match(C_SQUARE);
		if ( inputState->guessing==0 ) {
#line 514 "indrilang.g"
			
			int startWindow;
			
			for( startWindow = 0; startWindow < passageWindowSize->getText().size(); startWindow++ ) {
			if( isdigit( passageWindowSize->getText()[startWindow] ) )
			break;
			}
			
			int increment = atoi(inc->getText().c_str());
			int windowSize = atoi(passageWindowSize->getText().c_str() + startWindow );
			
			er = new indri::lang::FixedPassage(sn, windowSize, increment);
			
#line 866 "QueryParser.cpp"
		}
	}
	else if ((LA(1) == O_SQUARE)) {
		match(O_SQUARE);
		field = LT(1);
		match(TERM);
		match(C_SQUARE);
		if ( inputState->guessing==0 ) {
#line 528 "indrilang.g"
			
			f = new indri::lang::Field(field->getText());
			_nodes.push_back(f);
			er = new indri::lang::ExtentRestriction(sn, f);
			_nodes.push_back(er);
			
#line 882 "QueryParser.cpp"
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return er ;
}

 double  QueryParser::floating() {
#line 738 "indrilang.g"
	 double d ;
#line 895 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 738 "indrilang.g"
	
	d = 0;
	
#line 902 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 741 "indrilang.g"
			
			d = atof(f->getText().c_str());
			
#line 914 "QueryParser.cpp"
		}
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 744 "indrilang.g"
			
			d = atof(n->getText().c_str());
			
#line 927 "QueryParser.cpp"
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
	 indri::lang::WSumNode* wn 
) {
#line 262 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 944 "QueryParser.cpp"
#line 262 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 951 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(wn);
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
	int _cnt119=0;
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			n=scoredExtentNode();
			if ( inputState->guessing==0 ) {
#line 270 "indrilang.g"
				wn->addChild( 1.0, n );
#line 979 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt119>=1 ) { goto _loop119; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt119++;
	}
	_loop119:;
	}  // ( ... )+
	match(C_PAREN);
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::unweightedList(
	 indri::lang::UnweightedCombinationNode* cn 
) {
#line 274 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 999 "QueryParser.cpp"
#line 274 "indrilang.g"
	
	ScoredExtentNode* n = 0;
	sr = cn;
	
#line 1005 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(cn);
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
	int _cnt123=0;
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			n=scoredExtentNode();
			if ( inputState->guessing==0 ) {
#line 281 "indrilang.g"
				cn->addChild( n );
#line 1033 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt123>=1 ) { goto _loop123; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt123++;
	}
	_loop123:;
	}  // ( ... )+
	match(C_PAREN);
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::sumNode() {
#line 299 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 1051 "QueryParser.cpp"
#line 299 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 1057 "QueryParser.cpp"
	
	match(SUM);
	r=sumList(wn);
	return r ;
}

 indri::lang::WeightedExtentOr*  QueryParser::wsynNode() {
#line 374 "indrilang.g"
	 indri::lang::WeightedExtentOr* ws ;
#line 1067 "QueryParser.cpp"
#line 374 "indrilang.g"
	
	ws = new indri::lang::WeightedExtentOr;
	_nodes.push_back(ws);
	
	double w = 0;
	RawExtentNode* n = 0;
	
#line 1076 "QueryParser.cpp"
	
	match(WSYN);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt136=0;
	for (;;) {
		if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
			w=floating();
			n=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 383 "indrilang.g"
				ws->addChild( w, n );
#line 1089 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt136>=1 ) { goto _loop136; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt136++;
	}
	_loop136:;
	}  // ( ... )+
	match(C_PAREN);
	return ws ;
}

 RawExtentNode*  QueryParser::unscoredTerm() {
#line 467 "indrilang.g"
	 RawExtentNode* t ;
#line 1107 "QueryParser.cpp"
#line 467 "indrilang.g"
	
	t = 0;
	
#line 1112 "QueryParser.cpp"
	
	bool synPredMatched164 = false;
	if (((_tokenSet_1.member(LA(1))))) {
		int _m164 = mark();
		synPredMatched164 = true;
		inputState->guessing++;
		try {
			{
			qualifiedTerm();
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched164 = false;
		}
		rewind(_m164);
		inputState->guessing--;
	}
	if ( synPredMatched164 ) {
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
#line 387 "indrilang.g"
	 indri::lang::ODNode* od ;
#line 1146 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 387 "indrilang.g"
	
	RawExtentNode* rn = 0;
	od = new indri::lang::ODNode;
	_nodes.push_back(od);
	
#line 1155 "QueryParser.cpp"
	
	{
	bool synPredMatched140 = false;
	if (((LA(1) == OD))) {
		int _m140 = mark();
		synPredMatched140 = true;
		inputState->guessing++;
		try {
			{
			match(OD);
			match(NUMBER);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched140 = false;
		}
		rewind(_m140);
		inputState->guessing--;
	}
	if ( synPredMatched140 ) {
		{
		match(OD);
		n1 = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 396 "indrilang.g"
			od->setWindowSize( n1->getText() );
#line 1183 "QueryParser.cpp"
		}
		}
	}
	else {
		bool synPredMatched143 = false;
		if (((LA(1) == OD))) {
			int _m143 = mark();
			synPredMatched143 = true;
			inputState->guessing++;
			try {
				{
				match(OD);
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
			}
		}
		else if ((LA(1) == OPERATOR)) {
			{
			match(OPERATOR);
			n2 = LT(1);
			match(NUMBER);
			if ( inputState->guessing==0 ) {
#line 400 "indrilang.g"
				od->setWindowSize( n2->getText() );
#line 1217 "QueryParser.cpp"
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
	int _cnt147=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 404 "indrilang.g"
				od->addChild( rn );
#line 1235 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt147>=1 ) { goto _loop147; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt147++;
	}
	_loop147:;
	}  // ( ... )+
	match(C_PAREN);
	return od ;
}

 indri::lang::UWNode*  QueryParser::uwNode() {
#line 408 "indrilang.g"
	 indri::lang::UWNode* uw ;
#line 1253 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 408 "indrilang.g"
	
	uw = new indri::lang::UWNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(uw);
	
#line 1261 "QueryParser.cpp"
	
	{
	bool synPredMatched151 = false;
	if (((LA(1) == UW))) {
		int _m151 = mark();
		synPredMatched151 = true;
		inputState->guessing++;
		try {
			{
			match(UW);
			match(NUMBER);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched151 = false;
		}
		rewind(_m151);
		inputState->guessing--;
	}
	if ( synPredMatched151 ) {
		{
		match(UW);
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 416 "indrilang.g"
			uw->setWindowSize( n->getText() );
#line 1289 "QueryParser.cpp"
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
	int _cnt155=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 422 "indrilang.g"
				uw->addChild( rn );
#line 1312 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt155>=1 ) { goto _loop155; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt155++;
	}
	_loop155:;
	}  // ( ... )+
	match(C_PAREN);
	return uw ;
}

 indri::lang::BAndNode*  QueryParser::bandNode() {
#line 425 "indrilang.g"
	 indri::lang::BAndNode* b ;
#line 1330 "QueryParser.cpp"
#line 425 "indrilang.g"
	
	b = new indri::lang::BAndNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(b);
	
#line 1337 "QueryParser.cpp"
	
	match(BAND);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt158=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 433 "indrilang.g"
				b->addChild( rn );
#line 1349 "QueryParser.cpp"
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
	return b ;
}

 indri::lang::Field*  QueryParser::anyField() {
#line 458 "indrilang.g"
	 indri::lang::Field* f ;
#line 1367 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 458 "indrilang.g"
	
	f = 0;
	
#line 1373 "QueryParser.cpp"
	
	match(ANY);
	match(COLON);
	t = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 462 "indrilang.g"
		
		f = new Field(t->getText());
		_nodes.push_back(f);
		
#line 1385 "QueryParser.cpp"
	}
	return f ;
}

 indri::lang::ExtentAnd*  QueryParser::field_list() {
#line 564 "indrilang.g"
	 indri::lang::ExtentAnd* fields ;
#line 1393 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 564 "indrilang.g"
	
	fields = new ExtentAnd;
	_nodes.push_back( fields );
	
#line 1401 "QueryParser.cpp"
	
	first = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 570 "indrilang.g"
		
		Field* firstField = new indri::lang::Field( first->getText() );
		_nodes.push_back( firstField );
		fields->addChild( firstField );
		
#line 1412 "QueryParser.cpp"
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			additional = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 577 "indrilang.g"
				
				Field* additionalField = new Field(additional->getText());
				_nodes.push_back( additionalField );
				fields->addChild( additionalField );
				
#line 1427 "QueryParser.cpp"
			}
		}
		else {
			goto _loop211;
		}
		
	}
	_loop211:;
	} // ( ... )*
	return fields ;
}

 indri::lang::FieldLessNode*  QueryParser::dateBefore() {
#line 613 "indrilang.g"
	 indri::lang::FieldLessNode* extent ;
#line 1443 "QueryParser.cpp"
#line 613 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1450 "QueryParser.cpp"
	
	match(DATEBEFORE);
	match(O_PAREN);
	d=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 618 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldLessNode( dateField, d );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1464 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldGreaterNode*  QueryParser::dateAfter() {
#line 625 "indrilang.g"
	 indri::lang::FieldGreaterNode* extent ;
#line 1472 "QueryParser.cpp"
#line 625 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1479 "QueryParser.cpp"
	
	match(DATEAFTER);
	match(O_PAREN);
	d=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 630 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldGreaterNode( dateField, d );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1493 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldBetweenNode*  QueryParser::dateBetween() {
#line 637 "indrilang.g"
	 indri::lang::FieldBetweenNode* extent ;
#line 1501 "QueryParser.cpp"
#line 637 "indrilang.g"
	
	UINT64 low = 0;
	UINT64 high = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1509 "QueryParser.cpp"
	
	match(DATEBETWEEN);
	match(O_PAREN);
	low=date();
	match(COMMA);
	high=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 643 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldBetweenNode( dateField, low, high );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1525 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list() {
#line 535 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1533 "QueryParser.cpp"
#line 535 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1540 "QueryParser.cpp"
	
	match(O_ANGLE);
	{ // ( ... )+
	int _cnt202=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 541 "indrilang.g"
				s->addChild(term);
#line 1551 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt202>=1 ) { goto _loop202; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt202++;
	}
	_loop202:;
	}  // ( ... )+
	match(C_ANGLE);
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_brace() {
#line 544 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1569 "QueryParser.cpp"
#line 544 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1576 "QueryParser.cpp"
	
	match(O_BRACE);
	{ // ( ... )+
	int _cnt205=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 550 "indrilang.g"
				s->addChild(term);
#line 1587 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt205>=1 ) { goto _loop205; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt205++;
	}
	_loop205:;
	}  // ( ... )+
	match(C_BRACE);
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_alt() {
#line 553 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1605 "QueryParser.cpp"
#line 553 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	// semantics of this node will change
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1613 "QueryParser.cpp"
	
	match(SYN);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt208=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 561 "indrilang.g"
				s->addChild(term);
#line 1625 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt208>=1 ) { goto _loop208; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt208++;
	}
	_loop208:;
	}  // ( ... )+
	match(C_PAREN);
	return s ;
}

 indri::lang::FieldLessNode*  QueryParser::lessNode() {
#line 770 "indrilang.g"
	 indri::lang::FieldLessNode* ln ;
#line 1643 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 770 "indrilang.g"
	
	ln = 0;
	Field* compareField = 0;
	INT64 high = 0;
	
#line 1651 "QueryParser.cpp"
	
	match(LESS);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	high=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 775 "indrilang.g"
		
		compareField = new Field(field->getText());
		ln = new FieldLessNode( compareField, high );
		_nodes.push_back( compareField );
		_nodes.push_back( ln );
		
#line 1667 "QueryParser.cpp"
	}
	return ln ;
}

 indri::lang::FieldGreaterNode*  QueryParser::greaterNode() {
#line 758 "indrilang.g"
	 indri::lang::FieldGreaterNode* gn ;
#line 1675 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 758 "indrilang.g"
	
	gn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	
#line 1683 "QueryParser.cpp"
	
	match(GREATER);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	low=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 763 "indrilang.g"
		
		compareField = new Field(field->getText());
		gn = new FieldGreaterNode( compareField, low );
		_nodes.push_back( compareField );
		_nodes.push_back( gn );
		
#line 1699 "QueryParser.cpp"
	}
	return gn ;
}

 indri::lang::FieldBetweenNode*  QueryParser::betweenNode() {
#line 782 "indrilang.g"
	 indri::lang::FieldBetweenNode* bn ;
#line 1707 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 782 "indrilang.g"
	
	bn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	INT64 high = 0;
	
#line 1716 "QueryParser.cpp"
	
	match(BETWEEN);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	low=number();
	high=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 788 "indrilang.g"
		
		compareField = new Field(field->getText());
		bn = new FieldBetweenNode( compareField, low, high );
		_nodes.push_back( compareField );
		_nodes.push_back( bn );
		
#line 1733 "QueryParser.cpp"
	}
	return bn ;
}

 indri::lang::FieldEqualsNode*  QueryParser::equalsNode() {
#line 795 "indrilang.g"
	 indri::lang::FieldEqualsNode* en ;
#line 1741 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 795 "indrilang.g"
	
	en = 0;
	Field* compareField = 0;
	INT64 eq = 0;
	
#line 1749 "QueryParser.cpp"
	
	match(EQUALS);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	eq=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 800 "indrilang.g"
		
		compareField = new Field(field->getText());
		en = new FieldEqualsNode( compareField, eq );
		_nodes.push_back( compareField );
		_nodes.push_back( en );
		
#line 1765 "QueryParser.cpp"
	}
	return en ;
}

 indri::lang::IndexTerm*  QueryParser::rawText() {
#line 699 "indrilang.g"
	 indri::lang::IndexTerm* t ;
#line 1773 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nn = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  et = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  qet = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 699 "indrilang.g"
	
	t = 0;
	
#line 1784 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case TERM:
	{
		id = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 702 "indrilang.g"
			
			t = new indri::lang::IndexTerm(id->getText());
			_nodes.push_back(t);
			
#line 1797 "QueryParser.cpp"
		}
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 706 "indrilang.g"
			
			t = new indri::lang::IndexTerm(n->getText());
			_nodes.push_back(t);
			
#line 1811 "QueryParser.cpp"
		}
		break;
	}
	case NEGATIVE_NUMBER:
	{
		nn = LT(1);
		match(NEGATIVE_NUMBER);
		if ( inputState->guessing==0 ) {
#line 710 "indrilang.g"
			
			t = new indri::lang::IndexTerm(nn->getText());
			_nodes.push_back(t);
			
#line 1825 "QueryParser.cpp"
		}
		break;
	}
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 714 "indrilang.g"
			
			t = new indri::lang::IndexTerm(f->getText());
			_nodes.push_back(t);
			
#line 1839 "QueryParser.cpp"
		}
		break;
	}
	case DBL_QUOTE:
	{
		match(DBL_QUOTE);
		t=rawText();
		match(DBL_QUOTE);
		if ( inputState->guessing==0 ) {
#line 718 "indrilang.g"
			
			// if a text term appears in quotes, consider it stemmed
			t->setStemmed(true);
			
#line 1854 "QueryParser.cpp"
		}
		break;
	}
	case ENCODED_TERM:
	{
		et = LT(1);
		match(ENCODED_TERM);
		if ( inputState->guessing==0 ) {
#line 722 "indrilang.g"
			
			std::string decodedString; 
			base64_decode_string(decodedString, et->getText());
			t = new indri::lang::IndexTerm( decodedString );
			_nodes.push_back(t);
			
#line 1870 "QueryParser.cpp"
		}
		break;
	}
	case ENCODED_QUOTED_TERM:
	{
		qet = LT(1);
		match(ENCODED_QUOTED_TERM);
		if ( inputState->guessing==0 ) {
#line 728 "indrilang.g"
			
			std::string decodedString; 
			base64_decode_string(decodedString, qet->getText());
			t = new indri::lang::IndexTerm( decodedString );
			t->setStemmed(true);
			_nodes.push_back(t);
			
#line 1887 "QueryParser.cpp"
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

 indri::lang::Field*  QueryParser::field_restriction() {
#line 605 "indrilang.g"
	 indri::lang::Field* extent ;
#line 1902 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	match(O_SQUARE);
	fieldName = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 607 "indrilang.g"
		
		extent = new Field( fieldName->getText() );
		_nodes.push_back( extent );
		
#line 1914 "QueryParser.cpp"
	}
	match(C_SQUARE);
	return extent ;
}

 UINT64  QueryParser::date() {
#line 665 "indrilang.g"
	 UINT64 d ;
#line 1923 "QueryParser.cpp"
	
	bool synPredMatched221 = false;
	if (((LA(1) == NUMBER))) {
		int _m221 = mark();
		synPredMatched221 = true;
		inputState->guessing++;
		try {
			{
			match(NUMBER);
			match(SLASH);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched221 = false;
		}
		rewind(_m221);
		inputState->guessing--;
	}
	if ( synPredMatched221 ) {
		d=slashDate();
	}
	else {
		bool synPredMatched223 = false;
		if (((LA(1) == NUMBER))) {
			int _m223 = mark();
			synPredMatched223 = true;
			inputState->guessing++;
			try {
				{
				match(NUMBER);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched223 = false;
			}
			rewind(_m223);
			inputState->guessing--;
		}
		if ( synPredMatched223 ) {
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
#line 684 "indrilang.g"
	 UINT64 d ;
#line 1978 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 684 "indrilang.g"
	
	d = 0;
	
#line 1986 "QueryParser.cpp"
	
	month = LT(1);
	match(NUMBER);
	match(SLASH);
	day = LT(1);
	match(TERM);
	match(SLASH);
	year = LT(1);
	match(NUMBER);
	if ( inputState->guessing==0 ) {
#line 687 "indrilang.g"
		
		d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() ); 
		
#line 2001 "QueryParser.cpp"
	}
	return d ;
}

 UINT64  QueryParser::spaceDate() {
#line 691 "indrilang.g"
	 UINT64 d ;
#line 2009 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 691 "indrilang.g"
	
	d = 0;
	
#line 2017 "QueryParser.cpp"
	
	day = LT(1);
	match(NUMBER);
	month = LT(1);
	match(TERM);
	year = LT(1);
	match(NUMBER);
	if ( inputState->guessing==0 ) {
#line 694 "indrilang.g"
		
		d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() );
		
#line 2030 "QueryParser.cpp"
	}
	return d ;
}

 UINT64  QueryParser::dashDate() {
#line 670 "indrilang.g"
	 UINT64 d ;
#line 2038 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  dmy = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 670 "indrilang.g"
	
	d = 0;
	
#line 2044 "QueryParser.cpp"
	
	dmy = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 673 "indrilang.g"
		
		const std::string& text = dmy->getText();
		int firstDash = text.find('-');
		int secondDash = text.find('-', firstDash+1);
		std::string day = text.substr( 0, firstDash ); 
		std::string month = text.substr( firstDash+1, secondDash-firstDash-1 );
		std::string year = text.substr( secondDash );
		
		d = indri::parse::DateParse::convertDate( year, month, day ); 
		
#line 2060 "QueryParser.cpp"
	}
	return d ;
}

 INT64  QueryParser::number() {
#line 748 "indrilang.g"
	 INT64 v ;
#line 2068 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nn = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 748 "indrilang.g"
	
	v = 0;
	
#line 2075 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 751 "indrilang.g"
			
			v = string_to_i64(n->getText());
			
#line 2087 "QueryParser.cpp"
		}
		break;
	}
	case NEGATIVE_NUMBER:
	{
		nn = LT(1);
		match(NEGATIVE_NUMBER);
		if ( inputState->guessing==0 ) {
#line 754 "indrilang.g"
			
			v = string_to_i64(nn->getText());
			
#line 2100 "QueryParser.cpp"
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
	"SLASH",
	0
};

const unsigned long QueryParser::_tokenSet_0_data_[] = { 2147483616UL, 503316804UL, 0UL, 0UL };
// "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#band" "#wsyn" "#syn" "#prior" "#date:after" 
// "#date:before" "#date:between" "#less" "#greater" "#between" "#equals" 
// NUMBER NEGATIVE_NUMBER FLOAT O_ANGLE O_BRACE DBL_QUOTE TERM ENCODED_QUOTED_TERM 
// ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long QueryParser::_tokenSet_1_data_[] = { 2146370688UL, 503316804UL, 0UL, 0UL };
// "#od" "#uw" "#any" "#band" "#wsyn" "#syn" "#date:after" "#date:before" 
// "#date:between" "#less" "#greater" "#between" "#equals" NUMBER NEGATIVE_NUMBER 
// FLOAT O_ANGLE O_BRACE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM 
// OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_1(_tokenSet_1_data_,4);


ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
