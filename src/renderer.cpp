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

// Layout in design pixels: laid out on a 1200x900 canvas, px() scales to the
// actual framebuffer (2x on retina).

constexpr float GRID_SPACING_METERS = 0.25f;
constexpr std::size_t TRAIL_LENGTH = 900;

// HUD panel; the world area is everything left of it.
constexpr float PANEL_X = 935.f;
constexpr float PANEL_Y = 30.f;
constexpr float PANEL_W = 235.f;
constexpr float PANEL_H = 620.f;
constexpr float GAUGE_TOP = 110.f;
constexpr float GAUGE_HEIGHT = 400.f;
constexpr float GAUGE_WIDTH = 62.f;
constexpr float WORLD_RIGHT_PAD = 15.f; // gap between world area and panel

// Pendulum drawing.
constexpr float ARM_THICKNESS = 6.f;
constexpr float PIN_RADIUS = 5.f;
constexpr float PIN_OUTLINE = 2.f;
constexpr float BOB_MIN_RADIUS = 7.f;
constexpr float BOB_RADIUS_PER_CBRT_KG = 36.f;
constexpr float BOB_OUTLINE = 2.5f;
constexpr float MOUNT_W = 46.f;
constexpr float MOUNT_H = 5.f;
constexpr float MOUNT_LIFT = 12.f;

// Thin lines (grid, trail, ruler, zero line).
constexpr float HAIRLINE = 1.f;

// Font sizes.
constexpr unsigned FONT_TITLE = 16;
constexpr unsigned FONT_LABEL = 15;
constexpr unsigned FONT_SMALL = 13;
constexpr unsigned FONT_TINY = 12;

const char* FONT_CANDIDATES[] = {
  // macOS
  "/System/Library/Fonts/Menlo.ttc",
  "/System/Library/Fonts/Monaco.ttf",
  "/System/Library/Fonts/Helvetica.ttc",
  "/System/Library/Fonts/Supplemental/Arial.ttf",
  // Linux (Debian/Ubuntu, Fedora, Arch)
  "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
  "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
  "/usr/share/fonts/liberation/LiberationMono-Regular.ttf",
  "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
};

std::string formatJoules(double value) {
  char buffer[32];
  std::snprintf(buffer, sizeof(buffer), "%+.3f J", value);
  return buffer;
}

// GL line primitives are always 1 device pixel, so thick lines are triangles.
void appendSegment(std::vector<sf::Vertex>& out, sf::Vector2f a,
                   sf::Vector2f b, float thickness, sf::Color color_a,
                   sf::Color color_b) {
  sf::Vector2f d = b - a;
  float length = std::sqrt(d.x * d.x + d.y * d.y);
  if (length <= 0.f)
    return;
  sf::Vector2f n{-d.y / length * thickness / 2.f,
                 d.x / length * thickness / 2.f};
  out.push_back({a - n, color_a});
  out.push_back({b - n, color_b});
  out.push_back({b + n, color_b});
  out.push_back({a - n, color_a});
  out.push_back({b + n, color_b});
  out.push_back({a + n, color_a});
}

void appendSegment(std::vector<sf::Vertex>& out, sf::Vector2f a,
                   sf::Vector2f b, float thickness, sf::Color color) {
  appendSegment(out, a, b, thickness, color, color);
}

} // namespace

Renderer::Renderer(sf::RenderWindow& window, float pixels_per_meter,
                   float ui_scale)
    : window_(window), ppm_(pixels_per_meter), ui_(ui_scale) {
  // Center the pivot in the world area left of the HUD panel.
  pivot_ = {px(PANEL_X - WORLD_RIGHT_PAD) / 2.f, window_.getSize().y / 4.f};

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
  float thickness = px(HAIRLINE);

  std::vector<sf::Vertex> lines;
  // Grid lines fan out from the pivot so they mark whole fractions of a
  // meter from the suspension point.
  for (float x = std::fmod(pivot_.x, spacing); x < size.x; x += spacing)
    appendSegment(lines, {x, 0.f}, {x, static_cast<float>(size.y)},
                  thickness, GRID_LINE);
  for (float y = std::fmod(pivot_.y, spacing); y < size.y; y += spacing)
    appendSegment(lines, {0.f, y}, {static_cast<float>(size.x), y},
                  thickness, GRID_LINE);
  window_.draw(lines.data(), lines.size(), sf::PrimitiveType::Triangles);
}

