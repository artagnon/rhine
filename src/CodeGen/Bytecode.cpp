#include "rhine/CodeGen/Bytecode.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/Function.hpp"
#include "rhine/IR/Instruction.hpp"

namespace rhine {
std::stringstream Bytecode::generate() {
  std::stringstream oss;
  return oss;
}
void Bytecode::write() {}
void Bytecode::emitOpcode() {}

void CallInst::generate(Bytecode *) {}
void MallocInst::generate(Bytecode *) {}
void LoadInst::generate(Bytecode *) {}
void StoreInst::generate(Bytecode *) {}
void ReturnInst::generate(Bytecode *) {}
void IndexingInst::generate(Bytecode *) {}
void TerminatorInst::generate(Bytecode *) {}
void BinaryArithInst::generate(Bytecode *) {}
void IfInst::generate(Bytecode *) {}
void BindInst::generate(Bytecode *) {}

void ConstantInt::generate(Bytecode *) {}
void ConstantBool::generate(Bytecode *) {}
void ConstantFloat::generate(Bytecode *) {}
void Pointer::generate(Bytecode *) {}
void Prototype::generate(Bytecode *) {}
void Function::generate(Bytecode *) {}
}
