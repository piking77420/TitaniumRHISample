#include <Io/File.hpp>
#include <fstream>
#include <algorithm>
#include <fstream>
#include <limits>

namespace Ti::Io
{
    File::File(const std::filesystem::path& path, OpenMode mode)
        : m_mode(mode)
    {
        open(path);
    }

    File ::~File()
    {
        close();
    }

    void File::open(const std::filesystem::path& path)
    {
        close();
        m_stream.clear();

        auto flags = std::ios::binary;

        switch (m_mode)
        {
        case OpenMode::Read:
            flags |= std::ios::in;
            break;
        case OpenMode::Write:
            flags |= std::ios::out | std::ios::trunc;
            break;
        case OpenMode::Append:
            flags |= std::ios::out | std::ios::app;
            break;
        }

        m_stream.open(path, flags);
        if (!m_stream.is_open())
            return;

        if (m_mode == OpenMode::Read)
        {
            m_stream.seekg(0, std::ios::end);
            const auto end = m_stream.tellg();

            if (end == std::streampos(-1))
            {
                close();
                return;
            }

            m_size = static_cast<uint64_t>(end);
            m_stream.seekg(0, std::ios::beg);
        }
        else if (m_mode == OpenMode::Append)
        {
            m_stream.seekp(0, std::ios::end);
            const auto end = m_stream.tellp();

            if (end == std::streampos(-1))
            {
                close();
                return;
            }

            m_size = static_cast<uint64_t>(end);
        }

        if (!m_stream)
        {
            close();
        }
    }

    void File::close()
    {
        if (m_stream.is_open())
            m_stream.close();

        m_size = 0;
    }

    bool File::isOpen() const
    {
        return m_stream.is_open();
    }

    size_t File::read(std::span<std::byte> destination)
    {
        if (!isOpen() || m_mode != OpenMode::Read || destination.empty())
            return 0;

        const auto count = static_cast<std::streamsize>(
            std::min<uintmax_t>(destination.size(), std::numeric_limits<std::streamsize>::max()));

        m_stream.read(reinterpret_cast<char*>(destination.data()), count);
        return static_cast<size_t>(m_stream.gcount());
    }

    size_t File::write(std::span<const std::byte> source)
    {
        if (!isOpen() || m_mode == OpenMode::Read || source.empty())
            return 0;

        const auto count = static_cast<std::streamsize>(
            std::min<uintmax_t>(source.size(), std::numeric_limits<std::streamsize>::max()));

        m_stream.write(reinterpret_cast<const char*>(source.data()), count);
        if (!m_stream)
            return 0;

        const auto position = m_stream.tellp();
        if (position != std::streampos(-1))
            m_size = std::max(m_size, static_cast<uint64_t>(position));

        return static_cast<size_t>(count);
    }

    std::unique_ptr<std::byte[]> File::clone()
    {
        if (!isOpen() || m_mode == OpenMode::Write || m_size == 0)
            return {};

        std::unique_ptr<std::byte[]> out = std::make_unique<std::byte[]>(m_size);
        read(std::span(out.get(), m_size));

        return out;
    }

    bool File::seek(uint64_t offset)
    {
        if (!isOpen() || m_mode == OpenMode::Append ||
            offset > static_cast<uint64_t>(std::numeric_limits<std::streamoff>::max()))
        {
            return false;
        }

        // Allow seeking after a read reached EOF.
        if (m_stream.bad())
            return false;

        m_stream.clear();

        if (m_mode == OpenMode::Read)
            m_stream.seekg(static_cast<std::streamoff>(offset), std::ios::beg);
        else
            m_stream.seekp(static_cast<std::streamoff>(offset), std::ios::beg);

        return static_cast<bool>(m_stream);
    }

    uint64_t File::getSize() const
    {
        return m_size;
    }

} // namespace Ti::Io
