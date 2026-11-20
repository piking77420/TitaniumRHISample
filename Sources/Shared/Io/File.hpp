#ifndef TITANIUM_SAMPLE_IO_FILE_H
#define TITANIUM_SAMPLE_IO_FILE_H

#include <fstream>
#include <filesystem>
#include <memory>
#include <span>

namespace Ti::Io
{
    class File
    {
    public:
        enum class OpenMode
        {
            Read,  // Open an existing file for reading.
            Write, // Create or overwrite a file.
            Append // Create or add data to the end.
        };

        File() = default;
        ~File();

        File(const File&) = delete;
        File& operator=(const File&) = delete;

        File(File&&) = default;
        File& operator=(File&&) = default;

        File(const std::filesystem::path& path, OpenMode mode);

        OpenMode getMode() const
        {
            return m_mode;
        }
        bool isOpen() const;

        size_t read(std::span<std::byte> destination);
        size_t write(std::span<const std::byte> source);

        std::unique_ptr<std::byte[]> clone();

        bool seek(uint64_t offset);
        uint64_t getSize() const;

    private:
        std::fstream m_stream;
        OpenMode m_mode = OpenMode::Read;
        uint64_t m_size = 0;

        void open(const std::filesystem::path& path);

        void close();
    };

} // Ti::Io

#endif // TITANIUM_SAMPLE_IO_FILE_H
