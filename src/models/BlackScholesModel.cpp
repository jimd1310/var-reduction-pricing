#include "models/BlackScholesModel.hpp"
#include <cmath> 

BlackScholesModel::BlackScholesModel(
    double spot, 
    double rate, 
    double volatility
)
: spot_(spot), rate_(rate), vol_(volatility) {}

double BlackScholesModel::simulate(double T, double Z) const 
{
    return spot_ * std::exp(
        (rate_ - 0.5 * vol_ * vol_) * T + vol_ * std::sqrt(T) * Z
    );
};