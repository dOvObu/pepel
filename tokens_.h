#ifndef TOKENS_H
#define TOKENS_H
#include <vector>
#include <memory>
#include <string>

enum class Tk {
   USING,
   ID,
   STRING,
   EOL,
   EOF_,
   TYPE,
   OPN_PAR,
   CLS_PAR,
   COLON,
   SEMICOLON,
   LAMBDA,
   END,
   COMMA,
   DOUBLE_DOT,
   DOT,
   ADD,
   SUB,
   MUL,
   DIV,
   OR,
   AND,
   NOT,
   ADD_ASIGN,
   SUB_ASIGN,
   MUL_ASIGN,
   DIV_ASIGN,
   ASIGN,
   EQUAL,
   THIS,
   EXPR,
   RMVBL,
   IF,
   THEN,
   ELSE,
   ELIF,
   FOR,
   WHILE,
   IN,
   FROM,
   WHERE,
   AS,
   NUM,
   REAL_NUM,
   STR,
   INT,
   FLOAT,
   ENUMERABLE,
   NATIVE,
   STATIC,
   YIELD,
   RETURN,
   BREAK,
   CONTINUE,
   CALL_START,

   UNKNOWN,
};
#define Push Token::stack.push_back(std::shared_ptr<Token>(this))

namespace token_
{
struct Token { Tk t{ Tk::UNKNOWN }; static std::vector<std::shared_ptr<Token>> stack; static void StartModule() { stack.push_back(std::shared_ptr<Token>(&START, [](auto _) {})); } private: static Token START; };
struct Using      : public Token { Using     () { Token::t = Tk::USING;     Push; } };
struct Id         : public Token { Id        () { Token::t = Tk::ID;        Push; } std::string val; };
struct String     : public Token { String    () { Token::t = Tk::STRING;    Push; } std::string val; };
struct Eol        : public Token { Eol       () { Token::t = Tk::EOL;       Push; } };
struct Eof        : public Token { Eof       () { Token::t = Tk::EOF_;      Push; } };
struct Type       : public Token { Type      () { Token::t = Tk::TYPE;      Push; } };
struct OpnPar     : public Token { OpnPar    () { Token::t = Tk::OPN_PAR;   Push; } };
struct ClsPar     : public Token { ClsPar    () { Token::t = Tk::CLS_PAR;   Push; } };
struct Colon      : public Token { Colon     () { Token::t = Tk::COLON;     Push; } };
struct Semicolon  : public Token { Semicolon () { Token::t = Tk::SEMICOLON; Push; } };
struct Lambda     : public Token { Lambda    () { Token::t = Tk::LAMBDA;    Push; } };
struct End        : public Token { End       () { Token::t = Tk::END;       Push; } };
struct Comma      : public Token { Comma     () { Token::t = Tk::COMMA;     Push; } };
struct DoubleDot  : public Token { DoubleDot () { Token::t = Tk::DOUBLE_DOT;Push; } };
struct Dot        : public Token { Dot       () { Token::t = Tk::DOT;       Push; } };
struct Add        : public Token { Add       () { Token::t = Tk::ADD;       Push; } };
struct Sub        : public Token { Sub       () { Token::t = Tk::SUB;       Push; } };
struct Mul        : public Token { Mul       () { Token::t = Tk::MUL;       Push; } };
struct Div        : public Token { Div       () { Token::t = Tk::DIV;       Push; } };
struct And        : public Token { And       () { Token::t = Tk::AND;       Push; } };
struct Or         : public Token { Or        () { Token::t = Tk::OR;        Push; } };
struct AddAsign   : public Token { AddAsign  () { Token::t = Tk::ADD_ASIGN; Push; } };
struct SubAsign   : public Token { SubAsign  () { Token::t = Tk::SUB_ASIGN; Push; } };
struct MulAsign   : public Token { MulAsign  () { Token::t = Tk::MUL_ASIGN; Push; } };
struct DivAsign   : public Token { DivAsign  () { Token::t = Tk::DIV_ASIGN; Push; } };
struct Asign      : public Token { Asign     () { Token::t = Tk::ASIGN;     Push; } };
struct Equal      : public Token { Equal     () { Token::t = Tk::EQUAL;     Push; } };
struct This       : public Token { This      () { Token::t = Tk::THIS;      Push; } };
struct If         : public Token { If        () { Token::t = Tk::IF;        Push; } };
struct Then       : public Token { Then      () { Token::t = Tk::THEN;      Push; } };
struct Else       : public Token { Else      () { Token::t = Tk::ELSE;      Push; } };
struct Elif       : public Token { Elif      () { Token::t = Tk::ELIF;      Push; } };
struct For        : public Token { For       () { Token::t = Tk::FOR;       Push; } };
struct While      : public Token { While     () { Token::t = Tk::WHILE;     Push; } };
struct In         : public Token { In        () { Token::t = Tk::IN;        Push; } };
struct From       : public Token { From      () { Token::t = Tk::FROM;      Push; } };
struct Where      : public Token { Where     () { Token::t = Tk::WHERE;     Push; } };
struct As         : public Token { As        () { Token::t = Tk::AS;        Push; } };
struct RealNum    : public Token { RealNum   () { Token::t = Tk::REAL_NUM;  Push; } double val{ 0.0 }; };
struct Number     : public Token { Number    () { Token::t = Tk::NUM;       Push; } long   val{  0  }; };
struct Str        : public Token { Str       () { Token::t = Tk::STR;       Push; } };
struct Int        : public Token { Int       () { Token::t = Tk::INT;       Push; } };
struct Float      : public Token { Float     () { Token::t = Tk::FLOAT;     Push; } };
struct Enumerable : public Token { Enumerable() { Token::t = Tk::ENUMERABLE;Push; } };
struct Native     : public Token { Native    () { Token::t = Tk::NATIVE;    Push; } };
struct Static     : public Token { Static    () { Token::t = Tk::STATIC;    Push; } };
struct Yield      : public Token { Yield     () { Token::t = Tk::YIELD;     Push; } };
struct Return     : public Token { Return    () { Token::t = Tk::RETURN;    Push; } };
struct Break      : public Token { Break     () { Token::t = Tk::BREAK;     Push; } };
struct Continue   : public Token { Continue  () { Token::t = Tk::CONTINUE;  Push; } };

}
#undef Push
#endif // ! TOKENS_H
