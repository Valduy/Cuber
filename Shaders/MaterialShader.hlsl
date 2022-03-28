cbuffer	Transform : register(b0)
{
	matrix world;
	matrix world_view_proj;
	matrix inverse_transpose_world;
};

cbuffer Material : register(b1)
{
	float ambient_factor;
	float shininess;
	float specular_factor;
	float dummy0;
}

cbuffer Light : register(b2)
{
	float3 view_position;
	float dummy1;
	float3 light_direction;
	float dummy2;
	float3 light_color;
	float dummy3;
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

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;

	output.pos = mul(float4(input.pos, 1.0), world_view_proj);
	output.norm = mul(float4(input.norm, 0.0), inverse_transpose_world);
	output.tex = input.tex;
	output.world_pos = mul(float4(input.pos, 1.0), world);

	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 color = DiffuseMap.Sample(Sampler, input.tex);
	clip(color.a - 0.01f);

	// ambient
	float3 ambient = ambient_factor * light_color;

	// diffuse
	float3 normal = normalize(input.norm.xyz);
	float3 to_light_direction = -light_direction;
	float diff = max(0.0, dot(to_light_direction, normal));
	float3 diffuse = diff * light_color;

	// specular
	float3 vp = view_position;
	float3 view_direction = normalize(view_position - input.world_pos.xyz);
	float3 reflect_direction = normalize(reflect(light_direction, normal));
	float spec = pow(max(0.0, dot(view_direction, reflect_direction)), shininess);
	float3 specular = specular_factor * spec * light_color;

	float3 result = (ambient + diffuse + specular) * color.xyz;
	return float4(result, 1.0f);
}