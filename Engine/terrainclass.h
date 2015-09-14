/////////////////////////////////
// Fliename : terrainclass.h
/////////////////////////////////

#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_

#include <D3D10.h>
#include <D3DX10math.h>
#include <stdio.h>
#include "textureclass.h"

//////////////////
// GLOBALS //
const int TEXTURE_REPEAT = 8;

class TerrainClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(ID3D10Device*, char*, WCHAR*);
	void Shutdown();
	void Render(ID3D10Device*);

	int GetIndexCount();
	ID3D10ShaderResourceView* GetTexture();

private:
	bool LoadHeightMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	void CalculateTextureCoordinates();
	bool LoadTexture(ID3D10Device*, WCHAR*);
	void ReleaseTexture();

	bool InitializeBuffers(ID3D10Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D10Device*);

	void SetPosition(float, float, float);
	D3DXVECTOR3 GetPosition();

public:
	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount, m_indexCount;
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
	HeightMapType* m_heightMap;
	TextureClass* m_Texture;

};

#endif