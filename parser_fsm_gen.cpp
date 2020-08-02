#include <set>
#include <iostream>
#include "parser_fsm_gen.h"
#define Current (*_tokens)[_idx]->t
#define Prew (_idx > 0 ? (*_tokens)[_idx - 1]->t : Tk::UNKNOWN)
#define Next (_size > _idx + 1 ? (*_tokens)[_idx + 1]->t : Tk::UNKNOWN)
#define Follow if (!_stack.empty()) _stack.pop_back()
#define FollowedBy(STATE) if (!_stack.empty()) _stack.back() = ParserFsm::Parser__State::STATE
#define Containes(STATE) _stack.push_back(ParserFsm::Parser__State::STATE)
#define LastNodeAs(TYPE) reinterpret_cast<TYPE*>(Node::pool.back().get())
#define PrewTokenAs(TYPE) reinterpret_cast<token_::TYPE*>((*_tokens)[_idx - 1].get())
#define CurrentTokenAs(TYPE) reinterpret_cast<token_::TYPE*>((*_tokens)[_idx].get())
#define NextTokenAs(TYPE) reinterpret_cast<token_::TYPE*>((*_tokens)[_idx + 1].get())
#define LastExprAs(TYPE) reinterpret_cast<TYPE*>(_exprs.back())
#define LastStmtAs(TYPE) reinterpret_cast<TYPE*>(_bodies.back()->statements.back())
#define PopNode Node::stack.pop_back()
using Parser___State = ParserFsm::Parser__State;

