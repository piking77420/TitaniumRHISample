
#ifndef TITANIUM_SAMPLE_CORE_LOG_H
#define TITANIUM_SAMPLE_CORE_LOG_H

#include <Core/EnumToString.hpp>

#include <cstdio>
#include <exception>
#include <format>
#include <mutex>
#include <print>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace Ti::Core
{
    using namespace std::literals;

#define CORE_LOG_SEVERITY_LIST(X)                                                                                      \
    X(Verbose)                                                                                                         \
    X(Info)                                                                                                            \
    X(Warning)                                                                                                         \
    X(Error)                                                                                                           \
    X(Fatal)

    enum class Severity
    {
#define X(name) name,
        CORE_LOG_SEVERITY_LIST(X)
#undef X
    };

    IMPLEMENT_TO_STRING_TITANIUM(Severity, CORE_LOG_SEVERITY_LIST)

    struct LogCategory
    {
        std::string_view name;
        std::string_view color;
    };

    class Log
    {
    public:
        template<Severity severity = Severity::Info, typename... Args>
        static void print(const std::source_location& location, LogCategory category, std::format_string<Args...> fmt,
                          Args&&... args)
        {
            const std::string message = std::format(fmt, std::forward<Args>(args)...);

            {
                std::scoped_lock lock(m_mutex);

                std::println("{}[{},{}]{} {} ({}:{})", category.color, category.name, toString(severity), AnsiReset,
                             message, location.file_name(), location.line());

                if (m_file.is_open())
                {
                    std::format_to(std::ostreambuf_iterator<char>(m_file), "[{},{}] {} ({}:{})\n", category.name,
                                   toString(severity), message, location.file_name(), location.line());

                    if constexpr (severity >= Severity::Error)
                    {
                        m_file.flush();
                    }
                }

                if constexpr (severity == Severity::Fatal)
                    std::fflush(stdout);
            }

            if constexpr (severity == Severity::Fatal)
                std::terminate();
        }

        static void setFileOutPut(std::string_view path);

        static constexpr std::wstring_view wAnsiReset = L"\x1b[0m"sv;
        static constexpr std::wstring_view wAnsiBlack = L"\x1b[30m"sv;
        static constexpr std::wstring_view wAnsiRed = L"\x1b[31m"sv;
        static constexpr std::wstring_view wAnsiGreen = L"\x1b[32m"sv;
        static constexpr std::wstring_view wAnsiYellow = L"\x1b[33m"sv;
        static constexpr std::wstring_view wAnsiOrange = L"\x1b[38;2;255;165;0m"sv;
        static constexpr std::wstring_view wAnsiBlue = L"\x1b[34m"sv;
        static constexpr std::wstring_view wAnsiMagenta = L"\x1b[35m"sv;
        static constexpr std::wstring_view wAnsiCyan = L"\x1b[36m"sv;
        static constexpr std::wstring_view wAnsiWhite = L"\x1b[37m"sv;
        static constexpr std::wstring_view wAnsiGrey = L"\x1b[90m"sv;
        static constexpr std::wstring_view wAnsiBrightRed = L"\x1b[91m"sv;
        static constexpr std::wstring_view wAnsiBrightGreen = L"\x1b[92m"sv;
        static constexpr std::wstring_view wAnsiBrightYellow = L"\x1b[93m"sv;
        static constexpr std::wstring_view wAnsiBrightBlue = L"\x1b[94m"sv;
        static constexpr std::wstring_view wAnsiBrightMagenta = L"\x1b[95m"sv;
        static constexpr std::wstring_view wAnsiBrightCyan = L"\x1b[96m"sv;
        static constexpr std::wstring_view wAnsiBrightWhite = L"\x1b[97m"sv;

        static constexpr std::string_view AnsiReset = "\x1b[0m"sv;
        static constexpr std::string_view AnsiBlack = "\x1b[30m"sv;
        static constexpr std::string_view AnsiRed = "\x1b[31m"sv;
        static constexpr std::string_view AnsiGreen = "\x1b[32m"sv;
        static constexpr std::string_view AnsiYellow = "\x1b[33m"sv;
        static constexpr std::string_view AnsiOrange = "\x1b[38;2;255;165;0m"sv;
        static constexpr std::string_view AnsiBlue = "\x1b[34m"sv;
        static constexpr std::string_view AnsiMagenta = "\x1b[35m"sv;
        static constexpr std::string_view AnsiCyan = "\x1b[36m"sv;
        static constexpr std::string_view AnsiWhite = "\x1b[37m"sv;
        static constexpr std::string_view AnsiGrey = "\x1b[90m"sv;
        static constexpr std::string_view AnsiBrightRed = "\x1b[91m"sv;
        static constexpr std::string_view AnsiBrightGreen = "\x1b[92m"sv;
        static constexpr std::string_view AnsiBrightYellow = "\x1b[93m"sv;
        static constexpr std::string_view AnsiBrightBlue = "\x1b[94m"sv;
        static constexpr std::string_view AnsiBrightMagenta = "\x1b[95m"sv;
        static constexpr std::string_view AnsiBrightCyan = "\x1b[96m"sv;
        static constexpr std::string_view AnsiBrightWhite = "\x1b[97m"sv;

    private:
        static inline std::recursive_mutex m_mutex;
        static inline std::ofstream m_file;
    };

    namespace LogCategories
    {
        inline constexpr LogCategory Core{"Core", Log::AnsiWhite};
        inline constexpr LogCategory Rendering{"Rendering", Log::AnsiCyan};
        inline constexpr LogCategory Assets{"Assets", Log::AnsiYellow};
        inline constexpr LogCategory RHI{"RHI", Log::AnsiMagenta};

    }
}

#define TI_LOG_IMPL(severity, category, ...)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        Ti::Core::Log::print<Ti::Core::Severity::severity>(std::source_location::current(),                            \
                                                           Ti::Core::LogCategories::category, __VA_ARGS__);            \
    } while (false)

#define TI_LOG_VERBOSE(category, ...) TI_LOG_IMPL(Verbose, category, __VA_ARGS__)
#define TI_LOG_INFO(category, ...) TI_LOG_IMPL(Info, category, __VA_ARGS__)
#define TI_LOG_WARNING(category, ...) TI_LOG_IMPL(Warning, category, __VA_ARGS__)
#define TI_LOG_ERROR(category, ...) TI_LOG_IMPL(Error, category, __VA_ARGS__)
#define TI_LOG_FATAL(category, ...) TI_LOG_IMPL(Fatal, category, __VA_ARGS__)

#endif // TITANIUM_SAMPLE_CORE_LOG_H
