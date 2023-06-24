#include "chunkmanager.h"
#include "chunk.h"
#include "blockdatabase.h"
#include "meinmath.h"
#include <iostream>
#include <random>
#include <ctime>
#include <cmath>

static const float RAYCAST_PRECISION = 0.005f;
static const float RAYCAST_PRECISION_UNPRECISE = 0.1f;

static bool CHUNKGENERATOR_AVAILABLE = true;

const float ChunkManager::CHUNK_LOADING_SPEED = 0.1f;

ChunkManager::ChunkManager(int RD, int seed)
{
	loadedChunks.clear();
	renderDistance = RD;
	currentChunk[0] = 0;
	currentChunk[1] = 0;

	srand(clock());
	seed = rand();
	srand(seed);

	terrainOffset[0] = 6.9f * rand();
	terrainOffset[1] = 6.9f * rand();

	pendingChunks.clear();
	changedBlocks.clear();

	terrainGenerator = new TerrainGenerator(seed);
}

ChunkManager::~ChunkManager()
{
	for (int i = 0; i < loadedChunks.size(); i++)
		delete loadedChunks[i];

	loadedChunks.clear();
	loadedChunks.resize(0);

	pendingChunks.clear();
	pendingChunks.resize(0);

	changedBlocks.clear();
	changedBlocks.resize(0);

	delete terrainGenerator;
}

void ChunkManager::LoadChunks(float posX, float posZ)
{
	if (CHUNKGENERATOR_AVAILABLE == false) return;
	CHUNKGENERATOR_AVAILABLE = false;

	ChunkManager::PlayerChunkNumber(posX, posZ);//a currentChunk beallitasa
	//std::cout << currentChunk[0] << " " << currentChunk[1] << " | "<<posX<<" "<<posZ<< std::endl;
	int i, j, k;

	for (i = 0; i <= renderDistance; i++)
	{
		for (j = currentChunk[0] - i; j <= currentChunk[0] + i; j++)
		{
			for (k = currentChunk[1] - i; k <= currentChunk[1] + i; k++)
			{
				if (!ContainsChunkNumber(j, k))
				{
					loadedChunks.push_back(new Chunk(j, k, (void*)this));
					loadedChunks[loadedChunks.size() - 1]->GenerateChunk();

					CHUNKGENERATOR_AVAILABLE = true;
					return;
				}
			}
		}
	}

	CHUNKGENERATOR_AVAILABLE = true;
}

void ChunkManager::UnloadChunks(float posX, float posZ)
{
	if (CHUNKGENERATOR_AVAILABLE == false) return;
	CHUNKGENERATOR_AVAILABLE = false;

	ChunkManager::PlayerChunkNumber(posX, posZ);//a currentChunk beallitasa

	for (int i = 0; i < loadedChunks.size(); i++)
	{
		if (cm_abs_int(currentChunk[0] - loadedChunks[i]->GetChunkNumber(0)) > renderDistance || cm_abs_int(currentChunk[1] - loadedChunks[i]->GetChunkNumber(1)) > renderDistance)
		{
			delete loadedChunks[i];
			loadedChunks.erase(loadedChunks.begin() + i);
			break;
		}
	}

	CHUNKGENERATOR_AVAILABLE = true;
}

void ChunkManager::RecalculateChunks()
{
	if (CHUNKGENERATOR_AVAILABLE == false) return;
	CHUNKGENERATOR_AVAILABLE = false;

	int i=0;
	int examined[2];
	bool exists = false;
	while (pendingChunks.size() > 0&&!exists)//addig megy, amig ki nem urul a pending, vagy nem talal olyan chunkot, amit tenyleg ujraszamolhat
	{
		examined[0] = pendingChunks[0];
		examined[1] = pendingChunks[1];
		exists = false;

		if (RefreshChunk(examined[0], examined[1]) == 0)//azaz sikerult torolni
			exists = true;
		//std::cout << "Recalculated: " << examined[0] << " " << examined[1] << std::endl;

		for (i = 0; i < pendingChunks.size(); i += 2)
		{
			if (examined[0] == pendingChunks[i] && examined[1] == pendingChunks[i + 1])
			{
				pendingChunks.erase(pendingChunks.begin() + i);
				pendingChunks.erase(pendingChunks.begin() + i);
				i -= 2;
			}
		}
	}

	CHUNKGENERATOR_AVAILABLE = true;
	return;
}

