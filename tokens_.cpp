#include "tokens_.h"

std::vector<std::shared_ptr<token_::Token>> token_::Token::stack;
token_::Token token_::Token::START{ Tk::CALL_START };
