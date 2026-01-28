#pragma once
#include "samplers/PathSampler.hpp"
#include <memory>

class ControlSampler : public PathSampler 
{ 
public: 
    ControlSampler(
        std::unique_ptr<PathSampler> target, 
        std::unique_ptr<PathSampler> control, 
        double control_mean, 
        double beta
    ); 

    double sample(double Z) const override; 

private: 
    std::unique_ptr<PathSampler> target_; 
    std::unique_ptr<PathSampler> control_; 
    double control_mean_;
    double beta_; 
};