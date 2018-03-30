#include "Metaball.h"

#include <chrono>
#include <thread>

const char metaballFragment[] =
"uniform float outer_threshold;"
"uniform float inner_threshold;"
"uniform sampler2D texture;"

"uniform float innerR;"
"uniform float innerG;"
"uniform float innerB;"
"uniform float innerA;"

"uniform float outerR;"
"uniform float outerG;"
"uniform float outerB;"
"uniform float outerA;"

"void main()"
"{"
"    vec4 pixel = gl_Color * texture2D(texture, gl_TexCoord[0].xy);"
"	 if (pixel.g >= inner_threshold) {"
"       pixel.r = innerR;"
"		pixel.g = innerG;"
"		pixel.b = innerB;"
"		pixel.a = innerA;"
"	 }" 
"	 else if (pixel.a >= outer_threshold) {"
"       pixel.r = outerR;"
"       pixel.g = outerG;"
"       pixel.b = outerB;"
"       pixel.a = outerA;"
"    }"
"    else {"
"       discard;"
"    }"
"    gl_FragColor = pixel;"
"}";

const char VertexShader[] =
"uniform sampler2D texture;"
"void main()"
"{"
"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
"gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
"gl_FrontColor = gl_Color;"
"}";

// Desc: Creates a new metaball
// position: Position to create the metaball at
// velocity: Initial velocity to spawn the metaball with
// lifespan: The amount of time before the metaball is cleaned
// weight: The weight to modify the metabll by at each timestep
Metaball::Metaball(sf::Vector2f position, sf::Vector2f velocity, float lifespan, float weight) {

	this->position = position;
	this->velocity = velocity;
	this->lifespan = lifespan;
	this->weight = weight;

}

// Desc: Updates all metaballs and spawners by the given time change
// dt: The time that has passed since the last update
void MetaballHandler::update(float dt) {

	/* Spawn New Metaballs */
	for (size_t i = 0; i < spawners.size(); i++) {
		spawners[i]->spawn(dt);
	}

	/* Update Old Metaballs */
	for (size_t i = 0; i < metaballList.size(); i++) {
		
		/* Clean Dead Metaballs */
		if ((metaballList[i]->lifespan -= dt) <= 0)
			removeMetaball(i);

		metaballList[i]->position += metaballList[i]->velocity * dt * 50.0f;

		metaballList[i]->velocity.y += metaballList[i]->weight * dt * 50.0f;

	}

}

// Desc: Draws all metaballs to the metaball texture and draws the image
// window: The window to draw the metaball texture to
void MetaballHandler::draw(sf::RenderWindow * window) {

	/* Reset The Metaball Texture */
	metaballAddTexture.clear(sf::Color::Transparent);
	metaballShadedTexture.clear(sf::Color::Transparent);

	/* Draw All Metaballs Modified By Lifespan */
	for (size_t i = 0; i < metaballList.size(); i++) {

		if (metaballList[i]->lifespan < DEFAULT_SCALE)
			metaballSprite.setScale(sf::Vector2f(metaballList[i]->lifespan, metaballList[i]->lifespan));
		else
			metaballSprite.setScale(sf::Vector2f(DEFAULT_SCALE, DEFAULT_SCALE));

		metaballSprite.setPosition(metaballList[i]->position);

		metaballAddTexture.draw(metaballSprite, sf::BlendAdd);

	}

	/* Move Texture To The Sprite */
	metaballShadedTexture.draw(metaballAddSprite, &shader);

	window->draw(metaballShadedSprite);

}

// Desc: Adds a spawner to the handler to be updated
// position: The position to spawn metaballs at
// velocity: The initial velocity to spawn metaballs at
// weight: The weight to modify the metaball by at each timestep
// lifespan: How many seconds the metaball will last
// spawnPerSecond: How many metaballs the spawner will spawn per second
// spreadX: The range - to + to randomly modify the initial metaball velocity in the X direction
// spreadY: The range - to + to randomly modify the initial metaball velocity in the Y direction
// maxMetaballs: The maximum amount of metaballs to be spawned by this spawner at a given time
void MetaballHandler::addSpawner(sf::Vector2f position, sf::Vector2f velocity, float weight, float lifespan, int spawnPerSecond, int spreadX, int spreadY, float maxMetaballs) {

	MetaballSpawner * spawner = new MetaballSpawner(this, position, velocity, weight, lifespan, spawnPerSecond, spreadX, spreadY, maxMetaballs);

	spawners.push_back(spawner);

}

// Desc: Adds a metaball of the given type to the handler
// position: The position to spawn metaballs at
// velocity: The initial velocity to spawn metaballs at
// lifespan: How many seconds the metaball will last
// weight: The weight to modify the metaball by at each timestep
// spreadX: The range - to + to randomly modify the initial metaball velocity in the X direction
// spreadY: The range - to + to randomly modify the initial metaball velocity in the Y direction
void MetaballHandler::addMetaball(sf::Vector2f position, sf::Vector2f velocity, float lifespan, float weight, int spreadX, int spreadY) {

	float xMod = (rand() % spreadX) * 0.10f;
	if (rand() % 2 == 1)
		xMod = -xMod;
	float yMod = (rand() % spreadY) * 0.10f;
	if (rand() % 2 == 1)
		yMod = -yMod;

	velocity += sf::Vector2f(xMod, yMod);
	Metaball * metaball = new Metaball(position, velocity, lifespan, weight);

	metaballList.push_back(metaball);

}

