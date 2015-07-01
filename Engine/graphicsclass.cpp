////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#define WM_MOUSEMOVE


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Gun = 0;
	m_Cube = 0;
	m_LightShader = 0;
	m_Light = 0;

	movespeed = 0.03f;
	//These are the POSITIONS for each object in the scene
	gun  = D3DXVECTOR3 (0.5f, -0.5f, -3.5f);
	cube = D3DXVECTOR3 (0.0f, 0.5f, 0.0f);
	def  = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
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

	m_Convert->Convert (1);	//Convert model01
	m_Convert->Convert (2);	//Convert model02

	// Create the model objects.
	m_Gun  = new ModelClass (gun, m_D3D->GetDevice (),  "../Engine/data/model01.txt", L"../Engine/data/dog.jpg");
	m_Cube = new ModelClass (cube, m_D3D->GetDevice (), "../Engine/data/model02.txt", L"../Engine/data/dog.jpg");

	//Create the light shader object
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	//Initialize the texture shader object
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	//Create the new light object.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	//Initialize the light object.
	m_Light->SetAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_Light->SetDirection   (1.0f, 0.0f, 0.0f);

	return true;
}


void GraphicsClass::Shutdown()
{
	//Release the light object
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the texture shader object.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the gun object.
	if(m_Gun)
	{
		m_Gun->Shutdown();
		delete m_Gun;
		m_Gun = 0;
	}

	// Release the cube object.
	if (m_Cube)
	{
		m_Cube->Shutdown ();
		delete m_Cube;
		m_Cube = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
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

	rotation += (float)D3DX_PI * 0.01f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Render the graphics scene.
	result = Render(rotation);
	if(!result)
	{
		return false;
	}

	return true;
}

void GraphicsClass::Move (int dir)
{

	if (dir == 1)	//MOVE FWD
	{
		pos = m_Camera->GetPosition ();
		m_Camera->SetPosition (pos.x, pos.y, pos.z + movespeed);

		pos = m_Gun->GetPosition ();
		m_Gun->SetPosition	  (pos.x, pos.y, pos.z + movespeed);
	}

	if (dir == 2)	//MOVE LEFT
	{
		pos = m_Camera->GetPosition ();
		m_Camera->SetPosition (pos.x - movespeed, pos.y, pos.z);

		pos = m_Gun->GetPosition ();
		m_Gun->SetPosition	  (pos.x - movespeed, pos.y, pos.z);
	}

	if (dir == 3)	//MOVE BACK
	{
		pos = m_Camera->GetPosition ();
		m_Camera->SetPosition (pos.x, pos.y, pos.z - movespeed);

		pos = m_Gun->GetPosition ();
		m_Gun->SetPosition	  (pos.x, pos.y, pos.z - movespeed);
	}

	if (dir == 4)	//MOVE RIGHT
	{
		pos = m_Camera->GetPosition ();
		m_Camera->SetPosition (pos.x + movespeed, pos.y, pos.z);

		pos = m_Gun->GetPosition ();
		m_Gun->SetPosition	  (pos.x + movespeed, pos.y, pos.z);
	}
}

void GraphicsClass::Launch ()
{

}


bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	D3DXMATRIX scaleMatrix, rotationMatrix;
	D3DXMATRIX translationMatrix;

	D3DXVECTOR3 pos;

	int index;
	float pos_x, pos_y, pos_z;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera ->GetViewMatrix(viewMatrix);
	m_D3D	 ->GetWorldMatrix(worldMatrix);
	m_D3D	 ->GetProjectionMatrix(projectionMatrix);


	//-->OBJECT HANDLING-->

	//1. Gun
	//-------------------------------------------------------------------------//
	pos = m_Gun->GetPosition ();
	D3DXMatrixTranslation (&worldMatrix, pos.x, pos.y, pos.z);
	m_Gun->Render (m_D3D->GetDevice ());
		
	m_LightShader->Render (m_D3D->GetDevice(), m_Gun->GetIndexCount (),
							worldMatrix, viewMatrix, projectionMatrix, 
							m_Gun->GetTexture (), m_Light->GetDirection (),
							m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());

	m_D3D->GetWorldMatrix (worldMatrix);
	//-------------------------------------------------------------------------//

	//2. Cube
	//-------------------------------------------------------------------------//
	pos = m_Cube->GetPosition ();
	D3DXMatrixTranslation (&worldMatrix, pos.x, pos.y, pos.z);
	m_Cube->Render (m_D3D->GetDevice ());

	m_LightShader->Render (m_D3D->GetDevice (), m_Cube->GetIndexCount (),
						   worldMatrix, viewMatrix, projectionMatrix,
						   m_Cube->GetTexture (), m_Light->GetDirection (),
						   m_Light->GetAmbientColor (), m_Light->GetDiffuseColor ());

	m_D3D->GetWorldMatrix (worldMatrix);
	//-------------------------------------------------------------------------//

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}