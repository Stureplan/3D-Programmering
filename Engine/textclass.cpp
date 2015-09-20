//////////////
// Filename : textclass.cpp
//////////////
#include "textclass.h"

TextClass::TextClass()
{
	m_Font = 0;
	m_sentence1 = 0;
	m_sentence2 = 0;
	m_sentence3 = 0;
	m_sentence4 = 0;
	m_sentence5 = 0;
	m_sentence6 = 0;
	m_sentence7 = 0;
	m_sentence8 = 0;
	m_sentence9 = 0;
	m_sentence10 = 0;
	m_sentence11 = 0;

	//m_Font = 0;
	//m_FontShader = 0;

	//m_sentence1 = 0;
	//m_sentence2 = 0;
}

TextClass::TextClass(const TextClass& other)
{

}

TextClass::~TextClass()
{

}

bool TextClass::Initialize(ID3D10Device* device, HWND hwnd, int screenWidth, int screenHeight, D3DXMATRIX baseViewMatrix)
{
	bool result;

	// Store the screen width and height.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the base view matrix.
	m_baseViewMatrix = baseViewMatrix;

	// Create the font object.
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// Initialize the font object.
	result = m_Font->Initialize(device, "../Enginge/data/fontdata.txt", L"../Engine/data/font.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object", L"Error", MB_OK);
		return false;
	}

	//// Create the font shader object.
	//m_FontShader = new FontShaderClass;
	//if (!m_FontShader)
	//{
	//	return false;
	//}

	//// Initialize the font shader object.
	//result = m_FontShader->Initialize(device, hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
	//	return false;
	//}

	//// Now update the sentence vertex buffer with the new string information.
	//result = UpdateSentence(m_sentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f);
	//if (!result)
	//{
	//	return false;
	//}

	//// Initialize the first sentence.
	//result = InitializeSentence(&m_sentence2, 16, device);
	//if (!result)
	//{
	//	return false;
	//}

	//// Now update the sentence vertex buffer with the new string information.
	//result = UpdateSentence(m_sentence2, "Goodbye", 100, 200, 1.0f, 1.0f, 0.0f);
	//if (!result)
	//{
	//	return false;
	//}

	//return true;

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence1, 150, device);
	if (!result)
	{
		return false;
	}

	// Initialize the second sentence.
	result = InitializeSentence(&m_sentence2, 32, device);
	if (!result)
	{
		return false;
	}

	// Initialize the third sentence.
	result = InitializeSentence(&m_sentence3, 16, device);
	if (!result)
	{
		return false;
	}

	// Initialize the fourth sentence.
	result = InitializeSentence(&m_sentence4, 16, device);
	if (!result)
	{
		return false;
	}

	// Initialize the fifth sentence.
	result = InitializeSentence(&m_sentence5, 16, device);
	if (!result)
	{
		return false;
	}

	// Initialize the sixth sentence.
	result = InitializeSentence(&m_sentence6, 16, device);
	if (!result)
	{
		return false;
	}

	// Initialize the seventh sentence.
	result = InitializeSentence(&m_sentence7, 16, device);
	if (!result)
	{
		return false;
	}

	// Initialize the eighth sentence.
	result = InitializeSentence(&m_sentence8, 16, device);
	if (!result)
	{
		return false;
	}

	// Initialize the ninth sentence.
	result = InitializeSentence(&m_sentence9, 16, device);
	if (!result)
	{
		return false;
	}

	// Initialize the tenth sentence.
	result = InitializeSentence(&m_sentence10, 16, device);
	if (!result)
	{
		return false;
	}

	// Initialize the eleventh sentence.
	result = InitializeSentence(&m_sentence11, 32, device);
	if (!result)
	{
		return false;
	}
	
	return true;
}

