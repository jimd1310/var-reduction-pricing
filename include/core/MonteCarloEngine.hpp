#pragma once
#include "samplers/PathSampler.hpp"
#include "core/OnlineStatistics.hpp"
#include "core/RandomEngine.hpp"
#include <cstddef>

/// An interface to run MC simulation with a given sampler.
class MonteCarloEngine
{
public:
    explicit MonteCarloEngine(const PathSampler& sampler); 

    OnlineStatistics run(std::size_t n_paths, RandomEngine& rng) const;

private: 
    const PathSampler& sampler_; 
};