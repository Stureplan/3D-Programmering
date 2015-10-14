////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#define WM_MOUSEMOVE


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Light = 0;

	m_Cube = 0;
	m_GroundCube = 0;

	m_ShadowShader = 0;
	m_DepthShader = 0;
	m_RenderTexture = 0;

	movespeed = 0.04f;

	//These are the POSITIONS for each object in the scene
	cube	= D3DXVECTOR3 (0.0f, 0.5f, 0.0f);
	def		= D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
	ground	= D3DXVECTOR3 (0.0f, -1.0f, 0.0f);


	D3DXMatrixIdentity(&rot);
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	m_D3D			= new D3DClass;
	m_D3D->Initialize (screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

	m_Camera		= new CameraClass;
	m_Convert		= new ConverterClass;
	m_Convert->Convert (2);	//Convert model02 (cube)
	m_Convert->Convert (3); //Convert model03 (ground plane)


	m_Cube			= new ModelClass (cube, m_D3D->GetDevice (), "../Engine/data/model02.txt", L"../Engine/data/dog.jpg");
	m_GroundCube	= new ModelClass (ground, m_D3D->GetDevice (), "../Engine/data/model03.txt", L"../Engine/data/dog.jpg");
	m_Light			= new LightClass;
	m_RenderTexture = new RenderTextureClass;
	m_DepthShader	= new DepthShaderClass;
	m_ShadowShader	= new ShadowShaderClass;





	m_Camera->SetPosition		 (0.0f, 0.0f, -5.0f);
	m_Light->SetAmbientColor	 (0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor	 (1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetLookAt			 (0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjMatrix  (SCREEN_DEPTH, SCREEN_NEAR);
	m_Light->SetPosition		 (5.0f, 5.0f, 0.0f);


	m_RenderTexture	->Initialize (m_D3D->GetDevice (), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
	m_DepthShader	->Initialize (m_D3D->GetDevice (), hwnd);
	m_ShadowShader	->Initialize (m_D3D->GetDevice (), hwnd);

	return true;
}


void GraphicsClass::Shutdown()
{
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	if (m_ShadowShader)
	{
		m_ShadowShader->Shutdown ();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	}

	if (m_DepthShader)
	{
		m_DepthShader->Shutdown ();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown ();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	if (m_Cube)
	{
		m_Cube->Shutdown ();
		delete m_Cube;
		m_Cube = 0;
	}

	if (m_GroundCube)
	{
		m_GroundCube->Shutdown ();
		delete m_GroundCube;
		m_GroundCube = 0;
	}

	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame()			
{
	static float rotation = 0.0f;
	rotation += (float) D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	//D3DXVECTOR3 pos = m_Cube->GetPosition();
	//m_Cube->SetPosition(pos.x+0.01f, pos.y, pos.z);

	Render (rotation);
	return true;
}

void GraphicsClass::Move (int dir)
{
	D3DXVECTOR3 pos;

	if (dir == 1)	//MOVE FWD
	{
		pos = m_Camera->GetPosition ();
		m_Camera->SetPosition (pos.x, pos.y, pos.z + movespeed);
	}

	if (dir == 2)	//MOVE LEFT
	{
		pos = m_Camera->GetPosition ();
		m_Camera->SetPosition (pos.x - movespeed, pos.y, pos.z);
	}

	if (dir == 3)	//MOVE BACK
	{
		pos = m_Camera->GetPosition ();
		m_Camera->SetPosition (pos.x, pos.y, pos.z - movespeed);
	}

	if (dir == 4)	//MOVE RIGHT
	{
		pos = m_Camera->GetPosition ();
		m_Camera->SetPosition (pos.x + movespeed, pos.y, pos.z);
	}
}


bool GraphicsClass::RenderSceneToTexture ()
{
	D3DXMATRIX worldMatrix, lightViewMatrix, lightProjMatrix, translateMatrix;
	D3DXVECTOR3 xyz;

	m_RenderTexture->SetRenderTarget   (m_D3D->GetDevice ());
	m_RenderTexture->ClearRenderTarget (m_D3D->GetDevice (), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Light->GenerateViewMatrix ();
	m_D3D  ->GetWorldMatrix (worldMatrix);
	m_Light->GetViewMatrix	(lightViewMatrix);
	m_Light->GetProjMatrix (lightProjMatrix);

	//Render cube shadow
	xyz = m_Cube->GetPosition ();
	D3DXMatrixTranslation (&worldMatrix, xyz.x, xyz.y, xyz.z);
	D3DXMatrixMultiply (&worldMatrix, &rot, &worldMatrix);

	m_Cube		 ->Render (m_D3D->GetDevice ());
	m_DepthShader->Render (m_D3D->GetDevice (), m_Cube->GetIndexCount (), worldMatrix, lightViewMatrix, lightProjMatrix);




	//Reset
	m_D3D->GetWorldMatrix (worldMatrix);

	//Render ground shadow
	xyz = m_GroundCube->GetPosition ();
	D3DXMatrixTranslation (&worldMatrix, xyz.x, xyz.y, xyz.z);

	m_GroundCube ->Render (m_D3D->GetDevice ());
	m_DepthShader->Render (m_D3D->GetDevice (), m_GroundCube->GetIndexCount (), worldMatrix, lightViewMatrix, lightProjMatrix);

	m_D3D->SetBackBufferRenderTarget ();
	m_D3D->ResetViewport ();
	return true;
}


bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	D3DXMATRIX rotationMatrix;
	D3DXMATRIX translationMatrix;

	D3DXMATRIX lightViewMatrix, lightProjMatrix;

	D3DXVECTOR3 pos;

	
	RenderSceneToTexture ();

	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	m_Camera->Render();
	m_Camera ->GetViewMatrix(viewMatrix);
	m_D3D	 ->GetWorldMatrix(worldMatrix);
	m_D3D	 ->GetProjectionMatrix(projectionMatrix);

	m_Light->GetViewMatrix  (lightViewMatrix);
	m_Light->GetProjMatrix  (lightProjMatrix);


	//					--v-- OBJECT HANDLING --v--
	//1. Cube
	//-------------------------------------------------------------------------//
	m_D3D->GetWorldMatrix (worldMatrix);

	pos = m_Cube->GetPosition ();
	D3DXMatrixTranslation (&worldMatrix, pos.x, pos.y, pos.z);
	D3DXMatrixRotationY(&rot, rotation);
	D3DXMatrixMultiply(&worldMatrix, &rot, &worldMatrix);




	m_Cube		  ->Render (m_D3D->GetDevice ());
	m_ShadowShader->Render (m_D3D->GetDevice (), m_Cube->GetIndexCount (),
							worldMatrix, viewMatrix, projectionMatrix,
							lightViewMatrix, lightProjMatrix,
							m_Cube->GetTexture (), m_RenderTexture->GetShaderResourceView (),
							m_Light->GetPosition(), m_Light->GetAmbientColor (), m_Light->GetDiffuseColor ());
	//-------------------------------------------------------------------------//

	//2. GroundCube
	//-------------------------------------------------------------------------//
	m_D3D->GetWorldMatrix (worldMatrix);
	pos = m_GroundCube->GetPosition ();
	D3DXMatrixTranslation (&worldMatrix, pos.x, pos.y, pos.z);

	m_GroundCube  ->Render (m_D3D->GetDevice ());
	m_ShadowShader->Render (m_D3D->GetDevice (), m_Cube->GetIndexCount (),
							worldMatrix, viewMatrix, projectionMatrix,
							lightViewMatrix, lightProjMatrix,
							m_Cube->GetTexture (), m_RenderTexture->GetShaderResourceView (),
							m_Light->GetPosition (), m_Light->GetAmbientColor (), m_Light->GetDiffuseColor ());
	//-------------------------------------------------------------------------//

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}