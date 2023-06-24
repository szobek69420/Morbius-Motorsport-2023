#ifndef BLOCKDATABASE_H
#define BLOCKDATABASE_H

//a blokkok neveit tartalmazo enumerator
enum blocks {
	AIR,
	BEDROCK,
	BORSOD,
	STONE,
	COBBLESTONE,
	SAND,
	DIRT,
	GRASS_BLOCK,
	SNOW_BLOCK,
	DIAMOND_BLOCK,
	GOLD_BLOCK,
	IRON_BLOCK,
	COAL_BLOCK,
	EMERALD_BLOCK,
	REDSTONE_BLOCK,
	COPPER_BLOCK,
	NETHER_REACTOR_CORE,
	CRYING_OBSIDIAN,
	PRISMARINE_BRICKS,
	HONEYCOMB_BLOCK,
	AMETHYST_BLOCK,
	YELLOW_TERRACOTTA,
	STONE_BRICKS,
	PURPLE_CONCRETE_POWDER,
	OAK_LOG,//fontos, hogy egyazon fafajtanak a torzse es a levele egymas utan legyen az enumban
	OAK_LEAVES,
	BIRCH_LOG,
	BIRCH_LEAVES,
	//transparent block types (fontos, hogy ezutan csak olyan blokkok legyenek, amik mellett latszodnak a solid blokkok
	WATER,
	GLASS_BLOCK,
	//flowers (a flowers utan ne legyen semmi)
	RED_FLOWER,
	YELLOW_FLOWER,
	BLUE_FLOWER
};

enum sideEnums {
	NEGATIVE_Z=0,
	POSITIVE_X=1,
	POSITIVE_Z=2,
	NEGATIVE_X=3,
	POSITIVE_Y=4,
	NEGATIVE_Y=5
};

//mennyivel van lejjebb a viz legfelso retege a blokk altal kijelolt felso oldaltol
const float OBERWASSER_DEPTH_DIFFERENCE = 0.15f;

class BlockDatabase {
public:
	//egy blockvertex all a poziciobol, texture coordinate-kbol es a megvilagitottsagbol. a side a lekert blokk oldalat, a vertex pedig az ezen belul elhelyezkedo csucsot hatarozza meg
	static void GetVertex(int side, int vertex, float& x, float& y, float& z, float& brightness);

	//hasonlo, mint a GetVertex, csak az OBERWASSER_DEPTH_DIFFERENCE-T is beleszamolja
	static void GetWaterVertex(int side, int vertex, float& x, float& y, float& z, float& brightness, bool oberWasser = false);

	//a viragoknak nem olyan a felepitese, mint a blokkoknak, de ezt leszamitva ugyanaz, mint a GetVertex
	static void GetFlowerVertex(int side, int vertex, float& x, float& y, float& z, float& brightness);

	//hatekonyabb 4 csucsot es 6 indexet elmenteni oldalankent, mint 6 csucsot, a GetIndex az elmentett csucs indexet adja vissza a megjeleniteshez (glDrawElements)
	static unsigned int GetIndex(int subSide);

	//a lekert blokk lekert csucsanak a texture coords-at adja vissza
	static void GetUv(int side, int vertex, int type, float& uvx, float& uvy);
};

#endif