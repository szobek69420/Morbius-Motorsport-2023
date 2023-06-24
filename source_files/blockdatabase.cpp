#include "blockdatabase.h"

float vertices[] = {
    //-z
    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,

    //+x
    0.5f, -0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,

    //+z
    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,

    //-x
    -0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    //+y
    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,

    //-y
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f
};

float vertices_oberwasser[] = {
	//-z
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f- OBERWASSER_DEPTH_DIFFERENCE, -0.5f,
	0.5f, 0.5f - OBERWASSER_DEPTH_DIFFERENCE, -0.5f,
	0.5f, -0.5f, -0.5f,

	//+x
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f - OBERWASSER_DEPTH_DIFFERENCE, -0.5f,
	0.5f, 0.5f - OBERWASSER_DEPTH_DIFFERENCE, 0.5f,
	0.5f, -0.5f, 0.5f,

	//+z
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f - OBERWASSER_DEPTH_DIFFERENCE, 0.5f,
	-0.5f, 0.5f - OBERWASSER_DEPTH_DIFFERENCE, 0.5f,
	-0.5f, -0.5f, 0.5f,

	//-x
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f - OBERWASSER_DEPTH_DIFFERENCE, 0.5f,
	-0.5f, 0.5f - OBERWASSER_DEPTH_DIFFERENCE, -0.5f,
	-0.5f, -0.5f, -0.5f,

	//+y
	-0.5f, 0.5f - OBERWASSER_DEPTH_DIFFERENCE, -0.5f,
	-0.5f, 0.5f - OBERWASSER_DEPTH_DIFFERENCE, 0.5f,
	0.5f, 0.5f - OBERWASSER_DEPTH_DIFFERENCE, 0.5f,
	0.5f, 0.5f - OBERWASSER_DEPTH_DIFFERENCE, -0.5f,

	//-y
	-0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f
};

float vertices_flower[] = {
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,

	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,

	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f
};

float darkness[] = { 0.85f, 0.75f, 0.55f, 0.65f, 1.0f, 0.45f };

unsigned int indices[] = { 0,2,1, 0,3,2 };

void BlockDatabase::GetVertex(int side, int vertex, float& x, float& y, float& z, float& brightness)
{
    brightness = darkness[side];

    x = vertices[side * 12 + vertex * 3];
    y = vertices[side * 12 + vertex * 3 + 1];
    z = vertices[side * 12 + vertex * 3 + 2];
}

void BlockDatabase::GetWaterVertex(int side, int vertex, float& x, float& y, float& z, float& brightness, bool oberWasser)
{
	brightness = darkness[side];

	if (!oberWasser)
	{
		x = vertices[side * 12 + vertex * 3];
		y = vertices[side * 12 + vertex * 3 + 1];
		z = vertices[side * 12 + vertex * 3 + 2];
	}
	else
	{
		x = vertices_oberwasser[side * 12 + vertex * 3];
		y = vertices_oberwasser[side * 12 + vertex * 3 + 1];
		z = vertices_oberwasser[side * 12 + vertex * 3 + 2];
	}
}

void BlockDatabase::GetFlowerVertex(int side, int vertex, float& x, float& y, float& z, float& brightness)
{
	brightness = darkness[0];

	x = vertices_flower[side * 12 + vertex * 3];
	y = vertices_flower[side * 12 + vertex * 3 + 1];
	z = vertices_flower[side * 12 + vertex * 3 + 2];
}

unsigned int BlockDatabase::GetIndex(int subSide)
{
    return indices[subSide];
}

//uv mapping
#pragma region UVcoords

float uv_borsod[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.6f, 0.0f,
	 0.6f, 0.4f,
	 1.0f, 0.4f,
	 1.0f, 0.0f,

	 //+x
	 0.6f, 0.0f,
	 0.6f, 0.4f,
	 1.0f, 0.4f,
	 1.0f, 0.0f,

	 //+z
	 0.6f, 0.0f,
	 0.6f, 0.4f,
	 1.0f, 0.4f,
	 1.0f, 0.0f,

	 //-x
	 0.6f, 0.0f,
	 0.6f, 0.4f,
	 1.0f, 0.4f,
	 1.0f, 0.0f,

	  //+y
	  0.6f, 0.0f,
	 0.6f, 0.4f,
	 1.0f, 0.4f,
	 1.0f, 0.0f,

	   //-y
	   0.6f, 0.0f,
	 0.6f, 0.4f,
	 1.0f, 0.4f,
	 1.0f, 0.0f
};

