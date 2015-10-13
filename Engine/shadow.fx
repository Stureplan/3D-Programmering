////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.fx
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D colorTexture;
Texture2D normalTexture;
Texture2D diffuseTexture;
float3 lightDirection;
float4 ambientColor;
float4 diffuseColor;
float3 cameraPosition;
float specularPower;



///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypePoint
{
	Filter = MIN_MAG_MIP_POINT;
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
};


struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ShadowVertexShader (VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul (input.position, worldMatrix);
	output.position = mul (output.position, viewMatrix);
	output.position = mul (output.position, projectionMatrix);


	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	output.viewDirection = cameraPosition.xyz - output.position;
	output.viewDirection = normalize(output.viewDirection);

	return output;
}




////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ShadowPixelShader (PixelInputType input) : SV_Target
{
	float4 textureColor;
	float4 normals;
	float3 lightDir;
	float lightIntensity;
	float4 outputColor;
	float3 reflection;
	float4 specular;
	float4 diffuse;

	//Find out what color this pixel is
	textureColor = colorTexture.Sample (SampleTypePoint, input.tex);
	
	//Find out what normal this pixel has
	normals = normalTexture.Sample(SampleTypePoint, input.tex);

	diffuse = diffuseTexture.Sample(SampleTypePoint, input.tex);

	//Set all default light to ambient light
	outputColor = ambientColor;
	specular = float4(0.0f, 0.0f, 0.0f, 1.0f);

	//Calculate with normals
	lightDir = -lightDirection;

	lightIntensity = saturate(dot(normals.xyz, lightDir));



	//Calculate with normals
	lightDir = -lightDirection;

	lightIntensity = saturate(dot(normals.xyz, lightDir));

	//Check if pixel is supposed to have light
	if (lightIntensity > 0.0f)
	{
		//Add diffuse to outputColor
		outputColor += (diffuse * lightIntensity);

		if (specularPower != 0.0f)
		{
			reflection = normalize(2 * lightIntensity * normals.xyz - input.viewDirection);
			specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
		}
	}
		

	outputColor = outputColor * diffuse;

	//Final color
	if (textureColor.x)
	{
		outputColor = outputColor * textureColor;
	}
	
	if (specularPower != 0.0f)
	{
		outputColor = saturate(outputColor + specular);
	}

	if (!textureColor.x && !diffuse.x)
	{
		outputColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	//return normals
	return outputColor;
}


////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 ShadowTechnique
{
	pass pass0
	{
		SetVertexShader (CompileShader(vs_4_0, ShadowVertexShader ()));
		SetPixelShader (CompileShader(ps_4_0, ShadowPixelShader ()));
		SetGeometryShader(NULL);
	}
}