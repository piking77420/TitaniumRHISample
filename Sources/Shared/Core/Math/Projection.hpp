#ifndef TITANIUM_SAMPLE_CORE_MATH_PROJECTION_H
#define TITANIUM_SAMPLE_CORE_MATH_PROJECTION_H

#include <cmath>
#include <type_traits>
#include <Matrix4x4.hpp>

namespace Ti::Core::Math
{
    // DirectX: LH, depth [0, 1], column vectors, row-major storage
    template<typename T>
    requires(std::is_floating_point_v<T>)
    [[nodiscard]] MathLib::Matrix4x4<T> perspectiveLH(T fovY, T aspectRatio, T nearPlane, T farPlane)
    {
        const T yScale = T(1) / std::tan(fovY / T(2));
        const T xScale = yScale / aspectRatio;

        MathLib::Matrix4x4<T> m = MathLib::Matrix4x4<T>::identity();

        m[0][0] = xScale;
        m[1][1] = yScale;
        m[2][2] = farPlane / (farPlane - nearPlane);
        m[2][3] = -(farPlane * nearPlane) / (farPlane - nearPlane);
        m[3][2] = T(1);

        return m;
    }

    // RH, depth [0, 1], column vectors, row-major storage
    template<typename T>
    requires(std::is_floating_point_v<T>)
    [[nodiscard]] MathLib::Matrix4x4<T> perspectiveRH(T fovY, T aspectRatio, T nearPlane, T farPlane)
    {
        const T yScale = T(1) / std::tan(fovY / T(2));
        const T xScale = yScale / aspectRatio;

        MathLib::Matrix4x4<T> m = MathLib::Matrix4x4<T>::identity();

        m[0][0] = xScale;
        m[1][1] = yScale;
        m[2][2] = farPlane / (nearPlane - farPlane);
        m[2][3] = (farPlane * nearPlane) / (nearPlane - farPlane);
        m[3][2] = T(-1);
        m[3][3] = T(0);

        return m;
    }
}

#endif // TITANIUM_SAMPLE_CORE_MATH_PROJECTION_H
