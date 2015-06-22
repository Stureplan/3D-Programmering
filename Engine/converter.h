///////////////////////
//FILE CONVERTER CLASS
///////////////////////
#ifndef _CONVERTER_H_
#define _CONVERTER_H_


//INCLUDES
#include <iostream>
#include <fstream>
#include <string>
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

	int Convert();

private:
	void GetModelFilename(char*);
	bool ReadFileCounts(char*, int&, int&, int&, int&);
	bool LoadDataStructures(char*, int, int, int, int);
};



#endif