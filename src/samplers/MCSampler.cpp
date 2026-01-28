#include "samplers/MCSampler.hpp"
#include "core/RandomEngine.hpp"
#include "models/Model.hpp"
#include "options/Option.hpp"

MCSampler::MCSampler(
    const Model& model, 
    const Option& option
)
: model_(model), option_(option) {} 

double MCSampler::sample(double Z) const 
{
    double T = option_.maturity();

    double ST = model_.simulate(T, Z); 
    return option_.payoff(ST);
}