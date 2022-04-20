cbuffer	Matrix : register(b0)
{
	matrix mat;
};

cbuffer	Color : register(b1)
{
	float4 color;
};

struct VS_IN
{
	float3 pos : POSITION0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;

	float4 pos = float4(input.pos, 1.0);
	output.pos = mul(pos, mat);
	output.col = color;

	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 col = input.col;
	return col;
}