#include <array>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <numbers>
#include <vector>
#include <cmath>
#include "pendulum.hpp"

const double MATERIAL_DENSITY = 0.1; // kg per meter
const double FIRST_ARM_LENGTH = 0.5; // meters
const double delta_LENGTH = 0.05;     // meters
const double g = 9.8;

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

std::vector<double> generate_M(
  std::vector<double> masses,
  std::vector<double> lengths,
  std::vector<double> angles
) {
  // M_{ij} = \Big( \sum_{k = \max(i,j)}^{n}{m_k} \Big) l_i l_j \cos(\theta_i - \theta_j)
  size_t N = masses.size();
  std::vector<double> M(N*N);
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      double m = 0;
      for (size_t k = std::max(i, j); k < N; ++k) {
        m += masses[k];
      }
      M[i*N + j] = m * lengths[i] * lengths[j] * std::cos(angles[i] - angles[j]);
    }
  }
  return M;
}

std::vector<double> generate_h(
  std::vector<double> masses,
  std::vector<double> lengths,
  std::vector<double> angles,
  std::vector<double> angular_velocities
) {
  // \vec{h}_i = \Bigg(
  //                  \sum_{k = 1}^{n} \Big( \sum_{l = \max(i, k)}^{n} m_l \Big) l_i l_k \dot{\theta}_k^2 \sin(\theta_k - \theta_i)
  //             \Bigg)
  //             - g \Big( \sum_{k = i}^{n}{m_k} \Big) l_i \sin(\theta_i)
  size_t N = masses.size();
  std::vector<double> h(N);
  for (size_t i = 0; i < N; ++i) {
    for (size_t k = 0; k < N; ++k) {
      double m = 0;
      for (size_t l = std::max(i, k); l < N; ++l) {
        m += masses[l];
      }
      double omega_sq = angular_velocities[k]*angular_velocities[k];
      h[i] += m * lengths[i] * lengths[k] * omega_sq * std::sin(angles[k] - angles[i]);
    }
    for (size_t k = i; k < N; ++k) {
      h[i] -= g * masses[k] * lengths[i] * std::sin(angles[i]);
    }
  }

  return h;
}

std::vector<double> gausian_elimination(
  std::vector<double>& M,
  std::vector<double>& h
) {
  // Gausian elimination
  size_t N = h.size();

  for (size_t column = 0; column < N-1; column++) {
    for (size_t row = column+1; row < N; row++) {
      double r = M[row*N + column] / M[column*N + column];
      h[row] = h[row] - r * h[column];
      for (size_t j = column; j < N; ++j) {
        M[row*N + j] = M[row*N + j] - r * M[column*N + j];
      }
    }
  }

  std::vector<double> results(N);
  for (int variable = N-1; variable >= 0; variable--) {
    double result = h[variable];
    for (size_t i = N-1; i > variable; i--) {
      result -= results[i] * M[variable*N + i];
    }
    results[variable] = result/M[variable*N + variable];
  }

  return results;
}

std::vector<double> advance_angle(
  std::vector<double> angles,
  std::vector<double> angular_velocities,
  double dt
) {
  std::vector<double> new_angles;
  new_angles.reserve(angles.size());
  for (size_t i = 0; i < angles.size(); ++i) {
    new_angles.push_back(angles[i] + angular_velocities[i]*dt);
  }
  return new_angles;
}
std::vector<double> advance_omega(
  std::vector<double> angular_velocities,
  std::vector<double> d_omega,
  double dt
) {
  std::vector<double> new_omega;
  new_omega.reserve(angular_velocities.size());
  for (size_t i = 0; i < angular_velocities.size(); ++i) {
    new_omega.push_back(angular_velocities[i] + d_omega[i]*dt);
  }
  return new_omega;
}

void Pendulum::step(double dt) {
  double dt_2 = dt / 2.;
  double dt_6 = dt / 6.;
  // RK4
  auto M1 = generate_M(
    masses,
    lengths,
    angles
  );
  auto h1 = generate_h(
    masses,
    lengths,
    angles,
    angular_velocities
  );

  std::vector<double> r1 = gausian_elimination(M1, h1);

  auto angles_2 = advance_angle(angles, angular_velocities, dt_2);
  auto omega_2 = advance_omega(
    angular_velocities,
    std::vector<double>(std::begin(r1), std::end(r1)),
    dt_2
  );
  auto M2 = generate_M(
    masses,
    lengths,
    angles_2
  );
  auto h2 = generate_h(
    masses,
    lengths,
    angles_2,
    omega_2
  );

  std::vector<double> r2 = gausian_elimination(M2, h2);

  auto angles_3 = advance_angle(angles, omega_2, dt_2);
  auto omega_3 = advance_omega(
    angular_velocities,
    std::vector<double>(std::begin(r2), std::end(r2)),
    dt_2
  );
  auto M3 = generate_M(
    masses,
    lengths,
    angles_3
  );
  auto h3 = generate_h(
    masses,
    lengths,
    angles_3,
    omega_3
  );

  std::vector<double> r3 = gausian_elimination(M3, h3);

  auto angles_4 = advance_angle(angles, omega_3, dt);
  auto omega_4 = advance_omega(
    angular_velocities,
    std::vector<double>(std::begin(r3), std::end(r3)),
    dt
  );
  auto M4 = generate_M(
    masses,
    lengths,
    angles_4
  );
  auto h4 = generate_h(
    masses,
    lengths,
    angles_4,
    omega_4
  );

  std::vector<double> r4 = gausian_elimination(M4, h4);

  for (size_t i = 0; i < n_segments(); ++i) {
    angles[i] += dt_6 * (angular_velocities[i] + 2*omega_2[i] + 2*omega_3[i] + omega_4[i]);
    angular_velocities[i] += dt_6 * (r1[i] + 2*r2[i] + 2*r3[i] + r4[i]);
  }
}

double Pendulum::kinetic_energy() const {
  double energy = 0;
  double v_squared_sum = 0;
  for (size_t i = 0; i < n_segments(); ++i) {
    double l_squared = lengths[i]*lengths[i];
    double o_squared = angular_velocities[i]*angular_velocities[i];
    double v_squared = l_squared * o_squared;
    v_squared_sum += v_squared;
    for (size_t j = 0; j < i; ++j) {
      v_squared_sum +=
        2 *
        lengths[i]*lengths[j] *
        angular_velocities[i]*angular_velocities[j] *
        std::cos(angles[i] - angles[j]);
    }
    energy += 0.5 * masses[i] * v_squared_sum;
  }
  return energy;
}

double Pendulum::potential_energy() const {
  double mass_sum = 0;
  for (size_t i = 0; i < n_segments(); ++i) {
    mass_sum += masses[i];
  }
  double energy = 0;
  for (size_t i = 0; i < n_segments(); ++i) {
    double y = lengths[i] - lengths[i] * std::cos(angles[i]);
    energy += g * mass_sum * y;
    mass_sum -= masses[i];
  }
  return energy;
}
