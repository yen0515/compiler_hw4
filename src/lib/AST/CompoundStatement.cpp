#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <iostream>

CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col,
                                             Decls *p_var_decls,
                                             Nodes *p_statements)
    : AstNode{line, col}, var_decls(std::move(*p_var_decls)),
      statements(std::move(*p_statements)) {}

void CompoundStatementNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    //std::cout<<"visit level 0: "<<std::endl;
    for (auto &p_decl : var_decls) {
        //std::cout<<"visit level 1: "<<std::endl;
        p_decl->accept(p_visitor);
    }
    for (auto &p_statement : statements) {
        //std::cout<<"visit level 2: "<<std::endl;
        p_statement->accept(p_visitor);
    }
    //std::cout<<"visit level 3: "<<std::endl;
}
