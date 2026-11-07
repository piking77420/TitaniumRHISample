#include <Core/Log.hpp>

namespace Ti::Core
{
    void Log::setFileOutPut(std::string_view path)
    {
        std::scoped_lock lock(m_mutex);

        if (path.empty())
        {
            m_file.close();
            return;
        }
        const std::filesystem::path filePath{std::string(path)};

        if (filePath.has_parent_path())
            std::filesystem::create_directories(filePath.parent_path());

        std::ofstream file(filePath, std::ios::out | std::ios::trunc);

        if (!file.is_open())
        {
            TI_LOG_FATAL(Core, "Failed to create the log file {}", path);
        }

        m_file = std::move(file);
    }

} // namespace Ti::Core
