#pragma once
#include "options/EuropeanOption.hpp"

/// Gives the Black-Scholes analytic solution for European options.
double black_scholes_price(
    double spot, 
    double strike, 
    double rate, 
    double volatility, 
    double maturity, 
    OptionType type
);