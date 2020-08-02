#include <iostream>
#include "final_state_machine_generator.h"
#include "tokenizer_fsm_gen.h"
#include "parser_fsm_gen.h"
#include "interpreter.h"
#include "printer.h"
#include <string>
#include <vector>



int main(int argc, char *argv[])
{
   std::string src = "source.txt";

   if (argc > 1)
      src = argv[1];

   while (true)
   {
      system("cls");
	   auto res = ParserFsm{}(TokenizerFsm{}(src.c_str()));
	   //Printer p;
	   //res->accept(p);
      //std::cout << std::endl;

      Interpreter i;
      res->accept(i);
      std::cin.get();

      Node::pool.clear();
      Node::stack.clear();
   }
	std::cout << std::endl;

	return 0;
}