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
#include <set>

struct Interpreter : public IVisitor
{
   std::map<std::string, Module*> modules;
   std::vector<Type*> type_stack;
   std::vector<Context*> context_stack;
   std::set<Func*> visited_func;
   std::set<Type*> visited_type;
private:
   void visit(Using& n) override
   {
   }

   void visit(Type& n) override
   {
      if (visited_type.count(&n))
      {
         return;
      }
      visited_type.insert(&n);
      context_stack.push_back(n.context);
      Context context{ {}, n.dmethods, n.dfields };
      context_stack.push_back(&context);

      for (auto f : n.fields) f->accept(*this);
      for (auto m : n.methods) m->accept(*this);

      context_stack.pop_back();
      context_stack.pop_back();
   }

   void visit(TType& n) override
   {
   }

   void visit(TypeSeq& n) override
   {
   }

   void visit(TypeArrow& n) override
   {
   }

   void visit(Func& n) override
   {
      if (visited_func.count(&n))
      {
         return;
      }
      visited_func.insert(&n);

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

      Context context;
      for (auto arg : n.arguments) context.vars[arg->id] = arg;
      context_stack.push_back(&context);

      auto stackLen = type_stack.size();
      n.body->accept(*this);

      if (type_stack.size() > stackLen)
      {
         n.type->right = type_stack.back();
         type_stack.pop_back();
      }
      else if (n.is_native)
      {
         // TODO : Take info about returned type from API
         n.type->right = &Type::Void;
      }
      else
      {
         n.type->right = &Type::Void;
      }

      context_stack.pop_back();
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

         if (context_stack[i]->funcs.count(n.val))
         {
            context_stack[i]->funcs[n.val]->accept(*this);
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
            context_stack[i]->types[n.val]->accept(*this);
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
      auto stackLen = type_stack.size();
      n.funcSource->accept(*this);
      if (type_stack.size() > stackLen && type_stack.back()->st == Nd::TYPE_ARROW)
      {
         auto t = reinterpret_cast<TypeArrow*>(type_stack.back());
         type_stack.pop_back();
         if (t->left == nullptr)
         {
            t->left = n.arguments.empty() ? &Type::Void : new TType();
         }
         if (t->right == nullptr)
         {
            t->right = &Type::Int;
         }
         type_stack.push_back(t->right);
      }
      else
      {
         std::cout << "ERROR! Call not from function\n";
      }
   }

   void visit(BinOp& n) override
   {
      auto stackLen = type_stack.size();
      n.left->accept(*this);
      bool enter_module{ false };
      Type* left{ nullptr };
      Type* right{ nullptr };
      
      if (type_stack.size() > stackLen)
      {
         left = type_stack.back();
         type_stack.pop_back();
      }
      else
      {
         context_stack.push_back(&modules[reinterpret_cast<Id*>(n.left)->val]->context);
         enter_module = true;
      }

      bool in_type = false;
      if (!enter_module && n.st == Nd::DOT && left != &Type::Float && left != &Type::Int)
      {
         Context context;
         if (left != nullptr)
         {
            context.funcs = left->dmethods;
            context.vars = left->dfields;
            for (auto f : left->staticFields) { context.vars[f->id] = f; }
            context_stack.push_back(&context);
            in_type = true;
         }
         n.right->accept(*this);
      }
      else
      {
         n.right->accept(*this);
      }
      right = type_stack.size() > stackLen ? type_stack.back() : &Type::Void;
      if (in_type)
      {
         context_stack.pop_back();
      }

      if (left != right && n.st != Nd::DOT)
      {
         type_stack.pop_back();

         if (left == &Type::String || right == &Type::String)
         {
            type_stack.push_back(&Type::String);
         }
         else if ((left == &Type::Float && right == &Type::Int) || (left == &Type::Int && right == &Type::Float))
         {
            type_stack.push_back(&Type::Float);
         }
      }

      if (enter_module)
      {
         context_stack.pop_back();
      }
   }

   void visit(Sequence& n) override
   {
   }

   void visit(Plus& n) override
   {
      n.right->accept(*this);
      n.type = type_stack.back();
   }

   void visit(Minus& n) override
   {
      n.right->accept(*this);
      n.type = type_stack.back();
   }  

   void visit(Not& n) override
   {
      n.right->accept(*this);
      n.type = type_stack.back();
      type_stack.pop_back();
      type_stack.push_back(n.type);
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
      if (n.e == nullptr)
      {
         type_stack.push_back(&Type::Void);
      }
      else
      {
         n.e->accept(*this);
      }
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
