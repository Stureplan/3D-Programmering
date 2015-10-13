// GLOBALS
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;
float4 diffuse;

SamplerState SampleType
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct GeometryInputType
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	//float4 geopos : TEXCOORD1;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	//float4 geopos : TEXCOORD1;
};

struct PixelOutputType
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
	float4 diffuse : SV_Target2;
};

//VERTEX SHADER
PixelInputType DeferredVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//Proof variable
	//output.geopos = input.position;

	output.tex = input.tex;
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}

// Geometry Shader
[maxvertexcount(4)]
void ShadowGeometryShader(triangle PixelInputType input[3], inout TriangleStream<PixelInputType> triStream)
{
	float4 dir = input[0].position;
	dir = normalize(dir);
	dir = -dir;

	//----------------------------------------------------------
	//PROOF VARIABLES - change input positions below to geopos

	//float4 test = float4(0.0f, 1.0f, 0.0f, 1.0f);
	//float3 vec1 = (input[1].geopos - input[0].geopos);
	//float3 vec2 = (input[2].geopos - input[0].geopos);
	//----------------------------------------------------------

	float3 vec1 = (input[1].position - input[0].position);
	float3 vec2 = (input[2].position - input[0].position);
	float3 surface = cross(vec1, vec2);
	surface = normalize(surface);


	bool v1 = false;
	bool v2 = false;
	bool v3 = false;

	float isVisible = (dot(dir, surface));
	if (isVisible > 0.0f)
	{
		v1 = true;
	}

	isVisible = (dot(dir, surface));
	if (isVisible > 0.0f)
	{
		v2 = true;
	}

	isVisible = (dot(dir, surface));
	if (isVisible > 0.0f)
	{
		v3 = true;
	}

	if ((v1 == true) || (v2 == true) || (v3 == true))
	{
		triStream.Append(input[0]);
		triStream.Append(input[1]);
		triStream.Append(input[2]);
	}


	//triStream.RestartStrip();
}

//PIXEL SHADER
PixelOutputType DeferredPixelShader(PixelInputType input) : SV_Target
{
	PixelOutputType output;


	// Sample the color from the texture and store it for output to the render target.
	output.color = shaderTexture.Sample(SampleType, input.tex);
	
	// Store the normal for output to the render target.
	output.normal = float4(input.normal, 1.0f);

	output.diffuse = diffuse;

    return output;
}

//TECHNIQUE
technique10 DeferredTechnique
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_4_0, DeferredVertexShader()));
		SetGeometryShader(CompileShader(gs_4_0, ShadowGeometryShader()));
		SetPixelShader(CompileShader(ps_4_0, DeferredPixelShader()));
	}
}