#include <SFML/Graphics.hpp>

#include "Metaball.h"

// ToDo: 
// Add scalability of metaballs
// Modify to return drawables 
// Color all enum and functionality

int main()
{

	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(1500, 1200), "SFML works!");

	window.setFramerateLimit(60);

	MetaballHandler metaballHandler;
	metaballHandler.init(window.getView().getSize());
	metaballHandler.setColor(INNER, 1.0, 0.7, 0.1, 1);
	metaballHandler.setColor(OUTER, 0, 0, 0, 1);

	metaballHandler.addSpawner(sf::Vector2f(300, 1000), sf::Vector2f(0, 0.4f), -0.05f, 3.0f, 10, 5, 5, 1);
	metaballHandler.addSpawner(sf::Vector2f(600, 400), sf::Vector2f(0, -0.4f), 0.02f, 3.0f, 10, 5, 5, 1);
	metaballHandler.addSpawner(sf::Vector2f(1000, 400), sf::Vector2f(0, -3.5f), 0.02f, 6.0f, 3, 5, 5, 1);
	metaballHandler.addSpawner(sf::Vector2f(900, 900), sf::Vector2f(0, 0.0f), 0.00f, 0.5f, 20, 80, 80, 1);

	sf::Clock clock;
	sf::Time delta;
	float dt;

	sf::Event event;

	while (window.isOpen())
	{

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				metaballHandler.clean();
				window.close();
				exit(0);
			}
		}

		delta = clock.getElapsedTime();
		clock.restart();
		dt = delta.asSeconds();

		metaballHandler.update(dt);

		//RENDER
		window.clear(sf::Color::White);
		metaballHandler.draw(&window);
		window.display();
	}

	return 0;
}