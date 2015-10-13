////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTEXTURECLASS_H_
#define _RENDERTEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10math.h>

const int BUFFER_COUNT = 3;


////////////////////////////////////////////////////////////////////////////////
// Class name: RenderTextureClass
////////////////////////////////////////////////////////////////////////////////
class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D10Device*, int, int, float, float);
	void Shutdown();

	void SetRenderTarget(ID3D10Device*);
	void ClearRenderTarget(ID3D10Device*, float, float, float, float);
	ID3D10ShaderResourceView* GetShaderResourceView(int);
	//void GetProjectionMatrix(D3DXMATRIX&);	//deferred rendering makes it so
	//void GetOrthoMatrix(D3DXMATRIX&);

private:
	int m_textureW, m_textureH;
	ID3D10Texture2D* m_renderTargetTextureArray[BUFFER_COUNT];
	ID3D10RenderTargetView* m_renderTargetViewArray[BUFFER_COUNT];
	ID3D10ShaderResourceView* m_shaderResourceViewArray[BUFFER_COUNT];
	ID3D10Texture2D* m_depthStencilBuffer;
	ID3D10DepthStencilView* m_depthStencilView;
	D3D10_VIEWPORT m_viewport;
	//D3DXMATRIX m_projectionMatrix;
	//D3DXMATRIX m_orthoMatrix;
};

#endif