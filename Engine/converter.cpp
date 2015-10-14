//FILENAME: converter.cpp
#include "converter.h"

ConverterClass::ConverterClass()
{

}

ConverterClass::ConverterClass(const ConverterClass& other)
{

}

ConverterClass::~ConverterClass()
{

}

int ConverterClass::Convert(int type)
{
	bool result;
	char filename[27];

	int vertexCount, textureCount, normalCount, faceCount;
	char garbage;

	//Read the name of the model file
	GetModelFilename(filename, type);


	//Read the number of vvertices, texture coords, normals and faces
	//so that the data structures can be initialized with the exact sizes needed.
	result = ReadFileCounts(filename, vertexCount, textureCount, 
							normalCount, faceCount);
	if (!result)
	{
		return -1;
	}

	//Now read the data from the file into the data structures and then
	//output it in our model format, (x, y, z, U, V, nx, ny, nz)
	result = LoadDataStructures(filename, type, vertexCount, textureCount,
											normalCount, faceCount);
	if (!result)
	{
		return -1;
	}

	return 0;
}

void ConverterClass::GetModelFilename(char* filename, int type)
{
	ifstream fin;
	char* localFilename;

	switch (type)
	{
	case 1:			//If the int type was = 1 (gun)
		localFilename = "../Engine/data/model01.obj";
		break;
	case 2:			//If the int type was = 2 (cube)
		localFilename = "../Engine/data/model02.obj";
		break;
	case 3:			//If the int type was = 3 (ground plane)
		localFilename = "../Engine/data/model03.obj";
	default:
		break;
	}

	for (int i = 0; i < 27; i++)
	{
		filename[i] = localFilename[i];
	}
	return;
}

bool ConverterClass::ReadFileCounts(char* filename, int& vertexCount,
	int& textureCount, int& normalCount, int& faceCount)
{
	ifstream fin;
	char input;

	//Initialize the counts.
	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	//Open the file
	fin.open(filename);

	//Check if opening the file was successful
	if (fin.fail() == true)
	{
		return false;
	}

	//Read from the file and continue to read
	//until the end of the file is reached.
	fin.get(input);
	while (!fin.eof())
	{
		//If the line starts with v then count either the vertex,
		//the texture coordinates or the normal vector.
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') 
			{ 
				vertexCount++; 
			}
			if (input == 't')
			{
				textureCount++;
			}
			if (input == 'n')
			{
				normalCount++;
			}
		}

		//If the line starts with f then increment the face count.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' '){ faceCount++; }
		}

		//Otherwise read in the remainder of the file.
		while (input != '\n')
		{
			fin.get(input);
		}

		//Start readin the beginning of the next line.
		fin.get(input);
	}

	//Close the file.
	fin.close();

	return true;
}

bool ConverterClass::LoadDataStructures(char* filename, int type, int vertexCount,
										int textureCount, int normalCount, int faceCount)
{
	VertexType *vertices, *texcoords, *normals;
	FaceType *faces;
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, 
		faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;

	//Initialize the four data structures
	vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	texcoords = new VertexType[textureCount];
	if (!texcoords)
	{
		return false;
	}

	normals = new VertexType[normalCount];
	if (!normals)
	{
		return false;
	}

	faces = new FaceType[faceCount];
	if (!faces)
	{
		return false;
	}

	//Initialize the indexes
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	//Open the file.
	fin.open(filename);

	//Check if the file opening was successful
	if (fin.fail() == true)
	{
		return false;
	}

	//Read in the vertices, texture coords and normals into the data structures
	//IMPORTANT: Also convert to left hand coord system since Maya uses 
	//right hand coord system. (Left to right vs right to left)
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			//Read in the verts.
			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				//Invert the z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			//Read in the texture uv coords.
			if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				//Invert the v texture coords to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			//Read in the normals.
			if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				//Invert the z normal to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}
		}

		//Read in the faces
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				//Read in the face data backwards to convert it to left hand system
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		//Read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}

		//Start reading the beginning of the next line.
		fin.get(input);
	}

	//Close the file.
	fin.close();

	//Open the output file.
	switch (type)
	{
	case 1:			//If the int type was = 1 (gun)
		fout.open ("../Engine/data/model01.txt");
		break;
	case 2:			//If the int type was = 2 (cube)
		fout.open ("../Engine/data/model02.txt");
		break;
	case 3:			//If the int type was = 3 (ground plane)
		fout.open ("../Engine/data/model03.txt");
		break;

	default:
		break;
	}

	//Write out the file header that our model format uses.
	fout << "Vertex Count: " << (faceCount * 3) << endl;
	fout << endl;
	fout << "Data:" << endl;
	fout << endl;

	//Now loop through all the faces and output the three vertices for each face.
	for (int i = 0; i < faceIndex; i++)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;
	}

	//Close the output file.
	fout.close();

	//Release the four data structures.
	if (vertices)
	{
		delete[] vertices;
		vertices = 0;
	}

	if (texcoords)
	{
		delete[] texcoords;
		texcoords = 0;
	}

	if (normals)
	{
		delete[] normals;
		normals = 0;
	}

	if (faces)
	{
		delete[] faces;
		faces = 0;
	}

	return true;
}