#ifndef VALUE_H
#define VALUE_H

#include <iostream>

typedef struct VALUE {
    union {
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

    friend std::ostream& operator<<(std::ostream& out, VALUE &v) {
        if (v.isFP) {
            if (v.bitWidth == 32) out << v.val.f32;
            else                  out << v.val.f64;
        } else {
            if (v.isSigned) {
                if (v.bitWidth ==  8) out << v.val.i8s;
                if (v.bitWidth == 16) out << v.val.i16s;
                if (v.bitWidth == 32) out << v.val.i32s;
                else                  out << v.val.i64s;
            } else {
                if (v.bitWidth ==  8) out << v.val.i8u;
                if (v.bitWidth == 16) out << v.val.i16u;
                if (v.bitWidth == 32) out << v.val.i32u;
                else                  out << v.val.i64u;
            }
        }
        return out;
    }
} VALUE;


#endif /* end of include guard: VALUE_H */
