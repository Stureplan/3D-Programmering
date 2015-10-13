////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
//#define WM_MOUSEMOVE

#pragma comment (lib, "d3dx10d.lib")

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_ObjectLight = 0;
	m_Cube = 0;

	for (int i = 0; i < part; i++)
	{
		m_ParticleSystem[i] = 0;
	}

	m_ShadowShader = 0;
	m_RenderTexture = 0;
	m_NormalMapShader = 0;

	m_ParticleShader = 0;
	m_Deferred = 0;
	m_Window = 0;

	m_Terrain = 0;
	m_Frustum = 0;
	m_QuadTree = 0;
	
	m_Text = 0;

	movespeed = 0.6f;
	rotatespeed = 1.0f;

	//These are the POSITIONS for each object in the scene
	cube = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	normalcube = D3DXVECTOR3 (2.0f, 0.0f, 0.0f);
	def = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	ground = D3DXVECTOR3(0.0f, -2.0f, 0.0f);
	terrain = D3DXVECTOR3(20.0f, 0.0f, 50.0f);

	terrain_diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

	D3DXMatrixIdentity(&rot);
	camera_up = { 0.0f, 1.0f, 0.0f };

	specular_none  = 0.0f;
	specular_matte = 32.0f;
	specular_shiny = 8.0f;

	fd.Height = 25;
	fd.Width = 0;
	fd.Weight = 0;
	fd.MipLevels = 1;
	fd.Italic = false;
	fd.CharSet = OUT_DEFAULT_PRECIS;
	fd.Quality = DEFAULT_QUALITY;
	fd.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	wcscpy(fd.FaceName, L"Arial");

	fontColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	rectangleFps = { 25, 25, 0, 0 };
	rectangleCpu = { 25, 50, 0, 0 };
	rectangleRenderCount = { 25, 75, 0, 0 };
	rectanglePicking = { 25, 100, 0, 0 };
	rectangleCpu = { 25, 100, 0, 0 };
	rectangleRenderCount = { 25, 175, 0, 0 };

	pickingText = "None";
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

	resolution.x = screenWidth;
	resolution.y = screenHeight;

	// Create the Direct3D object.
	m_D3D		  = new D3DClass;
	m_Camera	  = new CameraClass;
	m_Text		  = new TextClass;
	m_Convert	  = new ConverterClass;
	m_ObjectLight = new LightClass;
	m_Terrain	  = new TerrainClass;
	m_Frustum	  = new FrustumClass;
	m_QuadTree	  = new QuadTreeClass;

	// Initialize the Direct3D object.
	m_D3D	  ->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	m_Terrain ->Initialize(m_D3D->GetDevice(), "../Engine/data/heightmap01.bmp", L"../Engine/data/grass.jpg");
	m_QuadTree->Initialize(m_Terrain, m_D3D->GetDevice(), terrain);
	m_Text	  ->Initialize();

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 1.0f, -5.0f);
	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix();

	//Convert all the models we're using to our format
	m_Convert->Convert(L"../Engine/data/sphereRGB.obj", 1);	//Convert sphere
	m_Convert->Convert(L"../Engine/data/cubeTextured.obj", 2);	//Convert cube

	// Create the model objects.

	m_ObjectLight->SetAmbientColor(0.4f, 0.4f, 0.4f, 1.0f);
	m_ObjectLight->SetPosition(4.0f, 4.0f, 0.0f);
	m_ObjectLight->SetDirection(0.0f, -1.0f, -1.0f);

	m_Cube		 = new ModelClass (cube, m_D3D->GetDevice(), L"../Engine/data/model01.txt", false);
	m_NormalCube = new ModelClass (normalcube, m_D3D->GetDevice(), L"../Engine/data/model02.txt", true);

	for (int i = 0; i < part; i++)
	{
		m_ParticleSystem[i] = new ParticleClass(m_D3D->GetDevice());
	}
	

	m_ObjectLight->SetAmbientColor(0.4f, 0.4f, 0.4f, 1.0f);
	m_ObjectLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_ObjectLight->SetPosition(4.0f, 4.0f, 0.0f);
	m_ObjectLight->SetDirection(0.0f, -1.0f, -4.0f);
	m_ObjectLight->GenerateOrthoMatrix(20.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
	m_ObjectLight->GenerateViewMatrix();

	m_RenderTexture = new RenderTextureClass;
	m_ShadowShader = new ShadowShaderClass;
	m_NormalMapShader = new NormalMapShaderClass;
	m_ParticleShader = new ParticleShaderClass;

	m_Deferred = new DeferredShaderClass;
	m_Window = new OrthoWindowClass;

	m_Deferred = new DeferredShaderClass;
	m_Window = new OrthoWindowClass;

	m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	m_ShadowShader->Initialize(m_D3D->GetDevice(), hwnd);
	m_NormalMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	m_ParticleShader->Initialize(m_D3D->GetDevice(), hwnd);
	m_Window->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	m_Deferred->Initialize(m_D3D->GetDevice(), hwnd);

	D3DX10CreateFontIndirect(m_D3D->GetDevice(), &fd, &font);

	return true;
}


