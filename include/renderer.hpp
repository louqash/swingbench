#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <string>

#include "pendulum.hpp"

// Draws one frame: background grid, the pendulum itself, a fading trail on
// the tip, a pixels-per-meter scale ruler, and PE/KE gauges. Owns no physics —
// energy values are passed in and rendered as-is.
class Renderer {
public:
  Renderer(sf::RenderWindow& window, float pixels_per_meter);

  void draw(const Pendulum& pendulum,
            double potential_energy,
            double kinetic_energy);

private:
  sf::RenderWindow& window_;
  float ppm_;
  sf::Vector2f pivot_;
  sf::Font font_;
  bool has_font_ = false;

  std::deque<sf::Vector2f> trail_;

  // Running range shared by both gauges so their heights are comparable.
  double energy_min_ = 0.0;
  double energy_max_ = 0.0;

  void drawGrid();
  void drawTrail();
  void drawPendulum(const Pendulum& pendulum);
  void drawScaleRuler();
  void drawGauges(double potential_energy, double kinetic_energy);
  void drawText(const std::string& str, sf::Vector2f position, unsigned size,
                sf::Color color, bool center_horizontally = false);
};
