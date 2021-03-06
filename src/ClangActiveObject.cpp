#include <clang/AST/AST.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Sema/Sema.h>
#include <llvm/Support/raw_ostream.h>

#include <mstch/mstch.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <fstream>
#include <iostream>
#include <memory>
using namespace clang;

namespace {

class FindInterfaceRecursiveASTVisitor
    : public RecursiveASTVisitor<FindInterfaceRecursiveASTVisitor> {
  public:
    FindInterfaceRecursiveASTVisitor(
        CompilerInstance& compilerInstance,
        const std::string& interfaceName,
        const boost::filesystem::path& activeObjectTemplate)
        : m_compilerInstance(compilerInstance)
        , m_astContext(m_compilerInstance.getASTContext())
        , m_interfaceName(interfaceName)
        , m_activeObjectTemplate(activeObjectTemplate)
    {
    }

  public: // RecursiveASTVisitor
    bool VisitCXXRecordDecl(CXXRecordDecl* declaration)
    {
        if (isInMainFile(declaration) && hasInterface(declaration, m_interfaceName)) {
            mstch::map context{ { "Includes",
                                  mstch::map{ { "InterfaceHeaderFileName",
                                                dropPathDirectory(getFileName(declaration)) } } },
                                { "Class",
                                  mstch::map{
                                      { "InterfaceName", m_interfaceName },
                                      { "InterfaceImplName", m_interfaceName + "ActiveObject" } } },
                                { "Methods", extractMethods(declaration) } };

            mstch::config::escape = [](const std::string& str) -> std::string { return str; };


            boost::filesystem::ifstream activeObjectTemplateStream{m_activeObjectTemplate};
            auto activeObjectTemplate = streamToString(activeObjectTemplateStream);
            std::cout << mstch::render(activeObjectTemplate, context) << std::endl;
            return false;
        } else if (isInMainFile(declaration)) {
            std::cerr << "Found declaration with different interface name" << std::endl;
            declaration->dump();
        }

        return true;
    }

  private:
    mstch::array extractMethods(const CXXRecordDecl* declaration) const
    {
        mstch::array methods;
        auto begin = declaration->method_begin();
        auto end = declaration->method_end();
        for (auto methodIt = begin; methodIt != end; methodIt++) {
            if (methodIt->isUserProvided() && methodIt->isPure()) {
                auto methodName = dropClassPrefix(methodIt->getQualifiedNameAsString());
                auto returnTypeName = methodIt->getReturnType().getAsString();
                auto parameters = std::__cxx11::string("");
                auto parametersWithType = std::__cxx11::string("");

                auto parametersBegin = methodIt->param_begin();
                auto parametersEnd = methodIt->param_end();

                for (auto parameterIt = parametersBegin; parameterIt != parametersEnd;
                     parameterIt++) {
                    auto parameter = *parameterIt;

                    auto printingPolicy = PrintingPolicy{ {} };
                    printingPolicy.adjustForCPlusPlus();

                    auto typeName
                        = QualType::getAsString(parameter->getType().split(), printingPolicy);
                    auto parameterName = (*parameterIt)->getQualifiedNameAsString();
                    parameters += parameterName + ", ";
                    parametersWithType += typeName + " " + parameterName + ", ";
                }

                auto signature = returnTypeName + " " + methodName + "("
                    + removeTrailingComma(parametersWithType) + ")";
                auto functionCall = methodName + "(" + removeTrailingComma(parameters) + ")";

                methods.push_back(mstch::map{ { "Signature", signature },
                                              { "Parameters", removeTrailingComma(parameters) },
                                              { "FunctionCall", functionCall } });
            }
        }
        return methods;
    }

    bool isInMainFile(CXXRecordDecl* declaration) const
    {
        return m_astContext.getSourceManager().isInMainFile(declaration->getLocStart());
    }

    bool hasInterface(CXXRecordDecl* declaration, const std::string& interfaceName) const
    {
        return declaration->getQualifiedNameAsString() == interfaceName;
    }

    std::string getFileName(CXXRecordDecl* declaration) const
    {
        return m_astContext.getSourceManager().getFilename(declaration->getLocStart()).str();
    }

    std::string streamToString(boost::filesystem::ifstream& in) const
    {
        std::stringstream sstr;
        sstr << in.rdbuf();
        return sstr.str();
    }

    std::string dropClassPrefix(const std::string& withClassPrefix) const
    {
        std::string separator = "::";

        auto pos = withClassPrefix.find(separator);

        if (pos == std::string::npos) {
            return withClassPrefix;
        }

        return withClassPrefix.substr(pos + separator.size());
    }

    std::string removeTrailingComma(const std::string& withTrailingComma) const
    {
        std::string comma = ",";

        auto pos = withTrailingComma.rfind(comma);

        if (pos == std::string::npos) {
            return withTrailingComma;
        }

        return withTrailingComma.substr(0, pos);
    }

    std::string dropPathDirectory(const std::string& pathWithDirectory) const
    {
        std::string separator = "/";

        auto pos = pathWithDirectory.rfind(separator);

        if (pos == std::string::npos) {
            return pathWithDirectory;
        }

        return pathWithDirectory.substr(pos + separator.size());
    }

  private:
    CompilerInstance& m_compilerInstance;
    ASTContext& m_astContext;
    const std::string m_interfaceName;
    const boost::filesystem::path m_activeObjectTemplate;
};

class MyASTConsumer : public ASTConsumer {
  public:
    explicit MyASTConsumer(
        CompilerInstance& compilerInstance,
        const std::string& interfaceName,
        const boost::filesystem::path& activeObjectTemplate)
        : m_compilerInstance(compilerInstance)
        , m_interfaceVisitor(m_compilerInstance, interfaceName, activeObjectTemplate)
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
        return llvm::make_unique<MyASTConsumer>(
            compilerInstance, m_interfaceName, m_activeObjectTemplate);
    }

    bool ParseArgs(const CompilerInstance&, const std::vector<std::string>& args) override
    {
        m_interfaceName = args[0];
        m_activeObjectTemplate = boost::filesystem::path(args[1]);
        return true;
    }

  private:
    std::string m_interfaceName;
    boost::filesystem::path m_activeObjectTemplate;
};

} // namespace

static FrontendPluginRegistry::Add<MyPluginASTAction>
    X("clang_active_object", "generate active object from interface");
