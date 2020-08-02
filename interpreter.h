#ifndef INCLUDED_INTERPRETER_H
#define INCLUDED_INTERPRETER_H
#include "ivisitor.h"
#include "parser_nodes_.h"
#include "tokenizer_fsm_gen.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>

struct Interpreter : public IVisitor
{
   std::map<std::string, Module*> modules;
private:
   void visit(Using& n) override
   {
   }

   void visit(Type& n) override
   {
   }

   void visit(TypeOp& n) override
   {
   }

   void visit(Func& n) override
   {
   }

   void visit(VarDefinition& n) override
   {
   }

   void visit(LambdaFunc& n) override
   {
   }

   void visit(IteExpr& n) override
   {
   }

   void visit(FwExpr& n) override
   {
   }

   void visit(Num& n) override
   {
   }

   void visit(RealNum& n) override
   {
   }

   void visit(String& n) override
   {
   }

   void visit(Id& n) override
   {
   }

   void visit(This& n) override
   {
   }

   void visit(Call& n) override
   {
   }

   void visit(BinOp& n) override
   {
   }

   void visit(Sequence& n) override
   {
   }

   void visit(Plus& n) override
   {
   }

   void visit(Minus& n) override
   {
   }

   void visit(Not& n) override
   {
   }

   void visit(Body& n) override
   {
      for (auto st : n.statements)
      {
         st->accept(*this);
      }
   }

   void visit(If& n) override
   {
   }

   void visit(For& n) override
   {
   }

   void visit(While& n) override
   {
   }

   void visit(Yield& n) override
   {
   }

   void visit(Return& n) override
   {
   }

   void visit(Break& n) override
   {
   }

   void visit(Continue& n) override
   {
   }

   void visit(Foreach& n) override
   {
   }

   void visit(Module& n) override
   {
      HandleDependencies(n);
      n.dfunctions["main"]->accept(*this);
   }

   void visit(EToken& n) override
   {
   }

   void HandleDependencies(Module& n)
   {
      for (auto u : n.dependecies)
      {
         auto& moduleName = u->metaPath[0];
         auto pathLength = u->metaPath.size();

         if (modules.count(moduleName) == 0)
         {
            modules[moduleName] = ParserFsm{}(TokenizerFsm{}((moduleName + ".txt").c_str()));
            HandleDependencies(*modules[moduleName]);
         }

         if (pathLength == 2)
         {
            auto moduleUsed = modules[moduleName];
            if (moduleUsed->dfunctions.count(u->metaPath[1]))
            {
               if (u->hasPseudonim)
               {
                  n.context.funcs[u->pseudonim] = moduleUsed->dfunctions[u->metaPath[1]];
               }
               else
               {
                  auto f = moduleUsed->dfunctions[u->metaPath[1]];
                  n.context.funcs[f->id] = f;
               }
            }
            else if (moduleUsed->dtypes.count(u->metaPath[1]))
            {
               if (u->hasPseudonim)
               {
                  n.context.types[u->pseudonim] = moduleUsed->dtypes[u->metaPath[1]];
               }
               else
               {
                  auto t = moduleUsed->dtypes[u->metaPath[1]];
                  n.context.types[t->id] = t;
               }
            }
            else
            {
               std::cerr << "ERROR! There is must to be some type or function with id: \'" << u->metaPath[1] << "\' in module \'" << moduleName << "\' to use it in \'" << n.name << "\' module\n" << std::endl;
            }
         }

         if (pathLength == 3)
         {
            if (u->hasPseudonim)
            {
               auto moduleUsed = modules[moduleName];

               if (moduleUsed->dtypes.count(u->metaPath[1]))
               {
                  auto t = moduleUsed->dtypes[u->metaPath[1]];
                  if (t != &Type::Int && t != &Type::Float && t != &Type::String)
                  {
                     if (t->dstaticFields.count(u->metaPath[2]))
                     {
                        if (u->hasPseudonim)
                        {
                           n.context.staticVars[u->pseudonim] = t->dstaticFields[u->metaPath[2]];
                        }
                        else
                        {
                           auto var = t->dstaticFields[u->metaPath[2]];
                           n.context.staticVars[var->id] = var;
                        }
                     }
                     else
                     {
                        std::cerr << "ERROR! There is must to be static field in \'" << moduleName << "." << t->id << "\' type to use it\n";
                     }
                  }
               }
               else
               {
                  std::cerr << "ERROR! There is must to be some type with static fields and id: \'" << u->metaPath[1] << "\' in module \'" << moduleName << "\' to use it in \'" << n.name << "\' module\n" << std::endl;
               }
            }
         }
      }
   }
};


#endif // ! INCLUDED_INTERPRETER_H
