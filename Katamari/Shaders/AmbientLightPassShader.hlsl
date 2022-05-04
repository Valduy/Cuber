struct LightData
{
	float3 light_color;
	float dummy0;
	float intensity;
	float dummy1;
	float dummy2;
	float dummy3;
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
	float3 color = g_buffer.diffuse.rgb;
	color = color * Light.light_color * Light.intensity;
	return float4(color, 1.0f);
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