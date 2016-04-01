#ifndef VALUE_H
#define VALUE_H

#include <sstream>
#include <iostream>

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

    VALUE(bool i)     { bitWidth = 1;  isSigned = true;  isFP = false; val.b = i;    }
    VALUE(int8_t i)   { bitWidth =  4; isSigned = true;  isFP = false; val.i8s = i;  }
    VALUE(int16_t i)  { bitWidth = 16; isSigned = true;  isFP = false; val.i16s = i; }
    VALUE(int32_t i)  { bitWidth = 32; isSigned = true;  isFP = false; val.i32s = i; }
    VALUE(int64_t i)  { bitWidth = 64; isSigned = true;  isFP = false; val.i64s = i; }
    VALUE(uint8_t i)  { bitWidth =  8; isSigned = false; isFP = false; val.i8u = i;  }
    VALUE(uint16_t i) { bitWidth = 16; isSigned = false; isFP = false; val.i16u = i; }
    VALUE(uint32_t i) { bitWidth = 32; isSigned = false; isFP = false; val.i32u = i; }
    VALUE(uint64_t i) { bitWidth = 64; isSigned = false; isFP = false; val.i64u = i; }
    VALUE(float f)    { bitWidth = 32;                   isFP = true ; val.f32 = f;  }
    VALUE(double f)   { bitWidth = 64;                   isFP = true ; val.f32 = f;  }

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
                if (v.bitWidth ==  1) out << v.val.b;
                else if (v.bitWidth ==  8) out << v.val.i8s;
                else if (v.bitWidth == 16) out << v.val.i16s;
                else if (v.bitWidth == 32) out << v.val.i32s;
                else                  out << v.val.i64s;
            } else {
                if (v.bitWidth ==  1) out << v.val.b;
                else if (v.bitWidth ==  8) out << v.val.i8u;
                else if (v.bitWidth == 16) out << v.val.i16u;
                else if (v.bitWidth == 32) out << v.val.i32u;
                else                  out << v.val.i64u;
            }
        }
        return out;
    }
} VALUE;


#endif /* end of include guard: VALUE_H */
