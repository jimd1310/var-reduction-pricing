#pragma once
#include "options/Option.hpp"

enum class OptionType { Call, Put }; 

/// European option (vanilla, payoff depends on terminal price only).
class EuropeanOption : public Option
{
public: 
    EuropeanOption(
        double strike, 
        double maturity, 
        OptionType type
    ); 
    
    /// Payoff at maturity given underlying price ST.
    double payoff(double ST) const override; 
    double maturity() const override { return maturity_; }

    double strike() const { return strike_; }
    OptionType type() const { return type_; }

private: 
    double strike_; 
    double maturity_; 
    OptionType type_; 
};