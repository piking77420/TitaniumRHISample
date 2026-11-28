
#ifndef TITANIUM_SAMPLE_CORE_CAMERA_H
#define TITANIUM_SAMPLE_CORE_CAMERA_H

#include <Matrix4x4.hpp>

namespace Ti::Core
{
    class Camera
    {
    public:
        Camera() = default;
        ~Camera() = default;

        float getFov() const
        {
            return m_fov;
        }

        Camera& setFov(float newFov)
        {
            m_fov = newFov;
            m_isDirtyProjection = true;

            return *this;
        }

        float getAspectRatio() const
        {
            return m_aspectRatio;
        }

        Camera& setAspectRation(float aspectRatio)
        {
            m_aspectRatio = aspectRatio;
            m_isDirtyProjection = true;

            return *this;
        }

        float getNear() const
        {
            return m_near;
        }

        Camera& setNear(float nearPlane)
        {
            m_near = nearPlane;
            m_isDirtyProjection = true;

            return *this;
        }

        float getFar() const
        {
            return m_far;
        }

        Camera& setFar(float farPlane)
        {
            m_far = farPlane;
            m_isDirtyProjection = true;

            return *this;
        }

        Camera& setAspectRatio(float aspectRatio)
        {
            return setAspectRation(aspectRatio);
        }

        const MathLib::Matrix4x4f& getProjection() const noexcept
        {
            return m_projection;
        }

        const MathLib::Matrix4x4f& getView() const noexcept
        {
            return m_view;
        }

        const MathLib::Matrix4x4f& getViewProjection() const noexcept
        {
            return m_viewProjection;
        }

        const MathLib::Matrix4x4f& getProjectionInverse() const noexcept
        {
            return m_projectionInverse;
        }

        const MathLib::Matrix4x4f& getViewInverse() const noexcept
        {
            return m_viewInverse;
        }

        const MathLib::Matrix4x4f& getViewProjectionInverse() const noexcept
        {
            return m_viewProjectionInverse;
        }

        Camera& setLookAt(const MathLib::Vector3f& eye, const MathLib::Vector3f& at, const MathLib::Vector3f& up);

        MathLib::Vector3f getEye() const
        {
            return m_eye;
        }

        Camera& setEye(const MathLib::Vector3f& eye)
        {
            m_eye = eye;
            m_isDirtyView = true;
            return *this;
        }

        MathLib::Vector3f getAt() const
        {
            return m_at;
        }

        Camera& setAt(const MathLib::Vector3f& at)
        {
            m_at = at;
            m_isDirtyView = true;
            return *this;
        }

        MathLib::Vector3f getUp()
        {
            return m_up;
        }

        Camera& setUp(const MathLib::Vector3f& at)
        {
            m_at = at;
            m_isDirtyView = true;
            return *this;
        }

        void handleDirty();

    private:
        MathLib::Matrix4x4f m_projection = MathLib::Matrix4x4f::identity();

        MathLib::Matrix4x4f m_view = MathLib::Matrix4x4f::identity();

        MathLib::Matrix4x4f m_viewProjection = MathLib::Matrix4x4f::identity();

        MathLib::Matrix4x4f m_projectionInverse = MathLib::Matrix4x4f::identity();

        MathLib::Matrix4x4f m_viewInverse = MathLib::Matrix4x4f::identity();

        MathLib::Matrix4x4f m_viewProjectionInverse = MathLib::Matrix4x4f::identity();

        MathLib::Vector3f m_eye = MathLib::Vector3f::zero();

        MathLib::Vector3f m_at = MathLib::Vector3f(0, 0, -1);

        MathLib::Vector3f m_up = MathLib::Vector3f::unitY();

        float m_fov = 1.39626f;

        float m_aspectRatio = 0.6;

        float m_near = 0.1f;

        float m_far = 1000.0f;

        bool m_isDirtyProjection = false;

        bool m_isDirtyView = false;

        void computeView();

        void computeProjection();

        void computeViewProjection();
    };

} // Ti::Core

#endif // TITANIUM_SAMPLE_CORE_CAMERA_H
