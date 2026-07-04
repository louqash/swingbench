#include <SFML/Graphics.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>

#include "pendulum.hpp"

double FPS = 120;
float PIXELS_PER_METER = 100;

int main()
{
	sf::RenderWindow window( sf::VideoMode( { 400, 400 } ), "Pendulum Simulation" );
  Pendulum main_pendulum(2);

  sf::Clock clock; // starts the clock
  std::vector<sf::Vertex> lines(main_pendulum.n_segments()*2, sf::Vertex{});
	while ( window.isOpen() )
	{
		while ( const std::optional event = window.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() )
				window.close();
		}

    sf::Vector2f last_hinge = {200, 10};
    for(size_t i = 0; i < main_pendulum.n_segments(); ++i) {
      sf::Vector2f next_hinge = {
        static_cast<float>(last_hinge.x + sin(main_pendulum.angles[i])*main_pendulum.lengths[i]*PIXELS_PER_METER),
        static_cast<float>(last_hinge.y + cos(main_pendulum.angles[i])*main_pendulum.lengths[i]*PIXELS_PER_METER),
      };
      lines[i*2] = sf::Vertex{last_hinge};
      lines[i*2 + 1] = sf::Vertex{next_hinge};

      last_hinge = next_hinge;
    }

    sf::Time elapsed = clock.getElapsedTime();
    double dt = elapsed.asSeconds();
    clock.restart();

    main_pendulum.step(dt);
    sf::Time sleep_time = std::chrono::milliseconds(
      static_cast<int>((1/FPS - dt) * 1000)
    );
    sf::sleep(sleep_time);

		window.clear();
    window.draw(lines.data(), lines.size(), sf::PrimitiveType::Lines);
		window.display();
	}
}
