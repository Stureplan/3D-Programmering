////////////////////////////////////////////////////////////////////////////////
// Filename: quadtreeclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _QUADTREECLASS_H_
#define _QUADTREECLASS_H_


/////////////
// GLOBALS //
/////////////
const int MAX_TRIANGLES = 10000;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "terrainclass.h"
#include "frustumclass.h"
#include "shadowshaderclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: QuadTreeClass
////////////////////////////////////////////////////////////////////////////////
class QuadTreeClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct VectorType
	{
		float x, y, z;
	};

	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D10Buffer *vertexBuffer, *indexBuffer;
		VectorType* vertexArray;
		NodeType* nodes[4];
	};

public:
	QuadTreeClass();
	QuadTreeClass(const QuadTreeClass&);
	~QuadTreeClass();

	bool Initialize(TerrainClass*, ID3D10Device*, D3DXVECTOR3);
	void Shutdown();
	void Render(FrustumClass*, ID3D10Device*, ShadowShaderClass*);

	int GetDrawCount();
	bool GetHeightAtPosition(float, float, float&);

private:
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(NodeType*, float, float, float, ID3D10Device*);
	int CountTriangles(float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void ReleaseNode(NodeType*);
	void RenderNode(NodeType*, FrustumClass*, ID3D10Device*, ShadowShaderClass*);

	void FindNode(NodeType*, float, float, float&);
	bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);

private:
	int m_triangleCount, m_drawCount;
	VertexType* m_vertexList;
	NodeType* m_parentNode;
	D3DXVECTOR3 terrain_offset;
};

#endif