#include <algorithm>
#include <vector>
#include "final_state_machine_generator.h"



State::State(char const* name)
{
   if (!_pool.count(name))
   {
      _pool.insert({ name, std::shared_ptr<StateData>(_stateData = new StateData(name)) });
   }
   else
   {
      _stateData = _pool[name].get();
   }
}


State& State::On(char const* key, char const* state)
{
   return On(key, StStack::ReplaceWith, { state });
}

State& State::On(char const* key, StStack action)
{
	return On(key, action, _stateData->_name.c_str());
}

State& State::On(char const* key, StStack action, char const* state)
{
	if (_stateData != nullptr)
	{
		auto& transition = _stateData->_transitions[key];
		transition.first = action;
		transition.second = State{ state }._stateData;
	}
	return *this;
}


void State::Goto(char const* key)
{
   _stateData = _stateData->_transitions[key].second;
}


void State::WriteTo(std::string name, char const* path)
{
   std::vector<std::string> keys;
   keys.reserve(_pool.size());
   for (auto& p : _pool) keys.push_back(p.first);
   std::sort(std::begin(keys), std::end(keys));

   // H
   std::ofstream h(std::string(path) + "_gen.h");
   //       OnCondition, v{from state, to state}
   struct Arc
   {
	   std::string from, to;
	   StStack by_action{ StStack::ReplaceWith };
   };
   std::map<std::string, std::vector<Arc>> transitions;
   for (auto& key : keys)
   {
      for (auto& transition : _pool[key]->_transitions)
      {
         if (transitions.count(transition.first))
         {
            transitions[transition.first].push_back({ key, transition.second.second->_name, transition.second.first });
         }
         else
         {
            (transitions[transition.first] = {}).push_back({ key, transition.second.second->_name, transition.second.first });
         }
      }
   }
   h << "#ifndef INCLUDED_" + std::string(path) + "_GEN_H\n#define INCLUDED_" + std::string(path) + "_GEN_H";
   if (_stack_is_active)
   {
	   h << "\n#include <vector>";
   }
   h << "\n\n\nstruct " + name + "Fsm\n{\n";
   h << "\tenum " + name + "__State {\n";
   for (auto& key : keys) h << "\t\t" + key + ",\n";
   h << "\t};\n\n\tunsigned State() { return " << (_stack_is_active ? "_stack.back()" : "_state") << "; }\n\n";
   for (auto& tr : transitions)
   {
       h << "\tvoid On" + tr.first + "();\n";
   }
   if (_stack_is_active)
   {
	  h << "private:\n\tstd::vector<unsigned> _stack{ 0 };\n";
   }
   else
   {
      h << "private:\n\tunsigned _state{ 0 };\n";
   }
   h << "};\n\n\n#endif // INCLUDED_" + std::string(path) + "_GEN_H\n";
   

   // CPP
   std::ofstream f(std::string(path) + "_gen.cpp");
   f << "#include \"" + std::string(path) + "_gen.h\"\n";
   f << "\n\n";

   auto prewState = name + "Fsm::" + name + "__State::";
   for (auto& tr : transitions)
   {
      f << "void " << name << "Fsm::On" << tr.first << "() {\n\tswitch(" << (_stack_is_active ? "_stack.back()" : "_state") << ") {\n";
	  for (auto& step : tr.second)
	  {
		  f << "\tcase " << prewState << step.from << ": ";
		  if (_stack_is_active)
		  {
			  switch (step.by_action)
			  {
			  case StStack::Push: f << "_stack.push_back(" << prewState << step.to << ")"; break;
			  case StStack::Pop: f << "if (!_stack.empty()) _stack.pop_back()"; break;
			  case StStack::ReplaceWith: f << "if (!_stack.empty()) _stack.back() = " << prewState << step.to; break;
			  case StStack::Keep: break;
			  }
		  }
		  else
		  {
			  f << "_state = " << prewState << step.to;
		  }
		  f << "; {} break;\n";
	  }
      f << "\tdefault: break;\n\t}\n}\n";
   }
}
void State::ActivateStack(bool _active) { _stack_is_active = _active; }
std::map<std::string, std::shared_ptr<StateData>> State::_pool;
bool State::_stack_is_active{ false };


