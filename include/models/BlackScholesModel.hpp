#pragma once 
#include "models/Model.hpp"
#include "core/RandomEngine.hpp"

/// Black-Scholes asset model (geometric Brownian motion).
class BlackScholesModel : public Model
{ 
public: 
    BlackScholesModel(
        double spot, 
        double rate, 
        double volatility
    ); 

    /// Simulate asset price at time t with standard Normal r.v. Z.
    double simulate(double t, double Z) const override; 

    double spot() const { return spot_; }
    double rate() const { return rate_; }
    double volatility() const { return vol_; }

private: 
    double spot_; 
    double rate_; 
    double vol_;
};