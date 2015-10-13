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
#include "shadowshaderclass.h"
#include "normalmapshaderclass.h"
#include "terrainclass.h"
#include "frustumclass.h"
#include "quadtreeclass.h"
#include "deferredshaderclass.h"
#include "orthowindowclass.h"
#include "textclass.h"
#include "particleshaderclass.h"
#include "particleclass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;
const float SHADOWMAP_DEPTH = 50.0f;
const float SHADOWMAP_NEAR = 1.0f;
const int part = 100;


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
	bool RenderSceneToTexture(float);
	bool Render(float);


private:
	D3DClass*	 m_D3D;
	CameraClass* m_Camera;
	LightClass*  m_ObjectLight;

	ModelClass* m_Cube;
	ModelClass* m_NormalCube;
	ParticleClass* m_ParticleSystem[part];
	

	ConverterClass*		  m_Convert;
	ShadowShaderClass*	  m_ShadowShader;
	RenderTextureClass*	  m_RenderTexture;	//"deferredbuffersclass"
	NormalMapShaderClass* m_NormalMapShader;

	ParticleShaderClass* m_ParticleShader;

	DeferredShaderClass*  m_Deferred;
	OrthoWindowClass*	  m_Window;

	TerrainClass*		  m_Terrain;
	FrustumClass*		  m_Frustum;
	QuadTreeClass*		  m_QuadTree;

	TextClass*			  m_Text;
	ID3DX10Font*		  font;

	float movespeed, rotatespeed;
	D3DXVECTOR3 cube, normalcube, def, ground, terrain;
	D3DXVECTOR3 cam_pos, rotate;
	D3DXVECTOR3 camera_forward, camera_lookat, camera_up;
	D3DXVECTOR3 camera_left, camera_right;
	D3DXMATRIX rot;

	float specular_none, specular_matte, specular_shiny;
	
	D3DXVECTOR4 terrain_diffuse;

	D3DX10_FONT_DESC fd;
	D3DXCOLOR fontColor;
	RECT rectangleFps;
	RECT rectangleCpu;
	RECT rectangleRenderCount;
	int t_fps, t_cpu, t_renderCount;
	float frametime;

};

#endif