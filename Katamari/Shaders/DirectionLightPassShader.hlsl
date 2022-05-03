struct LightData
{
	float3 view_position;
	float dummy0;
	float3 light_direction;
	float dummy1;
	float3 light_color;
	float dummy2;
};

cbuffer LightBuffer : register(b0)
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

struct PS_IN
{
	float4 pos : SV_POSITION;
};

struct PS_OUT
{
	float4 accumulator : SV_Target0;
};

PS_IN VSMain(uint id : SV_VertexID)
{
	PS_IN output = (PS_IN)0;

	float2 tex = float2(id & 1, (id & 2) >> 1);
	output.pos = float4(tex * float2(2, -2) + float2(-1, 1), 0, 1);

	return output;
}

GBufferData ReadGBuffer(float2 tex)
{
	GBufferData g_buffer = (GBufferData)0;

	float4 diff_spec = DiffuseMap.Load(float3(tex, 0));
	g_buffer.diffuse = diff_spec.rgb;
	g_buffer.specular = diff_spec.a;
	g_buffer.normal = NormalMap.Load(float3(tex, 0)).xyz;
	g_buffer.pos = NormalMap.Load(float3(tex, 0)).xyz;

	return g_buffer;
}

float4 CalculateLight(GBufferData g_buffer)
{
	float3 color = g_buffer.diffuse;

	// diffuse
	float3 normal = normalize(g_buffer.normal);
	float3 to_light_direction = -Light.light_direction;
	float diff = saturate(dot(to_light_direction, normal));
	if (diff <= 0) discard;
	float3 diffuse = diff * Light.light_color;

	// specular
	float shininess = 50;
	float3 view_direction = normalize(Light.view_position - g_buffer.pos);
	float3 reflect_direction = normalize(reflect(Light.light_direction, normal));
	float spec = pow(max(0.0, dot(view_direction, reflect_direction)), shininess);
	float3 specular = g_buffer.specular * spec * Light.light_color;

	float3 result = (0.1 + diffuse + specular) * color.xyz;
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