int ChunkManager::RefreshChunk(int cn1, int cn2)
{
	bool megvan = false;
	for (int i = 0; i < loadedChunks.size(); i ++)
	{
		if (loadedChunks[i]->GetChunkNumber(0) == cn1 && loadedChunks[i]->GetChunkNumber(1) == cn2)
		{
			loadedChunks[i]->GenerateChunk();
			megvan = true;
			break;
		}
	}
	
	if (megvan)
		return 0;
	else
		return -69;
}

void ChunkManager::AddPending(int chunkNumber1, int chunkNumber2)
{
	pendingChunks.push_back(chunkNumber1);
	pendingChunks.push_back(chunkNumber2);
}

void ChunkManager::RenderChunks(bool waterMesh)
{
	float* distance = new float[loadedChunks.size()];
	int* index = new int[loadedChunks.size()];
	int i, j;
	for (i = 0; i < loadedChunks.size(); i++)
	{
		distance[i] = (float)cm_abs_int(loadedChunks[i]->GetChunkNumber(0) - currentChunk[0]) + cm_abs_int(loadedChunks[i]->GetChunkNumber(1) - currentChunk[1]);
		index[i] = i;
	}

	//bubirendezes
	/*int temp;
	for (i = 0; i < loadedChunks.size(); i++)
	{
		for (j = 0; j < loadedChunks.size() - 1 - i; j++)
		{
			if (distance[j] < distance[j + 1])
			{
				temp = distance[j];
				distance[j] = distance[j + 1];
				distance[j + 1] = temp;

				temp = index[j];
				index[j] = index[j + 1];
				index[j + 1] = temp;
			}
		}
	}*/

	quicksort_physics(distance, index, 0, loadedChunks.size() - 1);

	//render
	for (i = 0; i < loadedChunks.size(); i++)
		loadedChunks[index[i]]->RenderChunk(waterMesh);


	delete[] index;
	delete[] distance;
}

bool ChunkManager::ContainsChunkNumber(int x, int z)
{
	for (int i = 0; i < loadedChunks.size(); i++)
	{
		if (x == loadedChunks[i]->GetChunkNumber(0) && z == loadedChunks[i]->GetChunkNumber(1))
			return true;
	}

	return false;
}


void ChunkManager::PlayerChunkNumber(float posX, float posZ)
{
	int x = 0, z = 0;
	x = (int)(posX / 16);
	if (posX < 0) x--;
	z = (int)(posZ / 16);
	if (posZ < 0) z--;

	currentChunk[0] = x;
	currentChunk[1] = z;
}

int ChunkManager::GetSeed()
{
	return seed;
}

float ChunkManager::GetTerrainOffset(int index)
{
	if (index > 1 || index < 0)
		return -69.0f;

	return terrainOffset[index];
}

