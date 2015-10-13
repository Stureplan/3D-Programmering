//DEFERRED SHADER CLASS .CPP
#include "deferredshaderclass.h"

DeferredShaderClass::DeferredShaderClass()
{
	m_effect = 0;
	m_technique = 0;
	m_layout = 0;

	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;
	m_texturePtr = 0;

	m_diffusePtr = 0;
}

DeferredShaderClass::DeferredShaderClass(const DeferredShaderClass& other)
{

}

DeferredShaderClass::~DeferredShaderClass()
{

}

void DeferredShaderClass::Initialize(ID3D10Device* device, HWND hwnd)
{
	InitializeShader(device, hwnd, L"../Engine/deferred.fx");
}

void DeferredShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

void DeferredShaderClass::Render(ID3D10Device* device, int indexCount, 
								 D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
								 ID3D10ShaderResourceView* texture, D3DXVECTOR4 diffuse)
{
	SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, texture, diffuse);
	RenderShader(device, indexCount);
	
	return;
}

void DeferredShaderClass::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	D3D10_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D10_PASS_DESC passDesc;


	// Initialize the error message.
	errorMessage = 0;

	// Load the shader in from the file.
	result = D3DX10CreateEffectFromFile(filename, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		device, NULL, NULL, &m_effect, &errorMessage, NULL);
	if (FAILED(result))
	{
		// If there was nothing in the error message then it simply could not find the shader file itself.
		MessageBox(hwnd, filename, L"Missing Shader File", MB_OK);

		return;
	}

	// Get a pointer to the technique inside the shader.
	m_technique = m_effect->GetTechniqueByName("DeferredTechnique");
	if (!m_technique)
	{
		return;
	}

	// Now setup the layout of the data that goes into the shader.
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
	m_technique->GetPassByIndex(0)->GetDesc(&passDesc);

	// Create the input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_layout);
	if (FAILED(result))
	{
		return;
	}

	// Get pointers to the three matrices inside the shader so we can update them from this class.
	m_worldMatrixPtr		= m_effect->GetVariableByName("worldMatrix")->AsMatrix();
	m_viewMatrixPtr			= m_effect->GetVariableByName("viewMatrix")->AsMatrix();
	m_projectionMatrixPtr	= m_effect->GetVariableByName("projectionMatrix")->AsMatrix();

	// Get pointer to the texture resource inside the shader.
	m_texturePtr			= m_effect->GetVariableByName("shaderTexture")->AsShaderResource();
	m_diffusePtr			= m_effect->GetVariableByName("diffuse")->AsVector ();


	return;
}

void DeferredShaderClass::ShutdownShader()
{
	m_texturePtr = 0;

	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	m_technique = 0;

	if (m_effect)
	{
		m_effect->Release();
		m_effect = 0;
	}

	return;
}

void DeferredShaderClass::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
											  ID3D10ShaderResourceView* texture, D3DXVECTOR4 diffuse)
{
	m_worldMatrixPtr->SetMatrix((float*)&worldMatrix);
	m_viewMatrixPtr->SetMatrix((float*)&viewMatrix);
	m_projectionMatrixPtr->SetMatrix((float*)&projectionMatrix);

	m_texturePtr->SetResource(texture);

	m_diffusePtr->SetFloatVector ((float*) &diffuse);

	return;
}

void DeferredShaderClass::RenderShader(ID3D10Device* device, int indexCount)
{
	D3D10_TECHNIQUE_DESC techniqueDesc;

	device->IASetInputLayout(m_layout);
	m_technique->GetDesc(&techniqueDesc);

	for (int i = 0; i < techniqueDesc.Passes; ++i)
	{
		m_technique->GetPassByIndex(i)->Apply(0);
		device->DrawIndexed(indexCount, 0, 0);
	}

	return;
}