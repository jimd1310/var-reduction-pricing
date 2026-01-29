#pragma once
#include "options/Option.hpp"

/// Degenerate option returning the underlying asset price at maturity.
class NoOption : public Option
{
public:
    explicit NoOption(double T) : T_(T) {}; 
    double payoff(double ST) const override { return ST; } 
    double maturity() const override { return T_; }

private: 
    double T_;
};