void Renderer::drawTrail() {
  if (trail_.size() < 2)
    return;

  auto fade = [&](std::size_t i) {
    float age = static_cast<float>(i) / trail_.size(); // 0 = oldest
    sf::Color c = TRAIL_COLOR;
    c.a = static_cast<std::uint8_t>(150.f * age * age);
    return c;
  };

  std::vector<sf::Vertex> segments;
  segments.reserve((trail_.size() - 1) * 6);
  for (std::size_t i = 0; i + 1 < trail_.size(); ++i)
    appendSegment(segments, trail_[i], trail_[i + 1], px(HAIRLINE),
                  fade(i), fade(i + 1));
  window_.draw(segments.data(), segments.size(),
               sf::PrimitiveType::Triangles);
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
  // sf::RectangleShape mount({px(MOUNT_W), px(MOUNT_H)});
  // mount.setOrigin({px(MOUNT_W) / 2.f, px(MOUNT_H) / 2.f});
  // mount.setPosition({pivot_.x, pivot_.y - px(MOUNT_LIFT)});
  // mount.setFillColor(HINGE_COLOR);
  // window_.draw(mount);

  // Arms as thick rotated rectangles.
  for (std::size_t i = 0; i < pendulum.n_segments(); ++i) {
    sf::Vector2f a = hinges[i];
    sf::Vector2f b = hinges[i + 1];
    sf::Vector2f d = b - a;
    float length = std::sqrt(d.x * d.x + d.y * d.y);

    sf::RectangleShape arm({length, px(ARM_THICKNESS)});
    arm.setOrigin({0.f, px(ARM_THICKNESS) / 2.f});
    arm.setPosition(a);
    arm.setRotation(sf::radians(std::atan2(d.y, d.x)));
    arm.setFillColor(ARM_COLOR);
    window_.draw(arm);
  }

  // Hinge pins on top of the arms.
  for (std::size_t i = 0; i + 1 < hinges.size(); ++i) {
    sf::CircleShape pin(px(PIN_RADIUS));
    pin.setOrigin({px(PIN_RADIUS), px(PIN_RADIUS)});
    pin.setPosition(hinges[i]);
    pin.setFillColor(HINGE_COLOR);
    pin.setOutlineThickness(px(PIN_OUTLINE));
    pin.setOutlineColor(BACKGROUND);
    window_.draw(pin);
  }

  // Bobs at the arm ends, radius grown with the cube root of the mass so
  // volume tracks mass.
  for (std::size_t i = 0; i < pendulum.n_segments(); ++i) {
    float radius = px(std::max(
      BOB_MIN_RADIUS,
      BOB_RADIUS_PER_CBRT_KG * static_cast<float>(std::cbrt(pendulum.masses[i]))));
    const sf::Color& color =
      BOB_COLORS[i % (sizeof(BOB_COLORS) / sizeof(BOB_COLORS[0]))];

    sf::CircleShape bob(radius);
    bob.setOrigin({radius, radius});
    bob.setPosition(hinges[i + 1]);
    bob.setFillColor(color);
    bob.setOutlineThickness(px(BOB_OUTLINE));
    bob.setOutlineColor(sf::Color{255, 255, 255, 60});
    window_.draw(bob);
  }
}

void Renderer::drawScaleRuler() {
  const float ruler_meters = 0.5f;
  const float ruler_px = ruler_meters * ppm_;
  const sf::Vector2f origin{px(40.f), window_.getSize().y - px(50.f)};
  const float thickness = px(HAIRLINE);

  std::vector<sf::Vertex> lines;
  appendSegment(lines, origin, {origin.x + ruler_px, origin.y}, thickness,
                TEXT_DIM);
  // End ticks plus one every 0.1 m.
  for (float m = 0.f; m <= ruler_meters + 1e-4f; m += 0.1f) {
    float x = origin.x + m * ppm_;
    float tick = px((m == 0.f || m >= ruler_meters - 1e-4f) ? 10.f : 5.f);
    appendSegment(lines, {x, origin.y - tick}, {x, origin.y + tick},
                  thickness, TEXT_DIM);
  }
  window_.draw(lines.data(), lines.size(), sf::PrimitiveType::Triangles);

  drawText("0.5 m", {origin.x + ruler_px / 2.f, origin.y - px(32.f)},
           FONT_LABEL, TEXT_COLOR, true);
  char label[48];
  std::snprintf(label, sizeof(label), "%.0f px = 1 m", static_cast<double>(ppm_));
  drawText(label, {origin.x + ruler_px / 2.f, origin.y + px(12.f)},
           FONT_SMALL, TEXT_DIM, true);
}

