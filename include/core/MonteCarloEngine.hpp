#pragma once
#include "samplers/PathSampler.hpp"
#include "core/OnlineStatistics.hpp"
#include "core/RandomEngine.hpp"
#include <cstddef>

class MonteCarloEngine
{
public:
    explicit MonteCarloEngine(const PathSampler& sampler); 

    OnlineStatistics run(std::size_t n_paths, RandomEngine& rng) const;

private: 
    const PathSampler& sampler_; 
};