float uv_stone[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.0f, 0.9f,
	 0.0f, 1.0f,
	 0.1f, 1.0f,
	 0.1f, 0.9f,

	 //+x
	 0.0f, 0.9f,
	 0.0f, 1.0f,
	 0.1f, 1.0f,
	 0.1f, 0.9f,

	 //+z
	 0.0f, 0.9f,
	 0.0f, 1.0f,
	 0.1f, 1.0f,
	 0.1f, 0.9f,

	 //-x
	  0.0f, 0.9f,
	  0.0f, 1.0f,
	  0.1f, 1.0f,
	  0.1f, 0.9f,

	  //+y
	   0.0f, 0.9f,
	   0.0f, 1.0f,
	   0.1f, 1.0f,
	   0.1f, 0.9f,

	   //-y
	   0.0f, 0.9f,
	   0.0f, 1.0f,
	   0.1f, 1.0f,
	   0.1f, 0.9f
};

float uv_cobblestone[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	0.9f, 0.9f,
	0.9f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.9f,

	//+x
	0.9f, 0.9f,
	0.9f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.9f,

	//+z
	0.9f, 0.9f,
	0.9f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.9f,

	//-x
	0.9f, 0.9f,
	0.9f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.9f,

	//+y
	0.9f, 0.9f,
	0.9f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.9f,

	//-y
	0.9f, 0.9f,
	0.9f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.9f
};

float uv_dirt[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.3f, 0.9f,
	 0.3f, 1.0f,
	 0.4f, 1.0f,
	 0.4f, 0.9f,

	 //+x
	 0.3f, 0.9f,
	 0.3f, 1.0f,
	 0.4f, 1.0f,
	 0.4f, 0.9f,

	 //+z
	 0.3f, 0.9f,
	 0.3f, 1.0f,
	 0.4f, 1.0f,
	 0.4f, 0.9f,

	 //-x
	  0.3f, 0.9f,
	 0.3f, 1.0f,
	 0.4f, 1.0f,
	 0.4f, 0.9f,

	 //+y
	  0.3f, 0.9f,
	0.3f, 1.0f,
	0.4f, 1.0f,
	0.4f, 0.9f,

	//-y
	0.3f, 0.9f,
	0.3f, 1.0f,
	0.4f, 1.0f,
	0.4f, 0.9f
};

float uv_grass[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.2f, 0.9f,
	 0.2f, 1.0f,
	 0.3f, 1.0f,
	 0.3f, 0.9f,

	 //+x
	  0.2f, 0.9f,
	 0.2f, 1.0f,
	 0.3f, 1.0f,
	 0.3f, 0.9f,

	 //+z
	  0.2f, 0.9f,
	 0.2f, 1.0f,
	 0.3f, 1.0f,
	 0.3f, 0.9f,

	 //-x
	   0.2f, 0.9f,
	 0.2f, 1.0f,
	 0.3f, 1.0f,
	 0.3f, 0.9f,

	 //+y
	  0.1f, 0.9f,
	0.1f, 1.0f,
	0.2f, 1.0f,
	0.2f, 0.9f,

	//-y
	0.3f, 0.9f,
	0.3f, 1.0f,
	0.4f, 1.0f,
	0.4f, 0.9f
};

float uv_snow[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.5f, 0.9f,
	 0.5f, 1.0f,
	 0.6f, 1.0f,
	 0.6f, 0.9f,

	 //+x
	 0.5f, 0.9f,
	 0.5f, 1.0f,
	 0.6f, 1.0f,
	 0.6f, 0.9f,

	 //+z
	 0.5f, 0.9f,
	 0.5f, 1.0f,
	 0.6f, 1.0f,
	 0.6f, 0.9f,

	 //-x
	 0.5f, 0.9f,
	 0.5f, 1.0f,
	 0.6f, 1.0f,
	 0.6f, 0.9f,

	 //+y
	 0.4f, 0.9f,
	 0.4f, 1.0f,
	 0.5f, 1.0f,
	 0.5f, 0.9f,

	 //-y
	 0.3f, 0.9f,
	 0.3f, 1.0f,
	 0.4f, 1.0f,
	 0.4f, 0.9f
};

float uv_water[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.0f, 0.7f,
	 0.0f, 0.8f,
	 0.1f, 0.8f,
	 0.1f, 0.7f,

	 //+x
	 0.0f, 0.7f,
	 0.0f, 0.8f,
	 0.1f, 0.8f,
	 0.1f, 0.7f,

	 //+z
	 0.0f, 0.7f,
	 0.0f, 0.8f,
	 0.1f, 0.8f,
	 0.1f, 0.7f,

	 //-x
	 0.0f, 0.7f,
	 0.0f, 0.8f,
	 0.1f, 0.8f,
	 0.1f, 0.7f,

	 //+y
	 0.0f, 0.7f,
	 0.0f, 0.8f,
	 0.1f, 0.8f,
	 0.1f, 0.7f,

	 //-y
	 0.0f, 0.7f,
	 0.0f, 0.8f,
	 0.1f, 0.8f,
	 0.1f, 0.7f,
};

