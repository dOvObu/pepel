#ifndef INCLUDED_AST_PRINTER_H
#define INCLUDED_AST_PRINTER_H
#include "ivisitor.h"
#include "parser_nodes_.h"
#include <iostream>

struct AstPrinter : public IVisitor
{
	void visit(Using& u) override {
		std::cout << "using ";
		bool first = false;
		for (auto& it : u.metaPath)
		{
			if (!first) std::cout << '.';
			else first = true;
			std::cout << it
		}
		if (u.hasPseudonim)
		{
			std::cout << " as " << u.pseudonim;
		}
		std::cout << std::endl;
	}
	virtual void visit(Type& r) = 0;
	virtual void visit(Func& r) = 0;
	virtual void visit(VarDefinition& r) = 0;
	virtual void visit(LambdaFunc&) = 0;
	virtual void visit(IteExpr&) = 0;
	virtual void visit(Num&) = 0;
	virtual void visit(RealNum&) = 0;
	virtual void visit(String&) = 0;
	virtual void visit(Id&) = 0;
	virtual void visit(This&) = 0;
	virtual void visit(Call&) = 0;
	virtual void visit(Dot&) = 0;
	virtual void visit(Add&) = 0;
	virtual void visit(Sub&) = 0;
	virtual void visit(Mul&) = 0;
	virtual void visit(Div&) = 0;
	virtual void visit(Or&) = 0;
	virtual void visit(And&) = 0;
	virtual void visit(In&) = 0;
	virtual void visit(AddAsign&) = 0;
	virtual void visit(SumAsign&) = 0;
	virtual void visit(MulAsign&) = 0;
	virtual void visit(DivAsign&) = 0;
	virtual void visit(Asign&) = 0;
	virtual void visit(Equal&) = 0;
	virtual void visit(Plus&) = 0;
	virtual void visit(Minus&) = 0;
	virtual void visit(Not&) = 0;
	virtual void visit(Body&) = 0;
	virtual void visit(If&) = 0;
	virtual void visit(For&) = 0;
	virtual void visit(While&) = 0;
	virtual void visit(Yield&) = 0;
	virtual void visit(Return&) = 0;
	virtual void visit(Break&) = 0;
	virtual void visit(Continue&) = 0;
	virtual void visit(Foreach&) = 0;
	virtual void visit(Module&) = 0;
};

#endif // ! INCLUDED_AST_PRINTER_H
