#pragma once 
#include "market/Discount.hpp"
#include <cmath>

/// Implements a continuously compounded discount factor.
class FlatDiscount : public Discount
{
public: 
    explicit FlatDiscount(double r) : r_(r) {}

    double operator()(double T) const override 
    {
        return std::exp(-r_ * T);
    }

private: 
    double r_;
};  