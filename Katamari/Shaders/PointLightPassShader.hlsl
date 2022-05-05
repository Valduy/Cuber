struct ModelTransformData
{
	matrix world;
	matrix world_view_proj;
	matrix inverse_transpose_world;
};

struct LightData
{
	float3 view_position;
	float dummy0;
	float3 light_position;
	float dummy1;
	float3 light_color;
	float dummy2;
	float fade_radius;
	float3 dummy3;
};

cbuffer ModelTransformBuffer : register(b0)
{
	ModelTransformData ModelTransform;
}

cbuffer LightBuffer : register(b1)
{
	LightData Light;
}

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D PositionMap : register(t2);

struct GBufferData
{
	float3 diffuse;
	float specular;
	float3 normal;
	float3 pos;
};

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
	float4 pos : SV_POSITION;
	float4 world_pos : TEXCOORD0;
};

struct PS_OUT
{
	float4 accumulator : SV_Target0;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;

	output.pos = mul(input.pos, ModelTransform.world_view_proj);
	output.world_pos = mul(float4(input.pos, 1.0), ModelTransform.world);

	return output;
}

GBufferData ReadGBuffer(float2 tex)
{
	GBufferData g_buffer = (GBufferData)0;

	float4 diff_spec = DiffuseMap.Load(float3(tex, 0));
	g_buffer.diffuse = diff_spec.rgb;
	g_buffer.specular = diff_spec.a;
	g_buffer.normal = NormalMap.Load(float3(tex, 0)).xyz;
	g_buffer.pos = PositionMap.Load(float3(tex, 0)).xyz;

	return g_buffer;
}

float4 CalculateLight(GBufferData g_buffer)
{
	float3 color = g_buffer.diffuse;

	float3 light_direction = g_buffer.pos - Light.light_position;
	float distance = length(light_direction);
	distance = max(distance - Light.fade_radius, 1);
	float3 light_color = Light.light_color / (distance * distance);

	// diffuse	
	float3 normal = normalize(g_buffer.normal);
	float3 to_light_direction = -light_direction;
	float diff = max(0.0, dot(to_light_direction, normal));
	if (diff <= 0) discard;
	float3 diffuse = diff * light_color;

	// specular
	float shininess = 50;
	float3 view_direction = normalize(Light.view_position - g_buffer.pos);
	float3 reflect_direction = normalize(reflect(light_direction, normal));
	float spec = pow(max(0.0, dot(view_direction, reflect_direction)), shininess);
	float3 specular = g_buffer.specular * spec * light_color;

	float3 result = (diffuse + specular) * color.xyz;
	return float4(result, 1.0f);
}

[earlydepthstencil]
PS_OUT PSMain(PS_IN input)
{
	PS_OUT output = (PS_OUT)0;

	GBufferData g_buffer = ReadGBuffer(input.pos.xy);
	float4 color = CalculateLight(g_buffer);

	output.accumulator = color;
	return output;
}