float uv_air[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.1f, 0.7f,
	 0.1f, 0.8f,
	 0.2f, 0.8f,
	 0.2f, 0.7f,

	 //+x
	 0.1f, 0.7f,
	 0.1f, 0.8f,
	 0.2f, 0.8f,
	 0.2f, 0.7f,

	 //+z
	 0.1f, 0.7f,
	 0.1f, 0.8f,
	 0.2f, 0.8f,
	 0.2f, 0.7f,

	 //-x
	 0.1f, 0.7f,
	 0.1f, 0.8f,
	 0.2f, 0.8f,
	 0.2f, 0.7f,

	 //+y
	 0.1f, 0.7f,
	 0.1f, 0.8f,
	 0.2f, 0.8f,
	 0.2f, 0.7f,

	 //-y
	 0.1f, 0.7f,
	 0.1f, 0.8f,
	 0.2f, 0.8f,
	 0.2f, 0.7f,
};

float uv_sand[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	0.6f, 0.9f,
	0.6f, 1.0f,
	0.7f, 1.0f,
	0.7f, 0.9f,

	//+x
	0.6f, 0.9f,
	0.6f, 1.0f,
	0.7f, 1.0f,
	0.7f, 0.9f,

	//+z
	0.6f, 0.9f,
	0.6f, 1.0f,
	0.7f, 1.0f,
	0.7f, 0.9f,

	//-x
	0.6f, 0.9f,
	0.6f, 1.0f,
	0.7f, 1.0f,
	0.7f, 0.9f,

	//+y
	0.6f, 0.9f,
	0.6f, 1.0f,
	0.7f, 1.0f,
	0.7f, 0.9f,

	//-y
	0.6f, 0.9f,
	0.6f, 1.0f,
	0.7f, 1.0f,
	0.7f, 0.9f,
};

float uv_glass[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	0.7f, 0.9f,
	0.7f, 1.0f,
	0.8f, 1.0f,
	0.8f, 0.9f,

	//+x
	0.7f, 0.9f,
	0.7f, 1.0f,
	0.8f, 1.0f,
	0.8f, 0.9f,

	//+z
	0.7f, 0.9f,
	0.7f, 1.0f,
	0.8f, 1.0f,
	0.8f, 0.9f,

	//-x
	0.7f, 0.9f,
	0.7f, 1.0f,
	0.8f, 1.0f,
	0.8f, 0.9f,

	//+y
	0.7f, 0.9f,
	0.7f, 1.0f,
	0.8f, 1.0f,
	0.8f, 0.9f,

	//-y
	0.7f, 0.9f,
	0.7f, 1.0f,
	0.8f, 1.0f,
	0.8f, 0.9f
};

float uv_bedrock[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	0.8f, 0.9f,
	0.8f, 1.0f,
	0.9f, 1.0f,
	0.9f, 0.9f,

	//+x
	0.8f, 0.9f,
	0.8f, 1.0f,
	0.9f, 1.0f,
	0.9f, 0.9f,

	//+z
	0.8f, 0.9f,
	0.8f, 1.0f,
	0.9f, 1.0f,
	0.9f, 0.9f,

	//-x
	0.8f, 0.9f,
	0.8f, 1.0f,
	0.9f, 1.0f,
	0.9f, 0.9f,

	//+y
	0.8f, 0.9f,
	0.8f, 1.0f,
	0.9f, 1.0f,
	0.9f, 0.9f,

	//-y
	0.8f, 0.9f,
	0.8f, 1.0f,
	0.9f, 1.0f,
	0.9f, 0.9f
};

float uv_oak_log[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.0f, 0.8f,
	 0.0f, 0.9f,
	 0.1f, 0.9f,
	 0.1f, 0.8f,

	 //+x
	 0.0f, 0.8f,
	 0.0f, 0.9f,
	 0.1f, 0.9f,
	 0.1f, 0.8f,

	 //+z
	 0.0f, 0.8f,
	 0.0f, 0.9f,
	 0.1f, 0.9f,
	 0.1f, 0.8f,

	 //-x
	 0.0f, 0.8f,
	 0.0f, 0.9f,
	 0.1f, 0.9f,
	 0.1f, 0.8f,

	 //+y
	 0.1f, 0.8f,
	 0.1f, 0.9f,
	 0.2f, 0.9f,
	 0.2f, 0.8f,

	 //-y
	 0.1f, 0.8f,
	 0.1f, 0.9f,
	 0.2f, 0.9f,
	 0.2f, 0.8f
};

