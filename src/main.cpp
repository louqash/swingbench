#include <SFML/Graphics.hpp>
#include <SFML/System/Sleep.hpp>
#include <chrono>

#include "pendulum.hpp"
#include "renderer.hpp"

double FPS = 120;
float PIXELS_PER_METER = 300;

int main()
{
  sf::ContextSettings settings;
  settings.antiAliasingLevel = 8;
  sf::RenderWindow window( sf::VideoMode( { 1200, 900 } ), "swingbench",
                           sf::Style::Default, sf::State::Windowed, settings );
  Pendulum main_pendulum(2);
  Renderer renderer(window, PIXELS_PER_METER);

  sf::Clock clock; // starts the clock
	while ( window.isOpen() )
	{
		while ( const std::optional event = window.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() )
				window.close();
		}

    sf::Time elapsed = clock.getElapsedTime();
    double dt = elapsed.asSeconds();
    clock.restart();

    main_pendulum.step(dt);
    sf::Time sleep_time = std::chrono::milliseconds(
      static_cast<int>((1/FPS - dt) * 1000)
    );
    sf::sleep(sleep_time);

    double potential_energy = main_pendulum.potential_energy();
    double kinetic_energy = main_pendulum.kinetic_energy();

    renderer.draw(main_pendulum, potential_energy, kinetic_energy);
	}
}
