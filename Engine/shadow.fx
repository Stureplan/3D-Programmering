////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.fx
////////////////////////////////////////////////////////////////////////////////


//GLOBALS//
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
matrix lightViewMatrix;
matrix lightProjectionMatrix;
Texture2D shaderTexture;
Texture2D depthMapTexture;
float4 ambientColor;
float4 diffuseColor;
float3 lightPosition;

SamplerState SampleTypeClamp
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerState SampleTypeWrap
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

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ShadowVertexShader (VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;


	input.position.w = 1.0f;
	output.position = mul (input.position, worldMatrix);
	output.position = mul (output.position, viewMatrix);
	output.position = mul (output.position, projectionMatrix);

	output.lightViewPosition = mul (input.position, worldMatrix);
	output.lightViewPosition = mul (output.lightViewPosition, lightViewMatrix);
	output.lightViewPosition = mul (output.lightViewPosition, lightProjectionMatrix);

	output.tex = input.tex;

	output.normal = mul (input.normal, (float3x3)worldMatrix);
	output.normal = normalize (output.normal);

	worldPosition = mul (input.position, worldMatrix);
	output.lightPos = lightPosition.xyz - worldPosition.xyz;
	output.lightPos = normalize (output.lightPos);

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ShadowPixelShader (PixelInputType input) : SV_Target
{
	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;

	color = ambientColor;

	bias = 0.01f;

	// Calculate the projected texture coordinates.
	projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if ((saturate (projectTexCoord.x) == projectTexCoord.x) && (saturate (projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = depthMapTexture.Sample (SampleTypeClamp, projectTexCoord).r;


		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if (lightDepthValue < depthValue)
		{
			// Calculate the amount of light on this pixel.
			lightIntensity = saturate (dot (input.normal, input.lightPos));

			if (lightIntensity > 0.0f)
			{
				color += (diffuseColor * lightIntensity);
				color = saturate (color);
			}
		}
	}

	else
	{
		lightIntensity = saturate (dot (input.normal, input.lightPos));
		color += (diffuseColor * lightIntensity);
		color = saturate (color);
	}

	textureColor = shaderTexture.Sample (SampleTypeWrap, input.tex);
	color = color * textureColor;

	return color;
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
		SetGeometryShader (NULL);
	}
}