void mk_parser()
{
	State::ActivateStack();

	State{ "module_statements" }
		.On("NotEOF", StStack::Push, "statement_of_module")
		.On("EOF", "stop")
		;
	State{ "statement_of_module" }
		.On("Using",     StStack::Push, "using_stmt")
		.On("Type",      StStack::Push, "type_stmt")
		.On("Lambda",    StStack::Push, "func_stmt")
		.On("NextEqual", StStack::Push, "global_const")
		.On("EOF",       StStack::Pop)
		;
	State{ "using_stmt" }
		.On("Id",  StStack::Keep)
		.On("Dot", StStack::Keep) // *[Dot Id]
		.On("As",  StStack::Pop)  //  [As  Id]
		.On("EOL", StStack::Pop)
		.On("EOF", StStack::Pop)
		;

	State{ "type_stmt" }
		.On("Id", StStack::Pop)
		.On("Colon", "type_parent_stmt")
		.On("OpenPar", "type_body")
		.On("EOF", StStack::Pop)
		;
	State{ "type_parent_stmt" }
		.On("Id",  StStack::Keep) // Id
		.On("Dot", StStack::Keep) // *[Dot Id]
		.On("OpenPar", "type_body")
		.On("EOF", StStack::Pop)
		;
	State{ "type_body" }
		.On("Id",         StStack::Push, "field_def")
		.On("Static",     StStack::Push, "field_def")
		.On("Lambda",     StStack::Push, "method_def")
		.On("Enumerable", StStack::Push, "method_def")
		.On("ClosePar",   StStack::Pop)
		.On("EOF",        StStack::Pop)
		;
	State{ "field_def" }
		.On("NextColon", "field_announcement")  // Id Colon
		.On("NextEqual", StStack::Push, "expr") // Id Equal
		.On("Comma", StStack::Pop)
		.On("EOL", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "field_announcement" }
		.On("Id",    StStack::Keep)
		.On("Dot",   StStack::Keep)
		.On("Int",   StStack::Pop)
		.On("Float", StStack::Pop)
		.On("Str",   StStack::Pop)
		.On("EOL",   StStack::Pop)
		.On("EOF",   StStack::Pop)
		;
	State{ "method_def" }
		.On("Static", StStack::Keep)
		.On("Id", StStack::Keep)
		.On("OpenPar", "method_arguments")
		.On("EOF", StStack::Pop)
		;
	State{ "method_arguments" }
		.On("Id",  StStack::Push, "field_def")
		.On("ClosePar", "func_body")
		.On("EOF", StStack::Pop)
		;
	State{ "func_body" }
		.On("NextEqual", StStack::Push, "check_native_func")
		.On("NextEOL",   StStack::Push, "body")
		.On("End", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "check_native_func" }
		.On("NextNative", StStack::Pop)
		.On("NextNotNative", "stmt")
		.On("EOF", StStack::Pop)
		;
	State{ "body" }
		.On("NotEnd", StStack::Push, "stmt")
		.On("End", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "stmt" }
		.On("If",    "if_stmt"   )
		.On("While", "while_stmt")
		.On("For",   "for_stmt"  )
		.On("Yield", "yield_stmt")
		.On("NextColon", "field_def")
		.On("NextEqual", "field_def")
		.On("Id", "expr")
		.On("Num", "expr")
		.On("String", "expr")
		.On("RealNum", "expr")
		.On("This", "expr")
		.On("Return", "return_stmt")
		.On("Enumerable", StStack::Push, "enumerable_method_def")
		.On("Continue", StStack::Pop)
		.On("Break", StStack::Pop)
		.On("End", StStack::Pop)
		.On("EOL", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "enumerable_method_def" }
		.On("Enumerable", StStack::Push, "method_def")
		.On("End", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "if_stmt" }
		.On("OpenPar", StStack::Push, "expr")
		.On("ClosePar", "if_body")// something like "than"
		.On("Comma", "if_body")	  //
		.On("EOF", StStack::Pop)
		;
	State{ "if_body" }
		.On("OrNextOpenPar", "else_stmt")
		.On("OrNextComma", "else_body")
		.On("End", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "else_stmt" }
		.On("OpenPar", StStack::Push, "expr")
		.On("ClosePar", "else_body")
		.On("EOF", StStack::Pop)
		;
	State{ "else_body" }
		.On("_not_EOF_or_End_or_OrNextComma", StStack::Push, "stmt")
		.On("OrNextComma", "else_body")
		.On("End", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "while_stmt" }
		.On("OpenPar", StStack::Push, "expr")
		.On("ClosePar", StStack::Push, "while_body")
		.On("EOF", StStack::Pop)
		;
	State{ "while_body" }
		.On("_not_EOF_or_End", StStack::Push, "stmt")
		.On("End", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "for_stmt" }
		.On("PrewOpenPar_and_CurrentId_and_NextComma_or_NextColon", "foeach_stmt")
		.On("PrewOpenPar_and_not_CurrentId_and_NextComma_or_NextColon", "c_for_stmt")
		.On("EOF", StStack::Pop)
		;
	State{ "foreach_stmt" }
		.On("PrewOpenPar", StStack::Push, "field_def")
		.On("PrewComma", StStack::Push, "field_def")
		.On("Colon", StStack::Push, "expr")
		.On("ClosePar", "for_body")
		.On("EOF", StStack::Pop)
		;
	State{ "c_for_stmt" }
		.On("PrewOpenPar", StStack::Push, "field_def")
		.On("Semicolon", StStack::Push, "expr")
		.On("ClosePar", "for_body")
		.On("EOF", StStack::Pop)
		;
	State{ "for_body" }
		.On("_not_EOF_or_End", StStack::Push, "stmt")
		.On("End", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "yield_stmt" }
		.On("Break", StStack::Pop)
		.On("Return", StStack::Push, "expr")
		.On("EOL", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "return_stmt" }
		.On("_not_EOL_or_EOF", "expr")
		.On("End", StStack::Pop)
		.On("EOL", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "expr" }
		.On("Lambda", StStack::Push, "lambda_expr")
		.On("If", StStack::Push, "if_expr")
		.On("_inExprStack__not_BinOp_OpenPar", StStack::Push, "call_expr") // expr ( expr )   => call_expr
		.On("_inExprStack__BinOp_OpenPar", StStack::Push, "exrp")          // expr * ( expr ) => expr * expr
		.On("Id", StStack::Keep)      // id      => expr
		.On("Num", StStack::Keep)     // Num     => expr
		.On("This", StStack::Keep)    // This    => expr
		.On("String", StStack::Keep)  // String  => expr
		.On("RealNum", StStack::Keep) // RealNum => expr
		.On("_inExprStack__Expr_LABinOp_Expr_LABinOp_or_End_or_Comma_or_ClosePar_or_EOL_or_EOF", StStack::Keep) // expr * expr -  => expr -
		.On("_inExprStack__Expr_RABinOp_Expr_LABinOp_or_End_or_Comma_or_ClosePar_or_EOL_or_EOF", StStack::Keep) // expr ^ expr +  => expr +
		.On("_inExprStack__UnOp_Expr_End_or_Comma_or_ClosePar_or_EOL_or_EOF_or_not_OpenPar_or_RABinOp", StStack::Keep) // - expr
		.On("ClosePar", StStack::Pop)
		.On("Comma", StStack::Pop)
		.On("End", StStack::Pop)
		.On("EOL", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "lambda_expr" }
		.On("Id", StStack::Keep) // argument
		.On("Dot", "lambda_ret")
		.On("DoubleDot", "lambda_body")
		.On("End", StStack::Pop)
		.On("EOL", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "lambda_body" }
		.On("NotEnd", StStack::Push, "body")
		.On("ClosePar", StStack::Pop)
		.On("End", StStack::Pop)
		.On("EOL", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "lambda_ret" }
		.On("_inExprStack__empty", StStack::Push, "expr")
		.On("ClosePar", StStack::Pop)
		.On("End", StStack::Pop)
		.On("EOL", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "if_expr" }
		.On("OpenPar", StStack::Push, "expr")
		.On("ClosePar", "then_expr")
		.On("EOF", StStack::Pop)
		;
	State{ "then_expr" }
		.On("OpenExpr", StStack::Push, "expr")
		.On("Comma", "else_expr")
		.On("EOF", StStack::Pop)
		;
	State{ "else_expr" }
		.On("Comma", StStack::Push, "expr")
		.On("ClosePar", StStack::Pop)
		.On("EOF", StStack::Pop)
		;
	State{ "call_expr" }
		.On("IdNextColon", StStack::Push, "exprt")
		.On("Comma", StStack::Keep)
		.On("ClosePar", StStack::Pop)
		.On("End", StStack::Pop)
		.On("EOL", StStack::Pop)
		.On("EOF", StStack::Pop)
		;

	State::WriteTo("Parser", "parser_fsm");
}

