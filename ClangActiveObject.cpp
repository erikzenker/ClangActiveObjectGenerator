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

class FindInterfaceRecursiveASTVisitor
    : public RecursiveASTVisitor<FindInterfaceRecursiveASTVisitor> {
  public:
    FindInterfaceRecursiveASTVisitor(
        CompilerInstance& compilerInstance, std::string&& interfaceName)
        : m_compilerInstance(compilerInstance)
        , m_astContext(m_compilerInstance.getASTContext())
        , m_interfaceName(interfaceName)
    {
    }

  public: // RecursiveASTVisitor
    bool VisitCXXRecordDecl(CXXRecordDecl* declaration)
    {
        if (isInMainFile(declaration)) {
            if (declaration->getQualifiedNameAsString() == m_interfaceName) {
                auto begin = declaration->method_begin();
                auto end = declaration->method_end();
                for(auto methodIt = begin; methodIt != end; methodIt++){
                    if(methodIt->isUserProvided()){
                        methodIt->dump();
                    }
                }
            }
        }
        return true;
    }

  private:
    bool isInMainFile(CXXRecordDecl* declaration)
    {
        return m_astContext.getSourceManager().isInMainFile(declaration->getBeginLoc());
    }

  private:
    CompilerInstance& m_compilerInstance;
    ASTContext& m_astContext;
    const std::string m_interfaceName;
};

class MyASTConsumer : public ASTConsumer {
  public:
    explicit MyASTConsumer(CompilerInstance& compilerInstance)
        : m_compilerInstance(compilerInstance)
        , m_interfaceVisitor(m_compilerInstance, "ICalculator")
    {
    }

    /*
     * Called when the AST of the whole translation unit was parsed.
     * It provide the AST by the AST context which can be
     * processed by an recursive AST visitor.
     */
    void HandleTranslationUnit(ASTContext& astContext) override
    {
        m_interfaceVisitor.TraverseDecl(astContext.getTranslationUnitDecl());
    }

  private:
    CompilerInstance& m_compilerInstance;
    FindInterfaceRecursiveASTVisitor m_interfaceVisitor;
};

class MyPluginASTAction : public PluginASTAction {
  protected:
    std::unique_ptr<ASTConsumer>
    CreateASTConsumer(CompilerInstance& compilerInstance, llvm::StringRef) override
    {
        return llvm::make_unique<MyASTConsumer>(compilerInstance);
    }

    bool ParseArgs(const CompilerInstance&, const std::vector<std::string>&) override
    {
        return true;
    }
};

} // namespace

static FrontendPluginRegistry::Add<MyPluginASTAction>
    X("clang-active-object", "generate active object from interface");
