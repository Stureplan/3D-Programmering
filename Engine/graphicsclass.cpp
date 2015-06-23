////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Models = 0;
	m_LightShader = 0;
	m_Light = 0;
	m_Terrain = 0;

	movespeed = 0.03f;
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
	

	// Create the terrain object
	m_Terrain = new TerrainClass;
	if (!m_Terrain)
	{
		return false;
	}

	// Initialize terrain object
	result = m_Terrain->Initialize(m_D3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize terrain object", L"Error", MB_OK);
		return false;
	}

	// Create the model object.
	m_Models = new ModelClass;
	if(!m_Models)
	{
		return false;
	}

	m_Convert = new ConverterClass;
	if (!m_Convert)
	{
		return false;
	}

	
	m_Convert->Convert(1);	//Convert model01
	m_Convert->Convert(2);	//Convert model02
	objCount = 2;

	// Initialize the model object.
	result = m_Models->Initialize(m_D3D->GetDevice(), 
		"../Engine/data/model01.txt",
		L"../Engine/data/dog.jpg");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	result = m_Models->InitializeObjects(objCount);
	if (!result)
	{
		MessageBox (hwnd, L"Could not initialize objects.", L"Error", MB_OK);
		return false;
	}

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
	m_Light->SetDirection(1.0f, 0.0f, 0.0f);

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

	// Release the terrain object
	if (m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	// Release the model object.
	if(m_Models)
	{
		m_Models->Shutdown();
		delete m_Models;
		m_Models = 0;
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
	D3DXVECTOR3 pos;

	if (dir == 1)	//MOVE FWD
	{
		pos = m_Camera->GetPosition ();
		m_Camera->SetPosition (pos.x, pos.y, pos.z + movespeed);

		pos = m_Models->GetPosition (1);
		m_Models->SetPosition (1, pos.x, pos.y, pos.z + movespeed);
	}

	if (dir == 2)	//MOVE LEFT
	{
		pos = m_Camera->GetPosition ();
		m_Camera->SetPosition (pos.x - movespeed, pos.y, pos.z);

		pos = m_Models->GetPosition (1);
		m_Models->SetPosition (1, pos.x - movespeed, pos.y, pos.z);
	}

	if (dir == 3)	//MOVE BACK
	{
		pos = m_Camera->GetPosition ();
		m_Camera->SetPosition (pos.x, pos.y, pos.z - movespeed);

		pos = m_Models->GetPosition (1);
		m_Models->SetPosition (1, pos.x, pos.y, pos.z - movespeed);
	}

	if (dir == 4)	//MOVE RIGHT
	{
		pos = m_Camera->GetPosition ();
		m_Camera->SetPosition (pos.x + movespeed, pos.y, pos.z);

		pos = m_Models->GetPosition (1);
		m_Models->SetPosition (1, pos.x + movespeed, pos.y, pos.z);
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

	// Render the terrain
	m_Terrain->Render(m_D3D->GetDevice());

	// Render the model using the light shader
	m_LightShader->Render(m_D3D->GetDevice(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	

	for (int i = 0; i < objCount; i++)
	{
		D3DXVECTOR3 pos = m_Models->GetPosition (i+1);
		D3DXMatrixTranslation (&worldMatrix, pos.x, pos.y, pos.z);
		m_Models->Render (m_D3D->GetDevice ());
		
		m_LightShader->Render (m_D3D->GetDevice(), m_Models->GetIndexCount(), 
			worldMatrix, viewMatrix, projectionMatrix, m_Models->GetTexture(), 
			m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());

		m_D3D->GetWorldMatrix (worldMatrix);
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}