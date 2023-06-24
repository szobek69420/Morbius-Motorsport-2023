#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include "FastNoiseLite.h"

const int TERRAIN_BASE_HEIGHT = 10;
const float TERRAIN_ELEVATION_MAX = 130.0f;

//a FastNoiseLite.h segitsegevel mukodo domborzatgeneralo
class TerrainGenerator {
private:
	int seed;
	float offsetX[3];
	float offsetZ[3];
	FastNoiseLite noise[3];

public:
	TerrainGenerator(int seed)
	{
		this->seed = seed;

		noise[0].SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		noise[1].SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		noise[2].SetNoiseType(FastNoiseLite::NoiseType_Perlin);

		//0.01, 0.01, 0.01 -> small islands
		noise[0].SetFrequency(0.0005f);
		noise[1].SetFrequency(0.005f);
		noise[2].SetFrequency(0.009f);

		noise[0].SetFractalOctaves(2);
		noise[1].SetFractalOctaves(2);
		noise[2].SetFractalOctaves(2);

		//noise[1].SetCellularJitter(4.0f);

		srand(seed);
		offsetX[0] = rand();
		offsetX[1] = rand();
		offsetX[2] = rand();

		offsetZ[0] = rand();
		offsetZ[1] = rand();
		offsetZ[2] = rand();
	}

	//a parameterek es az osztalypeldanyba toltott adatok alapjan visszaad egy domborzatmagassagot, mely garantaltan nulla es a build limit kozott van
	float GetTerrainHeight(float posx, float posz);
};

#endif