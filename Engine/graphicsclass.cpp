////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
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
	
	// Create the model object.
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	//FunctionGo <--- EXECUTE EXPORTER FUNCTION HERE
	m_Convert = new ConverterClass;
	if (!m_Convert)
	{
		return false;
	}
	m_Convert->Convert();

	// Initialize the model object.
	result = m_Model->Initialize(m_D3D->GetDevice(), 
		"../Engine/data/dogbox.txt", 
		L"../Engine/data/dog.jpg");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	result = m_Model->InitializeObjects();
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

	// Release the model object.
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
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
	result = Render(rotation);		//Calls the GraphicsClass::Render function every frame.
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
		m_Camera->SetPosition
	   (m_Camera->GetPosition ().x,
		m_Camera->GetPosition ().y,
		m_Camera->GetPosition ().z + 0.02f);

		m_Model->SetGunPosition
	   (m_Model->GetGunPosition ().x,
		m_Model->GetGunPosition ().y,
		m_Model->GetGunPosition ().z + 0.02f);
	}

	if (dir == 2)	//MOVE LEFT
	{
		m_Camera->SetPosition
	   (m_Camera->GetPosition ().x - 0.02f,
		m_Camera->GetPosition ().y,
		m_Camera->GetPosition ().z);

		m_Model->SetGunPosition
	   (m_Model->GetGunPosition ().x - 0.02f,
		m_Model->GetGunPosition ().y,
		m_Model->GetGunPosition ().z);
	}

	if (dir == 3)	//MOVE BACK
	{
		m_Camera->SetPosition
	   (m_Camera->GetPosition ().x,
		m_Camera->GetPosition ().y,
		m_Camera->GetPosition ().z - 0.02f);

		m_Model->SetGunPosition
	   (m_Model->GetGunPosition ().x,
		m_Model->GetGunPosition ().y,
		m_Model->GetGunPosition ().z - 0.02f);
	}

	if (dir == 4)	//MOVE RIGHT
	{
		m_Camera->SetPosition
	   (m_Camera->GetPosition ().x + 0.02f,
	    m_Camera->GetPosition ().y,
		m_Camera->GetPosition ().z);

		m_Model->SetGunPosition
	   (m_Model->GetGunPosition ().x + 0.02f,
		m_Model->GetGunPosition ().y,
		m_Model->GetGunPosition ().z);
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

	int objCount, index;
	float pos_x, pos_y, pos_z;

	

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera ->GetViewMatrix(viewMatrix);
	m_D3D	 ->GetWorldMatrix(worldMatrix);
	m_D3D	 ->GetProjectionMatrix(projectionMatrix);

	//--OLD ROTATION TESTS, OBSOLETE BUT WILL CONTINUE TESTING--//
	/*D3DXMatrixScaling(&scaleMatrix, 1.0f, 1.0f, 1.0f);
	D3DXMatrixRotationY(&rotationMatrix, rotation);
	D3DXMatrixTranslation(&translationMatrix, 0.0f, 0.0f, 0.0f);
	D3DXMatrixMultiply(&worldMatrix, &scaleMatrix, &rotationMatrix);*/

	objCount = m_Model->GetObjectCount();
	for (int i = 0; i < objCount; i++)
	{
		m_Model->GetObjectData(i, pos_x, pos_y, pos_z);
		D3DXMatrixTranslation (&worldMatrix, pos_x, pos_y, pos_z);
		m_Model->Render (m_D3D->GetDevice ());
		
		m_LightShader->Render (m_D3D->GetDevice(), m_Model->GetIndexCount(), 
			worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), 
			m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());

		m_D3D->GetWorldMatrix (worldMatrix);
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}