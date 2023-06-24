#include "terraingenerator.h"

float noise1_levels[] = {
	-0.1f,
	0.1f,
	0.3f,
	0.5f,
	0.65f,
	0.75f,
	0.95f,
	1.1f
};

float noise1_heights[] = {
	1.0f,
	0.9f,
	0.4f,
	0.6f,
	0.2f,
	0.5f,
	0.9f,
	1.0f
};

float noise2_levels[] = {
	-0.1f,
	0.15f,
	0.3f,
	0.5f,
	0.6f,
	1.1f
};

float noise2_heights[] = {
	0.6f,
	0.9f,
	0.6f,
	0.9f,
	0.65f,
	0.85f
};

float noise3_levels[] = {
	-0.1f,
	0.2f,
	0.4f,
	0.6f,
	0.85f,
	1.1f
};

float noise3_heights[] = {
	0.4f,
	0.8f,
	0.5f,
	0.75f,
	0.9f,
	0.7f
};

float TerrainGenerator::GetTerrainHeight(float posx, float posz)
{
	float vissza = 1.0f;
	
	float temp = 0;
	temp = noise[0].GetNoise(offsetX[0] + posx*4.0f, offsetZ[0] + posz*4.0f);
	temp += 1.0f;
	temp /= 2.0f;
	for (int i = 1; i < 7; i++)
	{
		if (temp < noise1_levels[i])
		{
			temp = noise1_heights[i - 1] + ((noise1_heights[i] - noise1_heights[i - 1]) * ((temp - noise1_levels[i - 1]) / (noise1_levels[i] - noise1_levels[i - 1])));
			break;
		}
	}
	vissza *= temp;

	temp = noise[1].GetNoise(offsetX[1] + posx*2.0f, offsetZ[1] + posz*2.0f);
	temp += 1.0f;
	temp /= 2.0f;
	for (int i = 1; i < 6; i++)
	{
		if (temp < noise2_levels[i])
		{
			temp = noise2_heights[i - 1] + ((noise2_heights[i] - noise2_heights[i - 1]) * ((temp - noise2_levels[i - 1]) / (noise2_levels[i] - noise2_levels[i - 1])));
			break;
		}
	}
	vissza *= temp;

	temp = noise[2].GetNoise(offsetX[2] + posx, offsetZ[2] + posz);
	temp += 1.0f;
	temp /= 2.0f;
	for (int i = 1; i < 6; i++)
	{
		if (temp < noise3_levels[i])
		{
			temp = noise3_heights[i - 1] + ((noise3_heights[i] - noise3_heights[i - 1]) * ((temp - noise3_levels[i - 1]) / (noise3_levels[i] - noise3_levels[i - 1])));
			break;
		}
	}
	vissza *= temp;

	vissza *= TERRAIN_ELEVATION_MAX;
	vissza += TERRAIN_BASE_HEIGHT;

	return vissza;
}