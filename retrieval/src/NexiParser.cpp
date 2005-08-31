/* $ANTLR 2.7.4: "nexilang.g" -> "NexiParser.cpp"$ */
#include "indri/NexiParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
ANTLR_BEGIN_NAMESPACE(indri)
ANTLR_BEGIN_NAMESPACE(lang)
#line 1 "nexilang.g"
#line 8 "NexiParser.cpp"
NexiParser::NexiParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

NexiParser::NexiParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,1)
{
}

NexiParser::NexiParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

NexiParser::NexiParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,1)
{
}

NexiParser::NexiParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,1)
{
}

 indri::lang::ScoredExtentNode*  NexiParser::query() {
#line 139 "nexilang.g"
	 indri::lang::ScoredExtentNode* q ;
#line 37 "NexiParser.cpp"
#line 139 "nexilang.g"
	
	indri::lang::ScoredExtentNode * c = 0;
	indri::lang::NestedExtentInside * p = 0;
	indri::lang::RawExtentNode * f = 0;
	indri::lang::ScoredExtentNode * c2 = 0;
	indri::lang::NestedExtentInside * p2 = 0;
	indri::lang::RawExtentNode * f2 = 0;
	
#line 47 "NexiParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case SLASH:
		{
			{
			p=path();
			f=field();
			match(O_SQUARE);
			c=clause();
			match(C_SQUARE);
			if ( inputState->guessing==0 ) {
#line 147 "nexilang.g"
				
				
				indri::lang::ExtentRestriction * r = 0;
				
				// finish the path with a field as the inner for the last extent inside
				if (p != 0) {
				indri::lang::NestedExtentInside * pt = p;
				while (pt->getInner() != NULL) {
				pt = (indri::lang::NestedExtentInside *) pt->getInner();
				}
				pt->setInner(f);
				
				r = new indri::lang::ExtentRestriction(c, p);
				} else {
				r = new indri::lang::ExtentRestriction(c, f);
				}
				_nodes.push_back(r);
				q=r;
				
#line 80 "NexiParser.cpp"
			}
			{
			switch ( LA(1)) {
			case SLASH:
			{
				p2=path();
				f2=field();
				match(O_SQUARE);
				c2=clause();
				match(C_SQUARE);
				if ( inputState->guessing==0 ) {
#line 166 "nexilang.g"
					
					// pop the extent restriction, we'll need an ExtentEnforcement instead
					_nodes.pop_back();
					
					// finish the path with a field as the inner for the last extent inside
					indri::lang::ExtentRestriction * r2;
					if (p2 != 0) {
					indri::lang::NestedExtentInside * pt2 = p2;
					while (pt2->getInner() != NULL) {
					pt2 = (indri::lang::NestedExtentInside *) pt2->getInner();
					}
					pt2->setInner(f2);
					
					r2 = new indri::lang::ExtentRestriction(c2, p2);
					} else {
					r2 = new indri::lang::ExtentRestriction(c2, f2);
					}
					_nodes.push_back(r2);
					
					indri::lang::ContextInclusionNode * combine = new indri::lang::ContextInclusionNode;
					_nodes.push_back(combine);
					
					combine->addChild(c);
					combine->addChild(r2, true);
					
					indri::lang::ExtentEnforcement * enf = new indri::lang::ExtentEnforcement(combine, f);
					_nodes.push_back(enf);
					q=enf;
					
					
#line 123 "NexiParser.cpp"
				}
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
			if ( inputState->guessing==0 ) {
#line 196 "nexilang.g"
				
				
				indri::lang::LengthPrior * prior = new indri::lang::LengthPrior(q, 0);
				_nodes.push_back(prior);
				q=prior;
				
				
#line 147 "NexiParser.cpp"
			}
			}
			break;
		}
		case NUMBER:
		case FLOAT:
		case DBL_QUOTE:
		case MINUS:
		case PLUS:
		case TERM:
		{
			{
			c=termList();
			match(ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE);
			if ( inputState->guessing==0 ) {
#line 203 "nexilang.g"
				
				indri::lang::FieldWildcard * wild = new indri::lang::FieldWildcard;
				_nodes.push_back(wild);
				indri::lang::ExtentRestriction * r = new indri::lang::ExtentRestriction(c, wild);
				_nodes.push_back(r);
				
				
				indri::lang::LengthPrior * prior = new indri::lang::LengthPrior(r, 0);
				_nodes.push_back(prior);
				q=prior;
				
				
#line 176 "NexiParser.cpp"
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
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_0);
		} else {
			throw;
		}
	}
	return q ;
}

 indri::lang::NestedExtentInside*  NexiParser::path() {
#line 241 "nexilang.g"
	 indri::lang::NestedExtentInside* e ;
#line 202 "NexiParser.cpp"
#line 241 "nexilang.g"
	
	indri::lang::RawExtentNode * f = 0;
	indri::lang::RawExtentNode * f2 = 0;
	indri::lang::NestedExtentInside * c;
	e = 0;
	
#line 210 "NexiParser.cpp"
	
	try {      // for error handling
		match(SLASH);
		match(SLASH);
		{
		bool synPredMatched61 = false;
		if (((LA(1) == WILD || LA(1) == O_PAREN || LA(1) == TERM))) {
			int _m61 = mark();
			synPredMatched61 = true;
			inputState->guessing++;
			try {
				{
				field();
				match(SLASH);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched61 = false;
			}
			rewind(_m61);
			inputState->guessing--;
		}
		if ( synPredMatched61 ) {
			f=field();
			match(SLASH);
			match(SLASH);
			if ( inputState->guessing==0 ) {
#line 248 "nexilang.g"
				
				c = new indri::lang::NestedExtentInside(NULL, f);
				_nodes.push_back(c);
				e = c;
				
#line 244 "NexiParser.cpp"
			}
			{ // ( ... )*
			for (;;) {
				bool synPredMatched64 = false;
				if (((LA(1) == WILD || LA(1) == O_PAREN || LA(1) == TERM))) {
					int _m64 = mark();
					synPredMatched64 = true;
					inputState->guessing++;
					try {
						{
						field();
						match(SLASH);
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched64 = false;
					}
					rewind(_m64);
					inputState->guessing--;
				}
				if ( synPredMatched64 ) {
					f2=field();
					match(SLASH);
					match(SLASH);
					if ( inputState->guessing==0 ) {
#line 256 "nexilang.g"
						
						indri::lang::NestedExtentInside * ct = new indri::lang::NestedExtentInside(NULL, f2);
						_nodes.push_back(ct);
						c->setInner(ct);
						c = ct;
						
#line 277 "NexiParser.cpp"
					}
				}
				else {
					goto _loop65;
				}
				
			}
			_loop65:;
			} // ( ... )*
		}
		else if ((LA(1) == WILD || LA(1) == O_PAREN || LA(1) == TERM)) {
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
			consumeUntil(_tokenSet_1);
		} else {
			throw;
		}
	}
	return e ;
}

 indri::lang::RawExtentNode *  NexiParser::field() {
#line 268 "nexilang.g"
	 indri::lang::RawExtentNode * e ;
#line 311 "NexiParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName3 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 268 "nexilang.g"
	
	indri::lang::Field * f = 0;
	indri::lang::ExtentOr * eo = 0;
	
#line 320 "NexiParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case WILD:
		{
			match(WILD);
			if ( inputState->guessing==0 ) {
#line 273 "nexilang.g"
				
				e = new indri::lang::FieldWildcard;
				_nodes.push_back(e);
				
#line 333 "NexiParser.cpp"
			}
			break;
		}
		case TERM:
		{
			fieldName = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 278 "nexilang.g"
				
				f = new indri::lang::Field(fieldName->getText());
				_nodes.push_back(f);
				e = f;
				
#line 348 "NexiParser.cpp"
			}
			break;
		}
		case O_PAREN:
		{
			{
			match(O_PAREN);
			fieldName2 = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 283 "nexilang.g"
				
				f = new indri::lang::Field(fieldName2->getText());
				_nodes.push_back(f);
				eo = new indri::lang::ExtentOr;
				_nodes.push_back(eo);
				eo->addChild(f);
				e = eo;
				
#line 368 "NexiParser.cpp"
			}
			{ // ( ... )+
			int _cnt69=0;
			for (;;) {
				if ((LA(1) == 41)) {
					match(41);
					fieldName3 = LT(1);
					match(TERM);
					if ( inputState->guessing==0 ) {
#line 291 "nexilang.g"
						
						f = new indri::lang::Field(fieldName3->getText());
						_nodes.push_back(f);
						eo->addChild(f);
						
#line 384 "NexiParser.cpp"
					}
				}
				else {
					if ( _cnt69>=1 ) { goto _loop69; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt69++;
			}
			_loop69:;
			}  // ( ... )+
			match(C_PAREN);
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
			consumeUntil(_tokenSet_2);
		} else {
			throw;
		}
	}
	return e ;
}

 indri::lang::ScoredExtentNode*  NexiParser::clause() {
#line 301 "nexilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 420 "NexiParser.cpp"
#line 301 "nexilang.g"
	
	indri::lang::ScoredExtentNode * c = 0;
	indri::lang::UnweightedCombinationNode * l = 0;
	
#line 426 "NexiParser.cpp"
	
	try {      // for error handling
		s=filter();
		{
		switch ( LA(1)) {
		case AND:
		case OR:
		{
			l=logical();
			c=filter();
			if ( inputState->guessing==0 ) {
#line 306 "nexilang.g"
				
				l->addChild(s);
				l->addChild(c);
				s = l;
				
#line 444 "NexiParser.cpp"
			}
			break;
		}
		case C_PAREN:
		case C_SQUARE:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_3);
		} else {
			throw;
		}
	}
	return s ;
}

 indri::lang::ScoredExtentNode*  NexiParser::termList() {
#line 217 "nexilang.g"
	 indri::lang::ScoredExtentNode* q ;
#line 475 "NexiParser.cpp"
#line 217 "nexilang.g"
	
	indri::lang::CombineNode* c = 0; 
	indri::lang::ScoredExtentNode* s = 0;
	
	
#line 482 "NexiParser.cpp"
	
	try {      // for error handling
		q=term();
		{
		switch ( LA(1)) {
		case NUMBER:
		case FLOAT:
		case DBL_QUOTE:
		case MINUS:
		case PLUS:
		case TERM:
		{
			s=term();
			if ( inputState->guessing==0 ) {
#line 224 "nexilang.g"
				
				c = new indri::lang::CombineNode;
				c->addChild(q);
				c->addChild(s);
				_nodes.push_back(c);
				q = c;
				
#line 505 "NexiParser.cpp"
			}
			{ // ( ... )*
			for (;;) {
				if ((_tokenSet_4.member(LA(1)))) {
					s=term();
					if ( inputState->guessing==0 ) {
#line 232 "nexilang.g"
						
						c->addChild(s);
						
#line 516 "NexiParser.cpp"
					}
				}
				else {
					goto _loop57;
				}
				
			}
			_loop57:;
			} // ( ... )*
			break;
		}
		case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
		case C_PAREN:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
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
	return q ;
}

 indri::lang::ScoredExtentNode*  NexiParser::term() {
#line 423 "nexilang.g"
	 indri::lang::ScoredExtentNode* t ;
#line 555 "NexiParser.cpp"
#line 423 "nexilang.g"
	
	indri::lang::NotNode * n = 0;
	
#line 560 "NexiParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case NUMBER:
		case FLOAT:
		case DBL_QUOTE:
		case TERM:
		{
			t=unrestrictedTerm();
			break;
		}
		case PLUS:
		{
			match(PLUS);
			t=unrestrictedTerm();
			break;
		}
		case MINUS:
		{
			match(MINUS);
			t=unrestrictedTerm();
			if ( inputState->guessing==0 ) {
#line 428 "nexilang.g"
				
				n = new indri::lang::NotNode;
				_nodes.push_back(n);
				n->setChild(t);
				t = n;
				
#line 590 "NexiParser.cpp"
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
			consumeUntil(_tokenSet_6);
		} else {
			throw;
		}
	}
	return t ;
}

 indri::lang::ScoredExtentNode*  NexiParser::filter() {
#line 324 "nexilang.g"
	 indri::lang::ScoredExtentNode* s;
#line 615 "NexiParser.cpp"
#line 324 "nexilang.g"
	
	indri::lang::RawExtentNode * a = 0;
	indri::lang::RawExtentNode * contexts = 0;  
	
#line 621 "NexiParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ABOUT:
		{
			s=aboutClause();
			break;
		}
		case 42:
		{
			a=arithmeticClause();
			if ( inputState->guessing==0 ) {
#line 329 "nexilang.g"
				
				
				s = new indri::lang::NestedRawScorerNode( a, contexts );
				_nodes.push_back( s );
				
				indri::lang::MaxNode * m = new indri::lang::MaxNode;
				_nodes.push_back(m);
				m->addChild(s);
				s = m;        
				
				
#line 646 "NexiParser.cpp"
			}
			break;
		}
		case O_PAREN:
		{
			s=filterParens();
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
			consumeUntil(_tokenSet_7);
		} else {
			throw;
		}
	}
	return s;
}

 indri::lang::UnweightedCombinationNode*  NexiParser::logical() {
#line 313 "nexilang.g"
	 indri::lang::UnweightedCombinationNode* s;
#line 676 "NexiParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case AND:
		{
			match(AND);
			if ( inputState->guessing==0 ) {
#line 314 "nexilang.g"
				
				s = new indri::lang::CombineNode;
				_nodes.push_back(s);
				
#line 689 "NexiParser.cpp"
			}
			break;
		}
		case OR:
		{
			match(OR);
			if ( inputState->guessing==0 ) {
#line 318 "nexilang.g"
				
				s = new indri::lang::OrNode;
				_nodes.push_back(s);
				
#line 702 "NexiParser.cpp"
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
			consumeUntil(_tokenSet_8);
		} else {
			throw;
		}
	}
	return s;
}

 indri::lang::ScoredExtentNode*  NexiParser::aboutClause() {
#line 348 "nexilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 727 "NexiParser.cpp"
#line 348 "nexilang.g"
	
	indri::lang::NestedExtentInside * p = 0;
	indri::lang::ScoredExtentNode * t = 0;
	indri::lang::RawExtentNode * f = 0;
	indri::lang::ExtentRestriction * r = 0;
	
#line 735 "NexiParser.cpp"
	
	try {      // for error handling
		match(ABOUT);
		match(O_PAREN);
		match(42);
		{
		switch ( LA(1)) {
		case SLASH:
		{
			{
			p=path();
			f=field();
			match(43);
			t=termList();
			}
			if ( inputState->guessing==0 ) {
#line 354 "nexilang.g"
				
				if (p != 0) { 
				indri::lang::NestedExtentInside * pt = p;
				while (pt->getInner() != NULL) {
				pt = (indri::lang::NestedExtentInside *) pt->getInner();    
				}    
				pt->setInner(f);                    
				r = new indri::lang::ExtentRestriction(t, p);
				} else {
				r = new indri::lang::ExtentRestriction(t, f);
				}
				_nodes.push_back(r);
				indri::lang::MaxNode * m = new indri::lang::MaxNode;
				_nodes.push_back(m);
				m->addChild(r);
				s = m;   
				
#line 770 "NexiParser.cpp"
			}
			break;
		}
		case 43:
		{
			{
			match(43);
			s=termList();
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(C_PAREN);
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
	return s ;
}

 indri::lang::RawExtentNode *  NexiParser::arithmeticClause() {
#line 376 "nexilang.g"
	 indri::lang::RawExtentNode * s;
#line 805 "NexiParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 376 "nexilang.g"
	
	indri::lang::Field* f = 0;
	indri::lang::RawExtentNode* c = 0;
	indri::lang::NestedExtentInside* p = 0;
	INT64 n = 0;
	s = 0;
	
#line 815 "NexiParser.cpp"
	
	try {      // for error handling
		match(42);
		p=path();
		field = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 385 "nexilang.g"
			
			f = new indri::lang::Field(field->getText());
			_nodes.push_back(f);
			
#line 828 "NexiParser.cpp"
		}
		{
		switch ( LA(1)) {
		case LESS:
		{
			{
			match(LESS);
			n=number();
			if ( inputState->guessing==0 ) {
#line 389 "nexilang.g"
				
				c = new indri::lang::FieldLessNode(f, n);
				_nodes.push_back(c);
				
#line 843 "NexiParser.cpp"
			}
			}
			break;
		}
		case LESSEQ:
		{
			{
			match(LESSEQ);
			n=number();
			if ( inputState->guessing==0 ) {
#line 393 "nexilang.g"
				
				c = new indri::lang::FieldLessNode(f, n + 1);
				_nodes.push_back(c);
				
#line 859 "NexiParser.cpp"
			}
			}
			break;
		}
		case EQUALS:
		{
			{
			match(EQUALS);
			n=number();
			if ( inputState->guessing==0 ) {
#line 397 "nexilang.g"
				
				c = new indri::lang::FieldEqualsNode(f, n);
				_nodes.push_back(c);
				
#line 875 "NexiParser.cpp"
			}
			}
			break;
		}
		case GREATER:
		{
			{
			match(GREATER);
			n=number();
			if ( inputState->guessing==0 ) {
#line 401 "nexilang.g"
				
				c = new indri::lang::FieldGreaterNode(f, n);
				_nodes.push_back(c);
				
#line 891 "NexiParser.cpp"
			}
			}
			break;
		}
		case GREATEREQ:
		{
			{
			match(GREATEREQ);
			n=number();
			if ( inputState->guessing==0 ) {
#line 405 "nexilang.g"
				
				c = new indri::lang::FieldGreaterNode(f, n - 1);
				_nodes.push_back(c);
				
#line 907 "NexiParser.cpp"
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
		if ( inputState->guessing==0 ) {
#line 408 "nexilang.g"
			
			if (p != 0) {
			indri::lang::NestedExtentInside * pt = p;
			while (pt->getInner() != NULL) {
			pt = (indri::lang::NestedExtentInside *) pt->getInner();    
			}    
			pt->setInner(c);
			s = p;
			} else {
			s = c;
			}
			
#line 932 "NexiParser.cpp"
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
	return s;
}

 indri::lang::ScoredExtentNode*  NexiParser::filterParens() {
#line 343 "nexilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 950 "NexiParser.cpp"
	
	try {      // for error handling
		match(O_PAREN);
		s=clause();
		match(C_PAREN);
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
	return s ;
}

 INT64  NexiParser::number() {
#line 491 "nexilang.g"
	 INT64 v ;
#line 972 "NexiParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 491 "nexilang.g"
	
	v = 0;
	
#line 978 "NexiParser.cpp"
	
	try {      // for error handling
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 494 "nexilang.g"
			
			v = string_to_i64(n->getText());
			
#line 988 "NexiParser.cpp"
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
	return v ;
}

 indri::lang::ScoredExtentNode *  NexiParser::unrestrictedTerm() {
#line 436 "nexilang.g"
	 indri::lang::ScoredExtentNode * t ;
#line 1006 "NexiParser.cpp"
#line 436 "nexilang.g"
	
	indri::lang::RawExtentNode * raw = 0;
	indri::lang::RawExtentNode * contexts = 0;
	t = 0;
	
#line 1013 "NexiParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case NUMBER:
		case FLOAT:
		case TERM:
		{
			raw=rawText();
			if ( inputState->guessing==0 ) {
#line 442 "nexilang.g"
				
				
				t = new indri::lang::NestedRawScorerNode( raw, contexts );
				_nodes.push_back( t );
				
#line 1029 "NexiParser.cpp"
			}
			break;
		}
		case DBL_QUOTE:
		{
			match(DBL_QUOTE);
			raw=odNode();
			match(DBL_QUOTE);
			if ( inputState->guessing==0 ) {
#line 447 "nexilang.g"
				
				
				t = new indri::lang::NestedRawScorerNode( raw, contexts );
				_nodes.push_back( t );
				
#line 1045 "NexiParser.cpp"
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
			consumeUntil(_tokenSet_6);
		} else {
			throw;
		}
	}
	return t ;
}

 indri::lang::RawExtentNode*  NexiParser::rawText() {
#line 457 "nexilang.g"
	 indri::lang::RawExtentNode* t ;
#line 1070 "NexiParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 457 "nexilang.g"
	
	t = 0;
	
#line 1078 "NexiParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case TERM:
		{
			id = LT(1);
			match(TERM);
			if ( inputState->guessing==0 ) {
#line 460 "nexilang.g"
				
				std::string text = id->getText();
				
				//     std::string::iterator c = text.begin();
				//     while (c != text.end()) { *c = tolower(*c); c++; }
				
				t = new indri::lang::IndexTerm(id->getText());
				_nodes.push_back(t);
				
#line 1097 "NexiParser.cpp"
			}
			break;
		}
		case NUMBER:
		{
			n = LT(1);
			match(NUMBER);
			if ( inputState->guessing==0 ) {
#line 469 "nexilang.g"
				
				t = new indri::lang::IndexTerm(n->getText());
				_nodes.push_back(t);
				
#line 1111 "NexiParser.cpp"
			}
			break;
		}
		case FLOAT:
		{
			f = LT(1);
			match(FLOAT);
			if ( inputState->guessing==0 ) {
#line 473 "nexilang.g"
				
				t = new indri::lang::IndexTerm(f->getText());
				_nodes.push_back(t);
				
#line 1125 "NexiParser.cpp"
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
			consumeUntil(_tokenSet_6);
		} else {
			throw;
		}
	}
	return t ;
}

 indri::lang::ODNode*  NexiParser::odNode() {
#line 479 "nexilang.g"
	 indri::lang::ODNode* od ;
#line 1150 "NexiParser.cpp"
#line 479 "nexilang.g"
	
	RawExtentNode* t = 0;
	od = new indri::lang::ODNode;
	_nodes.push_back(od);
	
#line 1157 "NexiParser.cpp"
	
	try {      // for error handling
		{ // ( ... )+
		int _cnt91=0;
		for (;;) {
			if ((LA(1) == NUMBER || LA(1) == FLOAT || LA(1) == TERM)) {
				t=rawText();
				if ( inputState->guessing==0 ) {
#line 486 "nexilang.g"
					
					od->addChild(t);
					
#line 1170 "NexiParser.cpp"
				}
			}
			else {
				if ( _cnt91>=1 ) { goto _loop91; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt91++;
		}
		_loop91:;
		}  // ( ... )+
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
	return od ;
}

void NexiParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* NexiParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"\"<\"",
	"\">\"",
	"\"<=\"",
	"\">=\"",
	"\"=\"",
	"\"about\"",
	"\"AND\"",
	"\"OR\"",
	"\"*\"",
	"NUMBER",
	"NEGATIVE_NUMBER",
	"FLOAT",
	"STAR",
	"O_PAREN",
	"C_PAREN",
	"O_SQUARE",
	"C_SQUARE",
	"DBL_QUOTE",
	"QUOTE",
	"DOT",
	"COMMA",
	"SLASH",
	"MINUS",
	"PLUS",
	"TAB",
	"CR",
	"LF",
	"SPACE",
	"HIGH_CHAR",
	"DIGIT",
	"ASCII_LETTER",
	"SAFE_LETTER",
	"SAFE_CHAR",
	"TEXT_TERM",
	"TERM",
	"OPERATORS",
	"JUNK",
	"\"|\"",
	"\".\"",
	"\",\"",
	0
};

const unsigned long NexiParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet NexiParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long NexiParser::_tokenSet_1_data_[] = { 135168UL, 64UL, 0UL, 0UL };
// "*" O_PAREN TERM 
const ANTLR_USE_NAMESPACE(antlr)BitSet NexiParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long NexiParser::_tokenSet_2_data_[] = { 34078720UL, 2048UL, 0UL, 0UL };
// O_SQUARE SLASH "," 
const ANTLR_USE_NAMESPACE(antlr)BitSet NexiParser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long NexiParser::_tokenSet_3_data_[] = { 1310720UL, 0UL, 0UL, 0UL };
// C_PAREN C_SQUARE 
const ANTLR_USE_NAMESPACE(antlr)BitSet NexiParser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long NexiParser::_tokenSet_4_data_[] = { 203464704UL, 64UL, 0UL, 0UL };
// NUMBER FLOAT DBL_QUOTE MINUS PLUS TERM 
const ANTLR_USE_NAMESPACE(antlr)BitSet NexiParser::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long NexiParser::_tokenSet_5_data_[] = { 262146UL, 0UL, 0UL, 0UL };
// EOF C_PAREN 
const ANTLR_USE_NAMESPACE(antlr)BitSet NexiParser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long NexiParser::_tokenSet_6_data_[] = { 203726850UL, 64UL, 0UL, 0UL };
// EOF NUMBER FLOAT C_PAREN DBL_QUOTE MINUS PLUS TERM 
const ANTLR_USE_NAMESPACE(antlr)BitSet NexiParser::_tokenSet_6(_tokenSet_6_data_,4);
const unsigned long NexiParser::_tokenSet_7_data_[] = { 1313792UL, 0UL, 0UL, 0UL };
// "AND" "OR" C_PAREN C_SQUARE 
const ANTLR_USE_NAMESPACE(antlr)BitSet NexiParser::_tokenSet_7(_tokenSet_7_data_,4);
const unsigned long NexiParser::_tokenSet_8_data_[] = { 131584UL, 1024UL, 0UL, 0UL };
// "about" O_PAREN "." 
const ANTLR_USE_NAMESPACE(antlr)BitSet NexiParser::_tokenSet_8(_tokenSet_8_data_,4);
const unsigned long NexiParser::_tokenSet_9_data_[] = { 2097152UL, 0UL, 0UL, 0UL };
// DBL_QUOTE 
const ANTLR_USE_NAMESPACE(antlr)BitSet NexiParser::_tokenSet_9(_tokenSet_9_data_,4);


ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
