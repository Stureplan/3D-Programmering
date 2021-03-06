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

int ConverterClass::Convert(WCHAR* filename, int type)
{
	bool result;

	int vertexCount, textureCount, normalCount, faceCount;

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

bool ConverterClass::ReadFileCounts(WCHAR* filename, int& vertexCount,
	int& textureCount, int& normalCount, int& faceCount)
{
	ifstream fin;
	char input;
	char mtlfile[50];
	int mtlchars = 0;

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
		//Check for mtllib and then write the mtl filename to mtlfile[]
		//We are assuming the following: 
		//1. The .OBJ file has an .MTL file associated with it (written in the .OBJ)
		//2. The .MTL file is not corrupt or missing
		//3. In the .MTL we assume that either a texture is specified or a diffuse

		if (input == 'm')
		{
			fin.get (input);
			if (input == 't')
			{
				fin.get (input);
				if (input == 'l')
				{
					fin.get (input);
					if (input == 'l')
					{
						fin.get (input); //i
						fin.get (input); //b
						fin.get (input); //(whitespace)
						fin.get (input); //first letter
						while (input != '\n')
						{
							mtlfile[mtlchars] = input;
							fin.get (input);
							mtlchars++;
						}
						mtlfile[mtlchars] = '\0';
						ReadMaterialFile (mtlfile);
					}
				}
			}
		}


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

bool ConverterClass::LoadDataStructures(WCHAR* filename, int type, int vertexCount,
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
	texcoords = new VertexType[textureCount];
	normals = new VertexType[normalCount];
	faces = new FaceType[faceCount];

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

	fout << "f: ";
	if (textured)
	{
		fout << "y" << endl;
		fout << "p: " << texturename << endl;
	}
	else if (!textured)
	{
		fout << "n" << endl;
		fout << "c: " << r << " " << g << " " << b << endl;
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

void ConverterClass::ReadMaterialFile (char* filename)
{
	textured = false;
	char input;
	ifstream fin;
	char texturefile[50];
	int texchars = 0;
	char filepath[16] = {'.','.','/','E','n','g','i','n','e','/','d','a','t','a','/','\0'};
	string totalname = filepath;
	texturename = filepath;
	totalname = totalname + filename;

	fin.open (totalname);
	fin.get (input);

	while (!fin.eof ())
	{
		fin.get (input);
		if (input == '\n')
		{
			fin.get (input);
			if (input == 'K')
			{
				fin.get (input);
				if (input == 'd')
				{
					fin.get (input);
					if (input == ' ')
					{
						fin >> r >> g >> b;
					}
				}
			}
		}

		if (input == 'm')
		{
			fin.get (input);
			if (input == 'a')
			{
				fin.get (input);
				if (input == 'p')
				{
					fin.get (input); //underscore
					fin.get (input); //K
					fin.get (input); //d
					fin.get (input); //whitespace

					fin.get (input); //first letter
					while (input != '\n')
					{
						texturefile[texchars] = input;
						fin.get (input);
						texchars++;
					}
					texturefile[texchars] = '\0';
					textured = true;
					texturename = texturename + texturefile;
				}
			}
		}
	}
	

}