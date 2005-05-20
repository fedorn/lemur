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
#line 169 "indrilang.g"
	 indri::lang::ScoredExtentNode* q ;
#line 37 "QueryParser.cpp"
#line 169 "indrilang.g"
	
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
#line 176 "indrilang.g"
			
			c = new CombineNode;
			c->addChild(q);
			c->addChild(s);
			_nodes.push_back(c);
			q = c;
			
#line 92 "QueryParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				s=scoredExtentNode();
				if ( inputState->guessing==0 ) {
#line 184 "indrilang.g"
					
					c->addChild(s);
					
#line 103 "QueryParser.cpp"
				}
			}
			else {
				goto _loop84;
			}
			
		}
		_loop84:;
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
#line 190 "indrilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 132 "QueryParser.cpp"
	
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
#line 286 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 222 "QueryParser.cpp"
#line 286 "indrilang.g"
	
	indri::lang::WeightNode* wn = new indri::lang::WeightNode;
	_nodes.push_back(wn);
	
#line 228 "QueryParser.cpp"
	
	match(WEIGHT);
	r=weightedList(wn);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::combineNode() {
#line 293 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 238 "QueryParser.cpp"
#line 293 "indrilang.g"
	
	indri::lang::CombineNode* cn = new indri::lang::CombineNode;
	_nodes.push_back(cn);
	
#line 244 "QueryParser.cpp"
	
	match(COMBINE);
	r=unweightedList(cn);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::orNode() {
#line 321 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 254 "QueryParser.cpp"
#line 321 "indrilang.g"
	
	indri::lang::OrNode* on = new indri::lang::OrNode;
	_nodes.push_back(on);
	
#line 260 "QueryParser.cpp"
	
	match(OR);
	r=unweightedList(on);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::notNode() {
#line 335 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 270 "QueryParser.cpp"
#line 335 "indrilang.g"
	
	indri::lang::NotNode* n = new indri::lang::NotNode;
	indri::lang::ScoredExtentNode* c = 0;
	_nodes.push_back(n);
	r = n;
	
#line 278 "QueryParser.cpp"
	
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
#line 343 "indrilang.g"
		
		n->setChild(c);
		
#line 306 "QueryParser.cpp"
	}
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::wandNode() {
#line 314 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 314 "QueryParser.cpp"
#line 314 "indrilang.g"
	
	indri::lang::WAndNode* wn = new indri::lang::WAndNode;
	_nodes.push_back(wn);
	
#line 320 "QueryParser.cpp"
	
	match(WAND);
	r=weightedList(wn);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::wsumNode() {
#line 307 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 330 "QueryParser.cpp"
#line 307 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 336 "QueryParser.cpp"
	
	match(WSUM);
	r=weightedList(wn);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::maxNode() {
#line 328 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 346 "QueryParser.cpp"
#line 328 "indrilang.g"
	
	indri::lang::MaxNode* mn = new indri::lang::MaxNode;
	_nodes.push_back(mn);
	
#line 352 "QueryParser.cpp"
	
	match(MAX);
	r=unweightedList(mn);
	return r ;
}

 indri::lang::PriorNode*  QueryParser::priorNode() {
#line 347 "indrilang.g"
	 indri::lang::PriorNode* p ;
#line 362 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 347 "indrilang.g"
	
	indri::lang::Field* field = 0;
	p = 0;
	
#line 369 "QueryParser.cpp"
	
	match(PRIOR);
	match(O_PAREN);
	name = LT(1);
	match(TERM);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 352 "indrilang.g"
		
		p = _priorFactory->create( name->getText() );
		
		if( p->getFieldName().length() ) {
		field = new Field( p->getFieldName() );
		p->setField( field );
		_nodes.push_back(field);
		}
		
		_nodes.push_back(p);
		
#line 389 "QueryParser.cpp"
	}
	return p ;
}

 indri::lang::FilRejNode*  QueryParser::filrejNode() {
#line 437 "indrilang.g"
	 indri::lang::FilRejNode* fj ;
#line 397 "QueryParser.cpp"
#line 437 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* disallowed = 0;
	
#line 403 "QueryParser.cpp"
	
	match(FILREJ);
	match(O_PAREN);
	filter=unscoredTerm();
	disallowed=scoredExtentNode();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 443 "indrilang.g"
		
		fj = new FilRejNode( filter, disallowed );
		_nodes.push_back(fj);
		
#line 416 "QueryParser.cpp"
	}
	return fj ;
}

 indri::lang::FilReqNode*  QueryParser::filreqNode() {
#line 448 "indrilang.g"
	 indri::lang::FilReqNode* fq ;
#line 424 "QueryParser.cpp"
#line 448 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* required = 0;
	
#line 430 "QueryParser.cpp"
	
	match(FILREQ);
	match(O_PAREN);
	filter=unscoredTerm();
	required=scoredExtentNode();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 454 "indrilang.g"
		
		fq = new FilReqNode( filter, required );
		_nodes.push_back(fq);
		
#line 443 "QueryParser.cpp"
	}
	return fq ;
}

 indri::lang::ScoredExtentNode*  QueryParser::scoredRaw() {
#line 209 "indrilang.g"
	 indri::lang::ScoredExtentNode* sn ;
#line 451 "QueryParser.cpp"
#line 209 "indrilang.g"
	
	RawExtentNode* raw = 0;
	RawExtentNode* contexts = 0;
	sn = 0;
	
#line 458 "QueryParser.cpp"
	
	bool synPredMatched108 = false;
	if (((_tokenSet_1.member(LA(1))))) {
		int _m108 = mark();
		synPredMatched108 = true;
		inputState->guessing++;
		try {
			{
			qualifiedTerm();
			context_list();
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched108 = false;
		}
		rewind(_m108);
		inputState->guessing--;
	}
	if ( synPredMatched108 ) {
		raw=qualifiedTerm();
		contexts=context_list();
		if ( inputState->guessing==0 ) {
#line 216 "indrilang.g"
			
			sn = new indri::lang::RawScorerNode( raw, contexts );
			_nodes.push_back(sn);
			
#line 486 "QueryParser.cpp"
		}
	}
	else {
		bool synPredMatched110 = false;
		if (((_tokenSet_1.member(LA(1))))) {
			int _m110 = mark();
			synPredMatched110 = true;
			inputState->guessing++;
			try {
				{
				qualifiedTerm();
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
			if ( inputState->guessing==0 ) {
#line 221 "indrilang.g"
				
				sn = new indri::lang::RawScorerNode( raw, contexts );
				_nodes.push_back(sn);
				
#line 514 "QueryParser.cpp"
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
					unqualifiedTerm();
					match(DOT);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched112 = false;
				}
				rewind(_m112);
				inputState->guessing--;
			}
			if ( synPredMatched112 ) {
				raw=unqualifiedTerm();
				match(DOT);
				contexts=context_list();
				if ( inputState->guessing==0 ) {
#line 226 "indrilang.g"
					
					sn = new indri::lang::RawScorerNode( raw, contexts );
					_nodes.push_back(sn);
					
#line 545 "QueryParser.cpp"
				}
			}
			else if ((_tokenSet_1.member(LA(1)))) {
				raw=unqualifiedTerm();
				if ( inputState->guessing==0 ) {
#line 231 "indrilang.g"
					
					sn = new indri::lang::RawScorerNode( raw, contexts );
					_nodes.push_back(sn);
					
#line 556 "QueryParser.cpp"
				}
			}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}}
	return sn ;
}

 RawExtentNode*  QueryParser::qualifiedTerm() {
#line 475 "indrilang.g"
	 RawExtentNode* t ;
#line 569 "QueryParser.cpp"
#line 475 "indrilang.g"
	
	RawExtentNode* synonyms = 0;
	RawExtentNode* fields = 0;
	t = 0;
	
#line 576 "QueryParser.cpp"
	
	synonyms=unqualifiedTerm();
	match(DOT);
	fields=field_list();
	if ( inputState->guessing==0 ) {
#line 482 "indrilang.g"
		
		if( fields ) {
		t = new indri::lang::ExtentInside( synonyms, fields );
		_nodes.push_back(t);
		} else {
		t = synonyms;
		}
		
#line 591 "QueryParser.cpp"
	}
	return t ;
}

 ExtentOr*  QueryParser::context_list() {
#line 584 "indrilang.g"
	 ExtentOr* contexts ;
#line 599 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 584 "indrilang.g"
	
	contexts = new ExtentOr;
	_nodes.push_back( contexts );
	
#line 607 "QueryParser.cpp"
	
	match(O_PAREN);
	first = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 590 "indrilang.g"
		
		Field* firstField = new indri::lang::Field( first->getText() );
		_nodes.push_back( firstField );
		contexts->addChild( firstField );
		
#line 619 "QueryParser.cpp"
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
				
#line 634 "QueryParser.cpp"
			}
		}
		else {
			goto _loop215;
		}
		
	}
	_loop215:;
	} // ( ... )*
	match(C_PAREN);
	return contexts ;
}

 indri::lang::RawExtentNode*  QueryParser::unqualifiedTerm() {
#line 491 "indrilang.g"
	 indri::lang::RawExtentNode* re ;
#line 651 "QueryParser.cpp"
	
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
#line 247 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 753 "QueryParser.cpp"
#line 247 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 760 "QueryParser.cpp"
	
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
	int _cnt116=0;
	for (;;) {
		if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
			w=floating();
			n=scoredExtentNode();
			if ( inputState->guessing==0 ) {
#line 258 "indrilang.g"
				wn->addChild( w, n );
#line 789 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt116>=1 ) { goto _loop116; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt116++;
	}
	_loop116:;
	}  // ( ... )+
	match(C_PAREN);
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::extentRestriction(
	 indri::lang::ScoredExtentNode* sn 
) {
#line 509 "indrilang.g"
	 indri::lang::ScoredExtentNode* er ;
#line 809 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  passageWindowSize = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  inc = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 509 "indrilang.g"
	
	indri::lang::Field* f = 0;
	er = 0;
	
#line 818 "QueryParser.cpp"
	
	bool synPredMatched200 = false;
	if (((LA(1) == O_SQUARE))) {
		int _m200 = mark();
		synPredMatched200 = true;
		inputState->guessing++;
		try {
			{
			match(O_SQUARE);
			match(TERM);
			match(COLON);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched200 = false;
		}
		rewind(_m200);
		inputState->guessing--;
	}
	if ( synPredMatched200 ) {
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
			
#line 861 "QueryParser.cpp"
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
			
#line 877 "QueryParser.cpp"
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return er ;
}

 double  QueryParser::floating() {
#line 734 "indrilang.g"
	 double d ;
#line 890 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 734 "indrilang.g"
	
	d = 0;
	
#line 897 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 737 "indrilang.g"
			
			d = atof(f->getText().c_str());
			
#line 909 "QueryParser.cpp"
		}
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 740 "indrilang.g"
			
			d = atof(n->getText().c_str());
			
#line 922 "QueryParser.cpp"
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
#line 263 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 939 "QueryParser.cpp"
#line 263 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 946 "QueryParser.cpp"
	
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
	int _cnt120=0;
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			n=scoredExtentNode();
			if ( inputState->guessing==0 ) {
#line 271 "indrilang.g"
				wn->addChild( 1.0, n );
#line 974 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt120>=1 ) { goto _loop120; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt120++;
	}
	_loop120:;
	}  // ( ... )+
	match(C_PAREN);
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::unweightedList(
	 indri::lang::UnweightedCombinationNode* cn 
) {
#line 275 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 994 "QueryParser.cpp"
#line 275 "indrilang.g"
	
	ScoredExtentNode* n = 0;
	sr = cn;
	
#line 1000 "QueryParser.cpp"
	
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
	int _cnt124=0;
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			n=scoredExtentNode();
			if ( inputState->guessing==0 ) {
#line 282 "indrilang.g"
				cn->addChild( n );
#line 1028 "QueryParser.cpp"
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

 indri::lang::ScoredExtentNode*  QueryParser::sumNode() {
#line 300 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 1046 "QueryParser.cpp"
#line 300 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 1052 "QueryParser.cpp"
	
	match(SUM);
	r=sumList(wn);
	return r ;
}

 indri::lang::WeightedExtentOr*  QueryParser::wsynNode() {
#line 375 "indrilang.g"
	 indri::lang::WeightedExtentOr* ws ;
#line 1062 "QueryParser.cpp"
#line 375 "indrilang.g"
	
	ws = new indri::lang::WeightedExtentOr;
	_nodes.push_back(ws);
	
	double w = 0;
	RawExtentNode* n = 0;
	
#line 1071 "QueryParser.cpp"
	
	match(WSYN);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt137=0;
	for (;;) {
		if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
			w=floating();
			n=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 384 "indrilang.g"
				ws->addChild( w, n );
#line 1084 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt137>=1 ) { goto _loop137; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt137++;
	}
	_loop137:;
	}  // ( ... )+
	match(C_PAREN);
	return ws ;
}

 RawExtentNode*  QueryParser::unscoredTerm() {
#line 468 "indrilang.g"
	 RawExtentNode* t ;
#line 1102 "QueryParser.cpp"
#line 468 "indrilang.g"
	
	t = 0;
	
#line 1107 "QueryParser.cpp"
	
	bool synPredMatched165 = false;
	if (((_tokenSet_1.member(LA(1))))) {
		int _m165 = mark();
		synPredMatched165 = true;
		inputState->guessing++;
		try {
			{
			qualifiedTerm();
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched165 = false;
		}
		rewind(_m165);
		inputState->guessing--;
	}
	if ( synPredMatched165 ) {
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
#line 388 "indrilang.g"
	 indri::lang::ODNode* od ;
#line 1141 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 388 "indrilang.g"
	
	RawExtentNode* rn = 0;
	od = new indri::lang::ODNode;
	_nodes.push_back(od);
	
#line 1150 "QueryParser.cpp"
	
	{
	bool synPredMatched141 = false;
	if (((LA(1) == OD))) {
		int _m141 = mark();
		synPredMatched141 = true;
		inputState->guessing++;
		try {
			{
			match(OD);
			match(NUMBER);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched141 = false;
		}
		rewind(_m141);
		inputState->guessing--;
	}
	if ( synPredMatched141 ) {
		{
		match(OD);
		n1 = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 397 "indrilang.g"
			od->setWindowSize( n1->getText() );
#line 1178 "QueryParser.cpp"
		}
		}
	}
	else {
		bool synPredMatched144 = false;
		if (((LA(1) == OD))) {
			int _m144 = mark();
			synPredMatched144 = true;
			inputState->guessing++;
			try {
				{
				match(OD);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched144 = false;
			}
			rewind(_m144);
			inputState->guessing--;
		}
		if ( synPredMatched144 ) {
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
#line 401 "indrilang.g"
				od->setWindowSize( n2->getText() );
#line 1212 "QueryParser.cpp"
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
	int _cnt148=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 405 "indrilang.g"
				od->addChild( rn );
#line 1230 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt148>=1 ) { goto _loop148; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt148++;
	}
	_loop148:;
	}  // ( ... )+
	match(C_PAREN);
	return od ;
}

 indri::lang::UWNode*  QueryParser::uwNode() {
#line 409 "indrilang.g"
	 indri::lang::UWNode* uw ;
#line 1248 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 409 "indrilang.g"
	
	uw = new indri::lang::UWNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(uw);
	
#line 1256 "QueryParser.cpp"
	
	{
	bool synPredMatched152 = false;
	if (((LA(1) == UW))) {
		int _m152 = mark();
		synPredMatched152 = true;
		inputState->guessing++;
		try {
			{
			match(UW);
			match(NUMBER);
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
		match(UW);
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 417 "indrilang.g"
			uw->setWindowSize( n->getText() );
#line 1284 "QueryParser.cpp"
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
	int _cnt156=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 423 "indrilang.g"
				uw->addChild( rn );
#line 1307 "QueryParser.cpp"
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
	return uw ;
}

 indri::lang::BAndNode*  QueryParser::bandNode() {
#line 426 "indrilang.g"
	 indri::lang::BAndNode* b ;
#line 1325 "QueryParser.cpp"
#line 426 "indrilang.g"
	
	b = new indri::lang::BAndNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(b);
	
#line 1332 "QueryParser.cpp"
	
	match(BAND);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt159=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 434 "indrilang.g"
				b->addChild( rn );
#line 1344 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt159>=1 ) { goto _loop159; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt159++;
	}
	_loop159:;
	}  // ( ... )+
	match(C_PAREN);
	return b ;
}

 indri::lang::Field*  QueryParser::anyField() {
#line 459 "indrilang.g"
	 indri::lang::Field* f ;
#line 1362 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 459 "indrilang.g"
	
	f = 0;
	
#line 1368 "QueryParser.cpp"
	
	match(ANY);
	match(COLON);
	t = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 463 "indrilang.g"
		
		f = new Field(t->getText());
		_nodes.push_back(f);
		
#line 1380 "QueryParser.cpp"
	}
	return f ;
}

 indri::lang::ExtentAnd*  QueryParser::field_list() {
#line 564 "indrilang.g"
	 indri::lang::ExtentAnd* fields ;
#line 1388 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 564 "indrilang.g"
	
	fields = new ExtentAnd;
	_nodes.push_back( fields );
	
#line 1396 "QueryParser.cpp"
	
	first = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 570 "indrilang.g"
		
		Field* firstField = new indri::lang::Field( first->getText() );
		_nodes.push_back( firstField );
		fields->addChild( firstField );
		
#line 1407 "QueryParser.cpp"
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
				
#line 1422 "QueryParser.cpp"
			}
		}
		else {
			goto _loop212;
		}
		
	}
	_loop212:;
	} // ( ... )*
	return fields ;
}

 indri::lang::FieldLessNode*  QueryParser::dateBefore() {
#line 613 "indrilang.g"
	 indri::lang::FieldLessNode* extent ;
#line 1438 "QueryParser.cpp"
#line 613 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1445 "QueryParser.cpp"
	
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
		
#line 1459 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldGreaterNode*  QueryParser::dateAfter() {
#line 625 "indrilang.g"
	 indri::lang::FieldGreaterNode* extent ;
#line 1467 "QueryParser.cpp"
#line 625 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1474 "QueryParser.cpp"
	
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
		
#line 1488 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldBetweenNode*  QueryParser::dateBetween() {
#line 637 "indrilang.g"
	 indri::lang::FieldBetweenNode* extent ;
#line 1496 "QueryParser.cpp"
#line 637 "indrilang.g"
	
	UINT64 low = 0;
	UINT64 high = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1504 "QueryParser.cpp"
	
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
		
#line 1520 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list() {
#line 535 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1528 "QueryParser.cpp"
#line 535 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1535 "QueryParser.cpp"
	
	match(O_ANGLE);
	{ // ( ... )+
	int _cnt203=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 541 "indrilang.g"
				s->addChild(term);
#line 1546 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt203>=1 ) { goto _loop203; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt203++;
	}
	_loop203:;
	}  // ( ... )+
	match(C_ANGLE);
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_brace() {
#line 544 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1564 "QueryParser.cpp"
#line 544 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1571 "QueryParser.cpp"
	
	match(O_BRACE);
	{ // ( ... )+
	int _cnt206=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 550 "indrilang.g"
				s->addChild(term);
#line 1582 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt206>=1 ) { goto _loop206; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt206++;
	}
	_loop206:;
	}  // ( ... )+
	match(C_BRACE);
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_alt() {
#line 553 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1600 "QueryParser.cpp"
#line 553 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	// semantics of this node will change
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1608 "QueryParser.cpp"
	
	match(SYN);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt209=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 561 "indrilang.g"
				s->addChild(term);
#line 1620 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt209>=1 ) { goto _loop209; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt209++;
	}
	_loop209:;
	}  // ( ... )+
	match(C_PAREN);
	return s ;
}

 indri::lang::FieldLessNode*  QueryParser::lessNode() {
#line 766 "indrilang.g"
	 indri::lang::FieldLessNode* ln ;
#line 1638 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 766 "indrilang.g"
	
	ln = 0;
	Field* compareField = 0;
	INT64 high = 0;
	
#line 1646 "QueryParser.cpp"
	
	match(LESS);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	high=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 771 "indrilang.g"
		
		compareField = new Field(field->getText());
		ln = new FieldLessNode( compareField, high );
		_nodes.push_back( compareField );
		_nodes.push_back( ln );
		
#line 1662 "QueryParser.cpp"
	}
	return ln ;
}

 indri::lang::FieldGreaterNode*  QueryParser::greaterNode() {
#line 754 "indrilang.g"
	 indri::lang::FieldGreaterNode* gn ;
#line 1670 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 754 "indrilang.g"
	
	gn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	
#line 1678 "QueryParser.cpp"
	
	match(GREATER);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	low=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 759 "indrilang.g"
		
		compareField = new Field(field->getText());
		gn = new FieldGreaterNode( compareField, low );
		_nodes.push_back( compareField );
		_nodes.push_back( gn );
		
#line 1694 "QueryParser.cpp"
	}
	return gn ;
}

 indri::lang::FieldBetweenNode*  QueryParser::betweenNode() {
#line 778 "indrilang.g"
	 indri::lang::FieldBetweenNode* bn ;
#line 1702 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 778 "indrilang.g"
	
	bn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	INT64 high = 0;
	
#line 1711 "QueryParser.cpp"
	
	match(BETWEEN);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	low=number();
	high=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 784 "indrilang.g"
		
		compareField = new Field(field->getText());
		bn = new FieldBetweenNode( compareField, low, high );
		_nodes.push_back( compareField );
		_nodes.push_back( bn );
		
#line 1728 "QueryParser.cpp"
	}
	return bn ;
}

 indri::lang::FieldEqualsNode*  QueryParser::equalsNode() {
#line 791 "indrilang.g"
	 indri::lang::FieldEqualsNode* en ;
#line 1736 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 791 "indrilang.g"
	
	en = 0;
	Field* compareField = 0;
	INT64 eq = 0;
	
#line 1744 "QueryParser.cpp"
	
	match(EQUALS);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	eq=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 796 "indrilang.g"
		
		compareField = new Field(field->getText());
		en = new FieldEqualsNode( compareField, eq );
		_nodes.push_back( compareField );
		_nodes.push_back( en );
		
#line 1760 "QueryParser.cpp"
	}
	return en ;
}

 indri::lang::IndexTerm*  QueryParser::rawText() {
#line 699 "indrilang.g"
	 indri::lang::IndexTerm* t ;
#line 1768 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  et = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  qet = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 699 "indrilang.g"
	
	t = 0;
	
#line 1778 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case TERM:
	{
		id = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 702 "indrilang.g"
			
			t = new indri::lang::IndexTerm(id->getText());
			_nodes.push_back(t);
			
#line 1791 "QueryParser.cpp"
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
			
#line 1805 "QueryParser.cpp"
		}
		break;
	}
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 710 "indrilang.g"
			
			t = new indri::lang::IndexTerm(f->getText());
			_nodes.push_back(t);
			
