//////////////
// Filename : textclass.h
/////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_


//////////////
// INCLUDES
//////////////
#include "fontclass.h"
#include "fontshaderclass.h"


//////////////
// Class name : TextClass
//////////////
class TextClass
{
private:
	struct SentenceType
	{
		ID3D10Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D10Device*, HWND, int, int, D3DXMATRIX);
	void Shutdown();
	bool Render(ID3D10Device*, FontShaderClass*, D3DXMATRIX, D3DXMATRIX);

	bool SetVideoCardInfo(char*, int, ID3D10Device*);
	bool SetFps(int, ID3D10Device*);
	bool SetCpu(int, ID3D10Device*);
	bool SetCameraPosition(float, float, float, ID3D10Device*);
	bool SetCameraRotation(float, float, float, ID3D10Device*);
	bool SetRenderCount(int, ID3D10Device*);


private:
	bool InitializeSentence(SentenceType**, int, ID3D10Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D10Device*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(SentenceType*, ID3D10Device*, FontShaderClass*, D3DXMATRIX, D3DXMATRIX);

private:
	int m_screenWidth, m_screenHeight;
	D3DXMATRIX m_baseViewMatrix;
	FontClass* m_Font;
	SentenceType *m_sentence1, *m_sentence2, *m_sentence3, *m_sentence4, *m_sentence5;
	SentenceType *m_sentence6, *m_sentence7, *m_sentence8, *m_sentence9, *m_sentence10, *m_sentence11;
	//FontClass* m_Font;
	//FontShaderClass* m_FontShader;
	//int m_screenWidth, m_screenHeight;
	//D3DXMATRIX m_baseViewMatrix;

	//SentenceType* m_sentence1;
	//SentenceType* m_sentence2;

};

#endif