void TextClass::Shutdown()
{
	//// Release the first sentence.
	//ReleaseSentence(&m_sentence1);

	//// Release the second sentence.
	//ReleaseSentence(&m_sentence2);

	//// Release the font shader object.
	//if (m_FontShader)
	//{
	//	m_FontShader->Shutdown();
	//	delete m_FontShader;
	//	m_FontShader = 0;
	//}

	// Release the font object.
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	// Release the sentences.
	ReleaseSentence(&m_sentence1);
	ReleaseSentence(&m_sentence2);
	ReleaseSentence(&m_sentence3);
	ReleaseSentence(&m_sentence4);
	ReleaseSentence(&m_sentence5);
	ReleaseSentence(&m_sentence6);
	ReleaseSentence(&m_sentence7);
	ReleaseSentence(&m_sentence8);
	ReleaseSentence(&m_sentence9);
	ReleaseSentence(&m_sentence10);
	ReleaseSentence(&m_sentence11);

	return;
}

bool TextClass::Render(ID3D10Device* device, FontShaderClass* FontShader, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	bool result;


	// Draw the sentences.
	result = RenderSentence(m_sentence1, device, FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence2, device, FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence3, device, FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence4, device, FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence5, device, FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence6, device, FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence7, device, FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence8, device, FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence9, device, FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence10, device, FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence11, device, FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D10Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create a new sentence object.
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// Initialize the sentence buffers to null.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(*sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(*sentence)->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// Initialize the index array.
	for (i = 0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}

bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, 
								float red, float green, float blue, ID3D10Device* device)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	VertexType* verticesPtr;
	HRESULT result;
	D3D10_SUBRESOURCE_DATA mappedResource;

	// Store the color of the sentence.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text);

	// Check for possible buffer overflow.
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	//// Initialize the vertex buffer pointer to null first.
	//verticesPtr = 0;

	// Lock the vertex buffer.
	result = sentence->vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&verticesPtr);
	if (FAILED(result))
	{
		return false;
	}

	///////////// CHECK

	// Get a pointer to hte data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pSysMem;

	// Copy the vertex array into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	sentence->vertexBuffer->Unmap();

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// Release the sentence vertex buffer.
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// Release the sentence.
		delete *sentence;
		*sentence = 0;
	}

	return;
}

bool TextClass::RenderSentence(SentenceType* sentence, ID3D10Device* device, FontShaderClass* FontShader,
	D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	D3DXVECTOR4 pixelColor;
	bool result;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	device->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	device->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	FontShader->Render(device, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix,
		m_Font->GetTexture(), pixelColor);
	
	return true;
}

bool TextClass::SetVideoCardInfo(char* videoCardName, int videoCardMemory, ID3D10Device* device)
{
	char dataString[150];
	bool result;
	char tempString[16];
	char memoryString[32];

	// Set up the video card name string.
	strcpy_s(dataString, "Video Card: ");
	strcat_s(dataString, videoCardName);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence1, dataString, 10, 10, 1.0f, 1.0f, 1.0f, device);
	if (!result)
	{
		return false;
	}

	// Truncate the memory value to prevent vuffer over flow.
	if (videoCardMemory > 9999999)
	{
		videoCardMemory = 9999999;
	}

	// Convert the video memory integer value to a string format.
	_itoa_s(videoCardMemory, tempString, 10);

	// Setup the video memory string.
	strcpy_s(memoryString, "Video Memeory: ");
	strcat_s(memoryString, tempString);
	strcat_s(memoryString, " MB");

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence2, memoryString, 10, 30, 1.0f, 1.0f, 1.0f, device);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D10Device* device)
{
	char tempString[16];
	char fpsString[16];
	bool result;

	// Truncate the fps to below 10 000.
	if (fps > 9999)
	{
		fps = 9999;
	}

	// Convert the fps integer to string format.
	_itoa_s(fps, tempString, 10);

	// Setup the fps string.
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence1, fpsString, 10, 70, 1.0f, 1.0f, 1.0f, device);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetCpu(int cpu, ID3D10Device* device)
{
	char tempString[16];
	char cpuString[16];
	bool result;

	// Convert the cpu integer to string format.
	_itoa_s(cpu, tempString, 10);

	// Set up the cpu string.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence2, cpuString, 10, 90, 0.0f, 1.0f, 0.0f, device);
	if (!result)
	{
		return false;
	}

	return true;
}


