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
	m_Light->SetDiffuseColor(1.0f, 0.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 1.0f, 1.0f);

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


bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	D3DXMATRIX scaleMatrix, rotationMatrix;
	D3DXMATRIX translationMatrix;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	D3DXMatrixScaling(&scaleMatrix, 1.0f, 1.0f, 1.0f);
	D3DXMatrixRotationY(&rotationMatrix, rotation);
	D3DXMatrixTranslation(&translationMatrix, 0.0f, 0.0f, 0.0f);
	D3DXMatrixMultiply(&worldMatrix, &scaleMatrix, &rotationMatrix);

	//worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_D3D->GetDevice());

	// Render the model using the texture shader.
	m_LightShader->Render(m_D3D->GetDevice(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, 
	projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}