#ifndef INCLUDED_PARSER_NODES_H
#define INCLUDED_PARSER_NODES_H
#include <iostream>
#include <memory>
#include <string>
#include <map>
#include "tokens_.h"
#include "ivisitor.h"

enum class Nd {
	USING,
	TYPE,
	TYPE_SEQ,
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

struct Context
{
   std::map<std::string, struct Type*> types;
   std::map<std::string, struct Func*> funcs;
   std::map<std::string, struct VarDefinition*> vars;
   bool root{ false };
};

struct Node  : token_::Token { Nd st{ Nd::UNKNOWN }; static std::vector<std::shared_ptr<Node>> pool; virtual void accept(IVisitor& v) = 0; virtual ~Node() = default; };
struct Using         : Node   { Using()                  { st=Nd::USING;          Push; } std::vector<std::string> metaPath; bool hasPseudonim{ false }; std::string pseudonim; AVis };
struct Type          : Node   { Type(char const*v,bool push=true):id(v) { st=Nd::TYPE; t=Tk::EXPR; if(push) { Push; } } std::string id; std::vector<std::string> metaPath; Type* parent{ nullptr }; bool HasParent() {return parent != nullptr;} virtual std::string ToStr() { return id; } std::vector<Func*> methods; std::vector<struct VarDefinition*> fields; std::vector<struct VarDefinition*> staticFields; std::map<std::string, Func*> dmethods; std::map<std::string, struct VarDefinition*> dfields; std::map<std::string, struct VarDefinition*> dstaticFields; Context* context{ nullptr }; static Type Int, Float, String, Void; AVis };
struct TypeOp        : Type   { TypeOp(char const*v):Type(v,false) { st=Nd::TYPE; Push; } Type* left{ nullptr }; Type* right{ nullptr }; std::string ToStr() override { return '(' + left->ToStr() + id + right->ToStr() + ')'; } AVis };
struct TType         : Type   { TType():Type("?",false)  { st=Nd::TYPE; std::cerr << "WARNING! Not concrete types are not implemented or broken\n"; Push; } Type* left{ nullptr }; std::vector<Node*> subscribers; std::string ToStr() override { return id; } AVis };
struct TypeSeq       : Type   { TypeSeq(char const*v):Type(v,false) { st=Nd::TYPE_SEQ; Push; } std::vector<Type*> seq; std::string ToStr() override { std::string s{ '(' }; bool first = true; for (auto item : seq) { if (first) { first = false; } else { s += id; } s += item->ToStr(); } s.push_back(')'); return s; } AVis };
struct Func          : Node   { Func()                   { st=Nd::FUNC;           Push; } std::string id; std::vector<struct VarDefinition*> arguments; Body* body{ nullptr }; bool is_native{ false }; TypeOp* type{ new TypeOp("->") }; Context* context{ nullptr }; AVis };
struct VarDefinition : Node   { VarDefinition(std::string& name):id(name)
                                                       { st=Nd::VAR_DEFINITION; Push; } VarDefinition():id(""){} std::string id; Node* val{ nullptr }; bool hasConcreteType{ false }; Type* type{ nullptr }; AVis };

// Expressions
struct LambdaFunc : Node { LambdaFunc()                { st=Nd::LAMBDA_FUNC; t=Tk::EXPR;       Push; } std::vector<Node*> arguments;  Node* body{ nullptr }; Type* type{ new TypeOp("->") };  AVis };
struct IteExpr    : Node { IteExpr()                   { st=Nd::ITE_EXPR;    t=Tk::EXPR;       Push; } Node* prop{ nullptr }; Node* passed{ nullptr }; Node* fall{ nullptr }; Type* type{ nullptr };  AVis };
struct FwExpr     : Node { FwExpr()                    { st=Nd::FW_EXPR;     t=Tk::EXPR;       Push; } Node* itemOp{ nullptr }; Node* id{ nullptr }; Node* idxId{ nullptr }; Node* source{ nullptr }; Node* prop{ nullptr }; AVis };
struct RealNum    : Node { RealNum(double n)    :val(n){ st=Nd::REAL_NUM;    t=Tk::EXPR;       Push; } double val{ 0.0 }; AVis        RealNum () {}          Type* type{ &Type::Float  }; RealNum*removable(double v){ val=v; t=Tk::RMVBL; st=Nd::REAL_NUM; return this; } };
struct Num        : Node { Num(long n)          :val(n){ st=Nd::NUM;         t=Tk::EXPR;       Push; } long   val{  0  }; AVis        Num     () {}          Type* type{ &Type::Int    }; Num*    removable(long   v){ val=v; t=Tk::RMVBL; st=Nd::NUM; return this; } };
struct String     : Node { String(std::string&v):val(v){ st=Nd::STRING;      t=Tk::EXPR;       Push; } std::string val;   AVis        String  () {}          Type* type{ &Type::String }; String* removable(std::string const& v){ val=v; t=Tk::RMVBL; st=Nd::STRING; return this; } };
struct Id         : Node { Id    (std::string&v):val(v){ st=Nd::ID;          t=Tk::EXPR;       Push; } std::string val;   Type* type{ nullptr }; AVis };
struct Call       : Node { Call()                      { st=Nd::CALL;        t=Tk::EXPR;       Push; } Node* funcSource{ nullptr }; std::vector<Node*> arguments; std::vector<std::string> arg_names; AVis };
struct This       : Node { This()                      { st=Nd::THIS;        t=Tk::EXPR;       Push; } AVis };
struct EToken     : Node { EToken(Tk token_type)       { st=Nd::TOK;         t=token_type;     Push; } AVis };

// Binary Operators
struct BinOp : Node { BinOp(Nd nd, Tk tk) { st = nd; t = tk; Push; } Node *left{ nullptr }, *right{ nullptr }; Type* type{ nullptr }; AVis };
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
static TypeOp*   Arrow    () { return new TypeOp("->"); }
static TypeSeq*  Deckard  () { return new TypeSeq("*"); }
struct Sequence : Node { Sequence() { st = Nd::SEQ; t = Tk::EXPR; Push; } std::vector<Node*> items; Type* type{ nullptr }; AVis };

// Unary Operators
struct Plus  : Node { Plus()  { st=Nd::PLUS;  t=Tk::ADD; Push; } Node *right{ nullptr }; Type* type{ nullptr }; AVis };
struct Minus : Node { Minus() { st=Nd::MINUS; t=Tk::SUB; Push; } Node *right{ nullptr }; Type* type{ nullptr }; AVis };
struct Not   : Node { Not()   { st=Nd::NOT;   t=Tk::NOT; Push; } Node *right{ nullptr }; Type* type{ nullptr }; AVis };

// Statements
struct Body     : Node { Body()     { st=Nd::BODY;     Push; } std::vector<Node*> statements; AVis };
struct If       : Node { If()       { st=Nd::IF;       Push; } Node* prop{ nullptr }; Body* body{ nullptr }; Node* else_body{ nullptr }; AVis };
struct For      : Node { For()      { st=Nd::FOR;      Push; } AVis };
struct While    : Node { While()    { st=Nd::WHILE;    Push; } Node* prop{ nullptr }; Body* body{ nullptr }; AVis };
struct Yield    : Node { Yield()    { st=Nd::YIELD;    Push; } bool to_break{ false }; AVis };
struct Return   : Node { Return()   { st=Nd::RETURN;   Push; } Node* e{ nullptr }; AVis };
struct Break    : Node { Break()    { st=Nd::BREAK;    Push; } AVis };
struct Continue : Node { Continue() { st=Nd::CONTINUE; Push; } AVis };
struct Foreach  : Node { Foreach() { st = Nd::FOREACH;  Push; } Id* it{ nullptr }; Id* idx{ nullptr }; Node* range{ nullptr }; Body* body{ nullptr }; AVis };


struct Module : Node
{
	std::string name;
	std::vector<Using*> dependecies;
	std::vector<Func*> functions;
	std::vector<Type*> types;
	std::map<std::string, Func*> dfunctions;
	std::map<std::string, Type*> dtypes;

   Context context;

	Module(std::string name) { this->name = name.substr(0, name.size() - 4); Push; }
	AVis
};

#undef AVis
#undef Push

#endif // ! INCLUDED_PARSER_NODES_H