float uv_oak_leaves[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.2f, 0.8f,
	 0.2f, 0.9f,
	 0.3f, 0.9f,
	 0.3f, 0.8f,

	 //+x
	 0.2f, 0.8f,
	 0.2f, 0.9f,
	 0.3f, 0.9f,
	 0.3f, 0.8f,

	 //+z
	 0.2f, 0.8f,
	 0.2f, 0.9f,
	 0.3f, 0.9f,
	 0.3f, 0.8f,

	 //-x
	 0.2f, 0.8f,
	 0.2f, 0.9f,
	 0.3f, 0.9f,
	 0.3f, 0.8f,

	 //+y
	 0.2f, 0.8f,
	 0.2f, 0.9f,
	 0.3f, 0.9f,
	 0.3f, 0.8f,

	 //-y
	 0.2f, 0.8f,
	 0.2f, 0.9f,
	 0.3f, 0.9f,
	 0.3f, 0.8f
};

float uv_birch_log[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.3f, 0.8f,
	 0.3f, 0.9f,
	 0.4f, 0.9f,
	 0.4f, 0.8f,

	 //+x
	 0.3f, 0.8f,
	 0.3f, 0.9f,
	 0.4f, 0.9f,
	 0.4f, 0.8f,

	 //+z
	 0.3f, 0.8f,
	 0.3f, 0.9f,
	 0.4f, 0.9f,
	 0.4f, 0.8f,

	 //-x
	 0.3f, 0.8f,
	 0.3f, 0.9f,
	 0.4f, 0.9f,
	 0.4f, 0.8f,

	 //+y
	 0.4f, 0.8f,
	 0.4f, 0.9f,
	 0.5f, 0.9f,
	 0.5f, 0.8f,

	 //-y
	 0.4f, 0.8f,
	 0.4f, 0.9f,
	 0.5f, 0.9f,
	 0.5f, 0.8f
};

float uv_birch_leaves[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.5f, 0.8f,
	 0.5f, 0.9f,
	 0.6f, 0.9f,
	 0.6f, 0.8f,

	 //+x
	 0.5f, 0.8f,
	 0.5f, 0.9f,
	 0.6f, 0.9f,
	 0.6f, 0.8f,

	 //+z
	 0.5f, 0.8f,
	 0.5f, 0.9f,
	 0.6f, 0.9f,
	 0.6f, 0.8f,

	 //-x
	 0.5f, 0.8f,
	 0.5f, 0.9f,
	 0.6f, 0.9f,
	 0.6f, 0.8f,

	 //+y
	 0.5f, 0.8f,
	 0.5f, 0.9f,
	 0.6f, 0.9f,
	 0.6f, 0.8f,

	 //-y
	 0.5f, 0.8f,
	 0.5f, 0.9f,
	 0.6f, 0.9f,
	 0.6f, 0.8f
};

float uv_red_flower[] = {
	0.0f, 0.6f,
	0.0f, 0.7f,
	0.1f, 0.7f,
	0.1f, 0.6f,

	0.0f, 0.6f,
	0.0f, 0.7f,
	0.1f, 0.7f,
	0.1f, 0.6f,

	0.0f, 0.6f,
	0.0f, 0.7f,
	0.1f, 0.7f,
	0.1f, 0.6f,

	0.0f, 0.6f,
	0.0f, 0.7f,
	0.1f, 0.7f,
	0.1f, 0.6f
};

float uv_yellow_flower[] = {
	0.1f, 0.6f,
	0.1f, 0.7f,
	0.2f, 0.7f,
	0.2f, 0.6f,

	0.1f, 0.6f,
	0.1f, 0.7f,
	0.2f, 0.7f,
	0.2f, 0.6f,

	0.1f, 0.6f,
	0.1f, 0.7f,
	0.2f, 0.7f,
	0.2f, 0.6f,

	0.1f, 0.6f,
	0.1f, 0.7f,
	0.2f, 0.7f,
	0.2f, 0.6f
};

float uv_blue_flower[] = {
	0.2f, 0.6f,
	0.2f, 0.7f,
	0.3f, 0.7f,
	0.3f, 0.6f,

	0.2f, 0.6f,
	0.2f, 0.7f,
	0.3f, 0.7f,
	0.3f, 0.6f,

	0.2f, 0.6f,
	0.2f, 0.7f,
	0.3f, 0.7f,
	0.3f, 0.6f,

	0.2f, 0.6f,
	0.2f, 0.7f,
	0.3f, 0.7f,
	0.3f, 0.6f
};

float uv_diamond_block[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.0f, 0.5f,
	 0.0f, 0.6f,
	 0.1f, 0.6f,
	 0.1f, 0.5f,

	 //+x
	 0.0f, 0.5f,
	 0.0f, 0.6f,
	 0.1f, 0.6f,
	 0.1f, 0.5f,

	 //+z
	 0.0f, 0.5f,
	 0.0f, 0.6f,
	 0.1f, 0.6f,
	 0.1f, 0.5f,

	 //-x
	 0.0f, 0.5f,
	 0.0f, 0.6f,
	 0.1f, 0.6f,
	 0.1f, 0.5f,

	 //+y
	 0.0f, 0.5f,
	 0.0f, 0.6f,
	 0.1f, 0.6f,
	 0.1f, 0.5f,

	 //-y
	 0.0f, 0.5f,
	 0.0f, 0.6f,
	 0.1f, 0.6f,
	 0.1f, 0.5f
};

