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
#line 186 "indrilang.g"
	 indri::lang::ScoredExtentNode* q ;
#line 37 "QueryParser.cpp"
#line 186 "indrilang.g"
	
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
	case TAND:
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
#line 193 "indrilang.g"
			
			c = new CombineNode;
			c->addChild(q);
			c->addChild(s);
			_nodes.push_back(c);
			q = c;
			
#line 94 "QueryParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				s=scoredExtentNode(0);
				if ( inputState->guessing==0 ) {
#line 201 "indrilang.g"
					
					c->addChild(s);
					
#line 105 "QueryParser.cpp"
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
#line 207 "indrilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 136 "QueryParser.cpp"
	
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
	case TAND:
	{
		s=tandNode(ou);
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
#line 305 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 234 "QueryParser.cpp"
#line 305 "indrilang.g"
	
	indri::lang::WeightNode* wn = new indri::lang::WeightNode;
	_nodes.push_back(wn);
	
#line 240 "QueryParser.cpp"
	
	match(WEIGHT);
	r=weightedList(wn, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::combineNode(
	indri::lang::RawExtentNode * ou 
) {
#line 312 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 252 "QueryParser.cpp"
#line 312 "indrilang.g"
	
	indri::lang::CombineNode* cn = new indri::lang::CombineNode;
	_nodes.push_back(cn);
	
#line 258 "QueryParser.cpp"
	
	match(COMBINE);
	r=unweightedList(cn, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::orNode(
	indri::lang::RawExtentNode * ou 
) {
#line 340 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 270 "QueryParser.cpp"
#line 340 "indrilang.g"
	
	indri::lang::OrNode* on = new indri::lang::OrNode;
	_nodes.push_back(on);
	
#line 276 "QueryParser.cpp"
	
	match(OR);
	r=unweightedList(on, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::notNode(
	indri::lang::RawExtentNode * ou 
) {
#line 354 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 288 "QueryParser.cpp"
#line 354 "indrilang.g"
	
	indri::lang::NotNode* n = new indri::lang::NotNode;
	indri::lang::ScoredExtentNode* c = 0;
	_nodes.push_back(n);
	r = n;
	
#line 296 "QueryParser.cpp"
	
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
#line 362 "indrilang.g"
		
		n->setChild(c);
		
#line 324 "QueryParser.cpp"
	}
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::wandNode(
	indri::lang::RawExtentNode * ou 
) {
#line 333 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 334 "QueryParser.cpp"
#line 333 "indrilang.g"
	
	indri::lang::WAndNode* wn = new indri::lang::WAndNode;
	_nodes.push_back(wn);
	
#line 340 "QueryParser.cpp"
	
	match(WAND);
	r=weightedList(wn, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::wsumNode(
	indri::lang::RawExtentNode * ou 
) {
#line 326 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 352 "QueryParser.cpp"
#line 326 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 358 "QueryParser.cpp"
	
	match(WSUM);
	r=weightedList(wn, ou);
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::maxNode(
	indri::lang::RawExtentNode * ou 
) {
#line 347 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 370 "QueryParser.cpp"
#line 347 "indrilang.g"
	
	indri::lang::MaxNode* mn = new indri::lang::MaxNode;
	_nodes.push_back(mn);
	
#line 376 "QueryParser.cpp"
	
	match(MAX);
	r=unweightedList(mn, ou);
	return r ;
}

 indri::lang::PriorNode*  QueryParser::priorNode() {
#line 366 "indrilang.g"
	 indri::lang::PriorNode* p ;
#line 386 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 366 "indrilang.g"
	
	indri::lang::Field* field = 0;
	p = 0;
	
#line 393 "QueryParser.cpp"
	
	match(PRIOR);
	match(O_PAREN);
	name = LT(1);
	match(TERM);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 371 "indrilang.g"
		
		p = new indri::lang::PriorNode( name->getText() );
		_nodes.push_back(p);
		
#line 406 "QueryParser.cpp"
	}
	return p ;
}

 indri::lang::FilRejNode*  QueryParser::filrejNode(
	 indri::lang::RawExtentNode * ou 
) {
#line 449 "indrilang.g"
	 indri::lang::FilRejNode* fj ;
#line 416 "QueryParser.cpp"
#line 449 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* disallowed = 0;
	
#line 422 "QueryParser.cpp"
	
	match(FILREJ);
	match(O_PAREN);
	filter=unscoredTerm();
	disallowed=scoredExtentNode(ou);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 455 "indrilang.g"
		
		fj = new FilRejNode( filter, disallowed );
		_nodes.push_back(fj);
		
#line 435 "QueryParser.cpp"
	}
	return fj ;
}

 indri::lang::FilReqNode*  QueryParser::filreqNode(
	 indri::lang::RawExtentNode * ou 
) {
#line 460 "indrilang.g"
	 indri::lang::FilReqNode* fq ;
#line 445 "QueryParser.cpp"
#line 460 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* required = 0;
	
#line 451 "QueryParser.cpp"
	
	match(FILREQ);
	match(O_PAREN);
	filter=unscoredTerm();
	required=scoredExtentNode(ou);
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 466 "indrilang.g"
		
		fq = new FilReqNode( filter, required );
		_nodes.push_back(fq);
		
#line 464 "QueryParser.cpp"
	}
	return fq ;
}

 indri::lang::ScoredExtentNode*  QueryParser::scoredRaw() {
#line 228 "indrilang.g"
	 indri::lang::ScoredExtentNode* sn ;
#line 472 "QueryParser.cpp"
#line 228 "indrilang.g"
	
	RawExtentNode* raw = 0;
	RawExtentNode* contexts = 0;
	sn = 0;
	
#line 479 "QueryParser.cpp"
	
	bool synPredMatched112 = false;
	if (((_tokenSet_1.member(LA(1))))) {
		int _m112 = mark();
		synPredMatched112 = true;
		inputState->guessing++;
		try {
			{
			qualifiedTerm();
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
		raw=qualifiedTerm();
		match(DOT);
		contexts=context_list();
		if ( inputState->guessing==0 ) {
#line 235 "indrilang.g"
			
			sn = new indri::lang::RawScorerNode( raw, contexts );
			_nodes.push_back(sn);
			
#line 508 "QueryParser.cpp"
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
				qualifiedTerm();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched114 = false;
			}
			rewind(_m114);
			inputState->guessing--;
		}
		if ( synPredMatched114 ) {
			raw=qualifiedTerm();
			if ( inputState->guessing==0 ) {
#line 240 "indrilang.g"
				
				sn = new indri::lang::RawScorerNode( raw, contexts );
				_nodes.push_back(sn);
				
#line 536 "QueryParser.cpp"
			}
		}
		else {
			bool synPredMatched116 = false;
			if (((_tokenSet_1.member(LA(1))))) {
				int _m116 = mark();
				synPredMatched116 = true;
				inputState->guessing++;
				try {
					{
					unqualifiedTerm();
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
				raw=unqualifiedTerm();
				match(DOT);
				contexts=context_list();
				if ( inputState->guessing==0 ) {
#line 245 "indrilang.g"
					
					sn = new indri::lang::RawScorerNode( raw, contexts );
					_nodes.push_back(sn);
					
#line 567 "QueryParser.cpp"
				}
			}
			else if ((_tokenSet_1.member(LA(1)))) {
				raw=unqualifiedTerm();
				if ( inputState->guessing==0 ) {
#line 250 "indrilang.g"
					
					sn = new indri::lang::RawScorerNode( raw, contexts );
					_nodes.push_back(sn);
					
#line 578 "QueryParser.cpp"
				}
			}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}}
	return sn ;
}

 indri::lang::ScoredExtentNode*  QueryParser::tandNode(
	indri::lang::RawExtentNode * ou 
) {
#line 893 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 593 "QueryParser.cpp"
#line 893 "indrilang.g"
	
	indri::lang::TAndNode* tn = new indri::lang::TAndNode;
	_nodes.push_back(tn);
	
#line 599 "QueryParser.cpp"
	
	match(TAND);
	r=tieredList(tn, ou);
	return r ;
}

 RawExtentNode*  QueryParser::qualifiedTerm() {
#line 487 "indrilang.g"
	 RawExtentNode* t ;
#line 609 "QueryParser.cpp"
#line 487 "indrilang.g"
	
	RawExtentNode* synonyms = 0;
	RawExtentNode* fields = 0;
	t = 0;
	
#line 616 "QueryParser.cpp"
	
	synonyms=unqualifiedTerm();
	match(DOT);
	fields=field_list();
	if ( inputState->guessing==0 ) {
#line 494 "indrilang.g"
		
		if( fields ) {
		t = new indri::lang::ExtentInside( synonyms, fields );
		_nodes.push_back(t);
		synonyms = t;
		} else {
		t = synonyms;
		}
		
#line 632 "QueryParser.cpp"
	}
	return t ;
}

 ExtentOr*  QueryParser::context_list() {
#line 643 "indrilang.g"
	 ExtentOr* contexts ;
#line 640 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 643 "indrilang.g"
	
	contexts = new ExtentOr;
	_nodes.push_back( contexts );
	
#line 648 "QueryParser.cpp"
	
	match(O_PAREN);
	first = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 649 "indrilang.g"
		
		Field* firstField = new indri::lang::Field( first->getText() );
		_nodes.push_back( firstField );
		contexts->addChild( firstField );
		
#line 660 "QueryParser.cpp"
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			additional = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 656 "indrilang.g"
				
				Field* additionalField = new Field(additional->getText());
				_nodes.push_back( additionalField );
				contexts->addChild( additionalField );
				
#line 675 "QueryParser.cpp"
			}
		}
		else {
			goto _loop223;
		}
		
	}
	_loop223:;
	} // ( ... )*
	match(C_PAREN);
	return contexts ;
}

 indri::lang::RawExtentNode*  QueryParser::unqualifiedTerm() {
#line 504 "indrilang.g"
	 indri::lang::RawExtentNode* re ;
#line 692 "QueryParser.cpp"
	
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
#line 266 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 795 "QueryParser.cpp"
#line 266 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 802 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(wn, ou);
		if ( inputState->guessing==0 ) {
#line 272 "indrilang.g"
			ou = innerMost(sr);
#line 812 "QueryParser.cpp"
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
	int _cnt120=0;
	for (;;) {
		if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
			w=floating();
			n=scoredExtentNode(ou);
			if ( inputState->guessing==0 ) {
#line 277 "indrilang.g"
				wn->addChild( w, n );
#line 836 "QueryParser.cpp"
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

 indri::lang::ScoredExtentNode*  QueryParser::extentRestriction(
	 indri::lang::ScoredExtentNode* sn, indri::lang::RawExtentNode * ou 
) {
#line 522 "indrilang.g"
	 indri::lang::ScoredExtentNode* er ;
#line 856 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  passageWindowSize = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  inc = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldRestricted = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 522 "indrilang.g"
	
	indri::lang::Field* f = 0;
	er = 0;
	indri::lang::ExtentInside * po = 0;
	
#line 867 "QueryParser.cpp"
	
	bool synPredMatched204 = false;
	if (((LA(1) == O_SQUARE))) {
		int _m204 = mark();
		synPredMatched204 = true;
		inputState->guessing++;
		try {
			{
			match(O_SQUARE);
			match(TERM);
			match(COLON);
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
		passageWindowSize = LT(1);
		match(TERM);
		match(COLON);
		inc = LT(1);
		match(NUMBER);
		match(C_SQUARE);
		if ( inputState->guessing==0 ) {
#line 528 "indrilang.g"
			
			int startWindow;
			
			for( startWindow = 0; startWindow < passageWindowSize->getText().size(); startWindow++ ) {
			if( isdigit( passageWindowSize->getText()[startWindow] ) )
			break;
			}
			
			int increment = atoi(inc->getText().c_str());
			int windowSize = atoi(passageWindowSize->getText().c_str() + startWindow );
			
			er = new indri::lang::FixedPassage(sn, windowSize, increment);
			
#line 910 "QueryParser.cpp"
		}
	}
	else {
		bool synPredMatched206 = false;
		if (((LA(1) == O_SQUARE))) {
			int _m206 = mark();
			synPredMatched206 = true;
			inputState->guessing++;
			try {
				{
				match(O_SQUARE);
				match(TERM);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched206 = false;
			}
			rewind(_m206);
			inputState->guessing--;
		}
		if ( synPredMatched206 ) {
			match(O_SQUARE);
			field = LT(1);
			match(TERM);
			match(C_SQUARE);
			if ( inputState->guessing==0 ) {
#line 542 "indrilang.g"
				
				f = new indri::lang::Field(field->getText());
				_nodes.push_back(f);
				er = new indri::lang::ExtentRestriction(sn, f);
				_nodes.push_back(er);
				
#line 944 "QueryParser.cpp"
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
#line 549 "indrilang.g"
				
				
				if ( ou == 0 ) {
				throw new antlr::SemanticException("Use of a . in a extent restriction without a valid outer context.");
				}
				f = new indri::lang::Field(fieldRestricted->getText());
				_nodes.push_back(f);
				po->setInner(f);
				po->setOuter(ou);
				er = new indri::lang::ExtentRestriction(sn, po);
				_nodes.push_back(er);
				
#line 968 "QueryParser.cpp"
			}
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return er ;
}

 double  QueryParser::floating() {
#line 797 "indrilang.g"
	 double d ;
#line 981 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 797 "indrilang.g"
	
	d = 0;
	
#line 988 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 800 "indrilang.g"
			
			d = atof(f->getText().c_str());
			
#line 1000 "QueryParser.cpp"
		}
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 803 "indrilang.g"
			
			d = atof(n->getText().c_str());
			
#line 1013 "QueryParser.cpp"
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
#line 282 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 1030 "QueryParser.cpp"
#line 282 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 1037 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(wn, ou);
		if ( inputState->guessing==0 ) {
#line 288 "indrilang.g"
			ou = innerMost(sr);
#line 1047 "QueryParser.cpp"
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
		if ((_tokenSet_0.member(LA(1)))) {
			n=scoredExtentNode(ou);
			if ( inputState->guessing==0 ) {
#line 290 "indrilang.g"
				wn->addChild( 1.0, n );
#line 1070 "QueryParser.cpp"
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

 indri::lang::ScoredExtentNode*  QueryParser::unweightedList(
	 indri::lang::UnweightedCombinationNode* cn, indri::lang::RawExtentNode * ou 
) {
#line 294 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 1090 "QueryParser.cpp"
#line 294 "indrilang.g"
	
	ScoredExtentNode* n = 0;
	sr = cn;
	
#line 1096 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(cn, ou);
		if ( inputState->guessing==0 ) {
#line 299 "indrilang.g"
			ou = innerMost(sr);
#line 1106 "QueryParser.cpp"
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
#line 301 "indrilang.g"
				cn->addChild( n );
#line 1129 "QueryParser.cpp"
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

 indri::lang::ScoredExtentNode*  QueryParser::sumNode(
	indri::lang::RawExtentNode * ou 
) {
#line 319 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 1149 "QueryParser.cpp"
#line 319 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 1155 "QueryParser.cpp"
	
	match(SUM);
	r=sumList(wn, ou);
	return r ;
}

 indri::lang::WeightedExtentOr*  QueryParser::wsynNode() {
#line 387 "indrilang.g"
	 indri::lang::WeightedExtentOr* ws ;
#line 1165 "QueryParser.cpp"
#line 387 "indrilang.g"
	
	ws = new indri::lang::WeightedExtentOr;
	_nodes.push_back(ws);
	
	double w = 0;
	RawExtentNode* n = 0;
	
#line 1174 "QueryParser.cpp"
	
	match(WSYN);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt141=0;
	for (;;) {
		if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
			w=floating();
			n=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 396 "indrilang.g"
				ws->addChild( w, n );
#line 1187 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt141>=1 ) { goto _loop141; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt141++;
	}
	_loop141:;
	}  // ( ... )+
	match(C_PAREN);
	return ws ;
}

 RawExtentNode*  QueryParser::unscoredTerm() {
#line 480 "indrilang.g"
	 RawExtentNode* t ;
#line 1205 "QueryParser.cpp"
#line 480 "indrilang.g"
	
	t = 0;
	
#line 1210 "QueryParser.cpp"
	
	bool synPredMatched169 = false;
	if (((_tokenSet_1.member(LA(1))))) {
		int _m169 = mark();
		synPredMatched169 = true;
		inputState->guessing++;
		try {
			{
			qualifiedTerm();
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched169 = false;
		}
		rewind(_m169);
		inputState->guessing--;
	}
	if ( synPredMatched169 ) {
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
#line 400 "indrilang.g"
	 indri::lang::ODNode* od ;
#line 1244 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 400 "indrilang.g"
	
	RawExtentNode* rn = 0;
	od = new indri::lang::ODNode;
	_nodes.push_back(od);
	
#line 1253 "QueryParser.cpp"
	
	{
	bool synPredMatched145 = false;
	if (((LA(1) == OD))) {
		int _m145 = mark();
		synPredMatched145 = true;
		inputState->guessing++;
		try {
			{
			match(OD);
			match(NUMBER);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched145 = false;
		}
		rewind(_m145);
		inputState->guessing--;
	}
	if ( synPredMatched145 ) {
		{
		match(OD);
		n1 = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 409 "indrilang.g"
			od->setWindowSize( n1->getText() );
#line 1281 "QueryParser.cpp"
		}
		}
	}
	else {
		bool synPredMatched148 = false;
		if (((LA(1) == OD))) {
			int _m148 = mark();
			synPredMatched148 = true;
			inputState->guessing++;
			try {
				{
				match(OD);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched148 = false;
			}
			rewind(_m148);
			inputState->guessing--;
		}
		if ( synPredMatched148 ) {
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
#line 413 "indrilang.g"
				od->setWindowSize( n2->getText() );
#line 1315 "QueryParser.cpp"
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
	int _cnt152=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 417 "indrilang.g"
				od->addChild( rn );
#line 1333 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt152>=1 ) { goto _loop152; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt152++;
	}
	_loop152:;
	}  // ( ... )+
	match(C_PAREN);
	return od ;
}

 indri::lang::UWNode*  QueryParser::uwNode() {
#line 421 "indrilang.g"
	 indri::lang::UWNode* uw ;
#line 1351 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 421 "indrilang.g"
	
	uw = new indri::lang::UWNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(uw);
	
#line 1359 "QueryParser.cpp"
	
	{
	bool synPredMatched156 = false;
	if (((LA(1) == UW))) {
		int _m156 = mark();
		synPredMatched156 = true;
		inputState->guessing++;
		try {
			{
			match(UW);
			match(NUMBER);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched156 = false;
		}
		rewind(_m156);
		inputState->guessing--;
	}
	if ( synPredMatched156 ) {
		{
		match(UW);
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 429 "indrilang.g"
			uw->setWindowSize( n->getText() );
#line 1387 "QueryParser.cpp"
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
	int _cnt160=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 435 "indrilang.g"
				uw->addChild( rn );
#line 1410 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt160>=1 ) { goto _loop160; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt160++;
	}
	_loop160:;
	}  // ( ... )+
	match(C_PAREN);
	return uw ;
}

 indri::lang::BAndNode*  QueryParser::bandNode() {
#line 438 "indrilang.g"
	 indri::lang::BAndNode* b ;
#line 1428 "QueryParser.cpp"
#line 438 "indrilang.g"
	
	b = new indri::lang::BAndNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(b);
	
#line 1435 "QueryParser.cpp"
	
	match(BAND);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt163=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			rn=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 446 "indrilang.g"
				b->addChild( rn );
#line 1447 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt163>=1 ) { goto _loop163; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt163++;
	}
	_loop163:;
	}  // ( ... )+
	match(C_PAREN);
	return b ;
}

 indri::lang::Field*  QueryParser::anyField() {
#line 471 "indrilang.g"
	 indri::lang::Field* f ;
#line 1465 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 471 "indrilang.g"
	
	f = 0;
	
#line 1471 "QueryParser.cpp"
	
	match(ANY);
	match(COLON);
	t = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 475 "indrilang.g"
		
		f = new Field(t->getText());
		_nodes.push_back(f);
		
#line 1483 "QueryParser.cpp"
	}
	return f ;
}

 indri::lang::ExtentAnd*  QueryParser::field_list() {
#line 623 "indrilang.g"
	 indri::lang::ExtentAnd* fields ;
#line 1491 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 623 "indrilang.g"
	
	fields = new ExtentAnd;
	_nodes.push_back( fields );
	
#line 1499 "QueryParser.cpp"
	
	first = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 629 "indrilang.g"
		
		Field* firstField = new indri::lang::Field( first->getText() );
		_nodes.push_back( firstField );
		fields->addChild( firstField );
		
#line 1510 "QueryParser.cpp"
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			additional = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 636 "indrilang.g"
				
				Field* additionalField = new Field(additional->getText());
				_nodes.push_back( additionalField );
				fields->addChild( additionalField );
				
#line 1525 "QueryParser.cpp"
			}
		}
		else {
			goto _loop220;
		}
		
	}
	_loop220:;
	} // ( ... )*
	return fields ;
}

 indri::lang::FieldLessNode*  QueryParser::dateBefore() {
#line 672 "indrilang.g"
	 indri::lang::FieldLessNode* extent ;
#line 1541 "QueryParser.cpp"
#line 672 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1548 "QueryParser.cpp"
	
	match(DATEBEFORE);
	match(O_PAREN);
	d=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 677 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldLessNode( dateField, d );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1562 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldGreaterNode*  QueryParser::dateAfter() {
#line 684 "indrilang.g"
	 indri::lang::FieldGreaterNode* extent ;
#line 1570 "QueryParser.cpp"
#line 684 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1577 "QueryParser.cpp"
	
	match(DATEAFTER);
	match(O_PAREN);
	d=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 689 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldGreaterNode( dateField, d );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1591 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::FieldBetweenNode*  QueryParser::dateBetween() {
#line 696 "indrilang.g"
	 indri::lang::FieldBetweenNode* extent ;
#line 1599 "QueryParser.cpp"
#line 696 "indrilang.g"
	
	UINT64 low = 0;
	UINT64 high = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1607 "QueryParser.cpp"
	
	match(DATEBETWEEN);
	match(O_PAREN);
	low=date();
	match(COMMA);
	high=date();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 702 "indrilang.g"
		
		dateField = new Field("date");
		extent = new FieldBetweenNode( dateField, low, high );
		_nodes.push_back( dateField );
		_nodes.push_back( extent );
		
#line 1623 "QueryParser.cpp"
	}
	return extent ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list() {
#line 594 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1631 "QueryParser.cpp"
#line 594 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1638 "QueryParser.cpp"
	
	match(O_ANGLE);
	{ // ( ... )+
	int _cnt211=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 600 "indrilang.g"
				s->addChild(term);
#line 1649 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt211>=1 ) { goto _loop211; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt211++;
	}
	_loop211:;
	}  // ( ... )+
	match(C_ANGLE);
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_brace() {
#line 603 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1667 "QueryParser.cpp"
#line 603 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1674 "QueryParser.cpp"
	
	match(O_BRACE);
	{ // ( ... )+
	int _cnt214=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 609 "indrilang.g"
				s->addChild(term);
#line 1685 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt214>=1 ) { goto _loop214; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt214++;
	}
	_loop214:;
	}  // ( ... )+
	match(C_BRACE);
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_alt() {
#line 612 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1703 "QueryParser.cpp"
#line 612 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	// semantics of this node will change
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1711 "QueryParser.cpp"
	
	match(SYN);
	match(O_PAREN);
	{ // ( ... )+
	int _cnt217=0;
	for (;;) {
		if ((_tokenSet_1.member(LA(1)))) {
			term=unscoredTerm();
			if ( inputState->guessing==0 ) {
#line 620 "indrilang.g"
				s->addChild(term);
#line 1723 "QueryParser.cpp"
			}
		}
		else {
			if ( _cnt217>=1 ) { goto _loop217; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt217++;
	}
	_loop217:;
	}  // ( ... )+
	match(C_PAREN);
	return s ;
}

 indri::lang::FieldLessNode*  QueryParser::lessNode() {
#line 829 "indrilang.g"
	 indri::lang::FieldLessNode* ln ;
#line 1741 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 829 "indrilang.g"
	
	ln = 0;
	Field* compareField = 0;
	INT64 high = 0;
	
#line 1749 "QueryParser.cpp"
	
	match(LESS);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	high=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 834 "indrilang.g"
		
		compareField = new Field(field->getText());
		ln = new FieldLessNode( compareField, high );
		_nodes.push_back( compareField );
		_nodes.push_back( ln );
		
#line 1765 "QueryParser.cpp"
	}
	return ln ;
}

 indri::lang::FieldGreaterNode*  QueryParser::greaterNode() {
#line 817 "indrilang.g"
	 indri::lang::FieldGreaterNode* gn ;
#line 1773 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 817 "indrilang.g"
	
	gn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	
#line 1781 "QueryParser.cpp"
	
	match(GREATER);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	low=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 822 "indrilang.g"
		
		compareField = new Field(field->getText());
		gn = new FieldGreaterNode( compareField, low );
		_nodes.push_back( compareField );
		_nodes.push_back( gn );
		
#line 1797 "QueryParser.cpp"
	}
	return gn ;
}

 indri::lang::FieldBetweenNode*  QueryParser::betweenNode() {
#line 841 "indrilang.g"
	 indri::lang::FieldBetweenNode* bn ;
#line 1805 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 841 "indrilang.g"
	
	bn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	INT64 high = 0;
	
#line 1814 "QueryParser.cpp"
	
	match(BETWEEN);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	low=number();
	high=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 847 "indrilang.g"
		
		compareField = new Field(field->getText());
		bn = new FieldBetweenNode( compareField, low, high );
		_nodes.push_back( compareField );
		_nodes.push_back( bn );
		
#line 1831 "QueryParser.cpp"
	}
	return bn ;
}

 indri::lang::FieldEqualsNode*  QueryParser::equalsNode() {
#line 854 "indrilang.g"
	 indri::lang::FieldEqualsNode* en ;
#line 1839 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 854 "indrilang.g"
	
	en = 0;
	Field* compareField = 0;
	INT64 eq = 0;
	
#line 1847 "QueryParser.cpp"
	
	match(EQUALS);
	match(O_PAREN);
	field = LT(1);
	match(TERM);
	eq=number();
	match(C_PAREN);
	if ( inputState->guessing==0 ) {
#line 859 "indrilang.g"
		
		compareField = new Field(field->getText());
		en = new FieldEqualsNode( compareField, eq );
		_nodes.push_back( compareField );
		_nodes.push_back( en );
		
#line 1863 "QueryParser.cpp"
	}
	return en ;
}

 indri::lang::IndexTerm*  QueryParser::rawText() {
#line 758 "indrilang.g"
	 indri::lang::IndexTerm* t ;
#line 1871 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nn = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  et = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  qet = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 758 "indrilang.g"
	
	t = 0;
	
#line 1882 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case TERM:
	{
		id = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 761 "indrilang.g"
			
			t = new indri::lang::IndexTerm(id->getText());
			_nodes.push_back(t);
			
#line 1895 "QueryParser.cpp"
		}
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 765 "indrilang.g"
			
			t = new indri::lang::IndexTerm(n->getText());
			_nodes.push_back(t);
			
#line 1909 "QueryParser.cpp"
		}
		break;
	}
	case NEGATIVE_NUMBER:
	{
		nn = LT(1);
		match(NEGATIVE_NUMBER);
		if ( inputState->guessing==0 ) {
#line 769 "indrilang.g"
			
			t = new indri::lang::IndexTerm(nn->getText());
			_nodes.push_back(t);
			
#line 1923 "QueryParser.cpp"
		}
		break;
	}
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 773 "indrilang.g"
			
			t = new indri::lang::IndexTerm(f->getText());
			_nodes.push_back(t);
			
#line 1937 "QueryParser.cpp"
		}
		break;
	}
	case DBL_QUOTE:
	{
		match(DBL_QUOTE);
		t=rawText();
		match(DBL_QUOTE);
		if ( inputState->guessing==0 ) {
#line 777 "indrilang.g"
			
			// if a text term appears in quotes, consider it stemmed
			t->setStemmed(true);
			
#line 1952 "QueryParser.cpp"
		}
		break;
	}
	case ENCODED_TERM:
	{
		et = LT(1);
		match(ENCODED_TERM);
		if ( inputState->guessing==0 ) {
#line 781 "indrilang.g"
			
			std::string decodedString; 
			base64_decode_string(decodedString, et->getText());
			t = new indri::lang::IndexTerm( decodedString );
			_nodes.push_back(t);
			
#line 1968 "QueryParser.cpp"
		}
		break;
	}
	case ENCODED_QUOTED_TERM:
	{
		qet = LT(1);
		match(ENCODED_QUOTED_TERM);
		if ( inputState->guessing==0 ) {
#line 787 "indrilang.g"
			
			std::string decodedString; 
			base64_decode_string(decodedString, qet->getText());
			t = new indri::lang::IndexTerm( decodedString );
			t->setStemmed(true);
			_nodes.push_back(t);
			
#line 1985 "QueryParser.cpp"
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
#line 563 "indrilang.g"
	 indri::lang::ExtentInside* r ;
#line 2000 "QueryParser.cpp"
#line 563 "indrilang.g"
	
	r = 0;
	indri::lang::DocumentStructureNode * ds = 0;
	
#line 2006 "QueryParser.cpp"
	
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
#line 567 "indrilang.g"
				
				ds = new indri::lang::DocumentStructureNode;
				_nodes.push_back(ds);
				r = new indri::lang::ExtentDescendant(NULL, NULL, ds);
				_nodes.push_back(r);
				
#line 2025 "QueryParser.cpp"
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
#line 572 "indrilang.g"
			
			if (r == 0) {
			ds = new indri::lang::DocumentStructureNode;
			_nodes.push_back(ds);
			r = new indri::lang::ExtentChild(NULL, NULL, ds);
			_nodes.push_back(r);
			}
			
#line 2049 "QueryParser.cpp"
		}
		break;
	}
	case B_SLASH:
	{
		match(B_SLASH);
		if ( inputState->guessing==0 ) {
#line 580 "indrilang.g"
			
			ds = new indri::lang::DocumentStructureNode;
			_nodes.push_back(ds);
			r = new indri::lang::ExtentParent(NULL, NULL, ds);
			_nodes.push_back(r);
			
#line 2064 "QueryParser.cpp"
		}
		break;
	}
	case O_BRACE:
	{
		match(O_BRACE);
		if ( inputState->guessing==0 ) {
#line 586 "indrilang.g"
			
			r = new indri::lang::ExtentInside(NULL, NULL);   
			_nodes.push_back(r);
			
#line 2077 "QueryParser.cpp"
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
#line 664 "indrilang.g"
	 indri::lang::Field* extent ;
#line 2092 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	match(O_SQUARE);
	fieldName = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 666 "indrilang.g"
		
		extent = new Field( fieldName->getText() );
		_nodes.push_back( extent );
		
#line 2104 "QueryParser.cpp"
	}
	match(C_SQUARE);
	return extent ;
}

 UINT64  QueryParser::date() {
#line 724 "indrilang.g"
	 UINT64 d ;
#line 2113 "QueryParser.cpp"
	
	bool synPredMatched230 = false;
	if (((LA(1) == NUMBER))) {
		int _m230 = mark();
		synPredMatched230 = true;
		inputState->guessing++;
		try {
			{
			match(NUMBER);
			match(SLASH);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched230 = false;
		}
		rewind(_m230);
		inputState->guessing--;
	}
	if ( synPredMatched230 ) {
		d=slashDate();
	}
	else {
		bool synPredMatched232 = false;
		if (((LA(1) == NUMBER))) {
			int _m232 = mark();
			synPredMatched232 = true;
			inputState->guessing++;
			try {
				{
				match(NUMBER);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched232 = false;
			}
			rewind(_m232);
			inputState->guessing--;
		}
		if ( synPredMatched232 ) {
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
#line 743 "indrilang.g"
	 UINT64 d ;
#line 2168 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 743 "indrilang.g"
	
	d = 0;
	
#line 2176 "QueryParser.cpp"
	
	month = LT(1);
	match(NUMBER);
	match(SLASH);
	day = LT(1);
	match(TERM);
	match(SLASH);
	year = LT(1);
	match(NUMBER);
	if ( inputState->guessing==0 ) {
#line 746 "indrilang.g"
		
		d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() ); 
		
#line 2191 "QueryParser.cpp"
	}
	return d ;
}

 UINT64  QueryParser::spaceDate() {
#line 750 "indrilang.g"
	 UINT64 d ;
#line 2199 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 750 "indrilang.g"
	
	d = 0;
	
#line 2207 "QueryParser.cpp"
	
	day = LT(1);
	match(NUMBER);
	month = LT(1);
	match(TERM);
	year = LT(1);
	match(NUMBER);
	if ( inputState->guessing==0 ) {
#line 753 "indrilang.g"
		
		d = indri::parse::DateParse::convertDate( year->getText(), month->getText(), day->getText() );
		
#line 2220 "QueryParser.cpp"
	}
	return d ;
}

 UINT64  QueryParser::dashDate() {
#line 729 "indrilang.g"
	 UINT64 d ;
#line 2228 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  dmy = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 729 "indrilang.g"
	
	d = 0;
	
#line 2234 "QueryParser.cpp"
	
	dmy = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 732 "indrilang.g"
		
		const std::string& text = dmy->getText();
		int firstDash = text.find('-');
		int secondDash = text.find('-', firstDash+1);
		std::string day = text.substr( 0, firstDash ); 
		std::string month = text.substr( firstDash+1, secondDash-firstDash-1 );
		std::string year = text.substr( secondDash );
		
		d = indri::parse::DateParse::convertDate( year, month, day ); 
		
#line 2250 "QueryParser.cpp"
	}
	return d ;
}

 INT64  QueryParser::number() {
#line 807 "indrilang.g"
	 INT64 v ;
#line 2258 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nn = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 807 "indrilang.g"
	
	v = 0;
	
#line 2265 "QueryParser.cpp"
	
	switch ( LA(1)) {
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 810 "indrilang.g"
			
			v = string_to_i64(n->getText());
			
#line 2277 "QueryParser.cpp"
		}
		break;
	}
	case NEGATIVE_NUMBER:
	{
		nn = LT(1);
		match(NEGATIVE_NUMBER);
		if ( inputState->guessing==0 ) {
#line 813 "indrilang.g"
			
			v = string_to_i64(nn->getText());
			
#line 2290 "QueryParser.cpp"
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

 indri::lang::ScoredExtentNode*  QueryParser::tieredList(
	 indri::lang::TieredCombinationNode * tn, indri::lang::RawExtentNode * ou 
) {
#line 875 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 2307 "QueryParser.cpp"
#line 875 "indrilang.g"
	
	double t = 0;
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = tn;
	
#line 2315 "QueryParser.cpp"
	
	{
	switch ( LA(1)) {
	case O_SQUARE:
	{
		sr=extentRestriction(tn, ou);
		if ( inputState->guessing==0 ) {
#line 882 "indrilang.g"
			ou = innerMost(sr);
#line 2325 "QueryParser.cpp"
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
	int _cnt246=0;
	for (;;) {
		if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
			t=floating();
			w=floating();
			n=scoredExtentNode(ou);
			if ( inputState->guessing==0 ) {
#line 888 "indrilang.g"
				tn->addChild( t, w, n );
#line 2350 "QueryParser.cpp"
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
	return sr ;
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
	"\"#tand\"",
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

const unsigned long QueryParser::_tokenSet_0_data_[] = { 4294967264UL, 4026532488UL, 0UL, 0UL };
// "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#band" "#wsyn" "#syn" "#prior" "#date:after" 
// "#date:before" "#date:between" "#less" "#greater" "#between" "#equals" 
// NUMBER NEGATIVE_NUMBER FLOAT "#tand" O_ANGLE O_BRACE DBL_QUOTE TERM 
// ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long QueryParser::_tokenSet_1_data_[] = { 2146370688UL, 4026532488UL, 0UL, 0UL };
// "#od" "#uw" "#any" "#band" "#wsyn" "#syn" "#date:after" "#date:before" 
// "#date:between" "#less" "#greater" "#between" "#equals" NUMBER NEGATIVE_NUMBER 
// FLOAT O_ANGLE O_BRACE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM 
// OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_1(_tokenSet_1_data_,4);


ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
