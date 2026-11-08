#include <Core/Log.hpp>
#include <Io/File.hpp>

namespace Ti::Core
{
    namespace
    {
        std::recursive_mutex logMutex;
        Ti::Io::File logFile;
    }

    void Log::writeMessage(const std::source_location& location, LogCategory category, Severity severity,
                           std::string_view message)
    {
        std::scoped_lock lock(logMutex);

        std::println("{}[{},{}]{} {} ({}:{})", category.color, category.name, toString(severity), AnsiReset, message,
                     location.file_name(), location.line());

        if (logFile.isOpen())
        {
            const auto line = std::format("[{},{}] {} ({}:{})\n", category.name, toString(severity), message,
                                          location.file_name(), location.line());

            logFile.write(std::as_bytes(std::span<const char>(line.data(), line.size())));
        }
    }

    void Log::setFileOutPut(std::filesystem::path path)
    {

        if (path.has_parent_path())
            std::filesystem::create_directories(path.parent_path());

        std::scoped_lock lock(logMutex);

        logFile = Io::File(path, Io::File::OpenMode::Write);

        if (!logFile.isOpen())
        {
            TI_LOG_FATAL(Core, "Failed to create the log file {}", path.generic_string());
        }
    }

    void Log::setFileOutPut(std::string_view path)
    {
        const std::filesystem::path filePath{std::string(path)};
        setFileOutPut(filePath);
    }

} // namespace Ti::Core
