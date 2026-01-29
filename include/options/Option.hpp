#pragma once 

/// Abstract interface for an option.
class Option
{ 
public: 
    virtual ~Option() = default; 
    virtual double payoff(double ST) const = 0; 
    virtual double maturity() const = 0; 
};