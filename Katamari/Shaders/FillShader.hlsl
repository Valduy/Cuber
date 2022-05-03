Texture2D Texture : register(t0);

SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

PS_IN VSMain(uint id : SV_VertexID)
{
	PS_IN output = (PS_IN)0;

	output.tex = float2(id & 1, (id & 2) >> 1);
	output.pos = float4(output.tex * float2(2, -2) + float2(-1, 1), 0.5, 1);

	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 diffuse = Texture.Sample(Sampler, input.tex);
	return diffuse;
}