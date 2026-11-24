
struct PushConstants
{
    float4 color;
};

[[vk::push_constant]]
PushConstants pushConstants;

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    float2 positions[3] =
    {
        float2(0.0, 0.5),
        float2(0.5, -0.5),
        float2(-0.5, -0.5)
    };

    VSOutput output;
    output.position = float4(positions[vertexID], 0.0, 1.0);

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET0
{
    return pushConstants.color;
}
