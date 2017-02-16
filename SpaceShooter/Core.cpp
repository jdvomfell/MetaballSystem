#include <SFML/Graphics.hpp>

#include "Metaball.h"

int main()
{

	srand((int)time(NULL));

	int randX;
	int randY;

	sf::RenderWindow window(sf::VideoMode(1500, 1200), "SFML works!");

	window.setFramerateLimit(60);

	MetaballHandler metaballHandler(window.getSize());

	/*for (int i = 0; i < 50; i++) {
		randX = rand() % 1500 + 1;
		randY = rand() % 1200 + 1;
		metaballHandler.addMetaball(sf::Vector2f((float)randX, (float)randY));
	}*/

	MetaballSpawner metaballSpawner(&metaballHandler, sf::Vector2f(300, 1000), sf::Vector2f(0, 0.4f), -0.05f, 3.0f, 10, 5, 5);
	MetaballSpawner metaballSpawner1(&metaballHandler, sf::Vector2f(600, 400), sf::Vector2f(0, -0.4f), 0.02f, 3.0f, 10, 5, 5);
	MetaballSpawner metaballSpawner2(&metaballHandler, sf::Vector2f(1000, 400), sf::Vector2f(0, -3.5f), 0.02f, 6.0f, 3, 5, 5);
	MetaballSpawner metaballSpawner3(&metaballHandler, sf::Vector2f(900, 900), sf::Vector2f(0, 0.0f), 0.00f, 0.5f, 30, 80, 80);
	MetaballSpawner metaballSpawner4(&metaballHandler, sf::Vector2f(1300, 1200), sf::Vector2f(0, 0.0f), 0.00f, 5.0f, 70, 5, 300);

	printf("Spawner Made\n");

	sf::Clock clock;
	sf::Time delta;
	float dt;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
				exit(0);
			}
		}

		delta = clock.getElapsedTime();
		clock.restart();
		dt = delta.asSeconds();

		/*int j;
		for (int i = 0; i < metaballHandler.metaballList.size(); i++) {
			j = i % 4;
			if (j == 0) {
				metaballHandler.metaballList[i]->velocity.x = 0.5f;
			}
			else if (j == 1) {
				metaballHandler.metaballList[i]->velocity.y = 0.5f;
			}
			else if (j == 3) {
				metaballHandler.metaballList[i]->velocity.x = -0.5f;
			}
			else {
				metaballHandler.metaballList[i]->velocity.y = -0.5f;
			}
		}*/

		metaballSpawner.spawn(dt);
		metaballSpawner1.spawn(dt);
		metaballSpawner2.spawn(dt);
		metaballSpawner3.spawn(dt);
		metaballSpawner4.spawn(dt);
		metaballHandler.update(dt);

		//RENDER
		//printf("YO");
		window.clear(sf::Color::White);
		metaballHandler.draw(&window);
		window.display();
	}

	return 0;
}