#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include "pendulum.hpp"

float PIXELS_PER_METER = 100;

int main()
{
	sf::RenderWindow window( sf::VideoMode( { 400, 400 } ), "Pendulum Simulation" );
  Pendulum main_pendulum(2);

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

		window.clear();
    window.draw(lines.data(), lines.size(), sf::PrimitiveType::Lines);
		window.display();
	}
}