int ChunkManager::Raycast(glm::vec3 cameraPos, glm::vec3 cameraFront, float range, int& x, int& y, int& z, bool isWater, bool shouldBeSolid)//0: semmi, 1: x-, 2: x+, 3: y-, 4: y+, 5: z-, 6: z+ (az x- azt ejelnti, hogy az eggyel kisebb x koordinataju helyre kell rakni)
{
	std::vector<int> nearbyChunks;//a jatekoshoz kozeli chunkok indexei a loadedChunks-ban
	int loaded = loadedChunks.size();
	int i, j, k;

	this->PlayerChunkNumber(cameraPos.x, cameraPos.z);//biztos, ami biztos

	
	//getting nearby chunks
	int examinedChunk[2];
	int shouldBeIncluded[2] = { 0,0 };//x,z-re, 0: ha nem, -1: ha negativ iranyba, 1: ha pozitiv iranyba
	if (cameraPos.x - range < 0)
		shouldBeIncluded[0] = -1;
	else if (cameraPos.x + range > 16.0)
		shouldBeIncluded[0] = 1;
	if (cameraPos.z - range < 0)
		shouldBeIncluded[1] = -1;
	else if (cameraPos.z + range > 16.0)
		shouldBeIncluded[1] = 1;

	for (i = 0; i < loaded; i++)
	{
		examinedChunk[0] = loadedChunks[i]->GetChunkNumber(0);
		examinedChunk[1] = loadedChunks[i]->GetChunkNumber(1);
		if (cm_abs_int(currentChunk[0] - examinedChunk[0]) > 1 || cm_abs_int(currentChunk[1] - examinedChunk[1]) > 2)
			continue;
		else
		{
			if (currentChunk[0] != examinedChunk[0] && currentChunk[0] + shouldBeIncluded[0] != examinedChunk[0])
				continue;
			if (currentChunk[1] != examinedChunk[1] && currentChunk[1] + shouldBeIncluded[1] != examinedChunk[1])
				continue;

			nearbyChunks.push_back(i);
		}
	}
	if (nearbyChunks.size() == 0)
		return 0;

	//it's raycastin' time
	float distance = 0.0f;
	bool plusz[3] = { false,false,false };//hogy tudjam, melyik iranybol jott a sugar
	if (glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), cameraFront) > 0) plusz[0] = true;
	if (glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), cameraFront) > 0) plusz[1] = true;
	if (glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), cameraFront) > 0) plusz[2] = true;

	cameraFront = glm::normalize(cameraFront);
	cameraFront *= RAYCAST_PRECISION;

	bool possibleBlock = false;//ha egy egesz koordinatat talalt
	bool AABBfound = false;
	int foundPos[3];

	//cameraPos und cameraFront atrakasa float tombokbe a kenyelem kedveert
	float cPos[3] = { cameraPos.x,cameraPos.y,cameraPos.z };
	float cDir[3] = { cameraFront.x, cameraFront.y, cameraFront.z };
	bool isPositive[3];//az adott koordinata pozitiv-e (mas a szamitas)

	
	//ok, it's actually raycastin' time
	int oldal = 0;//return value
	while (distance < range && !AABBfound)
	{
		possibleBlock = false;
		AABBfound = false;

		for (i = 0; i < 3; i++)
		{
			cPos[i] += cDir[i];
			isPositive[i] = cPos[i] > 0.0f ? true : false;
		}
		distance += RAYCAST_PRECISION;


		//searching for round coordinates (there are the block sides)
		for (i = 0; i < 3; i++)
		{
			//oldalat ernek-e (azert kell kulon pozitiv es negativ ertekekre nezni, mert (int)(-3.5f)==-3f, de (int)(3.5f)==3f
			if (isPositive[i] && fabs((cPos[i] - 0.5f) - (int)cPos[i]) < 0.01f)
			{
				for (j = 0; j < 3; j++)
				{
					if (j == i)
					{
						if (plusz[j])//az iranyvektor a pozitiv iranyba nez
						{
							foundPos[j] = (int)cPos[j] + 1;
							oldal = i * 2 + 1;
						}
						else
						{
							foundPos[j] = (int)cPos[j];
							oldal = i * 2 + 2;
						}
					}
					else if (isPositive[j])
						foundPos[j] = (int)(roundf(cPos[j]) + 0.01f);
					else
						foundPos[j] = (int)(roundf(cPos[j]) - 0.01f);
				}

				possibleBlock = true;
				break;
			}
			else if (!isPositive[i] && fabs((cPos[i] + 0.5f) - (int)cPos[i]) < 0.01f)
			{
				for (j = 0; j < 3; j++)
				{
					if (j == i)
					{
						if (plusz[j])//az iranyvektor a pozitiv iranyba nez
						{
							foundPos[j] = (int)cPos[j];
							oldal = i * 2 + 1;
						}
						else
						{
							foundPos[j] = (int)cPos[j] - 1;
							oldal = i * 2 + 2;
						}
					}
					else if (isPositive[j])
						foundPos[j] = (int)(roundf(cPos[j]) + 0.01f);
					else
						foundPos[j] = (int)(roundf(cPos[j]) - 0.01f);
				}

				possibleBlock = true;
				break;
			}
		}

		//if round coordinate is found, we check if there is actually an AABB
		if (possibleBlock)
		{
			//std::cout << foundpos[0] << " " << foundpos[1] << " " << foundpos[2] << std::endl;

			for (j = 0; j < nearbyChunks.size(); j++)
			{
				if (loadedChunks[nearbyChunks[j]]->RaycastMessenger(foundPos[0], foundPos[1], foundPos[2],isWater, shouldBeSolid))
				{
					x = foundPos[0];
					y = foundPos[1];
					z = foundPos[2];
					AABBfound = true;
					//std::cout << i << std::endl;
					break;
				}
			}
		}
	}

	nearbyChunks.clear();

	if (AABBfound) return oldal;
	else return 0;
}

