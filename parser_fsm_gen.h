#ifndef INCLUDED_parser_fsm_GEN_H
#define INCLUDED_parser_fsm_GEN_H
#include <vector>
#include "parser_nodes_.h"


Node* cb_Parenthesis(std::vector<Node*>& v);
Node* cb_MulBinOp   (std::vector<Node*>& v);
Node* cb_DivBinOp   (std::vector<Node*>& v);
Node* cb_AddBinOp   (std::vector<Node*>& v);
Node* cb_SubBinOp   (std::vector<Node*>& v);
Node* cb_DotBinOp   (std::vector<Node*>& v);
Node* cb_Call       (std::vector<Node*>& v);
Node* cb_VoidCall   (std::vector<Node*>& v);
Node* cb_Sequence   (std::vector<Node*>& v);
Node* cb_And        (std::vector<Node*>& v);
Node* cb_Or         (std::vector<Node*>& v);
Node* cb_Equal      (std::vector<Node*>& v);
Node* cb_ITE        (std::vector<Node*>& v);
Node* cb_For        (std::vector<Node*>& v);
Node* cb_Where      (std::vector<Node*>& v);
Node* cb_Lambda     (std::vector<Node*>& v);


struct ParserFsm
{
	enum class Parser__State {
		module_statements,
		body,
		c_for_stmt,
		call_expr,
		check_native_func,
		else_body,
		else_expr,
		else_stmt,
		enumerable_method_def,
		expr,
		exprt,
		exrp,
		field_announcement,
		field_def,
		foeach_stmt,
		for_body,
		for_stmt,
		foreach_stmt,
		func_body,
		func_stmt,
		global_const,
		if_body,
		if_expr,
		if_stmt,
		lambda_body,
		lambda_expr,
		lambda_ret,
		method_arguments,
		func_arguments,
		method_def,
		return_stmt,
		statement_of_module,
		stmt,
		then_expr,
		type_body,
		type_parent_stmt,
		type_stmt,
		using_stmt,
		while_body,
		while_stmt,
		yield_stmt,
		stop,
	};
	using RulePair = std::pair<std::vector<Tk>, Node*(*)(std::vector<Node*>&)>;
	std::vector<RulePair> rules {
	{{Tk::OPN_PAR, Tk::EXPR, Tk::CLS_PAR}, cb_Parenthesis},
	{{Tk::EXPR, Tk::DOT, Tk::EXPR}, cb_DotBinOp},

	{{Tk::EXPR, Tk::MUL, Tk::EXPR}, cb_MulBinOp}, {{Tk::EXPR, Tk::DIV, Tk::EXPR}, cb_DivBinOp},
	{{Tk::EXPR, Tk::ADD, Tk::EXPR}, cb_AddBinOp}, {{Tk::EXPR, Tk::SUB, Tk::EXPR}, cb_SubBinOp},
	{{Tk::EXPR, Tk::AND, Tk::EXPR}, cb_And}, {{Tk::EXPR, Tk::EQUAL, Tk::EXPR}, cb_Equal}, {{Tk::EXPR, Tk::OR, Tk::EXPR}, cb_Or},

	{{Tk::EXPR, Tk::OPN_PAR, Tk::EXPR, Tk::CLS_PAR}, cb_Call},    {{Tk::EXPR, Tk::OPN_PAR, Tk::CLS_PAR}, cb_VoidCall},  {{Tk::EXPR, Tk::IF, Tk::EXPR, Tk::ELSE, Tk::EXPR}, cb_ITE},
	{{Tk::EXPR, Tk::FOR, Tk::EXPR, Tk::FROM, Tk::EXPR}, cb_For},  {{Tk::EXPR, Tk::WHERE, Tk::EXPR}, cb_Where}, {{Tk::LAMBDA, Tk::EXPR, Tk::DOT, Tk::EXPR}, cb_Lambda}, {{Tk::EXPR, Tk::COMMA, Tk::EXPR}, cb_Sequence},
	};

	int power[16] = {
	      8      ,
		   7     ,
	    6 , 6    ,
	    5 , 5    ,
	    3,4,3    ,
	    2,2,2    ,
	   1,1,2,1   ,
	}     ;

	Parser__State State() { return _stack.back(); }

	void OnAs();
	void OnBreak();
	void OnClosePar();
	void OnColon();
	void OnComma();
	void OnContinue();
	void OnDot();
	void OnDoubleDot();
	void OnEOF();
	void OnEOL();
	void OnEnd();
	void OnEnumerable();
	void OnFloat();
	void OnFor();
	void OnId();
	void OnIdNextColon();
	void OnNextComma();
	void OnNextClosePar();
	void OnIf();
	void OnInt();
	void OnLambda();
	void OnNextColon();
	void OnNextEOL();
	void OnNextAsign();
	void OnNextNative();
	void OnNextNotNative();
	void OnNotEOF();
	void OnNotEnd();
	void OnNum();
	//void OnOpenExpr();
	void OnOpenPar();
	void OnOrNextComma();
	void OnOrNextOpenPar();
	void OnPrewComma();
	void OnPrewOpenPar();
	void OnPrewOpenPar_and_CurrentId_and_NextComma_or_NextColon();
	void OnPrewOpenPar_and_not_CurrentId_and_NextComma_or_NextColon();
	void OnRealNum();
	void OnReturn();
	void OnSemicolon();
	void OnStatic();
	void OnStr();
	void OnString();
	void OnThis();
	void OnType();
	void OnUsing();
	void OnWhile();
	void OnYield();

	void ParseExpr();

	void On_inExprStack__BinOp_OpenPar();
	void On_inExprStack__Expr_LABinOp_Expr_LABinOp_or_End_or_Comma_or_ClosePar_or_EOL_or_EOF();
	void On_inExprStack__Expr_RABinOp_Expr_LABinOp_or_End_or_Comma_or_ClosePar_or_EOL_or_EOF();
	void On_inExprStack__UnOp_Expr_End_or_Comma_or_ClosePar_or_EOL_or_EOF_or_not_OpenPar_or_RABinOp();
	void On_inExprStack__CallStart_ClosePar();
	void On_inExprStack__empty();
	void On_inExprStack__not_BinOp_OpenPar();
	void On_not_EOF_or_End();
	void On_not_EOF_or_End_or_OrNextComma();
	void On_not_EOL_or_EOF();

	Module* operator()(std::pair < std::vector<std::shared_ptr<token_::Token> >&, char const*> tokens);
private:
	std::vector<Parser__State> _stack{ Parser__State::module_statements };
	std::vector<std::shared_ptr<token_::Token>>* _tokens;

	std::vector<Node*> _exprs;
	std::vector<Body**> _bodies;
	std::vector<Node*> _statements;
	std::vector<std::vector<Node*>> _stack_of_exprs;
	VarDefinition* _announced_variable{ nullptr };
	Module* _module;
	bool _static_field{ false };

	size_t _idx{ 0 };
	size_t _size{ 0 };
};


#endif // INCLUDED_parser_fsm_GEN_H
