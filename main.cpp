#include <vector>

#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Value.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

static LLVMContext mContext;
static IRBuilder<> mBuilder(mContext);
static std::unique_ptr<Module> mModule = make_unique<Module>("example", mContext);
static Module *M = mModule.get();

static Type *dType = Type::getDoubleTy(mContext);
static Type *i32 = IntegerType::get(mContext, 32);

// helper functions
static AllocaInst *entryCreateBlockAllocaType(Function *func, std::string name, Type* type) {
  IRBuilder<> tmpBuilder(&func->getEntryBlock(), func->getEntryBlock().begin());
  return tmpBuilder.CreateAlloca(type, nullptr, name);
}

static ArrayRef<Value *> PrefixZero (Value *index) {
  std::vector<Value *> out;
  out.push_back(ConstantInt::get(mContext, APInt(32, 0)));
  out.push_back(index);
  return ArrayRef<Value *>(out);
}

static AllocaInst *createVariable () {
  auto *func = mBuilder.GetInsertBlock()->getParent();
  auto *initValue = ConstantInt::get(mContext, APInt(32, 0));

  auto *alloca = entryCreateBlockAllocaType(func, "var", initValue->getType());
  mBuilder.CreateStore(initValue, alloca);
  return alloca; 
}

static std::vector<Type *> elementTypes (3, dType);
static AllocaInst *createStruct () {
  auto *func = mBuilder.GetInsertBlock()->getParent();

  auto *mStructType = StructType::get(mContext, elementTypes);
  return entryCreateBlockAllocaType(func, "str", mStructType);
}

int main () {
  // create a main function
  auto *FT = FunctionType::get(i32, std::vector<Type *>(), false);
  auto *f = Function::Create(FT, Function::ExternalLinkage, "main", M);
 
  // set insert point for out below code
  auto *bb = BasicBlock::Create(mContext, "entry", f);
  mBuilder.SetInsertPoint(bb);

  // Create a variable
  auto *variable = createVariable();
  // create a struct
  auto *mStruct = createStruct();

  // Create a GEP with the loaded index
  auto *loadedVar = mBuilder.CreateLoad(variable, "loaded_index");
  auto *elementPtr = mBuilder.CreateGEP(mStruct, PrefixZero(loadedVar));

  mBuilder.CreateRet(ConstantInt::get(mContext, APInt(32, 0))); 
  f->print(errs()); // print out the function

  return 1;
}