float uv_gold_block[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.1f, 0.5f,
	 0.1f, 0.6f,
	 0.2f, 0.6f,
	 0.2f, 0.5f,

	 //+x
	 0.1f, 0.5f,
	 0.1f, 0.6f,
	 0.2f, 0.6f,
	 0.2f, 0.5f,

	 //+z
	 0.1f, 0.5f,
	 0.1f, 0.6f,
	 0.2f, 0.6f,
	 0.2f, 0.5f,

	 //-x
	 0.1f, 0.5f,
	 0.1f, 0.6f,
	 0.2f, 0.6f,
	 0.2f, 0.5f,

	 //+y
	 0.1f, 0.5f,
	 0.1f, 0.6f,
	 0.2f, 0.6f,
	 0.2f, 0.5f,

	 //-y
	 0.1f, 0.5f,
	 0.1f, 0.6f,
	 0.2f, 0.6f,
	 0.2f, 0.5f
};

float uv_iron_block[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.2f, 0.5f,
	 0.2f, 0.6f,
	 0.3f, 0.6f,
	 0.3f, 0.5f,

	 //+x
	 0.2f, 0.5f,
	 0.2f, 0.6f,
	 0.3f, 0.6f,
	 0.3f, 0.5f,

	 //+z
	 0.2f, 0.5f,
	 0.2f, 0.6f,
	 0.3f, 0.6f,
	 0.3f, 0.5f,

	 //-x
	 0.2f, 0.5f,
	 0.2f, 0.6f,
	 0.3f, 0.6f,
	 0.3f, 0.5f,

	 //+y
	 0.2f, 0.5f,
	 0.2f, 0.6f,
	 0.3f, 0.6f,
	 0.3f, 0.5f,

	 //-y
	 0.2f, 0.5f,
	 0.2f, 0.6f,
	 0.3f, 0.6f,
	 0.3f, 0.5f
};

float uv_coal_block[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.3f, 0.5f,
	 0.3f, 0.6f,
	 0.4f, 0.6f,
	 0.4f, 0.5f,

	 //+x
	 0.3f, 0.5f,
	 0.3f, 0.6f,
	 0.4f, 0.6f,
	 0.4f, 0.5f,

	 //+z
	 0.3f, 0.5f,
	 0.3f, 0.6f,
	 0.4f, 0.6f,
	 0.4f, 0.5f,

	 //-x
	 0.3f, 0.5f,
	 0.3f, 0.6f,
	 0.4f, 0.6f,
	 0.4f, 0.5f,

	 //+y
	 0.3f, 0.5f,
	 0.3f, 0.6f,
	 0.4f, 0.6f,
	 0.4f, 0.5f,

	 //-y
	 0.3f, 0.5f,
	 0.3f, 0.6f,
	 0.4f, 0.6f,
	 0.4f, 0.5f
};

float uv_emerald_block[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.4f, 0.5f,
	 0.4f, 0.6f,
	 0.5f, 0.6f,
	 0.5f, 0.5f,

	 //+x
	 0.4f, 0.5f,
	 0.4f, 0.6f,
	 0.5f, 0.6f,
	 0.5f, 0.5f,

	 //+z
	 0.4f, 0.5f,
	 0.4f, 0.6f,
	 0.5f, 0.6f,
	 0.5f, 0.5f,

	 //-x
	 0.4f, 0.5f,
	 0.4f, 0.6f,
	 0.5f, 0.6f,
	 0.5f, 0.5f,

	 //+y
	 0.4f, 0.5f,
	 0.4f, 0.6f,
	 0.5f, 0.6f,
	 0.5f, 0.5f,

	 //-y
	 0.4f, 0.5f,
	 0.4f, 0.6f,
	 0.5f, 0.6f,
	 0.5f, 0.5f
};

float uv_redstone_block[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.6f, 0.5f,
	 0.6f, 0.6f,
	 0.7f, 0.6f,
	 0.7f, 0.5f,

	 //+x
	 0.6f, 0.5f,
	 0.6f, 0.6f,
	 0.7f, 0.6f,
	 0.7f, 0.5f,

	 //+z
	 0.6f, 0.5f,
	 0.6f, 0.6f,
	 0.7f, 0.6f,
	 0.7f, 0.5f,

	 //-x
	 0.6f, 0.5f,
	 0.6f, 0.6f,
	 0.7f, 0.6f,
	 0.7f, 0.5f,

	 //+y
	 0.6f, 0.5f,
	 0.6f, 0.6f,
	 0.7f, 0.6f,
	 0.7f, 0.5f,

	 //-y
	 0.6f, 0.5f,
	 0.6f, 0.6f,
	 0.7f, 0.6f,
	 0.7f, 0.5f
};

