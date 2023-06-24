#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "hand.h"
#include "shader.h"
#include "blockdatabase.h"
#include "mesh.h"
#include "camera.h"

Hand::Hand()
{
	shader = createShader(SHADER_HELD_ITEM);
	glGenVertexArrays(1, &vao);
	vbo = 0;
	ebo = 0;
	held_block = AIR;
	vertexCount = 0;
	offset = glm::vec3(0.0);
}

Hand::~Hand()
{
	if (held_block != AIR)
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		vbo = 0;
		ebo = 0;
	}
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shader);
	held_block = AIR;
}

void Hand::SetHeldBlock(enum blocks block)
{
	if (block == held_block)
		return;
	if (block == AIR)//ha itt van, akkor eddig biztos nem volt ures a keze
	{
		held_block = AIR;
		glBindVertexArray(vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		vbo = 0;
		ebo = 0;
		vertexCount = 0;
	}
	else
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		vbo = 0;
		ebo = 0;

		held_block = block;
		bool isCube = Hand::isCubeMesh(block);

		Mesh_NotAssimp mesh;
		float x, y, z, uvx, uvy, dankness;
		if (isCube)//kockat rak ossze
		{
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					BlockDatabase::GetVertex(NEGATIVE_Z+i, j, x, y, z, dankness);
					BlockDatabase::GetUv(NEGATIVE_Z + i, j, block, uvx, uvy);
					mesh.AddVertex4(x, y, z, uvx, uvy, dankness);
				}
				for (int j=0;j<6;j++)
				{
					mesh.AddIndex(i*4 + BlockDatabase::GetIndex(j));
				}
			}

			vertexCount = 36;
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					BlockDatabase::GetFlowerVertex(i, j, x, y, z, dankness);
					BlockDatabase::GetUv(i, j, block, uvx, uvy);
					mesh.AddVertex4(x, y, z, uvx, uvy, dankness);
				}
				for (int j = 0; j < 6; j++)
				{
					mesh.AddIndex(i * 4 + BlockDatabase::GetIndex(j));
				}
			}

			vertexCount = 24;
		}

		glBindVertexArray(vao);
		
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.GetVertexCount(), mesh.GetVerticesFront(), GL_STATIC_DRAW);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);//position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));//uv coords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 5));//darkness
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * (mesh.GetIndexCount()), mesh.GetIndicesFront(), GL_STATIC_DRAW);
	
		glBindVertexArray(0);
	}
}

void Hand::SetOffset(float x, float y, float z)
{
	offset = glm::vec3(x, y, z);
}

void Hand::Render(GLuint texture, float aspectRatio)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0), offset);
	model = glm::rotate(model, glm::radians(8.0f), glm::vec3(0.0, 1.0, 0.0));
	model = glm::scale(model, glm::vec3(0.07, 0.07, 0.07));

	glm::mat4 combined = Camera::GetProjectionMatrix(false, glm::radians(90.0), aspectRatio, 0.05, 10.0);
	combined = combined * glm::lookAt(glm::vec3(0.0),glm::vec3(0.0,0.0,1.0),glm::vec3(0.0,1.0,0.0));
	combined = combined * model;

	glUseProgram(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader, "pvm"), 1, false, glm::value_ptr(combined));
	
	glUniform1i(glGetUniformLocation(shader, "atlas"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(0);

	glBindVertexArray(vao);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);
}