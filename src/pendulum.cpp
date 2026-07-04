#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <numbers>
#include <cmath>
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
      angular_velocities.push_back(0);
    }
}

size_t Pendulum::n_segments() const{
  return angles.size();
}

struct DoublePendulumDerivatives {
  double d_theta_1;
  double d_theta_2;
  double d_omega_1;
  double d_omega_2;
};

DoublePendulumDerivatives lagrange_euler_for_2_arms(
  double m_1,
  double m_2,
  double l_1,
  double l_2,
  double omega_1,
  double omega_2,
  double theta_1,
  double theta_2
) {
  double cos_t1_t2 = std::cos(theta_1 - theta_2);
  double sin_t1_t2 = std::sin(theta_1 - theta_2);
  double sigma_1 = (m_2 * l_1 * l_2 * cos_t1_t2)/((m_1+m_2) * l_1 * l_1);
  double sigma_2 = (m_2 * l_1 * l_2 * cos_t1_t2)/(m_2 * l_2 * l_2);
  double sigma_1_by_cos = (m_2 * l_1 * l_2)/((m_1+m_2) * l_1 * l_1);
  double sigma_2_by_cos = (m_2 * l_1 * l_2)/(m_2 * l_2 * l_2);
  double g = 9.8;
  double sin_t1 = std::sin(theta_1);
  double sin_t2 = std::sin(theta_2);

  double f_1 = -g/l_1 * sin_t1 - sigma_1_by_cos * omega_2*omega_2 * sin_t1_t2;
  double f_2 = l_1/l_2 * omega_1*omega_1 * sin_t1_t2 - sigma_2_by_cos * g/l_1 * sin_t2;

  double d_theta_1 = omega_1;
  double d_theta_2 = omega_2;
  double d_omega_1 = (f_1 - sigma_1*f_2)/(1 - sigma_1*sigma_2);
  double d_omega_2 = (f_2 - sigma_2*f_1)/(1 - sigma_1*sigma_2);

  return DoublePendulumDerivatives{d_theta_1, d_theta_2, d_omega_1, d_omega_2};
}

void Pendulum::step(double dt) {
  if(n_segments() != 2){
    return;
  }

  // explicit Euler
  DoublePendulumDerivatives derivatives = lagrange_euler_for_2_arms(
    masses[0],
    masses[1],
    lengths[0],
    lengths[1],
    angular_velocities[0],
    angular_velocities[1],
    angles[0],
    angles[1]
  );

  angles[0] += derivatives.d_theta_1 * dt;
  angles[1] += derivatives.d_theta_2 * dt;
  angular_velocities[0] += derivatives.d_omega_1 * dt;
  angular_velocities[1] += derivatives.d_omega_2 * dt;
}