void GraphicsClass::Shutdown()
{
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

	if (m_Cube)
	{
		m_Cube->Shutdown();
		delete m_Cube;
		m_Cube = 0;
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

	if (m_Deferred)
	{
		m_Deferred->Shutdown();
		delete m_Deferred;
		m_Deferred = 0;
	}

	if (m_Window)
	{
		m_Window->Shutdown();
		delete m_Window;
		m_Window = 0;
	}

	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	if (m_QuadTree)
	{
		m_QuadTree->Shutdown();
		delete m_QuadTree;
		m_QuadTree = 0;
	}

	if (m_Terrain)
	{
		delete m_Terrain;
		m_Terrain = 0;
	}

	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	if (m_Terrain)
	{
		delete m_Terrain;
		m_Terrain = 0;
	}

	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	if (m_ParticleShader)
	{
		delete m_ParticleShader;
		m_ParticleShader = 0;
	}

	if (m_ParticleSystem)
	{
		for (int i = 0; i < part; i++)
		{
			delete m_ParticleSystem[i];
			m_ParticleSystem[i] = 0;
		}
	}
	return;
}


bool GraphicsClass::Frame(int fps, int cpu, float frameTime)
{
	static float rotation = 0.0f;
	rotation += (float)D3DX_PI * 0.002f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	t_cpu = cpu;
	t_fps = fps;
	t_renderCount = m_QuadTree->GetDrawCount();
	frametime = frameTime;

	Render(rotation);

	return true;
}

void GraphicsClass::Move(int dir)
{
	bool foundHeight;
	D3DXVECTOR3 position;
	float height;

	cam_pos = m_Camera->GetPosition();
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

	if (dir == 7) //ROTATE UP
	{
		m_Camera->SetRotation(rotate.x + rotatespeed, rotate.y, rotate.z);
	}

	if (dir == 8) //ROTATE UP
	{
		m_Camera->SetRotation(rotate.x - rotatespeed, rotate.y, rotate.z);
	}

	position = m_Camera->GetPosition();

	foundHeight = m_QuadTree->GetHeightAtPosition(position.x - terrain.x, position.z - terrain.z, height);
	if (foundHeight)
	{
		m_Camera->SetPosition(position.x, height + 3.0f, position.z);
	}
}

void GraphicsClass::Launch()
{

}

bool GraphicsClass::RenderSceneToTexture(float rotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	D3DXMATRIX translateMatrix;
	D3DXVECTOR3 xyz, scale;
	D3DXVECTOR3 pos;

	bool render = false;

	D3DXVECTOR3 cameraPosition, modelPosition;
	double angle;
	float rotate;

	m_RenderTexture->SetRenderTarget (m_D3D->GetDevice ());
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	//Cube --->
	pos = m_Cube->GetPosition();
	render = m_Frustum->CheckSphere(pos.x, pos.y, pos.z, 1.0f);
	m_Cube->SetPosition(pos.x, pos.y, pos.z);
	D3DXMatrixTranslation(&worldMatrix, pos.x, pos.y, pos.z);

	if (render == true)
	{
		m_Cube->Render(m_D3D->GetDevice());
		m_Deferred->Render(m_D3D->GetDevice(), m_Cube->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			m_Cube->GetTexture(),
			m_Cube->GetDiffuse());
	}

	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	//Cube --->
	pos = m_Cube->GetPosition();

	if (m_Frustum->CheckSphere(pos.x, pos.y, pos.z, 0.5f) == true)
	{
	m_Cube->SetPosition(pos.x, pos.y, pos.z);
	D3DXMatrixTranslation(&worldMatrix, pos.x, pos.y, pos.z);

	m_Cube->Render(m_D3D->GetDevice());
	m_Deferred->Render(m_D3D->GetDevice(), m_Cube->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix,
		m_Cube->GetTexture(),
		m_Cube->GetDiffuse());
	}
	//Cube --->

	//Terrain --->
	m_D3D->GetWorldMatrix(worldMatrix);
	D3DXMatrixTranslation(&translateMatrix, terrain.x, terrain.y, terrain.z);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translateMatrix);

	m_Deferred->SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix,
		m_Terrain->GetTexture(), terrain_diffuse);

	m_QuadTree->Render(m_Frustum, m_D3D->GetDevice(), m_Deferred);
	//Terrain --->

	m_D3D->SetBackBufferRenderTarget();
	m_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderText()
{
	LPCSTR fpsText;
	LPCSTR cpuText;
	LPCSTR renderCountText;

	string convertedFpsText;
	string convertedCpuText;
	string convertedRenderCountText;


	convertedFpsText = to_string(t_fps);
	fpsText = convertedFpsText.c_str();

	font->DrawTextA(0, fpsText, -1, &rectangleFps, DT_NOCLIP, fontColor);

	convertedCpuText = to_string(t_cpu);
	cpuText = convertedCpuText.c_str();

	font->DrawTextA(0, cpuText, -1, &rectangleCpu, DT_NOCLIP, fontColor);


	convertedRenderCountText = to_string(t_renderCount);
	renderCountText = convertedRenderCountText.c_str();

	font->DrawTextA(0, renderCountText, -1, &rectangleRenderCount, DT_NOCLIP, fontColor);
	font->DrawTextA(0, pickingText, -1, &rectanglePicking, DT_NOCLIP, fontColor);

	return true;
}