int ChunkManager::Raycast2(glm::vec3 cameraPos, glm::vec3 cameraFront, float range, int& x, int& y, int& z, bool isWater, bool shouldBeSolid)//0: semmi, 1: x-, 2: x+, 3: y-, 4: y+, 5: z-, 6: z+ (az x- azt ejelnti, hogy az eggyel kisebb x koordinataju helyre kell rakni)
{
	std::vector<int> nearbyChunks;//a jatekoshoz kozeli chunkok indexei a loadedChunks-ban
	int loaded = loadedChunks.size();
	int i, j, k;

	this->PlayerChunkNumber(cameraPos.x, cameraPos.z);//biztos, ami biztos


	//getting nearby chunks
	int examinedChunk[2];
	int shouldBeIncluded[2] = { 0,0 };//x,z-re, 0: ha nem, -1: ha negativ iranyba, 1: ha pozitiv iranyba
	if (cameraPos.x-16*this->currentChunk[0] - range < 0)
		shouldBeIncluded[0] = -1;
	else if (cameraPos.x - 16 * this->currentChunk[0] + range > 16.0)
		shouldBeIncluded[0] = 1;
	if (cameraPos.z - 16 * this->currentChunk[1] - range < 0)
		shouldBeIncluded[1] = -1;
	else if (cameraPos.z - 16 * this->currentChunk[1] + range > 16.0)
		shouldBeIncluded[1] = 1;

	for (i = 0; i < loaded; i++)
	{
		examinedChunk[0] = loadedChunks[i]->GetChunkNumber(0);
		examinedChunk[1] = loadedChunks[i]->GetChunkNumber(1);
		if (cm_abs_int(currentChunk[0] - examinedChunk[0]) > 1 || cm_abs_int(currentChunk[1] - examinedChunk[1]) > 2)
			continue;
		else
		{
			if (currentChunk[0] != examinedChunk[0] && currentChunk[0] + shouldBeIncluded[0] != examinedChunk[0])
				continue;
			if (currentChunk[1] != examinedChunk[1] && currentChunk[1] + shouldBeIncluded[1] != examinedChunk[1])
				continue;

			nearbyChunks.push_back(i);
		}
	}
	if (nearbyChunks.size() == 0)
		return 0;

	//it's raycastin' time
	float distance = 0.0f;
	bool plusz[3] = { false,false,false };//hogy tudjam, melyik iranybol jott a sugar
	if (glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), cameraFront) > 0) plusz[0] = true;
	if (glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), cameraFront) > 0) plusz[1] = true;
	if (glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), cameraFront) > 0) plusz[2] = true;

	cameraFront = glm::normalize(cameraFront);
	cameraFront *= RAYCAST_PRECISION;

	bool possibleBlock = false;//ha egy egesz koordinatat talalt
	bool AABBfound = false;
	int foundPos[3];
	

	//cameraPos und cameraFront atrakasa float tombokbe a kenyelem kedveert
	float cPos[3] = { cameraPos.x,cameraPos.y,cameraPos.z };
	float cDir[3] = { cameraFront.x, cameraFront.y, cameraFront.z };
	bool isPositive[3];//az adott koordinata pozitiv-e (mas a szamitas)


	//ok, it's actually raycastin' time
	int oldal = 0;//return value
	while (distance < range && !AABBfound)
	{
		possibleBlock = false;
		AABBfound = false;

		for (i = 0; i < 3; i++)
		{
			cPos[i] += cDir[i];
			isPositive[i] = cPos[i] > 0.0f ? true : false;
		}
		distance += RAYCAST_PRECISION;


		//searching for round coordinates (there are the block sides)
		for (i = 0; i < 3; i++)
		{
			//oldalat ernek-e (azert kell kulon pozitiv es negativ ertekekre nezni, mert (int)(-3.5f)==-3f, de (int)(3.5f)==3f
			if (isPositive[i] && fabs((cPos[i] - 0.5f) - (int)cPos[i]) < 0.01f)
			{
				for (j = 0; j < 3; j++)
				{
					if (j == i)
					{
						if (plusz[j])//az iranyvektor a pozitiv iranyba nez
						{
							foundPos[j] = (int)cPos[j] + 1;
							oldal = i * 2 + 1;
						}
						else
						{
							foundPos[j] = (int)cPos[j];
							oldal = i * 2 + 2;
						}
					}
					else if (isPositive[j])
						foundPos[j] = (int)(roundf(cPos[j]) + 0.01f);
					else
						foundPos[j] = (int)(roundf(cPos[j]) - 0.01f);
				}

				possibleBlock = true;
				break;
			}
			else if (!isPositive[i] && fabs((cPos[i] + 0.5f) - (int)cPos[i]) < 0.01f)
			{
				for (j = 0; j < 3; j++)
				{
					if (j == i)
					{
						if (plusz[j])//az iranyvektor a pozitiv iranyba nez
						{
							foundPos[j] = (int)cPos[j];
							oldal = i * 2 + 1;
						}
						else
						{
							foundPos[j] = (int)cPos[j] - 1;
							oldal = i * 2 + 2;
						}
					}
					else if (isPositive[j])
						foundPos[j] = (int)(roundf(cPos[j]) + 0.01f);
					else
						foundPos[j] = (int)(roundf(cPos[j]) - 0.01f);
				}

				possibleBlock = true;
				break;
			}
		}

		//if round coordinate is found, we check if there is actually an AABB
		int type = 0;
		if (possibleBlock)
		{
			//std::cout << foundpos[0] << " " << foundpos[1] << " " << foundpos[2] << std::endl;
			
			for (j = 0; j < nearbyChunks.size(); j++)
			{
				if ((type = loadedChunks[nearbyChunks[j]]->GetBlockType(foundPos[0] - 16 * loadedChunks[nearbyChunks[j]]->GetChunkNumber(0), foundPos[1], foundPos[2] - 16 * loadedChunks[nearbyChunks[j]]->GetChunkNumber(1))))
				{
					if (!isWater && type != WATER)
					{
						x = foundPos[0];
						y = foundPos[1];
						z = foundPos[2];
						AABBfound = true;
						//std::cout << i << std::endl;
						break;
					}
				}
			}
		}
	}

	nearbyChunks.clear();

	if (AABBfound) return oldal;
	else return 0;
}

