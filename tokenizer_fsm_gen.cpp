#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "tokenizer_fsm_gen.h"
#define Current (*_src)[_idx]
#define Next (*_src)[_idx + 1]
#define RmEOLs while (!Token::stack.empty() && Token::stack.back()->t == Tk::EOL) Token::stack.pop_back()
using namespace token_;

std::pair<std::vector<std::shared_ptr<Token>>*, char const*> TokenizerFsm::operator()(char const* file_name)
{
   std::ifstream file(file_name);
   std::stringstream st;
   st << file.rdbuf();
   file.close();
   std::string text_from_file = st.str();

   _size = text_from_file.size();
   _src = &text_from_file;
   Token::StartModule();

   while (_size > _idx)
   {
      On_();
      if (Current == '\\') OnBackslash();
      if (Current == '0' || Current == '1' || Current == '2' || Current == '3' || Current == '4' || Current == '5' || Current == '6' || Current == '7' || Current == '8' || Current == '9') OnDigit();
      if (Current == '.') OnDot();
      if (Current == 'a' || Current == 'b' || Current == 'c' || Current == 'd' || Current == 'e' || Current == 'f' || Current == 'g' || Current == 'h' || Current == 'i' || Current == 'j' || Current == 'k' || Current == 'l' || Current == 'm' || Current == 'n' || Current == 'o' || Current == 'p' || Current == 'q' || Current == 'r' || Current == 's' || Current == 't' || Current == 'u' || Current == 'v' || Current == 'w' || Current == 'x' || Current == 'y' || Current == 'z'
      ||  Current == 'A' || Current == 'B' || Current == 'C' || Current == 'D' || Current == 'E' || Current == 'F' || Current == 'G' || Current == 'H' || Current == 'I' || Current == 'J' || Current == 'K' || Current == 'L' || Current == 'M' || Current == 'N' || Current == 'O' || Current == 'P' || Current == 'Q' || Current == 'R' || Current == 'S' || Current == 'T' || Current == 'U' || Current == 'V' || Current == 'W' || Current == 'X' || Current == 'Y' || Current == 'Z') OnLetter();
      if (Current == '_') OnUnderscore();
      if (Current == '\n') OnNewLine();
	  if (Current == '"') OnQuotation();
	  else if (_state == TokenizerFsm::Tokenizer__State::string) { _buffer += Current; ++_idx; }
      if (Current == ' ') OnSpace();
      if (Current == '\t') OnTab();
      if (Current == '.' || Current == '+' || Current == '-' || Current == '*' || Current == '/' || Current == ',' || Current == '!' || Current == ':' || Current == ';' || Current == '|' || Current == '?' || Current == '%' || Current == '\\' || Current == '(' || Current == ')' || Current == '=') OnSymbol();
   }
   new Eof();

   _src = nullptr;
   return { &Token::stack, file_name };
}
void TokenizerFsm::NewId()
{
   if      (_buffer == "using") new Using();
   else if (_buffer == "and"  ) new And();
   else if (_buffer == "or"   ) new Or();
   else if (_buffer == "type" ) new Type();
   else if (_buffer == "this" ) new This();
   else if (_buffer == "if"   ) new If();
   else if (_buffer == "then" ) new Then();
   else if (_buffer == "else" ) {RmEOLs; new Else();}
   else if (_buffer == "elif" ) {RmEOLs; new Elif();}
   else if (_buffer == "for"  ) new For();
   else if (_buffer == "while") new While();
   else if (_buffer == "in"   ) new In();
   else if (_buffer == "from" ) new From();
   else if (_buffer == "where") new Where();
   else if (_buffer == "as"   ) new As();
   else if (_buffer == "Str"  ) new Str();
   else if (_buffer == "Int"  ) new Int();
   else if (_buffer == "Float") new Float();
   else if (_buffer == "this" ) new This();
   else if (_buffer == "break") new Break();
   else if (_buffer == "yield") new Yield();
   else if (_buffer == "return" ) new Return();
   else if (_buffer == "native" ) new Native();
   else if (_buffer == "static" ) new Static();
   else if (_buffer == "continue" ) new Continue();
   else (new Id())->val = _buffer;
}
void TokenizerFsm::OnBackslash() {
	switch(_state) {
	case TokenizerFsm::Tokenizer__State::string: _state = TokenizerFsm::Tokenizer__State::concrete_symbol; { _buffer += Current; ++_idx; } break;
	default: break;
	}
}
void TokenizerFsm::OnDigit() {
	switch(_state) {
	case TokenizerFsm::Tokenizer__State::id: _state = TokenizerFsm::Tokenizer__State::id;          { _buffer += Current; ++_idx; } break;
	case Tokenizer__State::number:           _state = TokenizerFsm::Tokenizer__State::number;      { _buffer += Current; ++_idx; } break;
	case Tokenizer__State::real_number:      _state = TokenizerFsm::Tokenizer__State::real_number; { _buffer += Current; ++_idx; } break;
	case Tokenizer__State::string:           _state = TokenizerFsm::Tokenizer__State::string;      { _buffer += Current; ++_idx; } break;
	case Tokenizer__State::pass:             _state = TokenizerFsm::Tokenizer__State::number;      { _buffer += Current; ++_idx; } break;
	default: break;
	}
}
void TokenizerFsm::OnDot() {
	switch(_state) {
	case TokenizerFsm::Tokenizer__State::number          : _state = TokenizerFsm::Tokenizer__State::real_number; { _buffer += Current; ++_idx; } break;
	default: break;
	}
}
void TokenizerFsm::OnLetter() {
	switch(_state) {
	case TokenizerFsm::Tokenizer__State::id:     _state = TokenizerFsm::Tokenizer__State::id;     { _buffer += Current; ++_idx; } break;
	case TokenizerFsm::Tokenizer__State::string: _state = TokenizerFsm::Tokenizer__State::string; { _buffer += Current; ++_idx; } break;
	case TokenizerFsm::Tokenizer__State::pass:   _state = TokenizerFsm::Tokenizer__State::id;     { _buffer += Current; ++_idx; } break;
	default: break;
	}
}
void TokenizerFsm::OnNewLine() {
	switch(_state) {
    case TokenizerFsm::Tokenizer__State::string:      _state = TokenizerFsm::Tokenizer__State::string;   { _buffer += Current; ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::id:          _state = TokenizerFsm::Tokenizer__State::new_line; { NewId();                                           _buffer.clear(); } break;
    case TokenizerFsm::Tokenizer__State::number:      _state = TokenizerFsm::Tokenizer__State::new_line; { (new Number()) ->val = std::atol(_buffer.c_str()); _buffer.clear(); } break;
    case TokenizerFsm::Tokenizer__State::real_number: _state = TokenizerFsm::Tokenizer__State::new_line; { (new RealNum())->val = std::atof(_buffer.c_str()); _buffer.clear(); } break;
    case TokenizerFsm::Tokenizer__State::pass:        _state = TokenizerFsm::Tokenizer__State::new_line; { } break;
	default: break;
	}
}
void TokenizerFsm::OnQuotation() {
	switch(_state) {
    case TokenizerFsm::Tokenizer__State::pass: _state = TokenizerFsm::Tokenizer__State::string; { ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::string: _state = TokenizerFsm::Tokenizer__State::pass; { (new String())->val = _buffer; _buffer.clear(); ++_idx; } break;
	default: break;
	}
}
void TokenizerFsm::OnSpace() {
	switch(_state) {
    case TokenizerFsm::Tokenizer__State::id:          _state = TokenizerFsm::Tokenizer__State::pass;   { NewId();                                           _buffer.clear(); ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::number:      _state = TokenizerFsm::Tokenizer__State::pass;   { (new Number()) ->val = std::atol(_buffer.c_str()); _buffer.clear(); ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::real_number: _state = TokenizerFsm::Tokenizer__State::pass;   { (new RealNum())->val = std::atof(_buffer.c_str()); _buffer.clear(); ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::string:      _state = TokenizerFsm::Tokenizer__State::string; { _buffer += Current; ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::pass:        _state = TokenizerFsm::Tokenizer__State::pass;   { ++_idx; } break;
	default: break;
	}
}
void TokenizerFsm::OnSymbol() {
	switch(_state) {
    case TokenizerFsm::Tokenizer__State::concrete_symbol: _state = TokenizerFsm::Tokenizer__State::string; { _buffer += Current; ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::string:          _state = TokenizerFsm::Tokenizer__State::string; { _buffer += Current; ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::id:              _state = TokenizerFsm::Tokenizer__State::symbol;                     { NewId();                                           _buffer.clear(); } break;
    case TokenizerFsm::Tokenizer__State::number:          _state = TokenizerFsm::Tokenizer__State::symbol; if (Current != '.') { (new Number()) ->val = std::atol(_buffer.c_str()); _buffer.clear(); } else { _state = TokenizerFsm::Tokenizer__State::real_number; } break;
    case TokenizerFsm::Tokenizer__State::real_number:     _state = TokenizerFsm::Tokenizer__State::symbol; if (Current != '.') { (new RealNum())->val = std::atof(_buffer.c_str()); _buffer.clear(); } else { _buffer += Current; ++_idx;                           } break;
    case TokenizerFsm::Tokenizer__State::pass:            _state = TokenizerFsm::Tokenizer__State::symbol; { } break;
	default: break;
	}
}
void TokenizerFsm::OnTab() {
	switch(_state) {
    case TokenizerFsm::Tokenizer__State::id:          _state = TokenizerFsm::Tokenizer__State::pass;   { NewId();                                           _buffer.clear(); ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::number:      _state = TokenizerFsm::Tokenizer__State::pass;   { (new Number()) ->val = std::atol(_buffer.c_str()); _buffer.clear(); ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::real_number: _state = TokenizerFsm::Tokenizer__State::pass;   { (new RealNum())->val = std::atof(_buffer.c_str()); _buffer.clear(); ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::string:      _state = TokenizerFsm::Tokenizer__State::string; { _buffer += Current; ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::pass:        _state = TokenizerFsm::Tokenizer__State::pass;   { ++_idx; } break;
	default: break;
	}
}
void TokenizerFsm::OnUnderscore() {
	switch(_state) {
    case TokenizerFsm::Tokenizer__State::id:          _state = TokenizerFsm::Tokenizer__State::id;          { _buffer += Current; ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::pass:        _state = TokenizerFsm::Tokenizer__State::id;          { _buffer += Current; ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::string:      _state = TokenizerFsm::Tokenizer__State::string;      { _buffer += Current; ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::number:      _state = TokenizerFsm::Tokenizer__State::number;      { ++_idx; } break;
    case TokenizerFsm::Tokenizer__State::real_number: _state = TokenizerFsm::Tokenizer__State::real_number; { ++_idx; } break;
	default: break;
	}
}
void TokenizerFsm::On_() {
	switch(_state) {
    case TokenizerFsm::Tokenizer__State::new_line: _state = TokenizerFsm::Tokenizer__State::pass; {
      if (!Token::stack.empty())
      {
         Tk back = Token::stack.back()->t;
         if (back == Tk::DIV) { Token::stack.pop_back(); new End(); }
         if (back != Tk::EOL &&
			 back != Tk::ADD && back != Tk::ADD_ASIGN &&
			 back != Tk::MUL && back != Tk::MUL_ASIGN &&
			 back != Tk::SUB && back != Tk::SUB_ASIGN &&
			 back != Tk::ASIGN && back != Tk::DIV_ASIGN &&
			 back != Tk::COLON && back != Tk::EQUAL && back != Tk::IF &&
			 back != Tk::AS && back != Tk::IN && back != Tk::FOR &&
			 back != Tk::YIELD && back != Tk::RETURN &&
			 back != Tk::ENUMERABLE) new Eol();
      }
      ++_idx;
    } break;
    case TokenizerFsm::Tokenizer__State::symbol:   _state = TokenizerFsm::Tokenizer__State::pass; {
      if      (Current == '(')  new OpnPar();
      else if (Current == ')')  new ClsPar();
      else if (Current == '\\') new Lambda();
	  else if (Current == ':') { RmEOLs; new Colon(); }
	  else if (Current == ',') { RmEOLs; new Comma(); }
	  else if (Current == ';') { bool isEnd = !Token::stack.empty() && Token::stack.back()->t == Tk::EOL; RmEOLs; if (isEnd) new End(); else new Eol(); }
	  else if (Current == '.') { RmEOLs; new Dot(); }
      else if (Current == '+') { RmEOLs; if (Next == '=') { new AddAsign(); ++_idx; } else new Add(); }
      else if (Current == '-') { RmEOLs; if (Next == '=') { new SubAsign(); ++_idx; } else new Sub(); }
      else if (Current == '*') { RmEOLs; if (Next == '=') { new MulAsign(); ++_idx; } else new Mul(); }
      else if (Current == '/') { RmEOLs; if (Next == '=') { new DivAsign(); ++_idx; } else if (_idx == _size) new End(); else new Div(); }
      else if (Current == '=') { RmEOLs; if (Next == '=') { new Equal();    ++_idx; } else new Asign(); }
      ++_idx;
	} break;
	default: break;
	}
}
#undef Current
#undef Next
#undef RmEOLs
