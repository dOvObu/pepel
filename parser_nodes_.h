#ifndef INCLUDED_PARSER_NODES_H
#define INCLUDED_PARSER_NODES_H
#include <memory>
#include <string>
#include "tokens_.h"
#include "ivisitor.h"

enum class Nd {
	USING,
	TYPE,
	FUNC,
	VAR_DEFINITION,

// Expressions
	LAMBDA_FUNC,
	ITE_EXPR,
	FW_EXPR,
	NUM,
	REAL_NUM,
	STRING,
	ID,
	THIS,
	CALL,

	// Binary Operators
	DOT,
	ADD,
	SUB,
	MUL,
	DIV,
	OR,
	AND,
	IN,
	SEQ,
	ADD_ASIGN,
	SUB_ASIGN,
	MUL_ASIGN,
	DIV_ASIGN,
	ASIGN,
	EQUAL,

	// Unary Operators
	PLUS,
	MINUS,
	NOT,

// Statements
	IF,
	FOR,
	BODY,
	WHILE,
	YIELD,
	RETURN,
	BREAK,
	CONTINUE,
	FOREACH,

	TOK,
	UNKNOWN,
};

#define AVis void accept(IVisitor& v) { v.visit(*this); }
#define Push Node::stack.push_back(std::shared_ptr<Node>(this))

struct Node  : token_::Token { Nd st{ Nd::UNKNOWN }; static std::vector<std::shared_ptr<Node>> pool; virtual void accept(IVisitor& v) = 0; virtual ~Node() = default; };
struct Using         : Node { Using()                  { st=Nd::USING;          Push; } std::vector<std::string> metaPath; bool hasPseudonim{ false }; std::string pseudonim; AVis };
struct Func          : Node { Func()                   { st=Nd::FUNC;           Push; } std::string id; std::vector<struct VarDefinition*> arguments; Body* body{ nullptr }; AVis };
struct Type          : Node { Type(char const*v):id(v) { st=Nd::TYPE;           Push; } std::string id; std::vector<std::string> metaPath; Type* parent{ nullptr }; bool HasParent() {return parent != nullptr;} std::vector<Func*> methods; std::vector<struct VarDefinition*> fields; std::vector<struct VarDefinition*> staticFields; static Type Int, Float, String; AVis };
struct VarDefinition : Node { VarDefinition(std::string& name):id(name)
                                                       { st=Nd::VAR_DEFINITION; Push; } std::string id; Node* val{ nullptr }; bool hasConcreteType{ false }; Type* type{ nullptr }; AVis };

// Expressions
struct LambdaFunc : Node { LambdaFunc()                { st=Nd::LAMBDA_FUNC; t=Tk::EXPR;       Push; } std::vector<Node*> arguments;  Node* body{ nullptr }; AVis };
struct IteExpr    : Node { IteExpr()                   { st=Nd::ITE_EXPR;    t=Tk::EXPR;       Push; } Node* prop{ nullptr }; Node* passed{ nullptr }; Node* fall{ nullptr }; AVis };
struct FwExpr     : Node { FwExpr()                    { st=Nd::FW_EXPR;     t=Tk::EXPR;       Push; } Node* itemOp{ nullptr }; Node* id{ nullptr }; Node* idxId{ nullptr }; Node* source{ nullptr }; Node* prop{ nullptr }; AVis };
struct RealNum    : Node { RealNum(double n):val(n)    { st=Nd::REAL_NUM;    t=Tk::EXPR;       Push; } double val{ 0.0 }; AVis };
struct Num        : Node { Num(long n):val(n)          { st=Nd::NUM;         t=Tk::EXPR;       Push; } long   val{  0  }; AVis };
struct String     : Node { String(std::string&v):val(v){ st=Nd::STRING;      t=Tk::EXPR;       Push; } std::string val; AVis };
struct Id         : Node { Id    (std::string&v):val(v){ st=Nd::ID;          t=Tk::EXPR;       Push; } std::string val; AVis };
struct Call       : Node { Call()                      { st=Nd::CALL;        t=Tk::EXPR;       Push; } Node* funcSource{ nullptr }; std::vector<Node*> arguments; AVis };
struct This       : Node { This()                      { st=Nd::THIS;        t=Tk::EXPR;       Push; } AVis };
struct EToken     : Node { EToken(Tk type)             { st=Nd::TOK;         t=type;           Push; } AVis };

