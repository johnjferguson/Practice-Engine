
cbuffer cbuff : register(b0)
{
	row_major matrix transform;
	row_major matrix projection;
}

struct VSOut
{
	float2 tex : TEXTURE;
	float4 pos : SV_POSITION;
};

VSOut main(float3 pos : POSITION, float2 tex : TEXTURE)
{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), transform);
	vso.pos = mul(vso.pos, projection);
	vso.tex = tex;

	return vso;
}