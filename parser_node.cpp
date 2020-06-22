#include "parser_nodes_.h"
std::vector<std::shared_ptr<Node>> Node::pool;
Type Type::Int{ "Int" };
Type Type::Float{ "Float" };
Type Type::String{ "String" };
