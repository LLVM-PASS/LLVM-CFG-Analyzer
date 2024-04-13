#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/CFG.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <memory>

using namespace llvm;

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <LLVM IR file>" << std::endl;
        return 1;
    }

    LLVMContext context;
    SMDiagnostic err;

    std::unique_ptr<Module> mod = parseIRFile(argv[1], err, context);
    if (!mod) {
        err.print(argv[0], errs());
        return 1;
    }

    Function *mainFunc = mod->getFunction("main");
    if (!mainFunc) {
        std::cout << "Error: 'main' function not found in the IR file." << std::endl;
        return 1;
    }

    if (mainFunc->isDeclaration()) {
        std::cout << "Error: 'main' function is a declaration, not a definition." << std::endl;
        return 1;
    }

    for (BasicBlock &bb : *mainFunc) {
        for (Instruction &i : bb) {
            if (auto *call = dyn_cast<CallBase>(&i)) {
                if (Function *calledFunc = call->getCalledFunction()) {
                    std::cout << "Call to: " << calledFunc->getName().str() << std::endl;
                } else {
                    std::cout << "Indirect call through a function pointer." << std::endl;
                }
            }
        }
    }

    return 0;
}