void mk_tokenizer()
{
   State{ "pass" }
      .On("Letter", "id")
      .On("Digit", "number")
      .On("Quotation", "string")
      .On("Underscore", "id")
      .On("NewLine", "new_line")
      .On("Symbol", "symbol")
      .On("Tab", "pass")
      .On("Space", "pass")
      ;


   State{ "id" }
      .On("Letter", "id")
      .On("Underscore", "id")
      .On("Digit", "id")
      .On("NewLine", "new_line")
      .On("Symbol", "symbol")
      .On("Tab", "pass")
      .On("Space", "pass")
      ;

   State{ "number" }
      .On("Digit", "number")
      .On("Underscore", "number")
      .On("Dot", "real_number")
      .On("NewLine", "new_line")
      .On("Symbol", "symbol")
      .On("Tab", "pass")
      .On("Space", "pass")
      ;

   State{ "real_number" }
      .On("Underscore", "real_number")
      .On("Digit", "real_number")
      .On("NewLine", "new_line")
      .On("Symbol", "symbol")
      .On("Tab", "pass")
      .On("Space", "pass")
      ;

   State{ "concrete_symbol" }
      .On("Symbol", "string")
      ;

   State{ "string" }
      .On("Backslash", "concrete_symbol")
      .On("Tab", "string")
      .On("Space", "string")
      .On("NewLine", "string")
      .On("Digit",   "string")
      .On("Letter", "string")
      .On("Symbol", "string")
      .On("Underscore", "string")
      .On("Quotation", "pass")
      ;

   State{ "new_line" }
      .On("_", "pass")
      ;

   State{ "symbol" }
      .On("_", "pass")
      ;

   State::WriteTo("Tokenizer", "tokenizer_fsm");
}
