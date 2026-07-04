#include "renderer.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {

const sf::Color BACKGROUND{15, 18, 32};
const sf::Color GRID_LINE{255, 255, 255, 14};
const sf::Color ARM_COLOR{198, 205, 224};
const sf::Color HINGE_COLOR{120, 128, 152};
const sf::Color BOB_COLORS[] = {
  sf::Color{255, 179, 71},   // warm amber
  sf::Color{86, 204, 242},   // cyan
  sf::Color{190, 144, 255},  // violet (3rd arm onwards cycles)
};
const sf::Color TRAIL_COLOR{86, 204, 242};
const sf::Color TEXT_COLOR{215, 220, 235};
const sf::Color TEXT_DIM{140, 148, 170};
const sf::Color PE_COLOR{100, 150, 255};
const sf::Color KE_COLOR{255, 140, 90};

const float GRID_SPACING_METERS = 0.25f;
const std::size_t TRAIL_LENGTH = 900;

// HUD panel geometry (window is 1200x900; the world area is everything left
// of the panel).
const float PANEL_X = 935.f;
const float PANEL_Y = 30.f;
const float PANEL_W = 235.f;
const float PANEL_H = 620.f;
const float GAUGE_TOP = 110.f;
const float GAUGE_HEIGHT = 400.f;
const float GAUGE_WIDTH = 62.f;

const char* FONT_CANDIDATES[] = {
  "/System/Library/Fonts/Menlo.ttc",
  "/System/Library/Fonts/Monaco.ttf",
  "/System/Library/Fonts/Helvetica.ttc",
  "/System/Library/Fonts/Supplemental/Arial.ttf",
};

std::string formatJoules(double value) {
  char buffer[32];
  std::snprintf(buffer, sizeof(buffer), "%+.3f J", value);
  return buffer;
}

} // namespace

Renderer::Renderer(sf::RenderWindow& window, float pixels_per_meter)
    : window_(window), ppm_(pixels_per_meter) {
  // Center the pivot in the world area left of the HUD panel.
  pivot_ = {(PANEL_X - 15.f) / 2.f, window_.getSize().y / 2.f};

  for (const char* path : FONT_CANDIDATES) {
    if (font_.openFromFile(path)) {
      has_font_ = true;
      break;
    }
  }
}

void Renderer::draw(const Pendulum& pendulum,
                    double potential_energy,
                    double kinetic_energy) {
  window_.clear(BACKGROUND);

  drawGrid();
  drawTrail();
  drawPendulum(pendulum);
  drawScaleRuler();
  drawGauges(potential_energy, kinetic_energy);

  window_.display();
}

void Renderer::drawGrid() {
  sf::Vector2u size = window_.getSize();
  float spacing = GRID_SPACING_METERS * ppm_;

  std::vector<sf::Vertex> lines;
  // Grid lines fan out from the pivot so they mark whole fractions of a
  // meter from the suspension point.
  for (float x = std::fmod(pivot_.x, spacing); x < size.x; x += spacing) {
    lines.push_back(sf::Vertex{{x, 0.f}, GRID_LINE});
    lines.push_back(sf::Vertex{{x, static_cast<float>(size.y)}, GRID_LINE});
  }
  for (float y = std::fmod(pivot_.y, spacing); y < size.y; y += spacing) {
    lines.push_back(sf::Vertex{{0.f, y}, GRID_LINE});
    lines.push_back(sf::Vertex{{static_cast<float>(size.x), y}, GRID_LINE});
  }
  window_.draw(lines.data(), lines.size(), sf::PrimitiveType::Lines);
}

void Renderer::drawTrail() {
  if (trail_.size() < 2)
    return;

  std::vector<sf::Vertex> strip;
  strip.reserve(trail_.size());
  for (std::size_t i = 0; i < trail_.size(); ++i) {
    float age = static_cast<float>(i) / trail_.size(); // 0 = oldest
    sf::Color c = TRAIL_COLOR;
    c.a = static_cast<std::uint8_t>(150.f * age * age);
    strip.push_back(sf::Vertex{trail_[i], c});
  }
  window_.draw(strip.data(), strip.size(), sf::PrimitiveType::LineStrip);
}

