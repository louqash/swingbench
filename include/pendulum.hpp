#pragma once

#include <vector>

struct Pendulum {
    std::vector<double> masses;
    std::vector<double> lengths;
    std::vector<double> angles;
    std::vector<double> angular_velocities;

    Pendulum() = default;
    Pendulum(int n);
    size_t n_segments() const;
    void step(double dt);

    double kinetic_energy() const;
    double potential_energy() const;
};
