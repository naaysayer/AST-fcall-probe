#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;

class FindFunctionCallVisitor
    : public RecursiveASTVisitor<FindFunctionCallVisitor> {
public:
  explicit FindFunctionCallVisitor(ASTContext *Context) : Context(Context) {}

  virtual bool VisitCallExpr(CallExpr *CE) {
    if (FunctionDecl *FD = CE->getDirectCallee()) {
      llvm::outs() << "Function call: " << FD->getNameAsString()
          << " args number " << CE->getNumArgs()
          <<"\n";
      auto **Args = CE->getArgs();

      for (size_t i = 0; i < CE->getNumArgs(); ++i) {
          llvm::outs() << "Arg[" << i << "] " << Args[i]->getType() << "\n";
      }
    }
    return true;
  }

private:
  ASTContext *Context;
};

class FindNamedClassConsumer : public clang::ASTConsumer {
public:
  explicit FindNamedClassConsumer(ASTContext *Context) : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  FindFunctionCallVisitor Visitor;
};

class FindNamedClassAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::make_unique<FindNamedClassConsumer>(&Compiler.getASTContext());
  }
};

int main(int argc, char **argv) {
  if (argc > 1) {
    clang::tooling::runToolOnCode(std::make_unique<FindNamedClassAction>(),
                                  argv[1]);
  }
}
