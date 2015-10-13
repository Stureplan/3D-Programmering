//Filename: particleclass.h

#ifndef _PARTICLECLASS_H_
#define _PARTICLECLASS_H_

#include <D3D10.h>
#include <D3DX10.h>

//Class name: ParticleClass
class ParticleClass
{
private:
	struct ParticleType
	{
		D3DXVECTOR3 position;
	};

public:
	ParticleClass();
	ParticleClass(const ParticleClass&);
	ParticleClass(ID3D10Device*);
	~ParticleClass();

	void Initialize(ID3D10Device*);
	void Render(ID3D10Device*);
	void Shutdown();
	ID3D10ShaderResourceView* GetTexture();
	void SetPosition(float, float, float);
	D3DXVECTOR3 GetPosition();

private:
	void LoadTexture(ID3D10Device*);
	D3DXVECTOR3 RandomPosition();

private:
	ID3D10Buffer* m_vertexBuffer;
	ID3D10Buffer* m_indexBuffer;
	ID3D10ShaderResourceView* m_Texture;
	D3DXVECTOR3 position;

	float particleh;
};

#endif