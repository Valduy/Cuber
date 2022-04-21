struct TransformData
{
	matrix proj;
	float2 pos;
	float dummy0;
	float dummy1;
};

cbuffer TransformBuffer : register(b0)
{
	TransformData Transform;
}

Texture2D DiffuseMap : register(t0);

SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_IN
{
	float2 pos : POSITION0;
	float2 tex : TEXCOORD0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;

	float2 pos = input.pos + Transform.pos;
	output.pos = mul(float4(pos, 0.5, 1.0), Transform.proj);
	output.tex = input.tex;

	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 diffuse = DiffuseMap.Sample(Sampler, input.tex);
	return diffuse;
}