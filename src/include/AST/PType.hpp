#ifndef __AST_PTYPE_H
#define __AST_PTYPE_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class PType;

typedef std::shared_ptr<const PType> PTypeSharedPtr;

class PType {
  public:
    enum class PrimitiveTypeEnum : uint8_t {
        kVoidType,
        kIntegerType,
        kRealType,
        kBoolType,
        kStringType,
        ERROR
    };

    PType(PrimitiveTypeEnum type);
    ~PType() = default;

    void setDimensions(std::vector<uint64_t> &dims);
    int get_dimension_size() const;
    bool checkDimensions() const ;

    const PrimitiveTypeEnum getPrimitiveType() const;
    const char *getPTypeCString() const;
    bool dimension_legal() const;
    void print_dim() const;

  private:
    PrimitiveTypeEnum type;
    std::vector<uint64_t> dimensions;
    mutable std::string type_string;
    mutable bool type_string_is_valid = false;
};

#endif