// Binary Operators
struct BinOp : Node { BinOp(Nd nd, Tk tk) { st = nd; t = tk; Push; } Node *left{ nullptr }, *right{ nullptr }; AVis };
static BinOp* Dot      () { return new BinOp(Nd::DOT,       Tk::EXPR); }
static BinOp* Add      () { return new BinOp(Nd::ADD,       Tk::EXPR); }
static BinOp* Sub      () { return new BinOp(Nd::SUB,       Tk::EXPR); }
static BinOp* Mul      () { return new BinOp(Nd::MUL,       Tk::EXPR); }
static BinOp* Div      () { return new BinOp(Nd::DIV,       Tk::EXPR); }
static BinOp* Or       () { return new BinOp(Nd::OR ,       Tk::EXPR); }
static BinOp* And      () { return new BinOp(Nd::AND,       Tk::EXPR); }
static BinOp* In       () { return new BinOp(Nd::IN ,       Tk::EXPR); }
static BinOp* Asign    () { return new BinOp(Nd::ASIGN,     Tk::EXPR); }
static BinOp* Equal    () { return new BinOp(Nd::EQUAL,     Tk::EXPR); }
static BinOp* AddAsign () { return new BinOp(Nd::ADD_ASIGN, Tk::EXPR); }
static BinOp* SubAsign () { return new BinOp(Nd::SUB_ASIGN, Tk::EXPR); }
static BinOp* MulAsign () { return new BinOp(Nd::MUL_ASIGN, Tk::EXPR); }
static BinOp* DivAsign () { return new BinOp(Nd::DIV_ASIGN, Tk::EXPR); }
struct Sequence : Node { Sequence() { st = Nd::SEQ; t = Tk::EXPR; Push; } std::vector<Node*> items; AVis };

// Unary Operators
struct Plus  : Node { Plus()  { st=Nd::PLUS;  t=Tk::ADD; Push; } Node *right{ nullptr }; AVis };
struct Minus : Node { Minus() { st=Nd::MINUS; t=Tk::SUB; Push; } Node *right{ nullptr }; AVis };
struct Not   : Node { Not()   { st=Nd::NOT;   t=Tk::NOT; Push; } Node *right{ nullptr }; AVis };

// Statements
struct Body     : Node { Body()     { st=Nd::BODY;     Push; } std::vector<Node*> statements; AVis };
struct If       : Node { If()       { st=Nd::IF;       Push; } Node* prop{ nullptr }; Body* body{ nullptr }; Node* else_prop{ nullptr }; Body* else_body{ nullptr }; AVis };
struct For      : Node { For()      { st=Nd::FOR;      Push; } AVis };
struct While    : Node { While()    { st=Nd::WHILE;    Push; } Node* prop{ nullptr }; Body* body{ nullptr }; AVis };
struct Yield    : Node { Yield()    { st=Nd::YIELD;    Push; } bool to_break{ false }; AVis };
struct Return   : Node { Return()   { st=Nd::RETURN;   Push; } Node* e{ nullptr }; AVis };
struct Break    : Node { Break()    { st=Nd::BREAK;    Push; } AVis };
struct Continue : Node { Continue() { st=Nd::CONTINUE; Push; } AVis };
struct Foreach  : Node { Foreach()  { st=Nd::FOREACH;  Push; } Id* it{ nullptr }; Id* idx{ nullptr }; Node* range{ nullptr }; std::vector<Node*> body; AVis };

struct Module : Node
{
	std::string name;
	std::vector<Using*> dependecies;
	std::vector<Func*> functions;
	std::vector<Type*> types;

	Module(std::string name) { this->name = name.substr(0, name.size() - 4); Push; }
	AVis
};

#undef AVis
#undef Push

#endif // ! INCLUDED_PARSER_NODES_H
