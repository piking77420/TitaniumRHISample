#ifndef TITANIUM_SAMPLE_IO_FILE_H
#define TITANIUM_SAMPLE_IO_FILE_H

#include <memory>
#include <filesystem>

namespace Ti::Io
{
    class File
    {
    public:
        File() = default;
        ~File() = default;

        bool loadFileBinary(const std::filesystem::path& path);

        const std::byte* getData() const
        {
            return m_data.get();
        }

        uint32_t getSize() const
        {
            return m_size;
        }

        uint32_t getAlignement() const
        {
            return m_alignement;
        }

    private:
        std::unique_ptr<std::byte[]> m_data;

        uint32_t m_alignement = 0;

        uint32_t m_size = 0;
    };

} // Ti::Io

#endif // TITANIUM_SAMPLE_IO_FILE_H
