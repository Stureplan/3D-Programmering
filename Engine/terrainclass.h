/////////////////////////////////
// Fliename : terrainclass.h
/////////////////////////////////

#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_

#include <D3D10.h>
#include <D3DX10math.h>
#include <stdio.h>

class TerrainClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
	};

	struct HeightMapType
	{
		float x, y, z;
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

	bool Initialize(ID3D10Device*, char*);
	void Shutdown();
	void Render(ID3D10Device*);

	int GetIndexCount();

private:
	bool LoadHeightMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	bool InitializeBuffers(ID3D10Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D10Device*);

public:
	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount, m_indexCount;
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;

	HeightMapType* m_heightMap;

};

#endif