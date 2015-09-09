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

	m_Gun = 0;
	m_Cube = 0;
	m_GroundCube = 0;

	m_ShadowShader = 0;
	m_DepthShader = 0;
	m_RenderTexture = 0;
	m_NormalMapShader = 0;

	movespeed = 0.09f;
	rotatespeed = 1.0f;

	//These are the POSITIONS for each object in the scene
	gun		= D3DXVECTOR3 (0.5f, -0.5f, -3.5f);
	cube	= D3DXVECTOR3 (0.0f, 0.3f, 0.0f);
	def		= D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
	ground	= D3DXVECTOR3 (0.0f, -2.0f, 0.0f);
	cube2	= D3DXVECTOR3 (2.5f, 0.3f, 0.0f);


	D3DXMatrixIdentity(&rot);
	camera_up  = { 0.0f, 1.0f, 0.0f };
	gun_offset = { 0.5f,-0.5f, 1.5f };
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	

	m_Convert = new ConverterClass;
	if (!m_Convert)
	{
		return false;
	}

	//Convert all the models we're using to our format
	m_Convert->Convert (L"../Engine/data/model01.obj", 1);	//Convert model01 (gun)
	m_Convert->Convert (L"../Engine/data/model02.obj", 2);	//Convert model02 (cube)
	m_Convert->Convert (L"../Engine/data/model03.obj", 3); //Convert model03 (ground plane)

	// Create the model objects.
	m_Gun		 = new ModelClass (gun,    m_D3D->GetDevice (), L"../Engine/data/model01.txt", L"../Engine/data/dog.jpg", false);
	m_Cube		 = new ModelClass (cube,   m_D3D->GetDevice (), L"../Engine/data/model02.txt", L"../Engine/data/dog.jpg", false);
	m_GroundCube = new ModelClass (ground, m_D3D->GetDevice (), L"../Engine/data/model03.txt", L"../Engine/data/dog.jpg", false);
	m_NormalCube = new ModelClass (cube2,  m_D3D->GetDevice (), L"../Engine/data/model02.txt", L"../Engine/data/dog.jpg", true);

	//Create the new light object.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	m_Light->SetAmbientColor	 (0.05f, 0.05f, 0.05f, 1.0f);
	m_Light->SetDiffuseColor	 (1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetLookAt			 (-2.0f, -1.0f, 0.0f);
	m_Light->GenerateOrthoMatrix (20.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
	m_Light->SetPosition		 (2.0f, 5.0f, 0.0f);
	m_Light->SetDirection		 (-2.0f, -1.0f, 0.0f);

	m_RenderTexture	  = new RenderTextureClass;
	m_DepthShader	  = new DepthShaderClass;
	m_ShadowShader	  = new ShadowShaderClass;
	m_NormalMapShader = new NormalMapShaderClass;

	m_RenderTexture	 ->Initialize (m_D3D->GetDevice (), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
	m_DepthShader	 ->Initialize (m_D3D->GetDevice (), hwnd);
	m_ShadowShader	 ->Initialize (m_D3D->GetDevice (), hwnd);
	m_NormalMapShader->Initialize (m_D3D->GetDevice (), hwnd);

	return true;
}


void GraphicsClass::Shutdown()
{
	//Release objects
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

	if (m_NormalMapShader)
	{
		m_NormalMapShader->Shutdown ();
		delete m_NormalMapShader;
		m_NormalMapShader = 0;
	}

	if(m_Gun)
	{
		m_Gun->Shutdown();
		delete m_Gun;
		m_Gun = 0;
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
	bool result;
	static float rotation = 0.0f;
	rotation += (float) D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	
	// Render the graphics scene.
	Render(rotation);

	return true;
}

void GraphicsClass::Move (int dir)
{
	cam_pos		  = m_Camera->GetPosition ();
	gun_pos		  = m_Gun	->GetPosition ();
	rotate		  = m_Camera->GetRotation ();
	camera_lookat = m_Camera->GetLookAt	  ();

	camera_forward = camera_lookat - cam_pos;
	D3DXVec3Normalize (&camera_forward, &camera_forward);

	if (dir == 1)	//MOVE FWD
	{
		cam_pos		  = cam_pos		  + camera_forward * movespeed;
		camera_lookat = camera_lookat + camera_forward * movespeed;
		m_Camera->SetPosition (cam_pos.x, cam_pos.y, cam_pos.z);
	}

	if (dir == 2)	//MOVE LEFT
	{
		D3DXVec3Cross (&camera_left, &camera_forward, &camera_up);

		cam_pos		  = cam_pos		  + camera_left * movespeed;
		camera_lookat = camera_lookat + camera_left * movespeed;
		m_Camera->SetPosition (cam_pos.x, cam_pos.y, cam_pos.z);
	}

	if (dir == 3)	//MOVE BACK
	{
		cam_pos		  = cam_pos		  - camera_forward * movespeed;
		camera_lookat = camera_lookat - camera_forward * movespeed;
		m_Camera->SetPosition (cam_pos.x, cam_pos.y, cam_pos.z);
	}

	if (dir == 4)	//MOVE RIGHT
	{
		D3DXVec3Cross (&camera_right, &camera_up, &camera_forward);

		cam_pos		  = cam_pos		  + camera_right * movespeed;
		camera_lookat = camera_lookat + camera_right * movespeed;
		m_Camera->SetPosition (cam_pos.x, cam_pos.y, cam_pos.z);
	}

	if (dir == 5) //ROTATE LEFT
	{
		m_Camera->SetRotation (rotate.x, rotate.y - rotatespeed, rotate.z);
	}

	if (dir == 6) //ROTATE RIGHT
	{
		m_Camera->SetRotation (rotate.x, rotate.y + rotatespeed, rotate.z);
	}

	//gun_pos = camera_lookat;
	//m_Gun->SetPosition (gun_pos.x, gun_pos.y, gun_pos.z);
	//m_Gun->SetRotation (rotate.x, rotate.y, rotate.z);
}

void GraphicsClass::Launch ()
{

}

bool GraphicsClass::RenderSceneToTexture ()
{
	D3DXMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix, translateMatrix;
	D3DXVECTOR3 xyz, scale;

	m_RenderTexture->SetRenderTarget   (m_D3D->GetDevice ());
	m_RenderTexture->ClearRenderTarget (m_D3D->GetDevice (), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Light->GenerateViewMatrix ();
	m_D3D  ->GetWorldMatrix (worldMatrix);
	m_Light->GetViewMatrix	(lightViewMatrix);
	m_Light->GetOrthoMatrix (lightOrthoMatrix);

	//Render cube shadow
	xyz = m_Cube->GetPosition ();
	D3DXMatrixTranslation (&worldMatrix, xyz.x, xyz.y, xyz.z);
	D3DXMatrixMultiply (&worldMatrix, &rot, &worldMatrix);

	m_Cube		 ->Render (m_D3D->GetDevice ());
	m_DepthShader->Render (m_D3D->GetDevice (), m_Cube->GetIndexCount (), worldMatrix, lightViewMatrix, lightOrthoMatrix);
	
	//Reset
	m_D3D->GetWorldMatrix (worldMatrix);

	//Render ground shadow
	xyz = m_GroundCube->GetPosition ();
	//scale = m_GroundCube->GetScale ();
	D3DXMatrixTranslation (&worldMatrix, xyz.x, xyz.y, xyz.z);
	//D3DXMatrixScaling (&worldMatrix, scale.x, scale.y, scale.z);
	m_GroundCube ->Render (m_D3D->GetDevice ());
	m_DepthShader->Render (m_D3D->GetDevice (), m_GroundCube->GetIndexCount (), worldMatrix, lightViewMatrix, lightOrthoMatrix);

	//Reset
	m_D3D->GetWorldMatrix (worldMatrix);

	//Render normal cube shadow
	xyz = m_NormalCube->GetPosition ();
	D3DXMatrixTranslation (&worldMatrix, xyz.x, xyz.y, xyz.z);
	D3DXMatrixMultiply(&worldMatrix, &rot, &worldMatrix);
	
	m_NormalCube ->Render (m_D3D->GetDevice ());
	m_DepthShader->Render (m_D3D->GetDevice (), m_NormalCube->GetIndexCount (), worldMatrix, lightViewMatrix, lightOrthoMatrix);

	//Set rendering target to normal
	m_D3D->SetBackBufferRenderTarget ();
	m_D3D->ResetViewport ();

	return true;
}


bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	D3DXMATRIX scaleMatrix, rotationMatrix;
	D3DXMATRIX translationMatrix;

	D3DXMATRIX lightViewMatrix, lightOrthoMatrix;

	D3DXVECTOR3 pos, rotate;

	int index;
	float pos_x, pos_y, pos_z;
	
	RenderSceneToTexture ();

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	m_Light->GenerateViewMatrix ();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera ->GetViewMatrix(viewMatrix);
	m_D3D	 ->GetWorldMatrix(worldMatrix);
	m_D3D	 ->GetProjectionMatrix(projectionMatrix);

	m_Light->GetViewMatrix  (lightViewMatrix);
	m_Light->GetOrthoMatrix (lightOrthoMatrix);


	//					--v-- OBJECT HANDLING --v--
	//1. Gun
	//-------------------------------------------------------------------------//
	pos = m_Gun->GetPosition ();
	D3DXMatrixTranslation (&translationMatrix, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply (&worldMatrix, &worldMatrix, &translationMatrix);

	m_Gun		  ->Render (m_D3D->GetDevice ());
	m_ShadowShader->Render (m_D3D->GetDevice(), m_Gun->GetIndexCount (),
							worldMatrix, viewMatrix, projectionMatrix,
							lightViewMatrix, lightOrthoMatrix,
							m_Gun->GetTexture (), m_RenderTexture->GetShaderResourceView (),
							m_Light->GetDirection (), m_Light->GetAmbientColor (), m_Light->GetDiffuseColor ());

	//-------------------------------------------------------------------------//




	//2. Cube
	//-------------------------------------------------------------------------//
	m_D3D->GetWorldMatrix (worldMatrix);
	pos = m_Cube->GetPosition ();
	D3DXMatrixTranslation (&worldMatrix, pos.x, pos.y, pos.z);


	//rotation
	D3DXMatrixRotationY(&worldMatrix, rotation);
	D3DXMatrixRotationY(&rot, rotation);

	D3DXMatrixTranslation(&translationMatrix, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);

	m_Cube		  ->Render (m_D3D->GetDevice ());
	m_ShadowShader->Render (m_D3D->GetDevice (), m_Cube->GetIndexCount (),
							worldMatrix, viewMatrix, projectionMatrix,
							lightViewMatrix, lightOrthoMatrix,
							m_Cube->GetTexture (), m_RenderTexture->GetShaderResourceView (),
							m_Light->GetDirection (), m_Light->GetAmbientColor (), m_Light->GetDiffuseColor ());
	//-------------------------------------------------------------------------//




	//3. GroundCube
	//-------------------------------------------------------------------------//
	m_D3D->GetWorldMatrix (worldMatrix);
	pos = m_GroundCube->GetPosition ();
	D3DXMatrixTranslation (&worldMatrix, pos.x, pos.y, pos.z);

	m_GroundCube  ->Render (m_D3D->GetDevice ());
	m_ShadowShader->Render (m_D3D->GetDevice (), m_Cube->GetIndexCount (),
							worldMatrix, viewMatrix, projectionMatrix,
							lightViewMatrix, lightOrthoMatrix,
							m_Cube->GetTexture (), m_RenderTexture->GetShaderResourceView (),
							m_Light->GetDirection (), m_Light->GetAmbientColor (), m_Light->GetDiffuseColor ());
	//-------------------------------------------------------------------------//




	//3. NormalCube
	//-------------------------------------------------------------------------//
	m_D3D->GetWorldMatrix (worldMatrix);
	pos = m_NormalCube->GetPosition ();
	D3DXMatrixTranslation (&worldMatrix, pos.x, pos.y, pos.z);

	//rotation
	D3DXMatrixRotationY (&worldMatrix, rotation);
	D3DXMatrixRotationY(&rot, rotation);
	
	D3DXMatrixTranslation (&translationMatrix, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply	  (&worldMatrix, &worldMatrix, &translationMatrix);

	m_NormalCube	 ->Render (m_D3D->GetDevice ());
	m_NormalMapShader->Render (m_D3D->GetDevice (), m_NormalCube->GetIndexCount (),
							   worldMatrix, viewMatrix, projectionMatrix,
							   m_NormalCube->GetTexture (), m_NormalCube->GetNormalmap (),
							   m_Light->GetDirection (), m_Light->GetDiffuseColor ());
	//-------------------------------------------------------------------------//


	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}