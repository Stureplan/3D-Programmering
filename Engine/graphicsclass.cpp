////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#define WM_MOUSEMOVE


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_EnvironmentLight = 0;
	m_ObjectLight = 0;

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
	
	m_Text = 0;

	movespeed = 0.6f;
	rotatespeed = 1.0f;

	//These are the POSITIONS for each object in the scene
	gun = D3DXVECTOR3(0.5f, -0.5f, -2.5f);
	cube = D3DXVECTOR3(0.0f, 0.3f, 0.0f);
	def = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	ground = D3DXVECTOR3(0.0f, -2.0f, 0.0f);
	cube2 = D3DXVECTOR3(2.5f, 0.3f, 0.0f);
	terrain = D3DXVECTOR3(20.0f, -5.0f, 0.0f);
	

	D3DXMatrixIdentity(&rot);
	camera_up = { 0.0f, 1.0f, 0.0f };
	gun_offset = { 0.5f, -0.5f, 1.5f };

	specular_none  = 0.0f;
	specular_matte = 32.0f;
	specular_shiny = 8.0f;
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
	D3DXMATRIX baseViewMatrix;
	char videoCard[128];
	int videoMemory;


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

	// Initialize the terrain object
	m_Terrain = new TerrainClass;
	m_Terrain->Initialize(m_D3D->GetDevice(), "../Engine/data/heightmap01.bmp", L"../Engine/data/grass.jpg");

	// Initalize the frustum object
	m_Frustum = new FrustumClass;

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 3.0f, 0.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Create the text object.
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->GetDevice(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	m_Convert = new ConverterClass;
	if (!m_Convert)
	{
		return false;
	}

	// Retrieve the video card information.
	//m_D3D->GetVideoCardInfo(videoCard, videoMemory);

	// Set the video card information in the text object.
	//result = m_Text->SetVideoCardInfo(videoCard, videoMemory, m_D3D->GetDevice());
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not set video card info in the text object.", L"Error", MB_OK);
	//	return false;
	//}

	//Convert all the models we're using to our format
	m_Convert->Convert(L"../Engine/data/sphereRGB.obj", 1);	//Convert sphere
	m_Convert->Convert(L"../Engine/data/cubeTextured.obj", 2);	//Convert cube
	m_Convert->Convert(L"../Engine/data/planeTextured.obj", 3);	//Convert plane

	// Create the model objects.
	m_Gun = new ModelClass(gun, m_D3D->GetDevice(), L"../Engine/data/model01.txt", false);
	m_Cube = new ModelClass(cube, m_D3D->GetDevice(), L"../Engine/data/model01.txt", false);
	m_GroundCube = new ModelClass(ground, m_D3D->GetDevice(), L"../Engine/data/model03.txt", false);
	m_NormalCube = new ModelClass(cube2, m_D3D->GetDevice(), L"../Engine/data/model02.txt", true);

	m_EnvironmentLight = new LightClass;
	m_ObjectLight = new LightClass;

	m_EnvironmentLight->SetAmbientColor(0.02f, 0.02f, 0.02f, 1.0f);
	m_EnvironmentLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_EnvironmentLight->SetDirection(-2.0f, -1.0f, 0.0f);
	m_EnvironmentLight->GenerateOrthoMatrix(20.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
	m_EnvironmentLight->GenerateViewMatrix();

	m_ObjectLight->SetPosition(4.0f, 4.0f, 0.0f);
	m_ObjectLight->SetDirection(-4.0f, -4.0f, 0.0f);
	m_ObjectLight->GenerateOrthoMatrix(20.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
	m_ObjectLight->GenerateViewMatrix();

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

	// Create the quad tree object.
	m_QuadTree = new QuadTreeClass;
	if (!m_QuadTree)
	{
		return false;
	}

	// Initialize the quad tree object.
	result = m_QuadTree->Initialize(m_Terrain, m_D3D->GetDevice(), terrain);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the quad tree object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the text object.
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Release the quad tree object.
	if (m_QuadTree)
	{
		m_QuadTree->Shutdown();
		delete m_QuadTree;
		m_QuadTree = 0;
	}

	//Release terrain objects
	if (m_Terrain)
	{
		delete m_Terrain;
		m_Terrain = 0;
	}

	// Release the frustum object.
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	if (m_EnvironmentLight)
	{
		delete m_EnvironmentLight;
		m_EnvironmentLight = 0;
	}

	if (m_ObjectLight)
	{
		delete m_ObjectLight;
		m_ObjectLight = 0;
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


bool GraphicsClass::Frame(int fps, int cpu, float frameTime)
{
	bool result;

	// Set the frames per second.
	result = m_Text->SetFps(fps);
	if (!result)
	{
		return false;
	}

	// Set the cpu usage.
	result = m_Text->SetCpu(cpu);
	if (!result)
	{
		return false;
	}

	// Update the system stats
	
	
	static float rotation = 0.0f;
	rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Set the cpu usage.
	result = m_Text->SetCpu(cpu);
	if (!result)
	{
		return false;
	}

	// Set the frames per second.
	result = m_Text->SetFps(fps);
	if (!result);
	{
		return false;
	}

	// Render the graphics scene.
	Render(rotation);

	return true;
}

void GraphicsClass::Move(int dir)
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

	//gun_pos = camera_lookat;
	//m_Gun->SetPosition (gun_pos.x, gun_pos.y, gun_pos.z);
	//m_Gun->SetRotation (rotate.x, rotate.y, rotate.z);
}

void GraphicsClass::Launch()
{

}

bool GraphicsClass::RenderSceneToTexture()
{
	D3DXMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix, lightProjMatrix, translateMatrix;
	D3DXVECTOR3 xyz, scale;

	m_RenderTexture->SetRenderTarget(m_D3D->GetDevice());
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_D3D->GetWorldMatrix(worldMatrix);
	m_ObjectLight->GetViewMatrix(lightViewMatrix);
	m_ObjectLight->GetOrthoMatrix(lightOrthoMatrix);
	m_ObjectLight->GetProjMatrix(lightProjMatrix);

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
	D3DXMatrixTranslation(&worldMatrix, xyz.x, xyz.y, xyz.z);
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
	D3DXMATRIX objViewMatrix, objOrthoMatrix;


	D3DXVECTOR3 pos, rotate;

	bool rendermodel = false;

	RenderSceneToTexture();

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);

	m_Camera->GetViewMatrix(viewMatrix);

	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_EnvironmentLight->GetViewMatrix(lightViewMatrix);
	m_EnvironmentLight->GetOrthoMatrix(lightOrthoMatrix);

	m_ObjectLight->GetViewMatrix(objViewMatrix);
	m_ObjectLight->GetOrthoMatrix(objOrthoMatrix);


	//-------------------------------------------------------------------------//
	//					--/\-- TEXT TO SCREEN HANDLING --/\--				   //
	//-------------------------------------------------------------------------//

	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Render the text strings.
	m_Text->Render(m_D3D->GetDevice(), worldMatrix, objOrthoMatrix);

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();

	//-------------------------------------------------------------------------//
	//					--/\-- TEXT TO SCREEN HANDLING --/\--				   //
	//-------------------------------------------------------------------------//

	
	//-------------------------------------------------------------------------//
	//					--\/-- TERRAIN HANDLING --\/--						   //
	//-------------------------------------------------------------------------//
	
	D3DXMatrixTranslation(&translationMatrix, terrain.x, terrain.y, terrain.z);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);

	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	m_ShadowShader->SetShaderParametersTerrain(worldMatrix, viewMatrix, projectionMatrix, 
	objViewMatrix, objOrthoMatrix, m_Terrain->GetTexture(), m_RenderTexture->GetShaderResourceView(),
	m_EnvironmentLight->GetDirection(), m_EnvironmentLight->GetAmbientColor(), m_EnvironmentLight->GetDiffuseColor(),
	m_Camera->GetPosition(), specular_none);

	m_QuadTree->Render(m_Frustum, m_D3D->GetDevice(), m_ShadowShader);

	//-------------------------------------------------------------------------//
	//					--/\-- TERRAIN HANDLING --/\--						   //
	//-------------------------------------------------------------------------//



	//					--v-- OBJECT HANDLING --v--
	//1. Gun
	//-------------------------------------------------------------------------//
	
	pos = m_Gun->GetPosition();
	rendermodel = m_Frustum->CheckSphere(pos.x, pos.y, pos.z, 0.1f);

	if (rendermodel == true)
	{
		m_D3D->GetWorldMatrix(worldMatrix);

		D3DXMatrixTranslation(&translationMatrix, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);

		m_Gun->Render(m_D3D->GetDevice());
		m_ShadowShader->Render(m_D3D->GetDevice(), m_Gun->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			objViewMatrix, objOrthoMatrix,
			m_Gun->GetTexture(), m_RenderTexture->GetShaderResourceView(),
			m_ObjectLight->GetDirection(), m_EnvironmentLight->GetAmbientColor(), m_Gun->GetDiffuse(),
			m_Camera->GetPosition(), specular_matte);
	}
	//-------------------------------------------------------------------------//

	//2. Cube
	//-------------------------------------------------------------------------//

	pos = m_Cube->GetPosition();
	rendermodel = m_Frustum->CheckSphere(pos.x, pos.y, pos.z, 0.8f);

	if (rendermodel == true)
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
			objViewMatrix, objOrthoMatrix,
			m_Cube->GetTexture(), m_RenderTexture->GetShaderResourceView(),
			m_ObjectLight->GetDirection(), m_EnvironmentLight->GetAmbientColor(), m_Cube->GetDiffuse(),
			m_Camera->GetPosition(), specular_shiny);
	}
	//-------------------------------------------------------------------------//

	//3. GroundCube
	//-------------------------------------------------------------------------//
	pos = m_GroundCube->GetPosition();
	rendermodel = m_Frustum->CheckSphere(pos.x, pos.y, pos.z, 6.0f);

	if (rendermodel == true)
	{
		m_D3D->GetWorldMatrix(worldMatrix);

		D3DXMatrixTranslation(&worldMatrix, pos.x, pos.y, pos.z);

		m_GroundCube->Render(m_D3D->GetDevice());
		m_ShadowShader->Render(m_D3D->GetDevice(), m_GroundCube->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			objViewMatrix, objOrthoMatrix,
			m_GroundCube->GetTexture(), m_RenderTexture->GetShaderResourceView(),
			m_ObjectLight->GetDirection(), m_EnvironmentLight->GetAmbientColor(), m_GroundCube->GetDiffuse(),
			m_Camera->GetPosition(), specular_none);
	}
	//-------------------------------------------------------------------------//

	//3. NormalCube
	//-------------------------------------------------------------------------//

	pos = m_NormalCube->GetPosition();
	rendermodel = m_Frustum->CheckSphere(pos.x, pos.y, pos.z, 0.8f);

	if (rendermodel == true)
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
			m_ObjectLight->GetDirection(), m_EnvironmentLight->GetAmbientColor(), m_NormalCube->GetDiffuse());
	}
	//-------------------------------------------------------------------------//

	
	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}