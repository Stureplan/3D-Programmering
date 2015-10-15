////////////////////////////////////////////////////////////////////////////////
// Filename: shadowshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "shadowshaderclass.h"


ShadowShaderClass::ShadowShaderClass ()
{
	m_effect = 0;					//All pointers are set to null
	m_technique = 0;
	m_layout = 0;

	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;

	m_texturePtr = 0;
	m_normalTexturePtr = 0;
	m_diffuseTexturePtr = 0;

	m_lightDirectionPtr = 0;
	m_ambientColorPtr = 0;

	m_cameraPositionPtr = 0;
	m_specularPowerPtr = 0;
}


ShadowShaderClass::ShadowShaderClass (const ShadowShaderClass& other)
{
}


ShadowShaderClass::~ShadowShaderClass ()
{
}


bool ShadowShaderClass::Initialize (ID3D10Device* device, HWND hwnd)
{
	bool result;



	// Initialize the shader that will be used to draw the triangle.
	result = InitializeShader (device, hwnd, L"../Engine/shadow.fx");
	if (!result)
	{
		return false;			//If the file didn't load, we return false
	}

	return true;				//If the file loads, we return true
}


void ShadowShaderClass::Shutdown ()
{
	// Shutdown the shader effect.
	ShutdownShader ();

	return;
}


void ShadowShaderClass::Render (ID3D10Device* device, int indexCount, 
	D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
	ID3D10ShaderResourceView* texture, ID3D10ShaderResourceView* normalTexture, 
	ID3D10ShaderResourceView* diffuse,
	D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor,
	D3DXVECTOR3 cameraPosition, float specularPower)
{
	// Set the shader parameters that it will use for rendering.
	SetShaderParameters (worldMatrix, viewMatrix, projectionMatrix, 
						 texture, normalTexture, diffuse,
						 lightDirection, ambientColor, 
						 cameraPosition, specularPower);

	// Now render the prepared buffers with the shader.
	RenderShader (device, indexCount);

	return;
}



bool ShadowShaderClass::InitializeShader (ID3D10Device* device, HWND hwnd, WCHAR* filename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	D3D10_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D10_PASS_DESC passDesc;
	D3D10_BUFFER_DESC matrixBufferDesc;
	D3D10_BUFFER_DESC lightBufferDesc;


	errorMessage = 0;

	// Load the shader in from the file.
	D3DX10CreateEffectFromFile (filename, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		device, NULL, NULL, &m_effect, &errorMessage, NULL);


	// Get a pointer to the technique inside the shader.
	m_technique = m_effect->GetTechniqueByName ("ShadowTechnique");
	if (!m_technique)
	{
		return false;
	}

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the Orthowindowclass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;
	
	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Get the description of the first pass described in the shader technique.
	m_technique->GetPassByIndex (0)->GetDesc (&passDesc);

	// Create the input layout.
	result = device->CreateInputLayout (polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_layout);
	if (FAILED (result))
	{
		return false;
	}

	

	// Get pointers to the three matrices inside the shader so we can update them from this class.
	m_worldMatrixPtr	  = m_effect->GetVariableByName ("worldMatrix")		->AsMatrix ();
	m_viewMatrixPtr		  = m_effect->GetVariableByName ("viewMatrix")		->AsMatrix ();
	m_projectionMatrixPtr = m_effect->GetVariableByName ("projectionMatrix")->AsMatrix ();

	//Get pointer to the texture resources inside the shader
	m_texturePtr		 = m_effect->GetVariableByName ("colorTexture")  ->AsShaderResource ();
	m_normalTexturePtr   = m_effect->GetVariableByName ("normalTexture") ->AsShaderResource ();
	m_diffuseTexturePtr  = m_effect->GetVariableByName ("diffuseTexture")->AsShaderResource ();

	//Pointers to the light position and diffuse in the shader
	m_lightDirectionPtr = m_effect->GetVariableByName ("lightDirection")->AsVector ();
	m_ambientColorPtr	= m_effect->GetVariableByName ("ambientColor")  ->AsVector ();

	//Pointers to specular values
	m_cameraPositionPtr = m_effect->GetVariableByName("cameraPosition")->AsVector();
	m_specularPowerPtr  = m_effect->GetVariableByName("specularPower")->AsScalar();

	return true;
}


void ShadowShaderClass::ShutdownShader ()
{
	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;

	//Release the pointer to the texture in the shader file.
	m_texturePtr = 0;
	m_normalTexturePtr = 0;
	m_diffuseTexturePtr = 0;

	//Release the light pointers
	m_lightDirectionPtr = 0;
	m_ambientColorPtr = 0;
	m_cameraPositionPtr = 0;
	m_specularPowerPtr = 0;

	// Release the pointer to the shader layout.
	if (m_layout)
	{
		m_layout->Release ();
		m_layout = 0;
	}

	// Release the pointer to the shader technique.
	m_technique = 0;

	// Release the pointer to the shader.
	if (m_effect)
	{
		m_effect->Release ();
		m_effect = 0;
	}

	return;
}


void ShadowShaderClass::SetShaderParameters(
	D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
	ID3D10ShaderResourceView* texture, ID3D10ShaderResourceView* normalTexture,
	ID3D10ShaderResourceView* diffuse,
	D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor,
	D3DXVECTOR3 cameraPosition, float specularPower)
{
	m_worldMatrixPtr			->SetMatrix ((float*) &worldMatrix);
	m_viewMatrixPtr				->SetMatrix ((float*) &viewMatrix);
	m_projectionMatrixPtr		->SetMatrix ((float*) &projectionMatrix);

	m_texturePtr				->SetResource (texture);
	m_normalTexturePtr			->SetResource (normalTexture);
	m_diffuseTexturePtr			->SetResource (diffuse);

	m_lightDirectionPtr			->SetFloatVector ((float*) &lightDirection);
	m_ambientColorPtr			->SetFloatVector ((float*) &ambientColor);

	m_cameraPositionPtr			->SetFloatVector((float*)&cameraPosition);
	m_specularPowerPtr			->SetFloat(specularPower);

	return;
}



void ShadowShaderClass::RenderShader (ID3D10Device* device, int indexCount)	//RenderShader calls the shader
{																			//technique to render polygons.
	D3D10_TECHNIQUE_DESC techniqueDesc;
	unsigned int i;


	// Set the input layout.
	device->IASetInputLayout (m_layout);

	// Get the description structure of the technique from inside the shader so it can be used for rendering.
	m_technique->GetDesc (&techniqueDesc);

	// Go through each pass in the technique (should be just one currently) and render the triangles.
	for (i = 0; i<techniqueDesc.Passes; ++i)
	{
		m_technique->GetPassByIndex (i)->Apply (0);
		device->DrawIndexed (indexCount, 0, 0);
	}

	return;
}