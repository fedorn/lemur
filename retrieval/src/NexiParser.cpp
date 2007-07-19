/* $ANTLR 2.7.3 (20060307-1): "nexilang.g" -> "NexiParser.cpp"$ */
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
#line 151 "nexilang.g"
	 indri::lang::ScoredExtentNode* q ;
#line 37 "NexiParser.cpp"
#line 151 "nexilang.g"
	
	indri::lang::ScoredExtentNode * c = 0;
	indri::lang::NestedExtentInside * p = 0;
	indri::lang::RawExtentNode * f = 0;
	indri::lang::ScoredExtentNode * c2 = 0;
	indri::lang::NestedExtentInside * p2 = 0;
	indri::lang::RawExtentNode * f2 = 0;
	
#line 47 "NexiParser.cpp"
	
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
#line 159 "nexilang.g"
			
			
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
			
#line 79 "NexiParser.cpp"
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
#line 178 "nexilang.g"
				
				
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
				
				
#line 120 "NexiParser.cpp"
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
#line 206 "nexilang.g"
			
			
			indri::lang::LengthPrior * prior = new indri::lang::LengthPrior(q, 0);
			_nodes.push_back(prior);
			q=prior;
			
			
#line 144 "NexiParser.cpp"
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
#line 213 "nexilang.g"
			
			indri::lang::FieldWildcard * wild = new indri::lang::FieldWildcard;
			_nodes.push_back(wild);
			indri::lang::ExtentRestriction * r = new indri::lang::ExtentRestriction(c, wild);
			_nodes.push_back(r);
			
			
			indri::lang::LengthPrior * prior = new indri::lang::LengthPrior(r, 0);
			_nodes.push_back(prior);
			q=prior;
			
			
