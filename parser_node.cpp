#include "parser_nodes_.h"
std::vector<std::shared_ptr<Node>> Node::pool;
Type Type::Int{ "Int", false };
Type Type::Float{ "Float", false };
Type Type::String{ "String", false };
