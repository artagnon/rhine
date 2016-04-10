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
  auto K = getContext();
  assert(succ_size() == 2 && "Only 2 successors allowed for now");
  auto BranchBlock = K->Builder->GetInsertBlock();
  auto BranchContainers = zipSuccContainers(M);
  auto MergeBlockContainer = getMergeBlock()->toContainerLL(M);
  MergeBlockContainer->setName("merge");
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
  if (InstList.begin() == InstList.end())
    return nullptr;
  std::vector<Instruction *>::iterator It;
  for (It = InstList.begin(); std::next(It) != InstList.end(); ++It)
    (*It)->toLL(M);
  auto PossibleBrInst = *It;
  auto Ret = PossibleBrInst->toLL(M);
  if (auto MergeBlock = getMergeBlock())
    return MergeBlock->toValuesLL(M);
  return Ret;
}

llvm::BasicBlock *BasicBlock::toContainerLL(llvm::Module *M) {
  auto K = getContext();
  auto ParentF = cast<llvm::Function>(Parent->getLoweredValue());
  auto Ret = llvm::BasicBlock::Create(K->LLContext, Name, ParentF);
  K->Builder->SetInsertPoint(Ret);
  return Ret;
}

llvm::Value *BasicBlock::toLL(llvm::Module *M) {
  toContainerLL(M);
  return toValuesLL(M);
}
}
