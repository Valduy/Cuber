struct TransformData
{
	matrix world;
	matrix world_view_proj;
	matrix inverse_transpose_world;
};

struct MaterialData
{
	float ambient;
	float shininess;
	float specular;
	float dummy;
};

struct LightData
{
	float3 view_position;
	float dummy0;
	float3 light_direction;
	float dummy1;
	float3 light_color;
	float dummy2;
};

cbuffer TransformBuffer : register(b0)
{
	TransformData Transform;
}

cbuffer MaterialBuffer : register(b1)
{
	MaterialData Material;
}

cbuffer LightBuffer : register(b2)
{
	LightData Light;
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

	output.pos = mul(float4(input.pos, 1.0), Transform.world_view_proj);
	output.norm = mul(float4(input.norm, 0.0), Transform.inverse_transpose_world);
	output.tex = input.tex;
	output.world_pos = mul(float4(input.pos, 1.0), Transform.world);

	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 color = DiffuseMap.Sample(Sampler, input.tex);
	clip(color.a - 0.01f);

	float3 light_direction = input.world_pos - Light.light_direction;
	float distance = length(light_direction);
	float r = 10;
	distance = max(distance - r, 1);

	// ambient
	float3 light_color = Light.light_color / (distance * distance);
	float3 ambient = Material.ambient * light_color;

	// diffuse	
	float3 normal = normalize(input.norm.xyz);
	float3 to_light_direction = -light_direction;
	float diff = max(0.0, dot(to_light_direction, normal));
	float3 diffuse = diff * light_color;

	// specular
	float3 view_direction = normalize(Light.view_position - input.world_pos.xyz);
	float3 reflect_direction = normalize(reflect(light_direction, normal));
	float spec = pow(max(0.0, dot(view_direction, reflect_direction)), Material.shininess);
	float3 specular = Material.specular * spec * light_color;

	float3 result = (ambient + diffuse + specular) * color.xyz;
	return float4(result, 1.0f);
}