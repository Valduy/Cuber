struct TransformData
{
	matrix world_view_proj;
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
	float3 pos : POSITION0;
	float3 norm : NORMAL0;
	float2 tex : TEXCOORD0;
};

struct PS_IN
{
	float4 pos : SV_POSITION0;
	float2 tex : TEXCOORD0;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;

	output.pos = mul(float4(input.pos, 1.0), Transform.world_view_proj);
	output.tex = input.tex;

	return output;
}

void PSMain(PS_IN input)
{
	float4 diffuse = DiffuseMap.Sample(Sampler, input.tex);
	clip(diffuse.a - 0.15f);
}