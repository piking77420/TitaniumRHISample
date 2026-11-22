#ifndef TITANIUM_SAMPLE_CORE_MATH_VIEW_H
#define TITANIUM_SAMPLE_CORE_MATH_VIEW_H

#include <type_traits>
#include <Matrix4x4.hpp>
#include <Vector3.hpp>

namespace Ti::Core::Math
{
    // DirectX convention: LH, row vectors
    // +X right, +Y up, +Z foward
    template<typename T>
    requires(std::is_floating_point_v<T>)
    [[nodiscard]] MathLib::Matrix4x4<T> viewLH(const MathLib::Vector3<T>& eye, const MathLib::Vector3<T>& target,
                                               const MathLib::Vector3<T>& up)
    {
        const MathLib::Vector3<T> forward = (target - eye).normalize();
        const MathLib::Vector3<T> right = MathLib::Vector3<T>::cross(up, forward).normalize();
        const MathLib::Vector3<T> realUp = MathLib::Vector3<T>::cross(forward, right);

        MathLib::Matrix4x4<T> m{};

        m[0][0] = right.getX();
        m[0][1] = right.getY();
        m[0][2] = right.getZ();
        m[0][3] = -MathLib::Vector3<T>::dot(right, eye);

        m[1][0] = realUp.getX();
        m[1][1] = realUp.getY();
        m[1][2] = realUp.getZ();
        m[1][3] = -MathLib::Vector3<T>::dot(realUp, eye);

        m[2][0] = forward.getX();
        m[2][1] = forward.getY();
        m[2][2] = forward.getZ();
        m[2][3] = -MathLib::Vector3<T>::dot(forward, eye);

        m[3][0] = T(0);
        m[3][1] = T(0);
        m[3][2] = T(0);
        m[3][3] = T(1);

        return m;
    }
}

#endif // TITANIUM_SAMPLE_CORE_MATH_VIEW_H
