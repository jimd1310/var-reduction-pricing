#pragma once 
#include "samplers/PathSampler.hpp"
#include "core/RandomEngine.hpp"
#include <cstddef> 

/// Estimates the control variate coefficient (beta) within a given error target, 
/// using the sample covariance and variance. 
double calibrate_beta(
    PathSampler& target, 
    PathSampler& control, 
    RandomEngine& rng,
    std::size_t max_samples, 
    std::size_t min_samples = 1000,
    double error_target = 0.05
);