void Renderer::drawPendulum(const Pendulum& pendulum) {
  // Forward kinematics (same convention as the physics: theta measured from
  // straight down, y grows downward on screen).
  std::vector<sf::Vector2f> hinges{pivot_};
  for (std::size_t i = 0; i < pendulum.n_segments(); ++i) {
    sf::Vector2f prev = hinges.back();
    hinges.push_back({
      static_cast<float>(prev.x + std::sin(pendulum.angles[i]) * pendulum.lengths[i] * ppm_),
      static_cast<float>(prev.y + std::cos(pendulum.angles[i]) * pendulum.lengths[i] * ppm_),
    });
  }

  // Record the tip for the trail.
  trail_.push_back(hinges.back());
  if (trail_.size() > TRAIL_LENGTH)
    trail_.pop_front();

  // Ceiling mount: a short bar above the pivot.
  sf::RectangleShape mount({46.f, 5.f});
  mount.setOrigin({23.f, 2.5f});
  mount.setPosition({pivot_.x, pivot_.y - 12.f});
  mount.setFillColor(HINGE_COLOR);
  window_.draw(mount);

  // Arms as thick rotated rectangles.
  for (std::size_t i = 0; i < pendulum.n_segments(); ++i) {
    sf::Vector2f a = hinges[i];
    sf::Vector2f b = hinges[i + 1];
    sf::Vector2f d = b - a;
    float length = std::sqrt(d.x * d.x + d.y * d.y);

    sf::RectangleShape arm({length, 6.f});
    arm.setOrigin({0.f, 3.f});
    arm.setPosition(a);
    arm.setRotation(sf::radians(std::atan2(d.y, d.x)));
    arm.setFillColor(ARM_COLOR);
    window_.draw(arm);
  }

  // Hinge pins on top of the arms.
  for (std::size_t i = 0; i + 1 < hinges.size(); ++i) {
    sf::CircleShape pin(5.f);
    pin.setOrigin({5.f, 5.f});
    pin.setPosition(hinges[i]);
    pin.setFillColor(HINGE_COLOR);
    pin.setOutlineThickness(2.f);
    pin.setOutlineColor(BACKGROUND);
    window_.draw(pin);
  }

  // Bobs at the arm ends, radius grown with the cube root of the mass so
  // volume tracks mass.
  for (std::size_t i = 0; i < pendulum.n_segments(); ++i) {
    float radius = std::max(
      7.f, 36.f * static_cast<float>(std::cbrt(pendulum.masses[i])));
    const sf::Color& color =
      BOB_COLORS[i % (sizeof(BOB_COLORS) / sizeof(BOB_COLORS[0]))];

    sf::CircleShape bob(radius);
    bob.setOrigin({radius, radius});
    bob.setPosition(hinges[i + 1]);
    bob.setFillColor(color);
    bob.setOutlineThickness(2.5f);
    bob.setOutlineColor(sf::Color{255, 255, 255, 60});
    window_.draw(bob);
  }
}

void Renderer::drawScaleRuler() {
  const float ruler_meters = 0.5f;
  const float ruler_px = ruler_meters * ppm_;
  const sf::Vector2f origin{40.f, window_.getSize().y - 50.f};

  std::vector<sf::Vertex> lines;
  lines.push_back(sf::Vertex{origin, TEXT_DIM});
  lines.push_back(sf::Vertex{{origin.x + ruler_px, origin.y}, TEXT_DIM});
  // End ticks plus one every 0.1 m.
  for (float m = 0.f; m <= ruler_meters + 1e-4f; m += 0.1f) {
    float x = origin.x + m * ppm_;
    float tick = (m == 0.f || m >= ruler_meters - 1e-4f) ? 10.f : 5.f;
    lines.push_back(sf::Vertex{{x, origin.y - tick}, TEXT_DIM});
    lines.push_back(sf::Vertex{{x, origin.y + tick}, TEXT_DIM});
  }
  window_.draw(lines.data(), lines.size(), sf::PrimitiveType::Lines);

  drawText("0.5 m", {origin.x + ruler_px / 2.f, origin.y - 32.f}, 15,
           TEXT_COLOR, true);
  char label[48];
  std::snprintf(label, sizeof(label), "%.0f px = 1 m", static_cast<double>(ppm_));
  drawText(label, {origin.x + ruler_px / 2.f, origin.y + 12.f}, 13,
           TEXT_DIM, true);
}

