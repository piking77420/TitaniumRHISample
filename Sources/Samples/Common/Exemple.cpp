#include <Common/Exemple.hpp>
#include <Io/File.hpp>

namespace Ti::Common
{
    void Exemple::printCompilerError(Ti::ShaderCompiler::Compiler::GetCodeError error)
    {
        using namespace Ti::ShaderCompiler;

        switch (error)
        {
        case Compiler::GetCodeError::EmptyEntryPoints:
            TI_LOG_FATAL(Exemple, "Empty entry points");
            break;

        case Compiler::GetCodeError::InvalidSourceCode:
            TI_LOG_FATAL(Exemple, "Invalid source code");
            break;

        case Compiler::GetCodeError::LoadingCodeFailed:
            TI_LOG_FATAL(Exemple, "Failed to load shader code");
            break;

        case Compiler::GetCodeError::InvalidShaderStage:
            TI_LOG_FATAL(Exemple, "Invalid shader stage");
            break;

        case Compiler::GetCodeError::InvalidApi:
            TI_LOG_FATAL(Exemple, "Invalid API target");
            break;

        case Compiler::GetCodeError::ShaderCompilationFailed:
            TI_LOG_FATAL(Exemple, "Shader compilation failed");
            break;

        case Compiler::GetCodeError::UnexpectedError:
            TI_LOG_FATAL(Exemple, "Unexpected error");
            break;

        default:
            TI_LOG_FATAL(Exemple, "Unknown shader compiler error");
            break;
        }
    }

    auto Common::Exemple::compileShader(
        const char* filePath, const std::span<const Ti::ShaderCompiler::Compiler::ShaderEntryPoint>& shaderEntryPoint)
        -> std::vector<CompileModuleData>
    {
        namespace fs = std::filesystem;
        using namespace Ti::ShaderCompiler;

        size_t size = 0;
        std::unique_ptr<std::byte[]> data;
        {
            Ti::Io::File file(filePath, Ti::Io::File::OpenMode::Read);

            if (!file.isOpen())
                return {};

            if (file.getSize() == 0)
                return {};

            size = file.getSize();
            std::unique_ptr data2 = std::make_unique<std::byte[]>(size);
            file.read(std::span(data2.get(), size));
            data = std::move(data2);
        }

        auto getShaderCompilerApi = [&]()
        {
            switch (getRHI().getApi())
            {
            case TiRHI::RhiApi::DirectX12:
                return Compiler::ApiTarget::Dxil;
            case TiRHI::RhiApi::Metal:
                return Compiler::ApiTarget::Spirv;
            case TiRHI::RhiApi::Vulkan:
                return Compiler::ApiTarget::Spirv;
            default:
                break;
            }
            return Compiler::ApiTarget::Spirv;
        };

        Compiler::GetCodeInfo info;
        // clang-format off
        info.setCode(std::span(data.get(), size))
        .setApiTarget(getShaderCompilerApi())
        .setEntryPoints(shaderEntryPoint);

        std::expected modulesCode = m_shaderCompiler.getCode(info);

        if (!modulesCode)
        {
            const Compiler::CompileError& error = modulesCode.error();
            printCompilerError(std::get<Compiler::GetCodeError>(error));

            return {};
        }

        std::vector<std::vector<std::byte>>& codes = modulesCode.value();

        if (codes.size() != shaderEntryPoint.size())
        {
            TI_LOG_ERROR(ShaderCompiler, "Missmatch size between entry point and module code");
            return{};
        }

        auto toStringFromW = [](const std::wstring_view& ws)
            {
                return std::string{ws.begin(), ws.end()};
            };

        auto toRhi = [](Ti::ShaderCompiler::Compiler::ShaderStage compilerStage) -> TiRHI::ShaderStage
            {
                switch (compilerStage)
	            {
		            case Ti::ShaderCompiler::Compiler::ShaderStage::Vertex:
                     return TiRHI::ShaderStage::Vertex;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::Hull:
                     return TiRHI::ShaderStage::Hull;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::Domain:
                     return TiRHI::ShaderStage::Domain;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::Geometry:
                     return TiRHI::ShaderStage::Geometry;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::Pixel:
                     return TiRHI::ShaderStage::Pixel;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::Amplification:
                     return TiRHI::ShaderStage::Amplification;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::Mesh:
                     return TiRHI::ShaderStage::Mesh;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::Compute:
                     return TiRHI::ShaderStage::Compute;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::RayGeneration:
                     return TiRHI::ShaderStage::RayGeneration;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::AnyHit:
                     return TiRHI::ShaderStage::AnyHit;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::ClosestHit:
                     return TiRHI::ShaderStage::ClosestHit;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::Miss:
                     return TiRHI::ShaderStage::Miss;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::Intersection:
                     return TiRHI::ShaderStage::Intersection;
                    case Ti::ShaderCompiler::Compiler::ShaderStage::Callable:
                     return TiRHI::ShaderStage::Callable;
                    default:
                     TI_LOG_ERROR(ShaderCompiler, "Unknow shader stage");
	            }

                return {};
            };

        std::vector<CompileModuleData> compileModuleData;
        compileModuleData.reserve(codes.size());

        for (size_t i = 0; i < codes.size(); i++)
		{
			 auto& moduleData = compileModuleData.emplace_back();
             moduleData.data = std::move(codes[i]);
             moduleData.entryPoint = toStringFromW(shaderEntryPoint[i].entryPoint);
             moduleData.stage = toRhi(shaderEntryPoint[i].stage);
		}

        return compileModuleData;
    }

} // Ti::Common
