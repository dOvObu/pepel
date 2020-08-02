#ifndef INCLUDED_PRINTER_H
#define INCLUDED_PRINTER_H
#include "ivisitor.h"
#include "parser_nodes_.h"
#include <set>
#include <iostream>

class Printer : public IVisitor
{
	void visit(Using& r) override
	{
		std::cout << "(using ";
		bool first{ true };
		for (auto it : r.metaPath) { if (first) first = false; else std::cout << '.';  std::cout << it; }
		std::cout << ' ' << r.hasPseudonim << ' ' << r.pseudonim << ")" << std::endl;
	}

	void visit(Type& r) override
	{
		if (_shown_types.count(&r)) { std::cout << "(type " << r.id << ')'; return; } else _shown_types.insert(&r);

		std::cout << "(type " << r.id << (r.HasParent() ? " " + r.parent->id: "");
		if (!r.fields.empty()) std::cout << "\n::: fields :::\n";
		for (auto v : r.fields)
		{
			v->accept(*this);
			std::cout << std::endl;
		}
		if (!r.methods.empty()) std::cout << "\n::: methods :::\n";
		for (auto m : r.methods)
		{
			m->accept(*this);
		}
		std::cout << ")";
	}


   void visit(TType& r) override
   {
      std::cout << "(unknown type)";
   }

	void visit(TypeOp& r) override
	{
      std::cout << "(type " << r.ToStr() << ')';
	}

   void visit(TypeSeq& r) override
   {
      std::cout << "(type " << r.ToStr() << ')';
   }

	void visit(Func& r) override
	{
		std::cout << "(func " << r.id << "(";
		bool first{ true };
		for (auto v : r.arguments)
		{
			if (first) first = false; else std::cout << " ";
			v->accept(*this);
		}
		std::cout << ") ";
		if (r.is_native) std::cout << "native";
		else if (r.body != nullptr) r.body->accept(*this);
		std::cout << ")";
	}

	void visit(VarDefinition& r) override
	{
		std::cout << "(var_def " << r.id << " ";
		if (r.hasConcreteType) r.type->accept(*this);
		std::cout << " ";
		if (r.val != nullptr) r.val->accept(*this);
		std::cout << ")";
	}

	void visit(LambdaFunc& r) override
	{
		std::cout << "(lambda ";
		for (auto arg : r.arguments) arg->accept(*this);
		std::cout << " => ";
		r.body->accept(*this);
		std::cout << ")";
	}

	void visit(IteExpr& r) override
	{
		std::cout << "(ite ";
		r.prop->accept(*this);
		std::cout << " ";
		r.passed->accept(*this);
		std::cout << " ";
		r.fall->accept(*this);
		std::cout << ")";
	}

	void visit(FwExpr& r) override
	{
		std::cout << "(fw ";
		r.itemOp->accept(*this);
		std::cout << " : ";
		r.id->accept(*this);
		std::cout << " ";
		if (r.idxId != nullptr) r.idxId->accept(*this);
		if (r.prop != nullptr)
		{
			std::cout << " | ";
			r.prop->accept(*this);
		}
		std::cout << ")";
	}

	void visit(Num& r) override
	{
		std::cout << "(num " << r.val << ")";
	}

	void visit(RealNum& r) override
	{
		std::cout << "(real_num " << r.val << ")";
	}

	void visit(String& r) override
	{
		std::cout << "(string " << r.val << ")";
	}

	void visit(Id& r) override
	{
      std::cout << "(id " << r.val << (r.type == nullptr? "" : ":" + r.type->ToStr()) << ")";
	}

	void visit(This& r) override
	{
		std::cout << "(this)";
	}

	void visit(Call& r) override
	{
		std::cout << "(call ";
		r.funcSource->accept(*this);
		std::cout << " (";
		bool first{ true };
		for (auto a : r.arguments)
		{
			if (first) first = false; else std::cout << " ";
			a->accept(*this);
		}
		std::cout << "))";
	}

