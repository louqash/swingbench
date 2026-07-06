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
const double delta_LENGTH = 0.1;     // meters
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

std::array<double, 9> generate_M(
  std::vector<double> masses,
  std::vector<double> lengths,
  std::vector<double> angles
) {
    double m_1 = masses[0];
    double m_2 = masses[1];
    double m_3 = masses[2];
    double l_1 = lengths[0];
    double l_2 = lengths[1];
    double l_3 = lengths[2];
    double cos_t1_t2 = std::cos(angles[0] - angles[1]);
    double cos_t1_t3 = std::cos(angles[0] - angles[2]);
    double cos_t2_t3 = std::cos(angles[1] - angles[2]);
    return std::array {
      (m_1 + m_2 + m_3) * l_1*l_1,
      (m_2 + m_3) * l_1 * l_2 * cos_t1_t2,
      m_3 * l_1 * l_3 * cos_t1_t3,

      (m_2 + m_3) * l_1 * l_2 * cos_t1_t2,
      (m_2 + m_3) * l_2*l_2,
      m_3 * l_2 * l_3 * cos_t2_t3,

      m_3 * l_1 * l_3 * cos_t1_t3,
      m_3 * l_2 * l_3 * cos_t2_t3,
      m_3 * l_3*l_3
    };
}

std::array<double, 3> generate_v(
  std::vector<double> masses,
  std::vector<double> lengths,
  std::vector<double> angles,
  std::vector<double> angular_velocities
) {
    double m_1 = masses[0];
    double m_2 = masses[1];
    double m_3 = masses[2];
    double l_1 = lengths[0];
    double l_2 = lengths[1];
    double l_3 = lengths[2];
    double omega_1 = angular_velocities[0];
    double omega_2 = angular_velocities[1];
    double omega_3 = angular_velocities[2];
    double theta_1 = angles[0];
    double theta_2 = angles[1];
    double theta_3 = angles[2];
    double sin_t1_t2 = std::sin(angles[0] - angles[1]);
    double sin_t1_t3 = std::sin(angles[0] - angles[2]);
    double sin_t2_t3 = std::sin(angles[1] - angles[2]);

    return std::array {
      - (m_2 + m_3) * l_1 * l_2 * omega_2*omega_2 * sin_t1_t2 - m_3 * l_1 * l_3 * omega_3*omega_3 * sin_t1_t3 - g * l_1 * (m_1 + m_2 + m_3) * std::sin(theta_1),
      (m_2 + m_3) * l_1 * l_2 * omega_1*omega_1 * sin_t1_t2 - m_3 * l_2 * l_3 * omega_3*omega_3 * sin_t2_t3 - g * l_2 * (m_2 + m_3) * std::sin(theta_2),
      m_3 * l_1 * l_3 * omega_1*omega_1 * sin_t1_t3 + m_3 * l_2 * l_3 * omega_2*omega_2 * sin_t2_t3 - g * l_3 * m_3 * std::sin(theta_3),
    };
}

void Pendulum::step(double dt) {
  if(n_segments() == 2){
    step_2(dt);
  }
  if(n_segments() == 3){
    step_3(dt);
  }
}

