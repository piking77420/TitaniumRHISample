#ifndef TITANIUM_SAMPLE_RENDERING_GPU_RESOURCE_H
#define TITANIUM_SAMPLE_RENDERING_GPU_RESOURCE_H

namespace Ti::Rendering
{
    class GpuResource
    {
    public:
        GpuResource();
        virtual ~GpuResource();
        GpuResource(GpuResource&&);
        GpuResource(const GpuResource&);
        GpuResource& operator=(const GpuResource&);
        GpuResource& operator=(GpuResource&&);

    private:
    };

} // namespace Ti::Rendering

#endif // TITANIUM_SAMPLE_RENDERING_GPU_RESOURCE_H
