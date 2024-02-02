#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/JSON.h"

using namespace clang::tooling;
using namespace llvm;
using namespace clang;

using FuncCallInfo = struct {
  std::string name;
  std::vector<std::string> args;
  struct {
    unsigned line;
    unsigned column;
    std::string file;
  } location;
};

static std::vector<FuncCallInfo> functionCalls;

class FindFunctionCallVisitor
    : public RecursiveASTVisitor<FindFunctionCallVisitor> {
public:
  explicit FindFunctionCallVisitor(SourceManager *SM, ASTContext *Context)
      : SM(SM), Context(Context) {}

  virtual bool VisitCallExpr(CallExpr *CE) {
    if (FunctionDecl *FD = CE->getDirectCallee()) {

      SourceLocation loc = FD->getLocation();
      if (SM->isInSystemHeader(loc)) {
        return true;
      }

      FuncCallInfo info;

      FileID FID = SM->getFileID(loc);

      info.name = FD->getNameAsString();
      info.args.reserve(CE->getNumArgs());

      info.location.file = SM->getFilename(FD->getSourceRange().getBegin());
      info.location.line = SM->getLineNumber(FID, SM->getSpellingLineNumber(loc));
      info.location.column = SM->getColumnNumber(FID, SM->getSpellingColumnNumber(loc));

      QualType type;
      auto **Args = CE->getArgs();
      for (size_t i = 0; i < CE->getNumArgs(); ++i) {
          info.args.push_back(Args[i]->getType().getAsString());
      }

      functionCalls.push_back(info);
    }
    return true;
  }

private:

  ASTContext *Context;
  SourceManager *SM;
};

class FindNamedClassConsumer : public ASTConsumer {
public:
  explicit FindNamedClassConsumer(SourceManager *SM, ASTContext *Context)
      : SM(SM), Visitor(SM, Context) {}

  virtual void HandleTranslationUnit(ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  SourceManager *SM;
  FindFunctionCallVisitor Visitor;
};

class FindNamedClassAction : public ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(CompilerInstance &Compiler, StringRef InFile) {
    return std::make_unique<FindNamedClassConsumer>(&Compiler.getSourceManager(), &Compiler.getASTContext());
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

   int ret =  Tool.run(newFrontendActionFactory<FindNamedClassAction>().get());


   for (auto info: functionCalls) {
       llvm::outs() << "FC: "
           << info.name
           << " at " << info.location.file
           << " " << info.location.line << ":" << info.location.column
           << " args: ";

       for (auto arg: info.args) {
           llvm::outs() << arg << " ";
       }

       llvm::outs() << "\n";
   }
}
