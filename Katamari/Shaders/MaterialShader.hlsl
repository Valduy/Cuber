struct TransformData
{
	matrix world;
	matrix world_view_proj;
	matrix inverse_transpose_world;
	matrix light_world_view_proj;
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
Texture2D ShadowMap : register(t1);
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
	float4 light_pos : TEXCOORD2;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;

	output.pos = mul(float4(input.pos, 1.0), Transform.world_view_proj);
	output.norm = mul(float4(input.norm, 0.0), Transform.inverse_transpose_world);
	output.tex = input.tex;
	output.world_pos = mul(float4(input.pos, 1.0), Transform.world);
	output.light_pos = mul(float4(input.pos, 1.0), Transform.light_world_view_proj);

	return output;
}

float CalculateShadow(float4 light_pos, float4 norm)
{
	float3 proj = light_pos.xyz / light_pos.w;
	float proj_x = proj.x * 0.5 + 0.5;
	float proj_y = -proj.y * 0.5 + 0.5;
	float current_depth = proj.z;
	float closest_depth = ShadowMap.Sample(Sampler, float2(proj_x, proj_y)).r;
	float bias = 0.005;

	if (current_depth - bias > closest_depth) {
		return 1.0;
	}

	return 0.0;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 color = DiffuseMap.Sample(Sampler, input.tex);
	clip(color.a - 0.01f);

	// ambient
	float3 ambient = Material.ambient * Light.light_color;

	// diffuse
	float3 normal = normalize(input.norm.xyz);
	float3 to_light_direction = -Light.light_direction;
	float diff = max(0.0, dot(to_light_direction, normal));
	float3 diffuse = diff * Light.light_color;

	// specular
	float3 view_direction = normalize(Light.view_position - input.world_pos.xyz);
	float3 reflect_direction = normalize(reflect(Light.light_direction, normal));
	float spec = pow(max(0.0, dot(view_direction, reflect_direction)), Material.shininess);
	float3 specular = Material.specular * spec * Light.light_color;

	float shadow = CalculateShadow(input.light_pos, input.norm);
	float3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * color.xyz;
	return float4(result, 1.0f);
}