#include "analytics/BlackScholesClosedForm.hpp"
#include <cmath>

static double norm_cdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2)); 
}

double black_scholes_price(
    double S, 
    double K, 
    double r, 
    double v, 
    double T, 
    OptionType type
)
{
    double d1 = (std::log(S / K) + (r + 0.5 * v * v) * T) 
                / (v * std::sqrt(T));
    double d2 = d1 - v * std::sqrt(T);

    if (type == OptionType::Call) 
        return S * norm_cdf(d1) - K * std::exp(-r * T) * norm_cdf(d2); 
    else 
        return K * std::exp(-r * T) * norm_cdf(-d2) - S * norm_cdf(-d1); 
}