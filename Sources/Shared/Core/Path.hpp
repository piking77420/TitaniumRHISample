#ifndef TITANIUM_SAMPLE_PATHS_H
#define TITANIUM_SAMPLE_PATHS_H

#include <string_view>

namespace Ti::Paths
{
    inline constexpr std::string_view Saved = "Saved/";
    inline constexpr std::string_view Intermediate = "Intermediate/";
    inline constexpr std::string_view ShaderCache = "Intermediate/Shaders/";
} // namespace Ti::Paths

#endif // TITANIUM_SAMPLE_PATHS_H
