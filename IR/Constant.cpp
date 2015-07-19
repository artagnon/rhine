#include "rhine/IR/Constant.h"

namespace rhine {
Constant::Constant(Type *Ty, RTValue ID) : Value(Ty, ID) {}

ConstantInt::ConstantInt(int Val, unsigned Bitwidth, Context *K) :
    Constant(IntegerType::get(Bitwidth, K), RT_ConstantInt), Val(Val) {}
}
