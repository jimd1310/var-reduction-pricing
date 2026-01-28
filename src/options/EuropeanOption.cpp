#include "options/EuropeanOption.hpp"
#include <algorithm>

EuropeanOption::EuropeanOption(
    double strike, 
    double maturity, 
    OptionType type
)
: strike_(strike), maturity_(maturity), type_(type) {}

double EuropeanOption::payoff(double ST) const 
{
    if (type_ == OptionType::Call)
        return std::max(ST - strike_, 0.0);
    else 
        return std::max(strike_ - ST, 0.0); 
}