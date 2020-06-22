#ifndef FINAL_STATE_MACHINE_GENERATOR_H
#define FINAL_STATE_MACHINE_GENERATOR_H

#include <string>
#include <fstream>
#include <memory>
#include <map>

enum class StStack
{
	Push,
	Pop,
	Keep,
    ReplaceWith,
};

struct StateData
{

   std::string _name{ "unknown" };
   std::map<std::string, std::pair<StStack, StateData*>> _transitions;
   StateData(char const* name) :_name(name) {}
};


struct State
{
   State(char const* name);

   State& On(char const* key, char const* state);
   State& On(char const* key, StStack action);
   State& On(char const* key, StStack action, char const* state);

   void Goto(char const* key);

   static void WriteTo(std::string name, char const* path);
   static void ActivateStack(bool activate = true);
private:
   StateData* _stateData{ nullptr };
   static std::map<std::string, std::shared_ptr<StateData>> _pool;
   static bool _stack_is_active;
};

void mk_parser();


#endif // ! FINAL_STATE_MACHINE_GENERATOR_H