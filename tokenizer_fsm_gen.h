#ifndef INCLUDED_tokenizer_fsm_GEN_H
#define INCLUDED_tokenizer_fsm_GEN_H
#include <vector>
#include <memory>
#include "tokens_.h"



struct TokenizerFsm
{
	enum Tokenizer__State {
		pass,
		concrete_symbol,
		id,
		new_line,
		number,
		real_number,
		string,
		symbol,
	};

	unsigned State() { return _state; }
	void NewId();

	void OnBackslash();
	void OnDigit();
	void OnDot();
	void OnLetter();
	void OnNewLine();
	void OnQuotation();
	void OnSpace();
	void OnSymbol();
	void OnTab();
	void OnUnderscore();
	void On_();

	std::pair<std::vector<std::shared_ptr<token_::Token>>&, char const*> operator()(char const* file_name);
private:
	unsigned _state{ 0 };
	size_t _idx{ 0 };
	size_t _size{ 0 };
	std::string* _src{ nullptr };
	std::string _buffer;
};


#endif // INCLUDED_tokenizer_fsm_GEN_H