bool ChunkManager::Raycast2_unprecise(const glm::vec3& cameraPos, const glm::vec3& cameraFront, float range, int& x, int& y, int& z, bool isWater, bool shouldBeSolid)
{
	unsigned int _chunkcount = loadedChunks.size();

	float currentPos[3];
	currentPos[0] = cameraPos.x;
	currentPos[1] = cameraPos.y;
	currentPos[2] = cameraPos.z;


	float deltaLength = glm::length(cameraFront);
	deltaLength *= RAYCAST_PRECISION_UNPRECISE;

	float deltaFront[3];
	deltaFront[0] = deltaLength*cameraFront.x;
	deltaFront[1] = deltaLength*cameraFront.y;
	deltaFront[2] = deltaLength*cameraFront.z;

	float distance = 0.0;
	bool chunkFound = true;
	bool blockFound = false;

	int _chunkNumber[2] = { 0,0 };
	int localBlockPosition[3];
	while (distance < range&&!blockFound)
	{
		chunkFound = false;

		for (int i = 0; i < 3; i++)
			currentPos[i] += deltaFront[i];
		distance += RAYCAST_PRECISION_UNPRECISE;

		//converting data
		ChunkManager::GetPlayerChunkNumber(currentPos[0], currentPos[2], _chunkNumber[0], _chunkNumber[1]);
	
		localBlockPosition[0] = (int)roundf(currentPos[0] - 16.0f * _chunkNumber[0]);
		localBlockPosition[1] = (int)roundf(currentPos[1]);
		localBlockPosition[2] = (int)roundf(currentPos[2] - 16.0f * _chunkNumber[1]);
		
		//searching for chunk
		for (int i = 0; i < _chunkcount; i++)
		{
			if (loadedChunks[i]->GetChunkNumber(0) == _chunkNumber[0] && loadedChunks[i]->GetChunkNumber(1) == _chunkNumber[1])//chunk found
			{
				chunkFound = true;

				int _blockType = loadedChunks[i]->GetBlockType(localBlockPosition[0], localBlockPosition[1], localBlockPosition[2]);

				if (_blockType == WATER)
				{
					if (isWater) blockFound = true;
				}
				else if (_blockType != AIR)
					blockFound = true;

				break;
			}
		}
	}

	if (blockFound)
	{
		//std::cout << _chunkNumber[0] << " " << _chunkNumber[1] << std::endl;

		x = (int)roundf(currentPos[0]);
		y = (int)roundf(currentPos[1]);
		z = (int)roundf(currentPos[2]);

		return true;
	}
	else return false;
}

