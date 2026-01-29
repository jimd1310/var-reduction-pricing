#pragma once

/// Abstract interface for a stochastic asset model. 
class Model 
{
public: 
    virtual ~Model() = default; 
    virtual double simulate(double t, double Z) const = 0; 
};