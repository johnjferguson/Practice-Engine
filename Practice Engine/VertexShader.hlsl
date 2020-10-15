
cbuffer cbuff : register(b0)
{
	row_major matrix transform;
	row_major matrix projection;
}

struct VSOut
{
	float3 color : COLOR;
	float4 pos : SV_POSITION;
};

VSOut main(float3 pos : POSITION, float3 color : COLOR)
{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), transform);
	vso.pos = mul(vso.pos, projection);
	vso.color = color;

	return vso;
}