bool ChunkManager::IsBlockThere(const glm::vec3& position, int& x, int& y, int& z, bool isWater, bool shouldBeSolid)
{
	unsigned int _chunkcount = loadedChunks.size();

	float currentPos[3];
	currentPos[0] = position.x;
	currentPos[1] = position.y;
	currentPos[2] = position.z;

	bool blockFound = false;

	int _chunkNumber[2] = { 0,0 };
	int localBlockPosition[3];

	//converting data
	ChunkManager::GetPlayerChunkNumber(currentPos[0], currentPos[2], _chunkNumber[0], _chunkNumber[1]);

	localBlockPosition[0] = (int)roundf(currentPos[0] - 16.0f * _chunkNumber[0]);
	localBlockPosition[1] = (int)roundf(currentPos[1]);
	localBlockPosition[2] = (int)roundf(currentPos[2] - 16.0f * _chunkNumber[1]);

	//searching for chunk
	for (int i = 0; i < _chunkcount; i++)
	{
		if (loadedChunks[i]->GetChunkNumber(0) == _chunkNumber[0] && loadedChunks[i]->GetChunkNumber(1) == _chunkNumber[1])//chunk found
		{
			int _blockType = loadedChunks[i]->GetBlockType(localBlockPosition[0], localBlockPosition[1], localBlockPosition[2]);

			if (_blockType == WATER)
			{
				if (isWater) blockFound = true;
			}
			else if (_blockType != AIR)
				blockFound = true;

			break;
		}
	}

	if (blockFound)
	{
		//std::cout << _chunkNumber[0] << " " << _chunkNumber[1] << std::endl;

		x = (int)roundf(currentPos[0]);
		y = (int)roundf(currentPos[1]);
		z = (int)roundf(currentPos[2]);

		return true;
	}
	else return false;
}

void ChunkManager::AddBlock(int x, int y, int z, int type)
{
	int chunkNummahs[2];

	ChunkManager::GetPlayerChunkNumber(x, z, chunkNummahs[0], chunkNummahs[1]);
	x -= 16 * chunkNummahs[0];
	z -= 16 * chunkNummahs[1];
	//std::cout << chunkNummahs[0] << " " << chunkNummahs[1] << std::endl;
	/*for (int i = 0; i < loadedChunks.size(); i++)
	{
		if (chunkNummahs[0] == loadedChunks[i]->GetChunkNumber(0) && chunkNummahs[1] == loadedChunks[i]->GetChunkNumber(1))
		{
			//loadedChunks[i]->ChangeBlock(x, y, z, type);
			struct ChangedBlock_struct temp;
			temp.type = type;
			temp.localPos[0] = x;
			temp.localPos[1] = y;
			temp.localPos[2] = z;
			temp.chunkNumber[0] = chunkNummahs[0];
			temp.chunkNumber[1] = chunkNummahs[1];

			changedBlocks.push_back(temp);
			this->AddPending(chunkNummahs[0], chunkNummahs[1]);
			
			break;
		}
	}*/
	//a for azert van kommentben, mert nem valoszinu, hogy egy nem betoltott chunkra epit a jatekos
	struct ChangedBlock_struct temp;
	temp.type = type;
	temp.localPos[0] = x;
	temp.localPos[1] = y;
	temp.localPos[2] = z;
	temp.chunkNumber[0] = chunkNummahs[0];
	temp.chunkNumber[1] = chunkNummahs[1];

	changedBlocks.push_back(temp);
	this->AddPending(chunkNummahs[0], chunkNummahs[1]);
	//ha pont chunk hatar mellett utotte ki a jatekos a blokkot, akkor a kornyezo chunkot is ujraszamolja
	if (x == 0)
		this->AddPending(chunkNummahs[0] - 1, chunkNummahs[1]);
	else if (x == 15)
		this->AddPending(chunkNummahs[0] + 1, chunkNummahs[1]);
	if (z == 0)
		this->AddPending(chunkNummahs[0], chunkNummahs[1]-1);
	else if (z == 15)
		this->AddPending(chunkNummahs[0], chunkNummahs[1]+1);

}

