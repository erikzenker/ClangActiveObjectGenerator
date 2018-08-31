#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <memory>
using namespace clang;

namespace {

class PrintFunctionsConsumer : public ASTConsumer {
public:
  PrintFunctionsConsumer(CompilerInstance &compilerInstance,
                         std::set<std::string> parsedTemplates)
      : m_compilerInstance(compilerInstance),
        m_parsedTemplates(parsedTemplates) {}

  bool HandleTopLevelDecl(DeclGroupRef declGroupRef) override {
    for (auto &decl : declGroupRef) {
      if (const NamedDecl *ND = dyn_cast<NamedDecl>(decl)) {
        std::cout << "top-level-decl: \"" << ND->getNameAsString() << std::endl;
      }
    }
    return true;
  }

private:
  CompilerInstance &m_compilerInstance;
  std::set<std::string> m_parsedTemplates;
};

class PrintFunctionNamesAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &compilerInstance,
                    llvm::StringRef) override {
    return llvm::make_unique<PrintFunctionsConsumer>(compilerInstance,
                                                    m_parsedTemplates);
  }

  bool ParseArgs(const CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }

private:
  std::set<std::string> m_parsedTemplates;
  };

} // namespace

static FrontendPluginRegistry::Add<PrintFunctionNamesAction>
    X("learn-clang-ast", "learn how clang ast works");