	void visit(BinOp& r) override
	{
		if (r.st == Nd::ADD)   std::cout << "(+ ";
		if (r.st == Nd::SUB)   std::cout << "(- ";
		if (r.st == Nd::MUL)   std::cout << "(* ";
		if (r.st == Nd::DIV)   std::cout << "(/ ";
		if (r.st == Nd::DOT)   std::cout << "(. ";
		if (r.st == Nd::OR )   std::cout << "(| ";
		if (r.st == Nd::AND)   std::cout << "(& ";
		if (r.st == Nd::EQUAL) std::cout << "(== ";
		if (r.st == Nd::ASIGN) std::cout << "(:= ";
		if (r.st == Nd::ADD_ASIGN) std::cout << "(+= ";
		if (r.st == Nd::SUB_ASIGN) std::cout << "(-= ";
		if (r.st == Nd::MUL_ASIGN) std::cout << "(*= ";
		if (r.st == Nd::DIV_ASIGN) std::cout << "(/= ";
		r.left ->accept(*this);
		std::cout << " ";
		r.right->accept(*this);
		std::cout << ")";
	}

	void visit(Sequence& r) override
	{
		std::cout << "(seq " << std::endl;
		bool first{ true };
		for (auto i : r.items)
		{
			if (first) first = false; else std::cout << " ";
			i->accept(*this);
		}
		std::cout << ")";
	}

	void visit(Plus& r) override
	{
		std::cout << "plus" << std::endl;
	}

	void visit(Minus& r) override
	{
		std::cout << "minus" << std::endl;
	}

	void visit(Not& r) override
	{
		std::cout << "not" << std::endl;
	}

	void visit(Body& r) override
	{
		std::cout << "(body" << std::endl;
		for (auto s : r.statements)
		{
			std::cout << "  ";
			s->accept(*this);
			std::cout << std::endl;
		}
		std::cout << ")";
	}

	void visit(If& r) override
	{
		std::cout << "(if ";
		if (r.prop != nullptr) r.prop->accept(*this);
		if (r.body != nullptr) r.body->accept(*this); else std::cout << "{}";
		if (r.else_body != nullptr) { std::cout << " else "; r.else_body->accept(*this); }
		std::cout << ")";
	}

	void visit(For& r) override
	{
		std::cout << "(for)";
	}

	void visit(While& r) override
	{
		std::cout << "(while ";
		r.prop->accept(*this);
		std::cout << " ";
		r.body->accept(*this);
		std::cout << ")";
	}

	void visit(Yield& r) override
	{
		std::cout << "(yield)";
	}

	void visit(Return& r) override
	{
		std::cout << "(return ";
		r.e->accept(*this);
		std::cout << ")";
	}

	void visit(Break& r) override
	{
		std::cout << "(break)";
	}

	void visit(Continue& r) override
	{
		std::cout << "(continue)";
	}

	void visit(Foreach& r) override
	{
		std::cout << "(foreach ";
		r.it->accept(*this);
		if (r.idx != nullptr) r.idx->accept(*this);
		std::cout << " : ";
		r.range->accept(*this);
		r.body->accept(*this);
		std::cout << ")";
	}

	void visit(Module& r) override
	{
		std::cout << "(module " << r.name << std::endl;
		if (!r.dependecies.empty()) std::cout << "::: dependencies :::" << std::endl;
		for (auto u : r.dependecies) { u->accept(*this); std::cout << std::endl; }
		if (!r.functions.empty()) std::cout << "::: functions :::" << std::endl;
		for (auto f : r.functions) { f->accept(*this); std::cout << std::endl; }
		if (!r.types.empty()) std::cout << "::: types :::" << std::endl;
		for (auto t : r.types) { t->accept(*this); std::cout << std::endl; }
		std::cout << ")";
	}

	void visit(EToken&) override
	{
		std::cout << "etoken" << std::endl;
	}
	std::set<Type*> _shown_types;
};

#endif // ! INCLUDED_PRINTER_H //
