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
#line 193 "indrilang.g"
	 indri::lang::ScoredExtentNode* q ;
#line 37 "QueryParser.cpp"
#line 193 "indrilang.g"
	
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
#line 200 "indrilang.g"
				
				c = new CombineNode;
				c->addChild(q);
				c->addChild(s);
				_nodes.push_back(c);
				q = c;
				
#line 89 "QueryParser.cpp"
			}
			{ // ( ... )*
			for (;;) {
				if ((_tokenSet_0.member(LA(1)))) {
					s=scoredExtentNode();
					if ( inputState->guessing==0 ) {
#line 208 "indrilang.g"
						
						c->addChild(s);
						
#line 100 "QueryParser.cpp"
					}
				}
				else {
					goto _loop75;
				}
				
			}
			_loop75:;
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
#line 214 "indrilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 139 "QueryParser.cpp"
	
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
		case OD:
		case UW:
		case FILREQ:
		case FILREJ:
		case ANY:
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
#line 308 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 228 "QueryParser.cpp"
#line 308 "indrilang.g"
	
	indri::lang::WeightNode* wn = new indri::lang::WeightNode;
	_nodes.push_back(wn);
	
#line 234 "QueryParser.cpp"
	
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
#line 315 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 255 "QueryParser.cpp"
#line 315 "indrilang.g"
	
	indri::lang::CombineNode* cn = new indri::lang::CombineNode;
	_nodes.push_back(cn);
	
#line 261 "QueryParser.cpp"
	
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
#line 343 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 282 "QueryParser.cpp"
#line 343 "indrilang.g"
	
	indri::lang::OrNode* on = new indri::lang::OrNode;
	_nodes.push_back(on);
	
#line 288 "QueryParser.cpp"
	
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
#line 357 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 309 "QueryParser.cpp"
#line 357 "indrilang.g"
	
	indri::lang::NotNode* n = new indri::lang::NotNode;
	indri::lang::ScoredExtentNode* c = 0;
	_nodes.push_back(n);
	r = n;
	
#line 317 "QueryParser.cpp"
	
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
#line 365 "indrilang.g"
			
			n->setChild(c);
			
#line 346 "QueryParser.cpp"
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
#line 336 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 364 "QueryParser.cpp"
#line 336 "indrilang.g"
	
	indri::lang::WAndNode* wn = new indri::lang::WAndNode;
	_nodes.push_back(wn);
	
#line 370 "QueryParser.cpp"
	
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
#line 329 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 391 "QueryParser.cpp"
#line 329 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 397 "QueryParser.cpp"
	
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
#line 350 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 418 "QueryParser.cpp"
#line 350 "indrilang.g"
	
	indri::lang::MaxNode* mn = new indri::lang::MaxNode;
	_nodes.push_back(mn);
	
#line 424 "QueryParser.cpp"
	
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
#line 369 "indrilang.g"
	 indri::lang::PriorNode* p ;
#line 445 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 369 "indrilang.g"
	
	indri::lang::Field* field = 0;
	p = 0;
	
#line 452 "QueryParser.cpp"
	
	try {      // for error handling
		match(PRIOR);
		match(O_PAREN);
		name = LT(1);
		match(TERM);
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 374 "indrilang.g"
			
			p = _priorFactory->create( name->getText() );
			
			if( p->getFieldName().length() ) {
			field = new Field( p->getFieldName() );
			p->setField( field );
			_nodes.push_back(field);
			}
			
			_nodes.push_back(p);
			
#line 473 "QueryParser.cpp"
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

 ScoredExtentNode*  QueryParser::scoredRaw() {
#line 231 "indrilang.g"
	 ScoredExtentNode* sn ;
#line 491 "QueryParser.cpp"
#line 231 "indrilang.g"
	
	RawExtentNode* raw = 0;
	RawExtentNode* contexts = 0;
	sn = 0;
	
#line 498 "QueryParser.cpp"
	
	try {      // for error handling
		bool synPredMatched95 = false;
		if (((_tokenSet_3.member(LA(1))))) {
			int _m95 = mark();
			synPredMatched95 = true;
			inputState->guessing++;
			try {
				{
				qualifiedTerm();
				context_list();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched95 = false;
			}
			rewind(_m95);
			inputState->guessing--;
		}
		if ( synPredMatched95 ) {
			raw=qualifiedTerm();
			contexts=context_list();
			if ( inputState->guessing==0 ) {
#line 238 "indrilang.g"
				
				sn = new indri::lang::RawScorerNode( raw, contexts );
				_nodes.push_back(sn);
				
#line 527 "QueryParser.cpp"
			}
		}
		else {
			bool synPredMatched97 = false;
			if (((_tokenSet_3.member(LA(1))))) {
				int _m97 = mark();
				synPredMatched97 = true;
				inputState->guessing++;
				try {
					{
					qualifiedTerm();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched97 = false;
				}
				rewind(_m97);
				inputState->guessing--;
			}
			if ( synPredMatched97 ) {
				raw=qualifiedTerm();
				if ( inputState->guessing==0 ) {
#line 243 "indrilang.g"
					
					sn = new indri::lang::RawScorerNode( raw, contexts );
					_nodes.push_back(sn);
					
#line 555 "QueryParser.cpp"
				}
			}
			else {
				bool synPredMatched99 = false;
				if (((_tokenSet_3.member(LA(1))))) {
					int _m99 = mark();
					synPredMatched99 = true;
					inputState->guessing++;
					try {
						{
						unqualifiedTerm();
						match(DOT);
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched99 = false;
					}
					rewind(_m99);
					inputState->guessing--;
				}
				if ( synPredMatched99 ) {
					raw=unqualifiedTerm();
					match(DOT);
					contexts=context_list();
					if ( inputState->guessing==0 ) {
#line 248 "indrilang.g"
						
						sn = new indri::lang::RawScorerNode( raw, contexts );
						_nodes.push_back(sn);
						
#line 586 "QueryParser.cpp"
					}
				}
				else if ((_tokenSet_3.member(LA(1)))) {
					raw=unqualifiedTerm();
					if ( inputState->guessing==0 ) {
#line 253 "indrilang.g"
						
						sn = new indri::lang::RawScorerNode( raw, contexts );
						_nodes.push_back(sn);
						
#line 597 "QueryParser.cpp"
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
#line 471 "indrilang.g"
	 RawExtentNode* t ;
#line 620 "QueryParser.cpp"
#line 471 "indrilang.g"
	
	RawExtentNode* synonyms = 0;
	RawExtentNode* fields = 0;
	t = 0;
	
#line 627 "QueryParser.cpp"
	
	try {      // for error handling
		synonyms=unqualifiedTerm();
		match(DOT);
		fields=field_list();
		if ( inputState->guessing==0 ) {
#line 478 "indrilang.g"
			
			if( fields ) {
			t = new indri::lang::ExtentInside( synonyms, fields );
			_nodes.push_back(t);
			} else {
			t = synonyms;
			}
			
#line 643 "QueryParser.cpp"
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
#line 544 "indrilang.g"
	 ExtentOr* contexts ;
#line 661 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 544 "indrilang.g"
	
	contexts = new ExtentOr;
	_nodes.push_back( contexts );
	
#line 669 "QueryParser.cpp"
	
	try {      // for error handling
		match(O_PAREN);
		first = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 550 "indrilang.g"
			
			Field* firstField = new indri::lang::Field( first->getText() );
			_nodes.push_back( firstField );
			contexts->addChild( firstField );
			
#line 682 "QueryParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				additional = LT(1);
				match(TERM);
				if ( inputState->guessing==0 ) {
#line 557 "indrilang.g"
					
					Field* additionalField = new Field(additional->getText());
					_nodes.push_back( additionalField );
					contexts->addChild( additionalField );
					
#line 697 "QueryParser.cpp"
				}
			}
			else {
				goto _loop184;
			}
			
		}
		_loop184:;
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
#line 487 "indrilang.g"
	 RawExtentNode* re ;
#line 724 "QueryParser.cpp"
	
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
		case FILREJ:
		{
			re=filrejNode();
			break;
		}
		case FILREQ:
		{
			re=filreqNode();
			break;
		}
		case O_ANGLE:
		{
			re=synonym_list();
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
#line 269 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 827 "QueryParser.cpp"
#line 269 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 834 "QueryParser.cpp"
	
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
		int _cnt103=0;
		for (;;) {
			if ((LA(1) == NUMBER || LA(1) == FLOAT)) {
				w=floating();
				n=scoredExtentNode();
				if ( inputState->guessing==0 ) {
#line 280 "indrilang.g"
					wn->addChild( w, n );
#line 864 "QueryParser.cpp"
				}
			}
			else {
				if ( _cnt103>=1 ) { goto _loop103; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt103++;
		}
		_loop103:;
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
#line 503 "indrilang.g"
	 indri::lang::ExtentRestriction* er ;
#line 894 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 503 "indrilang.g"
	
	indri::lang::Field* f = 0;
	er = 0;
	
#line 901 "QueryParser.cpp"
	
	try {      // for error handling
		match(O_SQUARE);
		field = LT(1);
		match(TERM);
		match(C_SQUARE);
		if ( inputState->guessing==0 ) {
#line 508 "indrilang.g"
			
			f = new indri::lang::Field(field->getText());
			_nodes.push_back(f);
			er = new indri::lang::ExtentRestriction(sn, f);
			_nodes.push_back(er);
			
#line 916 "QueryParser.cpp"
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
#line 687 "indrilang.g"
	 double d ;
#line 934 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 687 "indrilang.g"
	
	d = 0;
	
#line 941 "QueryParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case FLOAT:
		{
			f = LT(1);
			match(FLOAT);
			if ( inputState->guessing==0 ) {
#line 690 "indrilang.g"
				
				d = atof(f->getText().c_str());
				
#line 954 "QueryParser.cpp"
			}
			break;
		}
		case NUMBER:
		{
			n = LT(1);
			match(NUMBER);
			if ( inputState->guessing==0 ) {
#line 693 "indrilang.g"
				
				d = atof(n->getText().c_str());
				
#line 967 "QueryParser.cpp"
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
#line 285 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 994 "QueryParser.cpp"
#line 285 "indrilang.g"
	
	double w = 0;
	ScoredExtentNode* n = 0;
	sr = wn;
	
#line 1001 "QueryParser.cpp"
	
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
		int _cnt107=0;
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				n=scoredExtentNode();
				if ( inputState->guessing==0 ) {
#line 293 "indrilang.g"
					wn->addChild( 1.0, n );
#line 1030 "QueryParser.cpp"
				}
			}
			else {
				if ( _cnt107>=1 ) { goto _loop107; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt107++;
		}
		_loop107:;
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
#line 297 "indrilang.g"
	 indri::lang::ScoredExtentNode* sr ;
#line 1060 "QueryParser.cpp"
#line 297 "indrilang.g"
	
	ScoredExtentNode* n = 0;
	sr = cn;
	
#line 1066 "QueryParser.cpp"
	
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
		int _cnt111=0;
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				n=scoredExtentNode();
				if ( inputState->guessing==0 ) {
#line 304 "indrilang.g"
					cn->addChild( n );
#line 1095 "QueryParser.cpp"
				}
			}
			else {
				if ( _cnt111>=1 ) { goto _loop111; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt111++;
		}
		_loop111:;
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
#line 322 "indrilang.g"
	 indri::lang::ScoredExtentNode* r ;
#line 1123 "QueryParser.cpp"
#line 322 "indrilang.g"
	
	indri::lang::WSumNode* wn = new indri::lang::WSumNode;
	_nodes.push_back(wn);
	
#line 1129 "QueryParser.cpp"
	
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
#line 395 "indrilang.g"
	 indri::lang::ODNode* od ;
#line 1150 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 395 "indrilang.g"
	
	RawExtentNode* rn = 0;
	od = new indri::lang::ODNode;
	_nodes.push_back(od);
	
#line 1159 "QueryParser.cpp"
	
	try {      // for error handling
		{
		bool synPredMatched125 = false;
		if (((LA(1) == OD))) {
			int _m125 = mark();
			synPredMatched125 = true;
			inputState->guessing++;
			try {
		{
				match(OD);
				match(NUMBER);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched125 = false;
			}
			rewind(_m125);
			inputState->guessing--;
		}
		if ( synPredMatched125 ) {
			{
			match(OD);
			n1 = LT(1);
			match(NUMBER);
			if ( inputState->guessing==0 ) {
#line 404 "indrilang.g"
				od->setWindowSize( n1->getText() );
#line 1188 "QueryParser.cpp"
			}
			}
		}
		else {
			bool synPredMatched128 = false;
			if (((LA(1) == OD))) {
				int _m128 = mark();
				synPredMatched128 = true;
				inputState->guessing++;
				try {
					{
					match(OD);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched128 = false;
				}
				rewind(_m128);
				inputState->guessing--;
			}
			if ( synPredMatched128 ) {
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
#line 1222 "QueryParser.cpp"
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
		int _cnt132=0;
		for (;;) {
			if ((_tokenSet_3.member(LA(1)))) {
				rn=unscoredTerm();
				if ( inputState->guessing==0 ) {
#line 412 "indrilang.g"
					od->addChild( rn );
#line 1240 "QueryParser.cpp"
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
#line 464 "indrilang.g"
	 RawExtentNode* t ;
#line 1268 "QueryParser.cpp"
#line 464 "indrilang.g"
	
	t = 0;
	
#line 1273 "QueryParser.cpp"
	
	try {      // for error handling
		bool synPredMatched146 = false;
		if (((_tokenSet_3.member(LA(1))))) {
			int _m146 = mark();
			synPredMatched146 = true;
			inputState->guessing++;
			try {
				{
				qualifiedTerm();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched146 = false;
			}
			rewind(_m146);
			inputState->guessing--;
		}
		if ( synPredMatched146 ) {
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
#line 416 "indrilang.g"
	 indri::lang::UWNode* uw ;
#line 1318 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 416 "indrilang.g"
	
	uw = new indri::lang::UWNode;
	RawExtentNode* rn = 0;
	_nodes.push_back(uw);
	
#line 1326 "QueryParser.cpp"
	
	try {      // for error handling
		{
		bool synPredMatched136 = false;
		if (((LA(1) == UW))) {
			int _m136 = mark();
			synPredMatched136 = true;
			inputState->guessing++;
			try {
				{
				match(UW);
				match(NUMBER);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched136 = false;
			}
			rewind(_m136);
			inputState->guessing--;
		}
		if ( synPredMatched136 ) {
			{
		match(UW);
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 424 "indrilang.g"
			uw->setWindowSize( n->getText() );
#line 1355 "QueryParser.cpp"
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
		int _cnt140=0;
		for (;;) {
			if ((_tokenSet_3.member(LA(1)))) {
				rn=unscoredTerm();
				if ( inputState->guessing==0 ) {
#line 430 "indrilang.g"
					uw->addChild( rn );
#line 1378 "QueryParser.cpp"
				}
			}
			else {
				if ( _cnt140>=1 ) { goto _loop140; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt140++;
		}
		_loop140:;
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

 indri::lang::FilRejNode*  QueryParser::filrejNode() {
#line 433 "indrilang.g"
	 indri::lang::FilRejNode* fj ;
#line 1406 "QueryParser.cpp"
#line 433 "indrilang.g"
	
	RawExtentNode* filtered = 0;
	RawExtentNode* disallowed = 0;
	
#line 1412 "QueryParser.cpp"
	
	try {      // for error handling
		match(FILREJ);
		match(O_PAREN);
		filtered=unscoredTerm();
		disallowed=unscoredTerm();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 439 "indrilang.g"
			
			fj = new FilRejNode( filtered, disallowed );
			_nodes.push_back(fj);
			
#line 1426 "QueryParser.cpp"
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
	return fj ;
}

 indri::lang::FilReqNode*  QueryParser::filreqNode() {
#line 444 "indrilang.g"
	 indri::lang::FilReqNode* fq ;
#line 1444 "QueryParser.cpp"
#line 444 "indrilang.g"
	
	RawExtentNode* filtered = 0;
	RawExtentNode* required = 0;
	
#line 1450 "QueryParser.cpp"
	
	try {      // for error handling
		match(FILREQ);
		match(O_PAREN);
		filtered=unscoredTerm();
		required=unscoredTerm();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 450 "indrilang.g"
			
			fq = new FilReqNode( filtered, required );
			_nodes.push_back(fq);
			
#line 1464 "QueryParser.cpp"
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
	return fq ;
}

 indri::lang::Field*  QueryParser::anyField() {
#line 455 "indrilang.g"
	 indri::lang::Field* f ;
#line 1482 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 455 "indrilang.g"
	
	f = 0;
	
#line 1488 "QueryParser.cpp"
	
	try {      // for error handling
		match(ANY);
		match(COLON);
		t = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 459 "indrilang.g"
			
			f = new Field(t->getText());
			_nodes.push_back(f);
			
#line 1501 "QueryParser.cpp"
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
#line 524 "indrilang.g"
	 ExtentAnd* fields ;
#line 1519 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  first = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  additional = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 524 "indrilang.g"
	
	fields = new ExtentAnd;
	_nodes.push_back( fields );
	
#line 1527 "QueryParser.cpp"
	
	try {      // for error handling
		first = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 530 "indrilang.g"
			
			Field* firstField = new indri::lang::Field( first->getText() );
			_nodes.push_back( firstField );
			fields->addChild( firstField );
			
#line 1539 "QueryParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				additional = LT(1);
				match(TERM);
				if ( inputState->guessing==0 ) {
#line 537 "indrilang.g"
					
					Field* additionalField = new Field(additional->getText());
					_nodes.push_back( additionalField );
					fields->addChild( additionalField );
					
#line 1554 "QueryParser.cpp"
				}
			}
			else {
				goto _loop181;
			}
			
		}
		_loop181:;
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
#line 573 "indrilang.g"
	 FieldLessNode* extent ;
#line 1580 "QueryParser.cpp"
#line 573 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1587 "QueryParser.cpp"
	
	try {      // for error handling
		match(DATEBEFORE);
		match(O_PAREN);
		d=date();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 578 "indrilang.g"
			
			dateField = new Field("date");
			extent = new FieldLessNode( dateField, d );
			_nodes.push_back( dateField );
			_nodes.push_back( extent );
			
#line 1602 "QueryParser.cpp"
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
#line 585 "indrilang.g"
	 FieldGreaterNode* extent ;
#line 1620 "QueryParser.cpp"
#line 585 "indrilang.g"
	
	UINT64 d = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1627 "QueryParser.cpp"
	
	try {      // for error handling
		match(DATEAFTER);
		match(O_PAREN);
		d=date();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 590 "indrilang.g"
			
			dateField = new Field("date");
			extent = new FieldGreaterNode( dateField, d );
			_nodes.push_back( dateField );
			_nodes.push_back( extent );
			
#line 1642 "QueryParser.cpp"
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
#line 597 "indrilang.g"
	 FieldBetweenNode* extent ;
#line 1660 "QueryParser.cpp"
#line 597 "indrilang.g"
	
	UINT64 low = 0;
	UINT64 high = 0;
	Field* dateField = 0;
	extent = 0;
	
#line 1668 "QueryParser.cpp"
	
	try {      // for error handling
		match(DATEBETWEEN);
		match(O_PAREN);
		low=date();
		match(COMMA);
		high=date();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 603 "indrilang.g"
			
			dateField = new Field("date");
			extent = new FieldBetweenNode( dateField, low, high );
			_nodes.push_back( dateField );
			_nodes.push_back( extent );
			
#line 1685 "QueryParser.cpp"
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
#line 515 "indrilang.g"
	 indri::lang::ExtentOr* s ;
#line 1703 "QueryParser.cpp"
#line 515 "indrilang.g"
	
	indri::lang::RawExtentNode* term = 0;
	s = new indri::lang::ExtentOr;
	_nodes.push_back(s);
	
#line 1710 "QueryParser.cpp"
	
	try {      // for error handling
		match(O_ANGLE);
		{ // ( ... )+
		int _cnt178=0;
		for (;;) {
			if ((_tokenSet_3.member(LA(1)))) {
				term=unscoredTerm();
				if ( inputState->guessing==0 ) {
#line 521 "indrilang.g"
					s->addChild(term);
#line 1722 "QueryParser.cpp"
				}
			}
			else {
				if ( _cnt178>=1 ) { goto _loop178; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt178++;
		}
		_loop178:;
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

 FieldLessNode*  QueryParser::lessNode() {
#line 716 "indrilang.g"
	 FieldLessNode* ln ;
#line 1750 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 716 "indrilang.g"
	
	ln = 0;
	Field* compareField = 0;
	INT64 high = 0;
	
#line 1758 "QueryParser.cpp"
	
	try {      // for error handling
		match(LESS);
		match(O_PAREN);
		field = LT(1);
		match(TERM);
		high=number();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 721 "indrilang.g"
			
			compareField = new Field(field->getText());
			ln = new FieldLessNode( compareField, high );
			_nodes.push_back( compareField );
			_nodes.push_back( ln );
			
#line 1775 "QueryParser.cpp"
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
#line 704 "indrilang.g"
	 FieldGreaterNode* gn ;
#line 1793 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 704 "indrilang.g"
	
	gn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	
#line 1801 "QueryParser.cpp"
	
	try {      // for error handling
		match(GREATER);
		match(O_PAREN);
		field = LT(1);
		match(TERM);
		low=number();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 709 "indrilang.g"
			
			compareField = new Field(field->getText());
			gn = new FieldGreaterNode( compareField, low );
			_nodes.push_back( compareField );
			_nodes.push_back( gn );
			
#line 1818 "QueryParser.cpp"
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
#line 728 "indrilang.g"
	 FieldBetweenNode* bn ;
#line 1836 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 728 "indrilang.g"
	
	bn = 0;
	Field* compareField = 0;
	INT64 low = 0;
	INT64 high = 0;
	
#line 1845 "QueryParser.cpp"
	
	try {      // for error handling
		match(BETWEEN);
		match(O_PAREN);
		field = LT(1);
		match(TERM);
		low=number();
		high=number();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 734 "indrilang.g"
			
			compareField = new Field(field->getText());
			bn = new FieldBetweenNode( compareField, low, high );
			_nodes.push_back( compareField );
			_nodes.push_back( bn );
			
#line 1863 "QueryParser.cpp"
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
#line 741 "indrilang.g"
	 FieldEqualsNode* en ;
#line 1881 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 741 "indrilang.g"
	
	en = 0;
	Field* compareField = 0;
	INT64 eq = 0;
	
#line 1889 "QueryParser.cpp"
	
	try {      // for error handling
		match(EQUALS);
		match(O_PAREN);
		field = LT(1);
		match(TERM);
		eq=number();
		match(C_PAREN);
		if ( inputState->guessing==0 ) {
#line 746 "indrilang.g"
			
			compareField = new Field(field->getText());
			en = new FieldEqualsNode( compareField, eq );
			_nodes.push_back( compareField );
			_nodes.push_back( en );
			
#line 1906 "QueryParser.cpp"
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
#line 652 "indrilang.g"
	 IndexTerm* t ;
#line 1924 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  et = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  qet = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 652 "indrilang.g"
	
	t = 0;
	
#line 1934 "QueryParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case TERM:
		{
			id = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 655 "indrilang.g"
				
				t = new indri::lang::IndexTerm(id->getText());
				_nodes.push_back(t);
				
#line 1948 "QueryParser.cpp"
			}
			break;
		}
		case NUMBER:
		{
			n = LT(1);
			match(NUMBER);
			if ( inputState->guessing==0 ) {
#line 659 "indrilang.g"
				
				t = new indri::lang::IndexTerm(n->getText());
				_nodes.push_back(t);
				
#line 1962 "QueryParser.cpp"
			}
			break;
		}
		case FLOAT:
		{
			f = LT(1);
			match(FLOAT);
			if ( inputState->guessing==0 ) {
#line 663 "indrilang.g"
				
				t = new indri::lang::IndexTerm(f->getText());
				_nodes.push_back(t);
				
#line 1976 "QueryParser.cpp"
			}
			break;
		}
		case DBL_QUOTE:
		{
			match(DBL_QUOTE);
			t=rawText();
			match(DBL_QUOTE);
			if ( inputState->guessing==0 ) {
#line 667 "indrilang.g"
				
				// if a text term appears in quotes, consider it stemmed
				t->setStemmed(true);
				
#line 1991 "QueryParser.cpp"
			}
			break;
		}
		case ENCODED_TERM:
		{
			et = LT(1);
			match(ENCODED_TERM);
			if ( inputState->guessing==0 ) {
#line 671 "indrilang.g"
				
				std::string decodedString; 
				base64_decode_string(decodedString, et->getText());
				t = new indri::lang::IndexTerm( decodedString );
				_nodes.push_back(t);
				
#line 2007 "QueryParser.cpp"
			}
			break;
		}
		case ENCODED_QUOTED_TERM:
		{
			qet = LT(1);
			match(ENCODED_QUOTED_TERM);
			if ( inputState->guessing==0 ) {
#line 677 "indrilang.g"
				
				std::string decodedString; 
				base64_decode_string(decodedString, et->getText());
				t = new indri::lang::IndexTerm( decodedString );
				t->setStemmed(true);
				_nodes.push_back(t);
				
#line 2024 "QueryParser.cpp"
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
#line 565 "indrilang.g"
	 Field* extent ;
#line 2049 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(O_SQUARE);
		fieldName = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 567 "indrilang.g"
			
			extent = new Field( fieldName->getText() );
			_nodes.push_back( extent );
			
#line 2062 "QueryParser.cpp"
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
#line 625 "indrilang.g"
	 UINT64 d ;
#line 2081 "QueryParser.cpp"
	
	try {      // for error handling
		bool synPredMatched191 = false;
		if (((LA(1) == NUMBER))) {
			int _m191 = mark();
			synPredMatched191 = true;
			inputState->guessing++;
			try {
				{
				match(NUMBER);
				match(DASH);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched191 = false;
			}
			rewind(_m191);
			inputState->guessing--;
		}
		if ( synPredMatched191 ) {
			d=dashDate();
		}
		else {
			bool synPredMatched193 = false;
			if (((LA(1) == NUMBER))) {
				int _m193 = mark();
				synPredMatched193 = true;
				inputState->guessing++;
				try {
					{
					match(NUMBER);
					match(SLASH);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched193 = false;
				}
				rewind(_m193);
				inputState->guessing--;
			}
			if ( synPredMatched193 ) {
				d=slashDate();
			}
			else if ((LA(1) == NUMBER)) {
				d=spaceDate();
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

 UINT64  QueryParser::dashDate() {
#line 630 "indrilang.g"
	 UINT64 d ;
#line 2148 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 630 "indrilang.g"
	
	d = 0;
	
#line 2156 "QueryParser.cpp"
	
	try {      // for error handling
		day = LT(1);
		match(NUMBER);
		match(DASH);
		month = LT(1);
		match(TERM);
		match(DASH);
		year = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 633 "indrilang.g"
			
			d = DateParse::convertDate( year->getText(), month->getText(), day->getText() ); 
			
#line 2172 "QueryParser.cpp"
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
#line 637 "indrilang.g"
	 UINT64 d ;
#line 2190 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 637 "indrilang.g"
	
	d = 0;
	
#line 2198 "QueryParser.cpp"
	
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
#line 640 "indrilang.g"
			
			d = DateParse::convertDate( year->getText(), month->getText(), day->getText() ); 
			
#line 2214 "QueryParser.cpp"
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
#line 644 "indrilang.g"
	 UINT64 d ;
#line 2232 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  day = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  month = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  year = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 644 "indrilang.g"
	
	d = 0;
	
#line 2240 "QueryParser.cpp"
	
	try {      // for error handling
		day = LT(1);
		match(NUMBER);
		month = LT(1);
		match(TERM);
		year = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 647 "indrilang.g"
			
			d = DateParse::convertDate( year->getText(), month->getText(), day->getText() );
			
#line 2254 "QueryParser.cpp"
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
#line 697 "indrilang.g"
	 INT64 v ;
#line 2272 "QueryParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 697 "indrilang.g"
	
	v = 0;
	
#line 2278 "QueryParser.cpp"
	
	try {      // for error handling
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 700 "indrilang.g"
			
			v = string_to_i64(n->getText());
			
#line 2288 "QueryParser.cpp"
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
	"\"#prior\"",
	"\"#date:after\"",
	"\"#date:before\"",
	"\"#date:between\"",
	"\"#less\"",
	"\"#greater\"",
	"\"#between\"",
	"\"#equals\"",
	"NUMBER",
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
	"ASCII_LETTER",
	"ASCII_LETTER_NO_B",
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

const unsigned long QueryParser::_tokenSet_0_data_[] = { 1207959520UL, 3932164UL, 0UL, 0UL };
// "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#prior" "#date:after" "#date:before" "#date:between" 
// "#less" "#greater" "#between" "#equals" NUMBER FLOAT O_ANGLE DBL_QUOTE 
// TERM ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long QueryParser::_tokenSet_1_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long QueryParser::_tokenSet_2_data_[] = { 1744830434UL, 3932164UL, 0UL, 0UL };
// EOF "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#prior" "#date:after" "#date:before" "#date:between" 
// "#less" "#greater" "#between" "#equals" NUMBER FLOAT C_PAREN O_ANGLE 
// DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long QueryParser::_tokenSet_3_data_[] = { 1207813248UL, 3932164UL, 0UL, 0UL };
// "#od" "#uw" "#filreq" "#filrej" "#any" "#date:after" "#date:before" 
// "#date:between" "#less" "#greater" "#between" "#equals" NUMBER FLOAT 
// O_ANGLE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long QueryParser::_tokenSet_4_data_[] = { 4160749538UL, 3932164UL, 0UL, 0UL };
// EOF "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#prior" "#date:after" "#date:before" "#date:between" 
// "#less" "#greater" "#between" "#equals" NUMBER FLOAT O_PAREN C_PAREN 
// O_ANGLE C_ANGLE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long QueryParser::_tokenSet_5_data_[] = { 3892314082UL, 3932180UL, 0UL, 0UL };
// EOF "#wsum" "#wand" "#od" "#or" "#not" "#uw" "#combine" "#weight" "#max" 
// "#filreq" "#filrej" "#any" "#prior" "#date:after" "#date:before" "#date:between" 
// "#less" "#greater" "#between" "#equals" NUMBER FLOAT C_PAREN O_ANGLE 
// C_ANGLE DBL_QUOTE DOT TERM ENCODED_QUOTED_TERM ENCODED_TERM OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long QueryParser::_tokenSet_6_data_[] = { 268435456UL, 0UL, 0UL, 0UL };
// O_PAREN 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_6(_tokenSet_6_data_,4);
const unsigned long QueryParser::_tokenSet_7_data_[] = { 3892167808UL, 3932164UL, 0UL, 0UL };
// "#od" "#uw" "#filreq" "#filrej" "#any" "#date:after" "#date:before" 
// "#date:between" "#less" "#greater" "#between" "#equals" NUMBER FLOAT 
// C_PAREN O_ANGLE C_ANGLE DBL_QUOTE TERM ENCODED_QUOTED_TERM ENCODED_TERM 
// OPERATOR 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_7(_tokenSet_7_data_,4);
const unsigned long QueryParser::_tokenSet_8_data_[] = { 536870912UL, 32UL, 0UL, 0UL };
// C_PAREN COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_8(_tokenSet_8_data_,4);
const unsigned long QueryParser::_tokenSet_9_data_[] = { 570425344UL, 0UL, 0UL, 0UL };
// NUMBER C_PAREN 
const ANTLR_USE_NAMESPACE(antlr)BitSet QueryParser::_tokenSet_9(_tokenSet_9_data_,4);


ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
