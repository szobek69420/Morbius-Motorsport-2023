#include "physics.h"
#include <glad/glad.h>
#include "shader.h"
#include "blockdatabase.h"
#include "meinmath.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

const float BLOCK_WIDTH = 1.0f;
const float BLOCK_HEIGHT = 1.0f;
const float BLOCK_WIDTH2 = 0.5f;
const float BLOCK_HEIGHT2 = 0.5f;

const float PLAYER_HEIGHT = 1.8f;
const float PLAYER_WIDTH = 0.5f;
const float PLAYER_HEIGHT2 = 0.9f;
const float PLAYER_WIDTH2 = 0.25f;
const float EYE_FROM_MIDDLE = 0.6f;//milyen tavol van a kamera az AABB kozepetol
const float DIFFERENCE_HEIGHT_WIDTH = PLAYER_HEIGHT2 - PLAYER_WIDTH2;

const float MAX_DISTANCE_XZ = PLAYER_WIDTH2 + BLOCK_WIDTH2;
const float MAX_DISTANCE_Y = PLAYER_HEIGHT2 + BLOCK_HEIGHT2;
const float MAX_DISTANCE_SQUARED = MAX_DISTANCE_Y*MAX_DISTANCE_Y + 2.0F*MAX_DISTANCE_XZ*MAX_DISTANCE_SQUARED;//a maximalis intersection tavolsag jatekos es blokk kozott (negyzeten van, hogy ne kelljen mindig gyokot vonni)


void Physics::AddAABB(glm::vec3 position, bool isSolid,bool isWater)
{
	if (isWater)
		this->waterAABBs.push_back(CreateAABB(position, isSolid, isWater));
	else
		this->blockAABBs.push_back(CreateAABB(position, isSolid, isWater));
}

void Physics::AddAABB(int x, int y, int z, bool isSolid, bool isWater)
{
	if (isWater)
		this->waterAABBs.push_back(CreateAABB(x,y,z, isSolid, isWater));
	else
		this->blockAABBs.push_back(CreateAABB(x,y,z, isSolid, isWater));
}

void Physics::AddAABB(float x, float y, float z, bool isSolid, bool isWater)
{
	if (isWater)
		this->waterAABBs.push_back(CreateAABB(x, y, z, isSolid, isWater));
	else
		this->blockAABBs.push_back(CreateAABB(x, y, z, isSolid, isWater));
}

void Physics::ResetAABBs()
{
	blockAABBs.clear();
	waterAABBs.clear();

	return;
}

unsigned int Physics::GetAABBCount(bool water)
{
	if (water)
		return waterAABBs.size();
	else
		return blockAABBs.size();
}

Physics::CollisionType Physics::PlayerCollisionDetection(glm::vec3& pos, glm::vec3& velocity)
{
	Physics::CollisionType returnValue = Physics::COLLISION_NONE;


	int length = this->blockAABBs.size();
	float* distances = new float[length];
	int* indices = new int[length];
	float temp;

	pos.y -= EYE_FROM_MIDDLE;

	for (int i = 0; i < length; i++)
	{
		indices[i] = i;

		//x
		temp = (pos.x - this->blockAABBs[i].worldPos[0]);
		temp *= temp;
		distances[i] = temp;
		//y
		temp = (pos.y - this->blockAABBs[i].worldPos[1]);
		temp *= temp;
		distances[i] += temp;
		//z
		temp = (pos.z - this->blockAABBs[i].worldPos[2]);
		temp *= temp;
		distances[i] += temp;
	}

	quicksort_physics(distances, indices, 0, length-1);

	//actual physics
	float* pos_temp;
	float abstand[3];
	int i = 0;
	for (i = 0; i < length; i++)
	{
		if (distances[i] > MAX_DISTANCE_SQUARED+1.0f)
			break;
		if (!this->blockAABBs[indices[i]].isSolid)
			continue;

		pos_temp = (this->blockAABBs[indices[i]].worldPos);
		abstand[0] = pos.x - pos_temp[0];
		abstand[1] = pos.y - pos_temp[1];
		abstand[2] = pos.z - pos_temp[2];
		if (fabs(abstand[0]) >= MAX_DISTANCE_XZ || fabs(abstand[1]) >= MAX_DISTANCE_Y || fabs(abstand[2]) >= MAX_DISTANCE_XZ)
			continue;


		if (fabs(abstand[0]) > fabs(abstand[1]) - DIFFERENCE_HEIGHT_WIDTH)
		{
			if (fabs(abstand[2]) > fabs(abstand[0]))//z van a legkijjebb
			{
				if (abstand[2] < 0)
				{
					pos.z = pos_temp[2] - MAX_DISTANCE_XZ;
					velocity.z = 0;
				}
				else
				{
					pos.z = pos_temp[2] + MAX_DISTANCE_XZ;
					velocity.z = 0;
				}

				returnValue = COLLISION_EDGE;
			}
			else//x van a legkijjebb
			{
				if (abstand[0] < 0)
				{
					pos.x = pos_temp[0] - MAX_DISTANCE_XZ;
					velocity.x = 0;
				}
				else
				{
					pos.x = pos_temp[0] + MAX_DISTANCE_XZ;
					velocity.x = 0;
				}

				returnValue = COLLISION_EDGE;
			}
		}
		else
		{
			if (fabs(abstand[2]) > fabs(abstand[1]) - DIFFERENCE_HEIGHT_WIDTH)//z van a legkijjebb
			{
				if (abstand[2] < 0)
				{
					pos.z = pos_temp[2] - MAX_DISTANCE_XZ;
					velocity.z = 0;
				}
				else
				{
					pos.z = pos_temp[2] + MAX_DISTANCE_XZ;
					velocity.z = 0;
				}

				returnValue = COLLISION_EDGE;
			}
			else//y van a legkijjebb
			{
				if (abstand[1] < 0)
				{
					pos.y = pos_temp[1] - MAX_DISTANCE_Y;
					if(velocity.y>0) velocity.y = -1.0f;

					returnValue = returnValue!=COLLISION_EDGE? COLLISION_CEILING:COLLISION_EDGE;//hogy a COLLISION_EDGE-et priorizalja
				}
				else
				{
					pos.y = pos_temp[1] + MAX_DISTANCE_Y;
					if(velocity.y<0) velocity.y = 0;

					returnValue = returnValue != COLLISION_EDGE ? COLLISION_FLOOR : COLLISION_EDGE;//hogy a COLLISION_EDGE-et priorizalja
				}
			}
		}
	}

	pos.y += EYE_FROM_MIDDLE;
	delete[] distances;
	delete[] indices;

	return returnValue;
	//std::cout << "aabbs: " << pos_temp[0]<<" "<<pos_temp[1]<<" "<<pos_temp[2]<<" | "<<i << std::endl;
}

