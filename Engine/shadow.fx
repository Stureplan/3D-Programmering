////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.fx
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
matrix lightViewMatrix;
matrix lightProjectionMatrix;
Texture2D shaderTexture;
Texture2D depthMapTexture;
float3 lightDirection;
float4 ambientColor;
float4 diffuseColor;
float3 cameraPosition;
float specularPower;

///////////////////
// SAMPLE STATES //
///////////////////
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
	float4 lightViewPosition : TEXCOORD1;
	float3 viewDirection : TEXCOORD2;
	float3 geopos : TEXCOORD3;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 viewDirection : TEXCOORD2;
	float3 geopos : TEXCOORD3;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ShadowVertexShader (VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;
	

	

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul (input.position, worldMatrix);

	output.geopos = output.position;

	output.position = mul (output.position, viewMatrix);
	output.position = mul (output.position, projectionMatrix);

	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPosition = mul (input.position, worldMatrix);
	output.lightViewPosition = mul (output.lightViewPosition, lightViewMatrix);
	output.lightViewPosition = mul (output.lightViewPosition, lightProjectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only.
	output.normal = mul (input.normal, (float3x3)worldMatrix);

	// Normalize the normal vector.
	output.normal = normalize (output.normal);

	worldPosition = mul(input.position, worldMatrix);
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	output.viewDirection = normalize(output.viewDirection);

	return output;
}


// Geometry Shader
[maxvertexcount(4)]
void ShadowGeometryShader(triangle PixelInputType input[3], inout TriangleStream<PixelInputType> triStream)
{	
	//input[0].viewDirection = (1.0f, 0.0f, 0.0f, 1.0f);
	//input[1].viewDirection = (1.0f, 0.0f, 0.0f, 1.0f);
	//input[2].viewDirection = (1.0f, 0.0f, 0.0f, 1.0f);


	//
	//if (input[0].normal.x > 0.0f && input[1].normal.x > 0.0f && input[2].normal.x > 0.0f &&
	//	input[0].normal.y > 0.0f && input[1].normal.y > 0.0f && input[2].normal.y > 0.0f &&
	//	input[0].normal.z > 0.0f && input[1].normal.z > 0.0f && input[2].normal.z > 0.0f )
	//{
	//	triStream.Append(input[0]);
	//	triStream.Append(input[1]);
	//	triStream.Append(input[2]);
	//}

	/*float3 normalized_0 = input[0].position.xyz;
	normalized_0 = normalize(normalized_0);
	if (input[0].normal > cameraPosition - normalized_0)
		triStream.Append(input[0]);

	float3 normalized_1 = input[1].position.xyz;
	normalized_1 = normalize(normalized_1);
	if (input[1].normal > cameraPosition - normalized_1)
		triStream.Append(input[1]);

	float3 normalized_2 = input[2].position.xyz;
	normalized_2 = normalize(normalized_2);
	if (input[2].normal > cameraPosition - normalized_2)
		triStream.Append(input[2]);

*/
	float3 dir = (viewMatrix._13, viewMatrix._23, viewMatrix._33);
	//float3 dir;
	//dir	= float3(0.0f, 0.0f, 1.0f);

	float3 vec1 = (input[1].geopos - input[0].geopos);
	float3 vec2 = (input[2].geopos - input[0].geopos);
	//float3 surface = cross(vec1, vec2);

	float normalX, normalY, normalZ;
	normalX = (vec1.y * vec2.z) - (vec1.z * vec2.y);
	normalY = (vec1.z * vec2.x) - (vec1.x * vec2.z);
	normalZ = (vec1.x * vec2.y) - (vec1.y * vec2.x);
	float3 surface = (normalX, normalY, normalZ);

	//dir = normalize(dir);
	//surface = normalize(surface);

	float test = dot(dir, surface);
	test = cos(test);
	//test = mul(test, 10);

	if (test > 0.0)
	{
		triStream.Append(input[0]);
		triStream.Append(input[1]);
		triStream.Append(input[2]);
	}

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
	float3 lightDir;
	float3 reflection;
	float4 specular;

	lightDir = -lightDirection;

	// Set the bias value for fixing the floating point precision issues.
	bias = 0.001f;

	// Set the default output color to the ambient light value for all pixels.
	color = ambientColor;
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Calculate the projected texture coordinates.
	projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if ((saturate (projectTexCoord.x) == projectTexCoord.x) && (saturate (projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = depthMapTexture.Sample (SampleTypeClamp, projectTexCoord).r;

		// Calculate the depth of the light.
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if (lightDepthValue < depthValue)
		{
			// Calculate the amount of light on this pixel.
			lightIntensity = saturate (dot (input.normal, lightDir));

			if (lightIntensity > 0.0f)
			{
				//Calculate diffuse
				color += (diffuseColor * lightIntensity);
				color = saturate (color);

				if (specularPower != 0.0f)
				{
					reflection = normalize (2 * lightIntensity * input.normal - input.viewDirection);
					specular = pow (saturate (dot (reflection, input.viewDirection)), specularPower);
				}
			}
		}
	}

	else
	{
		// If this is outside the area of shadow map range then draw things normally with regular lighting.
		lightIntensity = saturate (dot (input.normal, lightDir));
		if (lightIntensity > 0.0f)
		{
			//Calculate diffuse
			color += (diffuseColor * lightIntensity);
			color = saturate (color);


			//If we sent a 0 value as a parameter to the shader, DON'T calculate specular
			if (specularPower != 0.0f)
			{
				reflection = normalize (2 * lightIntensity * input.normal - lightDir);
				specular = pow (saturate (dot (reflection, input.viewDirection)), specularPower);
			}
		}
	}



	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample (SampleTypeWrap, input.tex);

	if (textureColor.x)
		color = color * textureColor;

	if (specularPower != 0.0f)
		color = saturate(color + specular);

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
		SetGeometryShader (CompileShader(gs_4_0, ShadowGeometryShader()));
		SetPixelShader (CompileShader(ps_4_0, ShadowPixelShader ()));
	}
}