#ifndef HAND_H
#define HAND_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include "blockdatabase.h"
#include "camera.h"

class Hand {
private:
	//render valtozok
	unsigned int shader;
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;

	//a tartott blokk tipusa
	enum blocks held_block;

	//a tartott blokknak hany csucsa van (pl. viragnak kevesebb van, mint nether reactor core-nak)
	int vertexCount;

	//a kameratol vett poziciokulonbseg
	glm::vec3 offset;

public:
	Hand();
	~Hand();

	void SetHeldBlock(enum blocks block);
	enum blocks GetHeldBlock() { return held_block; }

	void SetOffset(float x, float y, float z);
	const glm::vec3& GetOffset() { return offset; }


	void Render(GLuint texture, float aspectRatio);

	static bool isCubeMesh(enum blocks block)//a visszateresi erteke igaz, ha kocka, hamis, ha olyan a mesh-je, mint egy viragnak
	{
		switch (block)
		{
		case RED_FLOWER:
			return false;
		case BLUE_FLOWER:
			return false;
		case YELLOW_FLOWER:
			return false;
		default:
			return true;
		}
	}
};


#endif