bool TextClass::SetCameraPosition(float posX, float posY, float posZ, ID3D10Device* device)
{
	int positionX, positionY, positionZ;
	char tempString[16];
	char dataString[16];
	bool result;


	// Convert the position from floating point to integer.
	positionX = (int)posX;
	positionY = (int)posY;
	positionZ = (int)posZ;

	// Truncate the position if it exceeds either 9999 or -9999.
	if (positionX > 9999) { positionX = 9999; }
	if (positionY > 9999) { positionY = 9999; }
	if (positionZ > 9999) { positionZ = 9999; }

	if (positionX < -9999) { positionX = -9999; }
	if (positionY < -9999) { positionY = -9999; }
	if (positionZ < -9999) { positionZ = -9999; }

	// Setup the X position string.
	_itoa_s(positionX, tempString, 10);
	strcpy_s(dataString, "X: ");
	strcat_s(dataString, tempString);

	result = UpdateSentence(m_sentence5, dataString, 10, 130, 0.0f, 1.0f, 0.0f, device);
	if (!result)
	{
		return false;
	}

	// Setup the Y position string.
	_itoa_s(positionY, tempString, 10);
	strcpy_s(dataString, "Y: ");
	strcat_s(dataString, tempString);

	result = UpdateSentence(m_sentence6, dataString, 10, 150, 0.0f, 1.0f, 0.0f, device);
	if (!result)
	{
		return false;
	}

	// Setup the Z position string.
	_itoa_s(positionZ, tempString, 10);
	strcpy_s(dataString, "Z: ");
	strcat_s(dataString, tempString);

	result = UpdateSentence(m_sentence7, dataString, 10, 170, 0.0f, 1.0f, 0.0f, device);
	if (!result)
	{
		return false;
	}

	return true;
}


bool TextClass::SetCameraRotation(float rotX, float rotY, float rotZ, ID3D10Device* device)
{
	int rotationX, rotationY, rotationZ;
	char tempString[16];
	char dataString[16];
	bool result;


	// Convert the rotation from floating point to integer.
	rotationX = (int)rotX;
	rotationY = (int)rotY;
	rotationZ = (int)rotZ;

	// Setup the X rotation string.
	_itoa_s(rotationX, tempString, 10);
	strcpy_s(dataString, "rX: ");
	strcat_s(dataString, tempString);

	result = UpdateSentence(m_sentence8, dataString, 10, 210, 0.0f, 1.0f, 0.0f, device);
	if (!result)
	{
		return false;
	}

	// Setup the Y rotation string.
	_itoa_s(rotationY, tempString, 10);
	strcpy_s(dataString, "rY: ");
	strcat_s(dataString, tempString);

	result = UpdateSentence(m_sentence9, dataString, 10, 230, 0.0f, 1.0f, 0.0f, device);
	if (!result)
	{
		return false;
	}

	// Setup the Z rotation string.
	_itoa_s(rotationZ, tempString, 10);
	strcpy_s(dataString, "rZ: ");
	strcat_s(dataString, tempString);

	result = UpdateSentence(m_sentence10, dataString, 10, 250, 0.0f, 1.0f, 0.0f, device);
	if (!result)
	{
		return false;
	}

	return true;
}


bool TextClass::SetRenderCount(int count, ID3D10Device* device)
{
	char tempString[16];
	char renderString[32];
	bool result;


	// Truncate the render count if it gets to large to prevent a buffer overflow.
	if (count > 999999999)
	{
		count = 999999999;
	}

	// Convert the cpu integer to string format.
	_itoa_s(count, tempString, 10);

	// Setup the cpu string.
	strcpy_s(renderString, "Render Count: ");
	strcat_s(renderString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence11, renderString, 10, 290, 0.0f, 1.0f, 0.0f, device);
	if (!result)
	{
		return false;
	}

	return true;
}