void GraphicsClass::TestIntersection(int x, int y)
{
	float pointX, pointY;
	D3DXMATRIX projectionMatrix, viewMatrix, inverseViewMatrix, worldMatrix, translateMatrix, inverseWorldMatrix;
	D3DXVECTOR3 direction, origin, rayOrigin, rayDirection;
	bool intersect;

	pointX = ((2.0f*(float)x) / (float)resolution.x) - 1.0f;
	pointY = (((2.0f*(float)y) / (float)resolution.y) - 1.0f)* -1.0f;
	m_D3D->GetProjectionMatrix(projectionMatrix);
	pointX = pointX / projectionMatrix._11;
	pointY = pointY / projectionMatrix._22;

	m_Camera->GetViewMatrix(viewMatrix);
	D3DXMatrixInverse(&inverseViewMatrix, NULL, &viewMatrix);

	direction.x = (pointX*inverseViewMatrix._11) + (pointY*inverseViewMatrix._21) + inverseViewMatrix._31;
	direction.y = (pointX*inverseViewMatrix._12) + (pointY*inverseViewMatrix._22) + inverseViewMatrix._32;
	direction.z = (pointX*inverseViewMatrix._13) + (pointY*inverseViewMatrix._23) + inverseViewMatrix._33;

	origin = m_Camera->GetPosition();

	m_D3D->GetWorldMatrix(worldMatrix);
	D3DXMatrixTranslation(&translateMatrix, cube.x, cube.y, cube.z);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translateMatrix);

	D3DXMatrixInverse(&inverseWorldMatrix, NULL, &worldMatrix);

	D3DXVec3TransformCoord(&rayOrigin, &origin, &inverseWorldMatrix);
	D3DXVec3TransformNormal(&rayDirection, &direction, &inverseWorldMatrix);

	D3DXVec3Normalize(&rayDirection, &rayDirection);

	intersect = RaySphereIntersect(rayOrigin, rayDirection, 1.0f);
	
	if (intersect == true)
	{
		pickingText = "Hit";
	}

	else
	{
		pickingText = "Miss";
	}
	return;
}

