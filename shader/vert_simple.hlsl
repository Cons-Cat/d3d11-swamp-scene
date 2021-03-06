#pragma pack_matrix(row_major)

cbuffer IntoGpu
{
	matrix w, v, p;
};

float4 main(float3 inputVertex : POSITION) : SV_POSITION
{
	float4 output = float4(inputVertex, 1);
	return mul(mul(mul(output, w), v), p);
}
