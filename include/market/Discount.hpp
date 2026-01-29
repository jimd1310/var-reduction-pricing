#pragma once

/// Abstract interface for discount curves.
/// Evaluates the discount factor at a given time to maturity T (in years).
class Discount
{
public: 
    virtual ~Discount()= default; 
    virtual double operator()(double T) const = 0;
};