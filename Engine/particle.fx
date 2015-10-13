////////////////////////////////////////////////////////////////////////////////
// Filename: particle.fx
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
float3 cameraPosition;
Texture2D particleTexture;

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};


//////////////
// TYPEDEFS //
//////////////


struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct GeometryInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ParticleVertexShader(VertexInputType input)
{
	PixelInputType output;


	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;
	output.position = input.position;
	output.position = mul(input.position, worldMatrix);

	return output;
}


// Geometry Shader
[maxvertexcount(4)]
void ParticleGeometryShader(point PixelInputType input[1], inout TriangleStream<PixelInputType> triStream)
{
	float3 planeNormal;
	float3 upVector;
	float3 rightVector;
	float3 test;
	test = float3 (0.0f, 0.0f, 1.0f);
	//float size = 3.0f;

	planeNormal = input[0].position.xyz - cameraPosition;
	//planeNormal.y = 0.0f;
	planeNormal = normalize(planeNormal);

	upVector = float3 (0.0f, 1.0f, 0.0f);

	rightVector = normalize(cross(planeNormal, upVector));
	upVector = normalize(cross(rightVector, planeNormal));

	float3 vert[4];
	vert[0] = (input[0].position.xyz - upVector - rightVector);// * size;	//bottom left
	vert[1] = (input[0].position.xyz - upVector + rightVector);// * size;	//bottom right
	vert[2] = (input[0].position.xyz + upVector - rightVector);// * size;	//top left
	vert[3] = (input[0].position.xyz + upVector + rightVector);// * size;	//top right

	float2 uv[4];
	uv[0] = float2 (0, 1);
	uv[1] = float2 (1, 1);
	uv[2] = float2 (0, 0);
	uv[3] = float2 (1, 0);

	PixelInputType pout;
	for (int i = 0; i < 4; i++)
	{
		pout.position = float4(vert[i], 1.0f);
		pout.tex = uv[i];
		pout.normal = float3(0, 0, 0);

		pout.position = mul(pout.position, viewMatrix);
		pout.position = mul(pout.position, projectionMatrix);

		triStream.Append(pout);
	}
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ParticlePixelShader(PixelInputType input) : SV_Target
{
	float4 textureColor;

	textureColor = particleTexture.Sample(SampleType, input.tex);

	return textureColor;
}


////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 ParticleTechnique
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_4_0, ParticleVertexShader()));
		SetGeometryShader(CompileShader(gs_4_0, ParticleGeometryShader()));
		SetPixelShader(CompileShader(ps_4_0, ParticlePixelShader()));
	}
}
