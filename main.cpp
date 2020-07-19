#include <iostream>
#include "final_state_machine_generator.h"
#include "tokenizer_fsm_gen.h"
#include "parser_fsm_gen.h"
#include "interpreter.h"
#include "printer.h"
#include <string>
#include <vector>


void printNode(Interpreter& ctx, Node* a)
{
   switch (a->st)
   {
   case Nd::NUM:
      std::cout << reinterpret_cast<Num*>(a)->val << std::endl;
      break;
   case Nd::REAL_NUM:
      std::cout << reinterpret_cast<RealNum*>(a)->val << std::endl;
      break;
   case Nd::STRING:
      std::cout << reinterpret_cast<String*>(a)->val << std::endl;
      break;
   case Nd::ID:
   {
      auto t3 = ctx.GetDefOf(reinterpret_cast<Id*>(a)->val.c_str());
      if (std::get<0>(t3) != nullptr && std::get<0>(t3)->val != nullptr)
      {
         printNode(ctx, std::get<0>(t3)->val);
      }
   }  break;
   default:
      break;
   }
}

void print(Interpreter& ctx, size_t n)
{
   auto sz = ctx.expr_stack.size();
   for (int i = sz - n; sz > i; ++i)
   {
      auto a = ctx.expr_stack[i];
      printNode(ctx, a);
   }
}


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
      i.native_f["print"] = print;
      res->accept(i);
      std::cin.get();
      Node::pool.clear();
      Node::stack.clear();
   }
	std::cout << std::endl;

	return 0;
}