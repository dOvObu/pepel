#ifndef INCLUDED_INTERPRETER_H
#define INCLUDED_INTERPRETER_H
#include "ivisitor.h"
#include "parser_nodes_.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>

class Interpreter : public IVisitor
{
public:
   std::vector<std::map<std::string, VarDefinition*>> vis_area_v;
   std::vector<Node*> expr_stack;
   std::vector<std::map<std::string, Func*>> vis_area_f;
   std::vector<std::map<std::string, Type*>> vis_area_t;
   std::map<std::string, void(*)(Interpreter&, size_t)> native_f;
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
      vis_area_v.push_back({});
      auto& vav = vis_area_v.back();
      auto args = new VarDefinition[n.arguments.size()];
      for (size_t i = 0; n.arguments.size() > i; ++i)
      {
         args[i].id = n.arguments[i]->id;
         args[i].val = expr_stack[expr_stack.size() - n.arguments.size() + i];
         vav[args[i].id] = args + i;
      }
      for (size_t i = 0; n.arguments.size() > i; ++i)
      {
         expr_stack.pop_back();
      }

      if (n.is_native)
      {
         if (native_f.count(n.id) == 0)
         {
            native_f[n.id](*this, n.arguments.size());
         }
         else
         {
            std::cerr << "Error! There is no native function called " << n.id << "!!1\n";
         }
      }
      else
      {
         n.body->accept(*this);
      }

      delete[] args;
      vis_area_v.pop_back();
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
      expr_stack.push_back(&n);
   }

   void visit(RealNum& n) override
   {
      expr_stack.push_back(&n);
   }

   void visit(String& n) override
   {
      expr_stack.push_back(&n);
   }

   void visit(Id& n) override
   {
      expr_stack.push_back(&n);
   }

   void visit(This& n) override
   {
   }

   void visit(Call& n) override
   {
      n.funcSource->accept(*this);
      auto src = expr_stack.back();
      expr_stack.pop_back();

      for (auto a : n.arguments)
      {
         a->accept(*this);
      }

      Func* f = nullptr;
      char const* id = nullptr;

      f = TryGetFuncBySrcNode(src);

      if (f == nullptr)
      {
         std::cerr << "Error! There must to be a function with id \"" << id << "\" to call it!";
      }
      else
      {
         if (f->is_native)
         {
            native_f[f->id.c_str()](*this, f->arguments.size());
         }
         else
         {
            f->accept(*this);
         }
      }
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
      vis_area_f.push_back({});
      vis_area_t.push_back({});
      auto& vaf = vis_area_f.back();
      auto& vat = vis_area_t.back();

      for (auto f : n.functions)
      {
         vaf[f->id] = f;
      }
      for (auto t : n.types)
      {
         vat[t->id] = t;
      }

      vaf["main"]->accept(*this);
   }

   void visit(EToken& n) override
   {
   }

public:
   Func* TryGetFuncBySrcNode(Node* src)
   {
      Func* f = nullptr;
      if (src->st == Nd::ID)
      {
         auto id = reinterpret_cast<Id*>(src)->val.c_str();
         auto t3 = GetDefOf(id);
         if (std::get<1>(t3) != nullptr)
         {
            f = std::get<1>(t3);
         }
         else if (std::get<0>(t3) != nullptr && std::get<0>(t3)->val != nullptr)
         {
            f = TryGetFuncBySrcNode(std::get<0>(t3)->val);
         }
      }
      else if (src->st == Nd::FUNC)
      {
         f = reinterpret_cast<Func*>(src);
      }

      return f;
   }

   template<typename T>
   T* GetDefOfT(char const* name, std::vector<std::map<std::string, T*>>& vis_area)
   {
      T* res = nullptr;

      for (size_t i = vis_area.size() - 1;; --i)
      {
         if (vis_area[i].count(name) != 0)
         {
            res = vis_area[i][name];
            break;
         }

         if (i == 0)
         {
            break;
         }
      }
      return res;
   }

   template<>
   VarDefinition* GetDefOfT<VarDefinition>(char const* name, std::vector<std::map<std::string, VarDefinition*>>& vis_area)
   {
      VarDefinition* res = nullptr;

      for (size_t i = vis_area.size() - 1;; --i)
      {
         if (vis_area[i].count(name) != 0)
         {
            res = vis_area[i][name];
            if (res->val->st == Nd::ID)
            {
               auto id = reinterpret_cast<Id*>(res->val)->val;
               if (id == name)
               {
                  Node* deeper = nullptr;
                  for (size_t j = i - 1;; --j)
                  {
                     if (vis_area[j].count(name) != 0)
                     {
                        deeper = vis_area[j][name]->val;
                        break;
                     }
                     if (j == 0)
                     {
                        break;
                     }
                  }
                  if (deeper != nullptr)
                  {
                     res->val = deeper;
                  }
               }
            }
            break;
         }

         if (i == 0)
         {
            break;
         }
      }
      return res;
   }

   std::tuple<VarDefinition*, Func*, Type*> GetDefOf(char const* name)
   {
      return {
         GetDefOfT(name, vis_area_v),
         GetDefOfT(name, vis_area_f),
         GetDefOfT(name, vis_area_t),
      };
   }
};


#endif // ! INCLUDED_INTERPRETER_H