void Renderer::drawGauges(double potential_energy, double kinetic_energy) {
  const float panel_x = px(PANEL_X);
  const float panel_y = px(PANEL_Y);
  const float panel_w = px(PANEL_W);
  const float panel_h = px(PANEL_H);
  const float gauge_top = px(GAUGE_TOP);
  const float gauge_height = px(GAUGE_HEIGHT);
  const float gauge_width = px(GAUGE_WIDTH);

  // Panel background.
  sf::RectangleShape panel({panel_w, panel_h});
  panel.setPosition({panel_x, panel_y});
  panel.setFillColor(sf::Color{255, 255, 255, 10});
  panel.setOutlineThickness(px(HAIRLINE));
  panel.setOutlineColor(sf::Color{255, 255, 255, 25});
  window_.draw(panel);

  drawText("ENERGY", {panel_x + panel_w / 2.f, panel_y + px(18.f)},
           FONT_TITLE, TEXT_COLOR, true);

  // Both gauges share one running range so bar heights are comparable, and
  // the range always contains zero (PE may be negative depending on the
  // reference height).
  energy_min_ = std::min({energy_min_, potential_energy, kinetic_energy});
  energy_max_ = std::max({energy_max_, potential_energy, kinetic_energy});
  double lo = std::min(energy_min_, 0.0);
  double hi = std::max(energy_max_, 1e-9);
  double range = hi - lo;

  auto value_to_y = [&](double v) {
    return static_cast<float>(gauge_top + gauge_height * (hi - v) / range);
  };
  float zero_y = value_to_y(0.0);

  struct Gauge {
    const char* name;
    double value;
    sf::Color color;
    float x;
  };
  const float gauge_gap = (panel_w - 2 * gauge_width) / 3.f;
  Gauge gauges[] = {
    {"PE", potential_energy, PE_COLOR, panel_x + gauge_gap},
    {"KE", kinetic_energy, KE_COLOR, panel_x + 2 * gauge_gap + gauge_width},
  };

  for (const Gauge& g : gauges) {
    // Track outline.
    sf::RectangleShape track({gauge_width, gauge_height});
    track.setPosition({g.x, gauge_top});
    track.setFillColor(sf::Color{0, 0, 0, 60});
    track.setOutlineThickness(px(HAIRLINE));
    track.setOutlineColor(sf::Color{255, 255, 255, 40});
    window_.draw(track);

    // Fill from the zero line toward the value (up for positive, down for
    // negative).
    float vy = value_to_y(g.value);
    sf::RectangleShape fill({gauge_width - px(6.f), std::abs(vy - zero_y)});
    fill.setPosition({g.x + px(3.f), std::min(vy, zero_y)});
    fill.setFillColor(g.color);
    window_.draw(fill);

    drawText(g.name, {g.x + gauge_width / 2.f, gauge_top - px(26.f)},
             FONT_LABEL, g.color, true);
    drawText(formatJoules(g.value),
             {g.x + gauge_width / 2.f, gauge_top + gauge_height + px(10.f)},
             FONT_SMALL, TEXT_COLOR, true);
  }

  // Zero line across both gauges.
  std::vector<sf::Vertex> zero_line;
  appendSegment(zero_line, {panel_x + gauge_gap - px(8.f), zero_y},
                {panel_x + panel_w - gauge_gap + px(8.f), zero_y},
                px(HAIRLINE), sf::Color{255, 255, 255, 90});
  window_.draw(zero_line.data(), zero_line.size(),
               sf::PrimitiveType::Triangles);
  drawText("0", {panel_x + px(8.f), zero_y - px(8.f)}, FONT_TINY, TEXT_DIM);

  // Total mechanical energy readout.
  drawText("E = PE + KE",
           {panel_x + panel_w / 2.f, gauge_top + gauge_height + px(44.f)},
           FONT_SMALL, TEXT_DIM, true);
  drawText(formatJoules(potential_energy + kinetic_energy),
           {panel_x + panel_w / 2.f, gauge_top + gauge_height + px(62.f)},
           FONT_LABEL, TEXT_COLOR, true);
}

void Renderer::drawText(const std::string& str, sf::Vector2f position,
                        unsigned size, sf::Color color,
                        bool center_horizontally) {
  if (!has_font_)
    return;
  // scale the char size itself — scaling rendered glyphs blurs them
  sf::Text text(font_, str, static_cast<unsigned>(std::lround(size * ui_)));
  text.setFillColor(color);
  if (center_horizontally) {
    sf::FloatRect bounds = text.getLocalBounds();
    position.x -= bounds.size.x / 2.f;
  }
  text.setPosition(position);
  window_.draw(text);
}
