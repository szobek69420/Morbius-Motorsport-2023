#include "mesh.h"

Mesh_NotAssimp::Mesh_NotAssimp()
{
	vertexCount = 0;
	indexCount = 0;
}

Mesh_NotAssimp::Mesh_NotAssimp(const Mesh_NotAssimp& otter)
{
	vertexCount = otter.vertexCount;
	indexCount = otter.indexCount;

	vertices = otter.vertices;
	indices = otter.indices;
}

Mesh_NotAssimp::~Mesh_NotAssimp()
{
	vertices.clear();
	indices.clear();
}

void Mesh_NotAssimp::AddVertex4(float x, float y, float z, float uvx, float uvy, float brightness)//chunk-specific
{
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
	vertices.push_back(uvx);
	vertices.push_back(uvy);
	vertices.push_back(brightness);

	vertexCount += 6;
}

void Mesh_NotAssimp::AddVertex9(float x, float y, float z, float normX, float normY, float normZ, float uvx, float uvy, float brightness)
{
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
	vertices.push_back(normX);
	vertices.push_back(normY);
	vertices.push_back(normZ);
	vertices.push_back(uvx);
	vertices.push_back(uvy);
	vertices.push_back(brightness);

	vertexCount += 9;
}

void Mesh_NotAssimp::AddIndex(unsigned int index)
{
	indices.push_back(index);
	indexCount++;
}

void Mesh_NotAssimp::ResetMesh()
{
	vertices.clear();
	indices.clear();
	vertexCount = 0;
	indexCount = 0;
}

unsigned int Mesh_NotAssimp::GetVertexCount()
{
	return vertices.size();
}

const void* Mesh_NotAssimp::GetVerticesFront()
{
	if (vertices.size() == 0)
		return nullptr;
	else
		return (void*)(&(vertices[0]));
}

unsigned int Mesh_NotAssimp::GetIndexCount()
{
	return indices.size();
}

const void* Mesh_NotAssimp::GetIndicesFront()
{
	if (indices.size() == 0)
		return nullptr;
	else
		return (void*)(&(indices[0]));
}

Mesh_NotAssimp& Mesh_NotAssimp::operator=(const Mesh_NotAssimp& otter)
{
	if (this == &otter)
		return *this;

	vertexCount = otter.vertexCount;
	indexCount = otter.indexCount;

	vertices = otter.vertices;
	indices = otter.indices;
}