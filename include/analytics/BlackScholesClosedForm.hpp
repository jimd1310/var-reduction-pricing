#pragma once
#include "options/EuropeanOption.hpp"

double black_scholes_price(
    double spot, 
    double strike, 
    double rate, 
    double volatility, 
    double maturity, 
    OptionType type
);