float uv_copper_block[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.7f, 0.5f,
	 0.7f, 0.6f,
	 0.8f, 0.6f,
	 0.8f, 0.5f,

	 //+x
	 0.7f, 0.5f,
	 0.7f, 0.6f,
	 0.8f, 0.6f,
	 0.8f, 0.5f,

	 //+z
	 0.7f, 0.5f,
	 0.7f, 0.6f,
	 0.8f, 0.6f,
	 0.8f, 0.5f,

	 //-x
	 0.7f, 0.5f,
	 0.7f, 0.6f,
	 0.8f, 0.6f,
	 0.8f, 0.5f,

	 //+y
	 0.7f, 0.5f,
	 0.7f, 0.6f,
	 0.8f, 0.6f,
	 0.8f, 0.5f,

	 //-y
	 0.7f, 0.5f,
	 0.7f, 0.6f,
	 0.8f, 0.6f,
	 0.8f, 0.5f
};

float uv_nether_reactor_core[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.5f, 0.5f,
	 0.5f, 0.6f,
	 0.6f, 0.6f,
	 0.6f, 0.5f,

	 //+x
	 0.5f, 0.5f,
	 0.5f, 0.6f,
	 0.6f, 0.6f,
	 0.6f, 0.5f,

	 //+z
	 0.5f, 0.5f,
	 0.5f, 0.6f,
	 0.6f, 0.6f,
	 0.6f, 0.5f,

	 //-x
	 0.5f, 0.5f,
	 0.5f, 0.6f,
	 0.6f, 0.6f,
	 0.6f, 0.5f,

	 //+y
	 0.5f, 0.5f,
	 0.5f, 0.6f,
	 0.6f, 0.6f,
	 0.6f, 0.5f,

	 //-y
	 0.5f, 0.5f,
	 0.5f, 0.6f,
	 0.6f, 0.6f,
	 0.6f, 0.5f
};

float uv_crying_obsidian[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.0f, 0.4f,
	 0.0f, 0.5f,
	 0.1f, 0.5f,
	 0.1f, 0.4f,

	 //+x
	  0.0f, 0.4f,
	 0.0f, 0.5f,
	 0.1f, 0.5f,
	 0.1f, 0.4f,

	 //+z
	  0.0f, 0.4f,
	 0.0f, 0.5f,
	 0.1f, 0.5f,
	 0.1f, 0.4f,

	 //-x
	 0.0f, 0.4f,
	 0.0f, 0.5f,
	 0.1f, 0.5f,
	 0.1f, 0.4f,

	 //+y
	 0.0f, 0.4f,
	 0.0f, 0.5f,
	 0.1f, 0.5f,
	 0.1f, 0.4f,

	 //-y
	 0.0f, 0.4f,
	 0.0f, 0.5f,
	 0.1f, 0.5f,
	 0.1f, 0.4f
};

float uv_prismarine_bricks[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.1f, 0.4f,
	 0.1f, 0.5f,
	 0.2f, 0.5f,
	 0.2f, 0.4f,

	 //+x
	  0.1f, 0.4f,
	 0.1f, 0.5f,
	 0.2f, 0.5f,
	 0.2f, 0.4f,

	 //+z
	 0.1f, 0.4f,
	 0.1f, 0.5f,
	 0.2f, 0.5f,
	 0.2f, 0.4f,

	 //-x
	 0.1f, 0.4f,
	 0.1f, 0.5f,
	 0.2f, 0.5f,
	 0.2f, 0.4f,

	 //+y
	 0.1f, 0.4f,
	 0.1f, 0.5f,
	 0.2f, 0.5f,
	 0.2f, 0.4f,

	 //-y
	 0.1f, 0.4f,
	 0.1f, 0.5f,
	 0.2f, 0.5f,
	 0.2f, 0.4f
};

float uv_honeycomb_block[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.2f, 0.4f,
	 0.2f, 0.5f,
	 0.3f, 0.5f,
	 0.3f, 0.4f,

	 //+x
	 0.2f, 0.4f,
	 0.2f, 0.5f,
	 0.3f, 0.5f,
	 0.3f, 0.4f,

	 //+z
	 0.2f, 0.4f,
	 0.2f, 0.5f,
	 0.3f, 0.5f,
	 0.3f, 0.4f,

	 //-x
	 0.2f, 0.4f,
	 0.2f, 0.5f,
	 0.3f, 0.5f,
	 0.3f, 0.4f,

	 //+y
	 0.2f, 0.4f,
	 0.2f, 0.5f,
	 0.3f, 0.5f,
	 0.3f, 0.4f,

	 //-y
	 0.2f, 0.4f,
	 0.2f, 0.5f,
	 0.3f, 0.5f,
	 0.3f, 0.4f
};

