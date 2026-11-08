#ifndef TITATNIUM_SAMPLE_EXEMPLE_H
#define TITATNIUM_SAMPLE_EXEMPLE_H

#include <vector>
#include <Core/Log.hpp>
#include <Plateform/BaseExemple.hpp>
#include <ShaderCompiler.hpp>

namespace Ti::Core::LogCategories
{
    inline constexpr LogCategory Exemple{"Exemple", Log::AnsiGreen};
    inline constexpr LogCategory ShaderCompiler{"ShaderCompiler", Log::AnsiGrey};
} // Ti::Core::LogCategories

namespace Ti::Common
{
    class Exemple : public Ti::Plateform::BaseExemple
    {
    public:
        Exemple() = delete;
        virtual ~Exemple() = default;

        Exemple(std::string name)
            : Ti::Plateform::BaseExemple(std::move(name))
        {
        }

    protected:
        Ti::ShaderCompiler::Compiler m_shaderCompiler;

        void printCompilerError(Ti::ShaderCompiler::Compiler::GetCodeError error);

        struct CompileModuleData
        {
            std::vector<std::byte> data;
            TiRHI::ShaderStage stage;
            std::string entryPoint;
        };

        [[nodiscard]] std::vector<CompileModuleData>
        compileShader(const char* filePath,
                      const std::span<const Ti::ShaderCompiler::Compiler::ShaderEntryPoint>& shaderEntryPoint);
    };

} // Ti::Common

#endif // TITATNIUM_SAMPLE_EXEMPLE_H
