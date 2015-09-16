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
	m_Terrain = 0;
	m_Frustum = 0;
	m_QuadTree = 0;

	movespeed = 0.4f;
	rotatespeed = 1.0f;

	//These are the POSITIONS for each object in the scene
	gun = D3DXVECTOR3(0.5f, -0.5f, -3.5f);
	cube = D3DXVECTOR3(0.0f, 0.3f, 0.0f);
	def = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	ground = D3DXVECTOR3(0.0f, -2.0f, 0.0f);
	cube2 = D3DXVECTOR3(2.5f, 0.3f, 0.0f);
	terrain = D3DXVECTOR3(100.0f, 100.0f, 100.0f);


	D3DXMatrixIdentity(&rot);
	camera_up = { 0.0f, 1.0f, 0.0f };
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
	if (!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Create the terrain object
	m_Terrain = new TerrainClass;

	// Initialize the terrain object
	result = m_Terrain->Initialize(m_D3D->GetDevice(), "../Engine/data/heightmap01.bmp", L"../Engine/data/grass.jpg");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);


	m_Convert = new ConverterClass;
	if (!m_Convert)
	{
		return false;
	}

	m_Convert->Convert(1);	//Convert model01 (gun)
	m_Convert->Convert(2);	//Convert model02 (cube)
	m_Convert->Convert(3); //Convert model03 (ground plane)

						   // Create the model objects.
	m_Gun = new ModelClass(gun, m_D3D->GetDevice(), "../Engine/data/model01.txt", L"../Engine/data/dog.jpg", false);
	m_Cube = new ModelClass(cube, m_D3D->GetDevice(), "../Engine/data/model02.txt", L"../Engine/data/dog.jpg", false);
	m_GroundCube = new ModelClass(ground, m_D3D->GetDevice(), "../Engine/data/model03.txt", L"../Engine/data/dog.jpg", false);
	m_NormalCube = new ModelClass(cube2, m_D3D->GetDevice(), "../Engine/data/model02.txt", L"../Engine/data/dog.jpg", true);

	//Create the new light object.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetLookAt(-2.0f, -1.0f, 0.0f);
	m_Light->GenerateOrthoMatrix(20.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
	m_Light->SetPosition(2.0f, 1.0f, 0.0f);
	m_Light->SetDirection(-2.0f, -1.0f, 0.0f);

	m_RenderTexture = new RenderTextureClass;
	m_DepthShader = new DepthShaderClass;
	m_ShadowShader = new ShadowShaderClass;
	m_NormalMapShader = new NormalMapShaderClass;

	m_RenderTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
	m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd);
	m_ShadowShader->Initialize(m_D3D->GetDevice(), hwnd);
	m_NormalMapShader->Initialize(m_D3D->GetDevice(), hwnd);

	// Create new frustum object.
	m_Frustum = new FrustumClass;
	if (!m_Frustum)
	{
		return false;
	}

	// Create the quad tree object.
	m_QuadTree = new QuadTreeClass;
	if (!m_QuadTree)
	{
		return false;
	}

	// Initializde the quad tree object.
	result = m_QuadTree->Initialize(m_Terrain, m_D3D->GetDevice());
	if (!result)
	{
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the quad tree object.
	if (m_QuadTree)
	{
		m_QuadTree->Shutdown();
		delete m_QuadTree;
		m_QuadTree = 0;
	}

	// Release the frustum object.
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	//Release objects
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the terrain object
	if (m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	if (m_ShadowShader)
	{
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	}

	if (m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	if (m_NormalMapShader)
	{
		m_NormalMapShader->Shutdown();
		delete m_NormalMapShader;
		m_NormalMapShader = 0;
	}

	if (m_Gun)
	{
		m_Gun->Shutdown();
		delete m_Gun;
		m_Gun = 0;
	}

	if (m_Cube)
	{
		m_Cube->Shutdown();
		delete m_Cube;
		m_Cube = 0;
	}

	if (m_GroundCube)
	{
		m_GroundCube->Shutdown();
		delete m_GroundCube;
		m_GroundCube = 0;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_D3D)
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
	rotation += (float)D3DX_PI * 0.005f;
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
	cam_pos = m_Camera->GetPosition();
	gun_pos = m_Gun->GetPosition();
	rotate = m_Camera->GetRotation();
	camera_lookat = m_Camera->GetLookAt();

	camera_forward = camera_lookat - cam_pos;
	D3DXVec3Normalize(&camera_forward, &camera_forward);

	if (dir == 1)	//MOVE FWD
	{
		cam_pos = cam_pos + camera_forward * movespeed;
		camera_lookat = camera_lookat + camera_forward * movespeed;
		m_Camera->SetPosition(cam_pos.x, cam_pos.y, cam_pos.z);
	}

	if (dir == 2)	//MOVE LEFT
	{
		D3DXVec3Cross(&camera_left, &camera_forward, &camera_up);

		cam_pos = cam_pos + camera_left * movespeed;
		camera_lookat = camera_lookat + camera_left * movespeed;
		m_Camera->SetPosition(cam_pos.x, cam_pos.y, cam_pos.z);
	}

	if (dir == 3)	//MOVE BACK
	{
		cam_pos = cam_pos - camera_forward * movespeed;
		camera_lookat = camera_lookat - camera_forward * movespeed;
		m_Camera->SetPosition(cam_pos.x, cam_pos.y, cam_pos.z);
	}

	if (dir == 4)	//MOVE RIGHT
	{
		D3DXVec3Cross(&camera_right, &camera_up, &camera_forward);

		cam_pos = cam_pos + camera_right * movespeed;
		camera_lookat = camera_lookat + camera_right * movespeed;
		m_Camera->SetPosition(cam_pos.x, cam_pos.y, cam_pos.z);
	}

	if (dir == 5) //ROTATE LEFT
	{
		m_Camera->SetRotation(rotate.x, rotate.y - rotatespeed, rotate.z);
	}

	if (dir == 6) //ROTATE RIGHT
	{
		m_Camera->SetRotation(rotate.x, rotate.y + rotatespeed, rotate.z);
	}
}

void GraphicsClass::Launch ()
{

}

bool GraphicsClass::RenderSceneToTexture()
{
	D3DXMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix, translateMatrix;
	D3DXVECTOR3 xyz, scale;

	m_RenderTexture->SetRenderTarget(m_D3D->GetDevice());
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Light->GenerateViewMatrix();
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetOrthoMatrix(lightOrthoMatrix);

	//Render cube shadow
	xyz = m_Cube->GetPosition();
	D3DXMatrixTranslation(&worldMatrix, xyz.x, xyz.y, xyz.z);
	D3DXMatrixMultiply(&worldMatrix, &rot, &worldMatrix);

	m_Cube->Render(m_D3D->GetDevice());
	m_DepthShader->Render(m_D3D->GetDevice(), m_Cube->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);

	//Reset
	m_D3D->GetWorldMatrix(worldMatrix);

	//Render ground shadow
	xyz = m_GroundCube->GetPosition();
	//scale = m_GroundCube->GetScale ();
	D3DXMatrixTranslation(&worldMatrix, xyz.x, xyz.y, xyz.z);
	//D3DXMatrixScaling (&worldMatrix, scale.x, scale.y, scale.z);
	m_GroundCube->Render(m_D3D->GetDevice());
	m_DepthShader->Render(m_D3D->GetDevice(), m_GroundCube->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);

	//Reset
	m_D3D->GetWorldMatrix(worldMatrix);

	//Render normal cube shadow
	xyz = m_NormalCube->GetPosition();
	D3DXMatrixTranslation(&worldMatrix, xyz.x, xyz.y, xyz.z);
	D3DXMatrixMultiply(&worldMatrix, &rot, &worldMatrix);

	m_NormalCube->Render(m_D3D->GetDevice());
	m_DepthShader->Render(m_D3D->GetDevice(), m_NormalCube->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);

	//Set rendering target to normal
	m_D3D->SetBackBufferRenderTarget();
	m_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	D3DXMATRIX scaleMatrix, rotationMatrix;
	D3DXMATRIX translationMatrix;

	D3DXMATRIX lightViewMatrix, lightOrthoMatrix;

	D3DXVECTOR3 pos;

	int index;
	float pos_x, pos_y, pos_z;

	bool renderModel = false;
	bool result;

	RenderSceneToTexture();

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();
	m_Light->GenerateViewMatrix();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetOrthoMatrix(lightOrthoMatrix);

	// Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// Set the terrain shader parameters that it will use for rendering
	result = m_ShadowShader->SetShaderParametersTerrain(m_D3D->GetDevice(), worldMatrix, viewMatrix, 
									projectionMatrix, m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), 
									m_Light->GetDirection(), m_Terrain->GetTexture());
	if (!result)
	{
		return false;
	}

	// Render the terrain using the quad tree and terrain shader.
	m_QuadTree->Render(m_Frustum, m_D3D->GetDevice(), m_ShadowShader);

/*	// Render the terrain buffers
	m_Terrain->Render(m_D3D->GetDevice());

	// Render the terrain using the light shader
	//m_LightShader->Render(m_D3D->GetDevice(), m_Terrain->GetIndexCount(), worldMatrix, 
	//viewMatrix, projectionMatrix, m_Terrain->GetTexture(), m_Light->GetDirection(), 
	//m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
*/
	D3DXMatrixTranslation(&translationMatrix, 0.0f, -5.0f, 0.0f);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);

/*	//NEW SHADOWSHADER FOR TERRAIN
	m_ShadowShader->Render(m_D3D->GetDevice(), m_Terrain->GetIndexCount(),
						   worldMatrix, viewMatrix, projectionMatrix,
						   lightViewMatrix, lightOrthoMatrix,
						   m_Terrain->GetTexture(), m_RenderTexture->GetShaderResourceView(),
						   m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
*/
	m_D3D->GetWorldMatrix(worldMatrix);
	
	//					--v-- OBJECT HANDLING --v--
	//1. Gun
	//-------------------------------------------------------------------------//
	pos = m_Gun->GetPosition();
	renderModel = m_Frustum->CheckSphere(pos.x, pos.y, pos.z, 0.1f);

	if (renderModel == true)
	{
		D3DXMatrixTranslation(&translationMatrix, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);

		m_Gun->Render(m_D3D->GetDevice());
		m_ShadowShader->Render(m_D3D->GetDevice(), m_Gun->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			lightViewMatrix, lightOrthoMatrix,
			m_Gun->GetTexture(), m_RenderTexture->GetShaderResourceView(),
			m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	}
	//-------------------------------------------------------------------------//




	//2. Cube
	//-------------------------------------------------------------------------//
	pos = m_Cube->GetPosition();
	renderModel = m_Frustum->CheckSphere(pos.x, pos.y, pos.z, 0.8f);

	if (renderModel == true)
	{
		m_D3D->GetWorldMatrix(worldMatrix);
		D3DXMatrixTranslation(&worldMatrix, pos.x, pos.y, pos.z);


		//rotation
		D3DXMatrixRotationY(&worldMatrix, rotation);
		D3DXMatrixRotationY(&rot, rotation);

		D3DXMatrixTranslation(&translationMatrix, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);




		m_Cube->Render(m_D3D->GetDevice());
		m_ShadowShader->Render(m_D3D->GetDevice(), m_Cube->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			lightViewMatrix, lightOrthoMatrix,
			m_Cube->GetTexture(), m_RenderTexture->GetShaderResourceView(),
			m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	}
	//-------------------------------------------------------------------------//




	//3. GroundCube
	//-------------------------------------------------------------------------//
	pos = m_GroundCube->GetPosition();
	renderModel = m_Frustum->CheckSphere(pos.x, pos.y, pos.z, 6.0f);

	if (renderModel == true)
	{
		m_D3D->GetWorldMatrix(worldMatrix);
		D3DXMatrixTranslation(&worldMatrix, pos.x, pos.y, pos.z);

		m_GroundCube->Render(m_D3D->GetDevice());
		m_ShadowShader->Render(m_D3D->GetDevice(), m_Cube->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			lightViewMatrix, lightOrthoMatrix,
			m_Cube->GetTexture(), m_RenderTexture->GetShaderResourceView(),
			m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	}
	//-------------------------------------------------------------------------//




	//3. NormalCube
	//-------------------------------------------------------------------------//
	pos = m_NormalCube->GetPosition();
	renderModel = m_Frustum->CheckSphere(pos.x, pos.y, pos.z, 0.8f);

	if (renderModel == true)
	{
		m_D3D->GetWorldMatrix(worldMatrix);
		D3DXMatrixTranslation(&worldMatrix, pos.x, pos.y, pos.z);

		//rotation
		D3DXMatrixRotationY(&worldMatrix, rotation);
		D3DXMatrixRotationY(&rot, rotation);

		D3DXMatrixTranslation(&translationMatrix, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);

		m_NormalCube->Render(m_D3D->GetDevice());
		m_NormalMapShader->Render(m_D3D->GetDevice(), m_NormalCube->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			m_NormalCube->GetTexture(), m_NormalCube->GetNormalmap(),
			m_Light->GetDirection(), m_Light->GetDiffuseColor());
	}
	//-------------------------------------------------------------------------//

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}