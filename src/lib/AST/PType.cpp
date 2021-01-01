#include "AST/PType.hpp"
#include <iostream>

const char *kTypeString[] = {"void", "integer", "real", "boolean", "string", "error"};

PType::PType(PrimitiveTypeEnum type) : type(type) {}

void PType::setDimensions(std::vector<uint64_t> &dims) {
    dimensions = std::move(dims);
}

const PType::PrimitiveTypeEnum PType::getPrimitiveType() const { return type; }

// logical constness
const char *PType::getPTypeCString() const {
    if (!type_string_is_valid) {
        type_string += kTypeString[static_cast<int>(type)];

        if (dimensions.size() != 0) {
            type_string += " ";

            for (const auto &dim : dimensions) {
                type_string += "[" + std::to_string(dim) + "]";
            }
        }
        type_string_is_valid = true;
    }

    return type_string.c_str();
}

int PType::get_dimension_size() const{
    return dimensions.size();
}

bool PType::checkDimensions() const {
    return (dimensions.size()==0) ? true : false;
}

bool PType::dimension_legal() const{
    bool islegal = true;
    for(int i=0;i<dimensions.size();i++){
        if(dimensions[i]<=0) islegal = false;
    }
    return islegal;
}

void PType::print_dim() const{
    for(int i=0;i<dimensions.size();i++){
        std::cout<<"dims: "<<dimensions[i]<<std::endl;
    }
}
