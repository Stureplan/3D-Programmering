///////////////////////
//FILE CONVERTER CLASS
///////////////////////
#ifndef _CONVERTER_H_
#define _CONVERTER_H_


//INCLUDES
#include <iostream>
#include <fstream>
#include <string>
#include <D3D10.h>
#include <D3DX10.h>
using namespace std;


//TYPEDEFS
typedef struct
{
	float x, y, z;
}VertexType;

typedef struct
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
}FaceType;


//CLASS NAME
class ConverterClass
{
public:
	ConverterClass();
	ConverterClass(const ConverterClass&);
	~ConverterClass();

	int Convert(WCHAR*, int);
	void ReadMaterialFile (char*);

private:
	bool ReadFileCounts(WCHAR*, int&, int&, int&, int&);
	bool LoadDataStructures(WCHAR*, int, int, int, int, int);
	float r, g, b;
	bool textured;
	string texturename;
};



#endif