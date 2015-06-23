////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <fstream>
using namespace std;
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct ModelType		//This structure represents our model format
	{						//I.E: how is the file built?
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;


	};

	struct Object
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 rotation;
		D3DXVECTOR3 scale;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D10Device*, int, char*, WCHAR*);
	void SetPosition (float, float, float);
	D3DXVECTOR3 GetPosition();
	int GetObjectCount();
	void Shutdown();
	void Render(ID3D10Device*);
	int GetIndexCount();

	ID3D10ShaderResourceView* GetTexture();


private:
	bool InitializeBuffers(ID3D10Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D10Device*);
	bool LoadTexture(ID3D10Device*, WCHAR*);
	void ReleaseTexture();
	bool LoadModel(char*);
	void ReleaseModel();

private:
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount, m_objectCount;
	TextureClass* m_Texture;
	ModelType* m_model;
	Object* m_Object;

	D3DXVECTOR3 gun, cube;
};

#endif