////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <D3D10.h>
#include <D3DX10.h>
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
		
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};

	struct ModelType		//This structure represents our model format
	{						//I.E: how is the file built?
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;

		float tx, ty, tz;
		float bx, by, bz;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
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
	ModelClass(D3DXVECTOR3, ID3D10Device*, WCHAR*, WCHAR*, bool);
	~ModelClass();

	bool Initialize(ID3D10Device*, WCHAR*, WCHAR*);
	void SetPosition (float, float, float);
	D3DXVECTOR3 GetPosition();
	void SetRotation(float, float, float);
	D3DXVECTOR3 GetRotation();
	void SetScale (float, float, float);
	D3DXVECTOR3 GetScale();
	int GetObjectCount();
	void Shutdown();
	void Render(ID3D10Device*);
	int GetIndexCount();
	ID3D10ShaderResourceView* GetTexture();
	ID3D10ShaderResourceView* GetNormalmap ();

private:
	bool InitializeBuffers(ID3D10Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D10Device*);
	bool LoadTexture(ID3D10Device*, WCHAR*);
	void LoadNormalmap (ID3D10Device*, WCHAR*);
	void ReleaseTexture();
	bool LoadModel(WCHAR*);
	void ReleaseModel();

	void CalculateModelVectors ();
	void CalculateTangentBinormal (TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal (VectorType, VectorType, VectorType&);

private:
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount, m_objectCount;
	string texturefile;
	float r, g, b;
	bool m_normalMapped;
	TextureClass* m_Texture;
	TextureClass* m_Normalmap;
	ModelType* m_model;
	Object* m_Object;

	D3DXVECTOR3 defaultpos;
};

#endif