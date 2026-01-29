#pragma once
#include "samplers/PathSampler.hpp"
#include "models/Model.hpp"
#include "options/Option.hpp"
#include "core/RandomEngine.hpp"

/// Standard Monte Carlo sampler.
class MCSampler : public PathSampler 
{
public: 
    MCSampler(
        const Model& model, 
        const Option& option
    );

    /// Returns the option payoff.
    double sample(double Z) const override; 

private: 
    const Model& model_; 
    const Option& option_;
};