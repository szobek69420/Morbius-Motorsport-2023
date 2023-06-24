#ifndef MESH_H
#define MESH_H

#include "my_vector.hpp"

//azert ez a neve, mert a model loadernek is van egy class Mesh-je
class Mesh_NotAssimp {
private:
	gigachad::vector<float> vertices;
	gigachad::vector<int> indices;
	int vertexCount;
	int indexCount;

public:
	Mesh_NotAssimp();
	Mesh_NotAssimp(const Mesh_NotAssimp& otter);
	~Mesh_NotAssimp();

	//igazabol 6 szam, normalvektorokat nem tartalmazo csucs
	void AddVertex4(float x, float y, float z, float uvx, float uvy, float brightness);//chunk-specific

	//normalvektorokat is tartalmazo csucs
	void AddVertex9(float x, float y, float z, float normX, float normY, float normZ, float uvx, float uvy, float brightness);

	void AddIndex(unsigned int index);

	//kiuriti a mesht
	void ResetMesh();

	unsigned int GetVertexCount();

	const void* GetVerticesFront();

	unsigned int GetIndexCount();

	const void* GetIndicesFront();

	//egyenlove teves
	Mesh_NotAssimp& operator=(const Mesh_NotAssimp& otter);
};

#endif