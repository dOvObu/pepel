#include <iostream>
#include "final_state_machine_generator.h"
#include "tokenizer_fsm_gen.h"
#include "parser_fsm_gen.h"
#include "printer.h"
#include <vector>


int main()
{
	auto res = ParserFsm{}(TokenizerFsm{}("source.txt"));
	//std::cout << "Module '" << res->name << '\'' << std::endl;
	//std::cout << "include size [" << res->dependecies.size() << ']' << std::endl;
	//std::cout << "func size [" << res->functions.size() << ']' << std::endl;
	//std::cout << "type size [" << res->types.size() << ']' << std::endl;
	Printer p;
	res->functions[0]->accept(p);
	std::cout << std::endl;
	//std::cin.get();

	return 0;
}