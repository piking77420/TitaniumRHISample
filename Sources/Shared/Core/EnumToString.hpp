#ifndef TITANIUM_SAMPLE_CORE_ENUM_TO_STRING_H
#define TITANIUM_SAMPLE_CORE_ENUM_TO_STRING_H

#include <string>
#include <string_view>
#include <utility>

#define WIDEN_IMPL_TITANIUM(x) L##x
#define WIDEN_TITANIUM(x) WIDEN_IMPL_TITANIUM(x)

#define TO_STRING_CASE_TITANIUM(name)                                                                                  \
    case EnumT::name:                                                                                                  \
        return #name;

#define TO_WSTRING_CASE_TITANIUM(name)                                                                                 \
    case EnumT::name:                                                                                                  \
        return WIDEN_TITANIUM(#name);

#define IMPLEMENT_TO_STRING_TITANIUM(EnumType, List)                                                                   \
    constexpr std::string_view toString(EnumType value)                                                                \
    {                                                                                                                  \
        using EnumT = EnumType;                                                                                        \
        switch (value)                                                                                                 \
        {                                                                                                              \
            List(TO_STRING_CASE_TITANIUM)                                                                              \
        }                                                                                                              \
        return "Unknown";                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    constexpr std::wstring_view toWString(EnumType value)                                                              \
    {                                                                                                                  \
        using EnumT = EnumType;                                                                                        \
        switch (value)                                                                                                 \
        {                                                                                                              \
            List(TO_WSTRING_CASE_TITANIUM)                                                                             \
        }                                                                                                              \
        return L"Unknown";                                                                                             \
    }

#define TO_STRING_FLAG_CASE_TITANIUM(name, value)                                                                      \
    if constexpr (value != 0)                                                                                          \
    {                                                                                                                  \
        if ((bits & value) == value)                                                                                   \
        {                                                                                                              \
            if (!result.empty())                                                                                       \
                result += " | ";                                                                                       \
            result += #name;                                                                                           \
        }                                                                                                              \
    }

#define TO_WSTRING_FLAG_CASE_TITANIUM(name, value)                                                                     \
    if constexpr (value != 0)                                                                                          \
    {                                                                                                                  \
        if ((bits & value) == value)                                                                                   \
        {                                                                                                              \
            if (!result.empty())                                                                                       \
                result += L" | ";                                                                                      \
            result += WIDEN_TITANIUM(#name);                                                                           \
        }                                                                                                              \
    }

#define IMPLEMENT_TO_STRING_FLAGS_TITANIUM(EnumType, List)                                                             \
    inline std::string toString(EnumType flags)                                                                        \
    {                                                                                                                  \
        if (flags == EnumType::None)                                                                                   \
            return "None";                                                                                             \
                                                                                                                       \
        const auto bits = std::to_underlying(flags);                                                                   \
        std::string result;                                                                                            \
        List(TO_STRING_FLAG_CASE_TITANIUM) return result.empty() ? "Unknown" : result;                                 \
    }                                                                                                                  \
                                                                                                                       \
    inline std::wstring toWString(EnumType flags)                                                                      \
    {                                                                                                                  \
        if (flags == EnumType::None)                                                                                   \
            return L"None";                                                                                            \
                                                                                                                       \
        const auto bits = std::to_underlying(flags);                                                                   \
        std::wstring result;                                                                                           \
        List(TO_WSTRING_FLAG_CASE_TITANIUM) return result.empty() ? L"Unknown" : result;                               \
    }

#endif // TITANIUM_SAMPLE_CORE_ENUM_TO_STRING_H
