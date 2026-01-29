#pragma once 

/// Abstract interface for a Monte Carlo path sampler.
class PathSampler
{
public: 
    virtual ~PathSampler() = default; 

    /// Returns a single path evaluation given standard normal Z.
    virtual double sample(double Z) const = 0;
};