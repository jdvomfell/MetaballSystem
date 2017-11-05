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

Metaball::Metaball(sf::Vector2f position, sf::Vector2f velocity, float lifespan, float weight) {

	this->position = position;
	this->velocity = velocity;
	this->lifespan = lifespan;
	this->weight = weight;

}

void MetaballHandler::update(float dt) {

	for (size_t i = 0; i < spawners.size(); i++) {
		spawners[i]->spawn(dt);
	}

	for (size_t i = 0; i < metaballList.size(); i++) {
		
		metaballList[i]->position += metaballList[i]->velocity;

		metaballList[i]->velocity.y += metaballList[i]->weight;

		if ((metaballList[i]->lifespan -= dt) <= 0)
			removeMetaball(i);

	}

}

void MetaballHandler::draw(sf::RenderWindow * window) {

	metaballAddTexture.clear(sf::Color::Transparent);
	metaballShadedTexture.clear(sf::Color::Transparent);

	for (size_t i = 0; i < metaballList.size(); i++) {

		if (metaballList[i]->lifespan < DEFAULT_SCALE)
			metaballSprite.setScale(sf::Vector2f(metaballList[i]->lifespan, metaballList[i]->lifespan));
		else
			metaballSprite.setScale(sf::Vector2f(DEFAULT_SCALE, DEFAULT_SCALE));

		metaballSprite.setPosition(metaballList[i]->position);

		metaballAddTexture.draw(metaballSprite, sf::BlendAdd);

	}

	metaballShadedTexture.draw(metaballAddSprite, &shader);

	window->draw(metaballShadedSprite);

}

void MetaballHandler::addSpawner(sf::Vector2f position, sf::Vector2f velocity, float weight, float lifespan, int spawnPerSecond, int spreadX, int spreadY, float maxMetaballs) {

	MetaballSpawner * spawner = new MetaballSpawner(this, position, velocity, weight, lifespan, spawnPerSecond, spreadX, spreadY, maxMetaballs);

	spawners.push_back(spawner);

}

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

void MetaballHandler::removeMetaball(int i) {

	delete(metaballList[i]);
	metaballList.erase(metaballList.begin() + i);

}

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

MetaballHandler::MetaballHandler() {

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

	setColor(INNER, 0, 0, 0, 0);
	setColor(OUTER, 0, 0, 0, 0);

}

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

// RGBA values between 0 and 1
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

}

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

void MetaballSpawner::spawn(float dt) {

	toSpawn += spawnPerSecond * maxMetaballs * dt;

	for (int i = 0; i < (int)toSpawn; i++) {
		handler->addMetaball(this->position, this->velocity, this->lifespan, this->weight, this->spreadX, this->spreadY);
		toSpawn--;
	}

}