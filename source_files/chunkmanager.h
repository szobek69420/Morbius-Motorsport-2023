#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include "chunk.h"
#include "terraingenerator.h"
#include <vector>
#include <glm/glm.hpp>
#include "my_vector.hpp"


class ChunkManager {
private:
	int renderDistance;
	int currentChunk[2];//x,z
	int seed;

	//egy lista, amely az adott osztalypeldany altal betolott chunkokra mutat
	gigachad::vector<Chunk*> loadedChunks;
	float terrainOffset[2];//x, z

	gigachad::vector<int> pendingChunks;//chunks to calculate (minden paratlanadik elem a chunkNumber[0], minden masodik a chunkNumber[1] (azaz 2 elem pro chunk) )

	//hogy teljesen chunkmanager-hez legyen kotve a terrain generation
	TerrainGenerator* terrainGenerator;

public:
	//a megvaltoztatott blokkokat tartalmazo lista
	gigachad::vector<struct ChangedBlock_struct> changedBlocks;

	static const float CHUNK_LOADING_SPEED;

public:
	ChunkManager(int RD=4, int seed=0);

	~ChunkManager();

	//megnezi, hogy a renderdistance-en belul van-e betoltetlen chunk, es ha igen, betolti
	void LoadChunks(float posX, float posZ);

	//megnezi, hogy a render distance-en kivul van-e betoltott chunk, es ha igen, torli
	void UnloadChunks(float posX, float posZ);

	//a pendingchunks alapjan frissit max egy chunkot
	void RecalculateChunks();
private:
	int RefreshChunk(int cn1, int cn2);//egy celzott recalculator az osztaly fuggvenyeinek
public:
	void AddPending(int chunkNumber1, int chunkNumber2);

	//a jatekostol vett tavolsag alapjan jeleniti meg a chunkokat
	void RenderChunks(bool waterMesh);

	//be van-e toltve ilyen szamu chunk
	bool ContainsChunkNumber(int x, int z);

	//melyik chunkban van a jatekos
	void PlayerChunkNumber(float posX, float posZ);

	int GetSeed();

	float GetTerrainOffset(int index);
	float GetTerrainHeight(float posx, float posz) { return terrainGenerator->GetTerrainHeight(posx, posz); }


	//regi, AABB alapu raycast
	int Raycast(glm::vec3 cameraPos, glm::vec3 cameraFront, float range, int& x, int& y, int& z, bool isWater, bool shouldBeSolid = false);
	
	//uj, blokktipus alapu raycast (megkeresi a player chunkjat, es azon keresztul meghiv egy raycastet
	int Raycast2(glm::vec3 cameraPos, glm::vec3 cameraFront, float range, int& x, int& y, int& z, bool isWater, bool shouldBeSolid = false);
	//a should be solid igazabol nem jo semmire xd
	bool Raycast2_unprecise(const glm::vec3& cameraPos, const glm::vec3& cameraFront, float range, int& x, int& y, int& z, bool isWater, bool shouldBeSolid = false);
	bool IsBlockThere(const glm::vec3& position, int& x, int& y, int& z, bool isWater, bool shouldBeSolid = false);


	//x, y, z are global positions
	void AddBlock(int x, int y, int z, int type);

	//megkeresi, hogy melyik chunkban van a jatekos, es meghivja az adott (meg a szomszedos) chunk fizikas fuggvenyet 
	Physics::CollisionType PhysicsUpdate(glm::vec3& pos, glm::vec3& velocity);
	//vizben van-e a kamera
	bool isPointSubmerged(const glm::vec3& _point);

	//static
	static void GetPlayerChunkNumber(glm::vec3 playerPosition, int& x, int& z);
	static void GetPlayerChunkNumber(float posX, float posZ, int& x, int& z);
	static void GetPlayerChunkNumber(int posX, int posZ, int& x, int& z);
};

#endif