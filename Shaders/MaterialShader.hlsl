cbuffer	Matrixes : register(b0)
{
	matrix mat;
};

Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);

struct VS_IN
{
	float3 pos : POSITION0;
	float2 tex : TEXCOORD0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;

	output.pos = mul(float4(input.pos, 1.0), mat);
	output.tex = input.tex;

	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 color = DiffuseMap.Sample(Sampler, input.tex);
	//clip(color.a - 0.01f);
	return color;
}