std::array<double, 3> gausian_elimination(
  std::array<double, 9>& M,
  std::array<double, 3>& v
) {
  // Gausian elimination
  size_t N = 3;

  // First column
  for (size_t i = 1; i < N; ++i) {
    double r = M[i*N + 0] / M[0*N + 0];
    v[i] = v[i] - r * v[0];
    for (size_t j = 0; j < N; ++j) {
      M[i*N + j] = M[i*N + j] - r * M[0*N + j];
    }
  }
  // Second column
  for (size_t i = 2; i < N; ++i) {
    double r = M[i*N + 1] / M[1*N + 1];
    v[i] = v[i] - r * v[1];
    for (size_t j = 1; j < N; ++j) {
      M[i*N + j] = M[i*N + j] - r * M[1*N + j];
    }
  }
  double d_omega_3 = v[2] / M[2*N + 2];
  double d_omega_2 = (v[1] - d_omega_3 * M[1*N + 2]) / M[1*N + 1];
  double d_omega_1 = (v[0] - d_omega_3 * M[0*N + 2] - d_omega_2 * M[0*N + 1]) / M[0*N + 0];

  return std::array {d_omega_1, d_omega_2, d_omega_3};
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

void Pendulum::step_3(double dt) {
  double dt_2 = dt / 2.;
  double dt_6 = dt / 6.;
  // RK4
  auto M1 = generate_M(
    masses,
    lengths,
    angles
  );
  auto v1 = generate_v(
    masses,
    lengths,
    angles,
    angular_velocities
  );

  std::array<double, 3> r1 = gausian_elimination(M1, v1);

  auto omega_2 = advance_omega(
    angular_velocities,
    std::vector<double>(std::begin(r1), std::end(r1)),
    dt_2
  );
  auto angles_2 = advance_angle(angles, omega_2, dt_2);
  auto M2 = generate_M(
    masses,
    lengths,
    angles_2
  );
  auto v2 = generate_v(
    masses,
    lengths,
    angles_2,
    omega_2
  );

  std::array<double, 3> r2 = gausian_elimination(M2, v2);

  auto omega_3 = advance_omega(
    angular_velocities,
    std::vector<double>(std::begin(r2), std::end(r2)),
    dt_2
  );
  auto angles_3 = advance_angle(angles, omega_3, dt_2);
  auto M3 = generate_M(
    masses,
    lengths,
    angles_2
  );
  auto v3 = generate_v(
    masses,
    lengths,
    angles_2,
    omega_3
  );

  std::array<double, 3> r3 = gausian_elimination(M3, v3);

  auto omega_4 = advance_omega(
    angular_velocities,
    std::vector<double>(std::begin(r3), std::end(r3)),
    dt
  );
  auto angles_4 = advance_angle(angles, omega_4, dt);
  auto M4 = generate_M(
    masses,
    lengths,
    angles_4
  );
  auto v4 = generate_v(
    masses,
    lengths,
    angles_4,
    omega_4
  );

  std::array<double, 3> r4 = gausian_elimination(M4, v4);

  angles[0] += dt_6 * (angular_velocities[0] + 2*omega_2[0] + 2*omega_3[0] + omega_4[0]);
  angles[1] += dt_6 * (angular_velocities[1] + 2*omega_2[1] + 2*omega_3[1] + omega_4[1]);
  angles[2] += dt_6 * (angular_velocities[2] + 2*omega_2[2] + 2*omega_3[2] + omega_4[2]);
  angular_velocities[0] += dt_6 * (r1[0] + 2*r2[0] + 2*r3[0] + r4[0]);
  angular_velocities[1] += dt_6 * (r1[1] + 2*r2[1] + 2*r3[1] + r4[1]);
  angular_velocities[2] += dt_6 * (r1[2] + 2*r2[2] + 2*r3[2] + r4[2]);
}

void Pendulum::step_2(double dt) {
  double m_1 = masses[0];
  double m_2 = masses[1];
  double l_1 = lengths[0];
  double l_2 = lengths[1];
  double omega_1 = angular_velocities[0];
  double omega_2 = angular_velocities[1];
  double theta_1 = angles[0];
  double theta_2 = angles[1];

  // RK4
  double dt_2 = dt / 2.;
  DoublePendulumDerivatives k1 = lagrange_euler_for_2_arms(
    m_1,
    m_2,
    l_1,
    l_2,
    omega_1,
    omega_2,
    theta_1,
    theta_2
  );
  DoublePendulumDerivatives k2 = lagrange_euler_for_2_arms(
    m_1,
    m_2,
    l_1,
    l_2,
    omega_1 + k1.d_omega_1*dt_2,
    omega_2 + k1.d_omega_2*dt_2,
    theta_1 + k1.d_theta_1*dt_2,
    theta_2 + k1.d_theta_2*dt_2
  );
  DoublePendulumDerivatives k3 = lagrange_euler_for_2_arms(
    m_1,
    m_2,
    l_1,
    l_2,
    omega_1 + k2.d_omega_1*dt_2,
    omega_2 + k2.d_omega_2*dt_2,
    theta_1 + k2.d_theta_1*dt_2,
    theta_2 + k2.d_theta_2*dt_2
  );
  DoublePendulumDerivatives k4 = lagrange_euler_for_2_arms(
    m_1,
    m_2,
    l_1,
    l_2,
    omega_1 + k3.d_omega_1*dt,
    omega_2 + k3.d_omega_2*dt,
    theta_1 + k3.d_theta_1*dt,
    theta_2 + k3.d_theta_2*dt
  );

  double dt_6 = dt / 6.;
  DoublePendulumDerivatives average = {
    dt_6 * (k1.d_theta_1 + 2*k2.d_theta_1 + 2*k3.d_theta_1 + k4.d_theta_1),
    dt_6 * (k1.d_theta_2 + 2*k2.d_theta_2 + 2*k3.d_theta_2 + k4.d_theta_2),
    dt_6 * (k1.d_omega_1 + 2*k2.d_omega_1 + 2*k3.d_omega_1 + k4.d_omega_1),
    dt_6 * (k1.d_omega_2 + 2*k2.d_omega_2 + 2*k3.d_omega_2 + k4.d_omega_2),
  };

  angles[0] += average.d_theta_1;
  angles[1] += average.d_theta_2;
  angular_velocities[0] += average.d_omega_1;
  angular_velocities[1] += average.d_omega_2;
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
