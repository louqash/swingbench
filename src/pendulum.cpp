#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <numbers>
#include "pendulum.hpp"

const double MATERIAL_DENSITY = 0.1; // kg per meter
const double FIRST_ARM_LENGTH = 0.5; // meters
const double delta_LENGTH = 0.1;     // meters

Pendulum::Pendulum(int n) {
    std::srand(std::time(nullptr));
    for(int i = 0; i < n; ++i) {
      double length = FIRST_ARM_LENGTH - i*delta_LENGTH;
      lengths.emplace_back(length);
      masses.emplace_back(length*MATERIAL_DENSITY);
      double random_number = static_cast<double>(std::rand()) / RAND_MAX;
      angles.emplace_back(random_number * std::numbers::pi - std::numbers::pi/2.); // from -pi/2 to pi/2
    }
}

size_t Pendulum::n_segments() const{
  return angles.size();
}
