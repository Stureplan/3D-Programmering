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
#include "terrainclass.h"
#include "frustumclass.h"
#include "quadtreeclass.h"

#include "textclass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 1.0f;
const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;
const float SHADOWMAP_DEPTH = 50.0f;
const float SHADOWMAP_NEAR = 1.0f;


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
	bool Frame(int, int, float);
	void Move(int);
	void Launch();
	bool RenderText();

private:
	bool RenderSceneToTexture();
	bool Render(float);


private:
	D3DClass*	 m_D3D;
	CameraClass* m_Camera;
	LightClass*  m_EnvironmentLight;
	LightClass*  m_ObjectLight;

	ModelClass* m_Gun;
	ModelClass* m_Cube;
	ModelClass* m_GroundCube;
	ModelClass* m_NormalCube;

	ConverterClass*		  m_Convert;
	ShadowShaderClass*	  m_ShadowShader;
	DepthShaderClass*	  m_DepthShader;
	RenderTextureClass*	  m_RenderTexture;
	NormalMapShaderClass* m_NormalMapShader;
	
	TerrainClass*		  m_Terrain;
	FrustumClass*		  m_Frustum;
	QuadTreeClass*		  m_QuadTree;
	
	TextClass*			  m_Text;
	ID3DX10Font* font;

	float movespeed, rotatespeed;
	D3DXVECTOR3 gun, cube, def, ground, cube2, terrain;
	D3DXVECTOR3 cam_pos, gun_pos, gun_offset, rotate;
	D3DXVECTOR3 camera_forward, camera_lookat, camera_up;
	D3DXVECTOR3 camera_left, camera_right;
	D3DXMATRIX rot;

	float specular_none, specular_matte, specular_shiny;

	D3DX10_FONT_DESC fd;
	D3DXCOLOR fontColor;
	RECT rectangleFps;
	RECT rectangleCpu;
	RECT rectangleRenderCount;
	int t_fps, t_cpu, t_renderCount;
	float frametime;
};

#endif