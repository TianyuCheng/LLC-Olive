#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <sstream>
#include <iostream>

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

enum X86OperandType { X86Reg, X86Imm, X86Mem, X86Arg, X86Label, X86Function };
typedef X86OperandType OP_TYPE;

typedef struct VALUE {
    union {
        bool        b;
        int8_t      i8s;
        uint8_t     i8u;
        int16_t     i16s;
        uint16_t    i16u;
        int32_t     i32s;
        uint32_t    i32u;
        int64_t     i64s;
        uint64_t    i64u;
        float       f32;
        double      f64;
    } val;
    int bitWidth;
    bool isFP;
    bool isSigned;

    VALUE() {
        val.i64u = 0;
        bitWidth = 64;
        isFP = false;
        isSigned = false;
    }

    VALUE(bool i)     { val.i64u = 0; bitWidth = 1;  isSigned = true;  isFP = false; val.b = i;    }
    VALUE(int8_t i)   { val.i64u = 0; bitWidth =  4; isSigned = true;  isFP = false; val.i8s = i;  }
    VALUE(int16_t i)  { val.i64u = 0; bitWidth = 16; isSigned = true;  isFP = false; val.i16s = i; }
    VALUE(int32_t i)  { val.i64u = 0; bitWidth = 32; isSigned = true;  isFP = false; val.i32s = i; }
    VALUE(int64_t i)  { val.i64u = 0; bitWidth = 64; isSigned = true;  isFP = false; val.i64s = i; }
    VALUE(uint8_t i)  { val.i64u = 0; bitWidth =  8; isSigned = false; isFP = false; val.i8u = i;  }
    VALUE(uint16_t i) { val.i64u = 0; bitWidth = 16; isSigned = false; isFP = false; val.i16u = i; }
    VALUE(uint32_t i) { val.i64u = 0; bitWidth = 32; isSigned = false; isFP = false; val.i32u = i; }
    VALUE(uint64_t i) { val.i64u = 0; bitWidth = 64; isSigned = false; isFP = false; val.i64u = i; }
    VALUE(float f)    { val.i64u = 0; bitWidth = 32;                   isFP = true ; val.f32 = f;  }
    VALUE(double f)   { val.i64u = 0; bitWidth = 64;                   isFP = true ; val.f32 = f;  }

    int AsVirtualReg() const { return val.i32s; }       // some time a tree node to represent a register
    const char* AsLabel() const {
        std::stringstream ss;
        ss << ".L" << val.i32s;
        std::string s = ss.str();
        return s.c_str();
    }

    friend std::ostream& operator<<(std::ostream& out, VALUE &v) {
        if (v.isFP) {
            if (v.bitWidth == 32) out << v.val.f32;
            else                  out << v.val.f64;
        } else {
            if (v.isSigned) {
                if (v.bitWidth ==  1)      out << v.val.b;
                else if (v.bitWidth ==  8) out << v.val.i8s;
                else if (v.bitWidth == 16) out << v.val.i16s;
                else if (v.bitWidth == 32) out << v.val.i32s;
                else                       out << v.val.i64s;
            } else {
                if (v.bitWidth ==  1)      out << v.val.b;
                else if (v.bitWidth ==  8) out << v.val.i8u;
                else if (v.bitWidth == 16) out << v.val.i16u;
                else if (v.bitWidth == 32) out << v.val.i32u;
                else                       out << v.val.i64u;
            }
        }
        return out;
    }
} VALUE;

static int GetTypeSize(llvm::Type *type) {
    using namespace llvm;
    Type::TypeID id = type->getTypeID();
    switch (id) {
        case Type::TypeID::PointerTyID:
            return 8;       // 64-bit pointer
        case Type::TypeID::FloatTyID:
            return 4;       // 32 bits
        case Type::TypeID::DoubleTyID:
            return 8;       // 64 bits
        case Type::TypeID::ArrayTyID:
            {
                ArrayType *ty = dyn_cast<ArrayType>(type);
                assert(ty && "array type must not be null");
                return GetTypeSize(ty->getElementType());
            }
        case Type::TypeID::IntegerTyID:
            {
                IntegerType *ty = dyn_cast<IntegerType>(type);
                assert(ty && "integer type must not be null");
                return ty->getBitWidth() / 8;
            }
        default:
            std::cerr << "Unhandle-able TypeID: " << id << std::endl;
            assert(false && "not implemented Type Size");
    }
    return -1;
}

static std::string GetTypeSuffix(VALUE &v) {
    if (v.isFP) {
        if (v.bitWidth == 32) return "l";
        else                  return "q";
    } else {
        if (v.isSigned) {
            if (v.bitWidth ==  1)      return "b";
            else if (v.bitWidth ==  8) return "b";
            else if (v.bitWidth == 16) return "w";
            else if (v.bitWidth == 32) return "l";
            else                       return "q";
        } else {
            if (v.bitWidth ==  1)      return "b";
            else if (v.bitWidth ==  8) return "b";
            else if (v.bitWidth == 16) return "w";
            else if (v.bitWidth == 32) return "l";
            else                       return "q";
        }
    }
    assert(false && "should not reach here for GetTypeSuffix");
}

static std::string GetTypeSuffix(llvm::Type *type) {
    using namespace llvm;
    int size = GetTypeSize(type);
    switch (size) {
        case 1:
            return "b";
        case 2:
            return "w";
        case 4:
            return "l";
        case 8:
            return "q";
        default:
            std::cerr << "Unhandle-able Type Size: " << size << std::endl;
            assert(false && "not implemented Type Size");
    }
}


#endif /* end of include guard: VALUE_H */
