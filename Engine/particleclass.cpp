//Filename: particleclass.cpp
#include "particleclass.h"

ParticleClass::ParticleClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}

ParticleClass::ParticleClass(const ParticleClass& other)
{
}

ParticleClass::ParticleClass(ID3D10Device* device)
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	D3DXVECTOR3 pos = RandomPosition();
	
	particleh = pos.y;
	SetPosition(pos.x, pos.y, pos.z);
	Initialize(device);
	LoadTexture(device);

	
}

ParticleClass::~ParticleClass()
{
}

void ParticleClass::Initialize(ID3D10Device* device)
{
	ParticleType* particle;
	unsigned long* index;

	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;

	particle = new ParticleType[1];
	index = new unsigned long[1];

	//particle->position = position;
	index[0] = 1;

	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ParticleType);
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	vertexData.pSysMem = particle;

	device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long);
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	indexData.pSysMem = index;

	device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

	delete[0] particle;
	particle = 0;

	delete[0] index;
	index = 0;
}

void ParticleClass::Shutdown()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

ID3D10ShaderResourceView* ParticleClass::GetTexture()
{
	return m_Texture;
}

D3DXVECTOR3 ParticleClass::RandomPosition()
{
	D3DXVECTOR3 pos;
	int minX, maxX;
	int minZ, maxZ;
	
	int x, y, z;
	x = rand() % 255 + 20;
	y = rand() % 50 + 40;
	z = rand() % 265 + 50;

	pos.x = x;
	pos.y = y;
	pos.z = z;


	return pos;
}

void ParticleClass::Render(ID3D10Device* device)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(ParticleType);
	offset = 0;


	

	device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	SetPosition(position.x, position.y - 0.1f, position.z);

	if (position.y < 0.0f)
	{
		SetPosition(position.x, particleh, position.z);
	}
}

void ParticleClass::LoadTexture(ID3D10Device* device)
{
	WCHAR* filename = L"../Engine/data/dog.jpg";
	D3DX10CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_Texture, NULL);
}

void ParticleClass::SetPosition(float x, float y, float z)
{
	position = D3DXVECTOR3(x, y, z);
}

D3DXVECTOR3 ParticleClass::GetPosition()
{
	return position;
}