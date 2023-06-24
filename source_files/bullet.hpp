#ifndef BULLET_HPP
#define BULLET_HPP

#include <glm/glm.hpp>
#include "my_vector.hpp"
#include "sprite.hpp"
#include "chunkmanager.h"


class Bullet {
private:
	glm::vec3 position;
	glm::vec3 direction;//normalized

	static const float BULLET_SPEED;

public:
	Bullet()
	{
		position = glm::vec3(0.0);
		direction = glm::vec3(0.0);
	}
	Bullet(const glm::vec3& _position, const glm::vec3& _direction)
		:position(_position), direction(_direction){}

	~Bullet() {}

	void SetPosition(const glm::vec3& _position) { position = _position; }
	void SetDirection(const glm::vec3& _direction) { direction = _direction; }
	glm::vec3& GetPosition() { return position; }
	glm::vec3& GetDirection() { return direction; }


	void Step(int _stepValue = 1) { position += (float)_stepValue * direction; }
};

const float Bullet::BULLET_SPEED = 0.1f;


class BulletContainer
{
private:
	static const size_t MAX_BULLET_COUNT = 50;
	static const size_t MAX_STEP_COUNT = 500;
	Bullet* activeBullets[MAX_BULLET_COUNT];
	size_t stepCount[MAX_BULLET_COUNT];//az adott helyen levo loszer hany frissitesen van tul
	

public:
	BulletContainer()
	{
		for (size_t i = 0; i < MAX_BULLET_COUNT; i++)
		{
			activeBullets[i] = NULL;
			stepCount[i] = 0;
		}
	}
	~BulletContainer()
	{
		for (size_t i = 0; i < MAX_BULLET_COUNT; i++)
			delete activeBullets[i];
	}

	void AddBullet(const glm::vec3& _position, const glm::vec3& _direction)
	{
		bool isThereFreePlace = false;
		for (size_t i = 0; i < MAX_BULLET_COUNT; i++)
		{
			if (activeBullets[i] == NULL)
			{
				isThereFreePlace = true;
				activeBullets[i] = new Bullet(_position, _direction);
				stepCount[i] = 0;
				break;
			}
		}

		if (!isThereFreePlace)
		{
			size_t _maxIndex = 0;
			size_t _maxStepCount = 0;
			for (size_t i = 0; i < MAX_BULLET_COUNT; i++)
			{
				if (stepCount[i] > _maxStepCount)
				{
					_maxStepCount = stepCount[i];
					_maxIndex = i;
				}
			}

			delete activeBullets[_maxIndex];
			activeBullets[_maxIndex] = new Bullet(_position, _direction);
			stepCount[_maxIndex] = 0;
		}
	}

	void StepBullets(ChunkManager* _cm)
	{
		int x, y, z;

		for (size_t i = 0; i < MAX_BULLET_COUNT; i++)
		{
			if (activeBullets[i] == NULL)
				continue;

			stepCount[i]++;
			if (stepCount[i] > MAX_STEP_COUNT)
			{
				delete activeBullets[i];
				stepCount[i] = 0;
				activeBullets[i] = NULL;
				continue;
			}

			activeBullets[i]->Step();
			if (_cm->IsBlockThere(activeBullets[i]->GetPosition(), x, y, z, false))
			{
				_cm->AddBlock(x, y, z, AIR);

				delete activeBullets[i];
				stepCount[i] = 0;
				activeBullets[i] = NULL;
			}
		}
	}

	void RenderBullets(Billboard& bulletSprite, const glm::mat4& _view, const glm::mat4& _projection)
	{
		for (size_t i = 0; i < MAX_BULLET_COUNT; i++)
		{
			if (activeBullets[i] == NULL)
				continue;

			bulletSprite.SetPosition(activeBullets[i]->GetPosition());
			bulletSprite.RefreshRotation();
			bulletSprite.Render(_view, _projection);
		}
	}
};

#endif