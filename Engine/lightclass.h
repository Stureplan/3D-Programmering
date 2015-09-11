///////////////////////////////
// lightclass.h
//////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

//INCLUDES
#include <D3DX10math.h>


//CLASS NAME: LightClass
class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor (float, float, float, float);
	void SetDiffuseColor (float, float, float, float);
	void SetPosition	 (float, float, float);
	void SetLookAt		 (float, float, float);
	void SetDirection	 (float, float, float);

	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetDirection ();

	void GenerateViewMatrix ();
	void GenerateOrthoMatrix (float, float, float);

	void GetViewMatrix  (D3DXMATRIX&);
	void GetOrthoMatrix (D3DXMATRIX&);

private:
	D3DXVECTOR4 m_ambientColor;
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_lookAt;
	D3DXVECTOR3 m_direction;
	D3DXMATRIX  m_viewMatrix;
	D3DXMATRIX  m_orthoMatrix;
};


#endif