bool Physics::isPointSubmerged(const glm::vec3& _point)
{
	int length = this->waterAABBs.size();
	bool talalt = false;
	for (int i = 0; i < length; i++)
	{
		if (fabs(_point.x - waterAABBs[i].worldPos[0]) < BLOCK_WIDTH2 && fabs(_point.y - waterAABBs[i].worldPos[1]) < BLOCK_HEIGHT2 && fabs(_point.z - waterAABBs[i].worldPos[2]) < BLOCK_WIDTH2)
		{
			talalt = true;
			break;
		}
	}

	return talalt;
}


//static

struct aabb_block Physics::CreateAABB(glm::vec3 position, bool isSolid, bool isWater)
{
	struct aabb_block temp;
	temp.worldPos[0] = position.x;
	temp.worldPos[1] = position.y;
	temp.worldPos[2] = position.z;

	temp.isSolid = isSolid;

	temp.isWater = isWater;

	return temp;
}

struct aabb_block Physics::CreateAABB(int x, int y, int z, bool isSolid, bool isWater)//automatikusan blokkot keszit
{
	struct aabb_block temp;
	temp.worldPos[0] = x;
	temp.worldPos[1] = y;
	temp.worldPos[2] = z;

	temp.isSolid = isSolid;

	temp.isWater = isWater;

	return temp;
}

bool Physics::isIntersecting(glm::vec3 playerPos, glm::vec3 blockPos)
{
	playerPos.y -= EYE_FROM_MIDDLE;
	if (fabs(playerPos.x - blockPos.x) > BLOCK_WIDTH2 + PLAYER_WIDTH2 || fabs(playerPos.y - blockPos.y) > BLOCK_HEIGHT2 + PLAYER_HEIGHT2 || fabs(playerPos.z - blockPos.z) > BLOCK_WIDTH2 + PLAYER_WIDTH2)
		return false;
	return true;
}


//SelectionRenderer


float vertices_select[] = {
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.5f
};

unsigned int indices_select[] = {
	0,1,
	1,2,
	2,3,
	3,0,
	4,5,
	5,6,
	6,7,
	7,4,
	0,4,
	1,5,
	2,6,
	3,7
};//primitive: GL_LINES


SelectionRenderer::SelectionRenderer()
{
	glGenVertexArrays(1, &selectionVAO);
	glBindVertexArray(selectionVAO);


	glGenBuffers(1, &selectionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, selectionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_select), vertices_select, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	
	glGenBuffers(1, &selectionEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, selectionEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_select), indices_select, GL_STATIC_DRAW);

	//shader
	shaderProgram = createShader(SHADER_SELECTION);
}

SelectionRenderer::~SelectionRenderer()
{
	glDeleteBuffers(1, &selectionVBO);
	glDeleteBuffers(1, &selectionEBO);
	glDeleteVertexArrays(1, &selectionVAO);

	selectionVAO = 0;
	selectionVBO = 0;
	selectionEBO = 0;

	destroyShader(shaderProgram);
}

void SelectionRenderer::Render(glm::vec3 pos, glm::mat4 comboed)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glUseProgram(shaderProgram);
	glUniform3f(glGetUniformLocation(shaderProgram, "worldPos"), pos.x, pos.y, pos.z);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "comboed"), 1, GL_FALSE, glm::value_ptr(comboed));


	glBindVertexArray(selectionVAO);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, NULL);
}