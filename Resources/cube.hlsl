Texture2D<float4> colorTexture : register(t0);
SamplerState colorSampler : register(s0);

cbuffer TransformBuffer : register(b1)
{
    float4x4 wvp;
};

struct VSInput
{
    float3 position : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    output.position = mul(wvp, float4(input.position, 1.0f));
    output.uv = input.uv;

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET0
{
    return colorTexture.Sample(colorSampler, input.uv);
}