Physics::CollisionType ChunkManager::PhysicsUpdate(glm::vec3& pos, glm::vec3& velocity)
{
	Physics::CollisionType returnValue = Physics::COLLISION_NONE;
	Physics::CollisionType temp = Physics::COLLISION_NONE;

	int chunk[2];
	int chunk_loaded[2];

	int length;
	ChunkManager::GetPlayerChunkNumber(pos.x+0.5f,pos.z+0.5f, chunk[0], chunk[1]);

	length = this->loadedChunks.size();
	for (int i = 0; i < length; i++)
	{
		chunk_loaded[0] = this->loadedChunks[i]->GetChunkNumber(0);
		chunk_loaded[1] = this->loadedChunks[i]->GetChunkNumber(1);

		if (chunk_loaded[0] == chunk[0])
		{
			if (chunk_loaded[1] == chunk[1])
			{
				temp = this->loadedChunks[i]->UpdatePhysics(pos, velocity, PHYSICS_ALL);
				returnValue = temp != Physics::COLLISION_NONE&&returnValue!=Physics::COLLISION_EDGE ? temp : returnValue;//azert a van bent a returnValue!=Physics::COLLISION_EDGE, mert az a legmagasabb prioritasu
				continue;
			}
			else if (chunk[1] - chunk_loaded[1] == 1)//-z (ennek kell a +z fele nezo oldala)
			{
				temp = this->loadedChunks[i]->UpdatePhysics(pos, velocity, PHYSICS_POSITIVE_Z);
				returnValue = temp != Physics::COLLISION_NONE && returnValue != Physics::COLLISION_EDGE ? temp : returnValue;
				continue;
			}
			else if (chunk[1] - chunk_loaded[1] == -1)//+z
			{
				this->loadedChunks[i]->UpdatePhysics(pos, velocity, PHYSICS_NEGATIVE_Z);
				returnValue = temp != Physics::COLLISION_NONE && returnValue != Physics::COLLISION_EDGE ? temp : returnValue;
				continue;
			}
		}
		else if (chunk_loaded[1] == chunk[1])
		{
			if (chunk_loaded[0] == chunk[0])
			{
				this->loadedChunks[i]->UpdatePhysics(pos, velocity, PHYSICS_ALL);
				returnValue = temp != Physics::COLLISION_NONE && returnValue != Physics::COLLISION_EDGE ? temp : returnValue;
				continue;
			}
			else if (chunk[0] - chunk_loaded[0] == 1)//-x
			{
				this->loadedChunks[i]->UpdatePhysics(pos, velocity, PHYSICS_POSITIVE_X);
				returnValue = temp != Physics::COLLISION_NONE && returnValue != Physics::COLLISION_EDGE ? temp : returnValue;
				continue;
			}
			else if (chunk[0] - chunk_loaded[0] == -1)//+x
			{
				this->loadedChunks[i]->UpdatePhysics(pos, velocity, PHYSICS_NEGATIVE_X);
				returnValue = temp != Physics::COLLISION_NONE && returnValue != Physics::COLLISION_EDGE ? temp : returnValue;
				continue;
			}
		}
	}


	return returnValue;
	//std::cout << "physics done\n" << std::endl;
}

bool ChunkManager::isPointSubmerged(const glm::vec3& _point)
{
	int chunk[2];
	int length = loadedChunks.size();
	ChunkManager::GetPlayerChunkNumber(_point.x + 0.5f, _point.z + 0.5f, chunk[0], chunk[1]);

	for (int i = 0; i < length; i++)
	{
		if (loadedChunks[i]->GetChunkNumber(0) == chunk[0] && loadedChunks[i]->GetChunkNumber(1) == chunk[1])
		{
			return loadedChunks[i]->isPointSubmerged(_point);
		}
	}

	return false;
}


//static
void ChunkManager::GetPlayerChunkNumber(glm::vec3 playerPosition, int& x, int& z)
{
	x = (int)(playerPosition.x / 16);
	if (playerPosition.x < 0) x--;
	z = (int)(playerPosition.z / 16);
	if (playerPosition.z < 0) z--;
}

void ChunkManager::GetPlayerChunkNumber(float posX, float posZ, int& x, int& z)
{
	x = (int)(posX / 16);
	if (posX < 0) x--;
	z = (int)(posZ / 16);
	if (posZ < 0) z--;
}

void ChunkManager::GetPlayerChunkNumber(int posX, int posZ, int& x, int& z)
{
	x = posX / 16;
	if (posX < 0&&posX%16!=0) x--;
	z = posZ / 16;
	if (posZ < 0&&posZ%16!=0) z--;
}