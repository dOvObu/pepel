#ifndef INCLUDED_IVISITOR_H
#define INCLUDED_IVISITOR_H


struct IVisitor {
	virtual void visit(struct Using&) = 0;
	virtual void visit(struct Type&) = 0;
	virtual void visit(struct Func&) = 0;
	virtual void visit(struct VarDefinition&) = 0;
	virtual void visit(struct LambdaFunc&) = 0;
	virtual void visit(struct IteExpr&) = 0;
	virtual void visit(struct FwExpr&) = 0;
	virtual void visit(struct Num&) = 0;
	virtual void visit(struct RealNum&) = 0;
	virtual void visit(struct String&) = 0;
	virtual void visit(struct Id&) = 0;
	virtual void visit(struct This&) = 0;
	virtual void visit(struct Call&) = 0;
	virtual void visit(struct BinOp&) = 0;
	virtual void visit(struct Sequence&) = 0;
	virtual void visit(struct Plus&) = 0;
	virtual void visit(struct Minus&) = 0;
	virtual void visit(struct Not&) = 0;
	virtual void visit(struct Body&) = 0;
	virtual void visit(struct If&) = 0;
	virtual void visit(struct For&) = 0;
	virtual void visit(struct While&) = 0;
	virtual void visit(struct Yield&) = 0;
	virtual void visit(struct Return&) = 0;
	virtual void visit(struct Break&) = 0;
	virtual void visit(struct Continue&) = 0;
	virtual void visit(struct Foreach&) = 0;
	virtual void visit(struct Module&) = 0;

	virtual void visit(struct EToken&) = 0;
};

#endif // ! INCLUDED_IVISITOR_H
