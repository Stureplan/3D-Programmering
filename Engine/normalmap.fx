// GLOBALS 
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;
Texture2D shaderNormalTexture;
float4 diffuseColor;
float3 lightDirection;

// SAMPLE STATES
SamplerState SampleType
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

// TYPEDEFS
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

// VERTEX SHADER
PixelInputType NormalMapVertexShader (VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;
	output.position = mul (input.position, worldMatrix);
	output.position = mul (output.position, viewMatrix);
	output.position = mul (output.position, projectionMatrix);
	
	output.tex = input.tex;
	
	output.normal = mul (input.normal, (float3x3)worldMatrix);
	output.normal = normalize (output.normal);

	output.tangent = mul (input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize (output.tangent);

	output.binormal = mul (input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize (output.tangent);

	return output;
}

// PIXEL SHADER
float4 NormalMapPixelShader (PixelInputType input) : SV_Target
{
	float4 textureColor;
	float4 normalMap;
	float3 bumpNormal;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	textureColor = shaderTexture.Sample (SampleType, input.tex);

	normalMap = shaderNormalTexture.Sample (SampleType, input.tex);
	normalMap = (normalMap * 10.0f) - 1.0f;
	bumpNormal = (normalMap.x * input.tangent) + (normalMap.y * input.binormal) + (normalMap.z * input.normal);
	bumpNormal = normalize (bumpNormal);

	lightDir = -lightDirection;
	lightIntensity = saturate (dot (bumpNormal, lightDir));
	color = saturate (diffuseColor * lightIntensity);
	color = color * textureColor;

	return color;
}

// TECHNIQUE
technique10 NormalMapTechnique
{
	pass pass0
	{
		SetVertexShader (CompileShader(vs_4_0, NormalMapVertexShader ()));
		SetPixelShader  (CompileShader(ps_4_0, NormalMapPixelShader  ()));
		SetGeometryShader (NULL);
	}
}