float uv_amethyst_block[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.3f, 0.4f,
	 0.3f, 0.5f,
	 0.4f, 0.5f,
	 0.4f, 0.4f,

	 //+x
	 0.3f, 0.4f,
	 0.3f, 0.5f,
	 0.4f, 0.5f,
	 0.4f, 0.4f,

	 //+z
	 0.3f, 0.4f,
	 0.3f, 0.5f,
	 0.4f, 0.5f,
	 0.4f, 0.4f,

	 //-x
	 0.3f, 0.4f,
	 0.3f, 0.5f,
	 0.4f, 0.5f,
	 0.4f, 0.4f,

	 //+y
	 0.3f, 0.4f,
	 0.3f, 0.5f,
	 0.4f, 0.5f,
	 0.4f, 0.4f,

	 //-y
	 0.3f, 0.4f,
	 0.3f, 0.5f,
	 0.4f, 0.5f,
	 0.4f, 0.4f
};

float uv_yellow_terracotta[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.4f, 0.4f,
	 0.4f, 0.5f,
	 0.5f, 0.5f,
	 0.5f, 0.4f,

	 //+x
	 0.4f, 0.4f,
	 0.4f, 0.5f,
	 0.5f, 0.5f,
	 0.5f, 0.4f,

	 //+z
	 0.4f, 0.4f,
	 0.4f, 0.5f,
	 0.5f, 0.5f,
	 0.5f, 0.4f,

	 //-x
	 0.4f, 0.4f,
	 0.4f, 0.5f,
	 0.5f, 0.5f,
	 0.5f, 0.4f,

	 //+y
	 0.4f, 0.4f,
	 0.4f, 0.5f,
	 0.5f, 0.5f,
	 0.5f, 0.4f,

	 //-y
	 0.4f, 0.4f,
	 0.4f, 0.5f,
	 0.5f, 0.5f,
	 0.5f, 0.4f
};

float uv_stone_bricks[] = {
	//-z (ez a normal iranya)(sima, nem opengl-es z)
	 0.5f, 0.4f,
	 0.5f, 0.5f,
	 0.6f, 0.5f,
	 0.6f, 0.4f,

	 //+x
	 0.5f, 0.4f,
	 0.5f, 0.5f,
	 0.6f, 0.5f,
	 0.6f, 0.4f,

	 //+z
	 0.5f, 0.4f,
	 0.5f, 0.5f,
	 0.6f, 0.5f,
	 0.6f, 0.4f,

	 //-x
	 0.5f, 0.4f,
	 0.5f, 0.5f,
	 0.6f, 0.5f,
	 0.6f, 0.4f,

	 //+y
	 0.5f, 0.4f,
	 0.5f, 0.5f,
	 0.6f, 0.5f,
	 0.6f, 0.4f,

	 //-y
	 0.5f, 0.4f,
	 0.5f, 0.5f,
	 0.6f, 0.5f,
	 0.6f, 0.4f
};

#pragma endregion UVcoords

