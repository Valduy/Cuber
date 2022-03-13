cbuffer	Matrixes : register(b0)
{
	matrix mat;
};

struct VS_IN
{
	float3 pos : POSITION0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;

	float4 pos = float4(input.pos, 1.0);
	output.pos = mul(pos, mat);

	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 col = float4(1.0, 1.0, 1.0, 1.0);
	return col;
}