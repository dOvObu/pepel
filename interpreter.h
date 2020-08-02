#ifndef INCLUDED_INTERPRETER_H
#define INCLUDED_INTERPRETER_H
#include "ivisitor.h"
#include "parser_nodes_.h"
#include "tokenizer_fsm_gen.h"
#include "printer.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>

struct Interpreter : public IVisitor
{
   std::map<std::string, Module*> modules;
   std::vector<Type*> type_stack;
   std::vector<Context*> context_stack;
private:
   void visit(Using& n) override
   {
   }

   void visit(Type& n) override
   {
   }

   void visit(TType& n) override
   {
   }

   void visit(TypeSeq& n) override
   {
   }

   void visit(TypeOp& n) override
   {
   }

   void visit(Func& n) override
   {
      context_stack.push_back(n.context);

      if (n.arguments.size() > 1)
      {
         TypeSeq* sq = Deckard();
         n.type->left = sq;

         for (auto arg : n.arguments)
         {
            if (arg->hasConcreteType)
            {
               sq->seq.push_back(arg->type);
            }
            else
            {
               sq->seq.push_back(new TType());
            }
         }
      }
      else if (n.arguments.empty())
      {
         n.type->left = &Type::Void;
      }
      else
      {
         n.type->left = n.arguments[0]->hasConcreteType ? n.arguments[0]->type : new TType();
      }
      std::cout << "test " << n.id << std::endl;

      auto stackLen = type_stack.size();
      n.body->accept(*this);

      if (type_stack.size() > stackLen)
      {
         n.type->right = type_stack.back();
         type_stack.pop_back();
      }
      else
      {
         n.type->right = &Type::Void;
      }

      context_stack.pop_back();
   }

   void visit(VarDefinition& n) override
   {
      context_stack.back()->vars[n.id] = &n;

      if (n.type == nullptr)
      {
         auto stackLen = type_stack.size();
         
         n.val->accept(*this);
         if (type_stack.size() > stackLen)
         {
            n.type = type_stack.back();
            n.hasConcreteType = true;
            type_stack.pop_back();
         }
      }
      else
      {
      }
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
      type_stack.push_back(n.type);
   }

   void visit(RealNum& n) override
   {
      type_stack.push_back(n.type);
   }

   void visit(String& n) override
   {
      type_stack.push_back(n.type);
   }

   void visit(Id& n) override
   {
      for (auto i = context_stack.size(); i > 0;)
      {
         --i;
         std::cout << "--- " << n.val << std::endl;
         for (auto f : context_stack[i]->funcs) std::cout << f.first << std::endl;
         for (auto f : context_stack[i]->vars)  std::cout << f.first << std::endl;
         for (auto f : context_stack[i]->types) std::cout << f.first << std::endl;
         
         if (context_stack[i]->funcs.count(n.val))
         {
            type_stack.push_back(n.type = context_stack[i]->funcs[n.val]->type);
            break;
         }
         else if (context_stack[i]->vars.count(n.val))
         {
            type_stack.push_back(n.type = context_stack[i]->vars[n.val]->type);
            break;
         }
         else if (context_stack[i]->types.count(n.val))
         {
            type_stack.push_back(n.type = context_stack[i]->types[n.val]);
            break;
         }
      }
   }

   void visit(This& n) override
   {
   }

   void visit(Call& n) override
   {
      n.funcSource->accept(*this);
   }

   void visit(BinOp& n) override
   {
      n.left->accept(*this);
      n.right->accept(*this);
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
      Context context;
      context_stack.push_back(&context);

      for (auto st : n.statements)
      {
         st->accept(*this);
      }

      context_stack.pop_back();
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
            {
               Printer pr;
               modules[moduleName]->accept(pr);
            }
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
                           n.context.vars[u->pseudonim] = t->dstaticFields[u->metaPath[2]];
                        }
                        else
                        {
                           auto var = t->dstaticFields[u->metaPath[2]];
                           n.context.vars[var->id] = var;
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
