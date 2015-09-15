////////////////////////////////////////////////////////////////////////////////
// Filename: shadowshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SHADOWSHADERCLASS_H_
#define _SHADOWSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10math.h>
#include <d3dx10async.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: ShadowShaderClass
////////////////////////////////////////////////////////////////////////////////
class ShadowShaderClass
{
public:
	ShadowShaderClass ();
	ShadowShaderClass (const ShadowShaderClass&);
	~ShadowShaderClass ();

	bool Initialize (ID3D10Device*, HWND);
	void Shutdown ();
	void Render (ID3D10Device*, int, 
			 	 D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
				 ID3D10ShaderResourceView*, ID3D10ShaderResourceView*, 
				 D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);

	bool SetShaderParameters(ID3D10Device*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, ID3D10ShaderResourceView*);

	void RenderShader(ID3D10Device*, int);

private:
	bool InitializeShader (ID3D10Device*, HWND, WCHAR*);
	void ShutdownShader ();
	void OutputShaderErrorMessage (ID3D10Blob*, HWND, WCHAR*);

	void SetShaderParameters (D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
							  ID3D10ShaderResourceView*, ID3D10ShaderResourceView*,
							  D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);

private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
	};

	//Shader pointers
	ID3D10Effect* m_effect;
	ID3D10EffectTechnique* m_technique;
	ID3D10InputLayout* m_layout;

	//World, View & Projection matrix pointers
	ID3D10EffectMatrixVariable* m_worldMatrixPtr;
	ID3D10EffectMatrixVariable* m_viewMatrixPtr;
	ID3D10EffectMatrixVariable* m_projectionMatrixPtr;

	//Light View & Projection matrix pointers
	ID3D10EffectMatrixVariable* m_lightViewMatrixPtr;
	ID3D10EffectMatrixVariable* m_lightProjectionMatrixPtr;

	//Texture pointers
	ID3D10EffectShaderResourceVariable* m_texturePtr;
	ID3D10EffectShaderResourceVariable* m_depthMapTexturePtr;

	//Light pointers
	ID3D10EffectVectorVariable* m_lightDirectionPtr;
	ID3D10EffectVectorVariable* m_ambientColorPtr;
	ID3D10EffectVectorVariable* m_diffuseColorPtr;

	// Matrix and light buffers
	ID3D10Buffer* m_matrixBuffer;
	ID3D10Buffer* m_lightBuffer;
};

#endif