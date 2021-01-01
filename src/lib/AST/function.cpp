#include "AST/function.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <cassert>

FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
                           const char *p_name, Decls *p_parameters,
                           const PType *p_type, CompoundStatementNode *p_body)
    : AstNode{line, col}, name(p_name), parameters(std::move(*p_parameters)),
      return_type(PTypeSharedPtr(p_type)), body(p_body) {}

const char *FunctionNode::getNameCString() const { return name.c_str(); }

const char *FunctionNode::getTypeCString() const {
    return return_type->getPTypeCString();
}

const char *FunctionNode::getPrototypeCString() const {
    if (!prototype_string_is_valid) {
        prototype_string += return_type->getPTypeCString();

        prototype_string += " (";
        for (const auto &parameter : parameters) {
            for (const auto &var : parameter->variables()) {
                prototype_string.append(var->getTypeCString()).append(", ");
            }
        }

        if (!parameters.empty()) {
            // remove the trailing ", "
            prototype_string.erase(prototype_string.end() - 2,
                                   prototype_string.end());
        }

        prototype_string += ")";
        prototype_string_is_valid = true;
    }

    return prototype_string.c_str();
}

std::string FunctionNode::get_enumType(){
    switch(return_type->getPrimitiveType()){
        case PType::PrimitiveTypeEnum::kIntegerType: return "integer";
        case PType::PrimitiveTypeEnum::kRealType: return "real";
        case PType::PrimitiveTypeEnum::kBoolType: return "boolean";
        case PType::PrimitiveTypeEnum::kStringType: return "string";
        case PType::PrimitiveTypeEnum::kVoidType: return "void";
        default: return "error";
    }
}

std::vector <std::string> FunctionNode::get_paraType(){
    std::vector <std::string> para_type;
    for(auto &parameter : parameters){
        for (auto &var : parameter->variables()) {
            std::string tem;
            tem = var->get_enumType();
            para_type.push_back(tem);
        }
    }
    return para_type;
}

bool FunctionNode::isDefined() const { return (body) ? true : false; }

void FunctionNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto &parameter : parameters) {
        parameter->accept(p_visitor);
    }
    if (body) {
        body->accept(p_visitor);
    }
}