bool GraphicsClass::RaySphereIntersect(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDirection, float radius)
{
	float a, b, c, discriminant;

	a = (rayDirection.x * rayDirection.x) + (rayDirection.y * rayDirection.y) + (rayDirection.z * rayDirection.z);
	b = ((rayDirection.x* rayOrigin.x) + (rayDirection.y * rayOrigin.y) + (rayDirection.z * rayOrigin.z)) *2.0f;
	c = ((rayOrigin.x * rayOrigin.x) + (rayOrigin.y * rayOrigin.y) + (rayOrigin.z * rayOrigin.z)) - (radius * radius);

	discriminant = (b * b) - (4 * a * c);
	
	if (discriminant < 0.0f)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	D3DXMATRIX scaleMatrix, rotationMatrix;
	D3DXMATRIX translationMatrix;

	D3DXVECTOR3 pos, rotate;

	D3DXMATRIX viewMatrix, projectionMatrix;
	D3DXMATRIX scaleMatrix, rotationMatrix;
	D3DXMATRIX translationMatrix;

	D3DXVECTOR3 rotate;

	D3DXVECTOR3 xyz, scale;
	D3DXVECTOR3 pos, pos2;

	D3DXVECTOR3 cameraPosition, modelPosition;
	double angle;
	float rotates;

	bool rendermodel = false;

	RenderSceneToTexture(rotation);

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	m_Camera->Render();


	// Render fullscreen quad
	// ----------\/----------
	m_D3D->TurnZBufferOff();

	m_Window->Render(m_D3D->GetDevice());
	m_ShadowShader->Render(m_D3D->GetDevice(), m_Window->GetIndexCount(),
		worldMatrix, baseViewMatrix, orthoMatrix,
		m_RenderTexture->GetShaderResourceView(0),
		m_RenderTexture->GetShaderResourceView(1),
		m_RenderTexture->GetShaderResourceView(2),
		m_ObjectLight->GetDirection(),
		m_ObjectLight->GetAmbientColor(),
		m_Camera->GetPosition(),
		specular_shiny);
	
	m_D3D->TurnZBufferOn();
	// ----------/\----------
	// Render fullscreen quad




	cameraPosition = m_Camera->GetPosition();
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	for (int i = 0; i < part; i++)
	{
		m_ParticleSystem[i]->Render(m_D3D->GetDevice());

		pos = m_ParticleSystem[i]->GetPosition();
		D3DXMatrixTranslation(&worldMatrix, pos.x, pos.y, pos.z);

		m_ParticleShader->Render(m_D3D->GetDevice(), 1,
			worldMatrix, viewMatrix, projectionMatrix,
			cameraPosition,
			m_ParticleSystem[i]->GetTexture());
	}


	m_D3D->GetWorldMatrix(worldMatrix);

	// Render fullscreen quad
	// ----------\/----------
	m_D3D->TurnZBufferOff();

	m_Window->Render(m_D3D->GetDevice());
	m_ShadowShader->Render(m_D3D->GetDevice(), m_Window->GetIndexCount(),
		worldMatrix, baseViewMatrix, orthoMatrix,
		m_RenderTexture->GetShaderResourceView(0),
		m_RenderTexture->GetShaderResourceView(1),
		m_RenderTexture->GetShaderResourceView(2),
		m_ObjectLight->GetDirection(),
		m_ObjectLight->GetAmbientColor(),
		m_Camera->GetPosition(),
		specular_shiny);

	m_D3D->TurnZBufferOn ();
	// ----------/\----------
	// Render fullscreen quad

	pos = m_NormalCube->GetPosition ();
	m_D3D->GetWorldMatrix (worldMatrix);
	m_Camera->GetViewMatrix (viewMatrix);
	m_D3D->GetProjectionMatrix (projectionMatrix);

	//rotation
	D3DXMatrixRotationY (&rotationMatrix, rotation);
	D3DXMatrixTranslation (&translationMatrix, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply (&worldMatrix, &rotationMatrix, &translationMatrix);

	m_NormalCube->Render (m_D3D->GetDevice ());
	m_NormalMapShader->Render (m_D3D->GetDevice (), m_NormalCube->GetIndexCount (),
		worldMatrix, viewMatrix, projectionMatrix,
		m_NormalCube->GetTexture (), m_NormalCube->GetNormalmap (),
		m_ObjectLight->GetDirection (), m_ObjectLight->GetAmbientColor (), m_ObjectLight->GetDiffuseColor ());


	m_D3D->TurnZBufferOff();
	RenderText();
	m_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}