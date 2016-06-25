#include "rhine/Externals.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Function.hpp"
#include "rhine/IR/Instruction.hpp"

namespace rhine {
std::vector<std::pair<BasicBlock *, llvm::BasicBlock *>>
BasicBlock::zipSuccContainers(llvm::Module *M) {
  std::vector<std::pair<BasicBlock *, llvm::BasicBlock *>> SuccContainers;
  for (auto BB : succs()) {
    SuccContainers.push_back(std::make_pair(BB, BB->toContainerLL(M)));
  }
  return SuccContainers;
}

llvm::Value *BasicBlock::getPhiValueFromBranchBlock(llvm::Module *M) {
  auto K = context();
  assert(succ_size() == 2 && "Only 2 successors allowed for now");
  auto BranchBlock = K->Builder->GetInsertBlock();
  auto BranchContainers = zipSuccContainers(M);

  /// First codegen the container of the MergeBlock, codegen the full true/false
  /// Blocks, then get back to filling out the Merge Block.
  auto MergeBlockContainer = getMergeBlock()->toContainerLL(M);
  MergeBlockContainer->setName("merge");

  /// The IfInst.
  auto BrInst = cast<IfInst>(back());
  K->Builder->SetInsertPoint(BranchBlock);
  K->Builder->CreateCondBr(BrInst->getConditional()->toLL(M),
                           BranchContainers[0].second,
                           BranchContainers[1].second);
  std::vector<std::pair<llvm::BasicBlock *, llvm::Value *>> PhiIncoming;
  for (auto BBContainer : BranchContainers) {
    K->Builder->SetInsertPoint(BBContainer.second);
    auto BlockValue = BBContainer.first->toValuesLL(M);
    K->Builder->CreateBr(MergeBlockContainer);
    PhiIncoming.push_back(
        std::make_pair(K->Builder->GetInsertBlock(), BlockValue));
  }
  K->Builder->SetInsertPoint(MergeBlockContainer);
  auto VTy = back()->getType();
  if (!isa<VoidType>(VTy)) {
    auto PN = K->Builder->CreatePHI(VTy->toLL(M), 2, "iftmp");
    for (auto In : PhiIncoming) {
      PN->addIncoming(In.second, In.first);
    }
    return PN;
  }
  return nullptr;
}

llvm::Value *BasicBlock::toValuesLL(llvm::Module *M) {
  if (begin() == end())
    return nullptr;
  InstListType::iterator It;
  for (It = begin(); std::next(It) != end(); ++It)
    It->toLL(M);
  auto PossibleBrInst = *It;
  auto Ret = PossibleBrInst->toLL(M);
  if (auto MergeBlock = getMergeBlock())
    return MergeBlock->toValuesLL(M);
  return Ret;
}

llvm::BasicBlock *BasicBlock::toContainerLL(llvm::Module *M) {
  auto K = context();
  auto ParentF = cast<llvm::Function>(Parent->getLoweredValue());
  auto Ret = llvm::BasicBlock::Create(K->LLContext, Name, ParentF);
  K->Builder->SetInsertPoint(Ret);
  return Ret;
}

llvm::Value *BasicBlock::toLL(llvm::Module *M) {
  CHECK_LoweredValue;
  toContainerLL(M);
  auto Ret = toValuesLL(M);
  returni(Ret);
}
}
