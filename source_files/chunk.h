#ifndef CHUNK_H
#define CHUNK_H

#include "mesh.h"
#include "physics.h"
#include <vector>

//egy chunkhoz tartozo collidereket 5 kulonallo Physics osztalypeldanyban tarolom tarolom, hogy gyorsitsam a keresest physics update-kor
enum PhysicsUpdate_Mode {
	PHYSICS_NEGATIVE_Z,//az adott osztaly a chunknak csak azokat az AABB-it tartalmazza, amelyek a negativ Z fele mutato oldalan helyezkednek el a chunknak
	PHYSICS_POSITIVE_X,//az adott osztaly a chunknak csak azokat az AABB-it tartalmazza, amelyek a pozitiv X fele mutato oldalan helyezkednek el a chunknak
	PHYSICS_POSITIVE_Z,//az adott osztaly a chunknak csak azokat az AABB-it tartalmazza, amelyek a pozitiv Z fele mutato oldalan helyezkednek el a chunknak
	PHYSICS_NEGATIVE_X,//az adott osztaly a chunknak csak azokat az AABB-it tartalmazza, amelyek a negativ X fele mutato oldalan helyezkednek el a chunknak
	PHYSICS_ALL//az adott osztaly a chunk osszes AABB-jet tartalmazza
};

//a chunkot vezerlo chunkmanagernek van egy listaja, amely a valtoztatott blokkokat ezekben a rekordokban tarolja
struct ChangedBlock_struct {
	int type;
	int chunkNumber[2];
	int localPos[3];

	/*ChangedBlock(int type, int cn1, int cn2, int lp1, int lp2, int lp3)
	{
		this->type = type;
		this->chunkNumber[0] = cn1;
		this->chunkNumber[1] = cn2;
		this->localPos[0] = lp1;
		this->localPos[1] = lp2;
		this->localPos[2] = lp3;
	}*/
};

class Chunk {
private:
	int chunkNumber[2];//=pos/16
	unsigned char heightmap[16][16];//x,z
	unsigned char blockType[16][150][16];//x,y,z
	unsigned char lightLevel[16][150][16];
	bool isAir[4][16][150];//a kornyezo chunkok elso index: -z=0, +x=1, +z=2, -x=3 (mint a blockoknal)
	bool isWater[4][16][150];
	bool isGlass[4][16][150];

	//isTransparent[i][j][k]==true, ha isAir[i][j][k]||isWater[i][j][k]||isGlass[i][j][k]
	bool isTransparent[4][16][150];

	//a blockmesh (dolgok, melyekhez nem kell blending) megjelenitesi adatait tartalmazo valtozok
	Mesh_NotAssimp* blockMesh;
	unsigned int blockVAO, blockVBO, blockEBO;
	unsigned int elementCount_block;

	//a watermesh (dolgok, melyekhez kell blending) megjelenitesi adatait tartalmazo valtozok
	Mesh_NotAssimp* waterMesh;
	unsigned int waterVAO, waterVBO, waterEBO;
	unsigned int elementCount_water;

	//az osszes es csak az oldalso AABB-ket tartalmazo physics osztalyok
	Physics* physics;
	Physics* borderPhysics[4];//-z, +x, +z, -x

	//a chunkot iranyito chunkmanager-re mutato void*
	void* chunkManager;

public:
	//kezdeti adatok beallitasa
	Chunk(int X, int Z, void* cm);

	//minden memoriafoglalas kitorlese (c++ es opengl oldalrol)
	~Chunk();


	//felepiti a chunkot a kovetkezok szerint:

	//domborzat magassaganak lekerdezese (terraingenerator.h)
	//szomszedos chunkok oldalainak megvizsgalasa (magassag, atlatszosag)
	//domborzat (blokkok) felepitese a magassag, tengerszint es hohatar alapjan
	//novenyzet lerakasa (ez random, de reprodukalhato)
	//miutan mar nem valtoztat a chunk blokkjain a generalo, megnezi, hogy volt-e mar valtoztatas az adott chunkban, es ha igen, a megfelelo blokkokat kicsereli
	//megvilagitottsag kiszamolasa (ketlepcsos)
	//mesh halok (eloszor blockmesh, utana watermesh) felepitese az alapjan, hogy melyik blokknak melyik oldala latszik
	//ha egy blokknak latszik legalabb egy oldala, akkor a helyere egy AABB-t is rak
	//a felepitett mesh halok atkuldese a videokartya memoriajaba
	void GenerateChunk();

	//beallitja a megfelelo dolgokat az opengl-ben es utana render
	void RenderChunk(bool waterMesh);

	int GetChunkNumber(int index)//sokszor van meghivva, szoval lehet inline
	{
		if (index < 0 || index>1)
			return 0;
		return chunkNumber[index];
	}

	//a regi, AABB alapu raycast ezen keresztul kerdezte le, hogy az adott blokknal van-e AABB
	int RaycastMessenger(int x, int y, int z, bool isWater, bool shouldBeSolid = false);

	//az uj, gyorsabb, blokktipus alapu raycasthez kell
	int GetBlockType(int x, int y, int z);

	//ez egyertelmu
	void ChangeBlock(int x, int y, int z, int type);

	//egy atvezeto fuggveny az adott physics osztalypeldanyokhoz, az utkozes detektalashoz es megszunteteshez kell (a referenciaval atadott parametereken valtoztathat)
	Physics::CollisionType UpdatePhysics(glm::vec3& pos, glm::vec3& velocity, int mode) 
	{
		Physics::CollisionType returnValue = Physics::COLLISION_NONE;
		
		switch (mode)
		{
		case PHYSICS_ALL:
			returnValue = physics->PlayerCollisionDetection(pos, velocity);
			break;

		case PHYSICS_NEGATIVE_Z:
			returnValue = borderPhysics[0]->PlayerCollisionDetection(pos, velocity);
			break;

		case PHYSICS_POSITIVE_X:
			returnValue = borderPhysics[1]->PlayerCollisionDetection(pos, velocity);
			break;

		case PHYSICS_POSITIVE_Z:
			returnValue = borderPhysics[2]->PlayerCollisionDetection(pos, velocity);
			break;

		case PHYSICS_NEGATIVE_X:
			returnValue = borderPhysics[3]->PlayerCollisionDetection(pos, velocity);
			break;

		default:
			break;
		}

		return returnValue;
	}

	//a waterAABB-kkel kiszamolja, hogy vizben van-e a kamera
	bool isPointSubmerged(const glm::vec3& _point)
	{
		return physics->isPointSubmerged(_point);
	}
};

#endif