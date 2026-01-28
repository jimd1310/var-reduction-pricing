#pragma once 
#include "models/Model.hpp"
#include "core/RandomEngine.hpp"

class BlackScholesModel : public Model
{ 
public: 
    BlackScholesModel(
        double spot, 
        double rate, 
        double volatility
    ); 

    double simulate(double T, double Z) const override; 

    double spot() const { return spot_; }
    double rate() const { return rate_; }
    double volatility() const { return vol_; }

private: 
    double spot_; 
    double rate_; 
    double vol_;
};