void BlockDatabase::GetUv(int side, int vertex, int type, float& uvx, float& uvy)
{
	switch (type) {
	case AIR:
		uvx = uv_air[side * 8 + vertex * 2];
		uvy = uv_air[side * 8 + vertex * 2 + 1];
		break;

	case BORSOD:
		uvx = uv_borsod[side * 8 + vertex * 2];
		uvy = uv_borsod[side * 8 + vertex * 2 + 1];
		break;

	case STONE:
		uvx = uv_stone[side * 8 + vertex * 2];
		uvy = uv_stone[side * 8 + vertex * 2 + 1];
		break;

	case COBBLESTONE:
		uvx = uv_cobblestone[side * 8 + vertex * 2];
		uvy = uv_cobblestone[side * 8 + vertex * 2 + 1];
		break;

	case DIRT:
		uvx = uv_dirt[side * 8 + vertex * 2];
		uvy = uv_dirt[side * 8 + vertex * 2 + 1];
		break;

	case GRASS_BLOCK:
		uvx = uv_grass[side * 8 + vertex * 2];
		uvy = uv_grass[side * 8 + vertex * 2 + 1];
		break;

	case SNOW_BLOCK:
		uvx = uv_snow[side * 8 + vertex * 2];
		uvy = uv_snow[side * 8 + vertex * 2 + 1];
		break;

	case WATER:
		uvx = uv_water[side * 8 + vertex * 2];
		uvy = uv_water[side * 8 + vertex * 2 + 1];
		break;

	case SAND:
		uvx = uv_sand[side * 8 + vertex * 2];
		uvy = uv_sand[side * 8 + vertex * 2 + 1];
		break;

	case GLASS_BLOCK:
		uvx = uv_glass[side * 8 + vertex * 2];
		uvy = uv_glass[side * 8 + vertex * 2 + 1];
		break;

	case BEDROCK:
		uvx = uv_bedrock[side * 8 + vertex * 2];
		uvy = uv_bedrock[side * 8 + vertex * 2 + 1];
		break;

	case OAK_LOG:
		uvx = uv_oak_log[side * 8 + vertex * 2];
		uvy = uv_oak_log[side * 8 + vertex * 2 + 1];
		break;

	case OAK_LEAVES:
		uvx = uv_oak_leaves[side * 8 + vertex * 2];
		uvy = uv_oak_leaves[side * 8 + vertex * 2 + 1];
		break;

	case BIRCH_LOG:
		uvx = uv_birch_log[side * 8 + vertex * 2];
		uvy = uv_birch_log[side * 8 + vertex * 2 + 1];
		break;

	case BIRCH_LEAVES:
		uvx = uv_birch_leaves[side * 8 + vertex * 2];
		uvy = uv_birch_leaves[side * 8 + vertex * 2 + 1];
		break;

	case RED_FLOWER:
		uvx = uv_red_flower[side * 8 + vertex * 2];
		uvy = uv_red_flower[side * 8 + vertex * 2 + 1];
		break;

	case YELLOW_FLOWER:
		uvx = uv_yellow_flower[side * 8 + vertex * 2];
		uvy = uv_yellow_flower[side * 8 + vertex * 2 + 1];
		break;

	case BLUE_FLOWER:
		uvx = uv_blue_flower[side * 8 + vertex * 2];
		uvy = uv_blue_flower[side * 8 + vertex * 2 + 1];
		break;

	case DIAMOND_BLOCK:
		uvx = uv_diamond_block[side * 8 + vertex * 2];
		uvy = uv_diamond_block[side * 8 + vertex * 2 + 1];
		break;

	case GOLD_BLOCK:
		uvx = uv_gold_block[side * 8 + vertex * 2];
		uvy = uv_gold_block[side * 8 + vertex * 2 + 1];
		break;

	case IRON_BLOCK:
		uvx = uv_iron_block[side * 8 + vertex * 2];
		uvy = uv_iron_block[side * 8 + vertex * 2 + 1];
		break;

	case COAL_BLOCK:
		uvx = uv_coal_block[side * 8 + vertex * 2];
		uvy = uv_coal_block[side * 8 + vertex * 2 + 1];
		break;

	case EMERALD_BLOCK:
		uvx = uv_emerald_block[side * 8 + vertex * 2];
		uvy = uv_emerald_block[side * 8 + vertex * 2 + 1];
		break;

	case REDSTONE_BLOCK:
		uvx = uv_redstone_block[side * 8 + vertex * 2];
		uvy = uv_redstone_block[side * 8 + vertex * 2 + 1];
		break;

	case COPPER_BLOCK:
		uvx = uv_copper_block[side * 8 + vertex * 2];
		uvy = uv_copper_block[side * 8 + vertex * 2 + 1];
		break;

	case NETHER_REACTOR_CORE:
		uvx = uv_nether_reactor_core[side * 8 + vertex * 2];
		uvy = uv_nether_reactor_core[side * 8 + vertex * 2 + 1];
		break;

	case CRYING_OBSIDIAN:
		uvx = uv_crying_obsidian[side * 8 + vertex * 2];
		uvy = uv_crying_obsidian[side * 8 + vertex * 2 + 1];
		break;

	case PRISMARINE_BRICKS:
		uvx = uv_prismarine_bricks[side * 8 + vertex * 2];
		uvy = uv_prismarine_bricks[side * 8 + vertex * 2 + 1];
		break;

	case HONEYCOMB_BLOCK:
		uvx = uv_honeycomb_block[side * 8 + vertex * 2];
		uvy = uv_honeycomb_block[side * 8 + vertex * 2 + 1];
		break;

	case AMETHYST_BLOCK:
		uvx = uv_amethyst_block[side * 8 + vertex * 2];
		uvy = uv_amethyst_block[side * 8 + vertex * 2 + 1];
		break;

	case YELLOW_TERRACOTTA:
		uvx = uv_yellow_terracotta[side * 8 + vertex * 2];
		uvy = uv_yellow_terracotta[side * 8 + vertex * 2 + 1];
		break;

	case STONE_BRICKS:
		uvx = uv_stone_bricks[side * 8 + vertex * 2];
		uvy = uv_stone_bricks[side * 8 + vertex * 2 + 1];
		break;
	}
}