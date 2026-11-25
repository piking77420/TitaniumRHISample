#include <Core/Camera.hpp>
#include <Core/Math/View.hpp>
#include <Core/Math/Projection.hpp>

namespace Ti::Core
{

    void Camera::handleDirty()
    {
        if (m_isDirtyView)
            computeView();

        if (m_isDirtyProjection)
            computeProjection();

        if (m_isDirtyProjection || m_isDirtyView)
            computeViewProjection();

        m_isDirtyProjection = false;
        m_isDirtyView = false;
    }

    void Camera::computeProjection()
    {
        m_projection = Core::Math::perspectiveRH(m_fov, m_aspectRatio, m_near, m_far);
        m_projectionInverse = m_projection.getInverse();
    }

    void Camera::computeView()
    {
        m_view = Core::Math::viewRH(m_eye, m_at, m_up);
        m_viewInverse = m_view.getInverse();
    }

    void Camera::computeViewProjection()
    {
        m_viewProjection = m_projection * m_view;
        m_viewProjectionInverse = m_viewInverse * m_projectionInverse;
    }

    Camera& Camera::setLookAt(const MathLib::Vector3f& eye, const MathLib::Vector3f& at, const MathLib::Vector3f& up)
    {
        m_eye = eye;
        m_at = at;
        m_up = up;
        m_isDirtyView = true;
        return *this;
    }

} // Ti::Core
