////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightclass.h"
#include "converter.h"
#include "rendertextureclass.h"
#include "depthshaderclass.h"
#include "shadowshaderclass.h"
#include "normalmapshaderclass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN		= false;
const bool VSYNC_ENABLED	= true;
const float SCREEN_DEPTH	= 100.0f;
const float SCREEN_NEAR		= 1.0f;
const int SHADOWMAP_WIDTH	= 1024;
const int SHADOWMAP_HEIGHT	= 1024;
const float SHADOWMAP_DEPTH = 50.0f;
const float SHADOWMAP_NEAR  = 1.0f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	void Move (int);
	void Launch ();

private:
	bool RenderSceneToTexture ();
	bool Render(float);


private:
	D3DClass*	 m_D3D;
	CameraClass* m_Camera;
	LightClass*  m_Light;

	ModelClass* m_Gun;
	ModelClass* m_Cube;
	ModelClass* m_GroundCube;
	ModelClass* m_NormalCube;

	ConverterClass*		  m_Convert;
	ShadowShaderClass*	  m_ShadowShader;
	DepthShaderClass*	  m_DepthShader;
	RenderTextureClass*	  m_RenderTexture;
	NormalMapShaderClass* m_NormalMapShader;

	float movespeed, rotatespeed;
	D3DXVECTOR3 gun, cube, def, ground, cube2;
	D3DXVECTOR3 cam_pos, gun_pos, gun_offset, rotate;
	D3DXVECTOR3 camera_forward, camera_lookat, camera_up;
	D3DXVECTOR3 camera_left, camera_right;
	D3DXMATRIX rot;
};

#endif