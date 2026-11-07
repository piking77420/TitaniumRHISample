Texture2D<float4> colorTexture : register(t0);
SamplerState colorSampler : register(s0);

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 color : COLOR0;
    float2 uv : TEXCOORD0;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    float2 positions[3] =
    {
        float2(0.0, 0.5),
        float2(0.5, -0.5),
        float2(-0.5, -0.5)
    };

    float2 uvs[3] =
    {
        float2(0.5, 0.0),
        float2(1.0, 1.0),
        float2(0.0, 1.0)
    };

    float3 colors[3] =
    {
        float3(1, 0, 0),
        float3(0, 1, 0),
        float3(0, 0, 1)
    };

    VSOutput output;
    output.position = float4(positions[vertexID], 0.0, 1.0);
    output.color = colors[vertexID];
    output.uv = uvs[vertexID];

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET0
{
    float4 texColor = colorTexture.Sample(colorSampler, input.uv);

    return texColor;
}