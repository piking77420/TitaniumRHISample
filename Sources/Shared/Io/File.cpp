#include <Io/File.hpp>
#include <fstream>

namespace Ti::Io
{
    bool File::loadFileBinary(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path) || std::filesystem::is_directory(path))
            return false;

        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file)
            return false;

        const size_t size = file.tellg();

        if (size <= 0 || size > std::numeric_limits<uint32_t>::max())
            throw std::runtime_error("Invalid shader file size");

        auto data = std::make_unique<std::byte[]>(static_cast<size_t>(size));

        file.seekg(0, std::ios::beg);

        if (!file.read(reinterpret_cast<char*>(data.get()), size))
            return false;

        m_data = std::move(data);
        m_size = static_cast<uint32_t>(size);
        m_alignement = 1u;

        return true;
    }
} // namespace Ti::Io