void Renderer::drawGauges(double potential_energy, double kinetic_energy) {
  // Panel background.
  sf::RectangleShape panel({PANEL_W, PANEL_H});
  panel.setPosition({PANEL_X, PANEL_Y});
  panel.setFillColor(sf::Color{255, 255, 255, 10});
  panel.setOutlineThickness(1.f);
  panel.setOutlineColor(sf::Color{255, 255, 255, 25});
  window_.draw(panel);

  drawText("ENERGY", {PANEL_X + PANEL_W / 2.f, PANEL_Y + 18.f}, 16,
           TEXT_COLOR, true);

  // Both gauges share one running range so bar heights are comparable, and
  // the range always contains zero (PE may be negative depending on the
  // reference height).
  energy_min_ = std::min({energy_min_, potential_energy, kinetic_energy});
  energy_max_ = std::max({energy_max_, potential_energy, kinetic_energy});
  double lo = std::min(energy_min_, 0.0);
  double hi = std::max(energy_max_, 1e-9);
  double range = hi - lo;

  auto value_to_y = [&](double v) {
    return static_cast<float>(GAUGE_TOP + GAUGE_HEIGHT * (hi - v) / range);
  };
  float zero_y = value_to_y(0.0);

  struct Gauge {
    const char* name;
    double value;
    sf::Color color;
    float x;
  };
  const float gauge_gap = (PANEL_W - 2 * GAUGE_WIDTH) / 3.f;
  Gauge gauges[] = {
    {"PE", potential_energy, PE_COLOR, PANEL_X + gauge_gap},
    {"KE", kinetic_energy, KE_COLOR, PANEL_X + 2 * gauge_gap + GAUGE_WIDTH},
  };

  for (const Gauge& g : gauges) {
    // Track outline.
    sf::RectangleShape track({GAUGE_WIDTH, GAUGE_HEIGHT});
    track.setPosition({g.x, GAUGE_TOP});
    track.setFillColor(sf::Color{0, 0, 0, 60});
    track.setOutlineThickness(1.f);
    track.setOutlineColor(sf::Color{255, 255, 255, 40});
    window_.draw(track);

    // Fill from the zero line toward the value (up for positive, down for
    // negative).
    float vy = value_to_y(g.value);
    sf::RectangleShape fill({GAUGE_WIDTH - 6.f, std::abs(vy - zero_y)});
    fill.setPosition({g.x + 3.f, std::min(vy, zero_y)});
    fill.setFillColor(g.color);
    window_.draw(fill);

    drawText(g.name, {g.x + GAUGE_WIDTH / 2.f, GAUGE_TOP - 26.f}, 15,
             g.color, true);
    drawText(formatJoules(g.value),
             {g.x + GAUGE_WIDTH / 2.f, GAUGE_TOP + GAUGE_HEIGHT + 10.f}, 13,
             TEXT_COLOR, true);
  }

  // Zero line across both gauges.
  std::vector<sf::Vertex> zero_line{
    sf::Vertex{{PANEL_X + gauge_gap - 8.f, zero_y}, sf::Color{255, 255, 255, 90}},
    sf::Vertex{{PANEL_X + PANEL_W - gauge_gap + 8.f, zero_y}, sf::Color{255, 255, 255, 90}},
  };
  window_.draw(zero_line.data(), zero_line.size(), sf::PrimitiveType::Lines);
  drawText("0", {PANEL_X + 8.f, zero_y - 8.f}, 12, TEXT_DIM);

  // Total mechanical energy readout.
  drawText("E = PE + KE", {PANEL_X + PANEL_W / 2.f, GAUGE_TOP + GAUGE_HEIGHT + 44.f},
           13, TEXT_DIM, true);
  drawText(formatJoules(potential_energy + kinetic_energy),
           {PANEL_X + PANEL_W / 2.f, GAUGE_TOP + GAUGE_HEIGHT + 62.f}, 15,
           TEXT_COLOR, true);
}

void Renderer::drawText(const std::string& str, sf::Vector2f position,
                        unsigned size, sf::Color color,
                        bool center_horizontally) {
  if (!has_font_)
    return;
  sf::Text text(font_, str, size);
  text.setFillColor(color);
  if (center_horizontally) {
    sf::FloatRect bounds = text.getLocalBounds();
    position.x -= bounds.size.x / 2.f;
  }
  text.setPosition(position);
  window_.draw(text);
}
