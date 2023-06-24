#include "chunk.h"
#include "chunkmanager.h"
#include "blockdatabase.h"
#include <glad/glad.h>
#include "FastNoiseLite.h"
#include <cmath>
#include <iostream>

const int TREE_GENERATION_MIN_OAK = RAND_MAX - RAND_MAX * 0.006f;
const int TREE_GENERATION_MIN_BIRCH = RAND_MAX - RAND_MAX * 0.001f;

const int FLOWER_GENERATION_MIN_RED = RAND_MAX - RAND_MAX * 0.12F;
const int FLOWER_GENERATION_MIN_YELLOW = RAND_MAX - RAND_MAX * 0.08F;
const int FLOWER_GENERATION_MIN_BLUE = RAND_MAX - RAND_MAX * 0.04F;

const int SEA_LEVEL = 30;
const int SNOW_LEVEL = 70;

const int MINIMAL_LIGHT_LEVEL = 10;
const int LIGHT_OBSCURED = 15;//amennyivel lejjebb esik a megvilagitas erossege, ha van felette egy blokk

Chunk::Chunk(int X, int Z, void* cm)
{
	chunkNumber[0] = X;
	chunkNumber[1] = Z;

	blockMesh = new Mesh_NotAssimp();
	blockVAO = 0;
	blockVBO = 0;
	blockEBO = 0;
	elementCount_block = 0;

	waterMesh = new Mesh_NotAssimp();
	waterVAO = 0;
	waterVBO = 0;
	waterEBO = 0;
	elementCount_water = 0;

	physics = new Physics();
	for (int i = 0; i < 4; i++)
		borderPhysics[i] = new Physics();

	chunkManager = cm;
}

Chunk::~Chunk()
{
	if (blockVAO)
		glDeleteVertexArrays(1, &blockVAO);
	if (blockVBO)
		glDeleteBuffers(1, &blockVBO);
	if (blockEBO)
		glDeleteBuffers(1, &blockEBO);

	blockVAO = 0;
	blockVBO = 0;
	blockEBO = 0;
	elementCount_block = 0;


	if (waterVAO)
		glDeleteVertexArrays(1, &waterVAO);
	if (waterVBO)
		glDeleteBuffers(1, &waterVBO);
	if (waterEBO)
		glDeleteBuffers(1, &waterEBO);

	waterVAO = 0;
	waterVBO = 0;
	waterEBO = 0;
	elementCount_water = 0;


	delete physics;
	for (int i = 0; i < 4; i++)
		delete borderPhysics[i];

	delete blockMesh;
	delete waterMesh;
}


