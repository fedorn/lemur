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
#line 198 "indrilang.g"
	 indri::lang::ScoredExtentNode* q ;
#line 37 "QueryParser.cpp"
#line 198 "indrilang.g"
	
	indri::lang::CombineNode* c = 0;
	indri::lang::ScoredExtentNode* s = 0;
	q = 0;
	
#line 44 "QueryParser.cpp"
	
	try {      // for error handling
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
		case DBL_QUOTE:
		case TERM:
		case ENCODED_QUOTED_TERM:
		case ENCODED_TERM:
		case OPERATOR:
		{
			s=scoredExtentNode();
			if ( inputState->guessing==0 ) {
#line 205 "indrilang.g"
				
				c = new CombineNode;
				c->addChild(q);
				c->addChild(s);
				_nodes.push_back(c);
				q = c;
				
#line 91 "QueryParser.cpp"
			}
			{ // ( ... )*
			for (;;) {
				if ((_tokenSet_0.member(LA(1)))) {
					s=scoredExtentNode();
					if ( inputState->guessing==0 ) {
#line 213 "indrilang.g"
						
						c->addChild(s);
						
#line 102 "QueryParser.cpp"
					}
				}
				else {
					goto _loop80;
				}
				
			}
			_loop80:;
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_1);
		} else {
			throw;
		}
	}
	return q ;
}

 indri::lang::ScoredExtentNode*  QueryParser::scoredExtentNode() {
#line 219 "indrilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 141 "QueryParser.cpp"
	
	try {      // for error handling
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return s ;
}

 indri::lang::ScoredExtentNode*  QueryParser::weightNode() {
#line 315 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 240 "QueryParser.cpp"
#line 315 "indrilang.g"
	
	indri::lang::WeightNode* wn = new indri::lang::WeightNode;
	_nodes.push_back(wn);
	
#line 246 "QueryParser.cpp"
	
	try {      // for error handling
		match(WEIGHT);
		r=weightedList(wn);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::combineNode() {
#line 322 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 267 "QueryParser.cpp"
#line 322 "indrilang.g"
	
	indri::lang::CombineNode* cn = new indri::lang::CombineNode;
	_nodes.push_back(cn);
	
#line 273 "QueryParser.cpp"
	
	try {      // for error handling
		match(COMBINE);
		r=unweightedList(cn);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::orNode() {
#line 350 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 294 "QueryParser.cpp"
#line 350 "indrilang.g"
	
	indri::lang::OrNode* on = new indri::lang::OrNode;
	_nodes.push_back(on);
	
#line 300 "QueryParser.cpp"
	
	try {      // for error handling
		match(OR);
		r=unweightedList(on);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::notNode() {
#line 364 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 321 "QueryParser.cpp"
#line 364 "indrilang.g"
	
	indri::lang::NotNode* n = new indri::lang::NotNode;
	indri::lang::ScoredExtentNode* c = 0;
	_nodes.push_back(n);
	r = n;
	
#line 329 "QueryParser.cpp"
	
	try {      // for error handling
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
#line 372 "indrilang.g"
			
			n->setChild(c);
			
#line 358 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::wandNode() {
#line 343 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 376 "QueryParser.cpp"
#line 343 "indrilang.g"
	
	indri::lang::WAndNode* wn = new indri::lang::WAndNode;
	_nodes.push_back(wn);
	
#line 382 "QueryParser.cpp"
	
	try {      // for error handling
		match(WAND);
		r=weightedList(wn);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::wsumNode() {
#line 336 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 403 "QueryParser.cpp"
#line 336 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 409 "QueryParser.cpp"
	
	try {      // for error handling
		match(WSUM);
		r=weightedList(wn);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return r ;
}

 indri::lang::ScoredExtentNode*  QueryParser::maxNode() {
#line 357 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 430 "QueryParser.cpp"
#line 357 "indrilang.g"
	
	indri::lang::MaxNode* mn = new indri::lang::MaxNode;
	_nodes.push_back(mn);
	
#line 436 "QueryParser.cpp"
	
	try {      // for error handling
		match(MAX);
		r=unweightedList(mn);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return r ;
}

 indri::lang::PriorNode*  QueryParser::priorNode() {
#line 376 "indrilang.g"
	 indri::lang::PriorNode* p ;
#line 457 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 376 "indrilang.g"
	
	indri::lang::Field* field = 0;
	p = 0;
	
#line 464 "QueryParser.cpp"
	
	try {      // for error handling
		match(PRIOR);
		match(O_PAREN);
		name = LT(1);
		match(TERM);
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 381 "indrilang.g"
			
			p = _priorFactory->create( name->getText() );
			
			if( p->getFieldName().length() ) {
			field = new Field( p->getFieldName() );
			p->setField( field );
			_nodes.push_back(field);
			}
			
			_nodes.push_back(p);
			
#line 485 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return p ;
}

 indri::lang::FilRejNode*  QueryParser::filrejNode() {
#line 452 "indrilang.g"
	 indri::lang::FilRejNode* fj ;
#line 503 "QueryParser.cpp"
#line 452 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* disallowed = 0;
	
#line 509 "QueryParser.cpp"
	
	try {      // for error handling
		match(FILREJ);
		match(O_PAREN);
		filter=unscoredTerm();
		disallowed=scoredExtentNode();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 458 "indrilang.g"
			
			fj = new FilRejNode( filter, disallowed );
			_nodes.push_back(fj);
			
#line 523 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return fj ;
}

 indri::lang::FilReqNode*  QueryParser::filreqNode() {
#line 463 "indrilang.g"
	 indri::lang::FilReqNode* fq ;
#line 541 "QueryParser.cpp"
#line 463 "indrilang.g"
	
	RawExtentNode* filter = 0;
	ScoredExtentNode* required = 0;
	
#line 547 "QueryParser.cpp"
	
	try {      // for error handling
		match(FILREQ);
		match(O_PAREN);
		filter=unscoredTerm();
		required=scoredExtentNode();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 469 "indrilang.g"
			
			fq = new FilReqNode( filter, required );
			_nodes.push_back(fq);
			
#line 561 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return fq ;
}

 ScoredExtentNode*  QueryParser::scoredRaw() {
#line 238 "indrilang.g"
	 ScoredExtentNode* sn ;
#line 579 "QueryParser.cpp"
#line 238 "indrilang.g"
	
	RawExtentNode* raw = 0;
	RawExtentNode* contexts = 0;
	sn = 0;
	
#line 586 "QueryParser.cpp"
	
	try {      // for error handling
		bool synPredMatched104 = false;
		if (((_tokenSet_3.member(LA(1))))) {
			int _m104 = mark();
			synPredMatched104 = true;
			inputState->guessing++;
			try {
				{
				qualifiedTerm();
				context_list();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched104 = false;
			}
			rewind(_m104);
			inputState->guessing--;
		}
		if ( synPredMatched104 ) {
			raw=qualifiedTerm();
			contexts=context_list();
			if ( inputState->guessing==0 ) {
#line 245 "indrilang.g"
				
				sn = new indri::lang::RawScorerNode( raw, contexts );
				_nodes.push_back(sn);
				
#line 615 "QueryParser.cpp"
			}
		}
		else {
			bool synPredMatched106 = false;
			if (((_tokenSet_3.member(LA(1))))) {
				int _m106 = mark();
				synPredMatched106 = true;
				inputState->guessing++;
				try {
					{
					qualifiedTerm();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched106 = false;
				}
				rewind(_m106);
				inputState->guessing--;
			}
			if ( synPredMatched106 ) {
				raw=qualifiedTerm();
				if ( inputState->guessing==0 ) {
#line 250 "indrilang.g"
					
					sn = new indri::lang::RawScorerNode( raw, contexts );
					_nodes.push_back(sn);
					
#line 643 "QueryParser.cpp"
				}
			}
			else {
				bool synPredMatched108 = false;
				if (((_tokenSet_3.member(LA(1))))) {
					int _m108 = mark();
					synPredMatched108 = true;
					inputState->guessing++;
					try {
						{
						unqualifiedTerm();
						match(DOT);
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched108 = false;
					}
					rewind(_m108);
					inputState->guessing--;
				}
				if ( synPredMatched108 ) {
					raw=unqualifiedTerm();
					match(DOT);
					contexts=context_list();
					if ( inputState->guessing==0 ) {
#line 255 "indrilang.g"
						
						sn = new indri::lang::RawScorerNode( raw, contexts );
						_nodes.push_back(sn);
						
#line 674 "QueryParser.cpp"
					}
				}
				else if ((_tokenSet_3.member(LA(1)))) {
					raw=unqualifiedTerm();
					if ( inputState->guessing==0 ) {
#line 260 "indrilang.g"
						
						sn = new indri::lang::RawScorerNode( raw, contexts );
						_nodes.push_back(sn);
						
#line 685 "QueryParser.cpp"
					}
				}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return sn ;
}

 RawExtentNode*  QueryParser::qualifiedTerm() {
#line 490 "indrilang.g"
	 RawExtentNode* t ;
#line 708 "QueryParser.cpp"
#line 490 "indrilang.g"
	
	RawExtentNode* synonyms = 0;
	RawExtentNode* fields = 0;
	t = 0;
	
#line 715 "QueryParser.cpp"
	
	try {      // for error handling
		synonyms=unqualifiedTerm();
		match(DOT);
		fields=field_list();
		if ( inputState->guessing==0 ) {
#line 497 "indrilang.g"
			
			if( fields ) {
			t = new indri::lang::ExtentInside( synonyms, fields );
			_nodes.push_back(t);
			} else {
			t = synonyms;
			}
			
#line 731 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_4);
		} else {
			throw;
		}
	}
	return t ;
}

 ExtentOr*  QueryParser::context_list() {
#line 573 "indrilang.g"
	 ExtentOr* contexts ;
#line 749 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 573 "indrilang.g"
	
	contexts = new ExtentOr;
	_nodes.push_back( contexts );
	
#line 757 "QueryParser.cpp"
	
	try {      // for error handling
		match(O_PAREN);
		first = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 579 "indrilang.g"
			
			Field* firstField = new indri::lang::Field( first->getText() );
			_nodes.push_back( firstField );
			contexts->addChild( firstField );
			
#line 770 "QueryParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				additional = LT(1);
				match(TERM);
				if ( inputState->guessing==0 ) {
#line 586 "indrilang.g"
					
					Field* additionalField = new Field(additional->getText());
					_nodes.push_back( additionalField );
					contexts->addChild( additionalField );
					
#line 785 "QueryParser.cpp"
				}
			}
			else {
				goto _loop199;
			}
			
		}
		_loop199:;
		} // ( ... )*
		match(C_PAREN);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return contexts ;
}

 RawExtentNode*  QueryParser::unqualifiedTerm() {
#line 506 "indrilang.g"
	 RawExtentNode* re ;
#line 812 "QueryParser.cpp"
	
	try {      // for error handling
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
		case SYN:
		{
			re=synonym_list_alt();
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return re ;
}

 indri::lang::ScoredExtentNode*  QueryParser::weightedList(
	 indri::lang::WeightedCombinationNode* wn 
) {
#line 276 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 915 "QueryParser.cpp"
#line 276 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 922 "QueryParser.cpp"
	
	try {      // for error handling
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
		int _cnt112=0;
		for (;;) {
			if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
				w=floating();
				n=scoredExtentNode();
				if ( inputState->guessing==0 ) {
#line 287 "indrilang.g"
					wn->addChild( w, n );
#line 952 "QueryParser.cpp"
				}
			}
			else {
				if ( _cnt112>=1 ) { goto _loop112; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt112++;
		}
		_loop112:;
		}  // ( ... )+
		match(C_PAREN);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return sr ;
}

 indri::lang::ExtentRestriction*  QueryParser::extentRestriction(
	 indri::lang::ScoredExtentNode* sn 
) {
#line 522 "indrilang.g"
	 indri::lang::ExtentRestriction* er ;
#line 982 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 522 "indrilang.g"
	
	indri::lang::Field* f = 0;
	er = 0;
	
#line 989 "QueryParser.cpp"
	
	try {      // for error handling
		match(O_SQUARE);
		field = LT(1);
		match(TERM);
		match(C_SQUARE);
		if ( inputState->guessing==0 ) {
#line 527 "indrilang.g"
			
			f = new indri::lang::Field(field->getText());
			_nodes.push_back(f);
			er = new indri::lang::ExtentRestriction(sn, f);
			_nodes.push_back(er);
			
#line 1004 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_6);
		} else {
			throw;
		}
	}
	return er ;
}

 double  QueryParser::floating() {
#line 723 "indrilang.g"
	 double d ;
#line 1022 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 723 "indrilang.g"
	
	d = 0;
	
#line 1029 "QueryParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case FLOAT:
		{
			f = LT(1);
			match(FLOAT);
			if ( inputState->guessing==0 ) {
#line 726 "indrilang.g"
				
				d = atof(f->getText().c_str());
				
#line 1042 "QueryParser.cpp"
			}
			break;
		}
		case NUMBER:
		{
			n = LT(1);
			match(NUMBER);
			if ( inputState->guessing==0 ) {
#line 729 "indrilang.g"
				
				d = atof(n->getText().c_str());
				
#line 1055 "QueryParser.cpp"
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_0);
		} else {
			throw;
		}
	}
	return d ;
}

 indri::lang::ScoredExtentNode*  QueryParser::sumList(
	 indri::lang::WSumNode* wn 
) {
#line 292 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 1082 "QueryParser.cpp"
#line 292 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 1089 "QueryParser.cpp"
	
	try {      // for error handling
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
			if ((_tokenSet_0.member(LA(1)))) {
				n=scoredExtentNode();
				if ( inputState->guessing==0 ) {
#line 300 "indrilang.g"
					wn->addChild( 1.0, n );
#line 1118 "QueryParser.cpp"
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_1);
		} else {
			throw;
		}
	}
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::unweightedList(
	 indri::lang::UnweightedCombinationNode* cn 
) {
#line 304 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 1148 "QueryParser.cpp"
#line 304 "indrilang.g"
	
	ScoredExtentNode* n = 0;
	sr = cn;
	
#line 1154 "QueryParser.cpp"
	
	try {      // for error handling
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
		int _cnt120=0;
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				n=scoredExtentNode();
				if ( inputState->guessing==0 ) {
#line 311 "indrilang.g"
					cn->addChild( n );
#line 1183 "QueryParser.cpp"
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return sr ;
}

 indri::lang::ScoredExtentNode*  QueryParser::sumNode() {
#line 329 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 1211 "QueryParser.cpp"
#line 329 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 1217 "QueryParser.cpp"
	
	try {      // for error handling
		match(SUM);
		r=sumList(wn);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_1);
		} else {
			throw;
		}
	}
	return r ;
}

 indri::lang::ODNode*  QueryParser::odNode() {
#line 403 "indrilang.g"
	 indri::lang::ODNode* od ;
#line 1238 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 403 "indrilang.g"
	
	RawExtentNode* rn = 0;
	od = new indri::lang::ODNode;
	_nodes.push_back(od);
	
#line 1247 "QueryParser.cpp"
	
	try {      // for error handling
		{
		bool synPredMatched134 = false;
		if (((LA(1) == OD))) {
			int _m134 = mark();
			synPredMatched134 = true;
			inputState->guessing++;
			try {
				{
				match(OD);
				match(NUMBER);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched134 = false;
			}
			rewind(_m134);
			inputState->guessing--;
		}
		if ( synPredMatched134 ) {
			{
			match(OD);
			n1 = LT(1);
			match(NUMBER);
			if ( inputState->guessing==0 ) {
#line 412 "indrilang.g"
				od->setWindowSize( n1->getText() );
#line 1276 "QueryParser.cpp"
			}
			}
		}
		else {
			bool synPredMatched137 = false;
			if (((LA(1) == OD))) {
				int _m137 = mark();
				synPredMatched137 = true;
				inputState->guessing++;
				try {
					{
					match(OD);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched137 = false;
				}
				rewind(_m137);
				inputState->guessing--;
			}
			if ( synPredMatched137 ) {
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
#line 416 "indrilang.g"
					od->setWindowSize( n2->getText() );
#line 1310 "QueryParser.cpp"
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
		int _cnt141=0;
		for (;;) {
			if ((_tokenSet_3.member(LA(1)))) {
				rn=unscoredTerm();
				if ( inputState->guessing==0 ) {
#line 420 "indrilang.g"
					od->addChild( rn );
#line 1328 "QueryParser.cpp"
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return od ;
}

 RawExtentNode*  QueryParser::unscoredTerm() {
#line 483 "indrilang.g"
	 RawExtentNode* t ;
#line 1356 "QueryParser.cpp"
#line 483 "indrilang.g"
	
	t = 0;
	
#line 1361 "QueryParser.cpp"
	
	try {      // for error handling
		bool synPredMatched158 = false;
		if (((_tokenSet_3.member(LA(1))))) {
			int _m158 = mark();
			synPredMatched158 = true;
			inputState->guessing++;
			try {
				{
				qualifiedTerm();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched158 = false;
			}
			rewind(_m158);
			inputState->guessing--;
		}
		if ( synPredMatched158 ) {
			t=qualifiedTerm();
		}
		else if ((_tokenSet_3.member(LA(1)))) {
			t=unqualifiedTerm();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_7);
		} else {
			throw;
		}
	}
	return t ;
}

 indri::lang::UWNode*  QueryParser::uwNode() {
#line 424 "indrilang.g"
	 indri::lang::UWNode* uw ;
#line 1406 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 424 "indrilang.g"
	
	uw = new indri::lang::UWNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(uw);
	
#line 1414 "QueryParser.cpp"
	
	try {      // for error handling
		{
		bool synPredMatched145 = false;
		if (((LA(1) == UW))) {
			int _m145 = mark();
			synPredMatched145 = true;
			inputState->guessing++;
			try {
				{
				match(UW);
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
			match(UW);
			n = LT(1);
			match(NUMBER);
			if ( inputState->guessing==0 ) {
#line 432 "indrilang.g"
				uw->setWindowSize( n->getText() );
#line 1443 "QueryParser.cpp"
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
		int _cnt149=0;
		for (;;) {
			if ((_tokenSet_3.member(LA(1)))) {
				rn=unscoredTerm();
				if ( inputState->guessing==0 ) {
#line 438 "indrilang.g"
					uw->addChild( rn );
#line 1466 "QueryParser.cpp"
				}
			}
			else {
				if ( _cnt149>=1 ) { goto _loop149; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt149++;
		}
		_loop149:;
		}  // ( ... )+
		match(C_PAREN);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return uw ;
}

 indri::lang::BAndNode*  QueryParser::bandNode() {
#line 441 "indrilang.g"
	 indri::lang::BAndNode* b ;
#line 1494 "QueryParser.cpp"
#line 441 "indrilang.g"
	
	b = new indri::lang::BAndNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(b);
	
#line 1501 "QueryParser.cpp"
	
	try {      // for error handling
		match(BAND);
		match(O_PAREN);
		{ // ( ... )+
		int _cnt152=0;
		for (;;) {
			if ((_tokenSet_3.member(LA(1)))) {
				rn=unscoredTerm();
				if ( inputState->guessing==0 ) {
#line 449 "indrilang.g"
					b->addChild( rn );
#line 1514 "QueryParser.cpp"
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return b ;
}

 indri::lang::Field*  QueryParser::anyField() {
#line 474 "indrilang.g"
	 indri::lang::Field* f ;
#line 1542 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 474 "indrilang.g"
	
	f = 0;
	
#line 1548 "QueryParser.cpp"
	
	try {      // for error handling
		match(ANY);
		match(COLON);
		t = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 478 "indrilang.g"
			
			f = new Field(t->getText());
			_nodes.push_back(f);
			
#line 1561 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return f ;
}

 ExtentAnd*  QueryParser::field_list() {
#line 553 "indrilang.g"
	 ExtentAnd* fields ;
#line 1579 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 553 "indrilang.g"
	
	fields = new ExtentAnd;
	_nodes.push_back( fields );
	
#line 1587 "QueryParser.cpp"
	
	try {      // for error handling
		first = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 559 "indrilang.g"
			
			Field* firstField = new indri::lang::Field( first->getText() );
			_nodes.push_back( firstField );
			fields->addChild( firstField );
			
#line 1599 "QueryParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				additional = LT(1);
				match(TERM);
				if ( inputState->guessing==0 ) {
#line 566 "indrilang.g"
					
					Field* additionalField = new Field(additional->getText());
					_nodes.push_back( additionalField );
					fields->addChild( additionalField );
					
#line 1614 "QueryParser.cpp"
				}
			}
			else {
				goto _loop196;
			}
			
		}
		_loop196:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_4);
		} else {
			throw;
		}
	}
	return fields ;
}

 FieldLessNode*  QueryParser::dateBefore() {
#line 602 "indrilang.g"
	 FieldLessNode* extent ;
#line 1640 "QueryParser.cpp"
#line 602 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1647 "QueryParser.cpp"
	
	try {      // for error handling
		match(DATEBEFORE);
		match(O_PAREN);
		d=date();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 607 "indrilang.g"
			
			dateField = new Field("date");
			extent = new FieldLessNode( dateField, d );
			_nodes.push_back( dateField );
			_nodes.push_back( extent );
			
#line 1662 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return extent ;
}

 FieldGreaterNode*  QueryParser::dateAfter() {
#line 614 "indrilang.g"
	 FieldGreaterNode* extent ;
#line 1680 "QueryParser.cpp"
#line 614 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1687 "QueryParser.cpp"
	
	try {      // for error handling
		match(DATEAFTER);
		match(O_PAREN);
		d=date();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 619 "indrilang.g"
			
			dateField = new Field("date");
			extent = new FieldGreaterNode( dateField, d );
			_nodes.push_back( dateField );
			_nodes.push_back( extent );
			
#line 1702 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return extent ;
}

 FieldBetweenNode*  QueryParser::dateBetween() {
#line 626 "indrilang.g"
	 FieldBetweenNode* extent ;
#line 1720 "QueryParser.cpp"
#line 626 "indrilang.g"
	
	UINT64 low = 0;
	UINT64 high = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1728 "QueryParser.cpp"
	
	try {      // for error handling
		match(DATEBETWEEN);
		match(O_PAREN);
		low=date();
		match(COMMA);
		high=date();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 632 "indrilang.g"
			
			dateField = new Field("date");
			extent = new FieldBetweenNode( dateField, low, high );
			_nodes.push_back( dateField );
			_nodes.push_back( extent );
			
#line 1745 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return extent ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list() {
#line 534 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1763 "QueryParser.cpp"
#line 534 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1770 "QueryParser.cpp"
	
	try {      // for error handling
		match(O_ANGLE);
		{ // ( ... )+
		int _cnt190=0;
		for (;;) {
			if ((_tokenSet_3.member(LA(1)))) {
				term=unscoredTerm();
				if ( inputState->guessing==0 ) {
#line 540 "indrilang.g"
					s->addChild(term);
#line 1782 "QueryParser.cpp"
				}
			}
			else {
				if ( _cnt190>=1 ) { goto _loop190; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt190++;
		}
		_loop190:;
		}  // ( ... )+
		match(C_ANGLE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return s ;
}

 indri::lang::ExtentOr*  QueryParser::synonym_list_alt() {
#line 543 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1810 "QueryParser.cpp"
#line 543 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1817 "QueryParser.cpp"
	
	try {      // for error handling
		match(SYN);
		match(O_PAREN);
		{ // ( ... )+
		int _cnt193=0;
		for (;;) {
			if ((_tokenSet_3.member(LA(1)))) {
				term=unscoredTerm();
				if ( inputState->guessing==0 ) {
#line 550 "indrilang.g"
					s->addChild(term);
#line 1830 "QueryParser.cpp"
				}
			}
			else {
				if ( _cnt193>=1 ) { goto _loop193; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt193++;
		}
		_loop193:;
		}  // ( ... )+
		match(C_PAREN);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return s ;
}

 FieldLessNode*  QueryParser::lessNode() {
#line 755 "indrilang.g"
	 FieldLessNode* ln ;
#line 1858 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 755 "indrilang.g"
	
	ln = 0;
	Field* compareField = 0;
	INT64 high = 0;
	
#line 1866 "QueryParser.cpp"
	
	try {      // for error handling
		match(LESS);
		match(O_PAREN);
		field = LT(1);
		match(TERM);
		high=number();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 760 "indrilang.g"
			
			compareField = new Field(field->getText());
			ln = new FieldLessNode( compareField, high );
			_nodes.push_back( compareField );
			_nodes.push_back( ln );
			
#line 1883 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return ln ;
}

 FieldGreaterNode*  QueryParser::greaterNode() {
#line 743 "indrilang.g"
	 FieldGreaterNode* gn ;
#line 1901 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 743 "indrilang.g"
	
	gn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	
#line 1909 "QueryParser.cpp"
	
	try {      // for error handling
		match(GREATER);
		match(O_PAREN);
		field = LT(1);
		match(TERM);
		low=number();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 748 "indrilang.g"
			
			compareField = new Field(field->getText());
			gn = new FieldGreaterNode( compareField, low );
			_nodes.push_back( compareField );
			_nodes.push_back( gn );
			
#line 1926 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return gn ;
}

 FieldBetweenNode*  QueryParser::betweenNode() {
#line 767 "indrilang.g"
	 FieldBetweenNode* bn ;
#line 1944 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 767 "indrilang.g"
	
	bn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	INT64 high = 0;
	
#line 1953 "QueryParser.cpp"
	
	try {      // for error handling
		match(BETWEEN);
		match(O_PAREN);
		field = LT(1);
		match(TERM);
		low=number();
		high=number();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 773 "indrilang.g"
			
			compareField = new Field(field->getText());
			bn = new FieldBetweenNode( compareField, low, high );
			_nodes.push_back( compareField );
			_nodes.push_back( bn );
			
#line 1971 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return bn ;
}

 FieldEqualsNode*  QueryParser::equalsNode() {
#line 780 "indrilang.g"
	 FieldEqualsNode* en ;
#line 1989 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 780 "indrilang.g"
	
	en = 0;
	Field* compareField = 0;
	INT64 eq = 0;
	
#line 1997 "QueryParser.cpp"
	
	try {      // for error handling
		match(EQUALS);
		match(O_PAREN);
		field = LT(1);
		match(TERM);
		eq=number();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 785 "indrilang.g"
			
			compareField = new Field(field->getText());
			en = new FieldEqualsNode( compareField, eq );
			_nodes.push_back( compareField );
			_nodes.push_back( en );
			
#line 2014 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return en ;
}

 IndexTerm*  QueryParser::rawText() {
#line 688 "indrilang.g"
	 IndexTerm* t ;
#line 2032 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  et = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  qet = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 688 "indrilang.g"
	
	t = 0;
	
#line 2042 "QueryParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case TERM:
		{
			id = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 691 "indrilang.g"
				
				t = new indri::lang::IndexTerm(id->getText());
				_nodes.push_back(t);
				
#line 2056 "QueryParser.cpp"
			}
			break;
		}
		case NUMBER:
		{
			n = LT(1);
			match(NUMBER);
			if ( inputState->guessing==0 ) {
#line 695 "indrilang.g"
				
				t = new indri::lang::IndexTerm(n->getText());
				_nodes.push_back(t);
				
#line 2070 "QueryParser.cpp"
			}
			break;
		}
		case FLOAT:
		{
			f = LT(1);
			match(FLOAT);
			if ( inputState->guessing==0 ) {
#line 699 "indrilang.g"
				
				t = new indri::lang::IndexTerm(f->getText());
				_nodes.push_back(t);
				
#line 2084 "QueryParser.cpp"
			}
			break;
		}
		case DBL_QUOTE:
		{
			match(DBL_QUOTE);
			t=rawText();
			match(DBL_QUOTE);
			if ( inputState->guessing==0 ) {
#line 703 "indrilang.g"
				
				// if a text term appears in quotes, consider it stemmed
				t->setStemmed(true);
				
#line 2099 "QueryParser.cpp"
			}
			break;
		}
		case ENCODED_TERM:
		{
			et = LT(1);
			match(ENCODED_TERM);
			if ( inputState->guessing==0 ) {
#line 707 "indrilang.g"
				
				std::string decodedString; 
				base64_decode_string(decodedString, et->getText());
				t = new indri::lang::IndexTerm( decodedString );
				_nodes.push_back(t);
				
#line 2115 "QueryParser.cpp"
			}
			break;
		}
		case ENCODED_QUOTED_TERM:
		{
			qet = LT(1);
			match(ENCODED_QUOTED_TERM);
			if ( inputState->guessing==0 ) {
#line 713 "indrilang.g"
				
				std::string decodedString; 
				base64_decode_string(decodedString, et->getText());
				t = new indri::lang::IndexTerm( decodedString );
				t->setStemmed(true);
				_nodes.push_back(t);
				
#line 2132 "QueryParser.cpp"
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		} else {
			throw;
		}
	}
	return t ;
}

 Field*  QueryParser::field_restriction() {
#line 594 "indrilang.g"
	 Field* extent ;
#line 2157 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(O_SQUARE);
		fieldName = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 596 "indrilang.g"
			
			extent = new Field( fieldName->getText() );
			_nodes.push_back( extent );
			
#line 2170 "QueryParser.cpp"
		}
		match(C_SQUARE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_1);
		} else {
			throw;
		}
	}
	return extent ;
}

 UINT64  QueryParser::date() {
#line 654 "indrilang.g"
	 UINT64 d ;
#line 2189 "QueryParser.cpp"
	
	try {      // for error handling
		bool synPredMatched206 = false;
		if (((LA(1) == NUMBER))) {
			int _m206 = mark();
			synPredMatched206 = true;
			inputState->guessing++;
			try {
				{
				match(NUMBER);
				match(SLASH);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched206 = false;
			}
			rewind(_m206);
			inputState->guessing--;
		}
		if ( synPredMatched206 ) {
			d=slashDate();
		}
		else {
			bool synPredMatched208 = false;
			if (((LA(1) == NUMBER))) {
				int _m208 = mark();
				synPredMatched208 = true;
				inputState->guessing++;
				try {
					{
					match(NUMBER);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched208 = false;
				}
				rewind(_m208);
				inputState->guessing--;
			}
			if ( synPredMatched208 ) {
				d=spaceDate();
			}
			else if ((LA(1) == TERM)) {
				d=dashDate();
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_8);
		} else {
			throw;
		}
	}
	return d ;
}

 UINT64  QueryParser::slashDate() {
#line 673 "indrilang.g"
	 UINT64 d ;
#line 2255 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 673 "indrilang.g"
	
	d = 0;
	
#line 2263 "QueryParser.cpp"
	
	try {      // for error handling
		month = LT(1);
		match(NUMBER);
		match(SLASH);
		day = LT(1);
		match(TERM);
		match(SLASH);
		year = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 676 "indrilang.g"
			
			d = DateParse::convertDate( year->getText(), month->getText(), day->getText() ); 
			
#line 2279 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_8);
		} else {
			throw;
		}
	}
	return d ;
}

 UINT64  QueryParser::spaceDate() {
#line 680 "indrilang.g"
	 UINT64 d ;
#line 2297 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 680 "indrilang.g"
	
	d = 0;
	
#line 2305 "QueryParser.cpp"
	
	try {      // for error handling
		day = LT(1);
		match(NUMBER);
		month = LT(1);
		match(TERM);
		year = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 683 "indrilang.g"
			
			d = DateParse::convertDate( year->getText(), month->getText(), day->getText() );
			
#line 2319 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_8);
		} else {
			throw;
		}
	}
	return d ;
}

 UINT64  QueryParser::dashDate() {
#line 659 "indrilang.g"
	 UINT64 d ;
#line 2337 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  dmy = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 659 "indrilang.g"
	
	d = 0;
	
#line 2343 "QueryParser.cpp"
	
	try {      // for error handling
		dmy = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 662 "indrilang.g"
			
			const std::string& text = dmy->getText();
			int firstDash = text.find('-');
			int secondDash = text.find('-', firstDash+1);
			std::string day = text.substr( 0, firstDash ); 
			std::string month = text.substr( firstDash+1, secondDash-firstDash-1 );
			std::string year = text.substr( secondDash );
			
			d = DateParse::convertDate( year, month, day ); 
			
#line 2360 "QueryParser.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_8);
		} else {
			throw;
		}
	}
	return d ;
}

 INT64  QueryParser::number() {
#line 733 "indrilang.g"
	 INT64 v ;
#line 2378 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nn = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 733 "indrilang.g"
	
	v = 0;
	
#line 2385 "QueryParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case NUMBER:
		{
			n = LT(1);
			match(NUMBER);
			if ( inputState->guessing==0 ) {
#line 736 "indrilang.g"
				
				v = string_to_i64(n->getText());
				
#line 2398 "QueryParser.cpp"
			}
			break;
		}
		case NEGATIVE_NUMBER:
		{
			nn = LT(1);
			match(NEGATIVE_NUMBER);
			if ( inputState->guessing==0 ) {
#line 739 "indrilang.g"
				
				v = string_to_i64(nn->getText());
				
#line 2411 "QueryParser.cpp"
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_9);
		} else {
			throw;
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

const unsigned long QueryParser::_tokenSet_0_data_[] = { 805306336UL, 62914594UL, 0UL, 0UL };
// "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#band" "#syn" "#prior" "#date:after" "#date:before" 
// "#date:between" "#less" "#greater" "#between" "#equals" NUMBER FLOAT 
// O_ANGLE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long QueryParser::_tokenSet_1_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long QueryParser::_tokenSet_2_data_[] = { 805306338UL, 62914595UL, 0UL, 0UL };
// EOF "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#band" "#syn" "#prior" "#date:after" "#date:before" 
// "#date:between" "#less" "#greater" "#between" "#equals" NUMBER FLOAT 
// C_PAREN O_ANGLE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long QueryParser::_tokenSet_3_data_[] = { 804717696UL, 62914594UL, 0UL, 0UL };
// "#od" "#uw" "#any" "#band" "#syn" "#date:after" "#date:before" "#date:between" 
// "#less" "#greater" "#between" "#equals" NUMBER FLOAT O_ANGLE DBL_QUOTE 
// TERM ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long QueryParser::_tokenSet_4_data_[] = { 2952789986UL, 62914599UL, 0UL, 0UL };
// EOF "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#band" "#syn" "#prior" "#date:after" "#date:before" 
// "#date:between" "#less" "#greater" "#between" "#equals" NUMBER FLOAT 
// O_PAREN C_PAREN O_ANGLE C_ANGLE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM 
// OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long QueryParser::_tokenSet_5_data_[] = { 805306338UL, 62914727UL, 0UL, 0UL };
// EOF "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#band" "#syn" "#prior" "#date:after" "#date:before" 
// "#date:between" "#less" "#greater" "#between" "#equals" NUMBER FLOAT 
// C_PAREN O_ANGLE C_ANGLE DBL_QUOTE DOT TERM ENCODED_QUOTED_TERM ENCODED_TERM 
// OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long QueryParser::_tokenSet_6_data_[] = { 2147483648UL, 0UL, 0UL, 0UL };
// O_PAREN 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_6(_tokenSet_6_data_,4);
const unsigned long QueryParser::_tokenSet_7_data_[] = { 805306336UL, 62914599UL, 0UL, 0UL };
// "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#band" "#syn" "#prior" "#date:after" "#date:before" 
// "#date:between" "#less" "#greater" "#between" "#equals" NUMBER FLOAT 
// C_PAREN O_ANGLE C_ANGLE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM 
// OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_7(_tokenSet_7_data_,4);
const unsigned long QueryParser::_tokenSet_8_data_[] = { 0UL, 257UL, 0UL, 0UL };
// C_PAREN COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_8(_tokenSet_8_data_,4);
const unsigned long QueryParser::_tokenSet_9_data_[] = { 402653184UL, 1UL, 0UL, 0UL };
// NUMBER NEGATIVE_NUMBER C_PAREN 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_9(_tokenSet_9_data_,4);


ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
