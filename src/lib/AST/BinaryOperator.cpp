#include "AST/BinaryOperator.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <iostream>

BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col,
                                       Operator op, ExpressionNode *p_left,
                                       ExpressionNode *p_right)
    : ExpressionNode{line, col}, op(op), left(p_left), right(p_right) {}

const char *BinaryOperatorNode::getOpCString() const {
    return kOpString[static_cast<size_t>(op)];
}

Operator BinaryOperatorNode::get_enum(){
    return op;
}

void BinaryOperatorNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    //std::cout<<"here"<<std::endl;
}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    left->accept(p_visitor);
    right->accept(p_visitor);
}
