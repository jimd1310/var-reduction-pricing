#pragma once
#include "options/Option.hpp"

enum class OptionType { Call, Put }; 

class EuropeanOption : public Option
{
public: 
    EuropeanOption(
        double strike, 
        double maturity, 
        OptionType type
    ); 

    double payoff(double ST) const override; 
    double maturity() const override { return maturity_; }

    double strike() const { return strike_; }
    OptionType type() const { return type_; }

private: 
    double strike_; 
    double maturity_; 
    OptionType type_; 
};