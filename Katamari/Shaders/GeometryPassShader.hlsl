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
Texture2D NormalMap : register(t1);
Texture2D SpecularMap : register(t2);

SamplerState Sampler : register(s0);

struct VS_IN
{
	float3 pos : POSITION0;
	float3 normal : NORMAL0;
	float3 binormal : BINORMAL0;
	float3 tangent : TANGENT0;
	float2 tex : TEXCOORD0;
};

struct PS_IN
{
	float4 pos : SV_POSITION0;
	float4 normal : NORMAL;
	float4 binormal : BINORMAL;
	float4 tangent : TANGENT;
	float2 tex : TEXCOORD0;
	float4 world_pos : TEXCOORD1;
};

struct PS_OUT
{
	float4 diff : SV_Target0; 
	float4 norm : SV_Target1;
	float4 pos : SV_Target2;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;

	output.pos = mul(float4(input.pos, 1.0), ModelTransform.world_view_proj);
	output.normal = normalize(mul(float4(input.normal, 0.0), ModelTransform.inverse_transpose_world));
	output.binormal = normalize(mul(float4(input.binormal, 0.0), ModelTransform.inverse_transpose_world));
	output.tangent = normalize(mul(float4(input.tangent, 0.0), ModelTransform.inverse_transpose_world));
	output.tex = input.tex;
	output.world_pos = mul(float4(input.pos, 1.0), ModelTransform.world);

	return output;
}

[earlydepthstencil]
PS_OUT PSMain(PS_IN input)
{
	PS_OUT output = (PS_OUT)0;

	float3 T = normalize(input.tangent.xyz);
	float3 B = normalize(input.binormal.xyz);
	float3 N = normalize(input.normal.xyz);
	float3x3 TBN = float3x3(T, B, N);

	float3 normal = NormalMap.Sample(Sampler, input.tex.xy).xyz;
	float3 unpacked_normal = normalize(normal * 2.0 - 1.0);
	unpacked_normal = mul(unpacked_normal, TBN);

	output.diff.rgb = DiffuseMap.Sample(Sampler, input.tex.xy).rgb;
	output.diff.a = DiffuseMap.Sample(Sampler, input.tex.xy).r;
	output.norm = float4(unpacked_normal, 0.0);
	output.pos = input.world_pos;

	return output;
}