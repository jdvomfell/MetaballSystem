#ifndef METABALL_H
#define METABALL_H

#include<SFML/Graphics.hpp>

// The scaler applied to the metaballs size
#define DEFAULT_SCALE 0.5f
// The alpha threshold where metaballs are no longer visible
#define OUTER_THRESHOLD 0.8f
#define INNER_THRESHOLD 0.9f
// Inner and Outer color areas
const enum COLOR_AREA {INNER, OUTER, ALL};

// Desc: Basic structure of a single metaball
//		position: A vector containing the x and y position of the metaball
//		velocity: A vector containing the x and y velocity of the metaball
//		lifespan: The amount of time remaining before the metaball will be disposed
//		weight: The weight that is applied to the velocity
class Metaball{

public:

	Metaball(sf::Vector2f position, sf::Vector2f velocity, float lifespan, float weight);

	sf::Vector2f position;
	sf::Vector2f velocity;
	float lifespan;
	float weight;

};

// Desc: A system to handle all metaballs and metaball spawners in an enviornment
//		spawners: A list of spawners to update in the enviornment
//		metaballList: A list of metaballs to update in the enviornment
//		metaballTexture: The metaball texture
//		metaballSprite: A renderable sprite made with the metaball texture used to draw metaballs
//		metaballAddTexture: The texture that additive blending will be 
//			applied to as metaballs are drawn to it
//		metaballAddSprite: The renderable sprite that that additive blending has been applied to
//		metaballShadedTexture: The texture that alpha culling and metaball recoloring will be applied to
//		mataballShadedSprite: The renderable sprite that alpha culling and metaball recoloring has
//			been applied to
//		shader: The shader that will be applied to the metaballShadedTexture
class MetaballHandler {

	std::vector<class MetaballSpawner *> spawners;
	std::vector<Metaball *> metaballList;

	sf::Texture metaballTexture;
	sf::Sprite metaballSprite;

	sf::RenderTexture metaballAddTexture;
	sf::Sprite metaballAddSprite;

	sf::RenderTexture metaballShadedTexture;
	sf::Sprite metaballShadedSprite;

	sf::Shader shader;

	void removeMetaball(int i);

public:

	MetaballHandler();

	void init(sf::Vector2f viewSize);
	void resizeTexture(sf::Vector2f viewSize);
	void clean();

	void addSpawner(sf::Vector2f position, sf::Vector2f velocity, float weight, float lifespan, int spawnPerSecond, int spreadX, int spreadY, float maxMetaballs);
	void addMetaball(sf::Vector2f position, sf::Vector2f velocity, float lifespan, float weight, int spreadX, int spreadY);

	void update(float dt);
	void draw(sf::RenderWindow * window);

	void setColor(COLOR_AREA, float R, float G, float B, float A);

};

// Desc: An entity that spawns metaballs in a specified manner and adds them to the handler
//		handler: Handles the spawner and the metaballs produced by the spawner
//		position: A vector containing the x and y coordinates of the spawner
//		velocity: A vector containing the intial x and y velocity to spawn the metaballs at
//		spreadX: The amount of x velocity spread to apply to the metaballs
//		spreadY: The amount of y velocity spread to apply to the metaballs
//		weight: The weight each metaball will apply to its velocity each tick (can be negative)
//		lifespan: The amount of time before the metaball will be diposed of
//		maxMetaballs: The maximum amount of metaballs that can be spawned in ???
//		spawnPerSecond: The amount of metaballs to spawn per second
//		toSpawn: The amount of metaballs that should be spawned since the last tick
class MetaballSpawner {

	friend class MetaballHandler;

	MetaballHandler * handler;

	sf::Vector2f position;
	sf::Vector2f velocity;

	int spreadX;
	int spreadY;

	float weight;
	float lifespan;

	float maxMetaballs;
	int spawnPerSecond;
	float toSpawn;

	void spawn(float dt);

	MetaballSpawner(MetaballHandler *, sf::Vector2f position, sf::Vector2f velocity, float weight, float lifespan, int spawnPerSecond, int spreadX, int spreadY, float maxMetaballs);

};

#endif