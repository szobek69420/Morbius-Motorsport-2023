#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <glm/glm.hpp>
#include "camera.h"
#include "my_vector.hpp"

const float PHYSICS_TIME_STEP = 0.0166f;

//egy AABB-t leiro adatok
struct aabb_block {
public:
	float worldPos[3];
	bool isSolid;
	bool isWater;
};

class Physics
{
private:
	//melyik blokkhoz tartozik
	int chunkNumber[2];

	gigachad::vector<aabb_block> blockAABBs;
	gigachad::vector<aabb_block> waterAABBs;

public:
	Physics()
	{
		blockAABBs.clear();
		waterAABBs.clear();
	}

	~Physics()
	{
		blockAABBs.clear();
		waterAABBs.clear();
	}

	enum CollisionType {
		COLLISION_NONE,//nincs utkozes
		COLLISION_EDGE,//oldalrol utkozott blokknak
		COLLISION_FLOOR,//fentrol utkozott blokknak
		COLLISION_CEILING//lentrol utkozott blokknak
	};

	//a physics osztalyhoz AABB-t hozzaado fuggveny overloadjai
	void AddAABB(glm::vec3 position,  bool isSolid, bool isWater=false);
	void AddAABB(int x, int y, int z, bool isSolid, bool isWater=false);
	void AddAABB(float x, float y, float z, bool isSolid, bool isWater = false);

	//torli az osszes adatot
	void ResetAABBs();

	unsigned int GetAABBCount(bool water);

	//utkozik-e a jatekos valamelyik AABB-vel, es ha igen, akkor megvaltoztatja a poziciojat, hogy ne utkozzon (egy fuggvenyhivassal nem biztos, hogy csak egy poziciovaltas fog tortenni)
	//ha tobb utkozes van, akkor a prioritasi sorrend: COLLISION_EDGE, COLLISION_FLOOR, COLLISION_CEILING, COLLISION_NONE
	enum CollisionType PlayerCollisionDetection(glm::vec3& pos, glm::vec3& velocity);
	bool isPointSubmerged(const glm::vec3& _point);

	//static
	static struct aabb_block CreateAABB(glm::vec3 position, bool isSolid, bool isWater=false);
	static struct aabb_block CreateAABB(int x, int y, int z, bool isSolid, bool isWater = false);//automatikusan blokkot keszit
	static bool isIntersecting(glm::vec3 playerPos, glm::vec3 blockPos);//csekkolja, hogy a block es a jatekos erintkezik-e
};

//a blokk-kijelolest (a kis feher keret a blokk korul, amelyre a kurzor mutat) kezelo fuggveny
class SelectionRenderer
{
private:
	unsigned int selectionVAO, selectionVBO, selectionEBO;
	unsigned int shaderProgram;

public:
	//betolti a szukseges adatokat a videomemoriaba es meghivja a szukseges shadert
	SelectionRenderer();
	~SelectionRenderer();

	//megjelenito fuggveny, amely egy world space positiont es egy kombomatrixot (projection*view) var
	void Render(glm::vec3 pos, glm::mat4 comboed);
};

#endif