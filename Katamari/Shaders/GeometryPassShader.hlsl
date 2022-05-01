struct ModelTransformData
{
	matrix world;
	matrix world_view_proj;
	matrix inverse_transpose_world;
};

cbuffer ModelTransformBuffer : register(b0)
{
	ModelTransformData ModelTransform;
}

Texture2D DiffuseMap : register(t0);

SamplerState Sampler : register(s0);

struct VS_IN
{
	float3 pos : POSITION0;
	float3 norm : NORMAL0;
	float2 tex : TEXCOORD0;
};

struct PS_IN
{
	float4 pos : SV_POSITION0;
	float4 norm : NORMAL;
	float2 tex : TEXCOORD0;
	float4 world_pos : TEXCOORD1;
};

struct PS_OUT
{
	float4 diff : SV_Target0; // + specular
	float4 norm : SV_Target1;
	float4 pos : SV_Target2;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;

	output.pos = mul(float4(input.pos, 1.0), ModelTransform.world_view_proj);
	output.norm = mul(float4(input.norm, 0.0), ModelTransform.inverse_transpose_world);
	output.tex = input.tex;
	output.world_pos = mul(float4(input.pos, 1.0), ModelTransform.world);

	return output;
}

[earlydepthstencil]
PS_OUT PSMain(PS_IN input)
{
	PS_OUT output = (PS_OUT)0;

	output.diff.rgb = DiffuseMap.Sample(Sampler, input.tex.xy).rgb;
	output.diff.a = 0; // now just hard code it;
	output.norm = normalize(input.norm);
	output.pos = input.pos;

	return output;
}