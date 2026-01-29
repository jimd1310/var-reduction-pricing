#pragma once
#include "samplers/PathSampler.hpp"
#include "models/Model.hpp"
#include "options/Option.hpp"
#include "core/RandomEngine.hpp"

/// Sampler for antithetic variates.
class AntitheticSampler : public PathSampler
{
public: 
    AntitheticSampler(
        const Model& model, 
        const Option& option
    );

    /// Evaluate option payoff using the average of Z and its antithetic -Z.
    double sample(double Z) const override; 

private: 
    const Model& model_; 
    const Option& option_;
};