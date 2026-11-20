#include <Rendering/GpuResource.hpp>

namespace Ti::Rendering
{
    GpuResource::GpuResource() = default;

    GpuResource::~GpuResource() = default;

    GpuResource::GpuResource(GpuResource&&) = default;

    GpuResource::GpuResource(const GpuResource&) = default;

    GpuResource& GpuResource::operator=(const GpuResource&) = default;

    GpuResource& GpuResource::operator=(GpuResource&&) = default;

}
