struct VS_IN
{
	float3 pos : POSITION0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
};

//////////////////////////////////////////////////////
// Constant buffers
//////////////////////////////////////////////////////

#include "Common.hlsl"

cbuffer CBLookup : register(b3)
{
	uint index;
	float3 _pad;
};


//////////////////////////////////////////////////////
// Shader code
//////////////////////////////////////////////////////

uint PSMain(PS_IN input) : SV_Target
{
	return index;
}