#line 173 "NexiParser.cpp"
		}
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return q ;
}

 indri::lang::NestedExtentInside*  NexiParser::path() {
#line 251 "nexilang.g"
	 indri::lang::NestedExtentInside* e ;
#line 189 "NexiParser.cpp"
#line 251 "nexilang.g"
	
	indri::lang::RawExtentNode * f = 0;
	indri::lang::RawExtentNode * f2 = 0;
	indri::lang::NestedExtentInside * c;
	e = 0;
	
#line 197 "NexiParser.cpp"
	
	match(SLASH);
	match(SLASH);
	{
	bool synPredMatched66 = false;
	if (((LA(1) == WILD || LA(1) == O_PAREN || LA(1) == TERM))) {
		int _m66 = mark();
		synPredMatched66 = true;
		inputState->guessing++;
		try {
			{
			field();
			match(SLASH);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched66 = false;
		}
		rewind(_m66);
		inputState->guessing--;
	}
	if ( synPredMatched66 ) {
		f=field();
		match(SLASH);
		match(SLASH);
		if ( inputState->guessing==0 ) {
#line 258 "nexilang.g"
			
			c = new indri::lang::NestedExtentInside(NULL, f);
			_nodes.push_back(c);
			e = c;
			
#line 230 "NexiParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			bool synPredMatched69 = false;
			if (((LA(1) == WILD || LA(1) == O_PAREN || LA(1) == TERM))) {
				int _m69 = mark();
				synPredMatched69 = true;
				inputState->guessing++;
				try {
					{
					field();
					match(SLASH);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched69 = false;
				}
				rewind(_m69);
				inputState->guessing--;
			}
			if ( synPredMatched69 ) {
				f2=field();
				match(SLASH);
				match(SLASH);
				if ( inputState->guessing==0 ) {
#line 266 "nexilang.g"
					
					indri::lang::NestedExtentInside * ct = new indri::lang::NestedExtentInside(NULL, f2);
					_nodes.push_back(ct);
					c->setInner(ct);
					c = ct;
					
#line 263 "NexiParser.cpp"
				}
			}
			else {
				goto _loop70;
			}
			
		}
		_loop70:;
		} // ( ... )*
	}
	else if ((LA(1) == WILD || LA(1) == O_PAREN || LA(1) == TERM)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	return e ;
}

 indri::lang::RawExtentNode *  NexiParser::field() {
#line 278 "nexilang.g"
	 indri::lang::RawExtentNode * e ;
#line 287 "NexiParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  fieldName3 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 278 "nexilang.g"
	
	indri::lang::Field * f = 0;
	indri::lang::ExtentOr * eo = 0;
	e = 0;
	
#line 297 "NexiParser.cpp"
	
	switch ( LA(1)) {
	case WILD:
	{
		match(WILD);
		if ( inputState->guessing==0 ) {
#line 284 "nexilang.g"
			
			e = new indri::lang::FieldWildcard;
			_nodes.push_back(e);
			
#line 309 "NexiParser.cpp"
		}
		break;
	}
	case TERM:
	{
		fieldName = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 289 "nexilang.g"
			
			f = new indri::lang::Field(fieldName->getText());
			_nodes.push_back(f);
			e = f;
			
#line 324 "NexiParser.cpp"
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
#line 294 "nexilang.g"
			
			f = new indri::lang::Field(fieldName2->getText());
			_nodes.push_back(f);
			eo = new indri::lang::ExtentOr;
			_nodes.push_back(eo);
			eo->addChild(f);
			e = eo;
			
#line 344 "NexiParser.cpp"
		}
		{ // ( ... )+
		int _cnt74=0;
		for (;;) {
			if ((LA(1) == 41)) {
				match(41);
				fieldName3 = LT(1);
				match(TERM);
				if ( inputState->guessing==0 ) {
#line 302 "nexilang.g"
					
					f = new indri::lang::Field(fieldName3->getText());
					_nodes.push_back(f);
					eo->addChild(f);
					
#line 360 "NexiParser.cpp"
				}
			}
			else {
				if ( _cnt74>=1 ) { goto _loop74; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt74++;
		}
		_loop74:;
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
	return e ;
}

 indri::lang::ScoredExtentNode*  NexiParser::clause() {
#line 312 "nexilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 386 "NexiParser.cpp"
#line 312 "nexilang.g"
	
	indri::lang::ScoredExtentNode * c = 0;
	indri::lang::UnweightedCombinationNode * l = 0;
	
#line 392 "NexiParser.cpp"
	
	s=filter();
	{
	switch ( LA(1)) {
	case AND:
	case OR:
	{
		l=logical();
		c=filter();
		if ( inputState->guessing==0 ) {
#line 317 "nexilang.g"
			
			l->addChild(s);
			l->addChild(c);
			s = l;
			
#line 409 "NexiParser.cpp"
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
	return s ;
}

 indri::lang::ScoredExtentNode*  NexiParser::termList() {
#line 227 "nexilang.g"
	 indri::lang::ScoredExtentNode* q ;
#line 430 "NexiParser.cpp"
#line 227 "nexilang.g"
	
	indri::lang::CombineNode* c = 0; 
	indri::lang::ScoredExtentNode* s = 0;
	
	
#line 437 "NexiParser.cpp"
	
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
#line 234 "nexilang.g"
			
			c = new indri::lang::CombineNode;
			c->addChild(q);
			c->addChild(s);
			_nodes.push_back(c);
			q = c;
			
#line 459 "NexiParser.cpp"
		}
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				s=term();
				if ( inputState->guessing==0 ) {
#line 242 "nexilang.g"
					
					c->addChild(s);
					
#line 470 "NexiParser.cpp"
				}
			}
			else {
				goto _loop62;
			}
			
		}
		_loop62:;
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
	return q ;
}

 indri::lang::ScoredExtentNode*  NexiParser::term() {
#line 441 "nexilang.g"
	 indri::lang::ScoredExtentNode* t ;
#line 499 "NexiParser.cpp"
#line 441 "nexilang.g"
	
	indri::lang::NotNode * n = 0;
	
#line 504 "NexiParser.cpp"
	
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
#line 446 "nexilang.g"
			
			n = new indri::lang::NotNode;
			_nodes.push_back(n);
			n->setChild(t);
			t = n;
			
#line 533 "NexiParser.cpp"
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

 indri::lang::ScoredExtentNode*  NexiParser::filter() {
#line 335 "nexilang.g"
	 indri::lang::ScoredExtentNode* s;
#line 548 "NexiParser.cpp"
#line 335 "nexilang.g"
	
	indri::lang::RawExtentNode * a = 0;
	indri::lang::RawExtentNode * contexts = 0;  
	
#line 554 "NexiParser.cpp"
	
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
#line 340 "nexilang.g"
			
			
			if ( _shrinkage == true ) {
			indri::lang::DocumentStructureNode * d = new indri::lang::DocumentStructureNode;
			_nodes.push_back( d );
			s = new indri::lang::ShrinkageScorerNode( a, d );
			_nodes.push_back( s );
			} else {
			s = new indri::lang::NestedRawScorerNode( a, contexts );
			_nodes.push_back( s );
			}
			
			indri::lang::MaxNode * m = new indri::lang::MaxNode;
			_nodes.push_back(m);
			m->addChild(s);
			s = m;        
			
			
#line 585 "NexiParser.cpp"
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
	return s;
}

 indri::lang::UnweightedCombinationNode*  NexiParser::logical() {
#line 324 "nexilang.g"
	 indri::lang::UnweightedCombinationNode* s;
#line 605 "NexiParser.cpp"
	
	switch ( LA(1)) {
	case AND:
	{
		match(AND);
		if ( inputState->guessing==0 ) {
#line 325 "nexilang.g"
			
			s = new indri::lang::CombineNode;
			_nodes.push_back(s);
			
#line 617 "NexiParser.cpp"
		}
		break;
	}
	case OR:
	{
		match(OR);
		if ( inputState->guessing==0 ) {
#line 329 "nexilang.g"
			
			s = new indri::lang::OrNode;
			_nodes.push_back(s);
			
#line 630 "NexiParser.cpp"
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return s;
}

 indri::lang::ScoredExtentNode*  NexiParser::aboutClause() {
#line 366 "nexilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 645 "NexiParser.cpp"
#line 366 "nexilang.g"
	
	indri::lang::NestedExtentInside * p = 0;
	indri::lang::ScoredExtentNode * t = 0;
	indri::lang::RawExtentNode * f = 0;
	indri::lang::ExtentRestriction * r = 0;
	
#line 653 "NexiParser.cpp"
	
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
#line 372 "nexilang.g"
			
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
			
#line 687 "NexiParser.cpp"
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
	return s ;
}

 indri::lang::RawExtentNode *  NexiParser::arithmeticClause() {
#line 394 "nexilang.g"
	 indri::lang::RawExtentNode * s;
#line 712 "NexiParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  field = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 394 "nexilang.g"
	
	indri::lang::Field* f = 0;
	indri::lang::RawExtentNode* c = 0;
	indri::lang::NestedExtentInside* p = 0;
	INT64 n = 0;
	s = 0;
	
#line 722 "NexiParser.cpp"
	
	match(42);
	p=path();
	field = LT(1);
	match(TERM);
	if ( inputState->guessing==0 ) {
#line 403 "nexilang.g"
		
		f = new indri::lang::Field(field->getText());
		_nodes.push_back(f);
		
#line 734 "NexiParser.cpp"
	}
	{
	switch ( LA(1)) {
	case LESS:
	{
		match(LESS);
		n=number();
		if ( inputState->guessing==0 ) {
#line 407 "nexilang.g"
			
			c = new indri::lang::FieldLessNode(f, n);
			_nodes.push_back(c);
			
#line 748 "NexiParser.cpp"
		}
		break;
	}
	case LESSEQ:
	{
		match(LESSEQ);
		n=number();
		if ( inputState->guessing==0 ) {
#line 411 "nexilang.g"
			
			c = new indri::lang::FieldLessNode(f, n + 1);
			_nodes.push_back(c);
			
#line 762 "NexiParser.cpp"
		}
		break;
	}
	case EQUALS:
	{
		match(EQUALS);
		n=number();
		if ( inputState->guessing==0 ) {
#line 415 "nexilang.g"
			
			c = new indri::lang::FieldEqualsNode(f, n);
			_nodes.push_back(c);
			
#line 776 "NexiParser.cpp"
		}
		break;
	}
	case GREATER:
	{
		match(GREATER);
		n=number();
		if ( inputState->guessing==0 ) {
#line 419 "nexilang.g"
			
			c = new indri::lang::FieldGreaterNode(f, n);
			_nodes.push_back(c);
			
#line 790 "NexiParser.cpp"
		}
		break;
	}
	case GREATEREQ:
	{
		match(GREATEREQ);
		n=number();
		if ( inputState->guessing==0 ) {
#line 423 "nexilang.g"
			
			c = new indri::lang::FieldGreaterNode(f, n - 1);
			_nodes.push_back(c);
			
#line 804 "NexiParser.cpp"
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
#line 426 "nexilang.g"
		
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
		
#line 828 "NexiParser.cpp"
	}
	return s;
}

 indri::lang::ScoredExtentNode*  NexiParser::filterParens() {
#line 361 "nexilang.g"
	 indri::lang::ScoredExtentNode* s ;
#line 836 "NexiParser.cpp"
	
	match(O_PAREN);
	s=clause();
	match(C_PAREN);
	return s ;
}

 INT64  NexiParser::number() {
#line 522 "nexilang.g"
	 INT64 v ;
#line 847 "NexiParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 522 "nexilang.g"
	
	v = 0;
	
#line 853 "NexiParser.cpp"
	
	n = LT(1);
	match(NUMBER);
	if ( inputState->guessing==0 ) {
#line 525 "nexilang.g"
		
		v = string_to_i64(n->getText());
		
#line 862 "NexiParser.cpp"
	}
	return v ;
}

 indri::lang::ScoredExtentNode *  NexiParser::unrestrictedTerm() {
#line 454 "nexilang.g"
	 indri::lang::ScoredExtentNode * t ;
#line 870 "NexiParser.cpp"
#line 454 "nexilang.g"
	
	indri::lang::RawExtentNode * raw = 0;
	indri::lang::RawExtentNode * contexts = 0;
	t = 0;
	
#line 877 "NexiParser.cpp"
	
	switch ( LA(1)) {
	case NUMBER:
	case FLOAT:
	case TERM:
	{
		raw=rawText();
		if ( inputState->guessing==0 ) {
#line 460 "nexilang.g"
			
			if ( _shrinkage == true ) {
			indri::lang::DocumentStructureNode * d = new indri::lang::DocumentStructureNode;
			_nodes.push_back( d );
			t = new indri::lang::ShrinkageScorerNode( raw, d );
			_nodes.push_back( t );
			} else {
			t = new indri::lang::NestedRawScorerNode( raw, contexts );
			_nodes.push_back( t );
			}
			
#line 898 "NexiParser.cpp"
		}
		break;
	}
	case DBL_QUOTE:
	{
		match(DBL_QUOTE);
		raw=odNode();
		match(DBL_QUOTE);
		if ( inputState->guessing==0 ) {
#line 471 "nexilang.g"
			
			
			if ( _shrinkage == true ) {
			indri::lang::DocumentStructureNode * d = new indri::lang::DocumentStructureNode;
			_nodes.push_back( d );
			t = new indri::lang::ShrinkageScorerNode( raw, d );
			_nodes.push_back( t );
			} else {
			t = new indri::lang::NestedRawScorerNode( raw, contexts );
			_nodes.push_back( t );
			}
			
#line 921 "NexiParser.cpp"
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

 indri::lang::RawExtentNode*  NexiParser::rawText() {
#line 488 "nexilang.g"
	 indri::lang::RawExtentNode* t ;
#line 936 "NexiParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 488 "nexilang.g"
	
	t = 0;
	
#line 944 "NexiParser.cpp"
	
	switch ( LA(1)) {
	case TERM:
	{
		id = LT(1);
		match(TERM);
		if ( inputState->guessing==0 ) {
#line 491 "nexilang.g"
			
			std::string text = id->getText();
			
			//     std::string::iterator c = text.begin();
			//     while (c != text.end()) { *c = tolower(*c); c++; }
			
			t = new indri::lang::IndexTerm(id->getText());
			_nodes.push_back(t);
			
#line 962 "NexiParser.cpp"
		}
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
		if ( inputState->guessing==0 ) {
#line 500 "nexilang.g"
			
			t = new indri::lang::IndexTerm(n->getText());
			_nodes.push_back(t);
			
#line 976 "NexiParser.cpp"
		}
		break;
	}
	case FLOAT:
	{
		f = LT(1);
		match(FLOAT);
		if ( inputState->guessing==0 ) {
#line 504 "nexilang.g"
			
			t = new indri::lang::IndexTerm(f->getText());
			_nodes.push_back(t);
			
#line 990 "NexiParser.cpp"
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

 indri::lang::ODNode*  NexiParser::odNode() {
#line 510 "nexilang.g"
	 indri::lang::ODNode* od ;
#line 1005 "NexiParser.cpp"
#line 510 "nexilang.g"
	
	RawExtentNode* t = 0;
	od = new indri::lang::ODNode;
	_nodes.push_back(od);
	
#line 1012 "NexiParser.cpp"
	
	{ // ( ... )+
	int _cnt91=0;
	for (;;) {
		if ((LA(1) == NUMBER || LA(1) == FLOAT || LA(1) == TERM)) {
			t=rawText();
			if ( inputState->guessing==0 ) {
#line 517 "nexilang.g"
				
				od->addChild(t);
				
#line 1024 "NexiParser.cpp"
			}
		}
		else {
			if ( _cnt91>=1 ) { goto _loop91; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt91++;
	}
	_loop91:;
	}  // ( ... )+
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
	"LESS",
	"GREATER",
	"LESSEQ",
	"GREATEREQ",
	"EQUALS",
	"JUNK",
	"\"|\"",
	"\".\"",
	"\",\"",
	0
};

const unsigned long NexiParser::_tokenSet_0_data_[] = { 6358272UL, 2UL, 0UL, 0UL };
// NUMBER FLOAT DBL_QUOTE MINUS PLUS TERM 
const ANTLR_USE_NAMESPACE(antlr)BitSet NexiParser::_tokenSet_0(_tokenSet_0_data_,4);


ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