Module* ParserFsm::operator()(std::pair< std::vector<std::shared_ptr<token_::Token> >&, char const*> tokens)
{
	_module = new Module(tokens.second);
	_size = tokens.first.size();
	_tokens = &tokens.first;
	_idx = 0;
	std::set<Tk> binOp{
		Tk::DOT,
		Tk::ADD,
		Tk::SUB,
		Tk::MUL,
		Tk::DIV,
		Tk::OR,
		Tk::AND,
		Tk::IN,
		Tk::ADD_ASIGN,
		Tk::SUB_ASIGN,
		Tk::MUL_ASIGN,
		Tk::DIV_ASIGN,
		Tk::ASIGN,
		Tk::EQUAL,
	};

	std::set<Tk> laBinOp{
		Tk::DOT,
		Tk::ADD,
		Tk::SUB,
		Tk::MUL,
		Tk::DIV,
		Tk::OR,
		Tk::AND,
	};

	std::set<Tk> raBinOp{
		Tk::IN,
		Tk::ADD_ASIGN,
		Tk::SUB_ASIGN,
		Tk::MUL_ASIGN,
		Tk::DIV_ASIGN,
		Tk::ASIGN,
		Tk::EQUAL,
	};

	std::set<Tk> unOp {
		Tk::ADD,
		Tk::SUB,
		Tk::NOT,
	};

	while (State() != Parser__State::stop)
	{
		if (State() == Parser__State::expr) ParseExpr();

		if (Next != Tk::NATIVE  ) OnNextNotNative();
		if (Next != Tk::UNKNOWN && Next != Tk::EOF_) OnNotEOF();
		if (Next != Tk::END     ) OnNotEnd();
		if (Next == Tk::EOL     ) OnNextEOL();
		if (Next == Tk::COLON   ) OnNextColon();
		if (Next == Tk::DOT     ) OnNextDot();
		if (Next == Tk::ASIGN   ) OnNextAsign();
		if (Next == Tk::COMMA   ) OnNextComma();
		if (Next == Tk::CLS_PAR ) OnNextClosePar();

		if (Next == Tk::NATIVE  ) OnNextNative();
		if (Prew == Tk::COMMA   ) OnPrewComma();
		if (Prew == Tk::OPN_PAR ) OnPrewOpenPar();

		if (Prew == Tk::OPN_PAR &&  Current == Tk::ID && (Next == Tk::COMMA || Next == Tk::COLON) ) OnPrewOpenPar_and_CurrentId_and_NextComma_or_NextColon();
		if (Prew == Tk::OPN_PAR &&!(Current == Tk::ID && (Next == Tk::COMMA || Next == Tk::COLON))) OnPrewOpenPar_and_not_CurrentId_and_NextComma_or_NextColon();

		if (Current != Tk:: EOF_&& Current != Tk:: END   ) On_not_EOF_or_End();
		if (Current != Tk:: EOF_&& Current != Tk:: END
		&&!(Current == Tk:: OR  && Next    == Tk:: COMMA)) On_not_EOF_or_End_or_OrNextComma();
		if (Current != Tk:: EOL && Current != Tk:: EOF_  ) On_not_EOL_or_EOF();

		if (Current == Tk:: ID && Next == Tk:: COLON   ) OnIdNextColon();
		if (Current == Tk:: OR && Next == Tk:: COMMA   ) OnOrNextComma();
		if (Current == Tk:: OR && Next == Tk:: OPN_PAR ) OnOrNextOpenPar();

		if (Current == Tk:: AS         ) OnAs();
		if (Current == Tk:: BREAK      ) OnBreak();
		if (Current == Tk:: CLS_PAR    ) OnClosePar();
		if (Current == Tk:: COLON      ) OnColon();
		if (Current == Tk:: COMMA      ) OnComma();
		if (Current == Tk:: CONTINUE   ) OnContinue();
		if (Current == Tk:: DOT        ) OnDot();
		if (Current == Tk:: DOUBLE_DOT ) OnDoubleDot();
		if (Current == Tk:: EOF_       ) OnEOF();
		if (Current == Tk:: EOL        ) OnEOL();
		if (Current == Tk:: END        ) OnEnd();
		if (Current == Tk:: ENUMERABLE ) OnEnumerable();
		if (Current == Tk:: FLOAT      ) OnFloat();
		if (Current == Tk:: FOR        ) OnFor();
		if (Current == Tk:: ID         ) OnId();
		if (Current == Tk:: IF         ) OnIf();
		if (Current == Tk:: ELSE       ) OnElse();
		if (Current == Tk:: ELIF       ) OnElif();
		if (Current == Tk:: INT        ) OnInt();
		if (Current == Tk:: LAMBDA     ) OnLambda();
		if (Current == Tk:: NUM        ) OnNum();
		if (Current == Tk:: OPN_PAR    ) OnOpenPar();
		if (Current == Tk:: REAL_NUM   ) OnRealNum();
		if (Current == Tk:: RETURN     ) OnReturn();
		if (Current == Tk:: SEMICOLON  ) OnSemicolon();
		if (Current == Tk:: STATIC     ) OnStatic();
		if (Current == Tk:: STR        ) OnStr();
		if (Current == Tk:: STRING     ) OnString();
		if (Current == Tk:: THIS       ) OnThis();
		if (Current == Tk:: TYPE       ) OnType();
		if (Current == Tk:: USING      ) OnUsing();
		if (Current == Tk:: WHILE      ) OnWhile();
		if (Current == Tk:: YIELD      ) OnYield();

//		if (_exprs.empty())
//		{
//			On_inExprStack__empty();
//		}
//		else
//		{
//			auto const size = _exprs.size();
//#define StExpr(i) _exprs[size - (i)]->t
//			if (size >= 2 && binOp.count(StExpr(2)) && StExpr(1) == Tk::OPN_PAR) On_inExprStack__BinOp_OpenPar();
//			if (size >= 2 &&!binOp.count(StExpr(2)) && StExpr(1) == Tk::OPN_PAR) On_inExprStack__not_BinOp_OpenPar();
//			if (size >= 2 && (StExpr(4) == Tk::EXPR && laBinOp.count(StExpr(3)) && StExpr(2) == Tk::EXPR && (StExpr(1) == Tk::END   || StExpr(1) == Tk::COMMA   || StExpr(1) == Tk::CLS_PAR || StExpr(1) == Tk::EOL  || StExpr(1) == Tk::EOF_    || laBinOp.count(StExpr(1))))) On_inExprStack__Expr_LABinOp_Expr_LABinOp_or_End_or_Comma_or_ClosePar_or_EOL_or_EOF();
//			if (size >= 2 && (StExpr(4) == Tk::EXPR && raBinOp.count(StExpr(3)) && StExpr(2) == Tk::EXPR && (StExpr(1) == Tk::END   || StExpr(1) == Tk::COMMA   || StExpr(1) == Tk::CLS_PAR || StExpr(1) == Tk::EOL  || StExpr(1) == Tk::EOF_    || laBinOp.count(StExpr(1))))) On_inExprStack__Expr_RABinOp_Expr_LABinOp_or_End_or_Comma_or_ClosePar_or_EOL_or_EOF();
//			if (size >= 2 && unOp .count(StExpr(3)) && StExpr(2) == Tk::EXPR    &&(StExpr(1) == Tk::END  ||  StExpr(1) == Tk::COMMA || StExpr(1) == Tk::CLS_PAR || StExpr(1) == Tk::EOL     || StExpr(1) == Tk::EOF_ || StExpr(1) != Tk::OPN_PAR || StExpr(1) != Tk::DOT || !raBinOp.count(StExpr(1)))) On_inExprStack__UnOp_Expr_End_or_Comma_or_ClosePar_or_EOL_or_EOF_or_not_OpenPar_or_RABinOp();
//#undef StExpr
//		}
	}

   for (auto f : _module->functions)
   {
      _module->dfunctions[f->id] = f;
   }

   for (auto t : _module->types)
   {
      _module->dtypes[t->id] = t;
      for (auto f : t->fields) t->dfields[f->id] = f;
      for (auto f : t->staticFields) t->dstaticFields[f->id] = f;
      for (auto m : t->methods) t->dmethods[m->id] = m;
   }

	return _module;
}
void ParserFsm::OnAs() {
	switch(_stack.back()) {
	case Parser___State :: using_stmt : Follow; { auto p = LastStmtAs(Using); p->metaPath.push_back(LastExprAs(Id)->val); _exprs.pop_back(); p->hasPseudonim = true; p->pseudonim = NextTokenAs(Id)->val; _module->dependecies.push_back(p); _statements.pop_back(); _idx += 3; } break;
	default: break;
	}
}
void ParserFsm::OnBreak() {
	switch(_stack.back()) {
	case Parser___State :: stmt       : Follow; { _bodies.back()->statements.push_back(new Break()); ++_idx; } break;
	case Parser___State :: yield_stmt : Follow; { LastNodeAs(Yield)->to_break = true; ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnClosePar() {
	switch(_stack.back()) {
	case Parser___State :: call_expr        : Follow; if (!_exprs.empty()) { LastNodeAs(Call)->arguments.push_back(_exprs.back()); _exprs.pop_back(); ++_idx; } break;
	case Parser___State :: elif_expr        : Follow; if (!_exprs.empty()) { _exprs.back(); _exprs.pop_back(); ++_idx; } break;
	case Parser___State :: type_body        : Follow;                      { ++_idx; } break;
	//case Parser___State :: expr             : Follow;                      {/* ? */} break;
	//case Parser___State :: lambda_body      : Follow;                      { for (auto s : LastNodeAs(Body)->statements) LastExprAs(LambdaFunc)->body.push_back(s); _statements.pop_back(); } break;
	//case Parser___State :: lambda_ret       : Follow;                      { auto e = _exprs.back(); _exprs.pop_back();  LastExprAs(LambdaFunc)->body.push_back(e); } break;
	//case Parser___State :: if_expr          : FollowedBy(then_expr);       { auto e = _exprs.back(); _exprs.pop_back(); LastExprAs(IteExpr)->prop = e; } break;

	case Parser___State :: c_for_stmt       : FollowedBy(for_body);        {} break;
	case Parser___State :: else_stmt        : FollowedBy(if_body);         { If* s = new If(); _ifs.back()->else_body = s; _ifs.pop_back(); _ifs.push_back(s); s->prop = _exprs.back(); _exprs.pop_back(); _bodies.push_back(s->body = new Body()); ++_idx; } break;
	case Parser___State :: foreach_stmt     : FollowedBy(for_body);        { Foreach* s = LastStmtAs(Foreach); _bodies.push_back(s->body = new Body()); s->range = _exprs.back(); _exprs.pop_back(); ++_idx; } break;
	case Parser___State :: method_arguments : FollowedBy(func_body);       { ++_idx; if (Current == Tk::ASIGN) { if (_module->types.back()->methods.back()->is_native = Next == Tk::NATIVE) { Follow; _idx += 2; } else { _one_line_func = true; Containes(stmt); Containes(expr); ++_idx; } } } break;
	case Parser___State :: func_arguments   : FollowedBy(func_body);       { if (!_exprs.empty()) { _module->functions.back()->arguments.back()->val = _exprs.back(); _exprs.pop_back(); } ++_idx; if (Current == Tk::ASIGN) { if (_module->functions.back()->is_native = Next == Tk::NATIVE) { Follow; _idx += 2; } else { _one_line_func = true; Containes(stmt); Containes(expr); ++_idx; } } } break;
	case Parser___State :: field_def        : Follow;                      { if (_stack.back() == Parser___State::type_body) { _announced_variable->val = _exprs.back(); _announced_variable = nullptr; } else { LastStmtAs(VarDefinition)->val = _exprs.back(); } _exprs.pop_back(); } break;
	case Parser___State :: variable_def     : Follow;                      { LastStmtAs(VarDefinition)->val = _exprs.back(); _exprs.pop_back(); ++_idx; } break;
	case Parser___State :: if_stmt          : FollowedBy(if_body);         { If* s{ nullptr }; _ifs.push_back(s = new If()); _bodies.back()->statements.push_back(s); s->prop = _exprs.back(); _exprs.pop_back(); _bodies.push_back(s->body = new Body()); ++_idx; } break;
	case Parser___State :: stmt             : Follow;                      { if (!(_bodies.empty() || _exprs.empty())) { _bodies.back()->statements.push_back(_exprs.back()); _exprs.pop_back(); if (_one_line_func) { _one_line_func = false; _bodies.pop_back(); Follow; } } else ++_idx; } break;
	case Parser___State :: while_stmt       : FollowedBy(while_body);      { auto s = new While(); _bodies.back()->statements.push_back(s); _bodies.push_back(s->body = new Body()); s->prop = _exprs.back(); _exprs.pop_back(); ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnColon() {
	switch(_stack.back()) {
	case Parser___State :: foreach_stmt : Containes(expr);              { LastStmtAs(Foreach)->it = LastExprAs(Id); _exprs.pop_back(); } break;
	case Parser___State :: type_stmt    : FollowedBy(type_parent_stmt); { ++_idx; _module->types.back()->parent->id = CurrentTokenAs(Id)->val; ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnComma() {
	switch(_stack.back()) {
	case Parser___State :: call_expr        : ;                           {} break;
	case Parser___State :: elif_expr        : Containes(expr);            {} break;
	//case Parser___State :: expr             : Follow;                     {} break;
	case Parser___State :: field_def        : Follow;                     {} break;
	case Parser___State :: method_arguments : ;      if (!_exprs.empty()) { _module->types.back()->methods.back()->arguments.back()->val = _exprs.back(); _exprs.pop_back(); } ++_idx; break;
	case Parser___State :: func_arguments   : ;      if (!_exprs.empty()) { _module->functions.back()->arguments.back()->val = _exprs.back(); _exprs.pop_back(); } ++_idx; break;
	//case Parser___State :: if_stmt          : FollowedBy(if_body);        {} break;
	case Parser___State :: then_expr        : FollowedBy(else_expr);      {} break;
	default: break;
	}
}
void ParserFsm::OnContinue() {
	switch(_stack.back()) {
	case Parser___State :: stmt : Follow; { _bodies.back()->statements.push_back(new Continue()); ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnDot() {
	switch(_stack.back()) {
	case Parser___State :: field_announcement : ;                       {} break;
	case Parser___State :: lambda_expr        : FollowedBy(lambda_ret); {} break;
	case Parser___State :: type_parent_stmt   : ;                       { ++_idx; _module->types.back()->metaPath.push_back(_module->types.back()->id); _module->types.back()->id = CurrentTokenAs(Id)->val; ++_idx; } break;
	case Parser___State :: using_stmt         : ;                       { _module->dependecies.back()->metaPath.push_back(LastExprAs(Id)->val); _exprs.pop_back(); ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnDoubleDot() {
	switch(_stack.back()) {
	case Parser___State :: lambda_expr : FollowedBy(lambda_body); {} break;
	default: break;
	}
}
void ParserFsm::OnEOF() {
	switch(_stack.back()) {
	case Parser___State :: body                  : Follow;           {} break;
	case Parser___State :: c_for_stmt            : Follow;           {} break;
	case Parser___State :: call_expr             : Follow;           {} break;
	case Parser___State :: check_native_func     : Follow;           {} break;
	case Parser___State :: else_body             : Follow;           {} break;
	case Parser___State :: else_expr             : Follow;           {} break;
	case Parser___State :: elif_expr             : Follow;           {} break;
	case Parser___State :: else_stmt             : Follow;           {} break;
	case Parser___State :: enumerable_method_def : Follow;           {} break;
	case Parser___State :: expr                  : Follow;           {} break;
	case Parser___State :: field_announcement    : Follow;           {} break;
	case Parser___State :: field_def             : Follow;           {} break;
	case Parser___State :: for_body              : Follow;           {} break;
	case Parser___State :: for_stmt              : Follow;           {} break;
	case Parser___State :: foreach_stmt          : Follow;           {} break;
	case Parser___State :: func_body             : Follow;           {} break;
	case Parser___State :: if_body               : Follow;           {} break;
	case Parser___State :: if_expr               : Follow;           {} break;
	case Parser___State :: if_stmt               : Follow;           {} break;
	case Parser___State :: lambda_body           : Follow;           {} break;
	case Parser___State :: lambda_expr           : Follow;           {} break;
	case Parser___State :: lambda_ret            : Follow;           {} break;
	case Parser___State :: method_arguments      : Follow;           {} break;
	case Parser___State :: func_arguments        : Follow;           {} break;
	case Parser___State :: method_def            : Follow;           {} break;
	case Parser___State :: module_statements     : FollowedBy(stop); {} break;
	case Parser___State :: return_stmt           : Follow;           {} break;
	case Parser___State :: statement_of_module   : Follow;           {} break;
	case Parser___State :: stmt                  : Follow;           {} break;
	case Parser___State :: then_expr             : Follow;           {} break;
	case Parser___State :: type_body             : Follow;           {} break;
	case Parser___State :: type_parent_stmt      : Follow;           {} break;
	case Parser___State :: type_stmt             : Follow;           {} break;
	case Parser___State :: using_stmt            : Follow;           {} break;
	case Parser___State :: while_body            : Follow;           {} break;
	case Parser___State :: while_stmt            : Follow;           {} break;
	case Parser___State :: yield_stmt            : Follow;           {} break;
	default: break;
	}
}
void ParserFsm::OnEOL() {
	switch(_stack.back()) {
	case Parser___State :: call_expr          : Follow;          {} break;
	case Parser___State :: expr               : Follow;          {} break;
	case Parser___State :: field_announcement : Follow;          {} break;
	case Parser___State :: field_def          : Follow;          {if (_announced_variable != nullptr && !_exprs.empty()) { _announced_variable->val = _exprs.back(); _exprs.pop_back(); }} break;
	case Parser___State :: variable_def       : Follow;          {if (_announced_variable != nullptr && !_exprs.empty()) { _announced_variable->val = _exprs.back(); _exprs.pop_back(); }} break;
	case Parser___State :: lambda_body        : Follow;          {} break;
	case Parser___State :: lambda_expr        : Follow;          {} break;
	case Parser___State :: lambda_ret         : Follow;          {} break;
	case Parser___State :: return_stmt        : Follow;          { reinterpret_cast<Return*>(_bodies.back()->statements.back())->e = _exprs.back(); _exprs.pop_back(); } break;
	case Parser___State :: stmt               : Follow;          { if (!(_bodies.empty() || _exprs.empty())) {_bodies.back()->statements.push_back(_exprs.back()); _exprs.pop_back(); if (_one_line_func) {_one_line_func = false; _bodies.pop_back(); Follow;} } else ++_idx; } break;
	case Parser___State :: using_stmt         : Follow;          { auto p = LastStmtAs(Using); if (!p->hasPseudonim) { p->metaPath.push_back(LastExprAs(Id)->val); _exprs.pop_back(); } _module->dependecies.push_back(p); _statements.pop_back(); ++_idx; } break;
	case Parser___State :: yield_stmt         : Follow;          {} break;
	case Parser___State :: type_stmt          : ;                {++_idx;} break;
	case Parser___State :: statement_of_module: Follow;          {++_idx;} break;
	case Parser___State :: func_body          : Containes(stmt); {++_idx;} break;
	case Parser___State :: if_body            : Containes(stmt); {++_idx;} break;
	case Parser___State :: else_body          : Containes(stmt); {++_idx;} break;
	case Parser___State :: while_body         : Containes(stmt); {++_idx;} break;
	case Parser___State :: type_body          : if (Next != Tk::CLS_PAR) Containes(type_stmt); {++_idx;} break;
	default: break;
	}
}
void ParserFsm::OnEnd() {
	switch(_stack.back()) {
	case Parser___State :: body                  : Follow; {} break;
	case Parser___State :: call_expr             : Follow; {} break;
	case Parser___State :: variable_def          : Follow; {if (_announced_variable != nullptr && !_exprs.empty()) { _announced_variable->val = _exprs.back(); _exprs.pop_back(); }} break;
	case Parser___State :: enumerable_method_def : Follow; {} break;
	case Parser___State :: expr                  : Follow; {} break;
	case Parser___State :: for_body              : Follow; { ++_idx; _bodies.pop_back(); } break;
	case Parser___State :: func_body             : Follow; { ++_idx; _bodies.pop_back(); } break;
	case Parser___State :: if_body               : Follow; { ++_idx; _bodies.pop_back(); } break;
	case Parser___State :: else_body             : Follow; { ++_idx; _bodies.pop_back(); } break;
	case Parser___State :: while_body            : Follow; { ++_idx; _bodies.pop_back(); } break;
	case Parser___State :: lambda_body           : Follow; {} break;
	case Parser___State :: lambda_expr           : Follow; {} break;
	case Parser___State :: lambda_ret            : Follow; {} break;
	case Parser___State :: return_stmt           : Follow; { reinterpret_cast<Return*>(_bodies.back()->statements.back())->e = _exprs.back(); _exprs.pop_back(); } break;
	case Parser___State :: stmt                  : Follow; { _bodies.back()->statements.push_back(_exprs.back()); _exprs.pop_back(); } break;
	default: break;
	}
}
void ParserFsm::OnEnumerable() {
	switch(_stack.back()) {
	case Parser___State :: enumerable_method_def : Containes(method_def);            {} break;
	case Parser___State :: stmt                  : Containes(enumerable_method_def); {} break;
	case Parser___State :: type_body             : Containes(method_def);            {} break;
	default: break;
	}
}
void ParserFsm::OnFloat() {
	switch(_stack.back()) {
	case Parser___State :: field_announcement : if (Next == Tk::COMMA || Next == Tk::CLS_PAR) FollowedBy(field_def); { _announced_variable->type = &Type::Float; if (Next == Tk::ASIGN) FollowedBy(field_def); else ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnFor() {
	switch(_stack.back()) {
	case Parser___State :: stmt : FollowedBy(for_stmt); { _idx += 2; } break;
	default: break;
	}
}
void ParserFsm::OnId() {
	switch(_stack.back()) {
	//case Parser___State :: expr               : ;                     { _exprs.push_back(new Id(CurrentTokenAs(Id)->val)); ++_idx; } break;
	case Parser___State :: field_announcement : ;                     { auto s = CurrentTokenAs(Id)->val.c_str(); _announced_variable->type = _types.count(s)?_types[s]:(_types[s] = new Type(s)); _announced_variable = nullptr; } break;
	//case Parser___State :: lambda_expr        : ;                     { LastExprAs(LambdaFunc)->arguments.push_back(CurrentTokenAs(Id)->val); ++_idx; } break;
	case Parser___State :: method_arguments   : Containes(field_def); { _module->types.back()->methods.back()->arguments.push_back(_announced_variable = new VarDefinition(CurrentTokenAs(Id)->val)); } break;
	case Parser___State :: func_arguments     : Containes(field_def); { _module->functions.back()->arguments.push_back(_announced_variable = new VarDefinition(CurrentTokenAs(Id)->val)); } break;
	case Parser___State :: method_def         : ;                     { _module->types.back()->methods.back()->id = CurrentTokenAs(Id)->val; ++_idx; } break;
	case Parser___State :: stmt               : if (Next == Tk::ASIGN || Next == Tk::COLON) { if (Next== Tk::ASIGN) { FollowedBy(variable_def); } else { FollowedBy(variable_announcement); } _bodies.back()->statements.push_back(_announced_variable = new VarDefinition(CurrentTokenAs(Id)->val)); if (_announced_variable->hasConcreteType = Next == Tk::COLON) _idx += 2;} else Containes(expr); break;
	case Parser___State :: type_body          : Containes(field_def); { (_static_field ? _module->types.back()->staticFields : _module->types.back()->fields).push_back(_announced_variable = new VarDefinition(CurrentTokenAs(Id)->val)); _static_field = false; } break;
	case Parser___State :: type_parent_stmt   : ;                     {} break;
	case Parser___State :: type_stmt          : FollowedBy(field_def); { (_static_field ? _module->types.back()->staticFields : _module->types.back()->fields).push_back(_announced_variable = new VarDefinition(CurrentTokenAs(Id)->val)); _static_field = false; } break; //: if (Next == Tk::ASIGN || Next == Tk::COLON) { if (Next== Tk::ASIGN) { FollowedBy(field_def); } else { FollowedBy(field_announcement); } _module->types.back()->fields.push_back(_announced_variable = new VarDefinition(CurrentTokenAs(Id)->val)); if (_announced_variable->hasConcreteType = Next == Tk::COLON) _idx += 2; } break;
	case Parser___State :: func_stmt          : ;                     { _module->functions.back()->id = CurrentTokenAs(Id)->val; ++_idx; } break;
	case Parser___State :: using_stmt         : ;                     { _exprs.push_back(new Id(CurrentTokenAs(Id)->val)); ++_idx; } break;
	case Parser___State :: for_stmt           : FollowedBy(foreach_stmt); Containes(expr);      { Foreach* s{ nullptr }; _bodies.back()->statements.push_back(s = new Foreach()); if (Next == Tk::COLON) { s->it = new Id(CurrentTokenAs(Id)->val); } else if (Next == Tk::COMMA) { s->idx = new Id(CurrentTokenAs(Id)->val); _idx += 2; s->it = new Id(CurrentTokenAs(Id)->val); } _idx += 2; } break;
	case Parser___State :: func_body          : Containes(stmt); break;
	default: break;
	}
}
void ParserFsm::OnIdNextColon() {
	switch(_stack.back()) {
	case Parser___State :: call_expr : Containes(exprt); { } break;
	default: break;
	}
}
void ParserFsm::OnNextComma() {
	switch (_stack.back()) {
	case Parser___State :: field_def : Follow; { ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnNextClosePar() {
	switch (_stack.back()) {
	case Parser___State :: field_def          : Follow; { if (_stack.back() == Parser___State::type_body && !_exprs.empty()) { _module->types.back()->fields.back()->val = _exprs.back(); _exprs.pop_back(); _announced_variable = nullptr; } ++_idx; } break;
	case Parser___State :: field_announcement : Follow; { if (Current == Tk::INT) { _announced_variable->type = &Type::Int; } else if (Current == Tk::FLOAT) { _announced_variable->type = &Type::Float; } else if (Current == Tk::STR) { _announced_variable->type = &Type::String; } else if (Current == Tk::ID) { auto s = CurrentTokenAs(Id)->val.c_str(); _announced_variable->type = _types.count(s)?_types[s]:(_types[s] = new Type(s)); } _announced_variable = nullptr; ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnIf() {
	switch(_stack.back()) {
	case Parser___State :: stmt : FollowedBy(if_stmt); { Containes(expr); _idx += 2; } break;
	default: break;
	}
}
void ParserFsm::OnElif() {
	switch(_stack.back()) {
	case Parser___State :: stmt    : Follow;                {} break;
	case Parser___State :: if_body : FollowedBy(else_stmt); { Containes(expr); _idx += 2; _bodies.pop_back(); } break;
	default: break;
	}
}
void ParserFsm::OnElse() {
	switch(_stack.back()) {
	case Parser___State :: stmt    : Follow;                {} // ASSERT(_stack.back() == Parser___State::if_body);
	case Parser___State :: if_body : FollowedBy(else_body); { _bodies.pop_back(); _bodies.push_back(new Body()); _ifs.back()->else_body = _bodies.back(); _ifs.pop_back(); ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnInt() {
	switch(_stack.back()) {
	case Parser___State :: field_announcement : if (Next == Tk::COMMA || Next == Tk::CLS_PAR) FollowedBy(field_def); { _announced_variable->type = &Type::Int; if (Next == Tk::ASIGN) FollowedBy(field_def); else ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnLambda() {
	switch(_stack.back()) {
	//case Parser___State :: expr                : Containes(lambda_expr); { _exprs.push_back(new LambdaFunc()); ++_idx; } break;
	case Parser___State :: statement_of_module : Containes(func_stmt);   { _module->functions.push_back(new Func()); _bodies.push_back(_module->functions.back()->body = new Body()); ++_idx; } break;
	case Parser___State :: type_stmt           : Follow;
	case Parser___State :: type_body           : Containes(method_def);  { _module->types.back()->methods.push_back(new Func()); _bodies.push_back(_module->types.back()->methods.back()->body = new Body()); ++_idx; } break;
	case Parser___State :: stmt                : Containes(expr);        {} break;
	default: break;
	}
}
void ParserFsm::OnNextColon() {
	switch(_stack.back()) {
	case Parser___State :: field_def : FollowedBy(field_announcement); { _announced_variable->hasConcreteType = true; _idx += 2; } break;
	case Parser___State :: stmt      : FollowedBy(field_def);          {} break;
	default: break;
	}
}
void ParserFsm::OnNextEOL() {
	switch(_stack.back()) {
	case Parser___State :: field_announcement : Follow; { _announced_variable = nullptr; _idx += 2; } break;
	case Parser___State :: func_body          : if (Current != Tk::END) Containes(body); {} break;
	default: break;
	}
}
void ParserFsm::OnNextAsign() {
	switch(_stack.back()) {
	case Parser___State :: func_body             : Containes(check_native_func); {} break;
	case Parser___State :: statement_of_module   : Containes(global_const);      {} break;
	case Parser___State :: stmt                  : FollowedBy(field_def);        {} break;
	case Parser___State :: field_def             : Containes(expr);              { _idx += 2; } break;
	case Parser___State :: variable_def          : Containes(expr);              { _idx += 2; } break;
	case Parser___State :: field_announcement    : FollowedBy(field_def);        { if (Current == Tk::ID) {if (_announced_variable->type == nullptr) { auto s = CurrentTokenAs(Id)->val.c_str(); _announced_variable->type = _types.count(s)?_types[s]:(_types[s] = new Type(s)); } else {_announced_variable->type->metaPath.push_back(_announced_variable->type->id); _announced_variable->type->id = CurrentTokenAs(Id)->val;} } else if (Current == Tk::INT) _announced_variable->type = &Type::Int; else if (Current == Tk::FLOAT) _announced_variable->type = &Type::Float; else if (Current == Tk::STR) _announced_variable->type = &Type::String; } break;
	case Parser___State :: variable_announcement : FollowedBy(variable_def);     { if (Current == Tk::ID) {if (_announced_variable->type == nullptr) { auto s = CurrentTokenAs(Id)->val.c_str(); _announced_variable->type = _types.count(s)?_types[s]:(_types[s] = new Type(s)); } else {_announced_variable->type->metaPath.push_back(_announced_variable->type->id); _announced_variable->type->id = CurrentTokenAs(Id)->val;} } else if (Current == Tk::INT) _announced_variable->type = &Type::Int; else if (Current == Tk::FLOAT) _announced_variable->type = &Type::Float; else if (Current == Tk::STR) _announced_variable->type = &Type::String; } break;
	default: break;
	}
}
void ParserFsm::OnNextDot() {
	switch(_stack.back()) {
	case Parser___State :: variable_announcement : ; { if (Current == Tk::ID) {if (_announced_variable->type == nullptr) {auto s = CurrentTokenAs(Id)->val.c_str(); _announced_variable->type = _types.count(s)?_types[s]:(_types[s] = new Type(s)); } else {_announced_variable->type->metaPath.push_back(_announced_variable->type->id); _announced_variable->type->id = CurrentTokenAs(Id)->val;} } } break;
	default: break;
	}
}
void ParserFsm::OnNextNative() {
	switch(_stack.back()) {
	case Parser___State :: check_native_func : Follow; {} break;
	default: break;
	}
}
void ParserFsm::OnNextNotNative() {
	switch(_stack.back()) {
	case Parser___State :: check_native_func : FollowedBy(stmt); {} break;
	default: break;
	}
}
void ParserFsm::OnNotEOF() {
	switch(_stack.back()) {
	case Parser___State :: module_statements : Containes(statement_of_module); {} break;
	default: break;
	}
}
void ParserFsm::OnNotEnd() {
	switch(_stack.back()) {
	case Parser___State :: body        : Containes(stmt); {} break;
	case Parser___State :: lambda_body : Containes(body); {} break;
	default: break;
	}
}
void ParserFsm::OnNum() {
	switch(_stack.back()) {
	//case Parser___State :: expr : ;                 { _exprs.push_back(new Num(CurrentTokenAs(Number)->val)); ++_idx; } break;
	case Parser___State :: stmt : Containes(expr); {} break;
	default: break;
	}
}
void ParserFsm::OnOpenPar() {
	switch(_stack.back()) {
	case Parser___State :: method_def       : FollowedBy(method_arguments); { ++_idx; } break;
	case Parser___State :: func_stmt        : FollowedBy(func_arguments);   { ++_idx; } break;
	case Parser___State :: type_parent_stmt : FollowedBy(type_body);        { ++_idx; } break;
	case Parser___State :: type_stmt        : FollowedBy(type_body);        { ++_idx; } break;
	case Parser___State :: else_stmt        : Containes(expr);              {} break;
	case Parser___State :: if_expr          : Containes(expr);              {} break;
	case Parser___State :: if_stmt          : Containes(expr);              {} break;
	case Parser___State :: while_stmt       : Containes(expr);              {} break;
	case Parser___State :: then_expr        : Containes(expr);              {} break;
	case Parser___State :: stmt             : Containes(expr);              {} break;
	//case Parser___State :: expr             : Containes(expr);              { _exprs.push_back(new EToken(Current)); ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnOrNextComma() {
	switch(_stack.back()) {
	case Parser___State :: else_body : FollowedBy(else_body); {} break;
	case Parser___State :: if_body   : FollowedBy(else_body); {} break;
	default: break;
	}
}
void ParserFsm::OnOrNextOpenPar() {
	switch(_stack.back()) {
	case Parser___State :: if_body : FollowedBy(else_stmt); {} break;
	default: break;
	}
}
void ParserFsm::OnPrewComma() {
	switch(_stack.back()) {
	case Parser___State :: foreach_stmt : Containes(field_def); {} break;
	default: break;
	}
}
void ParserFsm::OnPrewOpenPar() {
	switch(_stack.back()) {
	case Parser___State :: c_for_stmt   : Containes(field_def); {} break;
	case Parser___State :: foreach_stmt : Containes(field_def); {} break;
	default: break;
	}
}
void ParserFsm::OnPrewOpenPar_and_CurrentId_and_NextComma_or_NextColon() {
	switch(_stack.back()) {
	case Parser___State :: for_stmt : FollowedBy(foeach_stmt); {} break;
	default: break;
	}
}
void ParserFsm::OnPrewOpenPar_and_not_CurrentId_and_NextComma_or_NextColon() {
	switch(_stack.back()) {
	case Parser___State :: for_stmt : FollowedBy(c_for_stmt); {} break;
	default: break;
	}
}
void ParserFsm::OnRealNum() {
	switch(_stack.back()) {
	//case Parser___State :: expr : ;                 { _exprs.push_back(new RealNum(CurrentTokenAs(RealNum)->val)); ++_idx; } break;
	case Parser___State :: stmt : Containes(expr); {} break;
	default: break;
	}
}
void ParserFsm::OnReturn() {
	switch(_stack.back()) {
	case Parser___State :: stmt       : FollowedBy(return_stmt); { _bodies.back()->statements.push_back(new Return()); if (!(Next == Tk::EOL || Next == Tk::END)) Containes(expr); else Follow; ++_idx; } break;
	case Parser___State :: yield_stmt : Containes(expr);         {} break;
	default: break;
	}
}
void ParserFsm::OnSemicolon() {
	switch(_stack.back()) {
	case Parser___State :: c_for_stmt : Containes(expr); {} break;
	default: break;
	}
}
void ParserFsm::OnStatic() {
	switch(_stack.back()) {
	case Parser___State :: method_def : ;                     {} break;
	case Parser___State :: type_body  : Containes(field_def); { _static_field = true; ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnStr() {
	switch(_stack.back()) {
	case Parser___State :: field_announcement : if (Next == Tk::COMMA || Next == Tk::CLS_PAR) FollowedBy(field_def); { _announced_variable->type = &Type::String; if (Next == Tk::ASIGN) FollowedBy(field_def); else ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnString() {
	switch(_stack.back()) {
	//case Parser___State :: expr : ;                 { _exprs.push_back(new String(CurrentTokenAs(String)->val)); ++_idx; } break;
	case Parser___State :: stmt : Containes(expr); {} break;
	default: break;
	}
}
void ParserFsm::OnThis() {
	switch(_stack.back()) {
	case Parser___State :: expr : ;                {} break;
	case Parser___State :: stmt : Containes(expr); {} break;
	default: break;
	}
}
void ParserFsm::OnType() {
	switch(_stack.back()) {
	case Parser___State :: statement_of_module : Containes(type_stmt); { ++_idx; auto s = CurrentTokenAs(Id)->val.c_str(); _module->types.push_back(_types[s] = new Type(s)); ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnUsing() {
	switch(_stack.back()) {
	case Parser___State :: statement_of_module : Containes(using_stmt); { _statements.push_back(new Using()); ++_idx; } break;
	default: break;
	}
}
void ParserFsm::OnWhile() {
	switch(_stack.back()) {
	case Parser___State :: stmt : FollowedBy(while_stmt); { Containes(expr); _idx += 2; } break;
	default: break;
	}
}
void ParserFsm::OnYield() {
	switch(_stack.back()) {
	case Parser___State :: stmt : FollowedBy(yield_stmt); {} break;
	default: break;
	}
}


bool EqWords(std::vector<Node*> const & word1, std::vector<Tk>& word2)
{
	auto const size = word1.size();
	if (size != word2.size()) return false;
	for (size_t i = 0; size > i; ++i) if (word1[i]->t != word2[i]) return false;
	return true;
}

int RuleWithWord(std::vector<Node*> const & word, std::vector<ParserFsm::RulePair>& rules)
{
	auto const size = rules.size();
	for (size_t i = 0; size > i; ++i) if (EqWords(word, rules[i].first)) return i;
	return -1;
}

bool IsRuleStartsWithWord(std::vector<Tk>& rule, std::vector<Node*> const & word)
{
	auto const size = word.size();
	if (size > rule.size()) return false;
	for (size_t i = 0; size > i; ++i) if (rule[i] != word[i]->t) return false;
	return true;
}

int WordStartsWithRule(std::vector<Node*> const & word, std::vector<ParserFsm::RulePair>& rules)
{
	auto const size = rules.size();
	for (size_t i = 0; size > i; ++i) if (IsRuleStartsWithWord(rules[i].first, word)) return i;
	return -1;
}


void ParserFsm::ParseExpr()
{
	ParseByRules(rules, power);
}


void ParserFsm::ParseType()
{
	//ParseByRules(type_rules, type_power);
}

void ParserFsm::ParseByRules(std::vector<RulePair>& rules, int* power)
{
	size_t& j = _idx;
	int parenthesis = 0;
	while (_stack.back() == Parser___State::expr)
	{
		auto c = (*_tokens)[j++].get();
		if (c->t == Tk::EOL && parenthesis > 0) continue;
		if (c->t == Tk::EOF_) { _stack.pop_back(); break; }
		{
			auto c2 = (*_tokens)[j]->t;
			if (c2 == Tk::ELIF || c2 == Tk::ELSE) { _stack.pop_back(); break; }
		}
		token_::Token* a = (j >= _tokens->size() ? nullptr : (*_tokens)[j].get());

		if (c->t == Tk::NUM)      _exprs.push_back(new ::Num(reinterpret_cast<token_::Number*>(c)->val));
		else if (c->t == Tk::REAL_NUM) _exprs.push_back(new ::RealNum(reinterpret_cast<token_::RealNum*>(c)->val));
		else if (c->t == Tk::STRING)   _exprs.push_back(new ::String(reinterpret_cast<token_::String*>(c)->val));
		else if (c->t == Tk::ID)       _exprs.push_back(new ::Id(reinterpret_cast<token_::Id*>(c)->val));
		else if (c->t == Tk::THIS)     _exprs.push_back(new ::This());
      else if (c->t == Tk::INT)      _exprs.push_back(&   ::Type::Int);
      else if (c->t == Tk::FLOAT)    _exprs.push_back(&   ::Type::Float);
      else if (c->t == Tk::STR)      _exprs.push_back(&   ::Type::String);
		else _exprs.push_back(new EToken(c->t));

		for (size_t i = 0; _exprs.size() > i;)
		{
			auto _exprs2 = std::vector<Node*>{ std::begin(_exprs) + i, std::end(_exprs) };

			int const n = RuleWithWord(_exprs2, rules);
			if (n != -1)
			{
				int p = power[n];
				_exprs2.push_back(new EToken(a == nullptr ? Tk::UNKNOWN : a->t));
				int p2 = p;
				for (size_t k = 0; _exprs2.size() > k; ++k)
				{
					int const n2 = WordStartsWithRule({ std::begin(_exprs2) + k, std::end(_exprs2) }, rules);
					if (n2 != -1)
					{
						p2 = power[n2];
						break;
					}
				}
				if (p2 > p) break;
				auto newExpr = rules[n].second(_exprs);
				_exprs.push_back(newExpr);
				i = 0;
				continue;
			}
			++i;
		}
		Tk curr = (*_tokens)[j]->t;
		if (curr == Tk::OPN_PAR) ++parenthesis;
		if (curr == Tk::CLS_PAR) --parenthesis;
		if (curr == Tk::EOF_ || (parenthesis == 0 && (curr == Tk::EOL || curr == Tk::END || curr == Tk::COMMA)) || 0 > parenthesis) { _stack.pop_back(); }
		//if (parenthesis < 0 && (*_tokens)[_idx-1]->t == Tk::CLS_PAR && _stack.back() != Parser___State::expr) --_idx;
	}
}



void ParserFsm::On_inExprStack__BinOp_OpenPar() {
	switch(_stack.back()) {
	//case Parser___State :: expr : Containes(exrp); {} break;
	default: break;
	}
}
void ParserFsm::On_inExprStack__CallStart_ClosePar()
{
	switch(_stack.back()) {
	case Parser___State :: expr      : ; {} break;
	case Parser___State :: call_expr : ; {} break;
	}
}
void ParserFsm::On_inExprStack__empty() {
	switch(_stack.back()) {
	case Parser___State :: lambda_ret : Containes(expr); {} break;
	default: break;
	}
}
void ParserFsm::On_inExprStack__not_BinOp_OpenPar() {
	switch(_stack.back()) {
	//case Parser___State :: expr : Containes(call_expr); { _exprs.pop_back(); Node::pool.pop_back(); auto fn = _exprs.back(); _exprs.pop_back(); Call* p; _exprs.push_back(p = new Call()); p->funcSource = fn; _stack_of_exprs.push_back(_exprs); _exprs.clear(); } break;
	default: break;
	}
}
void ParserFsm::On_not_EOF_or_End() {
	switch(_stack.back()) {
	case Parser___State :: for_body   : Containes(stmt); {} break;
	case Parser___State :: while_body : Containes(stmt); {} break;
	default: break;
	}
}
void ParserFsm::On_not_EOF_or_End_or_OrNextComma() {
	switch(_stack.back()) {
	case Parser___State :: else_body : Containes(stmt); {} break;
	default: break;
	}
}
void ParserFsm::On_not_EOL_or_EOF() {
	//switch(_stack.back()) {
	//case Parser___State :: return_stmt : FollowedBy(expr); {} break;
	//default: break;
	//}
}

Node* cb_Parenthesis(std::vector<Node*>& v)
{
	v.pop_back(); // )
	auto e = v.back();
	v.pop_back();
	v.pop_back(); // (
	return e;
}
Node* cb_MulBinOp(std::vector<Node*>& v)
{
	auto right = v.back();
	v.pop_back();
	v.pop_back(); // *
   auto left = v.back();
	v.pop_back();
   if (left->st == Nd::TYPE || right->st == Nd::TYPE)
   {
      auto e = Deckard();
      e->left = reinterpret_cast<Type*>(left);
      e->right = reinterpret_cast<Type*>(right);
      return e;
   }
	auto e = Mul();
   e->left = left;
   e->right = right;
	return e;
}
Node* cb_DivBinOp(std::vector<Node*>& v)
{
	auto e = Div();
	e->right = v.back();
	v.pop_back();
	v.pop_back();
	e->left = v.back();
	v.pop_back();
	return e;
}
Node* cb_AddBinOp(std::vector<Node*>& v)
{
	auto e = Add();
	e->right = v.back();
	v.pop_back();
	v.pop_back();
	e->left = v.back();
	v.pop_back();
	return e;
}
Node* cb_SubBinOp(std::vector<Node*>& v)
{
   auto right = v.back();
	v.pop_back();
	v.pop_back();
   auto left = v.back();
	v.pop_back();
   if (left->st == Nd::TYPE || right->st == Nd::TYPE)
   {
      auto e = Arrow();
      e->left = reinterpret_cast<Type*>(left);
      e->right = reinterpret_cast<Type*>(right);
      return e;
   }
	auto e = Sub();
	e->right = right;
	e->left = left;
	return e;
}
Node* cb_DotBinOp(std::vector<Node*>& v)
{
	auto e = Dot();
	e->right = v.back();
	v.pop_back();
	v.pop_back();
	e->left = v.back();
	v.pop_back();
	return e;
}
Node* cb_Call(std::vector<Node*>& v)
{
	auto e = new Call();
	v.pop_back();
	auto arg = v.back();
	v.pop_back();
	if (arg->st == Nd::SEQ) e->arguments = reinterpret_cast<Sequence*>(arg)->items;
	else e->arguments.push_back(arg);
	v.pop_back();
	e->funcSource = v.back();
	v.pop_back();
	return e;
}
Node* cb_VoidCall(std::vector<Node*>& v)
{
	auto e = new Call();
	v.pop_back();
	v.pop_back();
	e->funcSource = v.back();
	v.pop_back();
	return e;
}
Node* cb_Sequence(std::vector<Node*>& v)
{
	auto right = v.back();
	v.pop_back();
	v.pop_back();
	auto left = v.back();
	v.pop_back();
	Node* e;
	if (left->st == Nd::SEQ) { reinterpret_cast<Sequence*>(left)->items.push_back(right); e = left; }
	else { auto seq = new Sequence(); seq->items.push_back(left); seq->items.push_back(right); e = seq; }
	return e;
}
Node* cb_And(std::vector<Node*>& v)
{
	auto e = And();
	e->right = v.back();
	v.pop_back();
	v.pop_back();
	e->left = v.back();
	v.pop_back();
	return e;
}
Node* cb_Or(std::vector<Node*>& v)
{
	auto e = Or();
	e->right = v.back();
	v.pop_back();
	v.pop_back();
	e->left = v.back();
	v.pop_back();
	return e;
}
Node* cb_Equal(std::vector<Node*>& v)
{
	auto e = Equal();
	e->right = v.back();
	v.pop_back();
	v.pop_back();
	e->left = v.back();
	v.pop_back();
	return e;
}
Node* cb_ITE(std::vector<Node*>& v)
{
	auto e = new IteExpr();
	e->fall = v.back();
	v.pop_back();
	v.pop_back();
	e->prop = v.back();
	v.pop_back();
	v.pop_back();
	e->passed = v.back();
	v.pop_back();
	return e;
}
Node* cb_For(std::vector<Node*>& v)
{
	auto e = new FwExpr();
	e->source = v.back();
	v.pop_back();
	v.pop_back();
	auto id = v.back();
	if (id->st == Nd::SEQ)
	{
		auto sq = reinterpret_cast<Sequence*>(id);
		v.pop_back();
		auto const size = sq->items.size();
		if (size == 2 && sq->items[0]->st == Nd::ID && sq->items[1]->st == Nd::ID) e->id = sq->items[1], e->idxId = sq->items[0];
		else throw "Error!";
	}
	else if (id->st == Nd::ID)
	{
		e->id = id;
		v.pop_back();
	}
	else throw "Error!";
	v.pop_back();
	e->itemOp = v.back();
	v.pop_back();
	return e;
}
Node* cb_Where(std::vector<Node*>& v)
{
	auto p = v.back();
	v.pop_back();
	v.pop_back();
	auto e = v.back();
	v.pop_back();
	if (e->st != Nd::FW_EXPR) throw "Error!";
	reinterpret_cast<FwExpr*>(e)->prop = p;
	return e;
}

Node* cb_Lambda(std::vector<Node*>& v)
{
	auto e = new LambdaFunc();
	e->body = v.back();
	v.pop_back();
	v.pop_back();
	auto arg = v.back();
	if (arg->st == Nd::SEQ) e->arguments = reinterpret_cast<Sequence*>(arg)->items;
	else if (arg->st == Nd::ID) e->arguments.push_back(arg);
	else throw "Error!";
	v.pop_back();
	v.pop_back();
	return e;
}

Node* cb_Template(std::vector<Node*>& v)
{
}