#line 1819 "QueryParser.cpp"
		}
		break;
	}
	case DBL_QUOTE:
	{
		match(DBL_QUOTE);
		t=rawText();
		match(DBL_QUOTE);
		if ( inputState->guessing==0 ) {
#line 714 "indrilang.g"
			
			// if a text term appears in quotes, consider it stemmed
			t->setStemmed(true);
			
#line 1834 "QueryParser.cpp"
		}
		break;
	}
	case ENCODED_TERM:
	{
		et = LT(1);
		match(ENCODED_TERM);
		if ( inputState->guessing==0 ) {
#line 718 "indrilang.g"
			
			std::string decodedString; 
			base64_decode_string(decodedString, et->getText());
			t = new indri::lang::IndexTerm( decodedString );
			_nodes.push_back(t);
			
#line 1850 "QueryParser.cpp"
		}
		break;
	}
	case ENCODED_QUOTED_TERM:
	{
		qet = LT(1);
		match(ENCODED_QUOTED_TERM);
		if ( inputState->guessing==0 ) {
#line 724 "indrilang.g"
			
			std::string decodedString; 
			base64_decode_string(decodedString, et->getText());
			t = new indri::lang::IndexTerm( decodedString );
			t->setStemmed(true);
			_nodes.push_back(t);
			
#line 1867 "QueryParser.cpp"
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
#line 1882 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	match(O_SQUARE);
	fieldName = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 607 "indrilang.g"
		
		extent = new Field( fieldName->getText() );
		_nodes.push_back( extent );
		
#line 1894 "QueryParser.cpp"
	}
	match(C_SQUARE);
	return extent ;
}

 UINT64  QueryParser::date() {
#line 665 "indrilang.g"
	 UINT64 d ;
#line 1903 "QueryParser.cpp"
	
	bool synPredMatched222 = false;
	if (((LA(1) == NUMBER))) {
		int _m222 = mark();
		synPredMatched222 = true;
		inputState->guessing++;
		try {
			{
			match(NUMBER);
			match(SLASH);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched222 = false;
		}
		rewind(_m222);
		inputState->guessing--;
	}
	if ( synPredMatched222 ) {
		d=slashDate();
	}
	else {
		bool synPredMatched224 = false;
		if (((LA(1) == NUMBER))) {
			int _m224 = mark();
			synPredMatched224 = true;
			inputState->guessing++;
			try {
				{
				match(NUMBER);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched224 = false;
			}
			rewind(_m224);
			inputState->guessing--;
		}
		if ( synPredMatched224 ) {
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
#line 1958 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 684 "indrilang.g"
	
	d = 0;
	
#line 1966 "QueryParser.cpp"
	
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
		
#line 1981 "QueryParser.cpp"
	}
	return d ;
}

 UINT64  QueryParser::spaceDate() {
#line 691 "indrilang.g"
	 UINT64 d ;
#line 1989 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 691 "indrilang.g"
	
	d = 0;
	
#line 1997 "QueryParser.cpp"
	
	day = LT(1);
	match(NUMBER);
	month = LT(1);
	match(TERM);
	year = LT(1);
	match(NUMBER);
	if ( inputState->guessing==0 ) {
#line 694 "indrilang.g"
		
		d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() );
		
#line 2010 "QueryParser.cpp"
	}
	return d ;
}

 UINT64  QueryParser::dashDate() {
#line 670 "indrilang.g"
	 UINT64 d ;
#line 2018 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  dmy = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 670 "indrilang.g"
	
	d = 0;
	
#line 2024 "QueryParser.cpp"
	
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
		
#line 2040 "QueryParser.cpp"
	}
	return d ;
}

 INT64  QueryParser::number() {
#line 744 "indrilang.g"
	 INT64 v ;
#line 2048 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nn = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 744 "indrilang.g"
	
	v = 0;
	
#line 2055 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 747 "indrilang.g"
			
			v = string_to_i64(n->getText());
			
#line 2067 "QueryParser.cpp"
		}
		break;
	}
	case NEGATIVE_NUMBER:
	{
		nn = LT(1);
		match(NEGATIVE_NUMBER);
		if ( inputState->guessing==0 ) {
#line 750 "indrilang.g"
			
			v = string_to_i64(nn->getText());
			
#line 2080 "QueryParser.cpp"
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

const unsigned long QueryParser::_tokenSet_0_data_[] = { 1610612704UL, 503316804UL, 0UL, 0UL };
// "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#band" "#wsyn" "#syn" "#prior" "#date:after" 
// "#date:before" "#date:between" "#less" "#greater" "#between" "#equals" 
// NUMBER FLOAT O_ANGLE O_BRACE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM 
// OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long QueryParser::_tokenSet_1_data_[] = { 1609499776UL, 503316804UL, 0UL, 0UL };
// "#od" "#uw" "#any" "#band" "#wsyn" "#syn" "#date:after" "#date:before" 
// "#date:between" "#less" "#greater" "#between" "#equals" NUMBER FLOAT 
// O_ANGLE O_BRACE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_1(_tokenSet_1_data_,4);


ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
