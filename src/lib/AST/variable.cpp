#include "AST/variable.hpp"
#include "visitor/AstNodeVisitor.hpp"

VariableNode::VariableNode(const uint32_t line, const uint32_t col,
                           const std::string &name,
                           const PTypeSharedPtr &p_type,
                           const std::shared_ptr<ConstantValueNode> &p_constant)
    : AstNode{line, col}, name(std::move(name)), type(p_type),
      constant(p_constant) {}

const char *VariableNode::getNameCString() const { return name.c_str(); }

const char *VariableNode::getTypeCString() const {
    return type->getPTypeCString();
}

bool VariableNode::isconstant(){
    return constant ? true : false;
}

bool VariableNode::array_legal(){
    //type->print_dim();
    if(!type->checkDimensions()){
        return type->dimension_legal();
    }
    else return true;
}

int VariableNode::get_child_size() const{
    return type->get_dimension_size();
}

std::string VariableNode::get_enum(){
    switch(type->getPrimitiveType()){
        case PType::PrimitiveTypeEnum::kIntegerType: return "integer";
        case PType::PrimitiveTypeEnum::kRealType: return "real";
        case PType::PrimitiveTypeEnum::kBoolType: return "boolean";
        case PType::PrimitiveTypeEnum::kStringType: return "string";
    }
}

std::string VariableNode::get_enumType(){
    if(type->checkDimensions()){
        switch(type->getPrimitiveType()){
            case PType::PrimitiveTypeEnum::kIntegerType: return "integer";
            case PType::PrimitiveTypeEnum::kRealType: return "real";
            case PType::PrimitiveTypeEnum::kBoolType: return "boolean";
            case PType::PrimitiveTypeEnum::kStringType: return "string";
        }
    }
    else {
        return type->getPTypeCString();
    }
    
}

std::string VariableNode::get_constValue(){
    return constant->getConstantValueCString();
}

void VariableNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (constant) {
        constant->accept(p_visitor);
    }
}
