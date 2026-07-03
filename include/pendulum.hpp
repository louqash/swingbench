#pragma once

#include <vector>

struct Pendulum {
    std::vector<double> masses;
    std::vector<double> lengths;
    std::vector<double> angles;

    Pendulum() = default;
    Pendulum(int n);
    size_t n_segments() const;
};
