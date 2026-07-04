#include <SFML/Graphics.hpp>
#include <SFML/System/Sleep.hpp>
#include <chrono>

#include "pendulum.hpp"
#include "renderer.hpp"

double FPS = 120;
float PIXELS_PER_METER = 300; // at 1x
const sf::Vector2u WINDOW_SIZE_POINTS{ 1200, 900 };

int main()
{
  sf::ContextSettings settings;
  settings.antiAliasingLevel = 8;
  sf::RenderWindow window( sf::VideoMode( WINDOW_SIZE_POINTS ), "swingbench",
                           sf::Style::Default, sf::State::Windowed, settings );

  // retina gives a 2x framebuffer (needs the SFML patch, see
  // cmake/patch_sfml_highdpi.cmake), X11 gives 1x
  float ui_scale = static_cast<float>(window.getSize().x) / WINDOW_SIZE_POINTS.x;

  Pendulum main_pendulum(2);
  Renderer renderer(window, PIXELS_PER_METER * ui_scale, ui_scale);

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
