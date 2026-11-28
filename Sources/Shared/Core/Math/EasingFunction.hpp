#ifndef TITANIUM_SAMPLE_CORE_MATH_EASING_FUNCTION_H
#define TITANIUM_SAMPLE_CORE_MATH_EASING_FUNCTION_H

#include <type_traits>

namespace Ti::Core::Math::Easing
{
    template<typename T, typename F>
    requires(std::is_floating_point_v<F>)
    T lerp(const T& t1, const T& t2, F t)
    {
        return t1 * (static_cast<F>(1.0) - t) + t2 * t;
    }

    template<typename T>
    requires(std::is_floating_point_v<T>)
    float exponentialEaseOut(T smoothness, T deltaTime)
    {
        return 1.0f - std::exp(-smoothness * deltaTime);
    }
}

#endif // TITANIUM_SAMPLE_CORE_MATH_EASING_FUNCTION_H
