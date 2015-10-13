//DEFERRED SHADER CLASS
#ifndef _DEFERREDSHADERCLASS_H_
#define _DEFERREDSHADERCLASS_H_

#include <D3D10.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

//CLASS: DefferedShaderClass
class DeferredShaderClass
{
public:
	DeferredShaderClass();
	DeferredShaderClass(const DeferredShaderClass&);
	~DeferredShaderClass();

	void Initialize(ID3D10Device*, HWND);
	void Shutdown();
	void Render(ID3D10Device*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, D3DXVECTOR4);
	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, D3DXVECTOR4);
	void RenderShader(ID3D10Device*, int);

private:
	void InitializeShader(ID3D10Device*, HWND, WCHAR*);
	void ShutdownShader();

private:
	ID3D10Effect* m_effect;
	ID3D10EffectTechnique* m_technique;
	ID3D10InputLayout* m_layout;

	ID3D10EffectMatrixVariable* m_worldMatrixPtr;
	ID3D10EffectMatrixVariable* m_viewMatrixPtr;
	ID3D10EffectMatrixVariable* m_projectionMatrixPtr;
	ID3D10EffectShaderResourceVariable* m_texturePtr;

	ID3D10EffectVectorVariable* m_diffusePtr;
};


#endif