// Desc: Cleans and removes a metaball from the handler
// i: index of metabll to be removed
void MetaballHandler::removeMetaball(int i) {

	delete(metaballList[i]);
	metaballList.erase(metaballList.begin() + i);

}

// Desc: Deletes all metballs and spawners
void MetaballHandler::clean() {

	for (size_t i = 0; i < metaballList.size(); i++) {
		delete(metaballList[i]);
	}
	metaballList.clear();

	for (size_t i = 0; i < spawners.size(); i++) {
		delete(spawners[i]);
	}
	spawners.clear();

}

// Desc: Constructor to initialize the metaball handler
MetaballHandler::MetaballHandler() {

	/* Load and Modify The Metaball Texture */
	metaballTexture.loadFromFile("metaball.png");
	metaballSprite.setTexture(metaballTexture);
	metaballSprite.setOrigin(sf::Vector2f(metaballSprite.getLocalBounds().width / 2, metaballSprite.getLocalBounds().height / 2));
	metaballSprite.setScale(sf::Vector2f(DEFAULT_SCALE, DEFAULT_SCALE));

	if (!sf::Shader::isAvailable()) {
		printf("ERROR: Shaders Not Available\n");
		exit(1);
	}

	if (!shader.loadFromMemory(VertexShader, metaballFragment)) {
		printf("ERROR: Couldn't Load The Metaball Shader\n");
		exit(1);
	}

	shader.setUniform("outer_threshold", OUTER_THRESHOLD);
	shader.setUniform("inner_threshold", INNER_THRESHOLD);

	/* Initializes Metballs To Have A Color Of Black And No Opaqueness */
	setColor(ALL, 0, 0, 0, 0);

}

// Desc: Creates the required textures to produce the metaball effect for SFML
// viewSize: How large the viewport is where metaballs will be displayed
void MetaballHandler::init(sf::Vector2f viewSize) {

	metaballAddTexture.create((int)viewSize.x, (int)viewSize.y);
	metaballShadedTexture.create((int)viewSize.x, (int)viewSize.y);

	metaballAddSprite.setTexture(metaballAddTexture.getTexture());
	metaballShadedSprite.setTexture(metaballShadedTexture.getTexture());

}

// Desc: Resizes the texture that metaballs are drawn to, should be called when the
//			screen is resized.
// viewSize: The textures new size 
void MetaballHandler::resizeTexture(sf::Vector2f viewSize) {

	metaballAddTexture.create((int)viewSize.x, (int)viewSize.y);
	metaballShadedTexture.create((int)viewSize.x, (int)viewSize.y);

}

// Desc: Sets the color og the given color area to the given RGBA values
// colorArea: Inner or Outer areas determined by the metaball thresholds
// R, G, B, A: Red, Green, Blue, and Alpha values respectively (values between 0 and 1)
void MetaballHandler::setColor(COLOR_AREA colorArea, float R, float G, float B, float A) {

	if (colorArea == INNER) {
		this->shader.setUniform("innerR", R);
		this->shader.setUniform("innerG", G);
		this->shader.setUniform("innerB", B);
		this->shader.setUniform("innerA", A);
	}

	else if (colorArea == OUTER) {
		this->shader.setUniform("outerR", R);
		this->shader.setUniform("outerG", G);
		this->shader.setUniform("outerB", B);
		this->shader.setUniform("outerA", A);
	}

	else if (colorArea == ALL) {
		setColor(INNER, R, G, B, A);
		setColor(OUTER, R, G, B, A);
	}

}

// Desc: Contructor for defining a metaball spawner.
// handler: The handler which the metaballs will be controlled by
// position: The position to spawn metaballs at
// velocity: The initial velocity to spawn metaballs at
// weight: The weight to modify the metaball by at each timestep
// lifespan: How many seconds the metaball will last
// spawnPerSecond: How many metaballs the spawner will spawn per second
// spreadX: The range - to + to randomly modify the initial metaball velocity in the X direction
// spreadY: The range - to + to randomly modify the initial metaball velocity in the Y direction
// maxMetaballs: The maximum amount of metaballs to be spawned by this spawner at a given time
MetaballSpawner::MetaballSpawner(MetaballHandler * handler, sf::Vector2f position, sf::Vector2f velocity, float weight, float lifespan, int spawnPerSecond, int spreadX, int spreadY, float maxMetaballs) {

	this->handler = handler;

	this->position = position;
	this->velocity = velocity;
	this->weight = weight;
	this->lifespan = lifespan;
	this->spawnPerSecond = spawnPerSecond;
	this->spreadX = spreadX;
	this->spreadY = spreadY;
	this->maxMetaballs = maxMetaballs;

}

// Desc: Spawns new metaballs based on time passed, but will only spawn 
//          if there is room in the handler.
// dt: Time that has passed
void MetaballSpawner::spawn(float dt) {

    toSpawn += spawnPerSecond * maxMetaballs * dt;

	for (int i = 0; i < (int)toSpawn; i++) {
		handler->addMetaball(this->position, this->velocity, this->lifespan, this->weight, this->spreadX, this->spreadY);
		toSpawn--;
	}

}