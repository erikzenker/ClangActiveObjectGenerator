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

class MyASTConsumer : public ASTConsumer {
  public:
    MyASTConsumer(
        CompilerInstance& compilerInstance, std::set<std::string> parsedTemplates)
        : m_compilerInstance(compilerInstance)
        , m_parsedTemplates(parsedTemplates)
    {
    }

    /*
     * Called for all top level declarations e.g.:
     *  - free standing functions
     *  - classes and structs
     *  - global variables
     */
    bool HandleTopLevelDecl(DeclGroupRef declGroupRef) override
    {
        for (auto& decl : declGroupRef) {
            if (const NamedDecl* namedDecl = dyn_cast<NamedDecl>(decl)) {
                std::cout << "top-level-decl: \"" << namedDecl->getNameAsString() << " "
                          << namedDecl->getQualifiedNameAsString() << std::endl;
            }
        }
        return true;
    }

  private:
    CompilerInstance& m_compilerInstance;
    std::set<std::string> m_parsedTemplates;
};

class MyPluginASTAction : public PluginASTAction {
  protected:
    std::unique_ptr<ASTConsumer>
    CreateASTConsumer(CompilerInstance& compilerInstance, llvm::StringRef) override
    {
        return llvm::make_unique<MyASTConsumer>(compilerInstance, m_parsedTemplates);
    }

    bool ParseArgs(const CompilerInstance&, const std::vector<std::string>&) override
    {
        return true;
    }

  private:
    std::set<std::string> m_parsedTemplates;
};

} // namespace

static FrontendPluginRegistry::Add<MyPluginASTAction>
    X("clang-active-object", "generate active object from interface");
