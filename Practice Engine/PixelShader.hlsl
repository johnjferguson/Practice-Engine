Texture2D tex_in;
SamplerState sampler_in;

float4 main(float2 tc : TEXTURE) : SV_TARGET
{
	return tex_in.Sample(sampler_in, tc);
}