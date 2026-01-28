#include "core/MonteCarloEngine.hpp"

MonteCarloEngine::MonteCarloEngine(const PathSampler& sampler)
: sampler_(sampler) {}

OnlineStatistics MonteCarloEngine::run(
    std::size_t n_paths,
    RandomEngine& rng
) const
{
    OnlineStatistics stats; 

    for (std::size_t i = 0; i < n_paths; ++i) 
    {  
        double Z = rng.normal();
        double estimate = sampler_.sample(Z);
        stats.add(estimate);
    }
    return stats;
}