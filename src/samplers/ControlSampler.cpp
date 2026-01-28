#include "samplers/ControlSampler.hpp"

ControlSampler::ControlSampler(
    std::unique_ptr<PathSampler> target, 
    std::unique_ptr<PathSampler> control, 
    double control_mean,
    double beta
)
: target_(std::move(target)), control_(std::move(control)), 
control_mean_(control_mean), beta_(beta) {}

double ControlSampler::sample(double Z) const
{ 
    double X = target_->sample(Z); 
    double Y = control_->sample(Z); 
    return X - beta_ * (Y - control_mean_); 
}