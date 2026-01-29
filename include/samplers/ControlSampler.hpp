#pragma once
#include "samplers/PathSampler.hpp"
#include <memory>

/// Control variate Monte Carlo sampler. 
class ControlSampler : public PathSampler 
{ 
public: 
    ControlSampler(
        std::unique_ptr<PathSampler> target, 
        std::unique_ptr<PathSampler> control, 
        double control_mean, 
        double beta
    ); 

    /// Evaluate option payoff with control variate adjustment.
    double sample(double Z) const override; 

private: 
    std::unique_ptr<PathSampler> target_; 
    std::unique_ptr<PathSampler> control_; 
    double control_mean_;
    double beta_; 
};