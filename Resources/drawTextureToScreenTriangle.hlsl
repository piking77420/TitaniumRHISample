Texture2D<float4> colorTexture : register(t0);
SamplerState colorSampler : register(s0);

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;

    output.uv = float2(
        (vertexID << 1) & 2,
        vertexID & 2
    );

    output.position = float4(
        output.uv * float2(2.0f, -2.0f)
                  + float2(-1.0f, 1.0f),
        0.0f,
        1.0f
    );

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET0
{
    return colorTexture.Sample(colorSampler, input.uv);
}