void Chunk::GenerateChunk()
{
	int i, j, k, l;
	ChunkManager* cm_temp = (ChunkManager*)chunkManager;

	this->physics->ResetAABBs();
	for (i = 0; i < 4; i++)
		this->borderPhysics[i]->ResetAABBs();

	
	/*FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetFractalOctaves(10);
	noise.SetFrequency(0.005f);*/

	float offsets[2];
	offsets[0] = chunkNumber[0] * 16;
	offsets[1] = chunkNumber[1] * 16;
	/*offsets[0] = ((ChunkManager*)chunkManager)->GetTerrainOffset(0);
	offsets[1] = ((ChunkManager*)chunkManager)->GetTerrainOffset(1);*/
	//std::cout << offsets[0] << " " << offsets[1] << std::endl;

	float tempheight;
	//heightmap
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{
			/*tempheight = 50 + 30.0f * noise.GetNoise((chunkNumber[0] * 16.0f + i) * 2.0f+offsets[0], (chunkNumber[1] * 16.0f + j) * 2.0f + offsets[1]) + 15.0f * noise.GetNoise((chunkNumber[0] * 16.0f + i) * 4.0f + offsets[0], (chunkNumber[1] * 16.0f + j) * 4.0f + offsets[1]) + 4.0f * noise.GetNoise((chunkNumber[0] * 16.0f + i) * 8.0f + offsets[0], (chunkNumber[1] * 16.0f + j) * 8.0f + offsets[1]);
			if (tempheight < 10.0f)
				tempheight = 10.0f;*/
			tempheight = cm_temp->GetTerrainHeight(offsets[0] + i, offsets[1] + j);
			heightmap[i][j] = (unsigned char)tempheight;
		}
	}
	//surrounding chunks (changed surroundings are in the changed blocks part)
	//-z
	for (i = 0; i < 16; i++)
	{
		//tempheight = 50 + 30.0f * noise.GetNoise((chunkNumber[0] * 16.0f +i) * 2.0f + offsets[0], (chunkNumber[1] * 16.0f -1) * 2.0f + offsets[1]) + 15.0f * noise.GetNoise((chunkNumber[0] * 16.0f + i) * 4.0f + offsets[0], (chunkNumber[1] * 16.0f - 1) * 4.0f + offsets[1]) + 4.0f * noise.GetNoise((chunkNumber[0] * 16.0f + i) * 8.0f + offsets[0], (chunkNumber[1] * 16.0f -1) * 8.0f + offsets[1]);
		tempheight= cm_temp->GetTerrainHeight(offsets[0] + i, offsets[1] -1);
		for (j = 0; j < 150; j++)
		{
			if (j <= tempheight)
				isAir[0][i][j] = false;
			else
			{
				if (j <= SEA_LEVEL)
				{
					isWater[0][i][j] = true;
					isAir[0][i][j] = false;
				}
				else
				{
					isAir[0][i][j] = true;
					isWater[0][i][j] = false;
				}
			}

			isGlass[0][i][j] = false;
			//isTransparent[0][i][j] = false;
		}
	}
	//+x
	for (i = 0; i < 16; i++)
	{
		//tempheight = 50 + 30.0f * noise.GetNoise((chunkNumber[0] * 16.0f + 16) * 2.0f + offsets[0], (chunkNumber[1] * 16.0f +i) * 2.0f + offsets[1]) + 15.0f * noise.GetNoise((chunkNumber[0] * 16.0f + 16) * 4.0f + offsets[0], (chunkNumber[1] * 16.0f +i) * 4.0f + offsets[1]) + 4.0f * noise.GetNoise((chunkNumber[0] * 16.0f + 16) * 8.0f + offsets[0], (chunkNumber[1] * 16.0f +i) * 8.0f + offsets[1]);
		tempheight = cm_temp->GetTerrainHeight(offsets[0] + 16, offsets[1] + i);
		for (j = 0; j < 150; j++)
		{
			if (j <= tempheight)
				isAir[1][i][j] = false;
			else
			{
				if (j <= SEA_LEVEL)
				{
					isWater[1][i][j] = true;
					isAir[1][i][j] = false;
				}
				else
				{
					isAir[1][i][j] = true;
					isWater[1][i][j] = false;
				}
			}

			isGlass[1][i][j] = false;
			//isTransparent[1][i][j] = false;
		}
	}
	//+z
	for (i = 0; i < 16; i++)
	{
		//tempheight = 50 + 30.0f * noise.GetNoise((chunkNumber[0] * 16.0f + i) * 2.0f + offsets[0], (chunkNumber[1] * 16.0f +16) * 2.0f + offsets[1]) + 15.0f * noise.GetNoise((chunkNumber[0] * 16.0f + i) * 4.0f + offsets[0], (chunkNumber[1] * 16.0f +16) * 4.0f + offsets[1]) + 4.0f * noise.GetNoise((chunkNumber[0] * 16.0f + i) * 8.0f + offsets[0], (chunkNumber[1] * 16.0f +16) * 8.0f + offsets[1]);
		tempheight = cm_temp->GetTerrainHeight(offsets[0] + i, offsets[1] +16);
		for (j = 0; j < 150; j++)
		{
			if (j <= tempheight)
				isAir[2][i][j] = false;
			else
			{
				if (j <= SEA_LEVEL)
				{
					isWater[2][i][j] = true;
					isAir[2][i][j] = false;
				}
				else
				{
					isAir[2][i][j] = true;
					isWater[2][i][j] = false;
				}
			}

			isGlass[2][i][j] = false;
			//isTransparent[2][i][j] = false;
		}
	}
	//-x
	for (i = 0; i < 16; i++)
	{
		//tempheight = 50 + 30.0f * noise.GetNoise((chunkNumber[0] * 16.0f - 1) * 2.0f + offsets[0], (chunkNumber[1] * 16.0f + i) * 2.0f + offsets[1]) + 15.0f * noise.GetNoise((chunkNumber[0] * 16.0f - 1) * 4.0f + offsets[0], (chunkNumber[1] * 16.0f + i) * 4.0f + offsets[1]) + 4.0f * noise.GetNoise((chunkNumber[0] * 16.0f - 1) * 8.0f + offsets[0], (chunkNumber[1] * 16.0f + i) * 8.0f + offsets[1]);
		tempheight = cm_temp->GetTerrainHeight(offsets[0] -1, offsets[1] +i);
		for (j = 0; j < 150; j++)
		{
			if (j <= tempheight)
				isAir[3][i][j] = false;
			else
			{
				if (j <= SEA_LEVEL)
				{
					isWater[3][i][j] = true;
					isAir[3][i][j] = false;
				}
				else
				{
					isAir[3][i][j] = true;
					isWater[3][i][j] = false;
				}
			}

			isGlass[3][i][j] = false;
			//isTransparent[3][i][j] = false;
		}
	}


	//terrain generation
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 150; j++)
		{
			for (k = 0; k < 16; k++)
			{
				if (j > heightmap[i][k])
				{
					if(j>SEA_LEVEL)
						blockType[i][j][k] = AIR;
					else
						blockType[i][j][k] = WATER;
				}
				else if (j > heightmap[i][k] - 1)
				{
					if (j > SNOW_LEVEL)
						blockType[i][j][k] = SNOW_BLOCK;
					else if (j > SEA_LEVEL + 1)
						blockType[i][j][k] = GRASS_BLOCK;
					else
						blockType[i][j][k] = SAND;
				}
				else if (j > heightmap[i][k] - 3)
				{
					if (j > SEA_LEVEL - 2)
						blockType[i][j][k] = DIRT;
					else
						blockType[i][j][k] = SAND;
				}
				else if (j == 0)
					blockType[i][j][k] = BEDROCK;
				else
					blockType[i][j][k] = STONE;
			}
		}
	}

	//tree generation
	int seed = ((ChunkManager*)chunkManager)->GetSeed();
	seed += 1000 * chunkNumber[0] + 10*chunkNumber[1]+6;
	srand(seed);

	int randValue = 0;
	int treeHeight = 6;
	int _treeType;
	for (i = 2; i < 14; i++)
	{
		for (j = 2; j < 14; j++)
		{
			if (blockType[i][heightmap[i][j]][j] != GRASS_BLOCK)
				continue;

			randValue = rand();

			if (randValue > TREE_GENERATION_MIN_OAK)
			{
				_treeType = OAK_LOG;
				if (randValue > TREE_GENERATION_MIN_BIRCH)
					_treeType = BIRCH_LOG;

				k = heightmap[i][j];
				if (k + treeHeight + 1 >= 150)
					continue;
				
				blockType[i][k][j] = DIRT;
				k++;

				for (; k <= heightmap[i][j] + treeHeight - 3; k++)
					blockType[i][k][j] = _treeType;

				k = heightmap[i][j] + treeHeight - 2;
				blockType[i][k][j] = _treeType;
				for (int l = i - 2; l <= i + 2; l++)
				{
					for (int m = j - 2; m <= j + 2; m++)
					{
						if (blockType[l][k][m] == AIR)
							blockType[l][k][m] = _treeType+1;
					}
				}
				k++;

				blockType[i][k][j] = _treeType;
				for (int l = i - 2; l <= i + 2; l++)
				{
					for (int m = j - 2; m <= j + 2; m++)
					{
						if (blockType[l][k][m] == AIR)
							blockType[l][k][m] = _treeType+1;
					}
				}
				k++;

				blockType[i][k][j] = _treeType;
				for (int l = i - 1; l <= i + 1; l++)
				{
					for (int m = j - 1; m <= j + 1; m++)
					{
						if (blockType[l][k][m] == AIR)
							blockType[l][k][m] = _treeType+1;
					}
				}
				k++;

				
				for (int l = i - 1; l <= i + 1; l++)
				{
					for (int m = j - 1; m <= j + 1; m++)
					{
						if (l - i == -1 || l - i == 1)
							if (m - j == -1 || m - j == 1)
								continue;

						if (blockType[l][k][m] == AIR)
							blockType[l][k][m] = _treeType+1;
					}
				}
				k++;
			}
			else
			{
				randValue = rand();

				if (randValue > FLOWER_GENERATION_MIN_BLUE)
					blockType[i][heightmap[i][j] + 1][j] = BLUE_FLOWER;

				else if (randValue > FLOWER_GENERATION_MIN_YELLOW)
					blockType[i][heightmap[i][j] + 1][j] = YELLOW_FLOWER;

				else if (randValue > FLOWER_GENERATION_MIN_RED)
					blockType[i][heightmap[i][j] + 1][j] = RED_FLOWER;
			}
		}
	}

	//changed blocks
	//ChunkManager* cm_temp = (ChunkManager*)chunkManager; "cm_temp athelyezve a fuggveny elejere"
	int changed_cn[2];
	for (i = 0; i < cm_temp->changedBlocks.size(); i++)
	{
		changed_cn[0] = cm_temp->changedBlocks[i].chunkNumber[0];
		changed_cn[1] = cm_temp->changedBlocks[i].chunkNumber[1];

		if (changed_cn[0] == chunkNumber[0] && changed_cn[1] == chunkNumber[1])
			blockType[cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]][cm_temp->changedBlocks[i].localPos[2]] = cm_temp->changedBlocks[i].type;
		//changed surroundings
		if (changed_cn[0] == chunkNumber[0])
		{
			if (changed_cn[1] == chunkNumber[1] - 1 && cm_temp->changedBlocks[i].localPos[2] == 15)//-z
			{
				switch (cm_temp->changedBlocks[i].type)
				{
				case AIR:
					isAir[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = true;
					isWater[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isGlass[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;

				case WATER:
					isAir[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isWater[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = true;
					isGlass[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;

				case GLASS_BLOCK:
					isAir[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isWater[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = true;
					isGlass[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;

				default:
					if (cm_temp->changedBlocks[i].type >= RED_FLOWER)
						isAir[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = true;//levegonek veszem, mert ugysem szamit
					else
						isAir[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;

					isWater[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isGlass[0][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;
				}
			}
			else if (changed_cn[1] == chunkNumber[1] + 1 && cm_temp->changedBlocks[i].localPos[2] == 0)//+z
			{
				switch (cm_temp->changedBlocks[i].type)
				{
				case AIR:
					isAir[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = true;
					isWater[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isGlass[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;

				case WATER:
					isAir[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isWater[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = true;
					isGlass[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;

				case GLASS_BLOCK:
					isAir[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isWater[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isGlass[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = true;
					break;

				default:
					if (cm_temp->changedBlocks[i].type >= RED_FLOWER)
						isAir[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = true;
					else
						isAir[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;

					isWater[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isGlass[2][cm_temp->changedBlocks[i].localPos[0]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;
				}
			}
		}
		else if (changed_cn[1] == chunkNumber[1])
		{
			if (changed_cn[0] == chunkNumber[0] - 1 && cm_temp->changedBlocks[i].localPos[0] == 15)//-x
			{
				switch (cm_temp->changedBlocks[i].type)
				{
				case AIR:
					isAir[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = true;
					isWater[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isGlass[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;

				case WATER:
					isAir[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isWater[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = true;
					isGlass[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;

				case GLASS_BLOCK:
					isAir[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isWater[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isGlass[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = true;
					break;

				default:
					if(cm_temp->changedBlocks[i].type>=RED_FLOWER)
						isAir[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = true;
					else
						isAir[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;

					isWater[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isGlass[3][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;
				}
			}
			else if (changed_cn[0] == chunkNumber[0] + 1 && cm_temp->changedBlocks[i].localPos[0] == 0)//+x
			{
				switch (cm_temp->changedBlocks[i].type)
				{
				case AIR:
					isAir[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = true;
					isWater[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isGlass[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;

				case WATER:
					isAir[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isWater[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = true;
					isGlass[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;

				case GLASS_BLOCK:
					isAir[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isWater[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isGlass[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = true;
					break;

				default:
					if(cm_temp->changedBlocks[i].type>=RED_FLOWER)
						isAir[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = true;
					else
						isAir[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;

					isWater[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					isGlass[1][cm_temp->changedBlocks[i].localPos[2]][cm_temp->changedBlocks[i].localPos[1]] = false;
					break;
				}
			}
		}
	}

	for (i = 0; i < 4; i++)
		for (j = 0; j < 16; j++)
			for (k = 0; k < 150; k++)
			{
				//if (isAir[i][j][k] || isWater[i][j][k] || isGlass[i][j][k])
				//	isTransparent[i][j][k] = true;
				//else isTransparent[i][j][k] = false;
				isTransparent[i][j][k] = isAir[i][j][k] + isWater[i][j][k] + isGlass[i][j][k];
			}
	

	//calculating lightlevel
	unsigned char lightLevel_top[18][18];
	unsigned char lightLevel_temp[18][151][18];
	for (i = 0; i < 18; i++)
		for (j = 0; j < 18; j++)
			lightLevel_top[i][j] = 100;

#pragma region light_preparation
	//chunk belseje
	for (i = 1; i < 17; i++)
	{
		for (j = 149; j >= 0; j--)
		{
			for (k = 1; k < 17; k++)
			{
				lightLevel_temp[i][j][k] = lightLevel_top[i][k];
				if (blockType[i-1][j][k-1] != AIR&&blockType[i-1][j][k-1]!=GLASS_BLOCK && lightLevel_top[i][k] > MINIMAL_LIGHT_LEVEL)
					lightLevel_top[i][k] -= LIGHT_OBSCURED;
			}
		}
	}
	for (i = 0; i < 18; i++)
	{
		for (k = 0; k < 18; k++)
		{
			lightLevel_temp[i][150][k] = 100;
		}
	}
	//chunk mellett
	for (i = 1; i < 17; i++)//-z
	{
		for (j = 149; j >= 0; j--)
		{
			lightLevel_temp[i][j][0] = lightLevel_top[i][0];
			if (!isAir[0][i-1][j] && lightLevel_top[i][0] > MINIMAL_LIGHT_LEVEL)
				lightLevel_top[i][0] -= LIGHT_OBSCURED;
		}
	}
	for (i = 1; i < 17; i++)//+X
	{
		for (j = 149; j >= 0; j--)
		{
			lightLevel_temp[17][j][i] = lightLevel_top[17][i];
			if (!isAir[1][i - 1][j] && lightLevel_top[17][i] > MINIMAL_LIGHT_LEVEL)
				lightLevel_top[17][i] -= LIGHT_OBSCURED;
		}
	}
	for (i = 1; i < 17; i++)//+z
	{
		for (j = 149; j >= 0; j--)
		{
			lightLevel_temp[i][j][17] = lightLevel_top[i][17];
			if (!isAir[2][i - 1][j] && lightLevel_top[i][17] > MINIMAL_LIGHT_LEVEL)
				lightLevel_top[i][17] -= LIGHT_OBSCURED;
		}
	}
	for (i = 1; i < 17; i++)//-x
	{
		for (j = 149; j >= 0; j--)
		{
			lightLevel_temp[0][j][i] = lightLevel_top[0][i];
			if (!isAir[3][i - 1][j] && lightLevel_top[0][i] > MINIMAL_LIGHT_LEVEL)
				lightLevel_top[0][i] -= LIGHT_OBSCURED;
		}
	}
#pragma endregion light_preparation

	//finalize lights (lightLevel kiszamitasa a szomszedok alapjan)
	for (i = 1; i < 17; i++)
	{
		for (j = 0; j < 150; j++)
		{
			for (k = 1; k < 17; k++)
			{
				//atlagolja a kornyezo lightleveleket
				lightLevel[i-1][j][k-1] = (lightLevel_temp[i-1][j][k] + lightLevel_temp[i + 1][j][k] + lightLevel_temp[i][j][k -1] + lightLevel_temp[i][j][k + 1] + lightLevel_temp[i][j + 1][k]) / 5;
			}
		}
	}
	
	//calculating meshes

	blockMesh->ResetMesh();
	int sides = 0;//igazabol a megjelenitett oldalak 4-szerese, hogy kevesebbet kelljen szorozgatni
	float x, y, z, uvx, uvy, dankness;
	float baseX = chunkNumber[0] * 16.0, baseZ = chunkNumber[1] * 16.0;
	bool vanrender;

	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 150; j++)
		{
			for (k = 0; k < 16; k++)
			{
				if (blockType[i][j][k] == AIR||blockType[i][j][k]==WATER)
					continue;

				if (blockType[i][j][k] >=RED_FLOWER)
				{
					for (l = 0; l < 4; l++)
					{
						for (unsigned int m = 0; m < 4; m++)
						{
							BlockDatabase::GetFlowerVertex(l,m, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(l,m, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (unsigned int m = 0; m < 6; m++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(m));
						}

						sides += 4;
					}
				}
				else if (blockType[i][j][k] == GLASS_BLOCK)
				{
					vanrender = false;
					//elso oldal
					if (k == 0)
					{
						if (isTransparent[0][i][j]&&!isGlass[0][i][j])
						{
							for (l = 0; l < 4; l++)
							{
								BlockDatabase::GetVertex(NEGATIVE_Z, l, x, y, z, dankness);
								x += baseX + i;
								y += j;
								z += baseZ + k;
								dankness *= (lightLevel[i][j][k] / 100.0f);

								BlockDatabase::GetUv(NEGATIVE_Z, l, blockType[i][j][k], uvx, uvy);

								blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
							}

							for (l = 0; l < 6; l++)
							{
								blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
							}
							sides += 4;
							vanrender = true;
						}
					}
					else if ((blockType[i][j][k - 1] == AIR || blockType[i][j][k - 1] >= WATER)&&blockType[i][j][k-1]!=GLASS_BLOCK)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(NEGATIVE_Z, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(NEGATIVE_Z, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}

					//masodik oldal
					if (i == 15)
					{
						if (isTransparent[1][k][j] && !isGlass[1][k][j])
						{
							for (l = 0; l < 4; l++)
							{
								BlockDatabase::GetVertex(POSITIVE_X, l, x, y, z, dankness);
								x += baseX + i;
								y += j;
								z += baseZ + k;
								dankness *= (lightLevel[i][j][k] / 100.0f);

								BlockDatabase::GetUv(POSITIVE_X, l, blockType[i][j][k], uvx, uvy);

								blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
							}

							for (l = 0; l < 6; l++)
							{
								blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
							}
							sides += 4;
							vanrender = true;
						}
					}
					else if ((blockType[i + 1][j][k] == AIR || blockType[i + 1][j][k] >= WATER)&&blockType[i+1][j][k]!=GLASS_BLOCK)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(POSITIVE_X, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(POSITIVE_X, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}

					//harmadik oldal
					if (k == 15)
					{
						if (isTransparent[2][i][j] && !isGlass[2][i][j])
						{
							for (l = 0; l < 4; l++)
							{
								BlockDatabase::GetVertex(POSITIVE_Z, l, x, y, z, dankness);
								x += baseX + i;
								y += j;
								z += baseZ + k;
								dankness *= (lightLevel[i][j][k] / 100.0f);

								BlockDatabase::GetUv(POSITIVE_Z, l, blockType[i][j][k], uvx, uvy);

								blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
							}

							for (l = 0; l < 6; l++)
							{
								blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
							}
							sides += 4;
							vanrender = true;
						}
					}
					else if ((blockType[i][j][k + 1] == AIR || blockType[i][j][k + 1] >= WATER)&&blockType[i][j][k+1]!=GLASS_BLOCK)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(POSITIVE_Z, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(POSITIVE_Z, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}

					//negyedik oldal
					if (i == 0)
					{
						if (isTransparent[3][k][j] && !isGlass[3][k][j])
						{
							for (l = 0; l < 4; l++)
							{
								BlockDatabase::GetVertex(NEGATIVE_X, l, x, y, z, dankness);
								x += baseX + i;
								y += j;
								z += baseZ + k;
								dankness *= (lightLevel[i][j][k] / 100.0f);

								BlockDatabase::GetUv(NEGATIVE_X, l, blockType[i][j][k], uvx, uvy);

								blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
							}

							for (l = 0; l < 6; l++)
							{
								blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
							}
							sides += 4;
							vanrender = true;
						}
					}
					else if ((blockType[i - 1][j][k] == AIR || blockType[i - 1][j][k] >= WATER)&&blockType[i-1][j][k]!=GLASS_BLOCK)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(NEGATIVE_X, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(NEGATIVE_X, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}


					//otodik oldal
					if (j == 149)
					{

					}
					else if ((blockType[i][j + 1][k] == AIR || blockType[i][j + 1][k] >= WATER)&&blockType[i][j+1][k]!=GLASS_BLOCK)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(POSITIVE_Y, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(POSITIVE_Y, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}

					//hatodik oldal
					if (j == 0)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(NEGATIVE_Y, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(NEGATIVE_Y, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}
					else if ((blockType[i][j - 1][k] == AIR || blockType[i][j - 1][k] >= WATER)&&blockType[i][j-1][k]!=GLASS_BLOCK)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(NEGATIVE_Y, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(NEGATIVE_Y, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}

					if (vanrender)
					{
						vanrender = false;

						this->physics->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);
						if (k == 0)
							this->borderPhysics[0]->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);
						else if (k == 15)
							this->borderPhysics[2]->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);

						if (i == 15)
							this->borderPhysics[1]->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);
						else if (i == 0)
							this->borderPhysics[3]->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);
					}
				}
				else
				{
					vanrender = false;
					//elso oldal
					if (k == 0)
					{
						if (isTransparent[0][i][j])
						{
							for (l = 0; l < 4; l++)
							{
								BlockDatabase::GetVertex(NEGATIVE_Z, l, x, y, z, dankness);
								x += baseX + i;
								y += j;
								z += baseZ + k;
								dankness *= (lightLevel[i][j][k] / 100.0f);

								BlockDatabase::GetUv(NEGATIVE_Z, l, blockType[i][j][k], uvx, uvy);

								blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
							}

							for (l = 0; l < 6; l++)
							{
								blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
							}
							sides += 4;
							vanrender = true;
						}
					}
					else if (blockType[i][j][k - 1] == AIR || blockType[i][j][k - 1] >= WATER)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(NEGATIVE_Z, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(NEGATIVE_Z, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}

					//masodik oldal
					if (i == 15)
					{
						if (isTransparent[1][k][j])
						{
							for (l = 0; l < 4; l++)
							{
								BlockDatabase::GetVertex(POSITIVE_X, l, x, y, z, dankness);
								x += baseX + i;
								y += j;
								z += baseZ + k;
								dankness *= (lightLevel[i][j][k] / 100.0f);

								BlockDatabase::GetUv(POSITIVE_X, l, blockType[i][j][k], uvx, uvy);

								blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
							}

							for (l = 0; l < 6; l++)
							{
								blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
							}
							sides += 4;
							vanrender = true;
						}
					}
					else if (blockType[i + 1][j][k] == AIR || blockType[i + 1][j][k] >= WATER)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(POSITIVE_X, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(POSITIVE_X, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}

					//harmadik oldal
					if (k == 15)
					{
						if (isTransparent[2][i][j])
						{
							for (l = 0; l < 4; l++)
							{
								BlockDatabase::GetVertex(POSITIVE_Z, l, x, y, z, dankness);
								x += baseX + i;
								y += j;
								z += baseZ + k;
								dankness *= (lightLevel[i][j][k] / 100.0f);

								BlockDatabase::GetUv(POSITIVE_Z, l, blockType[i][j][k], uvx, uvy);

								blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
							}

							for (l = 0; l < 6; l++)
							{
								blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
							}
							sides += 4;
							vanrender = true;
						}
					}
					else if (blockType[i][j][k + 1] == AIR || blockType[i][j][k + 1] >= WATER)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(POSITIVE_Z, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(POSITIVE_Z, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}

					//negyedik oldal
					if (i == 0)
					{
						if (isTransparent[3][k][j])
						{
							for (l = 0; l < 4; l++)
							{
								BlockDatabase::GetVertex(NEGATIVE_X, l, x, y, z, dankness);
								x += baseX + i;
								y += j;
								z += baseZ + k;
								dankness *= (lightLevel[i][j][k] / 100.0f);

								BlockDatabase::GetUv(NEGATIVE_X, l, blockType[i][j][k], uvx, uvy);

								blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
							}

							for (l = 0; l < 6; l++)
							{
								blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
							}
							sides += 4;
							vanrender = true;
						}
					}
					else if (blockType[i - 1][j][k] == AIR || blockType[i - 1][j][k] >= WATER)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(NEGATIVE_X, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(NEGATIVE_X, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}


					//otodik oldal
					if (j == 149)
					{

					}
					else if (blockType[i][j + 1][k] == AIR || blockType[i][j + 1][k] >= WATER)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(POSITIVE_Y, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(POSITIVE_Y, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}

					//hatodik oldal
					if (j == 0)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(NEGATIVE_Y, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(NEGATIVE_Y, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}
					else if (blockType[i][j - 1][k] == AIR || blockType[i][j - 1][k] >= WATER)
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetVertex(NEGATIVE_Y, l, x, y, z, dankness);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(NEGATIVE_Y, l, blockType[i][j][k], uvx, uvy);

							blockMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							blockMesh->AddIndex(sides + BlockDatabase::GetIndex(l));
						}
						sides += 4;
						vanrender = true;
					}

					if (vanrender)
					{
						vanrender = false;

						this->physics->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);
						if (k == 0)
							this->borderPhysics[0]->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);
						else if (k == 15)
							this->borderPhysics[2]->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);

						if (i == 15)
							this->borderPhysics[1]->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);
						else if (i == 0)
							this->borderPhysics[3]->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);
					}
				}
			}
		}
	}

	//watermesh
	waterMesh->ResetMesh();
	int waterSides = 0;
	bool isOberWasser = false;

	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 150; j++)
		{
			for (k = 0; k < 16; k++)
			{
				if (blockType[i][j][k] != WATER)
					continue;

				//check if water is on top
				isOberWasser = false;
				if (j == 149)
					isOberWasser = true;
				else if (blockType[i][j + 1][k] != WATER)
					isOberWasser = true;

				//elso oldal
				if (k == 0)
				{
					if (isAir[0][i][j])
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetWaterVertex(NEGATIVE_Z, l, x, y, z, dankness, isOberWasser);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(NEGATIVE_Z, l, blockType[i][j][k], uvx, uvy);

							//waterMesh->AddVertex9(x, y, z,0.0f,0.0f,-1.0f, uvx, uvy, dankness);
							waterMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							waterMesh->AddIndex(waterSides + BlockDatabase::GetIndex(l));
						}
						waterSides += 4;
					}
				}
				else if (blockType[i][j][k - 1] == AIR)
				{
					for (l = 0; l < 4; l++)
					{
						BlockDatabase::GetWaterVertex(NEGATIVE_Z, l, x, y, z, dankness, isOberWasser);
						x += baseX + i;
						y += j;
						z += baseZ + k;
						dankness *= (lightLevel[i][j][k] / 100.0f);

						BlockDatabase::GetUv(NEGATIVE_Z, l, blockType[i][j][k], uvx, uvy);

						//waterMesh->AddVertex9(x, y, z, 0.0f, 0.0f, -1.0f, uvx, uvy, dankness);
						waterMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
					}

					for (l = 0; l < 6; l++)
					{
						waterMesh->AddIndex(waterSides + BlockDatabase::GetIndex(l));
					}
					waterSides += 4;
				}

				//masodik oldal
				if (i == 15)
				{
					if (isAir[1][k][j])
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetWaterVertex(POSITIVE_X, l, x, y, z, dankness, isOberWasser);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(POSITIVE_X, l, blockType[i][j][k], uvx, uvy);

							//waterMesh->AddVertex9(x, y, z, 1.0f, 0.0f, 0.0, uvx, uvy, dankness);
							waterMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							waterMesh->AddIndex(waterSides + BlockDatabase::GetIndex(l));
						}
						waterSides += 4;
					}
				}
				else if (blockType[i + 1][j][k] == AIR)
				{
					for (l = 0; l < 4; l++)
					{
						BlockDatabase::GetWaterVertex(POSITIVE_X, l, x, y, z, dankness, isOberWasser);
						x += baseX + i;
						y += j;
						z += baseZ + k;
						dankness *= (lightLevel[i][j][k] / 100.0f);

						BlockDatabase::GetUv(POSITIVE_X, l, blockType[i][j][k], uvx, uvy);

						//waterMesh->AddVertex9(x, y, z, 1.0f, 0.0f, 0.0, uvx, uvy, dankness);
						waterMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
					}

					for (l = 0; l < 6; l++)
					{
						waterMesh->AddIndex(waterSides + BlockDatabase::GetIndex(l));
					}
					waterSides += 4;
				}

				//harmadik oldal
				if (k == 15)
				{
					if (isAir[2][i][j])
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetWaterVertex(POSITIVE_Z, l, x, y, z, dankness, isOberWasser);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(POSITIVE_Z, l, blockType[i][j][k], uvx, uvy);

							//waterMesh->AddVertex9(x, y, z, 0.0f, 0.0f, 1.0, uvx, uvy, dankness);
							waterMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							waterMesh->AddIndex(waterSides + BlockDatabase::GetIndex(l));
						}
						waterSides += 4;
					}
				}
				else if (blockType[i][j][k + 1] == AIR)
				{
					for (l = 0; l < 4; l++)
					{
						BlockDatabase::GetWaterVertex(POSITIVE_Z, l, x, y, z, dankness, isOberWasser);
						x += baseX + i;
						y += j;
						z += baseZ + k;
						dankness *= (lightLevel[i][j][k] / 100.0f);

						BlockDatabase::GetUv(POSITIVE_Z, l, blockType[i][j][k], uvx, uvy);

						//waterMesh->AddVertex9(x, y, z, 0.0f, 0.0f, 1.0, uvx, uvy, dankness);
						waterMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
					}

					for (l = 0; l < 6; l++)
					{
						waterMesh->AddIndex(waterSides + BlockDatabase::GetIndex(l));
					}
					waterSides += 4;
				}

				//negyedik oldal
				if (i == 0)
				{
					if (isAir[3][k][j])
					{
						for (l = 0; l < 4; l++)
						{
							BlockDatabase::GetWaterVertex(NEGATIVE_X, l, x, y, z, dankness, isOberWasser);
							x += baseX + i;
							y += j;
							z += baseZ + k;
							dankness *= (lightLevel[i][j][k] / 100.0f);

							BlockDatabase::GetUv(NEGATIVE_X, l, blockType[i][j][k], uvx, uvy);

							//waterMesh->AddVertex9(x, y, z, -1.0f, 0.0f, 0.0, uvx, uvy, dankness);
							waterMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
						}

						for (l = 0; l < 6; l++)
						{
							waterMesh->AddIndex(waterSides + BlockDatabase::GetIndex(l));
						}
						waterSides += 4;
					}
				}
				else if (blockType[i - 1][j][k] == AIR)
				{
					for (l = 0; l < 4; l++)
					{
						BlockDatabase::GetWaterVertex(NEGATIVE_X, l, x, y, z, dankness, isOberWasser);
						x += baseX + i;
						y += j;
						z += baseZ + k;
						dankness *= (lightLevel[i][j][k] / 100.0f);

						BlockDatabase::GetUv(NEGATIVE_X, l, blockType[i][j][k], uvx, uvy);

						//waterMesh->AddVertex9(x, y, z, -1.0f, 0.0f, 0.0, uvx, uvy, dankness);
						waterMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
					}

					for (l = 0; l < 6; l++)
					{
						waterMesh->AddIndex(waterSides + BlockDatabase::GetIndex(l));
					}
					waterSides += 4;
				}


				//otodik oldal
				if (j == 149)
				{

				}
				else if (blockType[i][j + 1][k] == AIR)
				{
					for (l = 0; l < 4; l++)
					{
						BlockDatabase::GetWaterVertex(POSITIVE_Y, l, x, y, z, dankness, isOberWasser);
						x += baseX + i;
						y += j;
						z += baseZ + k;
						dankness *= (lightLevel[i][j][k] / 100.0f);

						BlockDatabase::GetUv(POSITIVE_Y, l, blockType[i][j][k], uvx, uvy);

						//waterMesh->AddVertex9(x, y, z, 0.0f, 1.0f, 0.0, uvx, uvy, dankness);
						waterMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
					}

					for (l = 0; l < 6; l++)
					{
						waterMesh->AddIndex(waterSides + BlockDatabase::GetIndex(l));
					}
					waterSides += 4;
				}

				//hatodik oldal
				if (j == 0)
				{
					for (l = 0; l < 4; l++)
					{
						BlockDatabase::GetWaterVertex(NEGATIVE_Y, l, x, y, z, dankness, isOberWasser);
						//y -= OBERWASSER_DEPTH_DIFFERENCE;
						x += baseX + i;
						y += j;
						z += baseZ + k;
						dankness *= (lightLevel[i][j][k] / 100.0f);

						BlockDatabase::GetUv(NEGATIVE_Y, l, blockType[i][j][k], uvx, uvy);

						//waterMesh->AddVertex9(x, y, z, 0.0f, -1.0f, 0.0, uvx, uvy, dankness);
						waterMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
					}

					for (l = 0; l < 6; l++)
					{
						waterMesh->AddIndex(waterSides + BlockDatabase::GetIndex(l));
					}
					waterSides += 4;
				}
				else if (blockType[i][j - 1][k] == AIR)
				{
					for (l = 0; l < 4; l++)
					{
						BlockDatabase::GetWaterVertex(NEGATIVE_Y, l, x, y, z, dankness, isOberWasser);
						//y -= OBERWASSER_DEPTH_DIFFERENCE;//hogy lejjebb legyen az alul hullamzo viz
						x += baseX + i;
						y += j;
						z += baseZ + k;
						dankness *= (lightLevel[i][j][k] / 100.0f);

						BlockDatabase::GetUv(NEGATIVE_Y, l, blockType[i][j][k], uvx, uvy);

						//waterMesh->AddVertex9(x, y, z, 0.0f, -1.0f, 0.0, uvx, uvy, dankness);
						waterMesh->AddVertex4(x, y, z, uvx, uvy, dankness);
					}

					for (l = 0; l < 6; l++)
					{
						waterMesh->AddIndex(waterSides + BlockDatabase::GetIndex(l));
					}
					waterSides += 4;
				}

				this->physics->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, false, true);
				
				/*if (k == 0)
					this->borderPhysics[0]->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);
				else if (k == 15)
					this->borderPhysics[2]->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);

				if (i == 15)
					this->borderPhysics[1]->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);
				else if (i == 0)
					this->borderPhysics[3]->AddAABB(chunkNumber[0] * 16 + i, j, chunkNumber[1] * 16 + k, true, false);*/
			}
		}
	}

	//std::cout << "AABB count: "<<physics->GetAABBCount(false) << std::endl;

	//setting up buffers
	if (sides != 0)
	{
		glGenVertexArrays(1, &blockVAO);
		glBindVertexArray(blockVAO);

		glGenBuffers(1, &blockVBO);
		glBindBuffer(GL_ARRAY_BUFFER, blockVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (blockMesh->GetVertexCount()), blockMesh->GetVerticesFront(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);//position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float)*3));//uv coords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 5));//darkness
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &blockEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blockEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * (blockMesh->GetIndexCount()), blockMesh->GetIndicesFront(), GL_STATIC_DRAW);

		elementCount_block = blockMesh->GetIndexCount();
	}
	else
	{
		blockVAO = 0;
		blockVBO = 0;
		blockEBO = 0;

		elementCount_block = 0;
	}


	if (waterSides != 0)
	{
		glGenVertexArrays(1, &waterVAO);
		glBindVertexArray(waterVAO);

		glGenBuffers(1, &waterVBO);
		glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (waterMesh->GetVertexCount()), waterMesh->GetVerticesFront(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);//position
		glEnableVertexAttribArray(0);
		/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(sizeof(float)*3));//normal
		glEnableVertexAttribArray(1);*/
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));//uv coords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 5));//darkness
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &waterEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * (waterMesh->GetIndexCount()), waterMesh->GetIndicesFront(), GL_STATIC_DRAW);

		elementCount_water = waterMesh->GetIndexCount();
	}
	else
	{
		waterVAO = 0;
		waterVBO = 0;
		waterEBO = 0;

		elementCount_water = 0;
	}
}

void Chunk::RenderChunk(bool waterMesh)
{
	glEnable(GL_DEPTH_TEST);
	if (!waterMesh&&blockVAO != 0)
	{
		glEnable(GL_CULL_FACE);
		glBindVertexArray(blockVAO);
		glDrawElements(GL_TRIANGLES, elementCount_block, GL_UNSIGNED_INT, NULL);
	}
	if (waterMesh&&waterVAO != 0)
	{
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glBindVertexArray(waterVAO);
		glDrawElements(GL_TRIANGLES, elementCount_water, GL_UNSIGNED_INT, NULL);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
	}
}

/*int Chunk::GetChunkNumber(int index)
{
	if (index < 0 || index>1)
		return 0;
	return chunkNumber[index];
}*/

int Chunk::RaycastMessenger(int x, int y, int z, bool isWater, bool shouldBeSolid)
{
	//return physics->GetAABB(x, y, z, isWater, shouldBeSolid);
	std::cout << "Raycast messenger is out of order" << std::endl;
	return 0;
}

int Chunk::GetBlockType(int x, int y, int z)
{
	if (x < 0)
		return 0;
	if (x > 15)
		return 0;
	if (y < 0)
		return 0;
	if (y > 149)
		return 0;
	if (z < 0)
		return 0;
	if (z > 15)
		return 0;

	return blockType[x][y][z];
}

void Chunk::ChangeBlock(int x, int y, int z, int type)
{
	if (x < 0)
		return;
	if (x > 15)
		return;
	if (y < 0)
		return;
	if (y > 149)
		return;
	if (z < 0)
		return;
	if (z > 15)
		return;

	blockType[x][y][z] = type;
}