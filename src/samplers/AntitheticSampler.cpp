#include "samplers/AntitheticSampler.hpp"
#include <cmath> 

AntitheticSampler::AntitheticSampler(
    const Model& model, 
    const Option& option
)
: model_(model), option_(option) {}

double AntitheticSampler::sample(double Z) const
{
    double T = option_.maturity();

    double ST1 = model_.simulate(T, Z); 
    double ST2 = model_.simulate(T, -Z);

    return 0.5 * (option_.payoff(ST1) + option_.payoff(ST2));
}