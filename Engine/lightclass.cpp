//FILENAME: lightclass.cpp
#include "lightclass.h"

LightClass::LightClass()
{
}

LightClass::LightClass(const LightClass& other)
{
}

LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor (float red, float green, float blue, float alpha)
{
	m_ambientColor = D3DXVECTOR4 (red, green, blue, alpha);
	return;
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetPosition(float x, float y, float z)
{
	m_position = D3DXVECTOR3(x, y, z);
	return;
}

void LightClass::SetLookAt (float x, float y, float z)
{
	m_lookAt.x = x;
	m_lookAt.y = y;
	m_lookAt.z = z;
	return;
}

void LightClass::SetDirection (float x, float y, float z)
{
	m_direction = D3DXVECTOR3 (x, y, z);
	return;
}

D3DXVECTOR4 LightClass::GetAmbientColor ()
{
	return m_ambientColor;
}

D3DXVECTOR4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}

D3DXVECTOR3 LightClass::GetPosition()
{
	return m_position;
}

D3DXVECTOR3 LightClass::GetDirection ()
{
	return m_direction;
}

void LightClass::GenerateViewMatrix ()
{
	D3DXVECTOR3 up;
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	D3DXMatrixLookAtLH (&m_viewMatrix, &m_position, &m_direction, &up);
	return;
}

void LightClass::GenerateOrthoMatrix (float width, float depthPlane, float nearPlane)
{
	D3DXMatrixOrthoLH (&m_orthoMatrix, width, width, nearPlane, depthPlane);
	return;
}

void LightClass::GenerateProjMatrix (float screenDepth, float screenNear)
{
	float fieldOfView, screenAspect;
	fieldOfView = (float) D3DX_PI / 2.0f;
	screenAspect = 1.0f;


	D3DXMatrixPerspectiveFovLH (&m_projMatrix, fieldOfView, screenAspect, screenNear, screenDepth);
	return;
}

void LightClass::GetViewMatrix (D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

void LightClass::GetOrthoMatrix (D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void LightClass::GetProjMatrix (D3DXMATRIX& projMatrix)
{
	projMatrix = m_projMatrix;
	return;
}