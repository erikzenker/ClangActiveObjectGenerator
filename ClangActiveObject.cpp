#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

#include <mstch/mstch.hpp>

#include <fstream>
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
                auto activeObjectTemplateStream
                    = std::ifstream("ActiveObject.mustache", std::ios::in);
                auto activeObjectTemplate = streamToString(activeObjectTemplateStream);

                auto interfaceHeaderFileName = getFileName(declaration);
                auto interfaceName = m_interfaceName;
                auto interfaceImplName = m_interfaceName + "ActiveObject";

                mstch::array context{ { mstch::map{
                    { "InterfaceHeaderFileName", getFileName(declaration) },
                    { "InterfaceName", m_interfaceName },
                    { "InterfaceImplName", m_interfaceName + "ActiveObject" } } } };

                mstch::array methods;

                auto begin = declaration->method_begin();
                auto end = declaration->method_end();
                for (auto methodIt = begin; methodIt != end; methodIt++) {
                    if (methodIt->isUserProvided() && methodIt->isPure()) {
                        methods.push_back(mstch::array{
                            mstch::map{
                                { "Signature", std::string{ "void foo(int a) override" } } },
                            mstch::map{ { "Parameters", std::string{ "a" } } },
                            mstch::map{ { "FunctionCall", std::string{ "foo(a)" } } } });
                    }
                }

                context.push_back(mstch::map{ { "Methods", methods } });
                std::cout << mstch::render(activeObjectTemplate, context) << std::endl;
            }
        }
        return true;
    }

  private:
    bool isInMainFile(CXXRecordDecl* declaration)
    {
        return m_astContext.getSourceManager().isInMainFile(declaration->getBeginLoc());
    }

    std::string getFileName(CXXRecordDecl* declaration)
    {
        return m_astContext.getSourceManager().getFilename(declaration->getBeginLoc());
    }

    std::string streamToString(std::ifstream& in)
    {
        std::stringstream sstr;
        sstr << in.rdbuf();
        return sstr.str();
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
