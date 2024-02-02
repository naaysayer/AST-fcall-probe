#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang::tooling;
using namespace llvm;
using namespace clang;

class FindFunctionCallVisitor
    : public RecursiveASTVisitor<FindFunctionCallVisitor> {
public:
  explicit FindFunctionCallVisitor(ASTContext *Context) : Context(Context) {}

  virtual bool VisitCallExpr(CallExpr *CE) {
    if (FunctionDecl *FD = CE->getDirectCallee()) {
      llvm::outs() << "Function call: " << FD->getNameAsString()
                   << " args number " << CE->getNumArgs() << "\n";
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

class FindNamedClassConsumer : public ASTConsumer {
public:
  explicit FindNamedClassConsumer(ASTContext *Context) : Visitor(Context) {}

  virtual void HandleTranslationUnit(ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  FindFunctionCallVisitor Visitor;
};

class FindNamedClassAction : public ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(CompilerInstance &Compiler, StringRef InFile) {
    return std::make_unique<FindNamedClassConsumer>(&Compiler.getASTContext());
  }
};

static cl::OptionCategory MyToolCategory("My tool options");
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

int main(int argc,const char **argv) {
   auto parser = CommonOptionsParser::create(argc, argv, MyToolCategory);
   if (!parser) {
       return -1;
   }

   ClangTool Tool(parser->getCompilations(),
                  parser->getSourcePathList());

   return Tool.run(newFrontendActionFactory<FindNamedClassAction>().get());
}
