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

	m_lightViewMatrixPtr = 0;
	m_lightProjectionMatrixPtr = 0;

	m_texturePtr = 0;
	m_depthMapTexturePtr = 0;

	m_lightDirectionPtr = 0;
	m_ambientColorPtr = 0;
	m_diffuseColorPtr = 0;

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
	D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix, 
	ID3D10ShaderResourceView* texture, ID3D10ShaderResourceView* depthMapTexture, 
	D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor,
	D3DXVECTOR3 cameraPosition, float specularPower)
{
	// Set the shader parameters that it will use for rendering.
	SetShaderParameters (worldMatrix, viewMatrix, projectionMatrix, 
						 lightViewMatrix, lightProjectionMatrix, 
						 texture, depthMapTexture,
						 lightDirection, ambientColor, diffuseColor,
						 cameraPosition, specularPower);

	// Now render the prepared buffers with the shader.
	RenderShader (device, indexCount);

	return;
}



bool ShadowShaderClass::InitializeShader (ID3D10Device* device, HWND hwnd, WCHAR* filename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	D3D10_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D10_PASS_DESC passDesc;


	// Initialize the error message.
	errorMessage = 0;

	// Load the shader in from the file.
	result = D3DX10CreateEffectFromFile (filename, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		device, NULL, NULL, &m_effect, &errorMessage, NULL);
	if (FAILED (result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage (errorMessage, hwnd, filename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox (hwnd, filename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Get a pointer to the technique inside the shader.
	m_technique = m_effect->GetTechniqueByName ("ShadowTechnique");
	if (!m_technique)
	{
		return false;
	}

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

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

	//Pointers to Light View & Projection matrixes
	m_lightViewMatrixPtr	   = m_effect->GetVariableByName ("lightViewMatrix")	  ->AsMatrix ();
	m_lightProjectionMatrixPtr = m_effect->GetVariableByName ("lightProjectionMatrix")->AsMatrix ();

	//Get pointer to the texture resources inside the shader
	m_texturePtr		 = m_effect->GetVariableByName ("shaderTexture")  ->AsShaderResource ();
	m_depthMapTexturePtr = m_effect->GetVariableByName ("depthMapTexture")->AsShaderResource ();

	//Pointers to the light position and diffuse in the shader
	m_lightDirectionPtr = m_effect->GetVariableByName ("lightDirection")->AsVector ();
	m_ambientColorPtr	= m_effect->GetVariableByName ("ambientColor")  ->AsVector ();
	m_diffuseColorPtr	= m_effect->GetVariableByName ("diffuseColor")  ->AsVector ();

	//Pointers to specular values
	m_cameraPositionPtr = m_effect->GetVariableByName("cameraPosition")->AsVector();
	m_specularPowerPtr = m_effect->GetVariableByName("specularPower")->AsScalar();

	return true;
}


void ShadowShaderClass::ShutdownShader ()
{
	//Release the light pointers
	m_lightDirectionPtr = 0;
	m_ambientColorPtr = 0;
	m_diffuseColorPtr = 0;
	m_cameraPositionPtr = 0;
	m_specularPowerPtr = 0;



	//Release the pointer to the texture in the shader file.
	m_texturePtr = 0;
	m_depthMapTexturePtr = 0;

	//Release light matrixes
	m_lightViewMatrixPtr = 0;
	m_lightProjectionMatrixPtr = 0;

	// Release the pointers to the matrices inside the shader.
	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;

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


void ShadowShaderClass::OutputShaderErrorMessage (ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*) (errorMessage->GetBufferPointer ());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize ();

	// Open a file to write the error message to.
	fout.open ("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close ();

	// Release the error message.
	errorMessage->Release ();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox (hwnd, L"Error compiling shader.  Check shader-error.txt for info.", shaderFilename, MB_OK);

	return;
}


void ShadowShaderClass::SetShaderParameters(
	D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
	D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix, 
	ID3D10ShaderResourceView* texture, ID3D10ShaderResourceView* depthMapTexture,
	D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor,
	D3DXVECTOR3 cameraPosition, float specularPower)
{
	//Set pointers inside the shader
	m_worldMatrixPtr			->SetMatrix ((float*) &worldMatrix);
	m_viewMatrixPtr				->SetMatrix ((float*) &viewMatrix);
	m_projectionMatrixPtr		->SetMatrix ((float*) &projectionMatrix);

	m_lightViewMatrixPtr		->SetMatrix ((float*) &lightViewMatrix);
	m_lightProjectionMatrixPtr	->SetMatrix ((float*) &lightProjectionMatrix);

	m_texturePtr				->SetResource (texture);
	m_depthMapTexturePtr		->SetResource (depthMapTexture);

	m_lightDirectionPtr			->SetFloatVector ((float*) &lightDirection);
	m_ambientColorPtr			->SetFloatVector ((float*) &ambientColor);
	m_diffuseColorPtr			->SetFloatVector ((float*) &diffuseColor);

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